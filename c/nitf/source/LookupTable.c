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

#include "nitf/LookupTable.h"

NITFAPI(nitf_LookupTable *) nitf_LookupTable_construct(nitf_Uint32 tables,
        nitf_Uint32 entries,
        nitf_Error * error)
{
    nitf_LookupTable *lt = NULL;

    lt = (nitf_LookupTable *) NITF_MALLOC(sizeof(nitf_LookupTable));

    if (!lt)
    {
        nitf_Error_init(error, NITF_STRERROR(NITF_ERRNO),
                        NITF_CTXT, NITF_ERR_MEMORY);
        return NULL;
    }

    lt->tables = tables;
    lt->entries = entries;
    lt->table = (unsigned char *) NITF_MALLOC(tables * entries);

    if (!lt->table)
    {
        nitf_Error_init(error, NITF_STRERROR(NITF_ERRNO),
                        NITF_CTXT, NITF_ERR_MEMORY);
        nitf_LookupTable_destruct(&lt);
    }

    return lt;
}


NITFAPI(nitf_LookupTable *) nitf_LookupTable_clone(nitf_LookupTable *
        donor,
        nitf_Error * error)
{
    nitf_LookupTable *lt;

    if (!donor)
    {
        nitf_Error_initf(error,
                         NITF_CTXT,
                         NITF_ERR_INVALID_OBJECT,
                         "Trying to clone NULL pointer");
        return NULL;
    }
    if (!(donor->tables * donor->entries))
    {
        nitf_Error_initf(error,
                         NITF_CTXT,
                         NITF_ERR_INVALID_OBJECT,
                         "Trying to clone bad LUT (size)");
        return NULL;
    }
    if (!donor->table)
    {
        nitf_Error_initf(error,
                         NITF_CTXT,
                         NITF_ERR_INVALID_OBJECT,
                         "Trying to clone bad LUT (pointer)");
        return NULL;
    }
    lt = nitf_LookupTable_construct(donor->tables, donor->entries, error);
    if (lt)
    {
        memcpy(lt->table, donor->table, (donor->tables * donor->entries));
    }
    return lt;
}


NITFAPI(void) nitf_LookupTable_destruct(nitf_LookupTable ** lt)
{
    if (*lt)
    {
        if ((*lt)->table)
        {
            NITF_FREE((*lt)->table);
            (*lt)->table;
        }
        NITF_FREE((*lt));
        *lt = NULL;
    }
}


NITFAPI(NITF_BOOL) nitf_LookupTable_init(nitf_LookupTable * lut,
        nitf_Uint32 numTables,
        nitf_Uint32 numEntries,
        NITF_DATA * tables,
        nitf_Error * error)
{

    if (lut->table != NULL)     /* Look for existing table */
        NITF_FREE(lut->table);

    lut->tables = numTables;
    lut->entries = numEntries;

    /*   Make a copy of the tables */

    lut->table =
        (nitf_Uint8 *) NITF_MALLOC(numTables * numEntries *
                                   sizeof(nitf_Uint8));
    if (lut->table == NULL)
    {
        nitf_Error_initf(error, NITF_CTXT,
                         NITF_ERR_MEMORY,
                         "Error allocating look-up table");
        return (NITF_FAILURE);
    }

    memcpy(lut->table, tables, numTables * numEntries);
    return (NITF_SUCCESS);
}
