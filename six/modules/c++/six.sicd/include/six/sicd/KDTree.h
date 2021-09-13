#ifndef SIX_KDTree_h_INCLUDED_
#define SIX_KDTree_h_INCLUDED_
#pragma once

#include <vector>
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

#endif // SIX_KDTree_h_INCLUDED_
