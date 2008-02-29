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

#include "nitf/DefaultTRE.h"

NITFPRIV(NITF_BOOL) defaultInit(nitf_TRE* tre, const char* id, nitf_Error * error)
{
	return NITF_SUCCESS;
}


NITFPRIV(int) defaultRead(nitf_IOHandle io, nitf_TRE * tre, struct _nitf_Record* record, nitf_Error * error)
{
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
    data = (char *) NITF_MALLOC(tre->length + 1);
    if (!data)
    {
        nitf_Error_init(error, NITF_STRERROR(NITF_ERRNO),
                        NITF_CTXT, NITF_ERR_MEMORY);

        goto CATCH_ERROR;
    }
    memset(data, 0, tre->length + 1);

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
    descr[0].data_count = tre->length;
    descr[0].label = "Unknown raw data";
    descr[0].tag = "raw_data";
    descr[1].data_type = NITF_END;
    descr[1].data_count = 0;
    descr[1].label = NULL;
    descr[1].tag = NULL;

    tre->priv = descr;

    /*  Read the data extension into the tre  */
    success = nitf_TREUtils_readField(io, data, (int) tre->length, error);
    if (!success)
        goto CATCH_ERROR;

    field = nitf_Field_construct(tre->length, NITF_BINARY, error);
    if (field == NULL)
    {
        goto CATCH_ERROR;
    }
    /* TODO -- likely inefficient, since we end up copying the raw data here */
    if (!nitf_Field_setRawData
            (field, (NITF_DATA *) data, tre->length, error))
    {
        goto CATCH_ERROR;
    }
    nitf_HashTable_insert(tre->hash, "raw_data", field, error);

    NITF_FREE(data);

#ifdef NITF_PRINT_TRES
    printf
    ("------------------------------------------------------------\n");
    printf("[%s] length %d (unknown TRE default handler)\n", tre->tag,
           tre->length);
    printf
    ("------------------------------------------------------------\n");
    printf("\n");
#endif

    return NITF_SUCCESS;

    /* Handle any errors */
CATCH_ERROR:
    return NITF_FAILURE;
}


NITFPRIV(NITF_BOOL) defaultWrite(nitf_IOHandle io, 
								struct _nitf_TRE* tre, 
								struct _nitf_Record* record, 
								nitf_Error* error)
{
	nitf_Field* field;
	nitf_Pair* pair = nitf_HashTable_find(tre->hash, "raw_data");
	if (pair == NULL)
	{
		nitf_Error_init(error, "No raw_data in default!", NITF_CTXT, NITF_ERR_INVALID_OBJECT);
		return NITF_FAILURE;
	}
	field = (nitf_Field*)pair->data;
	
	if (!nitf_IOHandle_write(io, field->raw, field->length, error))
		return NITF_FAILURE;
	return NITF_SUCCESS;
}

NITFPRIV(NITF_BOOL) defaultNullIncrement(nitf_TREEnumerator** it, nitf_Error* error)
{
	NITF_FREE(*it);
	*it = NULL;
	return NITF_SUCCESS;
}

NITFPRIV(nitf_Pair*) defaultGet(struct _nitf_TREEnumerator* iter, nitf_Error* error)
{
	if (!iter)
	{
		nitf_Error_init(error, "Null iterator!", NITF_CTXT, NITF_ERR_INVALID_PARAMETER);
		return NULL;
	}
	return (nitf_Pair*)iter->data;
}



NITFPRIV(nitf_TREEnumerator*) defaultBegin(nitf_TRE* tre, nitf_Error* error)
{
	nitf_TREEnumerator* it = (nitf_TREEnumerator*)NITF_MALLOC(sizeof(nitf_TREEnumerator));
	/* Check rv here */
	//it->tre = tre;
	it->next = defaultNullIncrement;
	it->get = defaultGet;
	it->data = nitf_HashTable_find(tre->hash, "raw_data");
	if (it->data == NULL)
	{
		nitf_Error_init(error, "No raw_data in default!", NITF_CTXT, NITF_ERR_INVALID_OBJECT);
		return NITF_FAILURE;
	}
	return it;
	
}

NITFPRIV(NITF_BOOL) defaultSetField(nitf_TRE * tre,
                                    const char *tag,
                                    NITF_DATA * data,
                                    size_t dataLength, nitf_Error * error)
{
	nitf_Field* field;
	
	if (strcmp(tag, "raw_data"))
	{
		nitf_Error_initf(error, NITF_CTXT, NITF_ERR_INVALID_PARAMETER, "Invalid param [%s]", tag);
		return NITF_FAILURE;
	}
	
	field = nitf_Field_construct(dataLength, NITF_BINARY, error);
    if (!field)
		return NITF_FAILURE;

	 /* TODO -- likely inefficient, since we end up copying the raw data here */
    if (!nitf_Field_setRawData
            (field, (NITF_DATA *) data, dataLength, error))
        return NITF_FAILURE;

	tre->length = dataLength;

	if (nitf_HashTable_exists(tre->hash, tag))
	{
		nitf_Field* oldValue;
		nitf_Pair* pair = nitf_HashTable_find(tre->hash, tag);
		oldValue = (nitf_Field*)pair->data;
		nitf_Field_destruct(&oldValue);
		pair->data = field;
		return NITF_SUCCESS;
	}
	
	return nitf_HashTable_insert(tre->hash, tag, field, error);
	
	 

}


//typedef NITF_BOOL (*NITF_TRE_WRITER)(nitf_IOHandle, nitf_TRE* tre, struct _nitf_Record* record, nitf_Error*);

NITFPRIV(int) defaultGetCurrentSize(nitf_TRE* tre, nitf_Error* error)
{
	/* TODO: maybe should check that these two fields are equal.. tre->length and "raw_data"->length*/
	return tre->length;
}

NITFAPI(nitf_TREHandler*) nitf_DefaultTRE_handler(nitf_Error * error)
{
	static nitf_TREHandler handler =
	{
		defaultInit,
		defaultRead,
		defaultSetField,
		defaultWrite,
		defaultBegin,
		defaultGetCurrentSize,
		NULL /* We dont need this! */	
	};
	
	return &handler;
}
