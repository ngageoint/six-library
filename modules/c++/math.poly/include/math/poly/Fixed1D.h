/* =========================================================================
 * This file is part of math.poly-c++ 
 * =========================================================================
 * 
 * (C) Copyright 2004 - 2009, General Dynamics - Advanced Information Systems
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

#ifndef __MATH_POLY_FIXED_1D_H__
#define __MATH_POLY_FIXED_1D_H__

#include <import/except.h>
#include <import/sys.h>
#include "math/poly/OneD.h"

namespace math
{
namespace poly
{

/*!
 *  One dimensional fixed size polynomial.  This is similar to the
 *  math::poly::OneD<_T> type except the user must know the size.
 *
 *  When a mutating method is used, such as *=, the order is fixed and
 *  therefore any higher information is lost, therefore, if higher-order
 *  precision is required, the dev. should use the non-mutating methods,
 *  such as *
 *
 *  We talk about polynomials in terms of orders, but orders are one less
 *  than the total number of terms, since there is a constant term as well.
 *  Its easier to think of polynomials this way, so we leave the standard
 *  notation, and compensate by iterating one extra in all of our loops
 *
 */
template <size_t _Order, typename _T=double> class Fixed1D
{
protected:
    _T mCoef[_Order+1];
public:

   /*!
    *  Default constructor clears memory
    *
    */
    Fixed1D()
    {
        // Initialize coefficents;
        for (size_t i = 0; i < _Order+1; i++)
        {
            mCoef[i] = 0;
        }
    }

    /*!
     *  Unlike the non-fixed size poly, this constructor
     *  will truncate higher orders.  This is what we want.
     */
    Fixed1D(const OneD<_T>& coeff)
    {
        size_t sizeC = coeff.order();
        sizeC = std::min<size_t>(sizeC, _Order);
        for (size_t i = 0; i <= sizeC; i++)
        {
            mCoef[i] = coeff[i];
        }
    }

    /*!
     *  Unlike the non-fixed size poly, this constructor
     *  will truncate higher orders.  This allows us to do something like
     *  \code
     *  Fixed1D<2> quadratic;
     *  Fixed1D<3> cubicCopy(quadratic);
     *  \code
     *
     */
    template<size_t _OtherOrder> Fixed1D(const Fixed1D<_OtherOrder, _T>& coeff)
    {

        size_t sizeC = std::min<size_t>(_OtherOrder, _Order);
        for (size_t i = 0; i <= sizeC; i++)
        {
            mCoef[i] = coeff[i];
        }
    }

    inline size_t order() const { return _Order; }
    inline size_t size() const { return _Order + 1; }

    /*!
     *
     *  Get a fixed polynomial from a non-fixed one
     *
     */
    Fixed1D<_Order, _T>& operator=(const OneD<_T>& coeff)
    {
        size_t sizeC = coeff.order();
        sizeC = std::min<size_t>(sizeC, _Order);
        for (size_t i = 0; i <= sizeC; i++)
        {
            mCoef[i] = coeff[i];
        }
        return *this;
    }

    template<size_t _OtherOrder> Fixed1D<_Order, _T>& operator=(const Fixed1D<_OtherOrder, _T>& coeff)
    {

        unsigned int sizeC = std::min<unsigned int>(_OtherOrder, _Order);
        for (size_t i = 0; i <= sizeC; i++)
        {
            mCoef[i] = coeff[i];
        }
        return *this;
    }



    /*!
     *  Evaluate our polynomial at 'at'
     *
     */
    _T operator() (double at) const
    {
        double rv(0);
        double atPower = 1;

        for (size_t i = 0; i <= _Order; i++)
        {
            rv += mCoef[i]*atPower;
            atPower *= at;
        }
        return rv;
    }

    /*!
     *  Integrate between start and end
     *
     */
    _T integrate(double start, double end) const
    {
        _T rv(0);
        double div;
        double newCoef;
        double endAtPower = end;
        double startAtPower = start;

        for (size_t i = 0; i <= _Order; i++)
        {
            div = 1.0/(i + 1);
            newCoef = mCoef[i] * div;
            rv += newCoef * endAtPower;
            rv -= newCoef * startAtPower;
            endAtPower *= end;
            startAtPower *= start;

        }
        return rv;
    }

    /*!
     *  Take the derivative of the polynomial.  This produces an order-
     *  lower polynomial as output.
     */
    Fixed1D<_Order-1, _T> derivative() const
    {
        Fixed1D<_Order-1, _T> dv;
        if (_Order)
        {
            for (size_t i = 0; i <= _Order-1; i++)
            {
                dv[i] = mCoef[i+1] * (i+1);
            }
        }
        return dv;
    }
    /*!
     *
     *
     */
     _T& operator [] (size_t i)
    {
        if (i > _Order)
            throw except::IndexOutOfRangeException(Ctxt(FmtX("index [%d] is not in range [0..%d]", i, _Order)));
        return mCoef[i];

    }
    _T operator [] (size_t i) const
    {
        if (i > _Order)
            throw except::IndexOutOfRangeException(Ctxt(FmtX("index [%d] is not in range [0..%d]", i, _Order)));

        return mCoef[i];

    }


    /*!
     *  Mult-assign (mutating method)
     *
     */

    Fixed1D<_Order, _T>& operator*=(double cv)
    {
        for (size_t i = 0; i <= _Order; i++)
        {
            mCoef[i] *= cv;
        }
        return *this;
    }
    /*!
     *  Multiply (non mutating)
     *
     */
    Fixed1D<_Order, _T> operator*(double cv) const
    {

        Fixed1D<_Order, _T> m(*this);
        m *= cv;

        return m;
    }

    /*!
     *  Important note.  We do not increase the order here, so be careful.
     *  If you need to increase the order, you should use the other
     *  method below (non-mutating)
     */
    template<size_t _OtherOrder>
        Fixed1D<_Order>& operator *= (const Fixed1D<_OtherOrder>& p)
    {
        // Multiply another into this and do not increase the order
        Fixed1D<_Order+_OtherOrder> newPoly = *this * p;

        // We need to be careful to overload this properly
        *this = newPoly;
        return *this;
    }

    template<size_t _OtherOrder>
        Fixed1D<_Order+_OtherOrder, _T>
        operator * (const Fixed1D<_OtherOrder, _T>& p) const
    {
        // Multiply this one by another, and increase the order
        //Fixed1D<_Order+_OtherOrder, _T> copy(*this);
        Fixed1D<_Order+_OtherOrder, _T> newPoly;

        for (size_t i = 0; i <= _Order; i++)
        {
            for (size_t j = 0; j <= _OtherOrder; j++)
            {
                newPoly[i+j] += mCoef[i] * p[j];
            }
        }
        return newPoly;

    }

    /* These are the trickiest, since we cant know anything at compile time */
    template<size_t _OtherOrder>
        Fixed1D<_Order+_OtherOrder, _T>
        operator+(const Fixed1D<_OtherOrder, _T>& p) const
    {
        Fixed1D<_Order+_OtherOrder, _T> newPoly;
        if (_Order > _OtherOrder)
        {
            newPoly = *this;
            for (size_t i = 0; i <= _OtherOrder; i++)
            {
                newPoly[i] += p[i];
            }
        }
        else
        {
            newPoly = p;
            for (size_t i = 0; i <= _Order; i++)
            {
                newPoly[i] += mCoef[i];
            }
        }
        return newPoly;

    }
    template<size_t _OtherOrder>
        Fixed1D<_Order+_OtherOrder, _T>
    operator-(const Fixed1D<_OtherOrder, _T>& p) const
    {

        return *this + (p * -1.0);

    }
    // In this one, if you try and mutate, we will not change the order
    template<size_t _OtherOrder>
        Fixed1D<_Order, _T>& operator += (const Fixed1D<_OtherOrder, _T>& p)
    {
        *this = *this + p;
        return *this;

    }
    /*!
     *
     *
     */
    template<size_t _OtherOrder>
        Fixed1D<_Order, _T>& operator -= (const Fixed1D<_OtherOrder, _T>& p)
    {
        *this = *this - p;
        return *this;
    }


    /*!
     *  Mult-assign by the reciprocal
     *
     */

    template<size_t _OtherOrder>
        Fixed1D<_Order, _T>& operator /= (double cv)
    {
        double recip = 1.0/cv;
        for (size_t i = 0; i <= _Order; i++)
        {
            mCoef[i] *= recip;
        }
        return *this;
    }

    /*!
     *  Make a copy of this, and mult-assign it
     *
     */
    Fixed1D<_Order, _T> operator / (double cv) const
    {
        Fixed1D<_Order, _T> copy(*this);
        copy *= (1.0/cv);
        return copy;
    }

};

/*!
 *  Actually, this is the same as p * cv, so we just reverse it
 *
 */
template<size_t _Order, typename _T>
    Fixed1D<_Order, _T> operator*(double cv,
                                  const Fixed1D<_Order, _T>& p)
{
    return p*cv;
}



}
}

/*!
 *  Print our poly (y first, so the 2D version looks right)
 */
template<size_t _Order, typename _T>
    std::ostream& operator << (std::ostream& out,
                               const math::poly::Fixed1D<_Order, _T>& p)
{
    for (size_t i = 0 ; i <= _Order; i++)
    {
        out << p[i] << "*y^" << i << " ";
    }
    return out;
}

#endif
