/* =========================================================================
 * This file is part of coda-oss.json-c++
 * =========================================================================
 *
 * (C) Copyright 2025 ARKA Group, L.P. All rights reserved
 *
 * types-c++ is free software; you can redistribute it and/or modify
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
 */

#pragma once
#ifndef CODA_OSS_json_math_h_INCLUDED_
#define CODA_OSS_json_math_h_INCLUDED_

#include <import/math/linear.h>
#include <import/math/poly.h>

namespace math
{
    namespace linear
    {
        template<typename BasicJsonType, size_t N, typename T>
        void to_json(BasicJsonType& j, const VectorN<N, T>& v)
        {
            std::array<T, N> arr;
            for (size_t idx = 0; idx < N; idx++)
            {
                arr[idx] = v.matrix()[idx][0];
            } 
            j = arr;
        }
        template<typename BasicJsonType, size_t N, typename T>
        void from_json(const BasicJsonType& j, VectorN<N, T>& v)
        { 
            v = j.template get<std::vector<T>>();
        }

        template<typename BasicJsonType, size_t M, size_t N, typename T>
        void to_json(BasicJsonType& j, const MatrixMxN<M, N, T>& v)
        {   
            std::array<std::array<T, N>, M> arr;
            for (size_t iRow = 0; iRow < M; iRow++)
            {
                for (size_t iCol = 0; iCol < N; iCol++)
                {
                    arr[iRow][iCol] = v.mRaw[iRow][iCol];
                }
            }
            j = arr;
        }
        template<typename BasicJsonType, size_t M, size_t N, typename T>
        void from_json(const BasicJsonType& j, MatrixMxN<M, N, T>& v)
        { 
            auto temp = j.template get< std::array< std::array<T, N>, M> > ();
            T arr[M * N];
            for (size_t iRow = 0; iRow < M; iRow++)
            {
                for (size_t iCol = 0; iCol < N; iCol++)
                {
                    arr[iRow * N + iCol] = temp[iRow][iCol];
                }
            }
            v = arr;
        }

    } // namespace linear

    namespace poly
    {
        template<typename BasicJsonType, typename T>
        void to_json(BasicJsonType& j, const OneD<T>& poly)
        {
            if (poly.empty())
            {
                return;
            }
            j = poly.coeffs();
        }
        template<typename BasicJsonType, typename T>
        void from_json(const BasicJsonType& j, OneD<T>& poly)
        {
            if (j.is_null())
            {
                poly = OneD<T>();
                return;
            }
            poly = OneD<T>(j.template get<std::vector<T>>());
        }

        template<typename BasicJsonType, typename T>
        void to_json(BasicJsonType& j, const TwoD<T>& poly)
        {
            if (poly.empty())
            {
                return;
            }
            std::vector<std::vector<T>> coeffs(poly.orderX() + 1);
            for (size_t ix = 0; ix <= poly.orderX(); ix++)
            {
                coeffs[ix] = poly[ix].coeffs();
            }
            j = coeffs;
        }
        template<typename BasicJsonType, typename T>
        void from_json(const BasicJsonType& j, TwoD<T>& poly)
        {
            if (j.is_null())
            {
                poly = TwoD<T>();
                return;
            }
            poly = TwoD<T>(j.template get<std::vector<OneD<T>>>());
        }

        template<typename BasicJsonType, size_t O, typename T>
        void to_json(BasicJsonType& j, const Fixed1D<O, T>& poly)
        {
            j = poly.coeffs();
        }
        template<typename BasicJsonType, size_t O, typename T>
        void from_json(const BasicJsonType& j, Fixed1D<O, T>& poly)
        {
            poly.coeffs() = j.template get<std::array<T, O + 1>>();
        }

        template<typename BasicJsonType, size_t OX, size_t OY, typename T>
        void to_json(BasicJsonType& j, const Fixed2D<OX, OY, T>& poly)
        {
            std::array<std::array<T, OY + 1>, OX + 1> coeffs;
            for (size_t ix = 0; ix <= poly.orderX(); ix++)
            {
                coeffs[ix] = poly[ix].coeffs();
            }
            j = coeffs;
        }
        template<typename BasicJsonType, size_t OX, size_t OY, typename T>
        void from_json(const BasicJsonType& j, Fixed2D<OX, OY, T>& poly)
        {
            poly = TwoD<T>(j.template get<std::vector<OneD<T>>>());
        }
    } // namespace poly
} // namespace math

#endif