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

#ifndef __MATH_POLY_TWOD_H__
#define __MATH_POLY_TWOD_H__

#include <math/poly/OneD.h>
#include <math/linear/Matrix2D.h>

namespace math
{
namespace poly
{
/*! This class defines an implementation for a 2-D polynomial. It should
    support datatypes that allow the following operators:
    _T(0), _T*_T, _T*=_T, _T+_T, _T+=_T, _T-_T, _T-=_T, -T*<double>

    It supports polynomials of the form:
    a00 + a01*x + a02*x^2 + ...
    y*(a10 + a11*x + a12*x^2 + ...)

    It supports evaluating the integral over a specified interval (a...b, c...d)

    It supports computing the derivative

    And, it supports the multiplication/addtion/subtraction of 2-D polynomials.

    Also note:
    In a 2-D sense,
       X -> line
       Y -> elem
*/
template<typename _T>
class TwoD
{
protected:
    //! using a vector of one-d polynomials simplify the implementation.
    std::vector<OneD<_T> > mCoef;

public:

    std::vector<OneD<_T> >& coeffs(){ return mCoef; }

    //! The polynomial is invalid (i.e. orderX() and orderY() will throw)
    TwoD() {}  // = default; // error w/ICC and "const" member data

    TwoD(size_t orderX, size_t orderY) : mCoef(orderX+1,OneD<_T>(orderY)) {}

    template<typename Vector_T> TwoD(size_t orderX, size_t orderY,
                                     const Vector_T& coeffs)
    {
        mCoef.resize(orderX+1,OneD<_T>(orderY));
        for (size_t i = 0; i <= orderX; ++i)
        {
            for (size_t j = 0; j <= orderY; ++j)
            {
                mCoef[i][j] = coeffs[i * (orderY+1) + j];
            }
        }
    }

    TwoD(const std::vector<OneD<_T> >& v) :
        mCoef(v)
    {
    }

    bool empty() const
    {
        return mCoef.empty();
    }
    size_t orderX() const
    {
        if (empty())
            throw except::IndexOutOfRangeException(Ctxt("Can't have an order less than zero"));

        return mCoef.size() - 1;
    }
    size_t orderY() const
    {
        if (empty())
            throw except::IndexOutOfRangeException(Ctxt("Can't have an order less than zero"));
        return mCoef[0].order();
    }
    _T operator () (double atX, double atY) const;
    _T integrate(double xStart, double xEnd, double yStart, double yEnd) const;

    //! Must check the size of the OneD coming in because
    //  the dimensions of the TwoD is rigid, and all OneD
    //  polys must be of the same size
    void set(size_t i, const OneD<_T>& p)
    {
        if (i > orderX())
            throw except::Exception(
                Ctxt("Index [" + str::toString<size_t>(i) +
                "] is out of bounds for orderX [" +
                str::toString<size_t>(orderX()) + "]"));
        else if (p.order() != orderY())
            throw except::Exception(
                Ctxt("OneD poly [" + str::toString<size_t>(p.order()) +
                "] is of the incorrect size for orderY [" +
                str::toString<size_t>(orderY()) + "]"));
        else
            mCoef[i] = p;
    }

    /*!
     *  Transposes the coefficients so that X is Y and Y is X
     *
     */
    TwoD<_T> flipXY() const;
    TwoD<_T> derivativeY() const;
    TwoD<_T> derivativeX() const;
    TwoD<_T> derivativeXY() const;

    /*!
     * Returns a scaled polynomial such that
     * P'(x, y) = P(x * scaleX, y * scaleY)
     *
     * \param scaleX The factor to apply to x
     * \param scaleY The factor to apply to y
     *
     * \return Scaled polynomial of the same order as the original polynomial
     */
    TwoD<_T> scaleVariable(double scaleX, double scaleY) const;

    /*!
     * Returns a scaled polynomial such that
     * P'(x, y) = P(x * scale, y * scale)
     *
     * If you had a polynomial in units of feet that you wanted in units of
     * meters, you would pass in a scale value of METERS_TO_FEET (since this
     * is what you would scale all your inputs by if you were using the
     * original polynomial in feet).
     *
     * \param scale The factor to apply to both x and y
     *
     * \return Scaled polynomial of the same order as the original polynomial
     */
    TwoD<_T> scaleVariable(double scale) const;

    /*!
     * Returns a truncated polynomial of the specified order.  All
     * higher-order terms are simply removed.
     *
     * \param orderX Order to truncate to for x
     * \param orderY Order to truncate to for y
     *
     * \return Truncated polynomial of order orderX x orderY
     */
    TwoD<_T> truncateTo(size_t orderX, size_t orderY) const;

    /*!
     * Returns a polynomial truncated to smallest order to retain only
     * non-zero coefficients.
     *
     * \param zeroEpsilon Tolerance for what a "zero" coefficient is
     *
     * \return Truncated polynomial
     */
    TwoD<_T> truncateToNonZeros(double zeroEpsilon = 0.0) const;

    /*!
     * Transforms the polynomial input so that
     * Fx(Gx(x, y), Gy(x, y)) = F'x(x, y).
     *
     * \param gx The polynomial to replace the x values.
     * \param gy The polynomial to replace the y values.
     * \param zeroEpsilon Internally this may create a bunch of "extra"
     *        coefficients with value of 0. This method will clean them
     *        up. Passing in a value other 0 will change the tolerance
     *        when looking for these coefficients.
     *
     * \return Fx(Gx(x, y), Gy(x, y))
     */
    TwoD<_T> transformInput(const math::poly::TwoD<_T>& gx,
                            const math::poly::TwoD<_T>& gy,
                            double zeroEpsilon = 0.0) const;

    /*!
     * Transforms the polynomial input so that
     * Fx(Gx(x, y), y) = F'x(x, y).
     *
     * \param gx The polynomial to replace the x values.
     * \param zeroEpsilon Internally this may create a bunch of "extra"
     *        coefficients with value of 0. This method will clean them
     *        up. Passing in a value other 0 will change the tolerance
     *        when looking for these coefficients.
     *
     * \return Fx(Gx(x, y), y)
     */
    TwoD<_T> transformInput(const math::poly::TwoD<_T>& gx,
                            double zeroEpsilon = 0.0) const;

    /*!
     * This evaluates y in the 2D polynomial, leaving a 1D polynomial in x
     * That is, poly(x, y) == poly.atY(y)(x)
     * If you want the equivalent of an atX() method so that you get a 1D
     * polynomial in y, you can do poly.flipXY().atY(x)
     */
    OneD<_T> atY(double y) const;
    OneD<_T> operator [] (size_t i) const;
    /*! In case you are curious about the return value, this guarantees that
      someone can only change the coefficient stored at [x][y], and not the
      polynomial itself. Unfortunately, however, it does not allow one bounds
      checking on the size of the polynomial.
    */
    _T* operator [] (size_t i);
    TwoD<_T>& operator *= (double cv) ;
    TwoD<_T> operator * (double cv) const;
    template<typename _TT>
        friend TwoD<_TT> operator * (double cv, const TwoD<_TT>& p);
    TwoD<_T>& operator *= (const TwoD<_T>& p);
    TwoD<_T> operator * (const TwoD<_T>& p) const;
    TwoD<_T>& operator += (const TwoD<_T>& p);
    TwoD<_T> operator + (const TwoD<_T>& p) const;
    TwoD<_T>& operator -= (const TwoD<_T>& p);
    TwoD<_T> operator - (const TwoD<_T>& p) const;
    TwoD<_T>& operator /= (double cv);
    TwoD<_T> operator / (double cv) const;
    bool operator == (const TwoD<_T>& p) const;
    bool operator != (const TwoD<_T>& p) const;

    TwoD<_T> power(size_t toThe) const;

    template<typename _TT>
        friend std::ostream& operator << (std::ostream& out, const TwoD<_TT>& p);

    /*!
     *  serialize out to a boost stream
     */
    template <class Archive>
    void serialize(Archive& ar, const unsigned int  /*version*/)
    {
        ar & mCoef;
    }

    /*
     * Return true if polynomial represents  scalar value
     * That is, every element other than [0][0] is 0
     */
    bool isScalar() const;
};

} // poly
} // math
#include "math/poly/TwoD.hpp"
#endif
