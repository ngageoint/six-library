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
#ifndef SIX_six_sicd_AMP8I_PHS8I_Converter_h_INCLUDED_
#define SIX_six_sicd_AMP8I_PHS8I_Converter_h_INCLUDED_
#pragma once

#include <complex>
#include <std/span>
#include <utility>

namespace six
{
struct AmplitudeTable; // forward

namespace sicd
{
class AMP8I_PHS8I_Converter final
{
    const AmplitudeTable* amplitudeTable = nullptr;
public:
    explicit AMP8I_PHS8I_Converter(const AmplitudeTable*);

    using cx_float = std::complex<float>;
    using AMP8I_PHS8I_t = std::pair<uint8_t, uint8_t>; // .first = AMP, .second = PHS

    // It would be nice to cache the results, but amplitudeTable could change at any time.
    cx_float from_AMP8I_PHS8I(const AMP8I_PHS8I_t&) const; // for unit-tests

    void from_AMP8I_PHS8I(std::span<const AMP8I_PHS8I_t>, std::span<cx_float>, ptrdiff_t cutoff = -1) const;
    void to_AMP8I_PHS8I(std::span<const cx_float>, std::span<AMP8I_PHS8I_t>, ptrdiff_t cutoff = -1) const;
};
}
}

#endif // SIX_six_sicd_AMP8I_PHS8I_Converter_h_INCLUDED_
