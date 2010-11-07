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

#include "nitf/LookupTable.hpp"

using namespace nitf;

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

int LookupTable::getTables() const
{
    return getNativeOrThrow()->tables;
}

int LookupTable::getEntries() const
{
    return getNativeOrThrow()->entries;
}

unsigned char * LookupTable::getTable() const
{
    return getNativeOrThrow()->table;
}

void LookupTable::setTable(unsigned char *table, int numTables, int numEntries)
{
    if (!nitf_LookupTable_init(getNativeOrThrow(), numTables,
            numEntries, table, &error))
    {
        throw nitf::NITFException(&error);
    }
}
