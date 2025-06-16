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
#include <import/except.h>
#include <import/sys.h>
#include <math/poly/Utils.h>
#include <math/linear/VectorN.h>

namespace math
{
namespace poly
{

template<typename _T>
_T
OneD<_T>::operator () (double at) const
{
   _T ret(0.0);
   double atPwr = 1.0;
   const auto sz = mCoef.size();
   for (size_t i = 0 ; i < sz; i++)
   {
      ret += mCoef[i]*atPwr;
      atPwr *= at;
   }
   return ret;
}

template<typename _T>
_T
OneD<_T>::integrate(double start, double end) const
{
    double ret(0.0);
   double endAtPwr = end;
   double startAtPwr = start;
   const auto sz = mCoef.size();
   for (size_t i = 0; i < sz; i++)
   {
      const auto div = 1.0 / (static_cast<double>(i) + 1.0);
      const double newCoef = mCoef[i] * div;
      ret += newCoef * endAtPwr;
      ret -= newCoef * startAtPwr;
      endAtPwr *= end;
      startAtPwr *= start;
   }
   return static_cast<_T>(ret);
}

template<>
inline
math::linear::VectorN<3, double>
OneD<math::linear::VectorN<3, double> >::integrate(double start, double end) const
{
   math::linear::VectorN<3, double> ret(0.0);

   const size_t polyOrder = order();
   OneD<double> poly0(polyOrder);
   OneD<double> poly1(polyOrder);
   OneD<double> poly2(polyOrder);

   const auto sz = mCoef.size();
   for (size_t term = 0; term < sz; term++)
   {
       poly0[term] = mCoef[term][0];
       poly1[term] = mCoef[term][1];
       poly2[term] = mCoef[term][2];
   }

   ret[0] = poly0.integrate(start, end);
   ret[1] = poly1.integrate(start, end);
   ret[2] = poly2.integrate(start, end);

   return ret;
}

template<typename T>
OneD<T>
OneD<T>::derivative() const
{
    OneD<T> ret(0);
    if (order() > 0)
    {
        ret = OneD<T>(order()-1);
        const auto sz = mCoef.size() - 1;
        for (size_t ii = 0; ii < sz; ii++)
        {
            ret[ii] = mCoef[ii + 1] * static_cast<T>(ii + 1);
        }
    }
    return ret;
}

template<>
inline
OneD< math::linear::VectorN<3, double> >
OneD< math::linear::VectorN<3, double> >::derivative() const
{
   OneD< math::linear::VectorN<3, double> > ret(0);

   const size_t polyOrder = order();
   if (polyOrder > 0)
   {
       OneD<double> poly0(polyOrder);
       OneD<double> poly1(polyOrder);
       OneD<double> poly2(polyOrder);

       {
           const auto sz = mCoef.size();
           for (size_t term = 0; term < sz; term++)
           {
               poly0[term] = mCoef[term][0];
               poly1[term] = mCoef[term][1];
               poly2[term] = mCoef[term][2];
           }
       }

       poly0 = poly0.derivative();
       poly1 = poly1.derivative();
       poly2 = poly2.derivative();

       ret = OneD<math::linear::VectorN<3, double> >(polyOrder - 1);
       {
           const auto sz = mCoef.size() - 1;
           for (size_t term = 0; term < sz; term++)
           {
               ret[term][0] = poly0[term];
               ret[term][1] = poly1[term];
               ret[term][2] = poly2[term];
           }
       }
   }

   return ret;
}

template<typename _T>
_T
OneD<_T>::velocity(double x) const
{
    return derivative()(x);
}

template<typename _T>
_T
OneD<_T>::acceleration(double x) const
{
    return derivative().derivative()(x);

}

template<typename _T>
_T&
OneD<_T>::operator [] (size_t i)
{
    if (i < mCoef.size())
    {
        return (mCoef[i]);
    }
    else
    {
        std::ostringstream str;
        str << "index: " << i << " not within range [0..." << mCoef.size() << ")";
        throw except::IndexOutOfRangeException(Ctxt(str));
    }
}


template<typename _T>
_T
OneD<_T>::operator [] (size_t i) const
{
   if (i < mCoef.size())
   {
      return mCoef[i];
   }
   else
   {
      std::ostringstream str;
      str << "idx(" << i << ") not within range [0..." << mCoef.size() << ")";
      throw except::IndexOutOfRangeException(Ctxt(str));
   }
}

template<typename _T>
std::ostream&
operator << (std::ostream& out, const OneD<_T>& p)
{
   for (size_t i = 0 ; i < p.mCoef.size() ; i++)
   {
      out << p[i] << "*y^" << i << " ";
   }
   return out;
}

template<typename _T>
OneD<_T>&
OneD<_T>::operator *= (double cv)
{
    const auto sz = mCoef.size();
    for (size_t i = 0; i < sz; i++)
    {
        mCoef[i] *= cv;
    }
    return *this;
}

template<typename _T>
OneD<_T>
OneD<_T>::operator * (double cv) const
{
    OneD<_T> ret(*this);
    ret *= cv;
    return ret;
}

template<typename _T>
OneD<_T>
operator * (double cv, const OneD<_T>& p)
{
    return p*cv;
}

template<typename _T>
OneD<_T>&
OneD<_T>::operator *= (const OneD<_T>& p)
{
   OneD<_T> tmp(order()+p.order());
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
OneD<_T>
OneD<_T>::operator * (const OneD<_T>& p) const
{
    OneD<_T> ret(*this);
    ret *= p;
    return ret;
}

template<typename _T>
OneD<_T>&
OneD<_T>::operator += (const OneD<_T>& p)
{
    OneD<_T> tmp(std::max<size_t>(order(), p.order()));
    {
        const auto sz = mCoef.size();
        for (size_t i = 0; i < sz; i++)
        {
            tmp.mCoef[i] = mCoef[i];
        }
    }
    {
        const auto sz = p.mCoef.size();
        for (size_t i = 0; i < sz; i++)
        {
            tmp.mCoef[i] += p.mCoef[i];
        }
    }
   *this = tmp;
   return *this;
}

template<typename _T>
OneD<_T>
OneD<_T>::operator + (const OneD<_T>& p) const
{
    OneD<_T> ret(*this);
    ret += p;
    return ret;
}

template<typename _T>
OneD<_T>&
OneD<_T>::operator -= (const OneD<_T>& p)
{
   OneD<_T> tmp(std::max<size_t>(order(), p.order()));
    {
        const auto sz = mCoef.size();
        for (size_t i = 0; i < sz; i++)
        {
            tmp.mCoef[i] = mCoef[i];
        }
    }
    {
        const auto sz = p.mCoef.size();
        for (size_t i = 0; i < sz; i++)
        {
            tmp.mCoef[i] -= p.mCoef[i];
        }
    }
   *this = tmp;
   return *this;
}

template<typename _T>
OneD<_T>
OneD<_T>::operator - (const OneD<_T>& p) const
{
   OneD<_T> ret(*this);
   ret -= p;
   return ret;
}

template<typename _T>
OneD<_T>&
OneD<_T>::operator /= (double cv)
{
    double recipCV = 1.0/cv;
    for (unsigned int i = 0, sz = mCoef.size() ; i < sz; i++)
    {
        mCoef[i] *= recipCV;
    }
    return *this;
}

template<typename _T>
OneD<_T>
OneD<_T>::operator / (double cv) const
{
    OneD<_T> ret(*this);
    ret *= (1.0/cv);
    return ret;
}

template<typename _T>
OneD<_T> OneD<_T>::power(size_t toThe) const
{
    // If its 0, we have to give back a 1*x^0 poly, since
    // we want a 2D poly out
    if (toThe == 0)
    {
        OneD<_T> zero(0);
        zero[0] = static_cast<_T>(1);
        return zero;
    }

    OneD<_T> rv = *this;

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
OneD<_T> OneD<_T>::scaleVariable(double scale) const
{
    return ::math::poly::scaleVariable<OneD<_T> >(*this, scale);
}

template<typename _T>
OneD<_T> OneD<_T>::truncateTo(size_t order) const
{
    order = std::min(this->order(), order);

    OneD<_T> newP(order);
    for (size_t ii = 0; ii <= order; ++ii)
    {
        newP[ii] = (*this)[ii];
    }
    return newP;
}

template<typename _T>
OneD<_T> OneD<_T>::truncateToNonZeros(double zeroEpsilon) const
{
    zeroEpsilon = std::abs(zeroEpsilon);
    size_t newOrder(0);

    // Find the highest order non-zero coefficient
    for (size_t ii = 0, idx = order(); ii <= order(); ++ii, --idx)
    {
        if (std::abs((*this)[idx]) > zeroEpsilon)
        {
            newOrder = idx;
            break;
        }
    }

    return truncateTo(newOrder);
}

template<>
inline
OneD<math::linear::VectorN<3, double> >
OneD< math::linear::VectorN<3, double> >::truncateToNonZeros(double zeroEpsilon) const
{
    zeroEpsilon = std::abs(zeroEpsilon);
    size_t newOrder(0);

    // Find the highest order non-zero coefficient
    for (size_t ii = 0, idx = order(); ii <= order(); ++ii, --idx)
    {
        if (std::abs((*this)[idx][0]) > zeroEpsilon ||
            std::abs((*this)[idx][1]) > zeroEpsilon ||
            std::abs((*this)[idx][2]) > zeroEpsilon)
        {
            newOrder = idx;
            break;
        }
    }

    return truncateTo(newOrder);
}

template<typename _T>
OneD<_T> OneD<_T>::transformInput(const OneD<_T>& gx,
                                  double zeroEpsilon) const
{
    OneD<_T> newP(order());

    for (size_t ii = 0; ii <= order(); ++ii)
    {
        newP += gx.power(ii) * (*this)[ii];
    }

    return newP.truncateToNonZeros(zeroEpsilon);
}

template<typename _T>
void OneD<_T>::copyFrom(const OneD<_T>& p)
{
    const auto numCopy = static_cast<ptrdiff_t>(std::min(size(), p.size()));
    const auto begin = p.mCoef.begin();
    const auto end = begin + numCopy;
    std::copy(begin, end, mCoef.begin());
}

template<typename _T>
template<typename Vector_T>
bool OneD<_T>::equalImpl(const Vector_T& p) const
{
    const auto sz = size();
    const auto psz = p.size();
    const auto minSize = std::min(sz, psz);

    // guard against uninitialized
    if (minSize == 0 && (sz != psz))
    {
        return false;
    }

    for (size_t ii = 0; ii < minSize; ++ii)
    {
        if (!math::linear::equals(mCoef[ii], p[ii]))
        {
            return false;
        }
    }

    _T defaultValue{};

    // Cover case where one polynomial has more
    // coefficients than the other
    if (sz > psz)
    {
        for (size_t ii = minSize; ii < sz; ++ii)
        {
            if (!math::linear::equals(mCoef[ii], defaultValue))
            {
                return false;
            }
        }
    }
    if (sz < psz)
    {
        for (size_t ii = minSize; ii < psz; ++ii)
        {
            if (!math::linear::equals(p[ii], defaultValue))
            {
                return false;
            }
        }
    }

    return true;
}

} // poly
} // math

