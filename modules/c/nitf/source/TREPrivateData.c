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

#include "nitf/TREPrivateData.h"


NITFAPI(nitf_TREPrivateData *) nitf_TREPrivateData_construct(
        nitf_Error * error)
{
    nitf_TREPrivateData *priv = (nitf_TREPrivateData*) NITF_MALLOC(
            sizeof(nitf_TREPrivateData));
    if (!priv)
    {
        nitf_Error_init(error, NITF_STRERROR(NITF_ERRNO),
                        NITF_CTXT, NITF_ERR_MEMORY);
        return NULL;
    }

    priv->length = 0;
    priv->descriptionName = NULL;
    priv->description = NULL;
    priv->userData = NULL;

    /* create the hashtable for the fields */
    priv->hash = nitf_HashTable_construct(NITF_TRE_HASH_SIZE, error);

    if (!priv->hash)
    {
        nitf_TREPrivateData_destruct(&priv);
        return NULL;
    }

    /* ??? change policy? */
    nitf_HashTable_setPolicy(priv->hash, NITF_DATA_ADOPT);

    return priv;
}


NITFAPI(nitf_TREPrivateData *)
nitf_TREPrivateData_clone(nitf_TREPrivateData *source, nitf_Error * error)
{
    nitf_TREPrivateData *priv = NULL;

    /* temporary nitf_List pointer */
    nitf_List *lPtr;

    /* temporary nitf_Field pointer */
    nitf_Field *field;

    /* temporary nitf_Pair pointer */
    nitf_Pair *pair;

    /* iterator to front of list */
    nitf_ListIterator iter;

    /* iterator to back of list */
    nitf_ListIterator end;

    /* used for iterating */
    int i;

    if (source)
    {
        priv = nitf_TREPrivateData_construct(error);
        if (!priv)
            goto CATCH_ERROR;

        if (!nitf_TREPrivateData_setDescriptionName(
                priv, source->descriptionName, error))
        {
            goto CATCH_ERROR;
        }

        /*  Copy the entire contents of the hash  */
        for (i = 0; i < source->hash->nbuckets; i++)
        {
            /*  Foreach chain in the hash table...  */
            lPtr = source->hash->buckets[i];
            iter = nitf_List_begin(lPtr);
            end = nitf_List_end(lPtr);

            /*  And while they are different...  */
            while (nitf_ListIterator_notEqualTo(&iter, &end))
            {
                /*  Retrieve the field at the iterator...  */
                pair = (nitf_Pair *) nitf_ListIterator_get(&iter);

                /*  Cast it back to a field...  */
                field = (nitf_Field *) pair->data;

                /* clone the field */
                field = nitf_Field_clone(field, error);

                /*  If that failed, we need to destruct  */
                if (!field)
                    goto CATCH_ERROR;

                /*  Yes, now we can insert the new field!  */
                if (!nitf_HashTable_insert(priv->hash,
                                           pair->key, field, error))
                {
                    goto CATCH_ERROR;
                }
                nitf_ListIterator_increment(&iter);
            }
        }
    }
    else
    {
        nitf_Error_initf(error,
                         NITF_CTXT,
                         NITF_ERR_INVALID_OBJECT,
                         "Trying to clone NULL pointer");
    }
    return priv;

  CATCH_ERROR:
    if (priv)
        nitf_TREPrivateData_destruct(&priv);
    return NULL;
}


/**
 * Helper function for destructing the HashTable pairs
 */
NITFPRIV(int) destructHashValue(nitf_HashTable * ht,
                                nitf_Pair * pair,
                                NITF_DATA* userData,
                                nitf_Error * error)
{
    (void)ht;
    (void)userData;
    (void)error;

    if (pair)
        if (pair->data)
            nitf_Field_destruct((nitf_Field **) & pair->data);
    return NITF_SUCCESS;
}


NITFAPI(void) nitf_TREPrivateData_destruct(nitf_TREPrivateData **priv)
{
    nitf_Error e;
    if (*priv)
    {
        if ((*priv)->descriptionName)
        {
            NITF_FREE((*priv)->descriptionName);
            (*priv)->descriptionName = NULL;
        }
        if ((*priv)->hash)
        {
            /* destruct each field in the hash */
            nitf_HashTable_foreach((*priv)->hash,
                                   (NITF_HASH_FUNCTOR) destructHashValue,
                                   NULL, &e);
            /* destruct the hash */
            nitf_HashTable_destruct(&((*priv)->hash));

        }
        NITF_FREE(*priv);
        *priv = NULL;
    }
}


NITFPROT(NITF_BOOL) nitf_TREPrivateData_flush(nitf_TREPrivateData *priv,
                                         nitf_Error * error)
{
    if (!priv)
    {
        return NITF_FAILURE;
    }

    if (priv->hash)
    {
        /* destruct each field in the hash */
        nitf_HashTable_foreach(priv->hash,
                               (NITF_HASH_FUNCTOR) destructHashValue,
                               NULL, error);
        /* destruct the hash */
        nitf_HashTable_destruct(&(priv->hash));

    }

    /* create the hashtable for the fields */
    priv->hash = nitf_HashTable_construct(NITF_TRE_HASH_SIZE, error);

    if (!priv->hash)
    {
        nitf_TREPrivateData_destruct(&priv);
        return NITF_FAILURE;
    }

    nitf_HashTable_setPolicy(priv->hash, NITF_DATA_ADOPT);

    return NITF_SUCCESS;
}


NITFPROT(NITF_BOOL) nitf_TREPrivateData_setDescriptionName(
        nitf_TREPrivateData *priv, const char* name, nitf_Error * error)
{
    /* if already set, free it */
    if (priv->descriptionName)
    {
        NITF_FREE(priv->descriptionName);
        priv->descriptionName = NULL;
    }

    /* copy the description id */
    priv->descriptionName = nitf_strdup(name);
    if ((name) && (!priv->descriptionName))
    {
        nitf_Error_init(error, NITF_STRERROR(NITF_ERRNO),
            NITF_CTXT, NITF_ERR_MEMORY);
        return NITF_FAILURE;
    }

    return NITF_SUCCESS;
}
