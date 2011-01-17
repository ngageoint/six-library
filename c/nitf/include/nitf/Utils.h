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

#ifndef __NITF_UTILS_H__
#define __NITF_UTILS_H__

#include "nitf/System.h"

NITF_CXX_GUARD

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
 *  Convert the corners type to a string.  If for some reason, the type
 *  is not known, return it as ' ', which is the only other valid NITF
 *  value.
 *
 */
NITFAPI(char) nitf_Utils_cornersTypeAsCoordRep(nitf_CornersType type);


NITF_CXX_ENDGUARD

#endif
