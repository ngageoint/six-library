/* =========================================================================
 * This file is part of mt-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2019, MDA Information Systems LLC
 *
 * mt-c++ is free software; you can redistribute it and/or modify
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

#ifndef CODA_OSS_mt_Algorithm_h_INCLUDED_
#define CODA_OSS_mt_Algorithm_h_INCLUDED_
#pragma once

#include <algorithm>
#include <iterator>
#include <future>

namespace mt
{
// There was a transform_async() utility here, but I removed it.
//
// First of all, C++11's std::async() is now (in 2023) thought of as maybe a
// bit "half baked," and perhaps shouldn't be emulated.  Then, C++17 added
// parallel algorithms which might be a better ... although we're still at C++14.
}

#endif // CODA_OSS_mt_Algorithm_h_INCLUDED_

