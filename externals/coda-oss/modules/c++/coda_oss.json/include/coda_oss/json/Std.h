/* =========================================================================
 * This file is part of coda-oss.json-c++
 * =========================================================================
 *
 * (C) Copyright 2025 ARKA Group, L.P. All rights reserved
 *
 * types-c++ is free software; you can redistribute it and/or modify
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
#ifndef CODA_OSS_json_std_h_INCLUDED_
#define CODA_OSS_json_std_h_INCLUDED_

#include <nlohmann/json.hpp>
#include <std/optional>
#include <complex>
#include <memory>

// Json definitions for some extra 'std' members.
// https://github.com/nlohmann/json?tab=readme-ov-file#how-do-i-convert-third-party-types
namespace nlohmann {
    template<typename T>
    struct adl_serializer<std::complex<T>> {
        template<typename BasicJsonType>
        static void to_json(BasicJsonType& j, const std::complex<T>& z) {
            j["real"] = z.real();
            j["imag"] = z.imag();
        }
        template<typename BasicJsonType>
        static void from_json(const BasicJsonType& j, std::complex<T>& z) {
            z.real(j["real"].template get<T>());
            z.imag(j["imag"].template get<T>());
        }
    };

    template<typename T>
    struct adl_serializer<std::unique_ptr<T>> {
        template<typename BasicJsonType>
        static void to_json(BasicJsonType& j, const std::unique_ptr<T>& p) {
            if (p) j = *p;
        }
        template<typename BasicJsonType>
        static void from_json(const BasicJsonType& j, std::unique_ptr<T>& p) {
            p.reset(j.is_null() ? nullptr : new T(j.template get<T>()));
        }
    };

    template<typename T>
    struct adl_serializer<std::shared_ptr<T>> {
        template<typename BasicJsonType>
        static void to_json(BasicJsonType& j, const std::shared_ptr<T>& p) {
            if (p) j = *p;
        }
        template<typename BasicJsonType>
        static void from_json(const BasicJsonType& j, std::shared_ptr<T>& p) {
            p.reset(j.is_null() ? nullptr : new T(j.template get<T>()));
        }
    };

    template<typename T>
    struct adl_serializer<std::optional<T>> {
        template<typename BasicJsonType>
        static void to_json(BasicJsonType& j, const std::optional<T>& p) {
            if (p.has_value()) j = p.value();
        }
        template<typename BasicJsonType>
        static void from_json(const BasicJsonType& j, std::optional<T>& p) {
            if (j.is_null())
            {
                p.reset();
                return;
            }
            p.emplace(j.template get<T>());
        }
    };
}
#endif