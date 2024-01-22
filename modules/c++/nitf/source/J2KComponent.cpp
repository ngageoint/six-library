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

#include "nitf/J2KComponent.hpp"

j2k::details::Component::Component(j2k_Component* x)
{
    setNative(x);
    getNativeOrThrow();
}
j2k::Component::Component(j2k_Component*&& x) : impl_(x) {}

// a macro to help avoid copy/paste errors
#define NITF_j2k__Component_IMPL(retval, name) retval j2k::Component::name() const { return impl_.callNative<retval>(j2k_Component_ ## name); }

NITF_j2k__Component_IMPL(bool, isSigned);

NITF_j2k__Component_IMPL(uint32_t, getPrecision);

NITF_j2k__Component_IMPL(uint32_t, getWidth);
NITF_j2k__Component_IMPL(uint32_t, getHeight);

NITF_j2k__Component_IMPL(int32_t, getOffsetX);
NITF_j2k__Component_IMPL(int32_t, getOffsetY);

NITF_j2k__Component_IMPL(uint32_t, getSeparationX);
NITF_j2k__Component_IMPL(uint32_t, getSeparationY);

#undef NITF_j2k__Component_IMPL
