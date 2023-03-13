/* =========================================================================
 * This file is part of math.linear-c++ 
 * =========================================================================
 * 
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
 *
 * math.linear-c++ is free software; you can redistribute it and/or modify
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
#ifndef __MATH_LINEAR_VECTOR_N_H__
#define __MATH_LINEAR_VECTOR_N_H__

#include <math/linear/MatrixMxN.h>
#include <cmath>

namespace math
{
namespace linear
{
 

template<size_t _ND, typename _T=double> class VectorN
{
    #if _MSC_VER
    __pragma(warning(push))
    __pragma(warning(disable: 26495)) // 26495: Variable '...' is uninitialized. Always initialize a member variable (type.6).
    #endif
    MatrixMxN<_ND, 1, _T> mRaw;
    #if _MSC_VER
    __pragma(warning(pop))
    #endif
    
public:
    typedef VectorN<_ND, _T> Like_T;

    //!  Default constructor (no initialization)
    VectorN() = default;
   
    /*!
     *  Create a vector of fixed size (_ND), each component
     *  initialized to a single value
     *
     *  \param sv The scalar value to assign from
     */
    VectorN(_T sv)
    {
        mRaw = sv;
    }

    /*!
     *  Vector copy from a _T pointer.
     *  This is not a pointer reference!
     *
     *  \param raw A raw pointer
     *
     */
    VectorN(const _T* raw)
    {
        mRaw = raw;
    }

    /*!
     *  Copy a vector from another vector
     *
     */
    VectorN(const VectorN& v)
    {
        mRaw = v.mRaw;
    }

    /*!
     *  Initialize from a one dimensional
     *  row vector matrix
     */
    VectorN(const MatrixMxN<_ND, 1, _T>& mx)
    {
        mRaw = mx;
    }
    /*!
     *  Copy a vector from a raw STL vector
     *  which must be sized with at least _ND
     *  elements (the rest will be ignored)
     *
     */
    VectorN(const std::vector<_T>& raw)
    {
        if (raw.size() < _ND)
            throw except::Exception(Ctxt("Not enough elements"));

        mRaw = raw;
    }

    /*!
     *  Copy from a standard STL vector of size
     *  greater or equal to _ND (the rest will
     *  be ignored).
     *
     *  \param raw A vector
     */
    VectorN& operator=(const std::vector<_T>& raw)
    {
        if (raw.size() < _ND)
            throw except::Exception(Ctxt("Not enough elements"));

        mRaw = raw;
        return *this;
    }

    /*!
     *  Construct a VectorN from a raw pointer
     *  where the raw pointer is expected to
     *  point to an array with at least _ND 
     *  elements
     *
     *  \param raw A raw pointer
     */
    VectorN& operator=(const _T* raw)
    {
        mRaw = raw;
        return *this;
    }

    /*!
     *  Assignment operator from a VectorN
     */
    VectorN& operator=(const VectorN& v)
    {
        if (this != &v)
        {
            mRaw = v.mRaw;
        }
        return *this;
    }

    /*!
     *  Assign a single scalar value into
     *  every component of this VectorN
     *
     *  \param sv Scalar value
     *  \return A reference
     */
    VectorN& operator=(const _T& sv)
    {
        mRaw = sv;
        return *this;
    }

    VectorN& operator=(const MatrixMxN<_ND, 1, _T>& mx)
    {
        mRaw = mx;
        return *this;
    }

    ~VectorN() = default;    
 
    MatrixMxN<_ND, 1, _T>& matrix() { return mRaw; }
    const MatrixMxN<_ND, 1, _T>& matrix() const { return mRaw; }

    inline _T operator[](size_t i) const noexcept
    {
#if defined(MATH_LINEAR_BOUNDS)
        assert( i < _ND );
#endif
        return mRaw[i][0];
    }
    inline _T& operator[](size_t i) noexcept
    {
#if defined(MATH_LINEAR_BOUNDS)
        assert( i < _ND );
#endif
        return mRaw[i][0];

    }

    inline size_t size() const noexcept { return _ND; }

    _T dot(const VectorN<_ND>& vec) const
    {
        _T acc(0);
        for (size_t i = 0; i < _ND; ++i)
        {
            acc += (*this)[i] * vec[i];
        }
        return acc;
    }

    //!  Compute normalized dot product 
    _T normDot(const VectorN<_ND>& vec) const
    {
        // We should be able to normalize the vectors first, then take the
        // dot product.  However, due numerical precision we could still end
        // up outsize the [-1, 1] range.

        const _T dotProduct = dot(vec) / (norm() * vec.norm());
        return std::max<_T>(std::min<_T>(dotProduct, 1.0), -1.0);
    }

    /*!
     * Euclidean, L2 norm
     */
    _T norm() const
    {
        return mRaw.norm();
    }
    /*
     * Square of the Euclidean norm,
     */
    _T normSq() const
    {
        return mRaw.normSq();
    }
    /*
     * Angle between this vector and v
     */
    _T angle(const Like_T& v) const
    {
        _T val = (dot(v) / norm()) / v.norm();
        return std::acos(std::max(-1.0, std::min(val, 1.0)));
    }

    void normalize()
    {
        mRaw.normalize();
    }

    //! Unit vector, same as normalize() but doesn't mutate this
    Like_T unit() const
    {
        return mRaw.unit();
    }

    void scale(_T scalar)
    {
        mRaw.scale(scalar);
    }

    Like_T& 
    operator+=(const Like_T& v)
    {
        mRaw += v.matrix();
        return *this;
    }
    Like_T&
    operator-=(const Like_T& v)
    {
        mRaw -= v.matrix();
        return *this;
    }

    Like_T add(const Like_T& v) const
    {
        Like_T v2(*this);
        v2 += v;
        return v2;
    }

    Like_T subtract(const Like_T& v) const
    {
        Like_T v2(*this);
        v2 -= v;
        return v2;
    }

    Like_T 
    operator+(const Like_T& v) const
    {
        return add(v);
    }
    Like_T
    operator-(const Like_T& v) const
    {
        return subtract(v);
    }

    Like_T
    operator-() const
    {
        Like_T v(*this);
        v.mRaw = -v.mRaw;
        return v;
    }

    Like_T& operator *=(const Like_T& v)
    {
        for (size_t i = 0; i < _ND; i++)
        {
            mRaw(i, 0) *= v[i];
        }
        return *this;
        
    }

    Like_T& operator *=(_T sv)
    {
        scale(sv);
        return *this;
        
    }

    Like_T operator *(_T sv) const
    {
        
        Like_T v2(*this);
        v2 *= sv;
        return v2;
        
    }


    Like_T& operator /=(const Like_T& v)
    {
        for (size_t i = 0; i < _ND; i++)
        {
            mRaw(i, 0) /= v[i];
        }
        return *this;
    }

    Like_T operator*(const Like_T& v) const
    {
        Like_T v2(*this);
        v2 *= v;
        return v2;
    }

    Like_T operator/(const Like_T& v) const
    {
        Like_T v2(*this);
        v2 /= v;
        return v2;
    }

    template<typename Vector_T> bool operator_eq(const Vector_T& v) const
    {
        const auto sz = v.size();
        for (size_t i = 0; i < sz; ++i)
            if (!equals<_T>((*this)[i], v[i])) 
                return false;

      
        return true;
    }

};

template<typename _T> VectorN<3, _T> cross(const VectorN<3, _T>& u,
                                           const VectorN<3, _T>& v)
{
    VectorN<3, _T> xp;
    xp[0] = (u[1]*v[2] - u[2]*v[1]);
    xp[1] = (u[2]*v[0] - u[0]*v[2]);
    xp[2] = (u[0]*v[1] - u[1]*v[0]);
    return xp;
}

template<size_t _ND, typename _T> VectorN<_ND, _T> 
    constantVector(_T cv = 0)
{
    VectorN<_ND, _T> v(constantMatrix<_ND, 1, _T>(cv));
    return v;
}

template<size_t _MD, size_t _ND, typename _T> 
    math::linear::VectorN<_MD, _T>
    operator*(const math::linear::MatrixMxN<_MD, _ND, _T>& m, 
              const math::linear::VectorN<_ND, _T>& v)
{
    return math::linear::VectorN<_MD, _T>(m * v.matrix());
}

template<size_t _ND, typename _T> math::linear::VectorN<_ND, _T>
    operator*(_T scalar, const math::linear::VectorN<_ND, _T>& v)
{
    return v * scalar;
}


template<size_t _ND, typename _T> 
    std::ostream& operator<<(std::ostream& os,
                             const math::linear::VectorN<_ND, _T>& v)
{
    for (size_t i = 0; i < _ND; ++i)
    {
        os << std::setw(10) << v[i] << " ";
    }
    return os;
}

template <typename Vector_T, size_t ND, typename T = double>
inline bool operator==(const VectorN<ND, T>& lhs, const Vector_T& rhs)
{
    return lhs.operator_eq(rhs);
}
template <typename Vector_T, size_t ND, typename T = double>
inline bool operator!=(const VectorN<ND, T>& lhs, const Vector_T& rhs)
{
    return !(lhs == rhs);
}

} // linear
} // math

#endif
