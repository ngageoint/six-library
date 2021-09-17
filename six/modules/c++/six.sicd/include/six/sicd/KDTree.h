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

#include <memory>

#include "six/sicd/ImageData.h" // can't forward declare nested KDNode

namespace six
{
    namespace sicd
    {
        class KDTree final
        {
            struct Impl;
            std::unique_ptr<Impl> pImpl;
        public:
            using node_t = ImageData::KDNode;

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

#endif // SIX_sicd_KDTree_h_INCLUDED_
