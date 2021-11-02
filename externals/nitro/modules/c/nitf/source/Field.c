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
 * License along with this program; if not, If not,
 * see <http://www.gnu.org/licenses/>.
 *
 */

#include <inttypes.h>

#include "nitf/Field.h"

/*  Spaces are added to the right  */
NITF_BOOL copyAndFillSpaces(nitf_Field * field,
                            const char *data,
                            size_t dataLength, nitf_Error * error)
{
    (void)error;

    memcpy(field->raw, data, dataLength);
    memset(field->raw + dataLength, ' ', field->length - dataLength);
    return NITF_SUCCESS;
}

/*  Zeros are added to the left  */
NITF_BOOL copyAndFillZeros(nitf_Field * field,
                           const char *data,
                           size_t dataLength, nitf_Error * error)
{
    (void)error;

    size_t zeros = field->length - dataLength;
    memset(field->raw, '0', zeros);
    memcpy(field->raw + zeros, data, dataLength);

    /*    Check for sign */

    /* Sign */
    if ((zeros != 0) && ((data[0] == '+') || (data[0] == '-')))
    {
        field->raw[0] = data[0];
        (field->raw + zeros)[0] = '0';
    }
    return NITF_SUCCESS;
}

NITFAPI(nitf_Field *) nitf_Field_construct(size_t length,
        nitf_FieldType type,
        nitf_Error * error)
{
    nitf_Field *field = NULL;

    if (length == 0)
    {
        nitf_Error_initf(error, NITF_CTXT, NITF_ERR_INVALID_PARAMETER,
                         "Cannot create field of size 0");
        goto CATCH_ERROR;
    }

    field = (nitf_Field *) NITF_MALLOC(sizeof(nitf_Field));
    if (!field)
    {
        nitf_Error_init(error, NITF_STRERROR(NITF_ERRNO),
                        NITF_CTXT, NITF_ERR_MEMORY);
        goto CATCH_ERROR;
    }

    field->type = type;
    field->raw = NULL;
    field->length = 0; /* this gets set by resizeField */
    field->resizable = 1; /* set to 1 so we can use the resize code */

    if (!nitf_Field_resizeField(field, length, error))
        goto CATCH_ERROR;

    field->resizable = 0; /* set to 0 - the default value */

    return field;

  CATCH_ERROR:
      if (field) nitf_Field_destruct(&field);
      return NULL;
}


NITFAPI(NITF_BOOL) nitf_Field_setRawData(nitf_Field * field,
        NITF_DATA * data,
        size_t dataLength,
        nitf_Error * error)
{
    /* check for NULL data */
    if (field == NULL || data == NULL)
    {
        nitf_Error_init(error, "NULL data",
                        NITF_CTXT, NITF_ERR_INVALID_PARAMETER);
        return NITF_FAILURE;
    }

    /* if it's resizable and a different length, we resize */
    if (field->resizable && dataLength != field->length)
    {
        if (!nitf_Field_resizeField(field, dataLength, error))
            return NITF_FAILURE;
    }

    /*  Should we also offer adoption?  */
    if (dataLength > field->length)
    {
        nitf_Error_init(error, "Invalid data length",
                        NITF_CTXT, NITF_ERR_INVALID_PARAMETER);
        return NITF_FAILURE;
    }
    /*  If it is the exact length, just memcpy  */
    if (field->length == dataLength)
    {
        memcpy(field->raw, data, field->length);
        return NITF_SUCCESS;
    }
    /*  If it is not the exact length, and it is BCS-A, fill right  */
    else if (field->type == NITF_BCS_A)
        return copyAndFillSpaces(field, (const char *) data, dataLength,
                                 error);
    else if (field->type == NITF_BCS_N)
        return copyAndFillZeros(field, (const char *) data, dataLength,
                                error);

    /*  Otherwise, we are failures -- it was binary and the length
       didnt match!
       if (dataLength != field->length && field->type == NITF_BINARY)
     */

    nitf_Error_init(error, "Invalid data length",
                    NITF_CTXT, NITF_ERR_INVALID_PARAMETER);
    return NITF_FAILURE;
}


/*  Helper for trimming strings */

NITFAPI(void) nitf_Field_trimString(char *str)
{
    nitf_Utils_trimString(str);
}


/*  Set a number field from a uint32 */

NITFAPI(NITF_BOOL) nitf_Field_setUint32(nitf_Field * field,
                                        uint32_t number,
                                        nitf_Error * error)
{
    char numberBuffer[20];      /* Holds converted number */
    uint32_t numberLen;      /* Length of converted number string */

    /*  Check the field type */

    if (field->type == NITF_BINARY)
    {
        nitf_Error_init(error, "Integer set for binary field ",
                        NITF_CTXT, NITF_ERR_INVALID_PARAMETER);
        return (NITF_FAILURE);
    }

    /*  Convert the number to a string */

    NITF_SNPRINTF(numberBuffer, 20, "%"PRIu32, number);
    numberLen = nrt_strlen32(numberBuffer);

    /* if it's resizable and a different length, we resize */
    if (field->resizable && numberLen != field->length)
    {
        if (!nitf_Field_resizeField(field, numberLen, error))
            return NITF_FAILURE;
    }

    if (numberLen > field->length)
    {
        nitf_Error_init(error, "Value for field is too long",
                        NITF_CTXT, NITF_ERR_INVALID_PARAMETER);
        return (NITF_FAILURE);
    }

    /*  Transfer and pad result */

    if (field->type == NITF_BCS_N)
        copyAndFillZeros(field, numberBuffer, numberLen, error);
    else
        copyAndFillSpaces(field, numberBuffer, numberLen, error);

    return (NITF_SUCCESS);
}


/*  Set a number field from a uint64 */

NITFAPI(NITF_BOOL) nitf_Field_setUint64(nitf_Field * field,
                                        uint64_t number,
                                        nitf_Error * error)
{
    char numberBuffer[20];      /* Holds converted number */
    uint32_t numberLen;      /* Length of converted number string */

    /*  Check the field type */

    if (field->type == NITF_BINARY)
    {
        nitf_Error_init(error, "Integer set for binary field ",
                        NITF_CTXT, NITF_ERR_INVALID_PARAMETER);
        return (NITF_FAILURE);
    }

    /*  Convert thte number to a string */

    NITF_SNPRINTF(numberBuffer, 20, "%"PRIu64"", number);
    numberLen = nrt_strlen32(numberBuffer);

    /* if it's resizable and a different length, we resize */
    if (field->resizable && numberLen != field->length)
    {
        if (!nitf_Field_resizeField(field, numberLen, error))
            return NITF_FAILURE;
    }

    if (numberLen > field->length)
    {
        nitf_Error_init(error, "Value for BCS_N field is too long",
                        NITF_CTXT, NITF_ERR_INVALID_PARAMETER);
        return (NITF_FAILURE);
    }

    /*  Transfer and pad result */

    if (field->type == NITF_BCS_N)
        copyAndFillZeros(field, numberBuffer, numberLen, error);
    else
        copyAndFillSpaces(field, numberBuffer, numberLen, error);

    return (NITF_SUCCESS);
}

/*  Set a number field from a int32 */

NITFAPI(NITF_BOOL) nitf_Field_setInt32(nitf_Field * field,
                                       int32_t number,
                                       nitf_Error * error)
{
    char numberBuffer[20];      /* Holds converted number */
    uint32_t numberLen;      /* Length of converted number string */

    /*  Check the field type */

    if (field->type == NITF_BINARY)
    {
        nitf_Error_init(error, "Integer set for binary field ",
                        NITF_CTXT, NITF_ERR_INVALID_PARAMETER);
        return (NITF_FAILURE);
    }

    /*  Convert the number to a string */

    NITF_SNPRINTF(numberBuffer, 20, "%ld", (long) number);
    numberLen = nrt_strlen32(numberBuffer);

    /* if it's resizable and a different length, we resize */
    if (field->resizable && numberLen != field->length)
    {
        if (!nitf_Field_resizeField(field, numberLen, error))
            return NITF_FAILURE;
    }

    if (numberLen > field->length)
    {
        nitf_Error_init(error, "Value for field is too long",
                        NITF_CTXT, NITF_ERR_INVALID_PARAMETER);
        return (NITF_FAILURE);
    }

    /*  Transfer and pad result */

    if (field->type == NITF_BCS_N)
        copyAndFillZeros(field, numberBuffer, numberLen, error);
    else
        copyAndFillSpaces(field, numberBuffer, numberLen, error);

    return (NITF_SUCCESS);
}

/*  Set a number field from a int64 */

NITFAPI(NITF_BOOL) nitf_Field_setInt64(nitf_Field * field,
                                       int64_t number,
                                       nitf_Error * error)
{
    char numberBuffer[20];      /* Holds converted number */
    uint32_t numberLen;      /* Length of converted number string */

    /*  Check the field type */

    if (field->type == NITF_BINARY)
    {
        nitf_Error_init(error, "Integer set for binary field ",
                        NITF_CTXT, NITF_ERR_INVALID_PARAMETER);
        return (NITF_FAILURE);
    }

    /*  Convert the number to a string */

    NITF_SNPRINTF(numberBuffer, 20, "%lld", (long long)number);
    numberLen = nrt_strlen32(numberBuffer);

    /* if it's resizable and a different length, we resize */
    if (field->resizable && numberLen != field->length)
    {
        if (!nitf_Field_resizeField(field, numberLen, error))
            return NITF_FAILURE;
    }

    if (numberLen > field->length)
    {
        nitf_Error_init(error, "Value for BCS_N field is too long",
                        NITF_CTXT, NITF_ERR_INVALID_PARAMETER);
        return (NITF_FAILURE);
    }

    /*  Transfer and pad result */

    if (field->type == NITF_BCS_N)
        copyAndFillZeros(field, numberBuffer, numberLen, error);
    else
        copyAndFillSpaces(field, numberBuffer, numberLen, error);

    return (NITF_SUCCESS);
}

/*  Set a string field */

NITFPRIV(NITF_BOOL) isBCSN(const char *str, size_t len, nitf_Error * error);
NITFPRIV(NITF_BOOL) isBCSA(const char *str, size_t len, nitf_Error * error);

NITFAPI(NITF_BOOL) nitf_Field_setString(nitf_Field * field,
                                        const char *str, nitf_Error * error)
{
    /*  Check the field type */
    if (field->type == NITF_BINARY)
    {
        nitf_Error_init(error,
                        "Type for string set for field can not be binary",
                        NITF_CTXT, NITF_ERR_INVALID_PARAMETER);
        return (NITF_FAILURE);
    }

    /*  Transfer and pad result (check for correct characters) */

    uint32_t strLen = nrt_strlen32(str); /* Length of input string */

    /* if it's resizable and a different length, we resize */
    if (field->resizable && strLen != field->length)
    {
        if (!nitf_Field_resizeField(field, strLen, error))
            return NITF_FAILURE;
    }

    if (strLen > field->length)
    {
        nitf_Error_initf(error, NITF_CTXT, NITF_ERR_INVALID_PARAMETER,
                        "Value %s is too long for field of length %"PRIu64"",
                        str, field->length);
        return (NITF_FAILURE);
    }

    if (field->type == NITF_BCS_A)
    {
        if (!isBCSA(str, strLen, error))
            return (NITF_FAILURE);
        copyAndFillSpaces(field, str, strLen, error);
    }
    else
    {
        if (!isBCSN(str, strLen, error))
            return (NITF_FAILURE);
        copyAndFillZeros(field, str, strLen, error);
    }

    return (NITF_SUCCESS);
}

NITFAPI(NITF_BOOL) nitf_Field_setDateTime(nitf_Field * field,
        const nitf_DateTime *dateTime, const char *dateFormat, nitf_Error * error)
{
    double millis;

    /*  Check the field type */
    if (field->type == NITF_BINARY)
    {
        nitf_Error_init(error,
                        "Type for date set for field can not be binary",
                        NITF_CTXT, NITF_ERR_INVALID_PARAMETER);
        return (NITF_FAILURE);
    }

    millis = dateTime ? dateTime->timeInMillis :
            nitf_Utils_getCurrentTimeMillis();

    return nitf_DateTime_formatMillis(millis, dateFormat,
            field->raw, field->length + 1, error);
}


NITFAPI(nitf_DateTime*) nitf_Field_asDateTime(nitf_Field * field,
        const char* dateFormat, nitf_Error * error)
{
    /*  Check the field type */
    if (field->type == NITF_BINARY)
    {
        nitf_Error_init(error,
                        "Type for date field can not be binary",
                        NITF_CTXT, NITF_ERR_INVALID_PARAMETER);
        return NULL;
    }

    return nitf_DateTime_fromString(field->raw, dateFormat, error);
}

/* Set a real field */

NITFAPI(NITF_BOOL) nitf_Field_setReal(nitf_Field * field,
                                      const char *type, NITF_BOOL plus,
                                      double value, nitf_Error *error)
{
    /*  Check type */

    if (
        (strcmp(type, "f") != 0)
        && (strcmp(type, "e") != 0)
        && (strcmp(type, "E") != 0))
    {
        nitf_Error_initf(error, NITF_CTXT, NITF_ERR_INVALID_PARAMETER,
                         "Invalid conversion type %s", type);
        return(NITF_FAILURE);
    }

    /* Allocate buffer used to build value */

    /* The 64 covers the puncuation and exponent and is overkill */
    uint32_t bufferLen = (uint32_t)(field->length * 2 + 64); /* Length of buffer */
    char* buffer = (char* )NITF_MALLOC(bufferLen + 1); /* Holds intermediate and final results */
    if (buffer == NULL)
    {
        nitf_Error_init(error, NITF_STRERROR(NITF_ERRNO),
                        NITF_CTXT, NITF_ERR_MEMORY);
        return(NITF_FAILURE);
    }

    /*
      Try a precision that will be too large and then adjust it based on the
      length of what you get. This results in a left justified string with the
      maximum number of decmal places. What you are actually figuring is the
      number of digits in the whole part of the number.
    */

    uint32_t precision = (uint32_t)field->length;   /* Must be too big */
    char fmt[64];              /* Format used */
    if (plus)
        NITF_SNPRINTF(fmt, 64, "%%+-1.%dl%s", precision, type);
    else
        NITF_SNPRINTF(fmt, 64, "%%-1.%dl%s", precision, type);
    size_t bufferLen_ = ((size_t)bufferLen) + 1;
    NITF_SNPRINTF(buffer, bufferLen_, fmt, value);

    bufferLen = nrt_strlen32(buffer);

    /* if it's resizable and a different length, we resize */
    if (field->resizable && bufferLen != field->length)
    {
        if (!nitf_Field_resizeField(field, bufferLen, error))
            return NITF_FAILURE;
    }

    if (bufferLen > field->length)
    {
        if (precision > bufferLen - field->length)
            precision -= (uint32_t)(bufferLen - field->length);
        else
            precision = 0;

        if (plus)
            NITF_SNPRINTF(fmt, 64, "%%+-1.%dl%s", precision, type);
        else
            NITF_SNPRINTF(fmt, 64, "%%-1.%dl%s", precision, type);
        bufferLen_ = ((size_t)bufferLen) + 1;
        NITF_SNPRINTF(buffer, bufferLen_, fmt, value);
    }

    if (!nitf_Field_setRawData(field, buffer, field->length, error))
    {
        NITF_FREE(buffer);
        return(NITF_FAILURE);
    }

    NITF_FREE(buffer);
    return(NITF_SUCCESS);
}

/*!
 *  \fn nitf_Field_destruct
 *  \param field The field to dereference, deallocate, and NULL-set
 *
 *  Destroy the field object, and NULL-set the pointer.  This is why
 *  we require a reference to the object pointer.
 */
NITFAPI(void) nitf_Field_destruct(nitf_Field ** field)
{
    if (*field)
    {
        if ((*field)->raw)
        {
            NITF_FREE((*field)->raw);
            (*field)->raw = NULL;
        }

        NITF_FREE(*field);
        *field = NULL;
    }
}


/*!
 *  Clone this object.  This is a deep copy operation.
 *
 *  \param source The source object
 *  \param error  An error to populate upon failure
 *  \return A new object that is identical to the old
 */
NITFAPI(nitf_Field *) nitf_Field_clone(nitf_Field * source,
                                       nitf_Error * error)
{
    nitf_Field *field = NULL;

    if (source)
    {
        /* construct new one */
        field = nitf_Field_construct(source->length, source->type, error);
        if (field)
        {
            field->resizable = source->resizable;
            /* set the data */
            if (!nitf_Field_setRawData
                    (field, (NITF_DATA *) source->raw, source->length, error))
            {
                nitf_Field_destruct(&field);
                field = NULL;
            }
        }
    }
    return field;
}


NITFPRIV(NITF_BOOL) fromStringToString(nitf_Field * field, char *outValue,
                                       size_t length, nitf_Error * error)
{
    (void)error;

    if (length)
    {
        if (length == 1)
        {
            outValue[0] = 0;
        }
        else
        {
            size_t amount = (( length - 1 ) < field->length ) ? (length - 1) : (field->length);
            memcpy(outValue, field->raw, amount);
            outValue[amount] = 0;
        }
    }
    return NITF_SUCCESS;
}


NITFPRIV(NITF_BOOL) toInt16(nitf_Field * field, int16_t * int16,
                            nitf_Error * error);
NITFPRIV(NITF_BOOL) toInt32(nitf_Field * field, int32_t * int32,
                            nitf_Error * error);
NITFPRIV(NITF_BOOL) toInt64(nitf_Field * field, int64_t * int64,
                            nitf_Error * error);
NITFPRIV(NITF_BOOL) toUint16(nitf_Field * field, uint16_t * int16,
                             nitf_Error * error);
NITFPRIV(NITF_BOOL) toUint32(nitf_Field * field, uint32_t * int32,
                             nitf_Error * error);
NITFPRIV(NITF_BOOL) toUint64(nitf_Field * field, uint64_t * int64,
                             nitf_Error * error);

/* We may want to rethink this function a bit.
 * The NITF spec. has many fields with a BINARY_INTEGER type
 * This could mean that the field contains 3 8-bit binary integers
 * For that example, the length of the field is 3, which isn't an integer size
 * Thus, that wouldn't work here if we just assumed BINARY_INTEGER has ONE integer in the field
 */
NITFPRIV(NITF_BOOL) fromIntToString(nitf_Field * field, char *outValue,
                                    size_t length, nitf_Error * error)
{
    char buffer[256];
    size_t actualLength = 0;
    /*  These are all two-step processes 1) get native int 2) NITF_SNPRINTF */
    switch (field->length)
    {
        case 2:
        {
            int16_t int16;
            if (!toInt16(field, &int16, error))
                goto CATCH_ERROR;
            actualLength = NITF_SNPRINTF(buffer, 256, "%d", int16);
        }
        break;
        case 4:
        {
            int32_t int32;
            if (!toInt32(field, &int32, error))
                goto CATCH_ERROR;
            actualLength = NITF_SNPRINTF(buffer, 256, "%ld", (long) int32);
        }
        break;
        case 8:
        {
            int64_t int64;
            if (!toInt64(field, &int64, error))
                goto CATCH_ERROR;
            actualLength = NITF_SNPRINTF(buffer, 256, "%lld", (long long)int64);
        }
        break;
        default:
        {
            /* otherwise, it must not be an integer value */
            return fromStringToString(field, outValue, length, error);
        }
    }
    if (actualLength > length)
    {
        nitf_Error_initf(error, NITF_CTXT, NITF_ERR_INVALID_PARAMETER,
                         "Out value too small [%ju] size required",
                         strlen(buffer));
        return NITF_FAILURE;
    }
    nrt_strcpy_s(outValue, length, buffer);
    return NITF_SUCCESS;

CATCH_ERROR:
    return NITF_FAILURE;
}


NITFPRIV(NITF_BOOL) toString(nitf_Field * field, char *outValue,
                             size_t length, nitf_Error * error)
{
    NITF_BOOL status = NITF_FAILURE;
    switch (field->type)
    {
        case NITF_BCS_A:
        case NITF_BCS_N:
            status = fromStringToString(field, outValue, length, error);
            break;
        case NITF_BINARY:
            status = fromIntToString(field, outValue, length, error);
            break;
        default:
            nitf_Error_init(error, "Unknown conversion", NITF_CTXT,
                            NITF_ERR_INVALID_PARAMETER);
            break;
    }
    return status;
}


NITFPRIV(NITF_BOOL) toRaw(nitf_Field * field, char *outValue,
                          size_t length, nitf_Error * error)
{
    NITF_BOOL status = NITF_SUCCESS;
    if (length && length <= field->length)
    {
        memcpy(outValue, field->raw, length);
    }
    else
    {
        nitf_Error_initf(error, NITF_CTXT, NITF_ERR_INVALID_PARAMETER,
                         "Length [%ju] is longer than field width [%ju]",
                         length, field->length);
        status = NITF_FAILURE;
    }
    return status;
}


NITFPRIV(NITF_BOOL) toReal(nitf_Field * field, NITF_DATA * outData,
                           size_t length, nitf_Error * error)
{
    NITF_BOOL status = NITF_SUCCESS;
    char* tmpBuf = NULL;

    switch (field->type)
    {
        case NITF_BCS_A:
        case NITF_BCS_N:
            tmpBuf = (char* )NITF_MALLOC(field->length + 1);
            if (!tmpBuf)
            {
                nitf_Error_init(error, NITF_STRERROR(NITF_ERRNO),
                                NITF_CTXT, NITF_ERR_MEMORY);
                status = NITF_FAILURE;
                break;
            }
            memcpy(tmpBuf, field->raw, field->length);
            tmpBuf[field->length] = 0;
            if (length == sizeof(float))
                *((float *) outData) = (float) atof(tmpBuf);
            else if (length == sizeof(double))
                *((double *) outData) = (double) atof(tmpBuf);
            else
            {
                nitf_Error_init(error, "toReal -> incorrect length", NITF_CTXT,
                                NITF_ERR_INVALID_PARAMETER);
                status = NITF_FAILURE;
            }
            NITF_FREE(tmpBuf);
            break;
        case NITF_BINARY:
            memcpy(outData, field->raw, length);
            break;
        default:
            nitf_Error_init(error, "Unknown conversion", NITF_CTXT,
                            NITF_ERR_INVALID_PARAMETER);
            status = NITF_FAILURE;
            break;
    }

    return status;
}


NITFPRIV(NITF_BOOL) toInt16(nitf_Field * field, int16_t * int16,
                            nitf_Error * error)
{
    (void)error;
    *int16 = *((int16_t *) field->raw);
    return NITF_SUCCESS;
}


NITFPRIV(NITF_BOOL) toInt32(nitf_Field * field, int32_t * int32,
                            nitf_Error * error)
{
    (void)error;
    *int32 = *((int32_t *) field->raw);
    return NITF_SUCCESS;
}


NITFPRIV(NITF_BOOL) toInt64(nitf_Field * field, int64_t * int64,
                            nitf_Error * error)
{
    (void)error;
    *int64 = *((int64_t *) field->raw);
    return NITF_SUCCESS;
}


NITFPRIV(NITF_BOOL) toUint16(nitf_Field * field, uint16_t * int16,
                             nitf_Error * error)
{
    (void)error;
    *int16 = *((uint16_t *) field->raw);
    return NITF_SUCCESS;
}


NITFPRIV(NITF_BOOL) toUint32(nitf_Field * field, uint32_t * int32,
                             nitf_Error * error)
{
    (void)error;
    *int32 = *((uint32_t *) field->raw);
    return NITF_SUCCESS;
}


NITFPRIV(NITF_BOOL) toUint64(nitf_Field * field, uint64_t * int64,
                             nitf_Error * error)
{
    (void)error;
    *int64 = *((uint64_t *) field->raw);
    return NITF_SUCCESS;
}


NITFPRIV(NITF_BOOL) fromStringToInt(nitf_Field * field,
                                    NITF_DATA * outData, size_t length,
                                    nitf_Error * error)
{
    char buffer[256];
    if (field->length >= 256)
    {
        nitf_Error_initf(error, NITF_CTXT, NITF_ERR_INVALID_PARAMETER,
                         "Field length too long for string conversion [%ju]",
                         field->length);
        return NITF_FAILURE;
    }
    memcpy(buffer, field->raw, field->length);
    buffer[field->length] = 0;
    switch (length)
    {
        case 1:
        {
            int8_t *int8 = (int8_t *) outData;
            *int8 = (int8_t) NITF_ATO32(buffer);
        }
        break;
        case 2:
        {
            int16_t *int16 = (int16_t *) outData;
            *int16 = (int16_t) NITF_ATO32(buffer);
        }
        break;
        case 4:
        {
            int32_t *int32 = (int32_t *) outData;
            *int32 = NITF_ATO32(buffer);
        }
        break;
        case 8:
        {
            int64_t *int64 = (int64_t *) outData;
#if defined(__aix__)
            sscanf(buffer, "%lld", int64);
#else
            *int64 = NITF_ATO64(buffer);
#endif
        }
        break;
        default:
            nitf_Error_initf(error, NITF_CTXT, NITF_ERR_INVALID_PARAMETER,
                             NITF_CTXT, "Unsupported length [%ju]", length);
            return NITF_FAILURE;
    }
    return NITF_SUCCESS;
}


NITFPRIV(NITF_BOOL) fromStringToUint(nitf_Field * field,
                                     NITF_DATA * outData, size_t length,
                                     nitf_Error * error)
{
    char buffer[256];
    if (field->length >= 256)
    {
        nitf_Error_initf(error, NITF_CTXT, NITF_ERR_INVALID_PARAMETER,
                         "Field length too long for string conversion [%ju]",
                         field->length);
        return NITF_FAILURE;
    }
    memcpy(buffer, field->raw, field->length);
    buffer[field->length] = 0;
    switch (length)
    {
        case 1:
        {
            uint8_t *int8 = (uint8_t *) outData;
            *int8 = (uint8_t) NITF_ATO32(buffer);
        }
        break;
        case 2:
        {
            uint16_t *int16 = (uint16_t *) outData;
            *int16 = (uint16_t) NITF_ATO32(buffer);
        }
        break;
        case 4:
        {
            uint32_t *int32 = (uint32_t *) outData;
            *int32 = (uint32_t) NITF_ATOU32(buffer);
        }
        break;
        case 8:
        {
            uint64_t *int64 = (uint64_t *) outData;
#if defined(__aix__)
            sscanf(buffer, "%lld", int64);
#else
            *int64 = (uint64_t) NITF_ATO64(buffer);
#endif
        }
        break;
        default:
            nitf_Error_initf(error, NITF_CTXT, NITF_ERR_INVALID_PARAMETER,
                             "Unsupported length [%ju]", length);
            return NITF_FAILURE;
    }
    return NITF_SUCCESS;
}


NITFPRIV(NITF_BOOL) toInt(nitf_Field * field,
                          NITF_DATA * outData,
                          size_t length, nitf_Error * error)
{
    /*  First we have to figure out what we are storing...
       See, its okay to convert a BCS-N to an int, and...
       its also okay to maintain a binary int...          */
    NITF_BOOL status = NITF_FAILURE;
    if (field->type == NITF_BINARY)
    {
        switch (field->length)
        {
            case 2:
                status = toInt16(field, (int16_t *) outData, error);
                break;
            case 4:
                status = toInt32(field, (int32_t *) outData, error);
                break;
            case 8:
                status = toInt64(field, (int64_t *) outData, error);
                break;
            default:
                nitf_Error_initf(error, NITF_CTXT,
                                 NITF_ERR_INVALID_PARAMETER,
                                 "Unexpected field size for int [%ju]",
                                 field->length);
        }
    }
    else
    {
        status = fromStringToInt(field, outData, length, error);
    }
    return status;
}


NITFPRIV(NITF_BOOL) toUint(nitf_Field * field,
                           NITF_DATA * outData,
                           size_t length, nitf_Error * error)
{
    /*  First we have to figure out what we are storing...
       See, its okay to convert a BCS-N to an int, and...
       its also okay to maintain a binary int...          */
    NITF_BOOL status = NITF_FAILURE;
    if (field->type == NITF_BINARY)
    {
        switch (field->length)
        {
            case 2:
                status = toUint16(field, (uint16_t *) outData, error);
                break;
            case 4:
                status = toUint32(field, (uint32_t *) outData, error);
                break;
            case 8:
                status = toUint64(field, (uint64_t *) outData, error);
                break;
            default:
                nitf_Error_initf(error, NITF_CTXT,
                                 NITF_ERR_INVALID_PARAMETER,
                                 "Unexpected field size for uint [%ju]",
                                 field->length);
        }
    }
    else
    {
        status = fromStringToUint(field, outData, length, error);
    }
    return status;
}


NITFAPI(NITF_BOOL) nitf_Field_get(nitf_Field * field,
                                  NITF_DATA * outValue,
                                  nitf_ConvType convType,
                                  size_t length, nitf_Error * error)
{
    NITF_BOOL status = NITF_FAILURE;
    switch (convType)
    {
        case NITF_CONV_UINT:
            status = toUint(field, outValue, length, error);
            break;

        case NITF_CONV_INT:
            status = toInt(field, outValue, length, error);
            break;

        case NITF_CONV_STRING:
            status = toString(field, (char *) outValue, length, error);
            break;

        case NITF_CONV_RAW:
            status = toRaw(field, (char *) outValue, length, error);
            break;

        case NITF_CONV_REAL:
            status = toReal(field, outValue, length, error);
            break;
    }
    return status;
}


NITFPROT(NITF_BOOL) nitf_Field_resetLength(nitf_Field * field,
        size_t newLength,
        NITF_BOOL keepData,
        nitf_Error * error)
{
    size_t oldLength;
    char *raw;

    if (newLength > 0)
    {
        /* remember old data */
        raw = field->raw;

        field->raw = (char *) NITF_MALLOC(newLength + 1);
        if (!field->raw)
        {
            field->raw = raw;
            nitf_Error_init(error, NITF_STRERROR(NITF_ERRNO),
                            NITF_CTXT, NITF_ERR_MEMORY);
            return 0;
        }

        field->raw[newLength] = 0; /* terminating null byte */
        oldLength = field->length;
        field->length = newLength;

        /* ignore old data */
        if (!keepData)
        {
            if (field->type == NITF_BCS_N)
                memset(field->raw, '0', newLength);
            else if (field->type == NITF_BCS_A)
                memset(field->raw, ' ', newLength);
            else
                memset(field->raw, 0, newLength);
        }
        /* copy the old data */
        else
        {
            int64_t diff = newLength - field->length;
            if (field->type == NITF_BCS_N)
                copyAndFillZeros(field, raw,
                                 diff < 0 ? newLength : oldLength, error);
            else if (field->type == NITF_BCS_A)
                copyAndFillSpaces(field, raw,
                                  diff < 0 ? newLength : oldLength, error);
            else
            {
                memset(field->raw, 0, newLength);
                memcpy(field->raw, raw, diff < 0 ? newLength : oldLength);
            }
        }

        /* free the old memory */
        NITF_FREE(raw);
    }
    else
    {
        nitf_Error_initf(error,
                         NITF_CTXT,
                         NITF_ERR_INVALID_PARAMETER,
                         "Invalid length specified");
        return 0;
    }
    return 1;
}

static void nitf_Field_snprint_(char* buffer, size_t buf_size, nitf_Field* field)
{
    if (!field || field->length <= 0)
    {
        if ((buffer != NULL) && (buf_size > 0))
        {
            buffer[0] = '\0';
        }
        return;
    }

    switch (field->type)
    {
    case NITF_BINARY:
    {
        /* avoid printing binary */
        uint64_t field_length = (uint64_t)field->length;
        if ((buffer == NULL) || (buf_size == 0))
            printf("<binary data, length %"PRIu64">", field_length);
        else
            snprintf(buffer, buf_size, "<binary data, length %"PRIu64">", field_length);
        break;
    }

    case NITF_BCS_N:
    case NITF_BCS_A:
        if ((buffer == NULL) || (buf_size == 0))
            printf("%.*s", (int)field->length, field->raw);
        else
            snprintf(buffer, buf_size, "%.*s", (int)field->length, field->raw);
        break;
    default:
        if ((buffer == NULL) || (buf_size == 0))
            printf("Invalid Field type [%d]\n", (int)field->type);
        else
            snprintf(buffer, buf_size, "Invalid Field type [%d]\n", (int)field->type);
        break;
    }
}
NITFPROT(void) nitf_Field_print(nitf_Field * field)
{
    nitf_Field_snprint_(NULL /*buffer*/, 0 /*buf_size*/, field);
}
NITFPROT(void) nitf_Field_snprint(char* buffer, size_t buf_size, nitf_Field* field)
{
    nitf_Field_snprint_(buffer, buf_size, field);
}

NITFAPI(NITF_BOOL) nitf_Field_resizeField(nitf_Field *field,
                                          size_t newLength,
                                          nitf_Error *error)
{
    char fill = 0;

    /* it must be resizable */
    if (!field->resizable)
        return NITF_FAILURE;

    if (field && newLength != field->length)
    {
        if (field->raw)
            NITF_FREE(field->raw);

        field->raw = NULL;

        /* re-malloc */
        field->raw = (char *) NITF_MALLOC(newLength + 1);
        if (!field->raw)
        {
            nitf_Error_init(error, NITF_STRERROR(NITF_ERRNO),
                            NITF_CTXT, NITF_ERR_MEMORY);
            goto CATCH_ERROR;
        }

        /* set the new length */
        field->length = newLength;

        field->raw[newLength] = 0; /* terminating null byte */
        switch (field->type)
        {
            case NITF_BCS_A:
                fill = ' ';
                break;
            case NITF_BCS_N:
                fill = '0';
                break;
            case NITF_BINARY:
                fill = 0;
                break;
            default:
                nitf_Error_initf(error, NITF_CTXT,
                                 NITF_ERR_INVALID_PARAMETER,
                                 "Invalid type [%d]", field->type);
                goto CATCH_ERROR;
        }

        memset(field->raw, fill, field->length);
    }

    return NITF_SUCCESS;

  CATCH_ERROR:
    return NITF_FAILURE;
}


/*
 *  Private function to check that a string is BCS_N.
 *
 *  A zero length string passes
 */

NITFPRIV(NITF_BOOL) isBCSN(const char *str, size_t len, nitf_Error * error)
{
    size_t ii;
    size_t startIdx;
    NITF_BOOL foundDecimalPoint = 0;

    /* Look for + or - which must be the first character */
    startIdx = (len > 0 && (str[0] == '+' || str[0] == '-')) ? 1 : 0;

    for (ii = startIdx; ii < len; ++ii)
    {
        /*
         * Some TRE's allow for all minus signs to represent
         * BCSN if number not known (e.g. BANDSB)
         * TODO: Probably should have a rule for where '/' can appear too
         *       Should also check that '.' isn't the last character in the
         *       string
         */
        const char ch = str[ii];
        if (ch == '.')
        {
            if (foundDecimalPoint)
            {
                nitf_Error_init(error,
                                "BCS_N string can only contain one decimal "
                                "point",
                                NITF_CTXT, NITF_ERR_INVALID_PARAMETER);
                return NITF_FAILURE;
            }
            else
            {
                foundDecimalPoint = 1;
            }
        }
        else if (!isdigit(ch) && ch != '-' && ch != '/')
        {
            nitf_Error_initf(error, NITF_CTXT, NITF_ERR_INVALID_PARAMETER,
                             "Invalid character %c in BCS_N string",
                             ch);
            return NITF_FAILURE;
        }
    }

    return NITF_SUCCESS;
}


/*
 *  Private function to check that a string is BCS_A.
 *
 *  A zero length string passes
 */

NITFPRIV(NITF_BOOL) isBCSA(const char *str, size_t len, nitf_Error * error)
{
    size_t ii;

    for (ii = 0; ii < len; ++ii)
    {
        const uint8_t ch = (uint8_t)str[ii];

        if (ch < 0x20 || ch > 0x7e)
        {
            nitf_Error_initf(error, NITF_CTXT, NITF_ERR_INVALID_PARAMETER,
                             "Invalid character %c in BCS_A string",
                             ch);
            return NITF_FAILURE;
        }
    }

    return NITF_SUCCESS;
}
