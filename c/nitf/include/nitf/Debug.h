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

#ifndef __NITF_DEBUG_H__
#define __NITF_DEBUG_H__



#include "nitf/Defines.h"
#include "nitf/Types.h"
#ifdef NITF_DEBUG

#    define NITF_MEM_LOG "memory_trace"


NITF_CXX_GUARD

/*!
 *  This will some space, and record that space in a file.
 *  The format will look like
 *  MALLOC <PTR> <SIZE> <FILE> <LINE>
 *  Not threadsafe!
 *
 *  \param file The file where the action occurred
 *  \param line The line where the action occurred
 *  \param sz   The size to allocate
 *  \return The Pointer, or NULL on failure
 */
NITFPROT(void *) nitf_Debug_malloc(const char *file, int line, size_t sz);

/*!
 *  Reallocate some space and record that space in a file.
 *  The format will look like
 *  REALLOC <PTR> <OLD PTR> <NEW SIZE> <FILE> <LINE>
 *  Not threadsafe!
 *
 *  \param file The file where the action occurred
 *  \param line The line where the action occurred
 *  \param sz   The sz to allocate
 *  \return The pointer or NULL on failure
 */
NITFPROT(void *) nitf_Debug_realloc(const char *file,
                                    int line, void *ptr, size_t sz);

/*!
 *  Free some space and record that space in a file.
 *  The format will look like
 *  FREE <PTR> <FILE> <LINE>
 *  \param file  The file where the action occurred
 *  \param line  The line where the action occurred
 *  \param ptr   The ptr to free
 */
NITFPROT(void) nitf_Debug_free(const char *file, int line, void *ptr);

NITF_CXX_ENDGUARD

#endif

NITF_CXX_GUARD

/*!
 *  \fn nitf_Debug_flogf
 *  \brief Used for debugging.  Prints only if NITF_DEBUG is defined
 *
 *  Print to file only if NITF_DEBUG defined
 *
 *  \param file The file to write out
 *  \param format Everything else
 */
NITFPROT(void) nitf_Debug_flogf(FILE * file, const char *format, ...);

NITF_CXX_ENDGUARD

#endif
