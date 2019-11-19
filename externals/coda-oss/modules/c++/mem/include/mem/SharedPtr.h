/* =========================================================================
 * This file is part of mem-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2018, MDA Information Systems LLC
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

#ifndef __MEM_SHARED_PTR_H__
#define __MEM_SHARED_PTR_H__

#include <config/coda_oss_config.h>

// Adding a forward declaration here in order to have SWIG correctly generate
// bindings for SharedPtr.
//
// SWIG's default behavior is to not process includes recursively when given an
// %include directive (neither SharedPtrCpp11 or SharedPtrLegacy will get
// processed) - including this header without the forward declaration
// as part of a SWIG interface will result in a missing template type error.
namespace mem
{
template<typename T>
class SharedPtr;
}

#include <mem/SharedPtrCpp11.h>

#endif
