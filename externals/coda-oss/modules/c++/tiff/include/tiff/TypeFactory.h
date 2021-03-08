/* =========================================================================
 * This file is part of tiff-c++ 
 * =========================================================================
 * 
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
 *
 * tiff-c++ is free software; you can redistribute it and/or modify
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
 * License along with this program; If not, 
 * see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef __TIFF_TYPE_FACTORY_H__
#define __TIFF_TYPE_FACTORY_H__

#include "tiff/GenericType.h"

namespace tiff
{

/**
 *********************************************************************
 * @class TypeFactory
 * @brief A factory for creating tiff::GenericTypes
 *
 * Contains a static member function that creates an tiff::GenericType,
 * inserting the provided data as the specified type.  The types 
 * assume a specific size for the data, see tiff::Const for the size
 * of each TIFF type.
 *********************************************************************/
class TypeFactory
{
public:
    //! Default constructor
    TypeFactory()
    {
    }

    //! Deconstructor
    ~TypeFactory()
    {
    }

    /**
     *****************************************************************
     * Creates a tiff::GenericType, inserting the provided data as the
     * specified type.  The types assume a specific size for the data,
     * see tiff::Const for the size of each TIFF type.
     *
     * @param data
     *   the data to insert into the tiff::GenericType
     * @param type
     *   the tiff::GenericType to create, see tiff::Const::Type
     * @return
     *   a pointer to a TypeInterface
     *****************************************************************/
    static tiff::TypeInterface *create(const unsigned char *data,
            const unsigned short type);
};

} // End namespace.

#endif // __TIFF_TYPE_FACTORY_H__
