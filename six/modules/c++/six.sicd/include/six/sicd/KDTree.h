#ifndef SIX_KDTree_h_INCLUDED_
#define SIX_KDTree_h_INCLUDED_
#pragma once

#include <vector>
#include <memory>

namespace six
{
    namespace sicd
    {
        // currently only implemented for certain types; see KDTree.cpp
        template<typename TNode>
        class KDTree final
        {
            struct Impl;
            std::unique_ptr<Impl> pImpl;
        public:
            KDTree(std::vector<TNode>&&);
            ~KDTree();
            KDTree(const KDTree&) = delete;
            KDTree& operator=(const KDTree&) = delete;
            KDTree(KDTree&&) = default;
            KDTree& operator=(KDTree&&) = default;

            // https://en.wikipedia.org/wiki/K-d_tree
            void nearest_neighbor(const TNode& point, TNode& result) const;
        };
    }
}

#endif // SIX_KDTree_h_INCLUDED_
