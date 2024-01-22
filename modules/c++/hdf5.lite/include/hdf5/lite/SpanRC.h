/* =========================================================================
 * This file is part of hdf5.lite-c++ 
 * =========================================================================
 * 
 * (C) Copyright 2022, Maxar Technologies, Inc.
 *
 * hdf5.lite-c++ is free software; you can redistribute it and/or modify
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

#pragma once 
#ifndef CODA_OSS_hdf5_lite_SpanRC_h_INCLUDED_
#define CODA_OSS_hdf5_lite_SpanRC_h_INCLUDED_

#include "coda_oss/mdspan.h"

namespace hdf5
{
namespace lite
{

template<typename T>
using SpanRC = coda_oss::mdspan<T, coda_oss::dextents<size_t, 2>>;

}
}

#endif // CODA_OSS_hdf5_lite_SpanRC_h_INCLUDED_
