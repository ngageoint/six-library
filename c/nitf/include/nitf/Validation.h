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

#ifndef __NITF_VALIDATION_H__
#define __NITF_VALIDATION_H__

#include "nitf/System.h"
#include "nitf/Error.h"
#include "nitf/List.h"
#include "nitf/Field.h"
#include "nitf/FieldWarning.h"

NITF_CXX_GUARD
/* Next are some Regular expressions that relate to the NITF specification */
#define NITF_VAL_BCS "([\\x20-\\x7E]|\\x0A|\\x0C|\\x0D)"
#define NITF_VAL_BCS_A "([\\x20-\\x7E])"
#define NITF_VAL_BCS_N "([\\x30-\\x39]|\\x2B|\\x2D)"
#define NITF_VAL_BCS_N_POS_INT "([\\x30-\\x39])"
#define NITF_VAL_BCS_SPACE "[\\x20]"
#define NITF_VAL_ECS "([\\x20-\\x7E]|[\\xA0-\\xFF]|\\x0A|\\x0C|\\x0D)"

#define NITF_VAL_SIGNED_DECIMAL "[+-]?[\\x30-\\x39]+(\\x2E[\\x30-\\x39]+)?"
#define NITF_VAL_POS_DECIMAL "[\\x30-\\x39]+(\\x2E[\\x30-\\x39]+)?"

/* The NITF 2.0 Date format */
#define NITF_VAL_20_DATE "((0[1-9])|([1-2][0-9])|(3[0-1]))(([0-1][0-9])|(2[0-3]))([0-5][0-9]){2}Z(JAN|FEB|MAR|APR|MAY|JUN|JUL|AUG|SEP|OCT|NOV|DEC)\\d{2}"

/* The NITF 2.1 Date format */
#define NITF_VAL_21_DATE "(\\d{2}|[-][-]){2}((0[1-9])|(1[0-2])|[-][-])((0[1-9])|([1-2][0-9])|(3[0-1])|[-][-])(([0-1][0-9])|(2[0-3])|[-][-])([0-5][0-9]|[-][-]){2}"

/* The standard 7-char date format */
#define NITF_VAL_DATE_DDMMMYY "(([0-2]\\d)|(3[0-1])(JAN|FEB|MAR|APR|MAY|JUN|JUL|AUG|SEP|OCT|NOV|DEC)\\d{2})"

/* Regex strings for validating degrees-related fields */
#define NITF_VAL_DEGREES_DMS "([0-8][0-9]([0-5][0-9]){2}[NS]((0[0-9]{2})|(1[0-7][0-9]))([0-5][0-9]){2}[EW]){4}"
#define NITF_VAL_DEGREES_LAT_LON "([+-][0-8][0-9][.][0-9]{3}[+-]((0[0-9]{2})|(1[0-7][0-9]))[.][0-9]{3}){4}"
#define NITF_VAL_DEGREES_UTM_MGRS "([0-24-6]\\d[C-HJ-NP-X][A-HJ-N-P-Z][A-HJ-N-P-V]\\d{5}+\\d{5}){4}|(3[135789]X[A-HJ-N-P-Z][A-HJ-N-P-V]\\d{5}+\\d{5}){4}|(3\\d[C-HJ-NP-W][A-HJ-N-P-Z][A-HJ-N-P-V]\\d{5}+\\d{5}){4}"
#define NITF_VAL_DEGREES_UTM "(\\d{15}){4}"

/* Regex string for the classification chars */
#define NITF_VAL_CLASSIFICATION "[TSCRU]"

/* macro for validating a varying number of BCS_A chars */
#define NITF_VALIDATE_BCS_A(NUM)    NITF_VAL_BCS_A"{"#NUM"}"
#define NITF_VALIDATE_BCS_N(NUM)    NITF_VAL_BCS_N"{"#NUM"}"
#define NITF_VAL_BCS_A_PLUS         NITF_VAL_BCS_A"+"
#define NITF_VAL_BCS_N_PLUS         NITF_VAL_BCS_N"+"


/* define some Validation regexes */
/* Regex for ILOC in image subheader */
#define NITF_VAL_ILOC "((\\d{5})|([-]\\d{4})){2}"

/* Regex for IMAG in image subheader */
#define NITF_VAL_IMAG "(/([1-9][0-9]{2})|(0[1-9][0-9])|(0[0-9][1-9]))|("NITF_VAL_POS_DECIMAL"[ ]*)"

/* ONLY include the functions if we have validation on! */
#if defined(NITF_VALIDATION)

/*!
 *  An enumeration of Validation status codes
 */
typedef enum _nitf_ValidationStatus
{
    NITF_VAL_MATCH = 0,         /* specifies the regex matches */
    NITF_VAL_NOMATCH,           /* specifies the regex does NOT match */
    NITF_VAL_ERROR              /* specifies an error occurred trying to match */
} nitf_ValidationStatus;



/*!
 *  Returns a status code that lets us know if the nitf_Value object
 *  contains integer data that falls within the given unsigned range.
 *  It will return NITF_VAL_MATCH if it matches, NITF_VAL_NOMATCH
 *  if it does NOT match, or NITF_VAL_ERROR if an error occurred.
 *  If a NITF_VAL_NOMATCH occurs, and the problems list is not NULL,
 *  then a nitf_FieldWarning is added to the list.
 *
 *  \param field A nitf_Field object
 *  \param min the minimum value it is allowed to have
 *  \param max the maximum value it is allowed to have
 *  \param name an optional name to give, that describes the field
 *  \param problems an optional List where warnings will populate
 *  \param error An error to populate if an error occurred
 *  \return A nitf_ValidationStatus code
 */
NITFAPI(int) nitf_Validation_urange32(nitf_Field * field,
                                      nitf_Uint32 min,
                                      nitf_Uint32 max,
                                      const char *name,
                                      nitf_List * problems,
                                      nitf_Error * error);

/*!
 *  Returns a status code that lets us know if the nitf_Value object
 *  contains large integer data that falls within the given unsigned range.
 *  It will return NITF_VAL_MATCH if it matches, NITF_VAL_NOMATCH
 *  if it does NOT match, or NITF_VAL_ERROR if an error occurred.
 *  If a NITF_VAL_NOMATCH occurs, and the problems list is not NULL,
 *  then a nitf_FieldWarning is added to the list.
 *
 *  \param field A nitf_Field object
 *  \param min the minimum value it is allowed to have
 *  \param max the maximum value it is allowed to have
 *  \param name an optional name to give, that describes the field
 *  \param problems an optional List where warnings will populate
 *  \param error An error to populate if an error occurred
 *  \return A nitf_ValidationStatus code
 */
NITFAPI(int) nitf_Validation_range64(nitf_Field * field,
                                     nitf_Int64 min,
                                     nitf_Int64 max,
                                     const char *name,
                                     nitf_List * problems,
                                     nitf_Error * error);


/*!
 *  Returns a status code that lets us know if the nitf_Value object
 *  matches the given regular expression pattern.
 *  It will return NITF_VAL_MATCH if it matches, NITF_VAL_NOMATCH
 *  if it does NOT match, or NITF_VAL_ERROR if an error occurred.
 *  If a NITF_VAL_NOMATCH occurs, and the problems list is not NULL,
 *  then a nitf_FieldWarning is added to the list.
 *
 *  \param field A nitf_Field object
 *  \param pattern a regular expression
 *  \param name an optional name to give, that describes the field
 *  \param message an optional message to use if a warning is issued
 *  \param problems an optional List where warnings will populate
 *  \param error An error to populate if an error occurred
 *  \return A nitf_ValidationStatus code
 */
NITFAPI(int) nitf_Validation_pattern(nitf_Field * field,
                                     const char *pattern,
                                     const char *name,
                                     const char *message,
                                     nitf_List * problems,
                                     nitf_Error * error);


/*!
 *  Returns a status code that lets us know if the pattern matches
 *  the data in the nitf_Field object.
 *  It will return NITF_VAL_MATCH if it matches, NITF_VAL_NOMATCH
 *  if it does NOT match, or NITF_VAL_ERROR if an error occurred.
 *
 *  \param field A nitf_Field object
 *  \param pattern a regular expression
 *  \param error An error to populate if an error occurred
 *  \return A nitf_ValidationStatus code
 */
NITFAPI(int) nitf_Validation_matches(nitf_Field * field,
                                     const char *pattern,
                                     nitf_Error * error);



/*!
 *  Returns a status code that lets us know if the pattern matches
 *  the given string buf.
 *  It will return NITF_VAL_MATCH if it matches, NITF_VAL_NOMATCH
 *  if it does NOT match, or NITF_VAL_ERROR if an error occurred.
 *
 *  \param buf a string
 *  \param pattern a regular expression
 *  \param error An error to populate if an error occurred
 *  \return A nitf_ValidationStatus code
 */
NITFAPI(int) nitf_Validation_matchString(const char *buf,
        const char *pattern,
        nitf_Error * error);



/* ====================================== */
/* =============== MACROS =============== */
/* ====================================== */

/* A macro that calls the pattern validation function
 * \param FIELD the Field object
 * \param PATTERN the regex
 * \param NAME a name that gets used when a warning is created
 * It assumes the following:
 * - a NITF_BOOL variable named 'status' that contains the status so far
 * - a pointer to an nitf_Error named 'error'
 * - a pointer to a nitf_List named 'problems'
 * - a local int variable named 'tempStatus'
 * - there is a goto label named 'CATCH_ERROR that assumes an error occurs
 */
#define NITF_VALIDATE_PATTERN(FIELD, PATTERN, NAME) \
    { \
        if (nitf_Validation_pattern(FIELD, PATTERN, NAME, PATTERN, problems, error) ==  \
                NITF_VAL_ERROR) goto CATCH_ERROR; \
    }



/* A macro that calls the pattern validation function
 * \param FIELD the Field object
 * \param PATTERN the regex
 * \param NAME a name that gets used when a warning is created
 * \param WARN a warning message that is used if a warning is issued
 * It assumes the following:
 * - a NITF_BOOL variable named 'status' that contains the status so far
 * - a pointer to an nitf_Error named 'error'
 * - a pointer to a nitf_List named 'problems'
 * - a local int variable named 'tempStatus'
 * - there is a goto label named 'CATCH_ERROR that assumes an error occurs
 */
#define NITF_VALIDATE_PATTERN_MSG(FIELD, PATTERN, NAME, WARN) \
    { \
        if (nitf_Validation_pattern(FIELD, PATTERN, NAME, WARN, problems, error) ==  \
                NITF_VAL_ERROR) goto CATCH_ERROR; \
    }


/* A macro that calls the pattern validation function
 * \param FIELD the Field object
 * \param MIN the minimum value
 * \param MAX the maximum value
 * \param NAME a name that gets used when a warning is created
 * It assumes the following:
 * - a NITF_BOOL variable named 'status' that contains the status so far
 * - a pointer to an nitf_Error named 'error'
 * - a pointer to a nitf_List named 'problems'
 * - a local int variable named 'tempStatus'
 * - there is a goto label named 'CATCH_ERROR that assumes an error occurs
 */
#define NITF_VALIDATE_URANGE32(FIELD, MIN, MAX, NAME) \
    { \
        if (nitf_Validation_urange32(FIELD, MIN, MAX, NAME, problems, error) == \
                NITF_VAL_ERROR) goto CATCH_ERROR; \
    }


/* A macro that calls the pattern validation function
 * \param FIELD the Field object
 * \param MIN the minimum value
 * \param MAX the maximum value
 * \param NAME a name that gets used when a warning is created
 * It assumes the following:
 * - a NITF_BOOL variable named 'status' that contains the status so far
 * - a pointer to an nitf_Error named 'error'
 * - a pointer to a nitf_List named 'problems'
 * - a local int variable named 'tempStatus'
 * - there is a goto label named 'CATCH_ERROR that assumes an error occurs
 */
#define NITF_VALIDATE_RANGE64(FIELD, MIN, MAX, NAME) \
    { \
        if (nitf_Validation_range64(FIELD, MIN, MAX, NAME, problems, error) == \
                NITF_VAL_ERROR) goto CATCH_ERROR; \
    }


/* A macro that creates a warning and adds it to the warning list
 * \param NAME the name of the field
 * \param FIELD the Field object
 * \param MESSAGE the message, or expectation
 * It assumes the following:
 * - a pointer to an nitf_Error named 'error'
 * - a pointer to a nitf_List named 'problems'
 * - a local pointer to a nitf_FieldWarning named 'warning'
 * - there is a goto label named 'CATCH_ERROR that assumes an error occurs
 */
#define NITF_VALIDATE_WARN(NAME, FIELD, MESSAGE) \
    { \
        nitf_FieldWarning* warning = nitf_FieldWarning_construct(0, NAME, FIELD, MESSAGE, error); \
        if (!warning || !nitf_List_pushBack(problems, warning, error)) goto CATCH_ERROR; \
    }

#endif

NITF_CXX_ENDGUARD

#endif
