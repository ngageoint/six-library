/* =========================================================================
 * This file is part of NITRO
 * =========================================================================
 * 
 * (C) Copyright 2004 - 2008, General Dynamics - Advanced Information Systems
 *
 * NITRO is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public 
 * License along with this program; if not, If not, 
 * see <http://www.gnu.org/licenses/>.
 *
 */

#include "nitf/ImageWriter.h"

NITFAPI(NITF_BOOL) nitf_ImageWriter_attachSource(nitf_ImageWriter * writer,
        nitf_ImageSource *
        imageSource,
        nitf_Error * error)
{
    if (writer->imageSource != NULL)
    {
        nitf_Error_init(error,
                        "Image source already attached",
                        NITF_CTXT, NITF_ERR_INVALID_PARAMETER);
        return NITF_FAILURE;
    }
    writer->imageSource = imageSource;
    return NITF_SUCCESS;
}


NITFPROT(NITF_BOOL) nitf_ImageWriter_write(nitf_ImageWriter * writer,
        int numBitsPerPixel,
        int numImageBands,
        int numMultispectralImageBands,
        int numRows,
        int numCols, nitf_Error * error)
{
    nitf_Uint8 **user;
    int row;
    size_t rowSize;
    int band;
    off_t offset;
    nitf_BandSource *bandSrc;

    numImageBands += numMultispectralImageBands;
    rowSize = numCols * NITF_NBPP_TO_BYTES(numBitsPerPixel);

    user = (nitf_Uint8 **) NITF_MALLOC(8 * numImageBands);
    assert(user);
    for (band = 0; band < numImageBands; band++)
    {
        user[band] = (nitf_Uint8 *) NITF_MALLOC(rowSize);
        assert(user[band]);
    }

    offset = nitf_IOHandle_tell(writer->outputHandle, error);
    if (!NITF_IO_SUCCESS(offset))
        goto CATCH_ERROR;

    if (!nitf_ImageIO_setFileOffset(writer->imageBlocker, offset, error))
    {
        goto CATCH_ERROR;
    }

    if (!nitf_ImageIO_writeSequential(writer->imageBlocker,
                                      writer->outputHandle, error))
    {
        goto CATCH_ERROR;
    }

    for (row = 0; row < numRows; ++row)
    {
        for (band = 0; band < numImageBands; ++band)
        {
            bandSrc = nitf_ImageSource_getBand(writer->imageSource,
                                               band, error);
            if (bandSrc == NULL)
                return NITF_FAILURE;

            if (!(*(bandSrc->iface->read)) (bandSrc->data, (char *) user[band],
                                            (size_t) rowSize, error))
            {
                goto CATCH_ERROR;
            }
        }

        if (!nitf_ImageIO_writeRows(writer->imageBlocker,
                                    writer->outputHandle, 1, user, error))
        {
            goto CATCH_ERROR;
        }
    }

    if (!nitf_ImageIO_writeDone(writer->imageBlocker,
                                writer->outputHandle, error))
    {
        goto CATCH_ERROR;
    }

    for (band = 0; band < numImageBands; band++)
    {
        NITF_FREE(user[band]);
    }

    NITF_FREE(user);

    return NITF_SUCCESS;

CATCH_ERROR:
    return NITF_FAILURE;
}


NITFAPI(void) nitf_ImageWriter_destruct(nitf_ImageWriter ** writer)
{
    if (*writer)
    {
        if ((*writer)->imageBlocker)
            nitf_ImageIO_destruct(&(*writer)->imageBlocker);
        if ((*writer)->imageSource)
            nitf_ImageSource_destruct(&(*writer)->imageSource);
        (*writer)->outputHandle = 0;
        NITF_FREE(*writer);
        *writer = NULL;
    }
}
NITFPROT(int) nitf_ImageWriter_setWriteCaching(
    nitf_ImageWriter * iWriter, int enable)
{
    return(nitf_ImageIO_setWriteCaching(iWriter->imageBlocker, enable));
}
