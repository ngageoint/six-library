/* =========================================================================
 * This file is part of NITRO
 * =========================================================================
 * 
 * (C) Copyright 2004 - 2010, General Dynamics - Advanced Information Systems
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

#include "nitf/System.h"
#include "nitf/SegmentSource.h"
#include "nitf/WriteHandler.h"

NITF_CXX_GUARD

typedef nitf_WriteHandler nitf_SegmentWriter;

/*!
 * Constructs a new SegmentWriter.
 */
NITFAPI(nitf_SegmentWriter*) nitf_SegmentWriter_construct(nitf_Error *error);

/*!
 * Attach the given segmentSource to this Writer
 * The Writer obtains ownership of the passed segmentSource, and will destruct
 * it when the Writer is destructed.
 */
NITFAPI(NITF_BOOL) nitf_SegmentWriter_attachSource(nitf_SegmentWriter *writer,
        nitf_SegmentSource *segmentSource,
        nitf_Error *error);


NITF_CXX_ENDGUARD

#endif
