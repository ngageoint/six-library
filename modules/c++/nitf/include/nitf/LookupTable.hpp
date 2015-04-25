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

#ifndef __NITF_LOOKUPTABLE_HPP__
#define __NITF_LOOKUPTABLE_HPP__

#include <string>
#include <stddef.h>
#include "nitf/LookupTable.h"
#include "nitf/NITFException.hpp"
#include "nitf/Object.hpp"

/*!
 *  \file LookupTable.hpp
 *  \brief  Contains wrapper implementation for LookupTable
 */

namespace nitf
{
/*!
 *  \class LookupTable
 *  \brief  The C++ wrapper for the nitf_LookupTable
 */
class LookupTable : public nitf::Object<nitf_LookupTable>
{
public:
    //! Create an uninitialized table of the specified dimensions
    LookupTable(size_t numTables, size_t numEntries);

    //! Create a table of the specified dimensions initialized to 'table'
    LookupTable(const unsigned char* table,
                size_t numTables,
                size_t numEntries);

    //! Copy constructor
    LookupTable(const LookupTable & x);

    //! Assignment Operator
    LookupTable & operator=(const LookupTable & x);

    //! Set native object
    LookupTable(nitf_LookupTable * x);

    //! Destructor
    ~LookupTable();

    //! Get the tables
    size_t getTables() const;

    //! Get the entries
    size_t getEntries() const;

    //! Get the table
    unsigned char * getTable() const;

    //! Set the table and dimensions
    void setTable(const unsigned char* table,
                  size_t numTables,
                  size_t numEntries);

private:
    nitf_Error error;

};

}
#endif
