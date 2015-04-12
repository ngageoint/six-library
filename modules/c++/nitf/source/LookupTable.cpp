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

#include "nitf/LookupTable.hpp"

using namespace nitf;

LookupTable::LookupTable(size_t numTables, size_t numEntries)
{
    nitf_LookupTable* const lookupTable =
        nitf_LookupTable_construct(numTables, numEntries, &error);
    if (!lookupTable)
    {
        throw nitf::NITFException(&error);

    }
    setNative(lookupTable);
}

LookupTable::LookupTable(const unsigned char* table,
                         size_t numTables,
                         size_t numEntries)
{
    nitf_LookupTable* const lookupTable =
        nitf_LookupTable_construct(numTables, numEntries, &error);
    if (!lookupTable)
    {
        throw nitf::NITFException(&error);

    }
    setNative(lookupTable);
    setTable(table, numTables, numEntries);
}

LookupTable::LookupTable(const LookupTable & x)
{
    setNative(x.getNative());
}

LookupTable & LookupTable::operator=(const LookupTable & x)
{
    if (&x != this)
        setNative(x.getNative());
    return *this;
}

LookupTable::LookupTable(nitf_LookupTable * x)
{
    setNative(x);
    getNativeOrThrow();
}

LookupTable::~LookupTable() {}

size_t LookupTable::getTables() const
{
    return getNativeOrThrow()->tables;
}

size_t LookupTable::getEntries() const
{
    return getNativeOrThrow()->entries;
}

unsigned char * LookupTable::getTable() const
{
    return getNativeOrThrow()->table;
}

void LookupTable::setTable(const unsigned char *table,
                           size_t numTables,
                           size_t numEntries)
{
    if (!nitf_LookupTable_init(getNativeOrThrow(), numTables,
            numEntries, table, &error))
    {
        throw nitf::NITFException(&error);
    }
}
