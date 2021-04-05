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

#include "nitf/DefaultTRE.h"
#include "nitf/TREPrivateData.h"

#define _NITF_DEFAULT_TRE_LABEL "Unknown raw data"

NITFPRIV(NITF_BOOL) defaultInit(nitf_TRE* tre, const char* id, nitf_Error * error)
{
    (void)id;

    nitf_TREDescription* descr;

    /* create a new private data struct */
    tre->priv = nitf_TREPrivateData_construct(error);
    if (!tre->priv)
        return NITF_FAILURE;


    descr =
        (nitf_TREDescription *) NITF_MALLOC(2 *
                                            sizeof(nitf_TREDescription));
    if (!descr)
    {
        nitf_Error_init(error, NITF_STRERROR(NITF_ERRNO),
                        NITF_CTXT, NITF_ERR_MEMORY);
        return NITF_FAILURE;
    }

    descr[0].data_type = NITF_BINARY;
    descr[0].data_count = NITF_TRE_GOBBLE;
    descr[0].label = _NITF_DEFAULT_TRE_LABEL;
    descr[0].tag = NITF_TRE_RAW;
    descr[1].data_type = NITF_END;
    descr[1].data_count = 0;
    descr[1].label = NULL;
    descr[1].tag = NULL;

    ((nitf_TREPrivateData*)tre->priv)->description = descr;

    return NITF_SUCCESS;
}


NITFPRIV(const char*) defaultGetID(nitf_TRE *tre)
{
    (void)tre;

    /* always return raw - since it is a raw description */
    return NITF_TRE_RAW;
}


NITFPRIV(NITF_BOOL) defaultRead(nitf_IOInterface *io,
                                uint32_t length,
                                nitf_TRE * tre,
                                struct _nitf_Record* record,
                                nitf_Error * error)
{
    (void)record;

    nitf_Field *field = NULL;
    nitf_TREDescription *descr = NULL;
    char *data = NULL;
    NITF_BOOL success;

    if (!tre)
    {
        /* set error ??? */
        goto CATCH_ERROR;
    }

    /*  malloc the space for the raw data */
    if (length >= UINT32_MAX)
    {
        nitf_Error_init(error, "uint32_t+1 overflow", NITF_CTXT, NITF_ERR_MEMORY);
        goto CATCH_ERROR;
    }
    const size_t length_ = ((size_t)length) + 1;
    data = (char *) NITF_MALLOC(length_);
    if (!data)
    {
        nitf_Error_init(error, NITF_STRERROR(NITF_ERRNO),
                        NITF_CTXT, NITF_ERR_MEMORY);

        goto CATCH_ERROR;
    }
    memset(data, 0, length_);

    descr =
        (nitf_TREDescription *) NITF_MALLOC(2 *
                                            sizeof(nitf_TREDescription));
    if (!descr)
    {
        nitf_Error_init(error, NITF_STRERROR(NITF_ERRNO),
                        NITF_CTXT, NITF_ERR_MEMORY);
        goto CATCH_ERROR;
    }

    descr[0].data_type = NITF_BINARY;
    descr[0].data_count = length;
    descr[0].label = _NITF_DEFAULT_TRE_LABEL;
    descr[0].tag = NITF_TRE_RAW;
    descr[1].data_type = NITF_END;
    descr[1].data_count = 0;
    descr[1].label = NULL;
    descr[1].tag = NULL;

    tre->priv = nitf_TREPrivateData_construct(error);
    if (!tre->priv)
        goto CATCH_ERROR;

    ((nitf_TREPrivateData*)tre->priv)->length = length;
    ((nitf_TREPrivateData*)tre->priv)->description = descr;

    /*  Read the data extension into the tre  */
    success = nitf_TREUtils_readField(io, data, (int) length, error);
    if (!success)
        goto CATCH_ERROR;

    field = nitf_Field_construct(length, NITF_BINARY, error);
    if (field == NULL)
    {
        goto CATCH_ERROR;
    }
    /* TODO -- likely inefficient, since we end up copying the raw data here */
    if (!nitf_Field_setRawData
            (field, (NITF_DATA *) data, length, error))
    {
        goto CATCH_ERROR;
    }
    nitf_HashTable_insert(((nitf_TREPrivateData*)tre->priv)->hash,
            NITF_TRE_RAW, field, error);

    NITF_FREE(data);

#ifdef NITF_PRINT_TRES
    printf
    ("------------------------------------------------------------\n");
    printf("[%s] length %d (unknown TRE default handler)\n", tre->tag,
           length);
    printf
    ("------------------------------------------------------------\n");
    printf("\n");
#endif

    return NITF_SUCCESS;

    /* Handle any errors */
CATCH_ERROR:
    if (tre && tre->priv)
        nitf_TREPrivateData_destruct((nitf_TREPrivateData**)&tre->priv);
    if (descr) NITF_FREE(descr);
    if (data) NITF_FREE(data);
    return NITF_FAILURE;
}


NITFPRIV(NITF_BOOL) defaultWrite(nitf_IOInterface* io,
				 struct _nitf_TRE* tre,
				 struct _nitf_Record* record,
				 nitf_Error* error)
{
    (void)record;

    nitf_Field* field;
    nitf_Pair* pair = nitf_HashTable_find(
            ((nitf_TREPrivateData*)tre->priv)->hash, NITF_TRE_RAW);
    if (pair == NULL)
    {
        nitf_Error_init(error, "No raw_data in default!", NITF_CTXT, NITF_ERR_INVALID_OBJECT);
        return NITF_FAILURE;
    }
    field = (nitf_Field*)pair->data;

    if (!nitf_IOInterface_write(io, field->raw, field->length, error))
        return NITF_FAILURE;
    return NITF_SUCCESS;
}

NITFPRIV(nitf_Pair*) defaultIncrement(nitf_TREEnumerator* it, nitf_Error* error)
{
    if (it && it->data)
    {
        nitf_Pair* data = nitf_HashTable_find(
                ((nitf_TREPrivateData*)it->data)->hash, NITF_TRE_RAW);
        if (data)
        {
            it->data = NULL; /* set to NULL, since we only have one value */
            return data;
        }
    }
    nitf_Error_init(error, "Null iterator!", NITF_CTXT, NITF_ERR_INVALID_OBJECT);
    return NULL;
}

NITFPRIV(NITF_BOOL) defaultHasNext(nitf_TREEnumerator** it)
{
    if (it && *it)
    {
        if ((*it)->data)
        {
            /* next hasn't been called yet */
            return NITF_SUCCESS;
        }
        else
        {
            /* next was already called once */
            NITF_FREE(*it);
            *it = NULL;
            return NITF_FAILURE;
        }
    }
    return NITF_FAILURE;
}


NITFPRIV(const char*) defaultGetFieldDescription(nitf_TREEnumerator* it,
                                                 nitf_Error* error)
{
    if (it && it->data)
    {
        nitf_TREDescription *desc = &((nitf_TREPrivateData*)it->data)->description[0];
        if (desc->label)
            return desc->label;
    }
    nitf_Error_init(error, "No TRE Description available",
                    NITF_CTXT, NITF_ERR_INVALID_OBJECT);
    return NULL;
}


NITFPRIV(nitf_TREEnumerator*) defaultBegin(nitf_TRE* tre, nitf_Error* error)
{
	nitf_TREEnumerator* it = (nitf_TREEnumerator*)NITF_MALLOC(sizeof(nitf_TREEnumerator));
    if (it != NULL)
    {
        it->next = defaultIncrement;
        it->hasNext = defaultHasNext;
        it->getFieldDescription = defaultGetFieldDescription;
        it->data = tre->priv;

        if (!it->data || !nitf_HashTable_find(
            ((nitf_TREPrivateData*)it->data)->hash, NITF_TRE_RAW))
        {
            nitf_Error_init(error, "No raw_data in default!", NITF_CTXT, NITF_ERR_INVALID_OBJECT);
            return NITF_FAILURE;
        }
    }
	return it;
}


NITFPRIV(nitf_List*) defaultFind(nitf_TRE* tre,
				 const char* pattern,
				 nitf_Error* error)
{
    nitf_List* list;

    nitf_HashTableIterator it = nitf_HashTable_begin(
            ((nitf_TREPrivateData*)tre->priv)->hash);
    nitf_HashTableIterator end = nitf_HashTable_end(
            ((nitf_TREPrivateData*)tre->priv)->hash);

    list = nitf_List_construct(error);
    if (!list) return NULL;

    while (nitf_HashTableIterator_notEqualTo(&it, &end))
    {
        nitf_Pair* pair = nitf_HashTableIterator_get(&it);

    	if (strstr(pair->key, pattern))
    	{
    	    /* Should check this, I suppose */
    	    nitf_List_pushBack(list, pair, error);
    	}
    	nitf_HashTableIterator_increment(&it);
    }

    return list;
}

NITFPRIV(NITF_BOOL) defaultSetField(nitf_TRE* tre,
    const char* tag,
    NITF_DATA* data,
    size_t dataLength, nitf_Error* error)
{
    nitf_Field* field = NULL;

    if (strcmp(tag, NITF_TRE_RAW))
    {
        nitf_Error_initf(error, NITF_CTXT, NITF_ERR_INVALID_PARAMETER, "Invalid param [%s]", tag);
        return NITF_FAILURE;
    }

    field = nitf_Field_construct(dataLength, NITF_BINARY, error);
    if (!field)
        return NITF_FAILURE;

    /* TODO -- likely inefficient, since we end up copying the raw data here */
    if (!nitf_Field_setRawData(field, (NITF_DATA*)data, dataLength, error))
        return NITF_FAILURE;

    if (nitf_HashTable_exists(((nitf_TREPrivateData*)tre->priv)->hash, tag))
    {
        nitf_Field* oldValue;
        nitf_Pair* pair = nitf_HashTable_find(
            ((nitf_TREPrivateData*)tre->priv)->hash, tag);
        oldValue = (nitf_Field*)pair->data;
        nitf_Field_destruct(&oldValue);
        pair->data = field;
        return NITF_SUCCESS;
    }

    /* reset the lengths in two places */
    ((nitf_TREPrivateData*)tre->priv)->length = (uint32_t)dataLength;
    ((nitf_TREPrivateData*)tre->priv)->description[0].data_count = (int)dataLength;

    return nitf_HashTable_insert(((nitf_TREPrivateData*)tre->priv)->hash,
        tag, field, error);
}

NITFPRIV(nitf_Field*) defaultGetField(nitf_TRE* tre, const char* tag)
{
    nitf_Pair* pair = nitf_HashTable_find(
            ((nitf_TREPrivateData*)tre->priv)->hash, tag);
    if (!pair) return NULL;
    return (nitf_Field*)pair->data;
}


NITFPRIV(int) defaultGetCurrentSize(nitf_TRE* tre, nitf_Error* error)
{
    (void)error;

    /* TODO - should we make sure length is equal to the descr data_count ? */
    return ((nitf_TREPrivateData*)tre->priv)->length;
}


NITFPRIV(NITF_BOOL) defaultClone(nitf_TRE *source,
                                 nitf_TRE *tre,
                                 nitf_Error* error)
{
    if (!tre || !source || !source->priv)
        return NITF_FAILURE;

    nitf_TREPrivateData* sourcePriv = (nitf_TREPrivateData*)source->priv;

    /* this clones the hash */
    tre->priv = nitf_TREPrivateData_clone(sourcePriv, error);
    if (tre->priv == NULL)
        return NITF_FAILURE;
    nitf_TREPrivateData* trePriv = tre->priv;

    /* just copy over the optional length */
    trePriv->length = sourcePriv->length;

    /* setup the description how we want it */
    trePriv->description = (nitf_TREDescription *) NITF_MALLOC(
            2 * sizeof(nitf_TREDescription));
    if (!trePriv->description)
    {
        nitf_TREPrivateData_destruct(&trePriv);
        nitf_Error_init(error, NITF_STRERROR(NITF_ERRNO),
                        NITF_CTXT, NITF_ERR_MEMORY);
        return NITF_FAILURE;
    }

    trePriv->description[0].data_type = NITF_BINARY;
    trePriv->description[0].data_count = sourcePriv->description[0].data_count;
    trePriv->description[0].label = _NITF_DEFAULT_TRE_LABEL;
    trePriv->description[0].tag = NITF_TRE_RAW;
    trePriv->description[1].data_type = NITF_END;
    trePriv->description[1].data_count = 0;
    trePriv->description[1].label = NULL;
    trePriv->description[1].tag = NULL;

    tre->priv = trePriv;

    return NITF_SUCCESS;
}


NITFPRIV(void) defaultDestruct(nitf_TRE *tre)
{
    if (tre && tre->priv)
    {
        NITF_FREE(((nitf_TREPrivateData*)tre->priv)->description);
        nitf_TREPrivateData_destruct((nitf_TREPrivateData**)&tre->priv);
    }
}

NITFAPI(nitf_TREHandler*) nitf_DefaultTRE_handler(nitf_Error * error)
{
    (void)error;
    static nitf_TREHandler handler =
    {
        defaultInit,
        defaultGetID,
        defaultRead,
        defaultSetField,
        defaultGetField,
        defaultFind,
        defaultWrite,
        defaultBegin,
        defaultGetCurrentSize,
        defaultClone,
        defaultDestruct,
        NULL    /* data - We don't need this! */
    };

    return &handler;
}
