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


#include <import/nitf.h>

NITF_CXX_GUARD

static nitf_TREDescription description[] = {
    {NITF_BCS_A, 20, "Unique Source System Name", "RESRC" },
    {NITF_BCS_N, 3, "Record Entry Count", "RECNT" },
    {NITF_LOOP, 0, NULL, "RECNT"},
        {NITF_BCS_N, 2, "Engineering Data Label Length", "ENGLN" },
        /* This one we don't know the length of, so we have to use the special length tag */
        {NITF_BCS_A, NITF_TRE_CONDITIONAL_LENGTH, "Engineering Data Label",
                "ENGLBL", "ENGLN" },
        {NITF_BCS_N, 4, "Engineering Matrix Data Column Count", "ENGMTXC" },
        {NITF_BCS_N, 4, "Engineering Matrix Data Row Count", "ENGMTXR" },
        {NITF_BCS_A, 1, "Value Type of Engineering Data Element", "ENGTYP" },
        {NITF_BCS_N, 1, "Engineering Data Element Size", "ENGDTS" },
        {NITF_BCS_A, 2, "Engineering Data Units", "ENGDATU" },
        {NITF_BCS_N, 8, "Engineering Data Count", "ENGDATC" },
        /* This one we don't know the length of, so we have to use the special length tag */
        /* Notice that we use postfix notation to compute the length
         * We also don't know the type of data (it depends on ENGDTS), so
         * we need to override the TREHandler's read method.  If we don't do
         * this, not only will the field type potentially be wrong, but
         * strings will be endian swapped if they're of length 2 or 4. */
        {NITF_BINARY, NITF_TRE_CONDITIONAL_LENGTH, "Engineering Data",
                "ENGDATA", "ENGDATC ENGDTS *"},
    {NITF_ENDLOOP, 0, NULL, NULL},
    {NITF_END, 0, NULL, NULL}
};

static nitf_TREDescriptionInfo descriptions[] = {
        { "ENGRDA", description, NITF_TRE_DESC_NO_LENGTH },
        { NULL, NULL, NITF_TRE_DESC_NO_LENGTH }
    };

static nitf_TREDescriptionSet descriptionSet = { 0, descriptions };

static char *ident[] = { NITF_PLUGIN_TRE_KEY, "ENGRDA", NULL };

static nitf_TREHandler engrdaHandler;

/* TODO  This is a cut and paste of nitf_TREUtils_parse() with a little bit
 *       of extra logic for determining the appropriate field type for
 *       engineering data.  Is there a way to reuse more of the other
 *       parse function? */
NITFPRIV(int) ENGRDA_parse(nitf_TRE * tre,
                           char *bufptr,
                           nitf_Error * error)
{
    int status = 1;
    int iterStatus = NITF_SUCCESS;
    int offset = 0;
    int length;
    nitf_TRECursor cursor;
    nitf_Field *field = NULL;
    nitf_TREPrivateData *privData = NULL;
    nitf_FieldType prevValueType;
    nitf_FieldType fieldType;

    /* get out if TRE is null */
    if (!tre)
    {
        nitf_Error_init(error, "parse -> invalid tre object",
                NITF_CTXT, NITF_ERR_INVALID_PARAMETER);
        return NITF_FAILURE;
    }

    privData = (nitf_TREPrivateData*)tre->priv;

    /* flush the hash first, to protect from duplicate entries */
    if (privData)
    {
        nitf_TREPrivateData_flush(privData, error);
    }

    cursor = nitf_TRECursor_begin(tre);
    prevValueType = NITF_BINARY;
    while (offset < privData->length && status)
    {
        if ((iterStatus =
             nitf_TRECursor_iterate(&cursor, error)) == NITF_SUCCESS)
        {
            length = cursor.length;
            if (length == NITF_TRE_GOBBLE)
            {
                length = privData->length - offset;
            }

            /* no need to call setValue, because we already know
             * it is OK for this one to be in the hash
             */

            /* for engineering data, the TREDescription specifies the type as
             * binary but in reality it's based on the value type field.  this
             * will be saved off for us below.  it's also critical to set this
             * correctly so that string types don't get endian swapped. */
            fieldType =
                !strncmp(cursor.tag_str, "ENGDATA", 7) ?
                    prevValueType : cursor.desc_ptr->data_type;

            /* construct the field */
            field = nitf_Field_construct(length, fieldType, error);
            if (!field)
                goto CATCH_ERROR;

            /* first, check to see if we need to swap bytes */
            if (field->type == NITF_BINARY
                    && (length == NITF_INT16_SZ || length == NITF_INT32_SZ))
            {
                if (length == NITF_INT16_SZ)
                {
                    nitf_Int16 int16 =
                        (nitf_Int16)NITF_NTOHS(*((nitf_Int16 *) (bufptr + offset)));
                    status = nitf_Field_setRawData(field,
                            (NITF_DATA *) & int16, length, error);
                }
                else if (length == NITF_INT32_SZ)
                {
                    nitf_Int32 int32 =
                        (nitf_Int32)NITF_NTOHL(*((nitf_Int32 *) (bufptr + offset)));
                    status = nitf_Field_setRawData(field,
                            (NITF_DATA *) & int32, length, error);
                }
            }
            else
            {
                /* check for the other binary lengths ... */
                if (field->type == NITF_BINARY)
                {
                    /* TODO what to do??? 8 bit is ok, but what about 64? */
                    /* for now, just let it go through... */
                }

                /* now, set the data */
                status = nitf_Field_setRawData(field, (NITF_DATA *) (bufptr + offset),
                        length, error);
            }

            /* when we see the value type, save it off
             * we'll eventually read this when we get to the engineering data
             * itself */
            if (!strncmp(cursor.tag_str, "ENGTYP", 6) &&
                field->type == NITF_BCS_A &&
                field->length == 1)
            {
                prevValueType = (field->raw[0] == 'A') ?
                    NITF_BCS_A : NITF_BINARY;
            }

#ifdef NITF_DEBUG
            {
                fprintf(stdout, "Adding Field [%s] to TRE [%s]\n",
                        cursor.tag_str, tre->tag);
            }
#endif

            /* add to the hash */
            nitf_HashTable_insert(((nitf_TREPrivateData*)tre->priv)->hash,
                    cursor.tag_str, field, error);

            offset += length;
        }
        /* otherwise, the iterate function thinks we are done */
        else
        {
            break;
        }
    }
    nitf_TRECursor_cleanup(&cursor);

    /* check if we still have more to parse, and throw an error if so */
    if (offset < privData->length)
    {
        nitf_Error_init(error, "TRE data is longer than it should be",
                NITF_CTXT, NITF_ERR_INVALID_OBJECT);
        status = NITF_FAILURE;
    }
    return status;

    /* deal with errors here */
    CATCH_ERROR:
    return NITF_FAILURE;
}

/* TODO  This is a cut and paste of nitf_TREUtils_basicRead() except that it
 *       calls ENGRDA_parse() instead.  Is there a way to reuse more of the
 *       original read function?
 */
NITFPRIV(NITF_BOOL) ENGRDA_read(nitf_IOInterface* io,
                                nitf_Uint32 length,
                                nitf_TRE* tre,
                                struct _nitf_Record* record,
                                nitf_Error* error)
{
    int ok;
    char *data = NULL;
    nitf_TREDescriptionSet *descriptions = NULL;
    nitf_TREDescriptionInfo *infoPtr = NULL;

    if (!tre)
        return NITF_FAILURE;

    data = (char*)NITF_MALLOC( length );
    if (!data)
    {
        nitf_Error_init(error, NITF_STRERROR( NITF_ERRNO ),NITF_CTXT, NITF_ERR_MEMORY );
        return NITF_FAILURE;
    }
    memset(data, 0, length);
    if (!nitf_TREUtils_readField(io, data, length, error))
    {
        NITF_FREE(data);
        return NITF_FAILURE;
    }

    descriptions = (nitf_TREDescriptionSet*)tre->handler->data;

    if (!descriptions)
    {
        nitf_Error_init(error, "TRE Description Set is NULL",
                        NITF_CTXT, NITF_ERR_INVALID_OBJECT);

        NITF_FREE(data);
        return NITF_FAILURE;
    }

    tre->priv = NULL;
    infoPtr = descriptions->descriptions;
    tre->priv = nitf_TREPrivateData_construct(error);
    ((nitf_TREPrivateData*)tre->priv)->length = length;

    ok = NITF_FAILURE;
    while (infoPtr && (infoPtr->description != NULL))
    {
        if (!tre->priv)
        {
            break;
        }

        ((nitf_TREPrivateData*)tre->priv)->description = infoPtr->description;
#ifdef NITF_DEBUG
        printf("Trying TRE with description: %s\n\n", infoPtr->name);
#endif
        ok = ENGRDA_parse(tre, data, error);
        if (ok)
        {
            nitf_TREPrivateData *priv = (nitf_TREPrivateData*)tre->priv;
            /* copy the name */
            if (!nitf_TREPrivateData_setDescriptionName(
                    priv, infoPtr->name, error))
            {
                /* something bad happened... so we need to cleanup */
                NITF_FREE(data);
                nitf_TREPrivateData_destruct(&priv);
                tre->priv = NULL;
                return NITF_FAILURE;
            }
            break;
        }

        infoPtr++;
    }

    if (data) NITF_FREE(data);
    return ok;
}

NITFAPI(char**) ENGRDA_init(nitf_Error* error)
{
    if (!nitf_TREUtils_createBasicHandler(&descriptionSet,
                                          &engrdaHandler,
                                          error))
    {
        return NULL;
    }

    // Override the read function with one that will set the engineering
    // data's field type based on the TRE's value type
    engrdaHandler.read = ENGRDA_read;

    return ident;
}

NITFAPI(nitf_TREHandler*) ENGRDA_handler(nitf_Error* error)
{
    return &engrdaHandler;
}

NITF_CXX_ENDGUARD
