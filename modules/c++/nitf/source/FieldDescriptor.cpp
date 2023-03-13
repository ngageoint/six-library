/* =========================================================================
 * This file is part of NITRO
 * =========================================================================
 *
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
 * (C) Copyright 2022, Maxar Technologies, Inc.
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

#include <type_traits>

#include "nitf/FieldDescriptor.hpp"

std::vector<nitf::FieldDescriptor> nitf::getFieldDescriptors(std::span<const nitf_StructFieldDescriptor> descriptors)
{
    std::vector<nitf::FieldDescriptor> retval;
    for (size_t i = 0; i < descriptors.size(); i++) // no iterators for our home-brew span<>
    {
        retval.emplace_back(descriptors[i]);
    }
    return retval;
}
