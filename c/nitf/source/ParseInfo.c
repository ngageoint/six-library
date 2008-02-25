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

#include "nitf/ParseInfo.h"
#include "nitf/Field.h"

/*!
 *  Construct a ParseInfo object
 *  This function creates an empty ParseInfo object.
 *  \param error An error object to fill if a problem occurs
 *  \return A ParseInfo object or NULL upon failure
 */
NITFAPI(nitf_ParseInfo *) nitf_ParseInfo_construct(nitf_Error * error)
{
    nitf_ParseInfo *result;     /* Return value */

    /* Get some memory */
    result = (nitf_ParseInfo *) NITF_MALLOC(sizeof(nitf_ParseInfo));

    /* If we couldn't get memory */
    if (!result)
    {
        nitf_Error_init(error,
                        NITF_STRERROR(NITF_ERRNO),
                        NITF_CTXT, NITF_ERR_MEMORY);
        return NULL;
    }

    result->hashTable =
        nitf_HashTable_construct(NITF_PARSEINFO_NUMHASHBUCKETS, error);
    /* If we couldn't create the object */
    if (!(result->hashTable))
    {
        /* Clean up */
        nitf_ParseInfo_destruct(&result);
        return NULL;
    }

    /* Set the policy so the HashTable does not gain ownership */
    nitf_HashTable_setPolicy(result->hashTable, NITF_DATA_RETAIN_OWNER);

    return result;
}


/*!
 *  Destruct a ParseInfo object
 *  This function destructs a ParseInfo object and NULL sets its pointer
 *  \param parseInfoPtr A pointer to a parseInfo object
 */
NITFAPI(void) nitf_ParseInfo_destruct(nitf_ParseInfo ** parseInfoPtr)
{
    if (!(*parseInfoPtr))
    {
        return;                 /* Yeah, Yeah, one entrance one ... */
    }

    if ((*parseInfoPtr)->hashTable)
    {
        nitf_HashTable_destruct(&((*parseInfoPtr)->hashTable));
    }

    NITF_FREE(*parseInfoPtr);
    *parseInfoPtr = NULL;

    return;
}


/*!
 *  Get information out of the ParseInfo object
 *  This function returns information in the ParseInfo object.  If the
 *  information does not exist (is not valid) NULL is returned.
 *  \param parseInfo A parseInfo object
 *  \param dataPtr A pointer to the data to be stored
 *  \param fieldName A string describing the field to check
 *  \param error An error object to fill if a problem occurs
 *  \return 1 on success and 0 otherwise
 */

NITFAPI(NITF_BOOL) nitf_ParseInfo_get(nitf_ParseInfo * parseInfo,
                                      NITF_DATA * dataPtr,
                                      const char *fieldName,
                                      nitf_Error * error)
{
    dataPtr = nitf_HashTable_find(parseInfo->hashTable, fieldName);
    if (dataPtr)
    {
        dataPtr = ((nitf_Pair *) dataPtr)->data;
    }

    return 1;
}


/*!
 *  Set information in the ParseInfo object
 *  This function sets information in the ParseInfo object. This information
 *  is assumed to have already been validated.  Be careful, what is actually
 *  stored is a pointer to the data, so make sure it has been dynamically
 *  allocated.
 *  \param parseInfo A parseInfo object
 *  \param fieldName A string describing the field to assign the data to
 *  \param dataPtr Pointer to the data to associate with the field
 *  \param error An error object to fill if a problem occurs
 *  \return 1 on success and 0 otherwise
 */
NITFPROT(NITF_BOOL) nitf_ParseInfo_set(nitf_ParseInfo * parseInfo,
                                       const char *fieldName,
                                       NITF_DATA * dataPtr,
                                       nitf_Error * error)
{
    NITF_BOOL success;          /* A flag */

    if (nitf_HashTable_exists(parseInfo->hashTable, fieldName))
    {
        nitf_Error_init(error,
                        "Field name already in use",
                        NITF_CTXT, NITF_ERR_INVALID_PARAMETER);
        return 0;
    }

    success = nitf_HashTable_insert(parseInfo->hashTable,
                                    fieldName, dataPtr, error);
    if (!success)
    {
        return 0;
    }

    return 1;
}


/*!
 *  Remove information from the ParseInfo object.
 *  NOT DELETE, here is the deal... When an object is removed the ParseInfo
 *  object passes back a handle to it and simply forgets about it.  That
 *  dynamic memory then becomes your responsibility.
 *  \param parseInfo A parseInfo object
 *  \param fieldName A string describing the field to remove
 *  \param error An error object to fill a problem occurs
 *  \return Pointer to the removed data or NULL on failure
 */
NITFPROT(NITF_DATA *) nitf_ParseInfo_remove(nitf_ParseInfo * parseInfo,
        const char *fieldName,
        nitf_Error * error)
{
    NITF_DATA *result;          /* Return value */

    if (!nitf_HashTable_exists(parseInfo->hashTable, fieldName))
    {
        nitf_Error_init(error,
                        "Invalid field name",
                        NITF_CTXT, NITF_ERR_INVALID_PARAMETER);
        return NULL;
    }

    result = nitf_HashTable_remove(parseInfo->hashTable, fieldName);
    if (!result)
    {
        nitf_Error_init(error,
                        "HashTable failure", NITF_CTXT, NITF_ERR_UNK);
        return NULL;
    }

    return result;
}
