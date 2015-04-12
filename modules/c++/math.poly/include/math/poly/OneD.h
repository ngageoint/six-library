/* =========================================================================
 * This file is part of math.poly-c++ 
 * =========================================================================
 * 
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
 *
 * math.poly-c++ is free software; you can redistribute it and/or modify
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

#ifndef __MATH_POLY_ONED_H__
#define __MATH_POLY_ONED_H__

#include <iostream>
#include <sstream>
#include <vector>
#include <iterator>
#include <math/linear/Vector.h>

namespace math
{
namespace poly
{
/*! 
 *  \class OneD
 *  \brief 1-D polynomial evaluation
 *
 *  This class defines an implementation for a 1-D polynomial. It should
 *  support datatypes that allow the following operators:
 *  _T(0), _T*_T, _T*=_T, _T+_T, _T+=_T, _T-_T, _T-=_T, -T*<double>
 *
 *  It supports polynomials of the form:
 *  a0 + a1*x + a2*x^2 + ... (in ascending powers)
 *
 *   It supports evaluating the integral over a specified interval (a...b)
 *
 *   It supports computing the derivative and 
 *   the multiplication/addition/subtraction of 1-D polynomials.
 */
template<typename _T>
class OneD
{
protected:
    std::vector<_T> mCoef;

public:
    /*!
     * The polynomial is invalid (i.e. size() will return 0 and order() will
     * throw)
     */
    OneD()
    {
    }

    /*!
     *  A vector of ascending power coefficients (note that
     *  this is the reverse of Matlab)
     */
    OneD(const std::vector<_T>& coef) :
        mCoef(coef)
    {
        if (mCoef.empty())
            mCoef.resize(1, (_T)0.0);
    }

    /*!
     *  Create a vector of given order, with each coefficient
     *  set to zero
     */
    OneD(size_t order) :
        mCoef(order + 1, (_T)0.0)
    {
    }

    //! assignment operator
    OneD& operator=(const OneD& o)
    {
        if (&o != this)
        {
            mCoef.clear();
            std::copy(o.mCoef.begin(), o.mCoef.end(), std::back_inserter(mCoef));
        }
        return *this;
    }

    /*!
     *  This function allows you to copy the values
     *  directly from a raw buffer.  The first argument
     *  is the order, NOT THE NUMBER OF COEFFICIENTS.
     *  Therefore, you should always pass an array with
     *  one more element than the first argument
     *
     *  The power coefficients are in ascending order
     *  (note that this is the reverse of Matlab)
     *
     *  \param order The order of the polynomial
     *  \param The order + 1 coefficients to initialize
     */
    OneD(size_t order, const _T* coef)
    {
        mCoef.resize(order + 1);
        memcpy(&mCoef[0], coef, (order + 1) * sizeof(_T));
    }

    size_t order() const
    {
        if (empty())
            throw except::IndexOutOfRangeException(
                    Ctxt("Can't have an order less than zero"));

        return mCoef.size() - 1;
    }

    inline size_t size() const
    {
        return mCoef.size();
    }

    inline bool empty() const
    {
        return mCoef.empty();
    }

    /*!
     * Returns a scaled polynomial such that
     * P'(x) = P(x * scale)
     *
     * If you had a polynomial in units of feet that you wanted in units of
     * meters, you would pass in a scale value of METERS_TO_FEET (since this
     * is what you would scale all your inputs by if you were using the
     * original polynomial in feet).
     *
     * \param scale The factor to apply
     *
     * \return Scaled polynomial of the same order as the original polynomial
     */
    OneD<_T> scaleVariable(double scale) const;

    /*!
     * Returns a truncated polynomial of the specified order.  All
     * higher-order terms are simply removed.
     *
     * \param order Order to truncate to for x
     *
     * \return Truncated polynomial of the desired order.
     */
    OneD<_T> truncateTo(size_t order) const;

    /*!
     * Returns a polynomial truncated to smallest order to retain only
     * non-zero coefficients.
     *
     * \param zeroEpsilon Tolerance for what a "zero" coefficient is
     *
     * \return Truncated polynomial
     */
    OneD<_T> truncateToNonZeros(double zeroEpsilon = 0.0) const;

    /*!
     * Transforms the polynomial input so that
     * Fx(Gx(x)) = F'x(x).
     *
     * \param gx The polynomial to replace the x values.
     * \param zeroEpsilon Internally this may create a bunch of "extra"
     *        coefficients with value of 0. This method will clean them
     *        up. Passing in a value other 0 will change the tolerance
     *        when looking for these coefficients.
     *
     * \return Fx(Gx(x))
     */
    OneD<_T> transformInput(const OneD<_T>& gx,
                            double zeroEpsilon = 0.0) const;
    /*!
     * Copies all valid data from p into the coefficients.
     * This is used in situations where we want to assign a OneD but do
     * not want to change the underlying order of the OneD.
     *
     * In general this method should only be used internally between poly
     * classes.
     *
     * \param p The polynomial to copy from.
     */
    void copyFrom(const OneD<_T>& p);

    _T operator ()(double at) const;
    _T integrate(double start, double end) const;
    OneD<_T>derivative() const;
    _T& operator[](size_t i);
    _T operator[](size_t i) const;
    template<typename _TT>
    friend std::ostream& operator <<(std::ostream& out, const OneD<_TT>& p);
    OneD<_T>& operator *=(double cv);
    OneD<_T>operator *(double cv) const;
    template<typename _TT>
    friend OneD<_TT>operator *(double cv, const OneD<_TT>& p);
    OneD<_T>& operator *=(const OneD<_T>& p);
    OneD<_T>operator *(const OneD<_T>& p) const;
    OneD<_T>& operator +=(const OneD<_T>& p);
    OneD<_T>operator +(const OneD<_T>& p) const;
    OneD<_T>& operator -=(const OneD<_T>& p);
    OneD<_T>operator -(const OneD<_T>& p) const;
    OneD<_T>& operator /=(double cv);
    OneD<_T>operator /(double cv) const;

    OneD<_T> power(size_t toThe) const;

    template<typename Vector_T> bool operator==(const Vector_T& p) const
    {
        size_t sz = size();
        size_t psz = p.size();
        size_t minSize = std::min<size_t>(sz, psz);

        // guard against uninitialized
        if (minSize == 0 && (sz != psz))
            return false;

        for (size_t i = 0; i < minSize; i++)
            if (!math::linear::equals(mCoef[i], p[i]))
                return false;

        _T dflt(0.0);

        // Cover case where one polynomial has more
        // coefficients than the other.
        if (sz > psz)
        {
            for (size_t i = minSize; i < sz; i++)
                if (!math::linear::equals(mCoef[i], dflt))
                    return false;
        }
        else if (sz < psz)
        {
            for (size_t i = minSize; i < psz; i++)
                if (!math::linear::equals(p[i], dflt))
                    return false;
        }

        return true;
    }

    template<typename Vector_T> bool operator!=(const Vector_T& p) const
    {
        return !(*this == p);
    }
};

} // poly
} // math
#include "math/poly/OneD.hpp"
#endif
