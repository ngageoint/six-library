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

#include "nitf/FieldWarning.h"

/*!
 *  Construct a FieldWarning object
 *  This function creates a FieldWarning object.
 *  \param fileOffset Offset in the file to the field
 *  \param field A string which represents the NITF field
 *  \param value The NITF field value
 *  \param expectation A string describing the expected field value
 *  \param error An error object to fill if a problem occurs
 *  \return A FieldWarning object or NULL upon failure
 */
NITFAPI(nitf_FieldWarning *) nitf_FieldWarning_construct(nitf_Off fileOffset,
        const char *fieldName,
        nitf_Field *field,
        const char
        *expectation,
        nitf_Error *
        error)
{
    nitf_FieldWarning *result;  /* Return value */

    /* Get some memory */
    result = (nitf_FieldWarning *) NITF_MALLOC(sizeof(nitf_FieldWarning));

    /* If we couldn't get memory */
    if (!result)
    {
        nitf_Error_init(error,
                        NITF_STRERROR(NITF_ERRNO),
                        NITF_CTXT, NITF_ERR_MEMORY);
        return NULL;
    }

    if (field)
    {
        result->field = nitf_Field_clone(field, error);
        if (!(result->field))
        {
            NITF_FREE(result);
            return NULL;
        }
    }
    else
    {
        result->field = NULL;
    }

    /* fileOffset */
    result->fileOffset = fileOffset;

    result->fieldName = nitf_strdup(fieldName);
    result->expectation = nitf_strdup(expectation);

    return result;
}


/*!
 *  Destruct a FieldWarning object
 *  This function destructs a FieldWarning object and NULL sets its pointer
 *  \param fieldWarningPtr A pointer to a FieldWarning object
 */
NITFAPI(void) nitf_FieldWarning_destruct(nitf_FieldWarning **
        fieldWarningPtr)
{
    if (!(*fieldWarningPtr))
    {
        return;                 /* Yeah, Yeah, one entrance one ... */
    }

    if ((*fieldWarningPtr)->fieldName)
    {
        NITF_FREE((*fieldWarningPtr)->fieldName);
    }

    if ((*fieldWarningPtr)->field)
    {
        nitf_Field_destruct(&((*fieldWarningPtr)->field));
    }

    if ((*fieldWarningPtr)->expectation)
    {
        NITF_FREE((*fieldWarningPtr)->expectation);
    }

    NITF_FREE(*fieldWarningPtr);
    *fieldWarningPtr = NULL;

    return;
}


/*!
 *  Clone a FieldWarning object
 *  This function clones (deep copies) a FieldWarning object
 *  \param source The source object
 *  \param error An error object to fill if a problem occurs
 *  \return A FieldWarning object or NULL upon failure
 */
NITFAPI(nitf_FieldWarning *) nitf_FieldWarning_clone(nitf_FieldWarning *
        source,
        nitf_Error * error)
{
    nitf_FieldWarning *result;  /* Return value */

    if (!source)
    {
        nitf_Error_initf(error,
                         NITF_CTXT,
                         NITF_ERR_INVALID_OBJECT,
                         "Trying to clone NULL pointer");
        return NULL;
    }

    /* Get some memory */
    result = (nitf_FieldWarning *) NITF_MALLOC(sizeof(nitf_FieldWarning));

    /* If we couldn't get memory */
    if (!result)
    {
        nitf_Error_init(error,
                        NITF_STRERROR(NITF_ERRNO),
                        NITF_CTXT, NITF_ERR_MEMORY);
        return NULL;
    }

    result->fileOffset = source->fileOffset;
    result->field = NULL;
    result->expectation = nitf_strdup(source->expectation);

    /* fieldName */
    result->fieldName = nitf_strdup(source->fieldName);
    if ((source->fieldName) && (!result->fieldName))
    {
        nitf_Error_init(error, NITF_STRERROR(NITF_ERRNO),
                        NITF_CTXT, NITF_ERR_MEMORY);
        goto CATCH_ERROR;
    }

    /* field */
    result->field = nitf_Field_clone(source->field, error);

    return result;
    ;
CATCH_ERROR:
    nitf_FieldWarning_destruct(&result);
    return NULL;
}
