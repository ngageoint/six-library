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

#ifndef __MATH_POLY_FIXED_2D_H__
#define __MATH_POLY_FIXED_2D_H__

#include "math/poly/Fixed1D.h"

namespace math
{
namespace poly
{

/*!
 *  This class is similar to the TwoD<_T> except that it uses a fixed
 *  size for order.  As in the Fixed1D case, the order in each dim is one
 *  less than the size of the coefficients.
 */
template<size_t _OrderX, size_t _OrderY, typename _T=double> class Fixed2D
{
protected:
    Fixed1D<_OrderY, _T> mCoef[_OrderX+1];
public:
    Fixed2D() {}

    template<size_t _OtherOrderX, size_t _OtherOrderY>
        Fixed2D(const Fixed2D<_OtherOrderX, _OtherOrderY, _T>& coeff)
    {

        size_t sizeC = std::min<unsigned int>(_OtherOrderX, _OrderX);
        for (size_t i = 0; i <= sizeC; i++)
        {
            mCoef[i] = coeff[i];
        }
    }



    Fixed2D(const TwoD<_T>& coeff)
    {

        size_t sizeC = std::min<size_t>(coeff.orderX(), _OrderX);
        for (size_t i = 0; i <= sizeC; i++)
        {
            mCoef[i] = coeff[i];
        }
    }

    Fixed2D<_OrderX, _OrderY, _T>& operator=(const TwoD<_T>& coeff)
    {

        size_t sizeC = std::min<unsigned int>(coeff.orderX(), _OrderX);
        for (size_t i = 0; i <= sizeC; i++)
        {
            mCoef[i] = coeff[i];
        }
        return *this;
    }

    template<size_t _OtherOrderX, size_t _OtherOrderY>
        Fixed2D<_OrderX, _OrderY, _T>&
             operator=(const Fixed2D<_OtherOrderX, _OtherOrderY, _T>& coeff)
    {

        size_t sizeC = std::min<size_t>(_OtherOrderX, _OrderX);
        for (size_t i = 0; i <= sizeC; i++)
        {
            mCoef[i] = coeff[i];
        }
        return *this;
    }

    size_t orderX() const { return _OrderX; }
    size_t orderY() const { return _OrderY; }

    inline _T operator()(double atX, double atY) const
    {
        _T rv(0);
        double atXPower(1);

        for (size_t i = 0; i <= _OrderX; i++)
        {
            rv += mCoef[i](atY)*atXPower;
            atXPower *= atX;

        }
        return rv;
    }
    _T integrate(double startX, double endX, double startY, double endY) const
    {
        _T rv(0);
        double div(0);
        double endAtPower = endX;
        double startAtPower = startX;
        double newCoef;
        for (size_t i = 0; i <= _OrderX; i++)
        {
            div = 1.0/(i+1);
            newCoef = mCoef[i].integrate(startY, endY) * div;
            rv += newCoef * endAtPower;
            rv -= newCoef * startAtPower;
            endAtPower *= endX;
            startAtPower *= startX;

        }
        return rv;
    }



    // Will not work with order 0!
    Fixed2D<_OrderX, _OrderY-1, _T> derivativeY() const
    {
        if (!_OrderY)
            throw except::Exception(Ctxt("Must have order 1 in Y"));
        Fixed2D<_OrderX, _OrderY-1, _T> dy;

        for (size_t i = 0; i <= _OrderX; i++)
        {
            dy[i] = mCoef[i].derivative();
        }
        return dy;
    }
    Fixed2D<_OrderX-1, _OrderY, _T> derivativeX() const
    {
        if (!_OrderX)
            throw except::Exception(Ctxt("Must have order 1 in Y"));
        Fixed2D<_OrderX-1, _OrderY, _T> dx;
        for (size_t i = 0; i <= _OrderX-1; i++)
        {
            dx[i] = mCoef[i+1] * (_T)(i+1);
        }
        return dx;

    }

    Fixed2D<_OrderX-1, _OrderY-1, _T> derivativeXY() const
    {
        Fixed2D<_OrderX-1, _OrderY-1> rv = derivativeY().derivativeX();
        return rv;
    }
    Fixed1D<_OrderX, _T> atY(double y) const
    {
        if (!_OrderX)
            throw except::Exception(Ctxt("Must have order 1 in Y"));

        // We will have a 1D poly of order X
        Fixed1D<_OrderX, _T> polyY;

        for (size_t i = 0; i <= _OrderX; i++)
        {
            polyY[i] = mCoef[i](y);
        }
        return polyY;

    }
    inline Fixed1D<_OrderY, _T> operator[](size_t i) const
    {
        return mCoef[i];
    }

    inline Fixed1D<_OrderY, _T>& operator[](size_t i)
    {
        return mCoef[i];
    }

    Fixed2D<_OrderX, _OrderY, _T> operator * (double cv) const
    {
        Fixed2D<_OrderX, _OrderY> copy(*this);

        for (size_t i = 0; i <= _OrderX; i++)
        {
            copy[i] *= cv;
        }
        return copy;

    }
    /*!
     *  As in the one D case, we make sure that only the multiply does
     *  the real work.  Here we just cut down the order (truncate) to
     *  whatever order we already have
     */
    template<size_t _OtherOrderX, size_t _OtherOrderY>
    Fixed2D<_OrderX, _OrderY, _T>& operator *= (const Fixed2D<_OtherOrderX, _OtherOrderY, _T>& p)
    {
        Fixed2D<_OrderX+_OtherOrderX, _OrderY+_OtherOrderY> newPoly
            = *this * p;
        *this = newPoly;
    }

    /*!
     *  We need to be careful with this operation.  Our new polynomial
     *  will have order X1 + X2, Y1 + Y2, and since we may want to truncate
     *  we can use either self-assignment or the overloaded copy operator.
     *
     */
    template<size_t _OtherOrderX, size_t _OtherOrderY>
        Fixed2D<_OrderX+_OtherOrderX, _OrderY+_OtherOrderY, _T>
        operator * (const Fixed2D<_OtherOrderX, _OtherOrderY, _T>& p) const
    {
        Fixed2D<_OrderX+_OtherOrderX, _OrderY+_OtherOrderY, _T> copy(*this);
        Fixed2D<_OrderX+_OtherOrderX, _OrderY+_OtherOrderY, _T> newPoly;

        for (size_t i = 0; i <= _OrderX; i++)
        {
            for (size_t j = 0; j <= _OtherOrderX; j++)
            {
                // This produces a new polynomial which uses the
                // copy constructor
                newPoly[i+j] += copy[i] * p[j];
            }

        }
        return newPoly;

    }
    template<size_t _OtherOrderX, size_t _OtherOrderY>
    Fixed2D<_OrderX, _OrderY, _T>& operator += (const Fixed2D<_OtherOrderX, _OtherOrderY, _T>& p)
    {
        *this = *this + p;
    }

    template<size_t _OtherOrderX, size_t _OtherOrderY>
    Fixed2D<_OrderX+_OtherOrderX, _OrderY+_OtherOrderY, _T>
        operator+(const Fixed2D<_OtherOrderX, _OtherOrderY, _T>& p) const

    {
        Fixed2D<_OrderX+_OtherOrderX, _OrderY+_OtherOrderY, _T> copy(*this);

        for (size_t i = 0; i <= _OtherOrderX; i++)
        {
            copy[i] += p[i];
        }
        return copy;

    }

    template<size_t _OtherOrderX, size_t _OtherOrderY>
        Fixed2D<_OrderX, _OrderY, _T>& operator-=(const Fixed2D<_OtherOrderX, _OtherOrderY, _T>& p)
    {

        *this = *this + (p * -1);
        return *this;

    }
    template<size_t _OtherOrderX, size_t _OtherOrderY>
        Fixed2D<_OrderX+_OtherOrderX, _OrderY+_OtherOrderY, _T>
        operator - (const Fixed2D<_OtherOrderX, _OtherOrderY, _T>& p) const
    {
        return *this + (p * -1);


    }
    Fixed2D<_OrderX, _OrderY, _T>& operator /= (double cv)
    {
        for (size_t i = 0; i <= _OrderX; i++)
        {
            mCoef[i] /= cv;
        }
        return *this;
    }

    Fixed2D<_OrderX, _OrderY, _T> operator / (double cv) const
    {
        Fixed2D<_OrderX, _OrderY> copy(*this);

        for (size_t i = 0; i <= _OrderX; i++)
        {
            copy[i] /= cv;
        }
        return copy;


    }
/*
    // Be careful that output order is high enough
    template<size_t _NewOrderX, size_t _NewOrderY>
    Fixed2D<_OrderX, _OrderY, _T> power(int toThe) const
    {
        // If its 0, we have to give back a 1*x^0*y^0 poly, since
        // we want a 2D poly out
        Fixed2D<_OrderX*toThe, _OrderY*toThe> rv;

        if (toThe == 0)
        {
            rv[0][0] = 1;
            return rv;
        }
        rv = *this;

        if (toThe == 1)
        {
            return rv;
        }

        for (int i = 2; i <= toThe; i++)
        {
            rv *= *this;
        }
        return rv;
    }


*/



};


}
}

template<size_t _OrderX, size_t _OrderY, typename _T>
math::poly::Fixed2D<_OrderX, _OrderY, _T>
    operator * (double cv, const math::poly::Fixed2D<_OrderX, _OrderY, _T>& p)
{
    math::poly::Fixed2D<_OrderX, _OrderY> copy(p);

    for (size_t i = 0; i <= _OrderX; i++)
    {
        copy[i] *= cv;
    }
    return copy;
}

template<size_t _OtherOrderX, size_t _OtherOrderY, typename _T>
std::ostream&
operator << (std::ostream& out, const math::poly::Fixed2D<_OtherOrderX, _OtherOrderY, _T>& p)
{
    for (size_t i = 0 ; i <= _OtherOrderX ; i++)
    {
      out << "x^" << i << "*(" << p[i] << ")" << std::endl;
    }
    return out;
}
#endif
