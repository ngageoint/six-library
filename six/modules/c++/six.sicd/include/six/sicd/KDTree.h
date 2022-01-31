/* =========================================================================
* This file is part of six.sicd-c++
* =========================================================================
*
* (C) Copyright 2021, Maxar Technologies, Inc.
*
* six.sicd-c++ is free software; you can redistribute it and/or modify
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

#ifndef SIX_sicd_KDTree_h_INCLUDED_
#define SIX_sicd_KDTree_h_INCLUDED_
#pragma once

#include <assert.h>
#include <stdint.h>

#include <memory>
#include <complex>
#include <vector>
#include <memory>
#include <utility>

#include "six/sicd/ImageData.h"

namespace six
{
    namespace sicd
    {
        namespace details
        {
            struct KDNode final
            {
                using value_type = typename cx_float::value_type;
                cx_float result;
                AMP8I_PHS8I_t amp_and_value;

                value_type& index(size_t i) noexcept
                {
                    assert(i <= 1);
                    // https://en.cppreference.com/w/cpp/numeric/complex
                    return reinterpret_cast<value_type(&)[2]>(result)[i];
                }
                const value_type& index(size_t i) const noexcept
                {
                    assert(i <= 1);
                    // https://en.cppreference.com/w/cpp/numeric/complex
                    return reinterpret_cast<const value_type(&)[2]>(result)[i];
                }

                constexpr size_t size() const noexcept
                {
                    return 2;
                }

                // Euklidean distance (L2 norm)
                static KDNode::value_type coordinate_distance(const KDNode& p, const KDNode& q, size_t i) noexcept
                {
                    const auto x = p.index(i);
                    const auto y = q.index(i);
                    return (x - y) * (x - y);
                }
                static KDNode::value_type distance(const KDNode& p, const KDNode& q) noexcept
                {
                    assert(p.size() == q.size());
                    assert(p.size() == 2);
                    return coordinate_distance(p, q, 0) + coordinate_distance(p, q, 1);
                }
            };

            class KDTree final
            {
                struct Impl;
                std::unique_ptr<Impl> pImpl;
            public:
                using node_t = KDNode;

                KDTree(std::vector<node_t>&&);
                ~KDTree();
                KDTree() = delete;
                KDTree(const KDTree&) = delete;
                KDTree& operator=(const KDTree&) = delete;
                KDTree(KDTree&&) noexcept;
                KDTree& operator=(KDTree&&) noexcept;

                // https://en.wikipedia.org/wiki/K-d_tree
                node_t nearest_neighbor(const node_t& point) const;
            };
        }
    }
}

#endif // SIX_sicd_KDTree_h_INCLUDED_
