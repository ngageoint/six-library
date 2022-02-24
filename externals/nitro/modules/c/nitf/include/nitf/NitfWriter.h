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

#ifndef __NITF_WRITER_H__
#define __NITF_WRITER_H__

#include "nitf/PluginRegistry.h"
#include "nitf/Record.h"
#include "nitf/ImageIO.h"
#include "nitf/ImageWriter.h"
#include "nitf/SegmentWriter.h"
#include "nitf/ComplexityLevel.h"

NITF_CXX_GUARD

// These represent the fill direction when writing NITF fields (i.e. do you
// fill with, say, spaces on the left or right of the field)
#define NITF_WRITER_FILL_LEFT 1
#define NITF_WRITER_FILL_RIGHT 2

/*!
 *  \struct nitf_Writer
 *  \brief  This object represents the 2.1 (2.0?) file writer
 */
#if _MSC_VER
#pragma warning(push)
#pragma warning(disable: 4820) // '...': '...' bytes padding added after data member '...'
#endif
typedef struct _nitf_Writer
{
    nitf_List *warningList;
    nitf_WriteHandler **imageWriters;
    nitf_WriteHandler **textWriters;
    nitf_WriteHandler **graphicWriters;
    nitf_WriteHandler **dataExtensionWriters;
    nitf_IOInterface* output;
    nitf_Record *record;
    int numImageWriters;
    int numTextWriters;
    int numGraphicWriters;
    int numDataExtensionWriters;
    NITF_BOOL ownOutput;
}
nitf_Writer;
#if _MSC_VER
#pragma warning(pop)
#endif

/* ------------------------------------------------------------------ */
/*                PUBLIC PROTOTYPES                                   */
/* ------------------------------------------------------------------ */

/*!
 *  Construct a new writer
 *  \param error A populated structure upon failure
 *  \return A Writer, or NULL on failure
 */
NITFAPI(nitf_Writer *) nitf_Writer_construct(nitf_Error * error);

/*!
 *  Destroy a writer, set *writer to NULL
 *  \param writer The writer to NULL-set
 */
NITFAPI(void) nitf_Writer_destruct(nitf_Writer ** writer);

NITFAPI(NITF_BOOL) nitf_Writer_prepare(nitf_Writer * writer,
                                       nitf_Record * record,
                                       nitf_IOHandle ioHandle,
                                       nitf_Error * error);



NITFAPI(NITF_BOOL) nitf_Writer_prepareIO(nitf_Writer * writer,
                                       nitf_Record * record,
                                       nitf_IOInterface* io,
                                       nitf_Error * error);


/*!
 * Sets the WriteHandler for the Image at the given index.
 */
NITFAPI(NITF_BOOL) nitf_Writer_setImageWriteHandler(nitf_Writer *writer,
        int index, nitf_WriteHandler *writeHandler, nitf_Error * error);

/*!
 * Sets the WriteHandler for the Graphic at the given index.
 */
NITFAPI(NITF_BOOL) nitf_Writer_setGraphicWriteHandler(nitf_Writer *writer,
        int index, nitf_WriteHandler *writeHandler, nitf_Error * error);

/*!
 * Sets the WriteHandler for the Text at the given index.
 */
NITFAPI(NITF_BOOL) nitf_Writer_setTextWriteHandler(nitf_Writer *writer,
        int index, nitf_WriteHandler *writeHandler, nitf_Error * error);

/*!
 * Sets the WriteHandler for the DE Segment at the given index.
 */
NITFAPI(NITF_BOOL) nitf_Writer_setDEWriteHandler(nitf_Writer *writer,
        int index, nitf_WriteHandler *writeHandler, nitf_Error * error);


/*!
 * Creates and returns a new ImageWriter for the image at the given index.
 * This is deprecated, but is available for backwards compatibility.
 *
 * \param writer    The Writer object
 * \param options
 * \param index     The segment index
 *
 * \deprecated - setImageWriteHandler is the preferred method to use.
 * \return A new ImageWriter, or NULL on failure
 */
NITFAPI(nitf_ImageWriter *) nitf_Writer_newImageWriter(nitf_Writer* writer,
                                                       int index,
                                                       nrt_HashTable* options,
                                                       nitf_Error* error);

/*!
 * Creates and returns a new SegmentWriter for the graphic at the given index.
 * This is deprecated, but is available for backwards compatibility.
 *
 * \param writer    The Writer object
 * \param index     The segment index
 *
 * \deprecated - setGraphicWriteHandler is the preferred method to use.
 * \return A new SegmentWriter, or NULL on failure
 */
NITFAPI(nitf_SegmentWriter*) nitf_Writer_newGraphicWriter(nitf_Writer *writer,
        int index, nitf_Error * error);

/*!
 * Creates and returns a new SegmentWriter for the text at the given index.
 * This is deprecated, but is available for backwards compatibility.
 *
 * \param writer    The Writer object
 * \param index     The segment index
 *
 * \deprecated - setTextWriteHandler is the preferred method to use.
 * \return A new SegmentWriter, or NULL on failure
 */
NITFAPI(nitf_SegmentWriter*) nitf_Writer_newTextWriter(nitf_Writer *writer,
        int index, nitf_Error * error);

/*!
 * Creates and returns a new SegmentWriter for the DESegment at the given index.
 * This is deprecated, but is available for backwards compatibility.
 *
 * \param writer    The Writer object
 * \param index     The segment index
 *
 * \deprecated - setDEWriteHandler is the preferred method to use.
 * \return A new SegmentWriter, or NULL on failure
 */
NITFAPI(nitf_SegmentWriter*) nitf_Writer_newDEWriter(nitf_Writer *writer,
        int index, nitf_Error * error);


/*!
 * Performs the write operation
 *
 * \return NITF_SUCCESS or NITF_FAILURE
 */
NITFAPI(NITF_BOOL) nitf_Writer_write(nitf_Writer * writer, nitf_Error * error);

// NOTE: In general the following functions are not needed.  Only use these
//       if you know what you're doing and are trying to write out a NITF
//       piecemeal rather than through the normal Writer object interface.

/*!
 * Writes out the file header (skips over the FL field as the total file length
 * is not known yet).  No seeking is performed so the underlying IO object
 * should be at the beginning of the file.
 *
 * \param writer Initialized writer object
 * \param fileLenOff Output parameter providing the offset in bytes in the file
 *     to the file length (FL) field in the header so that you can write it out
 *     later once the file length is known
 * \param hdrLen Output parameter providing the total number of bytes the file
 *     header is on disk
 * \param error Output parameter providing the error if one occurs
 *
 * \return NITF_SUCCESS on success, NITF_FAILURE otherwise
 *
 */
NITFPROT(NITF_BOOL) nitf_Writer_writeHeader(nitf_Writer* writer,
                                            nitf_Off* fileLenOff,
                                            uint32_t* hdrLen,
                                            nitf_Error* error);

/*!
 * Writes out an image subheader.  No seeking is performed so the underlying
 * IO object should be at the appropriate spot in the file prior to this call.
 *
 * \param writer Initialized writer object
 * \param subhdr Image subheader to write out
 * \param fver NITF file version to write (you probably want NITF_VER_21)
 * \param comratOff Output parameter containing the offset in bytes in the
 *     file where the COMRAT field would be populated for compressed files
 * \param error Output parameter providing the error if one occurs
 *
 * \return NITF_SUCCESS on success, NITF_FAILURE otherwise
 */
NITFPROT(NITF_BOOL)
nitf_Writer_writeImageSubheader(nitf_Writer* writer,
                                const nitf_ImageSubheader* subhdr,
                                nitf_Version fver,
                                nitf_Off* comratOff,
                                nitf_Error* error);

/*!
 * Writes out a data extension subheader.  No seeking is performed so the
 * underlying IO object should be at the appropriate spot in the file prior to
 * this call.
 *
 * \param writer Initialized writer object
 * \param subhdr Data extension subheader to write out
 * \param userSublen Output parameter containing the length in bytes of the
 *     user subheader
 * \param fver NITF file version to write (you probably want NITF_VER_21)
 * \param error Output parameter providing the error if one occurs
 *
 * \return NITF_SUCCESS on success, NITF_FAILURE otherwise
 */
NITFPROT(NITF_BOOL) nitf_Writer_writeDESubheader(nitf_Writer* writer,
                                                 const nitf_DESubheader* subhdr,
                                                 uint32_t* userSublen,
                                                 nitf_Version fver,
                                                 nitf_Error* error);

/*!
 * Writes an int64 field to the writer in its current position
 *
 * \param writer Initialized writer object
 * \param field Value of the field to write out
 * \param length Length of the field to write out
 * \param fill Fill character to use for any remaining bytes
 * \param fillDir Fill direction (NITF_WRITER_FILL_LEFT or
 *     NITF_WRITER_FILL_RIGHT)
 * \param error Output parameter providing the error if one occurs
 *
 * \return NITF_SUCCESS on success, NITF_FAILURE otherwise
 */
NITFPROT(NITF_BOOL) nitf_Writer_writeInt64Field(nitf_Writer* writer,
                                                uint64_t field,
                                                uint32_t length,
                                                char fill,
                                                uint32_t fillDir,
                                                nitf_Error* error);

NITF_CXX_ENDGUARD

#endif
