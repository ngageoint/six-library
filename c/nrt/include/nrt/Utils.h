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

#ifndef __NRT_UTILS_H__
#define __NRT_UTILS_H__

#include "nrt/System.h"
#include "nrt/List.h"

NRT_CXX_GUARD

/*!
 * Splits a string apart, delimited by spaces. This does not use strtok since
 * it is not thread safe.)
 * \param str   null-terminated string
 * \param max   the maximum # of parts to split into (0 = split all)
 */
NRTAPI(nrt_List*) nrt_Utils_splitString(
        char *str, unsigned int max, nrt_Error* error);

NRTAPI(NRT_BOOL) nrt_Utils_isNumeric(char *str);

NRTAPI(NRT_BOOL) nrt_Utils_isAlpha(char *str);

/**
 * Returns 1 if the input string is either null, empty (strlen == 0), or
 * if every character is a whitespace char.
 */
NRTAPI(NRT_BOOL) nrt_Utils_isBlank(char *str);

NRTAPI(void) nrt_Utils_trimString(char* str);

/*!
 *  Replace the oldValue with the newValue within this string
 *
 */
NRTPROT(void) nrt_Utils_replace(char* str, char oldValue, char newValue);

/*!
 * Return the base name for the fullName, up until the
 * extension is encountered.  Both base and fullName should
 * have at least NRT_MAX_PATH number of elements.
 *
 * \param base A user supplied base name target
 * \param fullName The source string
 * \param extension The extension name
 */
NRTAPI(void) nrt_Utils_baseName(char* base,
        const char* fullName,
        const char* extension);

/*!
 *  Take in a decimal degree format string and convert it into
 *  a double.  The string will be of the format +-ddd.dd or +-dd.dd.
 *
 *  \todo This function can be expanded to handle arbitrary
 *  size conversions.  It is TBD whether this is desirable, since the
 *  IGEOLO itself is very strict about what is allowed
 *
 */
NRTAPI(NRT_BOOL) nrt_Utils_parseDecimalString(char* d,
        double* decimal,
        nrt_Error* error);

NRTAPI(double) nrt_Utils_getCurrentTimeMillis();

NRTAPI(int) nrt_Utils_strncasecmp(char *s1, char *s2, size_t n);

NRT_CXX_ENDGUARD

#endif
