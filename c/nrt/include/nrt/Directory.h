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

#ifndef __NRT_DIRECTORY_H__
#define __NRT_DIRECTORY_H__

#include "nrt/Defines.h"
#include "nrt/Types.h"
#include "nrt/Error.h"
#include "nrt/Memory.h"

NRT_CXX_GUARD

typedef void nrt_Directory;

/*!
 *  Create a directory object.  Gives back a directory object
 *  \param error An error to populate on failure
 *  \return A directory object
 */
NRTAPI(nrt_Directory *) nrt_Directory_construct(nrt_Error * error);

/*!
 *  Find the first file in a directory.
 *  \param dir The directory object to operate on
 *  \param path The path to assign this directory
 *  \return The name of the first file in the directory
 */
NRTAPI(const char *) nrt_Directory_findFirstFile(nrt_Directory * dir,
                                                 const char *path);

/*!
 *  Opens the next file in a directory.  This call should be made
 *  after a call has been already made to findFirstFile
 *  \param dir The directory object to operate on
 *  \return The name of the next file in the directory, or NULL if none
 */
NRTAPI(const char *) nrt_Directory_findNextFile(nrt_Directory * dir);

/*!
 *  Destroy the directory object in question
 */
NRTAPI(void) nrt_Directory_destruct(nrt_Directory ** dir);

/*!
 *  Check if the directory exists
 *  \param dir The name of the directory to check
 *  \return 1 if exists, 0 if not exists
 */
NRTAPI(NRT_BOOL) nrt_Directory_exists(const char *dir);

NRT_CXX_ENDGUARD
#endif
