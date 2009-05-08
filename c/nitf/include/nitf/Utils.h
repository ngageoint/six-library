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
 * License along with this program;
 * If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef __NITF_UTILS_H__
#define __NITF_UTILS_H__

#include "nitf/System.h"
#include "nitf/Error.h"
#include "nitf/List.h"

NITF_CXX_GUARD

/*!
 * Splits a string apart, delimited by spaces. This does not use strtok since
 * it is not thread safe.)
 * \param str   null-terminated string
 * \param max   the maximum # of parts to split into (0 = split all)
 */
NITFAPI(nitf_List*) nitf_Utils_splitString(
        char *str, unsigned int max, nitf_Error* error);


NITFAPI(NITF_BOOL) nitf_Utils_isNumeric(char *str);

NITFAPI(NITF_BOOL) nitf_Utils_isAlpha(char *str);

NITFPROT(void) nitf_Utils_trimString(char* str);

/*!
 *  Replace the oldValue with the newValue within this string
 *
 */
NITFPROT(void) nitf_Utils_replace(char* str, char oldValue, char newValue);

/*!
 * Return the base name for the fullName, up until the 
 * extension is encountered.  Both base and fullName should
 * have at least NITF_MAX_PATH number of elements.
 *
 * \param base A user supplied base name target
 * \param fullName The source string
 * \param extension The extension name
 */
NITFPROT(void) nitf_Utils_baseName(char* base, 
                                   const char* fullName, 
                                   const char* extension);




NITF_CXX_ENDGUARD

#endif
