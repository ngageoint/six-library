/* =========================================================================
 * This file is part of mem-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
 *
 * mem-c++ is free software; you can redistribute it and/or modify
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

#ifndef __MEM_SCOPED_ARRAY_H__
#define __MEM_SCOPED_ARRAY_H__

#include <cstddef>
#include <memory>

#include "mem/SharedPtr.h" // mem::make::unique

namespace mem
{
    /*!
     *  \class ScopedArray
     *  \brief This class provides RAII for array allocations via new[].
     *         It is based on boost::scoped_array.
     */
    template <class T>
    using  ScopedArray = std::unique_ptr<T[]>;
}

#endif
