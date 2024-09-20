/* =========================================================================
 * This file is part of NITRO
 * =========================================================================
 *
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
 * (C) Copyright 2022 , Maxar Technologies, Inc.
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

#pragma once
#ifndef NITRO_nitf_FieldDescriptor_hpp_INCLUDED_
#define NITRO_nitf_FieldDescriptor_hpp_INCLUDED_

#include <string>
#include <vector>
#include <std/span>
#include <stdexcept>

#include <sys/Span.h>

#include "nitf/FieldDescriptor.h"
#include "nitf/Field.hpp"
#include "nitf/Object.hpp"

namespace nitf
{
    // wrap nitf_StructFieldDescriptor
    class FieldDescriptor final
    {
        const nitf_StructFieldDescriptor* pNative = nullptr;
    public:
        enum class Type
        {
            Field = NITF_FieldType_Field, // nitf_Field*
            FileSecurity = NITF_FieldType_FileSecurity, // nitf_FileSecurity*
            ComponentInfo = NITF_FieldType_ComponentInfo, // nitf_ComponentInfo*
            PComponentInfo = NITF_FieldType_PComponentInfo, // nitf_ComponentInfo** aka nitf_PComponentInfo*
            Extensions = NITF_FieldType_Extensions, // nitf_Extensions*
        };
        FieldDescriptor(const nitf_StructFieldDescriptor& native) noexcept : pNative(&native) {}
        ~FieldDescriptor() = default;
        FieldDescriptor(const FieldDescriptor&) = default;
        FieldDescriptor& operator=(const FieldDescriptor&) = default;
        FieldDescriptor(FieldDescriptor&&) = default;
        FieldDescriptor& operator=(FieldDescriptor&&) = default;

        Type type() const noexcept
        {
            return static_cast<Type>(pNative->type);
        }
        std::string name() const
        {
            return pNative->name;
        }
        size_t offset() const noexcept
        {
            return pNative->offset;
        }

        template<typename TObject>
        Field getField(const TObject& object) const
        {
            if (type() != Type::Field)
            {
                throw std::logic_error("Descriptor '" + name() + "' is not a 'Field' type.");
            }
            return fromNativeOffset<Field>(object, offset());
        }
    };

    extern std::vector<FieldDescriptor> getFieldDescriptors(std::span<const nitf_StructFieldDescriptor>);
    template<size_t N>
    inline std::vector<FieldDescriptor> getFieldDescriptors(const nitf_StructFieldDescriptor(&descriptors)[N])
    {
        const auto s = sys::make_span(descriptors, N);
        return getFieldDescriptors(s);
    }
}
#endif // NITRO_nitf_FieldDescriptor_hpp_INCLUDED_
