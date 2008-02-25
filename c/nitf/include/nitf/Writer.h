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

#ifndef __NITF_WRITER_H__
#define __NITF_WRITER_H__

#include "nitf/Error.h"
#include "nitf/IOHandle.h"
#include "nitf/PluginRegistry.h"
#include "nitf/Record.h"
#include "nitf/ImageIO.h"
#include "nitf/ImageWriter.h"
#include "nitf/SegmentWriter.h"

NITF_CXX_GUARD

/*!
 *  \struct nitf_Writer
 *  \brief  This object represents the 2.1 (2.0?) file writer
 */
typedef struct _nitf_Writer
{
    nitf_List *warningList;
    nitf_ImageWriter **imageWriters;
    nitf_SegmentWriter **textWriters;
    nitf_SegmentWriter **graphicWriters;
    nitf_SegmentWriter **dataExtensionWriters;
    nitf_IOHandle outputHandle;
    nitf_Record *record;
    int numImageWriters;
    int numTextWriters;
    int numGraphicWriters;
    int numDataExtensionWriters;
}
nitf_Writer;


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


NITFAPI(nitf_ImageWriter *) nitf_Writer_newImageWriter(nitf_Writer *
        writer,
        int imageNumber,
        nitf_Error * error);

/*!
 *  Creates a new Text Writer
 *  \param error A populated structure upon failure
 *  \return A Writer, or NULL on failure
 */
NITFAPI(nitf_SegmentWriter *) nitf_Writer_newTextWriter
(
    nitf_Writer * writer,
    int textNumber,
    nitf_Error * error
);

/*!
 *  Creates a new Graphics Writer
 *  \param error A populated structure upon failure
 *  \return A Writer, or NULL on failure
 */
NITFAPI(nitf_SegmentWriter *) nitf_Writer_newGraphicWriter
(
    nitf_Writer * writer,
    int graphicNumber,
    nitf_Error * error
);

/*!
 *  Creates a new DE segment Writer
 *  \param error A populated structure upon failure
 *  \return A Writer, or NULL on failure
 */
NITFAPI(nitf_SegmentWriter *) nitf_Writer_newDEWriter
(
    nitf_Writer * writer,
    int DENumber,
    nitf_Error * error
);

NITFAPI(NITF_BOOL) nitf_Writer_write(nitf_Writer * writer,
                                     nitf_Error * error);


NITF_CXX_ENDGUARD

#endif
