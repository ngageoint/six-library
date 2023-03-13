/* =========================================================================
 * This file is part of NITRO
 * =========================================================================
 *
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
 * (C) Copyright 2021, Maxar Technologies, Inc.
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

#include <stdint.h>
#include <assert.h>

#include <string>
#include <stdexcept>
#include <new> // std::nothrow

#include "nitf/TRE.hpp"
#include "nitf/exports.hpp"

namespace nitf
{
    namespace details
    {
        // A simple wrapper around TRE::setFieldValue() and TRE::getFieldValue().
        // Everything to make the call is in one spot.
        template<typename T>
        struct const_field final
        {
            const TRE& tre_;
            std::string tag_;

            const_field(const TRE& tre, const std::string& tag) : tre_(tre), tag_(tag) {}
            const_field(const const_field&) = default;
            const_field& operator=(const const_field&) = delete;
            const_field(const_field&&) = default;
            const_field& operator=(const_field&&) = delete;

            const T getFieldValue() const // "const" as a hint to clients that this value is really stored elsewhere
            {
                return tre_.getFieldValue<T>(tag_);
            }
        };
        template<typename T>
        struct field final
        {
            const_field<T> field_;
            TRE& tre_; // need non-const
            bool forceUpdate_;

            field(TRE& tre, const std::string& tag, bool forceUpdate = false) : field_(tre, tag), tre_(tre), forceUpdate_(forceUpdate) {}
            field(const field&) = default;
            field& operator=(const field&) = delete;
            field(field&&) = default;
            field& operator=(field&&) = delete;

            void setFieldValue(const T& v)
            {
                tre_.setFieldValue(field_.tag_, v, forceUpdate_);
            }
            const T getFieldValue() const 
            {
                return field_.getFieldValue();
            }
        };
    }

    // Include the size of the TRE field to make the types more unique; maybe check it against what's reported at run-time?
    // This class turns assignment into a call to setFieldValue() and a cast calls getFieldValue()
    template<nitf_FieldType, size_t sz, typename T>
    struct TREField final
    {
        details::field<T> field_;
        using value_type = T;
        static constexpr size_t size = sz;

        TREField(TRE& tre, const std::string& tag, bool forceUpdate = false) : field_(tre, tag, forceUpdate) {}
        TREField(const TREField&) = default;
        TREField& operator=(const TREField&) = delete;
        TREField(TREField&&) = default;
        TREField& operator=(TREField&&) = delete;

        void operator=(const value_type& v)
        {
            field_.setFieldValue(v);
        }

        const value_type value() const // "const" as a hint to clients that this value is really stored elsewhere
        {
            return field_.getFieldValue();
        }
        operator const value_type() const
        {
            return value();
        }
    };

    template<size_t sz = SIZE_MAX>
    using TREField_BCS_A = TREField<NITF_BCS_A, sz, std::string>;

    template<size_t sz, typename T = int64_t>
    using TREField_BCS_N = TREField<NITF_BCS_N, sz, T>;

    template<typename TTREField_BCS>
    class IndexedField final
    {
        TRE& tre_;
        std::string name_;
        // regardless of the actual count field, store it as size_t
        TREField_BCS_N< SIZE_MAX, size_t> countField_;
        template<size_t sz, typename T>
        static decltype(countField_) getCountField(const TREField_BCS_N<sz, T>& countField)
        {
            // Regardless of how the count field is actually defined, we'll store
            // in a fixed format; this avoids more template parameters.
            auto& tre = countField.field_.tre_;
            auto& tag = countField.field_.field_.tag_;
            auto& forceUpdate = countField.field_.forceUpdate_;
            return TREField_BCS_N< SIZE_MAX, size_t>(tre, tag, forceUpdate);
        }

        std::string make_tag(size_t i, std::nothrow_t) const
        {
            return name_ + "[" + std::to_string(i) + "]";
        }
        std::string make_tag(size_t i) const
        {
            auto retval = make_tag(i, std::nothrow); // don't duplicate code in throw(), below
            if (i < size()) // better than calling exists() on a non-existent field?
            {
                if (tre_.exists(retval))
                {
                    return retval;
                }
            }
            throw std::out_of_range("tag '" + retval + "' does not exist.");
        }

        TTREField_BCS make_field(size_t i, std::nothrow_t) const
        {
            return TTREField_BCS(tre_, make_tag(i, std::nothrow));
        }
        TTREField_BCS make_field(size_t i) const
        {
            return TTREField_BCS(tre_, make_tag(i));
        }

        using value_type = typename TTREField_BCS::value_type;

    public:
        // Indexed fields have their size set by another field.
        template<size_t sz, typename T = int64_t>
        IndexedField(TRE& tre, const std::string& name, const TREField_BCS_N<sz, T>& countField)
            : tre_(tre), name_(name), countField_(getCountField(countField))
        {
            (void)size(); // be sure the count field exists
        }
        ~IndexedField() = default;
        IndexedField(const IndexedField&) = delete;
        IndexedField& operator=(const IndexedField&) = delete;
        IndexedField(IndexedField&&) = default;
        IndexedField& operator=(IndexedField&&) = delete;

        TTREField_BCS operator[](size_t i)
        {
            return make_field(i, std::nothrow);
        }
        TTREField_BCS at(size_t i) // c.f. std::vector
        {
            return make_field(i);
        }

        // Directly return the underlying value rather than a TTREField_BCS
        value_type operator[](size_t i) const
        {
            return make_field(i, std::nothrow).value();
        }
        value_type at(size_t i) const // c.f. std::vector
        {
            return make_field(i).value();
        }

        size_t size() const // c.f. std::vector
        {
            return countField_.value(); // don't save this value as it could change after updateFields()
        }
        bool empty() const // c.f. std::vector
        {
            return size() == 0;
        }
    };
}

