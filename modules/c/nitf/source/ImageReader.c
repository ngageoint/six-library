/* =========================================================================
 * This file is part of NITRO
 * =========================================================================
 *
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
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

#include "nitf/ImageReader.h"

NITFAPI(nitf_BlockingInfo *)
nitf_ImageReader_getBlockingInfo(nitf_ImageReader * imageReader,
                                 nitf_Error * error)
{
    return nitf_ImageIO_getBlockingInfo(imageReader->imageDeblocker,
                                        imageReader->input, error);
}


NITFAPI(NITF_BOOL) nitf_ImageReader_read(nitf_ImageReader * imageReader,
                                         nitf_SubWindow * subWindow,
                                         uint8_t ** user,
                                         int *padded, nitf_Error * error)
{
    return (NITF_BOOL) nitf_ImageIO_read(imageReader->imageDeblocker,
                                         imageReader->input,
                                         subWindow, user, padded, error);
}

NITFAPI(uint8_t*) nitf_ImageReader_readBlock(nitf_ImageReader * imageReader,
                                                uint32_t blockNumber,
                                                uint64_t* blockSize,
                                                nitf_Error * error)
{
    if(!imageReader->directBlockRead)
    {
        if(!nitf_ImageIO_setupDirectBlockRead(imageReader->imageDeblocker,
                                              imageReader->input,
                                              1,
                                              error))
            return NULL;

        imageReader->directBlockRead = 1;
    }

    return nitf_ImageIO_readBlockDirect(imageReader->imageDeblocker,
                                        imageReader->input,
                                        blockNumber,
                                        blockSize,
                                        error);
}

NITFAPI(void) nitf_ImageReader_destruct(nitf_ImageReader ** imageReader)
{
    if (*imageReader)
    {
        if ((void *) (*imageReader)->imageDeblocker != NULL)
        {
            /*
               MANY PROBLEMS HERE.  Solution is that destructor is
               part of callback arena, or is deleted by ImageIO.
               Problem one, getting out the IC at this point.
               Problem two, getting the decompressor back from the struct
               nitf_DecompressionInterface* decompIface =
               (void*)(*segment)->imageIO->decompressor;
               if (decompIface)
               {
               nitf_Error error;
               if (!destroyDecompIface(&decompIface, error))
               nitf_Error_print(error,
               "Unsuccessful decompressor destruction",
               stderr);
               }
             */
            nitf_ImageIO_destruct(&(*imageReader)->imageDeblocker);
        }
        /*nitf_IOHandle_close((*imageReader)->inputHandle); */
        NITF_FREE(*imageReader);
        *imageReader = NULL;
    }
}

NITFPROT(void) nitf_ImageReader_setReadCaching(nitf_ImageReader * iReader)
{
    nitf_ImageIO_setReadCaching(iReader->imageDeblocker);
    return;
}
