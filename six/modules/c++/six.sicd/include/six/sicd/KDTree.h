#ifndef Kd_tree_h_INCLUDED_
#define Kd_tree_h_INCLUDED_
#pragma once

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

#include <vector>
#include <memory>

namespace Kdtree
{
    // for passing points to the constructor of kdtree
    struct Node final
    {
        using value_type = double;
    private:
        std::vector<value_type> value_;
    public:
        Node() = default;
        Node(value_type x, value_type y) : value_({ x, y }) {}
        //Node(value_type x, value_type y, value_type z) : value_({ x, y, z }) {}

        value_type& index(size_t i) { return value_[i]; }
        const value_type& index(size_t i) const { return value_[i]; }
        size_t size() const { return value_.size(); }
    };

    template<typename TNode>
    class Tree final
    {
        struct Impl;
        std::unique_ptr<Impl> pImpl;
    public:
        Tree(std::vector<TNode>&&);
        ~Tree();
        Tree(const Tree&) = delete;
        Tree& operator=(const Tree&) = delete;
        Tree(Tree&&) = default;
        Tree& operator=(Tree&&) = default;

        // https://en.wikipedia.org/wiki/K-d_tree
        void nearest_neighbor(const TNode& point, TNode& result);
    };

}  // end namespace Kdtree

#endif // Kd_tree_h_INCLUDED_
