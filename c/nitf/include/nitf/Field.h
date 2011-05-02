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

#ifndef __NITF_FIELD_H__
#define __NITF_FIELD_H__

#include "nitf/System.h"

NITF_CXX_GUARD

typedef enum _nitf_ConvType
{
    NITF_CONV_UINT,
    NITF_CONV_INT,
    NITF_CONV_REAL,
    NITF_CONV_STRING,
    NITF_CONV_RAW
} nitf_ConvType;

#define NITF_MAX_CONV_TYPE NITF_VAL_RAW

/* define standard lengths to use when converting */
#define NITF_INT8_SZ  1
#define NITF_INT16_SZ 2
#define NITF_INT32_SZ 4
#define NITF_INT64_SZ 8

/*!
 *  \enum FieldType
 *  \brief The field type describes the content of
 *  a field.
 *
 *  The field type enumeration describes the possible
 *  content information within a field.
 *
 *  At this point in time, we are assuming that, if it
 *  is BCS-A data, it is space-filled, right-aligned.
 *  If it is BCS-N, we expect zero-filled, left-aligned.
 *  If it is binary, we expect an exact fit (probably 16 or 32 bits)
 */
typedef enum _nitf_FieldType
{
    NITF_BCS_A,                 /* Field is basic character set alphanumeric */
    NITF_BCS_N,                 /* Field is basic character set numeric  */
    NITF_BINARY                 /* Field is binary data */
} nitf_FieldType;

/*!
 *  \struct nitf_Field
 *  \brief Contains information corresponding to a given value
 *
 *  The Field object contains a buffer for raw data.  This data
 *  is *not* null-terminated, and you should not try and print it.
 *  It also contains a length, which the value must satisfy, in order
 *  to be a field.  Finally, it contains a type, which is responsible
 *  for determining how it should compensate for the disparity between
 *  an actual length provided by the user, and the length that is required
 */
typedef struct _nitf_Field
{
    nitf_FieldType type;
    char *raw;
    size_t length;
    NITF_BOOL resizable; /* private member that states whether the field
                            can be resized - default is false */
}
nitf_Field;


/*!
 *  \fn nitf_Field_construct
 *  \param data The raw data to assign to the 'raw' field.
 *  \param length The length of the field.
 *  \param type The type of field we have (BCS-A, BCS-N, or binary)
 *  \param error The error to populate on failure
 *  \return The newly created field, or NULL on failure.
 *
 *  Construct a new field.  Return the field, unless an error occurred.
 */
NITFAPI(nitf_Field *) nitf_Field_construct(size_t length,
                                           nitf_FieldType type,
                                           nitf_Error * error);


/*!
 *  \fn nitf_Field_setRawData
 *  \param field The field to set
 *  \param data Any type of data, to be stored raw
 *  \param dataLength How long is the data to be stored (must be <= field->length)
 *  \param error The error to populate on miserable failure
 *  \return Success or failure
 */
NITFAPI(NITF_BOOL) nitf_Field_setRawData(nitf_Field * field,
        NITF_DATA * data,
        size_t dataLength,
        nitf_Error * error);

/*!
 *  \fn nitf_Field_setUint32
 *  \param field The field to set
 *  \param number The number to set
 *  \param error The error to populate on miserable failure
 *
 *  \return Success or failure
 *
 * The number is a unsigned 32 bit value. The number is converted to a string
 * and then placed into the field according to field type.
 *
 * For NITF_BCS_N, the field is left padded with zeros to the correct length.
 *
 * For NITF_BCS_A, the field is right padded with blanks to the correct length.
 *
 * An error is returned if the unpadded number will not fit in the field.
 *
 * If field type is NITF_BCS_BINARY or an error is returned
 */

NITFAPI(NITF_BOOL) nitf_Field_setUint32(nitf_Field * field,
                                        nitf_Uint32 number,
                                        nitf_Error * error);

/*!
 *  \fn nitf_Field_setUint64
 *  \param field The field to set
 *  \param number The number to set
 *  \param error The error to populate on miserable failure
 *
 *  \return Success or failure
 *
 * The number is a unsigned 64 bit value. There are a few fields that could
 * require a number outside of the 32 integer range (i.e., LIn in the file
 * header).
 *
 * For NITF_BCS_N, the field is left padded with zeros to the correct length.
 *
 * For NITF_BCS_A, the field is right padded with blanks to the correct length.
 *
 * An error is returned if the unpadded number will not fit in the field.
 *
 * If field type is NITF_BCS_BINARY or an error is returned
 */

NITFAPI(NITF_BOOL) nitf_Field_setUint64(nitf_Field * field,
                                        nitf_Uint64 number,
                                        nitf_Error * error);

/*!
 *  \fn nitf_Field_setInt32
 *  \param field The field to set
 *  \param number The number to set
 *  \param error The error to populate on miserable failure
 *
 *  \return Success or failure
 *
 * The number is a signed 32 bit value. The number is converted to a string
 * and then placed into the field according to field type.
 *
 * For NITF_BCS_N, the field is left padded with zeros to the correct length.
 *
 * For NITF_BCS_A, the field is right padded with blanks to the correct length.
 *
 * An error is returned if the unpadded number will not fit in the field.
 *
 * If field type is NITF_BCS_BINARY or an error is returned
 */

NITFAPI(NITF_BOOL) nitf_Field_setInt32(nitf_Field * field,
                                       nitf_Int32 number,
                                       nitf_Error * error);

/*!
 *  \fn nitf_Field_setInt64
 *  \param field The field to set
 *  \param number The number to set
 *  \param error The error to populate on miserable failure
 *
 *  \return Success or failure
 *
 * The number is a signed 64 bit value.
 *
 * For NITF_BCS_N, the field is left padded with zeros to the correct length.
 *
 * For NITF_BCS_A, the field is right padded with blanks to the correct length.
 *
 * An error is returned if the unpadded number will not fit in the field.
 *
 * If field type is NITF_BCS_BINARY or an error is returned
 */

NITFAPI(NITF_BOOL) nitf_Field_setInt64(nitf_Field * field,
                                       nitf_Int64 number,
                                       nitf_Error * error);

/*!
 *  \fn nitf_Field_trimString
 *  \param str The string to trim
 *
 *  \return None
 *
 *  nitf_Field_trimString is a helper fucntion that trims leading and trailing
 *  spaces from a string. It is intended for trimming string values returned
 *  from nitf_Field_get. The string must be NULL terminated.
 *
 *  The function clears both leading and trailing spaces.
 *
 *  A string composed entirely of blanks will become an empty string
 */

NITFAPI(void) nitf_Field_trimString(char *str);

/*!
 *  \fn nitf_Field_setString
 *  \param field The field to set
 *  \param str The string to set
 *  \param error The error to populate on miserable failure
 *
 *  \return Success or failure
 *
 * The string should be NUL terminated.
 *
 * The field being set must be either NITF_BCA_A or NITF_BCA_N.
 *
 * If the string is not long enough it is right padded with spaces or left
 * padded wit zeros, depending on the type. An error is returned if the unpadded
 * string will not fit in the field.
 *
 */

NITFAPI(NITF_BOOL) nitf_Field_setString(nitf_Field * field,
                                        const char *str, nitf_Error * error);


/**
 * Sets the value of the field to the formatted date string, represented by
 * the given DateTime and date format.
 */
NITFAPI(NITF_BOOL) nitf_Field_setDateTime(nitf_Field * field,
        nitf_DateTime *dateTime, const char *format, nitf_Error * error);


/*!
 *  \fn nitf_Field_setReal
 *  \param field The field to set
 *  \param type  Conversion type character (f,e, or E)
 *  \param plus  Include plus sign if TRUE
 *  \param value The value to set (a double)
 *  \param error The error to populate on miserable failure
 *
 *  nitf_Field_setReal sets a real valued field to the maximum precision
 *  representation of the input value. Maximum precision means most decimal
 *  places. The result will be the sign (forcing + if requested) followed
 *  by the whole part with the minimum digits (atleast one) followed byt
 *  the decimal places and then the exponent for some types. All characters
 *  in the field length are used.
 */

NITFAPI(NITF_BOOL) nitf_Field_setReal(nitf_Field * field,
                                      const char *type, NITF_BOOL plus,
                                      double value, nitf_Error *error);

/*!
 *  \fn nitf_Field_destruct
 *  \param field The field to dereference, deallocate, and NULL-set
 *
 *  Destroy the field object, and NULL-set the pointer.  This is why
 *  we require a reference to the object pointer.
 */
NITFAPI(void) nitf_Field_destruct(nitf_Field ** field);

/*!
 *  Clone this object.  This is a deep copy operation.
 *  \todo IMPLEMENT THIS
 *  \param source The source object
 *  \param error  An error to populate upon failure
 *  \return A new object that is identical to the old
 */
NITFAPI(nitf_Field *) nitf_Field_clone(nitf_Field * source,
                                       nitf_Error * error);

/*!
 *  \fn nitf_Field_get
 *  \param field The field to get
 *  \param outValue The output value (should be allocated already if a string)
 *  \param convType What to convert to
 *  \param length The capacity of outValue
 *  \param error On failure, we populate this
 *  \return A status
 *
 *  \todo If length was zero, we could cast outValue to a char** and malloc it for them
 *  This would require that the user is smart enough to pass in an address to a char* though
 *
 *  \todo Implement Uint stuff
 *  \todo Implement double and float stuff
 *  \todo Somebody has to ntohs() this stuff when it gets read.  That should be the NITF readers
 *  job, since we allow users to set this internally.
 *
 *  This method is fairly complicated.  Here is what we do:
 *  = If the data is BCS-A||BCS-N:
 *      - If the user is requesting a string, the length must include the null-byte.
 *      - If the user is requesting a raw, the length is an actual amount to memcpy
 *      - If the user is requesting a 16 bit integer
 *                   * Hurt them and then return 16 bit value
 *      - If the user is requesting a 32 bit integer
 *                   * Give them back a 32 bit integer, converted using NITF_ATO32()
 *      - If the user is requesting a 64 bit integer
 *                   * Give them back a 64 bit integer, converted using NITF_ATO64()
 *  = If the data is a binary integer
 *      - If the user is requesting an integer
 *                   * Hurt them (just kidding).  Make sure the internal and external sizes are same
 *                     then give them it back in the right format
 *      - If the user is requesting a string
 *                   * Allocate a 256 byte buffer, snprintf to the buffer, and give them what they are
 *                     asking for (currently not implemented for 64 bit numbers)
 *
 */
NITFAPI(NITF_BOOL) nitf_Field_get(nitf_Field * field,
                                  NITF_DATA * outValue,
                                  nitf_ConvType convType,
                                  size_t length, nitf_Error * error);

/**
 * Attempt to get the Field as a DateTime object, represented by the given
 * date format.
 * \return  a new DateTime object, which is up to the user to destroy
 */
NITFAPI(nitf_DateTime*) nitf_Field_asDateTime(nitf_Field * field,
        const char* dateFormat, nitf_Error * error);

/*!
 *  TODO: Add documentation
 */
NITFPROT(NITF_BOOL) nitf_Field_resetLength(nitf_Field * field,
        size_t newLength,
        NITF_BOOL keepData,
        nitf_Error * error);

/*!
 *  Print the contents of this Field
 *  \param field The field object
 */
NITFPROT(void) nitf_Field_print(nitf_Field * field);


/*!
 * Resizes the field, if it is allowed to be resized. It returns false if the
 * resizable member of the field is false. Otherwise, it attempts to resize
 * the field, which will destroy the current raw data.
 */
NITFAPI(NITF_BOOL) nitf_Field_resizeField(nitf_Field *field,
                                          size_t newLength,
                                          nitf_Error *error);

/*!
 *  TODO: Add documentation
 */
#define NITF_TRY_GET_UINT32(FIELD, DATA_PTR, error)                        \
    if (!nitf_Field_get(FIELD, DATA_PTR, NITF_CONV_UINT, NITF_INT32_SZ, error)) \
        goto CATCH_ERROR

/*!
 *  TODO: Add documentation
 */
#define NITF_TRY_GET_UINT64(FIELD, DATA_PTR, error)        \
    if (!nitf_Field_get(FIELD, DATA_PTR, NITF_CONV_UINT,NITF_INT64_SZ, error)) \
        goto CATCH_ERROR

NITF_CXX_ENDGUARD

#endif
