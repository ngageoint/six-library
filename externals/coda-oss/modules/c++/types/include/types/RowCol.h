/* =========================================================================
 * This file is part of types-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
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
 *
 */
 
#ifndef __TYPES_ROW_COL_H__
#define __TYPES_ROW_COL_H__

#include <cstddef>
#include <cmath>
#include <cstdlib>
#include <utility>
#include <limits>

namespace types
{

/*!
 *  \class RowCol
 *  \brief Templated RowCol type
 *
 *  This class attempts to provide a simple interface for
 *  accessing row-col pairs that just does what you want.
 *
 *  Unfortunately, std::pair is a little bit too general for
 *  our purposes most of the time, and we often want a way
 *  of describing image distances and scaling with overloaded
 *  operations that just make sense
 *
 */
template<typename T> struct RowCol
{
    T row{static_cast<T>(0.0)};
    T col{static_cast<T>(0.0)};

    // Try to protect us from the unfortunate and probably
    // unintendet case where row gets set and col doesnt, especially
    // when doing scalar operations that might otherwise create
    // ambiguities
    RowCol() = default;

    RowCol(T r, T c) :
        row(r), col(c) {}

    template<typename Other_T> RowCol(const RowCol<Other_T>& p)
    {
        row = static_cast<T>(p.row);
        col = static_cast<T>(p.col);
    }

    RowCol(const std::pair<T, T>& p)
    {
        row = p.first;
        col = p.second;
    }

    template<typename Other_T> RowCol& operator=(const RowCol<Other_T>& p)
    {
        if (this != (RowCol*)&p)
        {
            row = static_cast<T>(p.row);
            col = static_cast<T>(p.col);
        }
        return *this;
    }

    RowCol& operator=(const std::pair<T, T>& p)
    {
        row = p.first;
        col = p.second;
        return *this;
    }

    
    template<typename Other_T> RowCol& operator+=(const RowCol<Other_T>& p)
    {
        row += static_cast<T>(p.row);
        col += static_cast<T>(p.col);
        return *this;
    }

    template<typename Other_T> RowCol operator+(const RowCol<Other_T>& p) const
    {
        RowCol copy(*this);
        return copy += p;
    }

    template<typename Other_T> RowCol& operator*=(const RowCol<Other_T>& p)
    {
        row *= static_cast<T>(p.row);
        col *= static_cast<T>(p.col);
        return *this;
    }

    template<typename Other_T> RowCol operator*(const RowCol<Other_T>& p) const
    {
        RowCol copy(*this);
        return copy *= p;
    }
    
    
    template<typename Other_T> RowCol& operator-=(const RowCol<Other_T>& p)
    {
        row -= static_cast<T>(p.row);
        col -= static_cast<T>(p.col);
        return *this;
    }
    
    template<typename Other_T> RowCol operator-(const RowCol<Other_T>& p) const
    {
        RowCol copy(*this);
        return copy -= p;
    }

    template<typename Other_T> RowCol& operator/=(const RowCol<Other_T>& p)
    {
        row /= static_cast<T>(p.row);
        col /= static_cast<T>(p.col);
        return *this;
    }
    
    template<typename Other_T> RowCol operator/(const RowCol<Other_T>& p) const
    {
        RowCol copy(*this);
        return copy /= p;
    }


    RowCol& operator+=(T scalar)
    {
        row += scalar;
        col += scalar;
        return *this;
    }

    RowCol operator+(T scalar) const
    {
        RowCol copy(*this);
        return copy += scalar;
    }
    
    RowCol& operator-=(T scalar)
    {
        row -= scalar;
        col -= scalar;
        return *this;
    }

    RowCol operator-(T scalar) const
    {
        RowCol copy(*this);
        return copy -= scalar;
    }

    RowCol& operator*=(T scalar)
    {
        row *= scalar;
        col *= scalar;
        return *this;
    }

    RowCol operator*(T scalar) const
    {
        RowCol copy(*this);
        return copy *= scalar;
    }
    
    RowCol& operator/=(T scalar)
    {
        row /= scalar;
        col /= scalar;
        return *this;
    }

    RowCol operator/(T scalar) const
    {
        RowCol copy(*this);
        return copy /= scalar;
    }
    
    /*!
     *  Compare the types considering that some
     *  specializations (e.g., double)
     *  are not exact
     */
    bool operator==(const RowCol<T>& p) const
    {
        return row == p.row && col == p.col;
    }


    bool operator!=(const RowCol<T>& p) const
    {
        return ! (RowCol::operator==(p));
    }
    
    T area() const
    {
        return std::abs(row) * std::abs(col);
    }

    T normL2() const
    {
        //! Should be able to just use sqrt() here but VC++ 2010 compiler
        //  appears to (incorrectly) be calling std::sqrt() in this scenario
        //  and then complaining it can't find an overloading for some types
        //  (like size_t)
        //  So, cast to double and at that point we might as well just call
        //  std::sqrt()
        return static_cast<T>(
            std::sqrt(static_cast<double>(row * row + col * col)));
    }
};

template <>
inline size_t RowCol<size_t>::area() const
{
    return row * col;
}

template <>
inline bool RowCol<float>::operator==(const RowCol<float>& p) const
{
    constexpr auto eps = std::numeric_limits<float>::epsilon();
    return std::abs(row - p.row) < eps &&
           std::abs(col - p.col) < eps;
}
template <>
inline bool RowCol<double>::operator==(const RowCol<double>& p) const
{
    constexpr auto eps = std::numeric_limits<double>::epsilon();
    return std::abs(row - p.row) < eps &&
           std::abs(col - p.col) < eps;
}
}

#endif
