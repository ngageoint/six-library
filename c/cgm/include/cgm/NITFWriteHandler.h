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
 * License along with this program;
 * If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef __CGM_NITF_WRITE_HANDLER_H__
#define __CGM_NITF_WRITE_HANDLER_H__

#include <import/nitf.h>
#include "cgm/Metafile.h"
#include "cgm/MetafileWriter.h"

NITF_CXX_GUARD


/**
 * Create a WriteHandler for writing CGM directly to a NITF file. This would
 * be used in applications that are writing out NITF files and want to stream
 * a cgm Metafile* directly to the output NITF file.
 * 
 * \param mf    The CGM Metafile to write out
 * \param error The error object, which will get populated on error
 * \return      a nitf_WriteHandler*, or NULL on error
 */
nitf_WriteHandler* cgm_NITFWriteHandler_construct(
        cgm_Metafile *mf, nitf_Error *error);

NITF_CXX_ENDGUARD

#endif
