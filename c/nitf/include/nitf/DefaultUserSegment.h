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

#ifndef __NITF_DEFAULT_USER_SEGMENT_H__
#define __NITF_DEFAULT_USER_SEGMENT_H__

#include "nitf/System.h"
#include "nitf/UserSegment.h"
#include "nitf/TRE.h"
#include "nitf/Record.h"

NITF_CXX_GUARD

/*!
 *  \fn nitf_DefaultUserSegment_construct
 *  \brief The default UserSegment constructor is what gets run if no
 *  constructor is found as a plugin.
 *
 *  When the reader finds a DE UserSegment, it first checks the plug-in registry
 *  to see if it has a known plug-in handler.  If not, this function
 *  is called instead.  The default constructor behavior is simply to
 *  store the raw data in a string in the hash table.
 *
 *  \param error The structure to populate if an error occurs
 *  \return The nitf_IUserSegment* interface
 *
 */
NITFAPI(void *) nitf_DefaultUserSegment_construct(nitf_Error * error);


NITF_CXX_ENDGUARD

#endif
