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
#ifndef __MATH_LINEAR_VECTOR_H__
#define __MATH_LINEAR_VECTOR_H__

#include <math/linear/Matrix2D.h>
#include <cmath>

namespace math
{
namespace linear
{
 
/*!
 *  \class Vector
 *  \brief Flexible size vector implementation
 *
 *  This class provides a resizable, non-fixed implementation
 *  of a vector.  It shares most functionality with VectorN
 *  but VectorN should always be preferred whenever the sizes
 *  are known at compile time.
 *
 *  This class implementation is also borrowed from VectorN
 *  where possible - the internal representation is still
 *  a single (2D) column vector represented as a matrix.
 *
 */
template<typename _T=double> class Vector
{
    Matrix2D<_T> mRaw;
public:

    //!  Default constructor (no initialization)
    Vector() {}

    /*!
     *  Create a vector of given size, each component
     *  initialized to a single value
     *
     *  \param sz The size of the Vector
     *  \param sv The scalar value to assign from
     */
    Vector(size_t sz, _T sv = 0.0)
    {
        mRaw = Matrix2D<_T>(sz, 1, sv);
    }

    /*!
     *  Create a vector of given size from a raw
     *  pointer (which must be pointing at an array
     *  that is at least as large as the size argument
     *
     *  \param sz The size of the Vector
     *  \param raw A raw array of values to assign
     */
    Vector(size_t sz, const _T* raw)
    {
        mRaw = Matrix2D<_T>(sz, 1, raw);
    }

    /*!
     *  Copy the contents from one Vector
     *  to another.
     *  
     *  \param v The vector to assign from
     */
    Vector(const Vector& v)
    {
        mRaw = v.mRaw;
    }

    /*!
     *  Copy the contents from a std::vector
     *  into our Vector object
     *  
     *  \param v The vector to assign from
     */
    Vector(const std::vector<_T>& v)
    {
         mRaw = Matrix2D<_T>(v.size(), 1, v);
    }

    /*!
     *  Copy the contents of a Matrix into
     *  memory.  This effectively will
     *  reshape the matrix into a single column
     *  vector.  For single column or single row
     *  matrices, the Vector will become a single
     *  column vector Matrix
     *
     *  \param mx A matrix to copy from
     */
    Vector(const Matrix2D<_T>& mx)
    {
        mRaw = Matrix2D<_T>(mx.size(), 1, mx.mRaw);
    }

    /*!
     *  Copy the contents from one Vector
     *  to another.
     *  
     *  \param v The vector to assign from
     *  \return A reference
     */
    Vector& operator=(const Vector& v)
    {
        if (this != &v)
        {
            mRaw = v.mRaw;
        }
        return *this;
    }

    /*!
     *  Copy the contents of a Matrix into
     *  memory.  This effectively will
     *  reshape the matrix into a single column
     *  vector.  For single column or single row
     *  matrices, the Vector will become a single
     *  column vector Matrix
     *
     *  \param mx A matrix to copy from
     *  \param A reference
     */
    Vector& operator=(const Matrix2D<_T>& mx)
    {
        mRaw = Matrix2D<_T>(mx.size(), 1, mx.mRaw);
        return *this;
    }

    /*!
     *  Create a vector of size 1 with a
     *  single value specified.
     *
     *  \param sv The scalar value to assign from
     *  \return The vector
     */
    Vector& operator=(const _T& sv)
    {
        mRaw = sv;
        return *this;
    }

    /*!
     *  Copy the contents from a std::vector
     *  into our Vector object
     *  
     *  \param v The vector to assign from
     *  \return A reference
     */
    Vector& operator=(const std::vector<_T>& v)
    {
        mRaw = Matrix2D<_T>(v.size(), 1, v);
        return *this;
    }
   

    //!  Destructor
    ~Vector() {}

    /*!
     *  Get the number of components in the vector
     *  \return size
     */
    size_t size() const { return mRaw.size(); }

    //!  Get back the raw matrix implementation
    Matrix2D<_T>& matrix() { return mRaw; }

    //!  Get back the const raw matrix implementation
    const Matrix2D<_T>& matrix() const { return mRaw; }

    //!  Get back a const-vector
    const _T* get() const { return mRaw.get(); }

    //!  Const dereference operator
    inline _T operator[](size_t i) const
    {
#if defined(MATH_LINEAR_BOUNDS)
        assert( i < _ND );
#endif
        return mRaw.get()[i];
    }

    //!  Non-const reference operator
    inline _T& operator[](size_t i)
    {
#if defined(MATH_LINEAR_BOUNDS)
        assert( i < _ND );
#endif
        return mRaw.mRaw[i];

    }

    /*!
     *  Compute the dot product between
     *  two vectors of equal size.  This
     *  method could just call the matrix
     *  multiply, though this method is more 
     *  efficient.
     * 
     *  \param vec Vector to dot with
     *  \return The magnitude
     */
    _T dot(const Vector& vec_) const
    {
        _T acc(0);
        size_t sz = mRaw.size();
        if (vec_.size() != sz)
            throw except::Exception(Ctxt("Dot product requires equal size vectors"));
        for (size_t i = 0; i < sz; ++i)
        {
            acc += vec_[i] * mRaw.mRaw[i];
        }
        return acc;
    }

    _T angle(const Vector& v) const
    {
        _T val = (dot(v) / norm()) / v.norm();
        return std::acos(std::max(-1.0, std::min(val, 1.0)));
    }
    
    _T normSq() const
    {
        return mRaw.normSq();
    }
    /*!
     * Euclidean, L2 norm
     */
    _T norm() const
    {

        return mRaw.norm();
    }

    //!  Normalize a value
    void normalize()
    {
        mRaw.normalize();
    }

    //!  Scale a value
    void scale(_T scalar)
    {
        mRaw.scale(scalar);
    }

    //! Unit vector, same as normalize but doesn't mutate this
    Vector unit() const
    {
        return mRaw.unit();
    }

    //!  Add this to another vector
    Vector& 
    operator+=(const Vector& v)
    {
        mRaw += v.matrix();
        return *this;
    }

    //!  Subtract another vector from this
    Vector&
    operator-=(const Vector& v)
    {
        mRaw -= v.matrix();
        return *this;
    }

    //!  Add this to another vector and return a copy
    Vector add(const Vector& v) const
    {
        Vector v2(*this);
        v2 += v;
        return v2;
    }

    //!  Subtract this from another vector and return a copy
    Vector subtract(const Vector& v) const
    {
        Vector v2(*this);
        v2 -= v;
        return v2;
    }

    //!  Overloaded plus operator
    Vector 
    operator+(const Vector& v) const
    {
        return add(v);
    }

    //!  Overloaded minus operator
    Vector
    operator-(const Vector& v) const
    {
        return subtract(v);
    }

    //!  Overloaded negation operator
    Vector
    operator-() const
    {
        Vector v(*this);
        v.mRaw = -v.mRaw;
        return v;
    }

    //!  Element-wise multiply assign from another vector
    Vector& operator *=(const Vector& v)
    {
        size_t N = size();
        for (unsigned int i = 0; i < N; i++)
        {
            mRaw.mRaw[i] *= v.mRaw.mRaw[i];
        }
        return *this;
        
    }

    //! Scalar value multiply assignment
    Vector& operator *=(_T sv)
    {
        scale(sv);
        return *this;
        
    }

    //! Scalar value assignment
    Vector operator *(_T sv) const
    {
        
        Vector v2(*this);
        v2 *= sv;
        return v2;
        
    }

    /*!
     *  Divide another vector.  This doesnt mean much
     *  geometrically by itself, but is handy for
     *  many equations
     */
    Vector& operator /=(const Vector& v)
    {
        size_t sz = size();
        for (size_t i = 0; i < sz; i++)
        {
            mRaw.mRaw[i] /= v.mRaw.mRaw[i];
        }
        return *this;
    }

    /*
     *  Multiply another vector and produce a
     *  copy
     */
    Vector operator*(const Vector& v) const
    {
        Vector v2(*this);
        v2 *= v;
        return v2;
    }

    //!  Divide anotehr vector into this and product a copy
    Vector operator/(const Vector& v) const
    {
        Vector v2(*this);
        v2 /= v;
        return v2;
    }

    /*!
     *  Flexible templated equality operator.  This allows
     *  comparisons of types other than just Vectors
     *  including fixed type VectorN and std::vector
     */
    template<typename Vector_T> bool operator_eq(const Vector_T& v) const
    {
        size_t sz = v.size();
        for (size_t i = 0; i < sz; ++i)
             if (!equals<_T>((*this)[i], v[i]))
                 return false;
             
        return true;
    }

    /*!
     *  serialize out to a boost stream
     */
    template <class Archive>
    void serialize(Archive& ar, const unsigned int  /*version*/)
    {
        ar & mRaw;
    }
};

/*!
 *  Cross product of two 3-dimensional vectors.
 *  You should never ever use this, Vector's are
 *  MUCH slower than VectorN's
 */
template<typename _T> Vector<_T> cross(const Vector<_T>& u,
                                       const Vector<_T>& v)
{
    Vector<_T> xp(3);
    xp[0] = (u[1]*v[2] - u[2]*v[1]);
    xp[1] = (u[2]*v[0] - u[0]*v[2]);
    xp[2] = (u[0]*v[1] - u[1]*v[0]);
    return xp;
}

//!  Fairly non-useful method, retained for backwards compatibility
template<typename _T> Vector<_T> 
    constantVector(size_t sz, _T cv = 0)
{
    Vector<_T> v(sz, cv);
    return v;
}

/*!
 *  Matrix-vector multiplication.  Produces
 *  a vector object.
 */

template<typename _T> 
Vector<_T>
operator*(const Matrix2D<_T>& m, const Vector<_T>& v)
{
    return Vector<_T>(m * v.matrix());
}

/*!
 *  Reverse order template overload for scalar * Vector
 */
template<typename _T> Vector<_T>
operator*(_T scalar, const Vector<_T>& v)
{
    return v * scalar;
}

template<typename Vector_T, typename T = double>
inline bool operator==(const Vector<T>& lhs, const Vector_T& rhs)
{
    return lhs.operator_eq(rhs);
}
template <typename Vector_T, typename T = double>
inline bool operator!=(const Vector<T>& lhs, const Vector_T& rhs)
{
    return !(lhs == rhs);
}

/*!
 *  Pretty(?)-print vector
 */
template<typename _T> 
std::ostream& operator<<(std::ostream& os, const Vector<_T>& v)
{
    for (size_t i = 0; i < v.size(); ++i)
    {
        os << std::setw(10) << v[i] << " ";
    }
    return os;
}
}
}

#endif
