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

#include <cmath>
#include <limits>

#include <import/except.h>
#include <import/sys.h>
#include <math/poly/OneD.h>
#include <math/poly/Utils.h>

namespace math
{
namespace poly
{

template<typename _T>
_T
TwoD<_T>::operator () (double atX, double atY) const
{
    _T ret(0.0);
    double atXPwr = 1.0;
    const auto sz = mCoef.size();
    for (size_t i = 0; i < sz; i++)
    {
        ret += mCoef[i](atY) * atXPwr;
        atXPwr *= atX;
    }
    return ret;
}

template<typename _T>
_T
TwoD<_T>::integrate(double xStart, double xEnd,
                    double yStart, double yEnd) const
{
    _T ret(0.0);
    double endAtPwr = xEnd;
    double startAtPwr = xStart;
    double div = 0;
    double newCoef;
    for (size_t i = 0, sz = mCoef.size(); i < sz; i++)
    {
        div = 1.0 / (i + 1);
        newCoef = mCoef[i].integrate(yStart, yEnd) * div;
        ret += newCoef * endAtPwr;
        ret -= newCoef * startAtPwr;
        endAtPwr *= xEnd;
        startAtPwr *= xStart;
    }
    return ret;
}

template<typename _T>
TwoD<_T>
TwoD<_T>::derivativeY() const
{
    TwoD<_T> ret(0, 0);
    if ((orderY() > 0))
    {
        ret = TwoD<_T>(orderX(), orderY()-1);
        for (size_t i = 0 ; i < mCoef.size() ; i++)
        {
            ret.mCoef[i] = mCoef[i].derivative();
        }
    }
    return ret;
}

template<typename _T>
TwoD<_T>
TwoD<_T>::derivativeX() const
{
    TwoD<_T> ret(0, 0);
    if ((orderX() > 0))
    {
        ret = TwoD<_T>(orderX()-1, orderY());
        for (size_t i = 0, sz = mCoef.size()-1; i < sz; i++)
        {
            ret.mCoef[i] = mCoef[i + 1] * (_T)(i+1);
        }
    }
    return ret;
}

template<typename _T>
OneD<_T>
TwoD<_T>::atY(double y) const
{
    OneD<_T> ret(0);
    if (!empty())
    {
        // We have no more X, but we have Y still
        ret = OneD<_T>(orderX());
        for (size_t i = 0; i < mCoef.size(); i++)
        {
            // Get me down to an X
            ret[i] = mCoef[i](y);
        }
    }
    return ret;
}

template<typename _T>
TwoD<_T>
TwoD<_T>::power(size_t toThe) const
{
    // If its 0, we have to give back a 1*x^0*y^0 poly, since
    // we want a 2D poly out
    if (toThe == 0)
    {
        TwoD<_T> zero(0, 0);
        zero[0][0] = 1;
        return zero;
    }

    TwoD<double> rv = *this;

    // If its 1 give it back now
    if (toThe == 1)
        return rv;


    // Otherwise, we have to raise it
    for (size_t i = 2; i <= toThe; i++)
    {
        rv *= *this;
    }
    return rv;
}


template<typename _T>
TwoD<_T>
TwoD<_T>::flipXY() const
{
    if (empty())
    {
        return TwoD<_T>();
    }

    size_t oY = orderX();
    size_t oX = orderY();
    TwoD<_T> prime(oX, oY);

    for (size_t i = 0; i <= oX; i++)
        for (size_t j = 0; j <= oY; j++)
            prime[i][j] = mCoef[j][i];
    return prime;
}

template<typename _T>
TwoD<_T>
TwoD<_T>::derivativeXY() const
{
    TwoD<_T> ret = derivativeY().derivativeX();
    return ret;
}

template<typename _T>
OneD<_T>
TwoD<_T>::operator [] (size_t i) const
{
    OneD<_T> ret(0);
    if (i < mCoef.size())
    {
        ret = mCoef[i];
    }
    else
    {
        std::ostringstream str;
        str << "index:" << i << " not within range [0..." << mCoef.size() << ")";
        throw except::IndexOutOfRangeException(Ctxt(str));
    }
    return ret;
}

template<typename _T>
_T*
TwoD<_T>::operator [] (size_t i)
{
    if (i < mCoef.size())
    {
        return(&(mCoef[i][0]));
    }
    else
    {
        std::ostringstream str;
        str << "index: " << i << " not within range [0..." << mCoef.size() << ")";
        throw except::IndexOutOfRangeException(Ctxt(str));
    }
}

template<typename _T>
TwoD<_T>&
TwoD<_T>::operator *= (double cv)
{
    const auto sz = mCoef.size();
    for (size_t i = 0; i < sz; i++)
    {
        mCoef[i] *= cv;
    }
    return *this;
}

template<typename _T>
TwoD<_T>
TwoD<_T>::operator * (double cv) const
{
    TwoD<_T> ret(*this);
    ret *= cv;
    return ret;
}

template<typename _T>
TwoD<_T>
operator * (double cv, const TwoD<_T>& p)
{
    return p * cv;
}

template<typename _T>
TwoD<_T>&
TwoD<_T>::operator *= (const TwoD<_T>& p)
{
    TwoD<_T> tmp(orderX() + p.orderX(), orderY() + p.orderY());
    const auto xsz = mCoef.size();
    const auto ysz = p.mCoef.size();
    for (size_t i = 0; i < xsz; i++)
    {
        for (size_t j = 0; j < ysz; j++)
        {
            tmp.mCoef[i + j] += mCoef[i] * p.mCoef[j];
        }
    }
    *this = tmp;
    return *this;
}

template<typename _T>
TwoD<_T>
TwoD<_T>::operator * (const TwoD<_T>& p) const
{
    TwoD<_T> ret(*this);
    ret *= p;
    return ret;
}

template<typename _T>
TwoD<_T>&
TwoD<_T>::operator += (const TwoD<_T>& p)
{
    TwoD<_T> tmp(std::max<size_t>(orderX(), p.orderX()),
                 std::max<size_t>(orderY(), p.orderY()));

    {
        const auto sz = mCoef.size();
        for (size_t ii = 0; ii < sz; ++ii)
        {
            // We use copyFrom instead of an assignment operator here because
            // the assignment operator can potentially change the order of
            // our polynomial.
            tmp.mCoef[ii].copyFrom(mCoef[ii]);
        }
    }
    {
        const auto sz = p.mCoef.size();
        for (size_t ii = 0; ii < sz; ++ii)
        {
            tmp.mCoef[ii] += p.mCoef[ii];
        }
    }

    *this = tmp;
    return *this;
}

template<typename _T>
TwoD<_T>
TwoD<_T>::operator + (const TwoD<_T>& p) const
{
    TwoD<_T> ret(*this);
    ret += p;
    return ret;
}

template<typename _T>
TwoD<_T>&
TwoD<_T>::operator -= (const TwoD<_T>& p)
{
    TwoD<_T> tmp(std::max<size_t>(orderX() ,p.orderX()),
                 std::max<size_t>(orderY(), p.orderY()));

    {
        const auto sz = mCoef.size();
        for (size_t ii = 0; ii < sz; ++ii)
        {
            // We use copyFrom instead of an assignment operator here because
            // the assignment operator can potentially change the order of
            // our polynomial.
            tmp.mCoef[ii].copyFrom(mCoef[ii]);
        }
    }

    {
        const auto sz = p.mCoef.size();
        for (size_t ii = 0; ii < sz; ++ii)
        {
            tmp.mCoef[ii] -= p.mCoef[ii];
        }
    }

    *this = tmp;
    return *this;
}

template<typename _T>
TwoD<_T>
TwoD<_T>::operator-(const TwoD<_T>& p) const
{
    TwoD<_T> ret(*this);
    ret -= p;
    return ret;
}

template<typename _T>
TwoD<_T>&
TwoD<_T>::operator/=(double cv)
{
    double recipCV = 1.0/cv;
    for (size_t i = 0, sz = mCoef.size() ; i < sz; i++)
    {
        mCoef[i] *= recipCV;
    }
    return *this;
}

template<typename _T>
TwoD<_T>
TwoD<_T>::operator/(double cv) const
{
    TwoD<_T> ret(*this);
    ret *= (1.0/cv);
    return ret;
}

template<typename _T>
std::ostream&
operator << (std::ostream& out, const TwoD<_T>& p)
{
    for (size_t i = 0 ; i < p.mCoef.size() ; i++)
    {
        out <<  "x^" << i << "*(" << p[i] << ")" << std::endl;
    }
    return out;
}

template<typename _T>
bool
TwoD<_T>::operator == (const TwoD<_T>& p) const
{
    const auto sz = mCoef.size();
    const auto psz = p.mCoef.size();
    const auto minSize = std::min(sz, psz);

    // guard against uninitialized
    if (minSize == 0 && (sz != psz))
        return false;

    for (size_t i = 0 ; i < minSize ; i++)
    {
        if (mCoef[i] != p.mCoef[i])
            return false;
    }

    // Cover case where one polynomial has more coefficients than the other.
    if (sz > psz)
    {
        OneD<_T> dflt(orderY());

        for (size_t ii = minSize ; ii < sz; ++ii)
        {
            if (mCoef[ii] != dflt)
            {
                return false;
            }
        }
    }
    else if (sz < psz)
    {
        OneD<_T> dflt(p.orderY());

        for (size_t ii = minSize ; ii < psz ; ++ii)
        {
            if (p.mCoef[ii] != dflt)
            {
                return false;
            }
        }
    }

    return true;
}

template<typename _T>
bool
TwoD<_T>::operator != (const TwoD<_T>& p) const
{
    return !(*this == p);
}

template<typename _T>
TwoD<_T> TwoD<_T>::scaleVariable(double scale) const
{
    return scaleVariable(scale, scale);
}

template<typename _T>
TwoD<_T> TwoD<_T>::scaleVariable(double scaleX, double scaleY) const
{
    return ::math::poly::scaleVariable<TwoD<_T> >(*this, scaleX, scaleY);
}

template<typename _T>
TwoD<_T> TwoD<_T>::truncateTo(size_t orderX, size_t orderY) const
{
    orderX = std::min(this->orderX(), orderX);
    orderY = std::min(this->orderY(), orderY);

    TwoD<_T> newP(orderX, orderY);
    for (size_t ii = 0; ii <= orderX; ++ii)
    {
        for (size_t jj = 0; jj <= orderY; ++jj)
        {
            newP[ii][jj] = (*this)[ii][jj];
        }
    }

    return newP;
}

template<typename _T>
TwoD<_T> TwoD<_T>::truncateToNonZeros(double zeroEpsilon) const
{
    zeroEpsilon = std::abs(zeroEpsilon);
    constexpr size_t NOT_FOUND(std::numeric_limits<size_t>::max());
    size_t newOrderX(NOT_FOUND);
    size_t newOrderY(NOT_FOUND);

    // Find the highest order non-zero coefficient.
    for (size_t ii = 0, xi = orderX(); ii <= orderX(); ++ii, --xi)
    {
        for (size_t jj = 0, yi = orderY(); jj <= orderY(); ++jj, --yi)
        {
            if (std::abs((*this)[xi][yi]) > zeroEpsilon)
            {
                if (newOrderX == NOT_FOUND)
                {
                    newOrderX = xi;
                }
                if (newOrderY == NOT_FOUND || yi > newOrderY)
                {
                    newOrderY = yi;
                }
                break;
            }
        }
    }

    // In case we never found a non-zero term, don't go lower than zero order
    if (newOrderX == NOT_FOUND)
    {
        newOrderX = 0;
    }
    if (newOrderY == NOT_FOUND)
    {
        newOrderY = 0;
    }

    return truncateTo(newOrderX, newOrderY);
}

template<typename _T>
TwoD<_T> TwoD<_T>::transformInput(
        const math::poly::TwoD<_T>& gx,
        const math::poly::TwoD<_T>& gy,
        double zeroEpsilon) const
{
    math::poly::TwoD<_T> newP(orderX(), orderY());

    for (size_t ii = 0; ii <= orderX(); ++ii)
    {
        for (size_t jj = 0; jj <= orderY(); ++jj)
        {
            newP += gx.power(ii) * gy.power(jj) * (*this)[ii][jj];
        }
    }

    return newP.truncateToNonZeros(zeroEpsilon);
}

template<typename _T>
TwoD<_T> TwoD<_T>::transformInput(const math::poly::TwoD<_T>& gx,
                                  double zeroEpsilon) const
{
    // We want to retain our y value. So create a polynomial which
    // is y = y.
    TwoD<_T> gy(1, 1);
    gy[0][1] = 1;
    gy[0][0] = 0;

    return transformInput(gx, gy, zeroEpsilon);
}

template<typename _T>
bool TwoD<_T>::isScalar() const
{
    for (size_t ii = 0; ii <= orderX(); ++ii)
    {
        for (size_t jj = 0; jj <= orderY(); ++jj)
        {
            // Initial term may be anything
            if ((ii != 0 || jj != 0) && (*this)[ii][jj] != 0)
            {
                return false;
            }
        }
    }
    return true;
}

} // poly
} // math
