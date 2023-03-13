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

#include "nitf/LookupTable.h"

NITFAPI(nitf_LookupTable *) nitf_LookupTable_construct(uint32_t tables,
        uint32_t entries,
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
    lt->table = NULL;
    if (!nitf_LookupTable_init(lt, tables, entries, NULL, error))
    {
        nitf_LookupTable_destruct(&lt);
        lt = NULL;
    }

    return lt;
}

static size_t nitf_LookupTable_size(uint32_t numTables, uint32_t numEntries)
{
    const size_t sz = ((size_t)numTables) * numEntries;
    return sz;
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
    if (!(donor->tables && donor->entries))
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
        memcpy(lt->table, donor->table, nitf_LookupTable_size(donor->tables, donor->entries));
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
        }
        NITF_FREE((*lt));
        *lt = NULL;
    }
}

NITFAPI(NITF_BOOL) nitf_LookupTable_init(nitf_LookupTable * lut,
        uint32_t numTables,
        uint32_t numEntries,
        const NITF_DATA * tables,
        nitf_Error * error)
{

    /* Look for existing table of a different size */
    if (lut->tables != numTables || lut->entries != numEntries)
    {
        NITF_FREE(lut->table);
        lut->table = NULL;
    }

    lut->tables = numTables;
    lut->entries = numEntries;

    /* only create the table data if we really should */
    if (numTables > 0 && numEntries > 0)
    {
        if (!lut->table)
        {
            lut->table = (uint8_t *) NITF_MALLOC(nitf_LookupTable_size(numTables, numEntries));
            if (!lut->table)
            {
                nitf_Error_initf(error, NITF_CTXT,
                                 NITF_ERR_MEMORY,
                                 "Error allocating look-up table");
                return NITF_FAILURE;
            }
        }

        /* only copy if one existed */
        if (tables)
        {
            memcpy(lut->table, tables, nitf_LookupTable_size(numTables, numEntries));
        }
    }
    else
    {
        lut->table = NULL;
    }
    return NITF_SUCCESS;
}
