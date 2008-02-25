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

#ifndef __NITF_SEGMENT_WRITER_H__
#define __NITF_SEGMENT_WRITER_H__

#include "nitf/ImageIO.h"
#include "nitf/IOHandle.h"
#include "nitf/System.h"
#include "nitf/Error.h"
#include "nitf/SegmentSource.h"

NITF_CXX_GUARD

/*!
 *  TODO: Add documentation
 */
typedef struct _nitf_SegmentWriter
{
    nitf_IOHandle outputHandle;
    nitf_SegmentSource *segmentSource;
}
nitf_SegmentWriter;

/*!
 *  \fn nitf_SegmentWriter_write
 *  \brief Method used by the actual Writer to write a segment out.
 *
 *  This function writes out a segment from a source, by pulling
 *  the required amounts from the source attached.  This function is
 *  probably only useable from the Writer.write() function.  The
 *  segment source must be attached at this point, or an exception
 *  will occur.
 *
 *  \param writer The segment writer object
 *  \param An error to return on failure
 *
 */
NITFPROT(NITF_BOOL) nitf_SegmentWriter_write
(
    nitf_SegmentWriter * writer,
    nitf_Error * error
);

/*!
 *  TODO: Add documentation
 */
NITFAPI(void) nitf_SegmentWriter_destruct(nitf_SegmentWriter ** writer);

/*!
 * Attach the given segmentSource to this Writer
 * The Writer obtains ownership of the passed segmentSource, and will destruct
 * it when the Writer is destructed.
 */
NITFAPI(NITF_BOOL) nitf_SegmentWriter_attachSource
(
    nitf_SegmentWriter * writer,
    nitf_SegmentSource * segmentSource,
    nitf_Error * error
);

NITF_CXX_ENDGUARD

#endif
