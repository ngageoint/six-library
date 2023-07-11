/* =========================================================================
 * This file is part of types-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2019, MDA Information Systems LLC
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

#ifndef __TYPES_PAGE_ROW_COL_H__
#define __TYPES_PAGE_ROW_COL_H__

#include <cstddef>
#include <cmath>
#include <cstdlib>
#include <limits>

#include <types/RowCol.h>

namespace types
{
/*!
 *  \class PageRowCol
 *  \brief Templated PageRowCol type
 *
 *  This class attempts to provide a simple interface for
 *  accessing page-row-col triples that just does what you want.
 *
 */
template<typename T>
struct PageRowCol
{
    T page;
    T row;
    T col;

    // Try to protect us from the unfortunate and probably unintended case
    // where one or more fields is set and the remaining ones are not,
    // especially when doing scalar operations that might otherwise
    // create ambiguities
    PageRowCol() :
        page(static_cast<T>(0.0)), row(static_cast<T>(0.0)), col(static_cast<T>(0.0)) {}


    PageRowCol(T p, T r, T c) :
        page(p), row(r), col(c) {}

    template<typename Other_T>
    explicit PageRowCol(const PageRowCol<Other_T>& p)
    {
        page = static_cast<T>(p.page);
        row = static_cast<T>(p.row);
        col = static_cast<T>(p.col);
    }

    template<typename Other_T1, typename Other_T2>
    PageRowCol(Other_T1 p, const RowCol<Other_T2>& rc)
    {
        page = static_cast<T>(p);
        row = static_cast<T>(rc.row);
        col = static_cast<T>(rc.col);
    }

    template<typename Other_T>
    PageRowCol& operator=(const PageRowCol<Other_T>& p)
    {
        if (this != (PageRowCol*)&p)
        {
	    page = static_cast<T>(p.page);
	    row = static_cast<T>(p.row);
	    col = static_cast<T>(p.col);
        }
        return *this;
    }

    template<typename Other_T>
    PageRowCol& operator+=(const PageRowCol<Other_T>& p)
    {
        page += static_cast<T>(p.page);
        row += static_cast<T>(p.row);
        col += static_cast<T>(p.col);
        return *this;
    }

    template<typename Other_T>
    PageRowCol operator+(const PageRowCol<Other_T>& p) const
    {
        PageRowCol copy(*this);
        return copy += p;
    }

    template<typename Other_T>
    PageRowCol& operator*=(const PageRowCol<Other_T>& p)
    {
        page *= static_cast<T>(p.page);
        row *= static_cast<T>(p.row);
        col *= static_cast<T>(p.col);
        return *this;
    }

    template<typename Other_T>
    PageRowCol operator*(const PageRowCol<Other_T>& p) const
    {
        PageRowCol copy(*this);
        return copy *= p;
    }

    template<typename Other_T>
    PageRowCol& operator-=(const PageRowCol<Other_T>& p)
    {
        page -= static_cast<T>(p.page);
        row -= static_cast<T>(p.row);
        col -= static_cast<T>(p.col);
        return *this;
    }

    template<typename Other_T>
    PageRowCol operator-(const PageRowCol<Other_T>& p) const
    {
        PageRowCol copy(*this);
        return copy -= p;
    }

    template<typename Other_T>
    PageRowCol& operator/=(const PageRowCol<Other_T>& p)
    {
        page /= static_cast<T>(p.page);
        row /= static_cast<T>(p.row);
        col /= static_cast<T>(p.col);
        return *this;
    }

    template<typename Other_T>
    PageRowCol operator/(const PageRowCol<Other_T>& p) const
    {
        PageRowCol copy(*this);
        return copy /= p;
    }

    PageRowCol& operator+=(T scalar)
    {
        page += scalar;
        row += scalar;
        col += scalar;
        return *this;
    }

    PageRowCol operator+(T scalar) const
    {
        PageRowCol copy(*this);
        return copy += scalar;
    }

    PageRowCol& operator-=(T scalar)
    {
        page -= scalar;
        row -= scalar;
        col -= scalar;
        return *this;
    }

    PageRowCol operator-(T scalar) const
    {
        PageRowCol copy(*this);
        return copy -= scalar;
    }

    PageRowCol& operator*=(T scalar)
    {
        page *= scalar;
        row *= scalar;
        col *= scalar;
        return *this;
    }

    PageRowCol operator*(T scalar) const
    {
        PageRowCol copy(*this);
        return copy *= scalar;
    }

    PageRowCol& operator/=(T scalar)
    {
        page /= scalar;
        row /= scalar;
        col /= scalar;
        return *this;
    }

    PageRowCol operator/(T scalar) const
    {
        PageRowCol copy(*this);
        return copy /= scalar;
    }

    /*!
     *  Compare the types considering that some
     *  specializations (e.g., double)
     *  are not exact
     */
    bool operator==(const PageRowCol<T>& p) const
    {
        return page == p.page && row == p.row && col == p.col;
    }


    bool operator!=(const PageRowCol<T>& p) const
    {
        return ! (PageRowCol::operator==(p));
    }

    T volume() const
    {
        return std::abs(page) * std::abs(row) * std::abs(col);
    }

    T normL2() const
    {
        //! Should be able to just use sqrt() here but VC++ 2010 compiler
        //  appears to (incorrectly) be calling std::sqrt() in this scenario
        //  and then complaining it can't find an overloading for some types
        //  (like size_t)
        //  So, cast to double and at that point we might as well just call
        //  std::sqrt()
        return static_cast<T>(std::sqrt(
                static_cast<double>(page * page + row * row + col * col)));
    }
};

template <>
inline size_t PageRowCol<size_t>::volume() const
{
    return page * row * col;
}

template <>
inline bool PageRowCol<float>::operator==(const PageRowCol<float>& p) const
{
    constexpr auto eps = std::numeric_limits<float>::epsilon();
    return std::abs(page - p.page) < eps &&
           std::abs(row - p.row) < eps &&
           std::abs(col - p.col) < eps;
}
template <>
inline bool PageRowCol<double>::operator==(const PageRowCol<double>& p) const
{
    constexpr auto eps = std::numeric_limits<double>::epsilon();
    return std::abs(page - p.page) < eps &&
           std::abs(row - p.row) < eps &&
           std::abs(col - p.col) < eps;
}
}

#endif
