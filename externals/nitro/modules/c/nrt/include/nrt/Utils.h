/* =========================================================================
 * This file is part of NITRO
 * =========================================================================
 *
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
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
#pragma once

#include "nrt/System.h"
#include "nrt/List.h"

NRT_CXX_GUARD
/*!
 * Splits a string apart, delimited by spaces. This does not use strtok since
 * it is not thread safe.)
 * \param str   null-terminated string
 * \param max   the maximum # of parts to split into (0 = split all)
 */
NRTAPI(nrt_List *) nrt_Utils_splitString(const char *str, unsigned int max,
                                         nrt_Error * error);

NRTAPI(NRT_BOOL) nrt_Utils_isNumeric(const char *str);

NRTAPI(NRT_BOOL) nrt_Utils_isAlpha(const char *str);

/**
 * Returns 1 if the input string is either null, empty (strlen == 0), or
 * if every character is a whitespace char.
 */
NRTAPI(NRT_BOOL) nrt_Utils_isBlank(const char *str);

NRTAPI(void) nrt_Utils_trimString(char *str);

/*!
 *  Replace the oldValue with the newValue within this string
 *
 */
NRTPROT(void) nrt_Utils_replace(char *str, char oldValue, char newValue);

/*!
 * Return the base name for the fullName, up until the
 * extension is encountered.  Both base and fullName should
 * have at least NRT_MAX_PATH number of elements.
 *
 * \param base A user supplied base name target
 * \param fullName The source string
 * \param extension The extension name
 */
NRTAPI(void) nrt_Utils_baseName(char *base, const char *fullName,
                                const char *extension);

/*!
 *  Take in a decimal degree format string and convert it into
 *  a double.  The string will be of the format +-ddd.dd or +-dd.dd.
 *
 *  \param d The decimal string to convert
 *  \param decimal [output] The value of the given string
 *  \param error Set if there is an error
 *  \return NITF_SUCCESS or NITF_FAILURE as appropriate
 *
 *  \todo This function can be expanded to handle arbitrary
 *  size conversions.  It is TBD whether this is desirable, since the
 *  IGEOLO itself is very strict about what is allowed
 *
 */
NRTAPI(NRT_BOOL) nrt_Utils_parseDecimalString(const char* d, double *decimal,
                                              nrt_Error * error);

NRTAPI(double) nrt_Utils_getCurrentTimeMillis(void);

NRTAPI(int) nrt_Utils_strncasecmp(const char *s1, const char *s2, size_t n);

/*!
 *  Convert a double representing decimal degrees into 2 integers,
 *  one for degrees, one for minutes, and a double for seconds.
 *
 *  The sign of the value is given by the most significant nonzero DMS value.
 *  All remaining values should be positive.
 *
 *  The function returns these values through the passed in parameters.
 *  Parameters may not be NULL
 *
 *  \param decimal An existing decimal degree
 *  \param degrees [output] The degrees as an integer
 *  \param minutes [output] The minutes as an integer
 *  \param seconds [output] The seconds as an integer
 */
NRTAPI(void) nrt_Utils_decimalToGeographic(double decimal, int *degrees,
                                           int *minutes, double *seconds);

/*!
 *  Convert the geographic coordinates (i.e., DMS) into decimal
 *  degrees as a double. The sign of the value is given by the most significant
 *  nonzero DMS value. All remaining values should be positive.
 *
 *  \param degrees geographic degrees
 *  \param minutes geographic minutes
 *  \param seconds geographic seconds
 */
NRTAPI(double) nrt_Utils_geographicToDecimal(int degrees, int minutes,
                                             double seconds);

/*!
 *  Take in a degree of DMS format and convert it into integers
 *  The string will be of the format dddmmss[NSEW] or ddmmss[NSEW]
 *  Blank values are accepted (per NRT 2500C) and converted to 0s.
 *
 *  The sign of the result will be given by the largest non-zero number.
 *  All other numbers will be nonnegative.
 *
 *  Any other string will produce an error object with code
 *  NRT_ERR_INVALID_PARAMETER.
 *
 *  \param dms The string
 *  \param degrees [output] The degrees as an integer
 *  \param minutes [output] The minutes as an integer
 *  \param seconds [output] The seconds as an integer
 *
 */
NRTAPI(NRT_BOOL) nrt_Utils_parseGeographicString(const char *dms, int *degrees,
                                                 int *minutes, double *seconds,
                                                 nrt_Error * error);

/*!
 *  Turn the geographic value into a string.  You must have a buffer
 *  with 7 elements to hand in to this function, otherwise you will
 *  have memory corruption.
 */
NRTPROT(void) nrt_Utils_geographicLatToCharArray(int degrees, int minutes,
                                                 double seconds, char *buffer7);

/*!
 *  Turn the geographic value into a string.  You must have a buffer
 *  with 7 elements to hand in to this function, otherwise you will
 *  have memory corruption.
 */
NRTPROT(void) nrt_Utils_geographicLonToCharArray(int degrees, int minutes,
                                                 double seconds, char *buffer8);

/*!
 *  Turn the decimal value into a string +-dd.ddd.  You must have a buffer
 *  with 7 elements to hand in to this function, otherwise you will
 *  have memory corruption.
 */
NRTPROT(void) nrt_Utils_decimalLatToCharArray(double decimal, char *buffer7);

/*!
 *  Turn the decimal value into a string +-ddd.ddd.  You must have a buffer
 *  with 7 elements to hand in to this function, otherwise you will
 *  have memory corruption.
 */
NRTPROT(void) nrt_Utils_decimalLonToCharArray(double decimal, char *buffer8);

NRTPROT(void) nrt_Utils_decimalLatToGeoCharArray(double decimal, char *buffer7);

/*!
 *  Turn the decimal value into a string +-ddd.ddd.  You must have a buffer
 *  with 7 elements to hand in to this function, otherwise you will
 *  have memory corruption.
 */
NRTPROT(void) nrt_Utils_decimalLonToGeoCharArray(double decimal, char *buffer8);

/*!
 *  Convert the corners type to a string.  If for some reason, the type
 *  is not known, return it as ' ', which is the only other valid NRT
 *  value.
 *
 */
NRTAPI(char) nrt_Utils_cornersTypeAsCoordRep(nrt_CornersType type);

/*!
 *  Byte-swap a given value of length `size` bytes in-place.
 *  Sizes of length 2, 4, and 8 are supported.
 *
 *  \param value Pointer to value to be swapped
 *  \param size The size, in bytes, of each buffer element
 */
NRTAPI(void) nrt_Utils_byteSwap(uint8_t* value, size_t size);

NRTAPI(void) nrt_strcpy_s(char* dest, size_t sz, const char* src);
NRTAPI(void) nrt_strncpy_s(char* dest, size_t dest_sz, const char* src, size_t src_chars);
NRTAPI(void) nrt_strcat_s(char* dest, size_t sz, const char* src);
NRTAPI(char*) nrt_strdup(const char* src);

NRTAPI(size_t) nrt_strlen(const char* src); // aka strlen()
NRTAPI(uint32_t) nrt_strlen32(const char* src);
NRTAPI(uint16_t) nrt_strlen16(const char* src);
NRTAPI(uint8_t) nrt_strlen8(const char* src);

NRT_CXX_ENDGUARD
#endif
