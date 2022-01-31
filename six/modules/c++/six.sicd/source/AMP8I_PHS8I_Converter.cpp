/* =========================================================================
 * This file is part of six.sicd-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
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
#include "six/sicd/AMP8I_PHS8I_Converter.h"

#include <stdexcept>
#include <array>
#include <std/memory>

#include <gsl/gsl.h>
#include <mt/Algorithm.h>

#include "six/sicd/ComplexToAMP8IPHS8I.h"
#include "six/sicd/ImageData.h"
#include "six/sicd/Utilities.h"
#include "six/sicd/KDTree.h"

using namespace six;
using namespace six::sicd;

AMP8I_PHS8I_Converter::AMP8I_PHS8I_Converter(const AmplitudeTable* pAmplitudeTable)
    : amplitudeTable(pAmplitudeTable)
{
}

static std::vector<ImageData::KDNode> make_KDNodes(const six::AmplitudeTable* pAmplitudeTable)
{
    // For all possible amp/phase values (there are "only" 256*256), get and save the
    // complex<float> value.
    //
    // Be careful with indexing so that we don't wrap-around in the loops.
    std::vector<ImageData::KDNode> retval;
    for (uint16_t input_amplitude = 0; input_amplitude <= UINT8_MAX; input_amplitude++)
    {
        ImageData::KDNode v;
        v.amp_and_value.first = gsl::narrow<uint8_t>(input_amplitude);

        for (uint16_t input_value = 0; input_value <= UINT8_MAX; input_value++)
        {
            v.amp_and_value.second = gsl::narrow<uint8_t>(input_value);
            v.result = Utilities::from_AMP8I_PHS8I(v.amp_and_value.first, v.amp_and_value.second, pAmplitudeTable);
            retval.push_back(v);
        }
    }
    return retval;
}

using input_values_t = std::array<std::complex<float>, UINT8_MAX + 1>;
using input_amplitudes_t = std::array<input_values_t, UINT8_MAX + 1>;

// input_amplitudes_t is too big for the stack
static std::unique_ptr<input_amplitudes_t> AMP8I_PHS8I_to_RE32F_IM32F_(const six::AmplitudeTable* pAmplitudeTable)
{
    auto nodes = make_KDNodes(pAmplitudeTable);

    auto retval = std::make_unique<input_amplitudes_t>();
    auto& values = *retval;
    for (auto&& n : nodes)
    {
        values[n.amp_and_value.first][n.amp_and_value.second] = std::move(n.result);
    }

    return retval;
}

// This is a non-templatized function so that there is copy of the "static" data with a NULL AmplutdeTable.
static const input_amplitudes_t* get_RE32F_IM32F_values(const six::AmplitudeTable* pAmplitudeTable)
{
    if (pAmplitudeTable == nullptr)
    {
        static const auto RE32F_IM32F_values_no_amp = AMP8I_PHS8I_to_RE32F_IM32F_(nullptr);
        return RE32F_IM32F_values_no_amp.get();
    }
    return nullptr;
}

std::complex<float> AMP8I_PHS8I_Converter::from_AMP8I_PHS8I(const AMP8I_PHS8I_t& input) const
{
    auto const pAmplitudeTable = amplitudeTable;
    auto const pValues = get_RE32F_IM32F_values(pAmplitudeTable);

    // Do we have a cahced result to use (no amplitude table)?
    // Or must it be recomputed (have an amplutude table)?
    return pValues != nullptr ? (*pValues)[input.first][input.second] :
        Utilities::from_AMP8I_PHS8I(input.first, input.second, pAmplitudeTable);
}

static const input_amplitudes_t& get_RE32F_IM32F_values(const six::AmplitudeTable* pAmplitudeTable,
    std::unique_ptr<input_amplitudes_t>& pValues_)

{
    const input_amplitudes_t* pValues = get_RE32F_IM32F_values(pAmplitudeTable);
    if (pValues == nullptr)
    {
        assert(pAmplitudeTable != nullptr);
        pValues_ = AMP8I_PHS8I_to_RE32F_IM32F_(pAmplitudeTable);
        pValues = pValues_.get();
    }
    assert(pValues != nullptr);
    return *pValues;
}

void AMP8I_PHS8I_Converter::from_AMP8I_PHS8I(std::span<const AMP8I_PHS8I_t> inputs, std::span<std::complex<float>> results,
    ptrdiff_t cutoff_) const
{
    std::unique_ptr<input_amplitudes_t> pValues_;
    const auto& values = get_RE32F_IM32F_values(amplitudeTable, pValues_);
    const auto get_RE32F_IM32F_value_f = [&values](const ImageData::AMP8I_PHS8I_t& v)
    {
        return values[v.first][v.second];
    };

    const auto begin = inputs.data(); // no iterators with our homebrew span<>
    const auto end = begin + inputs.size();
    const auto out = results.data(); // no iterators with our homebrew span<>

    if (cutoff_ < 0)
    {
        (void) std::transform(begin,end, out, get_RE32F_IM32F_value_f);
    }
    else
    {
        // The value of "default_cutoff" was determined by testing; there is nothing special about it, feel free to change it.
        constexpr auto dimension = 128 * 8;
        constexpr auto default_cutoff = dimension * dimension;
        const auto cutoff = cutoff_ == 0 ? default_cutoff : cutoff_;
        (void) mt::transform_async(begin, end, out, get_RE32F_IM32F_value_f, cutoff, std::launch::async);
    }
}

static const ComplexToAMP8IPHS8I* make_ComplexToAMP8IPHS8I(const six::AmplitudeTable* pAmplitudeTable, std::unique_ptr<ComplexToAMP8IPHS8I>& pTree)
{
    if (pAmplitudeTable == nullptr)
    {
        // this won't change, so OK to cache
        static const ComplexToAMP8IPHS8I tree{};
        return &tree;
    }
    else
    {
        pTree = std::make_unique<ComplexToAMP8IPHS8I>(*pAmplitudeTable);
        return pTree.get();
    }
}

void AMP8I_PHS8I_Converter::to_AMP8I_PHS8I(std::span<const cx_float> inputs, std::span<AMP8I_PHS8I_t> results,
    ptrdiff_t cutoff_) const
{
    // make a structure to quickly find the nearest neighbor
    std::unique_ptr<ComplexToAMP8IPHS8I> pTree; // not-cached, non-NULL amplitudeTable
    const auto& tree = *(make_ComplexToAMP8IPHS8I(amplitudeTable, pTree));
    const auto nearest_neighbor_f = [&](const std::complex<float>& v)
    {
        return tree.nearest_neighbor(v);
    };

    const auto begin = inputs.data(); // no iterators with our homebrew span<>
    const auto end = begin + inputs.size();
    const auto out = results.data(); // no iterators with our homebrew span<>
    if (cutoff_ < 0)
    {
        (void) std::transform(begin, end, out, nearest_neighbor_f);
    }
    else
    {
        // The value of "default_cutoff" was determined by testing; there is nothing special about it, feel free to change it.
        constexpr auto dimension = 128 * 8;
        constexpr auto default_cutoff = dimension * dimension;
        const auto cutoff = cutoff_ == 0 ? default_cutoff : cutoff_;
        (void) mt::transform_async(begin, end, out, nearest_neighbor_f, cutoff, std::launch::async);
    }
}

