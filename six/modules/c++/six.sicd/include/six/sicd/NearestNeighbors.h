/* =========================================================================
 * This file is part of six.sicd-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
 * © Copyright 2024, Maxar Technologies, Inc.
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
#pragma once

#include <stdint.h>
#include <vector>

#include <scene/sys_Conf.h>

#include "six/AmplitudeTable.h"

namespace six
{
namespace sicd
{
/*!
 * \brief A utility that's used to convert complex values into 8-bit amplitude and phase values.
 */
struct NearestNeighbors final
{
    NearestNeighbors(const details::ComplexToAMP8IPHS8I&);
    ~NearestNeighbors() = default;
    NearestNeighbors(const NearestNeighbors&) = delete;
    NearestNeighbors& operator=(const NearestNeighbors&) = delete;
    NearestNeighbors(NearestNeighbors&&) = delete; // implicitly deleted because of =delete for copy
    NearestNeighbors& operator=(NearestNeighbors&&) = delete; // implicitly deleted because of =delete for copy

    /*!
     * Get the nearest amplitude and phase value given a complex value
     * @param v complex value to query with
     * @return nearest amplitude and phase value
     */
    AMP8I_PHS8I_t nearest_neighbor(const six::zfloat& v) const;
    void nearest_neighbors(execution_policy, std::span<const six::zfloat> inputs, std::span<AMP8I_PHS8I_t> results) const;
    void nearest_neighbors(std::span<const six::zfloat> inputs, std::span<AMP8I_PHS8I_t> results) const; // system picks execution_policy

    void nearest_neighbors_seq(std::span<const six::zfloat> inputs, std::span<AMP8I_PHS8I_t> results) const;
    void nearest_neighbors_par(std::span<const six::zfloat> inputs, std::span<AMP8I_PHS8I_t> results) const;

    #if SIX_sicd_ComplexToAMP8IPHS8I_unseq
    #if SIX_sicd_has_sisd
    // This uses the UNSEQ code/logic with the specififed execution policy. The intent
    // is to run the generic SIMD-enabled code with non-SIMD types.
    //
    // See https://mattkretz.github.io/2021/07/29/data-structure-vectorization.html
    // "... if you’re careful about `if` statements, you can instantiate the template with either an arithmetic type or a [simd] type."
    // and https://mattkretz.github.io/2019/07/25/making-the-conditional-operator-overloadable.html
    void unseq_nearest_neighbors(execution_policy, std::span<const six::zfloat> inputs, std::span<AMP8I_PHS8I_t> results) const;
    #endif // SIX_sicd_has_sisd

    void nearest_neighbors_unseq(std::span<const zfloat> inputs, std::span<AMP8I_PHS8I_t> results) const;
    void nearest_neighbors_par_unseq(std::span<const zfloat> inputs, std::span<AMP8I_PHS8I_t> results) const;
    #endif    

private:
    const details::ComplexToAMP8IPHS8I& converter_;

    uint8_t find_nearest(six::zfloat phase_direction, six::zfloat v) const;
    uint8_t getPhase(six::zfloat) const;

    #if SIX_sicd_ComplexToAMP8IPHS8I_unseq
    void nearest_neighbors_(execution_policy, std::span<const zfloat> inputs, std::span<AMP8I_PHS8I_t> results) const;

    template<typename ZFloatV>
    auto unseq_nearest_neighbors(std::span<const zfloat> p) const; // TODO: std::span<T, N> ... ?

    template<typename ZFloatV, int elements_per_iteration>
    void nearest_neighbors_T(execution_policy, std::span<const zfloat> inputs, std::span<AMP8I_PHS8I_t> results) const;
    #endif 

};
}
}


