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

/**
 * Returns 1 if the input string is either null, empty (strlen == 0), or
 * if every character is a whitespace char.
 */
NITFAPI(NITF_BOOL) nitf_Utils_isBlank(char *str);

NITFAPI(void) nitf_Utils_trimString(char* str);

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
NITFAPI(void) nitf_Utils_baseName(char* base,
                                  const char* fullName,
                                  const char* extension);


/*!
 *  Convert a double representing decimal degrees into 3 integers,
 *  one for degrees, one for minutes, and one for seconds.
 *
 *  The function returns these values through the passed in parameters.
 *  parameters may not be NULL
 *
 *  \param decimal An existing decimal degree
 *  \param degrees [output] The degrees as an integer
 *  \param minutes [output] The minutes as an integer
 *  \param seconds [output] The seconds as an integer
 */
NITFAPI(void) nitf_Utils_decimalToGeographic(double decimal,
                                             int *degrees,
                                             int *minutes,
                                             double *seconds);

/*!
 *  Convert the geographic coordinates (i.e., DMS) into decimal
 *  degrees as a double.
 *
 *  \param degrees geographic degrees
 *  \param minutes geographic minutes
 *  \param seconds geographic seconds
 */
NITFAPI(double) nitf_Utils_geographicToDecimal(int degrees,
                                               int minutes,
                                               double seconds);

/*!
 *  Take in a degree of DMS format and convert it into integers
 *  The string will be of the format dddmmss[NSEW] or ddmmss[NSEW]
 *  Blank values are accepted (per NITF 2500C) and converted to 0s.
 *
 *  Any other string will produce an error object with code
 *  NITF_ERR_INVALID_PARAMETER.
 *
 *  \param dms The string
 *  \param degrees [output] The degrees as an integer
 *  \param minutes [output] The minutes as an integer
 *  \param seconds [output] The seconds as an integer
 *
 */
NITFAPI(NITF_BOOL) nitf_Utils_parseGeographicString(char* dms,
                                                    int* degrees,
                                                    int* minutes,
                                                    double* seconds,
                                                    nitf_Error* error);

/*!
 *  Turn the geographic value into a string.  You must have a buffer
 *  with 7 elements to hand in to this function, otherwise you will
 *  have memory corruption.
 */
NITFPROT(void) nitf_Utils_geographicLatToCharArray(int degrees,
                                                   int minutes,
                                                   double seconds,
                                                   char *buffer7);

/*!
 *  Turn the geographic value into a string.  You must have a buffer
 *  with 7 elements to hand in to this function, otherwise you will
 *  have memory corruption.
 */
NITFPROT(void) nitf_Utils_geographicLonToCharArray(int degrees,
                                                   int minutes,
                                                   double seconds,
                                                   char *buffer8);

/*!
 *  Turn the decimal value into a string +-dd.ddd.  You must have a buffer
 *  with 7 elements to hand in to this function, otherwise you will
 *  have memory corruption.
 */
NITFPROT(void) nitf_Utils_decimalLatToCharArray(double decimal,
                                                char *buffer7);

/*!
 *  Turn the decimal value into a string +-ddd.ddd.  You must have a buffer
 *  with 7 elements to hand in to this function, otherwise you will
 *  have memory corruption.
 */
NITFPROT(void) nitf_Utils_decimalLonToCharArray(double decimal,
                                                char *buffer8);

NITFPROT(void) nitf_Utils_decimalLatToGeoCharArray(double decimal,
                                                   char *buffer7);

/*!
 *  Turn the decimal value into a string +-ddd.ddd.  You must have a buffer
 *  with 7 elements to hand in to this function, otherwise you will
 *  have memory corruption.
 */
NITFPROT(void) nitf_Utils_decimalLonToGeoCharArray(double decimal,
                                                   char *buffer8);

/*!
 *  Take in a decimal degree format string and convert it into
 *  a double.  The string will be of the format +-ddd.dd or +-dd.dd.
 *
 *  \todo This function can be expanded to handle arbitrary
 *  size conversions.  It is TBD whether this is desirable, since the
 *  IGEOLO itself is very strict about what is allowed
 *
 */
NITFAPI(NITF_BOOL) nitf_Utils_parseDecimalString(char* d,
                                                 double* decimal,
                                                 nitf_Error* error);

/*!
 *  Convert the corners type to a string.  If for some reason, the type
 *  is not known, return it as ' ', which is the only other valid NITF
 *  value.
 *
 */
NITFAPI(char) nitf_Utils_cornersTypeAsCoordRep(nitf_CornersType type);


NITFAPI(double) nitf_Utils_getCurrentTimeMillis();

NITFAPI(int) nitf_Utils_strncasecmp(char *s1, char *s2, size_t n);

NITF_CXX_ENDGUARD

#endif
