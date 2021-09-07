///////////////////////////////////////////////
// https://github.com/cdalitz/kdtree-cpp
///////////////////////////////////////////////
// Kd-Tree implementation.
//
// Copyright: Christoph Dalitz, 2018
//            Jens Wilberg, 2018
// License:   BSD style license
//            (see the file LICENSE for details)

/* LICENSE

﻿Copyright:
  * 2018 Christoph Dalitz and Jens Wilberg
    Niederrhein University of Applied Sciences,
    Institute for Pattern Recognition,
    Reinarzstr. 49, 47805 Krefeld, Germany
    <http://www.hsnr.de/ipattern/>

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "six/sicd/KDTree.h"

#include <math.h>
#include <assert.h>

#include <algorithm>
#include <limits>
#include <stdexcept>
#include <cstdlib>
#include <queue>
#include <complex>
#include <std/memory>

#if _MSC_VER
#pragma warning(disable: 5045) // Compiler will insert Spectre mitigation for memory load if /Qspectre switch specified
#pragma warning(disable: 4820) // '...': '...' bytes padding added after data member '...'
#endif

namespace KDTree
{
    using node_t = six::sicd::KDTree::node_t;
    inline node_t::value_type& index(node_t& p, size_t i)
    {
        return p.index(i);
    }
    inline const node_t::value_type& index(const node_t& p, size_t i)
    {
        return p.index(i);
    }
    inline size_t size(const node_t& p)
    {
        return p.size();
    }

    // Euklidean distance (L2 norm)
    inline node_t::value_type coordinate_distance(const node_t& p, const node_t& q, size_t i)
    {
        return node_t::coordinate_distance(p, q, i);
    }
    inline node_t::value_type distance(const node_t& p, const node_t& q)
    {
        return node_t::distance(p, q);
    }
}

namespace KDTree
{
    //--------------------------------------------------------------
    // internal node structure used by kdtree
    //--------------------------------------------------------------
    template<typename TNode>
    struct Ttree_node final
    {
        Ttree_node() = delete;
        Ttree_node(const Ttree_node&) = delete;
        Ttree_node& operator=(const Ttree_node&) = delete;
        Ttree_node(Ttree_node&&) = default;
        Ttree_node& operator=(Ttree_node&&) = default;
        Ttree_node(const TNode& lo, const TNode& up, size_t cd) : cutdim(cd), lobound(lo), upbound(up) {}
        ~Ttree_node() = default;

        // index of node data in kdtree array "allnodes"
        size_t dataindex = 0;
        // cutting dimension
        size_t cutdim = 0;
        // value of point
        // double cutval; // == point[cutdim]
        TNode point;
        //  roots of the two subtrees
        std::unique_ptr<Ttree_node> loson;
        std::unique_ptr<Ttree_node> hison;
        // bounding rectangle of this node's subtree
        TNode lobound, upbound;

        void set_point(const std::vector<TNode>& allnodes, size_t i)
        {
            dataindex = i;
            point = allnodes[dataindex];
        }
    };

    //--------------------------------------------------------------
    // recursive build of tree
    // "a" and "b"-1 are the lower and upper indices
    // from "allnodes" from which the subtree is to be built
    //--------------------------------------------------------------
    template<typename TNode>
    static std::unique_ptr<Ttree_node<TNode>> build_tree_(size_t depth, size_t a, size_t b,
        std::vector<TNode>& allnodes, size_t dimension, TNode& lobound, TNode& upbound)
    {
        const auto cutdim = depth % dimension;
        auto pNode = std::make_unique<Ttree_node<TNode>>(lobound, upbound, cutdim);
        if (b - a <= 1)
        {
            pNode->set_point(allnodes, a);
            return pNode;
        }

        const auto m = (a + b) / 2;
        //--------------------------------------------------------------
        // function object for comparing only dimension d of two vecotrs
        //--------------------------------------------------------------
        const auto compare_dimension = [&](const TNode& p, const TNode& q) {
            return (index(p, cutdim) < index(q, cutdim));
        };
        const auto a_ = allnodes.begin() + static_cast<int64_t>(a);
        const auto m_ = allnodes.begin() + static_cast<int64_t>(m);
        const auto b_ = allnodes.begin() + static_cast<int64_t>(b);
        std::nth_element(a_, m_, b_, compare_dimension);
        pNode->set_point(allnodes, m);
        const auto cutval = index(allnodes[m], cutdim);
        if (m - a > 0)
        {
            const auto temp = index(upbound, cutdim);
            index(upbound, cutdim) = cutval;
            pNode->loson = build_tree_(depth + 1, a, m, allnodes, dimension, lobound, upbound);
            index(upbound, cutdim) = temp;
        }
        if (b - m > 1) {
            const auto temp = index(lobound, cutdim);
            index(lobound, cutdim) = cutval;
            pNode->hison = build_tree_(depth + 1, m + 1, b, allnodes, dimension, lobound, upbound);
            index(lobound, cutdim) = temp;
        }
        return pNode;
    }
    template<typename TNode>
    static std::unique_ptr<Ttree_node<TNode>> build_tree(std::vector<TNode>& allnodes, size_t dimension, TNode& lobound, TNode& upbound)
    {
        constexpr size_t depth = 0;
        constexpr size_t a = 0;
        const auto b = allnodes.size();
        return build_tree_(depth, a, b, allnodes, dimension, lobound, upbound);
    }

    // helper class for priority queue in k nearest neighbor search
    template<typename T>
    struct nn4heap final
    {
        size_t dataindex;  // index of actual kdnode in *allnodes*
        T distance;   // distance of this neighbor from *point*

        struct compare final
        {
            bool operator()(const nn4heap& n, const nn4heap& m) const {
                return (n.distance < m.distance);
            }
        };
    };

    template<typename TNode>
    class Tree final
    {
        using node_t = TNode;
        using value_type = typename TNode::value_type;
        using tree_node = Ttree_node<node_t>;

        // helper variable for keeping track of subtree bounding box
        node_t lobound, upbound;
        // helper variables and functions for k nearest neighbor search
        using nn4heap_t = nn4heap<value_type>;
        using priority_queue = std::priority_queue<nn4heap_t, std::vector<nn4heap_t>, typename nn4heap_t::compare>;
        std::vector<size_t> range_result;

        //--------------------------------------------------------------
        // recursive function for nearest neighbor search in subtree
        // under *node*. Updates the heap (class member) *neighborheap*.
        // returns "true" when no nearer neighbor elsewhere possible
        //--------------------------------------------------------------
        bool neighbor_search(const node_t& point, const tree_node& node, size_t k, priority_queue& neighborheap) const
        {
            const auto curdist = distance(point, node.point);
            if (neighborheap.size() < k) {
                neighborheap.push(nn4heap_t{ node.dataindex, curdist });
            }
            else if (curdist < neighborheap.top().distance) {
                neighborheap.pop();
                neighborheap.push(nn4heap_t{ node.dataindex, curdist });
            }

            // first search on side closer to point
            if (index(point, node.cutdim) < index(node.point, node.cutdim)) {
                if (node.loson)
                    if (neighbor_search(point, *(node.loson), k, neighborheap)) return true;
            }
            else {
                if (node.hison)
                    if (neighbor_search(point, *(node.hison), k, neighborheap)) return true;
            }
            // second search on farther side, if necessary
            value_type dist{ 0 };
            if (neighborheap.size() < k) {
                dist = std::numeric_limits<value_type>::max();
            }
            else {
                dist = neighborheap.top().distance;
            }
            if (index(point, node.cutdim) < index(node.point, node.cutdim)) {
                if (node.hison && bounds_overlap_ball(point, dist, *(node.hison)))
                    if (neighbor_search(point, *(node.hison), k, neighborheap)) return true;
            }
            else {
                if (node.loson && bounds_overlap_ball(point, dist, *(node.loson)))
                    if (neighbor_search(point, *(node.loson), k, neighborheap)) return true;
            }

            if (neighborheap.size() == k)
            {
                dist = neighborheap.top().distance;
            }
            return ball_within_bounds(point, dist, node);
        }

        static value_type coordinate_distance(const node_t& p, const node_t& q, size_t i)
        {
            return KDTree::coordinate_distance(p, q, i);
        }

        // returns true when the bounds of *node* overlap with the
        // ball with radius *dist* around *point*
        bool bounds_overlap_ball(const node_t& point, value_type dist, const tree_node& node) const
        {
            value_type distsum = 0.0;
            for (size_t i = 0; i < dimension; i++)
            {
                if (index(point, i) < index(node.lobound, i)) {  // lower than low boundary
                    distsum += coordinate_distance(point, node.lobound, i);
                    if (distsum > dist) return false;
                }
                else if (index(point, i) > index(node.upbound, i)) {  // higher than high boundary
                    distsum += coordinate_distance(point, node.upbound, i);
                    if (distsum > dist) return false;
                }
            }
            return true;
        }

        // returns true when the bounds of *node* completely contain the
        // ball with radius *dist* around *point*
        bool ball_within_bounds(const node_t& point, value_type dist, const tree_node& node) const
        {
            for (size_t i = 0; i < dimension; i++)
            {
                if (coordinate_distance(point, node.lobound, i) <= dist ||
                    coordinate_distance(point, node.upbound, i) <= dist)
                    return false;
            }
            return true;
        }

        const size_t dimension;
        std::vector<node_t> allnodes;
        std::unique_ptr<const tree_node> root;

    public:
        //--------------------------------------------------------------
       // destructor and constructor of kdtree
       //--------------------------------------------------------------
        ~Tree() = default;
        Tree(const Tree&) = delete;
        Tree& operator=(const Tree&) = delete;
        Tree(Tree&&) = delete;
        Tree& operator=(Tree&&) = delete;
        Tree(std::vector<node_t>&& nodes)
            : dimension(size(nodes[0])), allnodes(std::move(nodes))
        {
            // compute global bounding box
            lobound = allnodes[0];
            upbound = allnodes[0];
            for (size_t i = 1; i < allnodes.size(); i++)
            {
                for (size_t j = 0; j < dimension; j++)
                {
                    const auto val = index(allnodes[i], j);
                    if (index(lobound, j) > val) index(lobound, j) = val;
                    if (index(upbound, j) < val) index(upbound, j) = val;
                }
            }

            // build tree recursively
            root = KDTree::build_tree(allnodes, dimension, lobound, upbound);
        }

        //--------------------------------------------------------------
        // k nearest neighbor search
        // returns the *k* nearest neighbors of *point* in O(log(n))
        // time. The result is returned in *result* and is sorted by
        // distance from *point*.
        // The optional search predicate is a callable class (aka "functor")
        // derived from KdNodePredicate. When Null (default, no search
        // predicate is applied).
        //--------------------------------------------------------------
        void k_nearest_neighbors_(const node_t& point, size_t k, std::vector<node_t>& result,
            priority_queue& neighborheap) const
        {
            result.clear();
            if (k < 1) return;
            if (size(point) != dimension)
            {
                throw std::invalid_argument(
                    "kdtree::k_nearest_neighbors(): point must be of same dimension as kdtree");
            }

            // collect result of k values in neighborheap
            if (k > allnodes.size())
            {
                // when more neighbors asked than nodes in tree, return everything
                k = allnodes.size();
                for (size_t i = 0; i < k; i++) {
                    neighborheap.push(nn4heap_t{ i, distance(allnodes[i], point) });
                }
            }
            else
            {
                neighbor_search(point, *root, k, neighborheap);
            }

            // copy over result sorted by distance
            // (we must revert the vector for ascending order)
            while (!neighborheap.empty())
            {
                const auto i = neighborheap.top().dataindex;
                neighborheap.pop();
                result.push_back(allnodes[i]);
            }
            // beware that less than k results might have been returned
            k = result.size();
            for (size_t i = 0; i < k / 2; i++)
            {
                node_t temp = result[i];
                result[i] = result[k - 1 - i];
                result[k - 1 - i] = temp;
            }
        }
        void k_nearest_neighbors(const node_t& point, size_t k, std::vector<node_t>& result) const
        {
            priority_queue neighborheap;
            k_nearest_neighbors_(point, k, result, neighborheap);
        }
    };
}  // namespace Kdtree


namespace six
{
    namespace sicd
    {
        struct KDTree::Impl final
        {
            ::KDTree::Tree<node_t> tree;
            Impl(std::vector<node_t>&& nodes) : tree(std::move(nodes)) { }
            ~Impl() = default;
            Impl(const Impl&) = delete;
            Impl& operator=(const Impl&) = delete;
            Impl(Impl&&) = delete;
            Impl& operator=(Impl&&) = delete;
        };

        KDTree::KDTree(std::vector<node_t>&& nodes)
            : pImpl(std::make_unique<Impl>(std::move(nodes))) { }
        KDTree::~KDTree() = default;
        KDTree::KDTree(KDTree&&) = default;
        KDTree& KDTree::operator=(KDTree&&) = default;
        
        KDTree::node_t KDTree::nearest_neighbor(const node_t& point) const
        {
            std::vector<node_t> result;
            pImpl->tree.k_nearest_neighbors(point, 1, result);
            assert(result.size() == 1);
            return result[0];
        }
    }
}
