/* =========================================================================
* This file is part of six.sicd-c++
* =========================================================================
*
* (C) Copyright 2022, Maxar Technologies, Inc.
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
#ifndef SIX_six_sicd_ComplexToAMP8IPHS8I_h_INCLUDED_
#define SIX_six_sicd_ComplexToAMP8IPHS8I_h_INCLUDED_
#pragma once

#include <stdint.h>

#include <six/sicd/ImageData.h>

namespace six
{
namespace sicd
{
/*!
 * \brief A utility that's used to convert complex values into 8-bit amplitude and phase values.
 */
struct ComplexToAMP8IPHS8I final
{
    /*!
     * Create a lookup structure that converts from complex to amplitude and phase.
     * @param pAmplitudeTable optional amplitude table.
     */
    explicit ComplexToAMP8IPHS8I(const six::AmplitudeTable* pAmplitudeTable = nullptr);

    /*!
     * Get the nearest amplitude and phase value given a complex value
     * @param v complex value to query with
     * @return nearest amplitude and phase value
     */
    ImageData::AMP8I_PHS8I_t nearest_neighbor(const std::complex<float>& v) const;

private:
    //! The sorted set of possible magnitudes order from small to large.
    std::array<long double, UINT8_MAX + 1> magnitudes;
    //! The difference in phase angle between two UINT phase values.
    long double phase_delta;
    //! Unit vector rays that represent each direction that phase can point.
    std::array<std::complex<long double>, UINT8_MAX + 1> phase_directions;
};
}
}
#endif // SIX_six_sicd_ComplexToAMP8IPHS8I_h_INCLUDED_
