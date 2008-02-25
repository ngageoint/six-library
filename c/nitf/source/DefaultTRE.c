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

NITFAPI(int) nitf_DefaultTRE_handler(nitf_IOHandle io,
                                     nitf_TRE * tre, nitf_Error * error)
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
    descr[0].pattern = ".*";
    descr[0].rangeCategory = NITF_NO_RANGE;
    descr[0].minValue = NULL;
    descr[0].maxValue = NULL;
    descr[1].data_type = NITF_END;
    descr[1].data_count = 0;
    descr[1].label = NULL;
    descr[1].tag = NULL;

    tre->descrip = descr;

    /*  Read the data extension into the tre  */
    success = nitf_TRE_readField(io, data, (int) tre->length, error);
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
