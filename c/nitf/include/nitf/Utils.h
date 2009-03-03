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


NITF_CXX_ENDGUARD

#endif
