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

#ifndef NITF_J2KComponent_hpp_INCLUDED_
#define NITF_J2KComponent_hpp_INCLUDED_
#pragma once

#include "nitf/coda-oss.hpp"

#include "j2k/Component.h"

#include "nitf/Object.hpp"
#include "nitf/System.hpp"
#include "nitf/NITFException.hpp"

namespace j2k
{
namespace details
{
/*!
 *  \class j2k::Component
 *  \brief  The C++ wrapper for the j2k_Component
 */
 NITRO_DECLARE_CLASS_J2K(Component)
{
     Component() = default;

public:
    // TODO: put in Object.hpp?
    template<typename TReturn, typename Func, typename... Args>
    TReturn callNative(Func f, Args&&... args) const // c.f. getNativeOrThrow()
    {
        return nitf::callNative<TReturn>(f, getNativeOrThrow(), std::forward<Args>(args)...);
    }

    Component(const Component&) = delete;
    Component& operator=(const Component&) = delete;
    Component(Component&&) = default;
    Component& operator=(Component&&) = default;
    ~Component() = default;

    //! Set native object
    Component(j2k_Component*);
};
}
struct Component final
{
    Component() = delete;
    Component(const Component&) = delete;
    Component& operator=(const Component&) = delete;
    Component(Component&&) = default;
    Component& operator=(Component&&) = default;
    ~Component() = default;

    //! Set native object
    Component(j2k_Component*&&);

    uint32_t getWidth() const;
    uint32_t getHeight() const;

    int32_t getOffsetX() const;
    int32_t getOffsetY() const;

    uint32_t getSeparationX() const;
    uint32_t getSeparationY() const;

    uint32_t getPrecision() const;
    bool isSigned() const;

private:
    details::Component impl_;
};
}
#endif // NITF_J2KComponent_hpp_INCLUDED_

