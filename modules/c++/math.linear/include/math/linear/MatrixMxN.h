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
#ifndef CODA_OSS_math_linear_MatrixMxN_h_INCLUDED_
#define CODA_OSS_math_linear_MatrixMxN_h_INCLUDED_
#pragma once

#include <cmath>
#include <limits>
#include <complex>
#include <iomanip> // std::setw()

#include <import/sys.h>

namespace math
{
namespace linear
{

// Create a safe comparison
template<typename _T> inline bool equals(const _T& e1, const _T& e2)
{
    return e1 == e2;
}

template<typename _T> inline bool equals(const _T& e1, const _T& e2, _T eps)
{
    if (std::numeric_limits<_T>::has_infinity)
    {
        if (e1 == std::numeric_limits<_T>::infinity() ||
            e1 == -std::numeric_limits<_T>::infinity())
        {
            return e1 == e2;
        }
    }
    return std::abs(e1 - e2) < eps;
}

template<> inline bool equals(const float& e1, const float& e2)
{
    return equals<float>(e1, e2, std::numeric_limits<float>::epsilon());
}
template<> inline bool equals(const double& e1, const double& e2)
{
    // It's a really bold assertion here to say numeric_limits<double>
    return equals<double>(e1, e2, std::numeric_limits<float>::epsilon());
}

/*!
 *  Note that for double's this will differ from using equals(0, value)
 *  since we'll use numeric_limits<double>::epsilon() here.
 *  We're trying to assess if something is close to 0, and using epsilon()
 *  is in some sense just picking a magic number, but it's a number that
 *  will force double's to be closer to 0 than float's.  Had problems in
 *  the past using equals(0, value) for double's with the matrix inversion
 *  logic below when the determinant was close to 0 but still valid but
 *  equals was returning true.
 */
template <typename T>
bool almostZero(const std::complex<T>& value)
{
    return (std::abs(value) < std::numeric_limits<T>::epsilon());
}
template <typename T>
bool almostZero(const T& value)
{
    return (std::abs(value) < std::numeric_limits<T>::epsilon());
}

/*!
 *  \class MatrixMxN
 *  \brief Compile-time fixed size Matrix template
 *
 *  This class provides a compile time fixed size class of arbitrary
 *  dimensions M and N.  Any invocation of this class is therefore required
 *  to know the absolute size of the matrix at compile time.  This class should
 *  always be used instead of Matrix2D when possible, as it should perform much
 *  better than its flexible-sized alternative. Use Matrix2D only when the matrix
 *  dimensions cannot be known at compile time
 *
 *  Attempts are made where possible to take advantage of the fixed size,
 *  providing full specialization where possible.  Additionally, it is
 *  expected that, during compilation loops may be unrolled by the compiler,
 *  given the right optimization settings.  To faciliate compiler optimizations,
 *  the internal storage of the class is as a two-dimensional vector,
 *  following the Rationale presented in the AMD Software Optimization Guide
 *  for AMD64 Processors 25112 Rev. 3.06 (see chapter 2.2).  Over time, the
 *  headers will be extended to support LAPACK implementation routine
 *
 *  The class attempts to provide an intuitive interface for matrices, while
 *  achieving reasonable performance and optimizations where possible.  It does
 *  not attempt to provide a comprehensive solution for every matrix problem,
 *  but it does try to handle practical problems gracefully.  Efforts are made
 *  to make this class compatible with STL and raw C pointers where possible.
 *
 *  The doxygen attempts to demonstrate the functionality of the class, though the
 *  source code and unit tests should be consulted for example usage as well.
 *
 */
template <size_t _MD, size_t _ND, typename _T=double>
class MatrixMxN
{
public:
    typedef MatrixMxN<_MD, _ND, _T> Like_T;

    //!  Public but really should be avoided
    _T mRaw[_MD][_ND]{};

    MatrixMxN() = default;

    /*!
     *  Create a matrix with a constant value for
     *  each element.
     *
     *  \param cv A constant value (defaults to 0)
     *
     *  \code
           MatrixMxN<2, 2> mx(4.2);
     *  \endcode
     *
     */
    MatrixMxN(_T cv)
    {
        for (size_t i = 0; i < rows(); i++)
        {
            for (size_t j = 0; j < cols(); j++)
            {
                mRaw[i][j] = cv;
            }
        }
    }


    /*!
     *  Construct a matrix from a 1D raw M*N pointer.
     *  Assumes that the pointer is of correct size.
     *
     *  \code
          double raw9[] = 
          {
             1, 2, 3
             4, 5, 6,
             7, 8, 8
          };
          MatrixMxN<3, 3> A(raw9);
     *  \endcode
     *  
     *  \param raw A raw pointer to copy internally
     */
private:
    template<typename TVectorLike>
    void assign_from_raw(const TVectorLike& raw)
    {
        for (size_t i = 0; i < rows(); i++)
        {
            for (size_t j = 0; j < cols(); j++)
            {
                mRaw[i][j] = raw[i * cols() + j];
            }
        }
    }
public:
    MatrixMxN(const _T* raw)
    {
        assign_from_raw(raw);
    }

    /*!
     *  Construct a matrix from a 1D M*N vector.
     *
     *  \code
          std::vector<double> vec9(9, 42.0);
          MatrixMxN<3, 3> A(vec9);
     *  \endcode
     *
     *
     */
    MatrixMxN(const std::vector<_T>& raw)
    {
        if (raw.size() < size())
            throw except::Exception(Ctxt("Invalid size exception"));
        assign_from_raw(raw);
    }

    /*!
     *  Supports explicit assignment from
     *  one matrix to another
     *
     *
     *  \param mx A constant matrix to copy
     *
     *  \code
          MatrixMxN<3, 3> At(A.transpose());
     *  \endcode
     */
    MatrixMxN(const MatrixMxN& mx)
    {
        for (size_t i = 0; i < rows(); i++)
        {
            for (size_t j = 0; j < cols(); j++)
            {
                mRaw[i][j] = mx.mRaw[i][j];
            }
        }
    }
    /*!
     *  Assign a matrix from a 1D raw M*N pointer.
     *  Assumes that the pointer is of correct size.
     *
     *  \code
          double raw9[] = 
          {
             1, 2, 3
             4, 5, 6,
             7, 8, 8
          };
          MatrixMxN<3, 3> A = raw9;
     *  \endcode
     *  
     *  \param raw A raw pointer to copy internally
     */
    MatrixMxN& operator=(const _T* raw)
    {
        assign_from_raw(raw);
        return *this;
    }

    /*!
     *  Assign a matrix from a 1D M*N vector.
     *
     *  \code
          std::vector<double> vec9(9, 42.0);
          MatrixMxN<3, 3> A = vec9;
     *  \endcode
     *
     *
     */
    MatrixMxN& operator=(const std::vector<_T>& raw)
    {
        if (raw.size() < size())
            throw except::Exception(Ctxt("Invalid size exception"));
        assign_from_raw(raw);
        return *this;
    }

 
    /*!
     *  Assignment operator from one matrix to another
     *
     *  \code
          MatrixMxN<3, 3> At = A.transpose();
     *  \endcode
     *
     *  \param mx The source matrix
     *  \return this (the copy)
     */
    MatrixMxN& operator=(const MatrixMxN& mx)
    {
        if (this != &mx)
        for (size_t i = 0; i < rows(); i++)
        {
            for (size_t j = 0; j < cols(); j++)
            {
                mRaw[i][j] = mx.mRaw[i][j];
            }
        }
        return *this;
    }

    /*!
     *  Set a matrix (each element) to the contents
     *  of a scalar value.  Be careful to make sure
     *  that the compiler knows that the right-hand side
     *  is of matching type to this matrix, otherwise
     *  it may not be able to determine if this is the
     *  scalar operation or a raw pointer;
     *
     *  \code
           Matrix<3, 3, float> mx = 4.3f;
     *  \endcode
     *
     */
    MatrixMxN& operator=(const _T& sv)
    {
        for (size_t i = 0; i < rows(); i++)
        {
            for (size_t j = 0; j < cols(); j++)
            {
                mRaw[i][0] = sv;
            }
        }
        return *this;
    }

    ~MatrixMxN() = default;


    /*!
     *  Get back the value at index i, j
     *  \code
           double Aij = A(i, j);
     *  \endcode
     *
     *  \param i The row index
     *  \param j The column index
     */
    inline const _T& operator()(size_t i, size_t j) const noexcept
    {
#if defined(MATH_LINEAR_BOUNDS)
        assert( i < rows() && j < cols() );
#endif
        return mRaw[i][j];
    }
    /*!
     *  This operator allows you to mutate an element
     *  at A(i, j):
     *  
     *  \code
           A(i, j) = 4.3;
     *  \endcode
     *
     *  \param i The ith index into the rows (M)
     *  \param j The jth index into the cols (N)
     */
    inline _T& operator()(size_t i, size_t j) noexcept
    {
#if defined(MATH_LINEAR_BOUNDS)
        assert( i < rows() && j < cols() );
#endif
        return mRaw[i][j];
    }

    /*!
     *  Please try to avoid this method.  There is extensive information
     *  here about why you should not use it:
     *  http://www.parashift.com/c++-faq-lite/operator-overloading.html#faq-13.10
     *  http://www.parashift.com/c++-faq-lite/operator-overloading.html#faq-13.11
     */
    inline const _T* operator[](size_t i) const noexcept
    {
        return row(i);
    }

    /*!
     *  Never use this method!  It should not be used for the same reasons as
     *  its const-alternative:
     *  http://www.parashift.com/c++-faq-lite/operator-overloading.html#faq-13.10
     *  http://www.parashift.com/c++-faq-lite/operator-overloading.html#faq-13.11
     *
     *  But it is even more dangerous, since the user can cause damage by unwittingly
     *  treating row i as a mutable pointer.  This method is only preserved for compatibility
     */
    inline _T* operator[](size_t i) noexcept
    { 
        return row(i);
    }

    /*!
     *  Get a constant pointer to a row
     *  
     *  \code
          // Get second row vector
          const double* rowVector = A.row(1);
     *  \endcode
     *
     */
    inline const _T* row(size_t i) const noexcept
    {
#if defined(MATH_LINEAR_BOUNDS)
        assert( i < rows());
#endif
        return mRaw[i];
    }

    /*!
     *  Never use this method!  This method is dangerous
     *  since the user can cause damage by unwittingly
     *  treating row i as a mutable pointer.
     */
    inline _T* row(size_t i) noexcept
    {
#if defined(MATH_LINEAR_BOUNDS)
        assert( i < rows());
#endif
        return mRaw[i];
    }

    /*!
     *  Set the matrix row i to a copy of 
     *  the row vector
     *
     *  \code
          Matrix<3, 3> A(42.0);
          double rowVec[] = { 1, 2, 3 };
          A.row(0, rowVec);
     *  \endcode
     *  
     *  \param i The row index
     *  \param vec The row vector to copy from
     */
    inline void row(size_t i, const _T* vec)
    {
        for (size_t j = 0; j < cols(); j++)
        {
            mRaw[i][j] = vec[j];
        }
    }

    /*!
     *  Set the matrix row i to a copy of 
     *  the row vector
     *  
     *  \code
          Matrix<3, 3> A(42.0);
          std::vector<double> rowVec(3, 1.2);
          A.row(0, rowVec);
     *  \endcode
     *
     *  \param i The row index
     *  \param vec The row vector to copy from
     */
    inline void row(size_t i, const std::vector<_T>& vec)
    {
        if (!vec.empty())
        {
            row(i, &vec[0]);
        }
    }

  
    /*!
     *  Get back the column vector at index j
     *  
     *  \code
          std::vector<double> colVec = A.col(0);
     *  \endcode
     *
     *  \param j The column index
     *  \return A vector copy of the column
     */
    std::vector<_T> col(size_t j) const
    {
        std::vector<_T> jth(rows());
        for (size_t i = 0; i < rows(); ++i)
        {
            jth[i] = mRaw[i][j];
        }
        return jth;
    }


    /*!
     *  Set column from vector at index j
     *
     *  \code
          Matrix<3, 3> A(42.0);
          double colVec[] = { 1, 2, 3 };
          A.col(0, colVec);
     *  \endcode
     *
     *  \param j The column index
     *  \param vec The vector to copy from
     */
    void col(size_t j, const _T* vec)
    {
        for (size_t i = 0; i < rows(); ++i)
        {
            mRaw[i][j] = vec[i];
        }
    }

    /*!
     *  Set column from vector at index j
     *
     *  \code
          Matrix<3, 3> A(42.0);
          std::vector<double> colVec(3, 1.2);
          A.col(0, colVec);
     *  \endcode
     *
     *  \param j The column index
     *  \param vec The vector to copy from
     */
    void col(size_t j, const std::vector<_T>& vec)
    {
        if (!vec.empty())
        {
            col(j, &vec[0]);
        }
    }

    /*!
     *  Set column from matrix at index colIdx
     *
     *  \code
          Matrix<3, 3> A(42.0);
          Matrix<3, 1> colVec(31.0);
          A.col(0, colVec);
     *  \endcode
     *
     *  \param colIdx The column index
     *  \param vec The matrix to copy from
     */
     void col(size_t colIdx, const MatrixMxN<_MD, 1, _T>& vec)
     {
         for (size_t row = 0; row < rows(); ++row)
         {
             mRaw[row][colIdx] = vec(row, 0);
         }
     }


    /*!
     *  This function is not really necessary since presumably
     *  the caller knows the dims, but it is helpful for
     *  template functions where the exact matrix type is
     *  not know (e.g., MatrixMxN vs. Matrix2D
     *
     *  It is assumed that the compiler, in most cases can
     *  hardcode in the proper value of _MD, though this has
     *  not been verified
     *
     *  \return _MD
     */
    constexpr size_t rows() const noexcept { return _MD; }
    
    /*!
     *  This function is not really necessary, but
     *  might be handy if you have some template code
     *  
     *  It is assumed that the compiler, in most cases can
     *  hardcode in the proper value of _ND, though this has
     *  not been verified
     *
     *  \return _ND
     */
    constexpr size_t cols() const noexcept { return _ND; }

    /*!
     *  Gives back the value full size of the matrix
     *
     *  \return _MD * cols()
     */
    constexpr size_t size() const noexcept { return rows() * cols(); }



    /*!
     *  Equality operator test
     *
     *  \code

          double raw4[] = { 1, 2, 3, 4 };
          Matrix<2, 2> A(raw4);
          Matrix2D<double> B(2, 2, raw4);
          Matrix<2, 2> C = A;
          assert( A == B );
          assert( A == C );

     *  \endcode
     *
     *  \param mx The source matrix
     *  \return this (the copy)
     */
           
    template<typename Matrix_T> inline bool operator==(const Matrix_T& mx) const
    {
        
        if (rows() != mx.rows() || cols() != mx.cols())
            return false;

        for (size_t i = 0; i < rows(); ++i)
        {
            for (size_t j = 0; j < cols(); ++j)
            {
                if (! equals(mRaw[i][j], mx(i, j)))
                    return false;
            }
        }
        return true;
    }
    
    /*!
     *  Non-equality operator test
     *
     *  \code

          double raw4[] = { 1, 2, 3, 4 };
          Matrix<2, 2> A(raw4);
          Matrix2D<double> B(2, 2, raw4);
          B(1, 1) = 42.0;
          Matrix<2, 2> C(B.vec());
          assert( A != B );
          assert( A != C );

     *  \endcode
     *
     *  \param mx The source matrix
     *  \return this (the copy)
     */

    template<typename Matrix_T> inline bool operator!=(const Matrix_T& mx) const
    {
            return !(*this == mx);
    }

    /*!
     *  The scale function allows you to scale
     *  the matrix by a scalar value in-place.
     *  
     *  If you can afford to mutate the matrix,
     *  this will be more efficient than its
     *  multiply counterpart
     *
     *  \code
           Matrix<3, 3> mx = createIdentity<3, double>();
           mx.scale(4.2f);   
     *  \endcode
     *
     *
     *  \param scalar The value to multiply into mx
     *  \return This object
     *
     */
    MatrixMxN& scale(_T scalar)
    {
        for (size_t i = 0; i < rows(); i++)
        {
            for (size_t j = 0; j < cols(); j++)
            {
                mRaw[i][j] *= scalar;
            }
        }
        return *this;
    }

    /*!
     *  Does a matrix-scalar multiplication.  The
     *  output is a matrix of same dimensions.  This
     *  function is identical to the scale() method
     *  except that this matrix is not mutated -
     *  a scale copy is produced and returned.  Note that
     *  it is never necessary to do this function directly,
     *  as the multiply ('*') operator is overloaded
     *  
     *  \code
           scaled = mx.multiply(scalar);
     *  \endcode
     *
     *  \param scalar A scalar value
     *  return a scaled matrix
     *
     */
    MatrixMxN<_MD, _ND> multiply(_T scalar) const
    {
        MatrixMxN<_MD, _ND> mx = *this;
        for (size_t i = 0; i < rows(); i++)
        {
            for (size_t j = 0; j < cols(); j++)
            {
                mx[i][j] *= scalar;
            }
        }
        return mx;
    }

    /*!
     *  Multiply an NxP matrix to a MxN matrix (this) to
     *  produce an MxP matrix output.  This function will
     *  fail at compile time if the input and output matrix
     *  dimensions are incorrect.
     *
     *  This function accesses the inner arrays for
     *  (potential, though slight) performance reasons.
     *
     *  One would hope that the compiler will unroll these
     *  loops since they are of constant size.
     *
     *  \param mx An NxP matrix
     *  \return An MxP matrix
     *
     *
     *  \code
           MatrixMxN<3, 1> A(42.0);
           MatrixMxN<1, 3> B(1.0);
           MatrixMxN<3, 3> C(A.multiply(B));
           MatrixMxN<1, 1> D(B.multiply(A));
     *  \endcode
     *
     */
    template<size_t _PD> MatrixMxN<_MD, _PD, _T>
        multiply(const MatrixMxN<_ND, _PD, _T>& mx) const
    {
        MatrixMxN<_MD, _PD, _T> newM{};

        for (size_t i = 0; i < rows(); i++)
        {
            for (size_t j = 0; j < _PD; j++)
            {
                newM.mRaw[i][j] = 0;
                for (size_t k = 0; k < cols(); k++)
                {
                    newM.mRaw[i][j] += mRaw[i][k] * mx.mRaw[k][j];
                }
            }
        }
        return newM;

    }


    /*!
     *  Take in a matrix that is NxN and apply each diagonal
     *  element to the column vector.  This method mutates this,
     *  but returns it out as well.
     *
     *  For each column vector in this, multiply it by the scalar
     *  diagonal value then assign the result.
     *
     *  \param mx An NxN matrix whose diagonals scale the columns
     *  \return A reference to this.
     *
     *  \code
           C = A.scaleDiagonal(diagonalMatrix);
     *  \endcode
     *
     *
     */
    MatrixMxN& scaleDiagonal(const MatrixMxN<_ND, _ND, _T>& mx)
    {
        size_t i, j;
        for (i = 0; i < rows(); i++)
        {
            for (j = 0; j < cols(); j++)
            {
                mRaw[i][j] *= mx.mRaw[j][j];
            }
        }
        return *this;
    }

    /*!
     *  This function is the same as scaleDiagonal except that
     *  it does not mutate this (it makes a copy and then calls
     *  that function on the copy).
     *
     *  \param mx An NxN matrix whose diagonals scale the columns
     *  \return a copy matrix
     *  
     *  \code
           C = A.multiplyDiagonal(diagonalMatrix);
     *  \endcode
     *
     *
     */
    MatrixMxN<_MD, _ND, _T>
        multiplyDiagonal(const MatrixMxN<_ND, _ND, _T>& mx) const
    {
        MatrixMxN<_MD, _ND, _T> newM = *this;
        newM.scaleDiagonal(mx);
        return newM;
    }

    /*!
     *  This function does an add and accumulate
     *  operation.  The parameter is add-assigned
     *  element-wise to this.  
     *
     *  This method generates a
     *  compile time error if the matrix dimensions 
     *  do not agree
     *
     *  \param mx The matrix to assign (MxN)
     *  \return This
     *     
     *  \code
           A += B;
     *  \endcode
     *
     */
    Like_T&
    operator+=(const Like_T& mx)
    {
        for (size_t i = 0; i < rows(); i++)
        {
            for (size_t j = 0; j < cols(); j++)
            {
                mRaw[i][j] += mx.mRaw[i][j];
            }
        }
        return *this;
    }

    /*!
     *  This function does a subtraction
     *  operation element wise.
     *
     *  This method generates a
     *  compile time error if the matrix dimensions 
     *  do not agree
     *
     *  \param mx MxN matrix to subtract from this
     *  \return This
     *
     *  \code
           A -= B;
     *  \endcode
     *
     */
    Like_T&
    operator-=(const Like_T& mx)
    {
        for (size_t i = 0; i < rows(); i++)
        {
            for (size_t j = 0; j < cols(); j++)
            {
                mRaw[i][j] -= mx(i, j);
            }
        }
        return *this;

    }

    /*!
     *  Add an MxN matrix to another and return a third
     *  that is the sum.  This operation does not mutate this.
     *  You can use the overloaded ('+') operator instead
     *
     *  This method generates a
     *  compile time error if the matrix dimensions 
     *  do not agree
     *
     *  \param mx
     *  \return The sum
     *
     *  \code
           C = A.add(B);
     *  \endcode
     *
     */
    Like_T add(const Like_T& mx) const
    {
        Like_T newM = *this;
        newM += mx;
        return newM;
    }

    /*!
     *  Add an MxN matrix to this matrix starting at position (row, col)
     *
     *  This method assumes the MxN matrix fits inside this matrix
     *
     *  \param rhs The matrix to add to this
     *  \param rowOffset The row offset when indexing into this matrix
     *  \param colOffset The column offset when indexing into this matrix
     *
     *  \code
     *     MatrixMxN<3,3> A(42.0);
     *     MatrixMxN<2,2> B(31.0);
     *     A.addInPlace(B,1,1);
     *  \endcode
     *
     */
     template <size_t _Mmx, size_t _Nmx, typename _Tmx>
     void addInPlace(const MatrixMxN<_Mmx, _Nmx, _Tmx>& rhs,
                     size_t rowOffset = 0,
                     size_t colOffset = 0)
     {
         for (size_t row = 0; row < _Mmx; ++row)
         {
             for (size_t col = 0; col < _Nmx; ++col)
             {
                 mRaw[row + rowOffset][col + colOffset] += rhs(row, col);
             }
         }
     }

    /*!
     *  Subtract an MxN matrix to another and return a third
     *  that is the sum.  This operation does not mutate this.
     *  You can use the overloaded ('-') operator instead
     *
     *  This method generates a
     *  compile time error if the matrix dimensions 
     *  do not agree
     *
     *  \param mx
     *  \return The sum
     *
     *  \code
           C = A.subtract(B);
     *  \endcode
     *
     */

    Like_T subtract(const Like_T& mx) const
    {
        Like_T newM = *this;
        newM -= mx;
        return newM;
    }

    /*!
     *  Create a NxM matrix which is the transpose of this
     *  MxN matrix.  This method generates a compiler error
     *  if the output matrix dimensions are not NxM
     *
     *  \return An NxM matrix that is the transpose
     *
     *  \code
           B = A.tranpose();
     *  \endcode
     *
     */
    MatrixMxN<_ND, _MD, _T> transpose() const
    {

        MatrixMxN<_ND, _MD, _T> x{};
        for (size_t i = 0; i < rows(); i++)
            for (size_t j = 0; j < cols(); j++)
                x.mRaw[j][i] = mRaw[i][j];

        return x;
    }

    /*!
     *  Does LU decomposition on a matrix.
     *  In order to do this efficiently, we get back
     *  as a return value a copy of the matrix that is
     *  decomposed, but we also produce the pivots for
     *  permutation.  This function is used for the generalized
     *  inverse.
     *
     *  This function is based on the TNT LU decomposition
     *  function.
     *
     *  \param [out] pivotsM
     *
     */
    Like_T decomposeLU(std::vector<size_t>& pivotsM) const
    {

        Like_T lu;

        for (size_t i = 0; i < rows(); i++)
        {
            // Start by making our pivots unpermuted
            pivotsM[i] = i;
            for (size_t j = 0; j < cols(); j++)
            {
                // And copying elements
                lu(i, j) = mRaw[i][j];
            }
        }


        std::vector<_T> colj(_MD);
        _T* rowi;
        for (size_t j = 0; j < cols(); j++)
        {
            for (size_t i = 0; i < rows(); i++)
            {
                colj[i] = lu(i, j);
            }

            for (size_t i = 0; i < rows(); i++)
            {
                rowi = lu[i];

                size_t kmax = std::min<size_t>(i, j);
                _T s(0);
                for (size_t k = 0; k < kmax; k++)
                {
                    s += rowi[k] * colj[k];
                }
                colj[i] -= s;
                rowi[j] = colj[i];
            }

            size_t p = j;
            for (size_t i = j + 1; i < rows(); i++)
            {
                if (std::abs(colj[i]) > std::abs(colj[p]))
                    p = i;

            }
            if (p != j)
            {
                size_t k = 0;
                for (; k < cols(); k++)
                {
                    // We are swapping
                    _T t = lu(p, k);
                    lu(p, k) = lu(j, k);
                    lu(j, k) = t;
                }
                k = pivotsM[p];
                pivotsM[p] = pivotsM[j];
                pivotsM[j] = k;
            }
            if (j < rows() && std::abs( lu(j, j) ))
            {
                for (size_t i = j + 1; i < rows(); i++)
                {
                    // Divide out our rows
                    lu(i, j) /= lu(j, j);
                }
            }

        }

        return lu;
    }

    /*!
     *  Permute a matrix from pivots.  This funtion does
     *  not mutate this.
     *
     *  \param pivotsM The M pivots vector
     *  \param n The number of columns (defaults to this' N)
     *  \return A copy of this matrix that is permuted
     *
     *  \code
           int p[2] = { 1, 0 };
           MatrixMxN<2,2> G = F.permute(P);
     *  \endcode
     *
     */
    Like_T permute(const std::vector<size_t>&  pivotsM, size_t n = _ND) const
    {
        Like_T perm;
        for (size_t i = 0; i < rows(); i++)
        {
            for (size_t j = 0; j < n; j++)
            {
                perm[i][j] = mRaw[pivotsM[i]][j];
            }
        }
        return perm;
    }
    
    /*
     * Find the square of the L2 norm
     * Sum of squares of the vector elements
     */
    _T normSq() const
    {
        _T acc(0);
        for (size_t i = 0; i < rows(); ++i)
        {
            for (size_t j = 0; j < cols(); ++j)
            {
                acc += mRaw[i][j] * mRaw[i][j];
            }
        }
        return acc;
    }
    
    /*!
     *  Find the L2 norm of the matrix.
     *  \return The norm
     */
    _T norm() const
    {
        return static_cast<_T>(::sqrt(normSq()));
    }
    
    /*!
     *  Scale the entire matrix inplace by the L2 norm value.
     *  \return A reference to this
     */
    MatrixMxN& normalize()
    {
        return scale(1.0/norm());
    }

    /*!
     *  Scale the matrix by the L2 norm value. This
     *  function performs the same operation as normalize()
     *  except that this matrix is not mutated.
     */
    Like_T unit() const
    {
        return multiply(1.0/norm());
    }

    /*!
     *  Alias for this->add();
     *
     *  \code
           C = A + B;
     *  \endcode
     *
     */
    Like_T operator+(const Like_T& mx) const
    {
        return add(mx);
    }


    /*!
     *  Alias for this->subtract();
     *
     *  \code
           C = A - B;
     *  \endcode
     *
     */
    Like_T operator-(const Like_T& mx) const
    {
        return subtract(mx);
    }

    /*!
     *  Alias for this->multiply(scalar);
     *
     *  \code
           scaled = A * scalar;
     *  \endcode
     *
     */
    Like_T operator*(_T scalar) const
    {

        return multiply(scalar);
    }

    /*!
     *  Alias for this->multiply(1/scalar);
     *
     *  \code
           scaled = A / scalar;
     *  \endcode
     *
     */
    Like_T operator/(_T scalar) const
    {

        return multiply(1/scalar);
    }

    /*!
     *  Alias for this->multiply(NxP);
     *
     *  \code
           C = A * B;
     *  \endcode
     */
    template<size_t _PD>
    MatrixMxN<_MD, _PD, _T>
        operator*(const MatrixMxN<_ND, _PD, _T>& mx) const
    {
        return multiply(mx);
    }
    
    /*!
     *  Negation operator;
     *
     *  \code
           B = -A;
     *  \endcode
     *
     */
    Like_T operator-() const
    {
        Like_T neg{};
        for (size_t ii = 0; ii < rows(); ++ii)
        {
            for (size_t jj = 0; jj < cols(); ++jj)
            {
                neg.mRaw[ii][jj] = -mRaw[ii][jj];
            }
        }
        return neg;
    }

};

// A = LU
// A*x = b(piv,:)
// A<M, N>, x<N, P>, b<M, P>

/*!
 *  This function creates a matrix of size MxN and type _T
 *  with a constant value in each element (argument defaults to 0)
 *
 *  This function produces a matrix that is set to this value
 *
 *  \code
       MatrixMxN<4, 4> mx = constantMatrix<4, 4, double>(4.2)
 *  \endcode
 *
 *  This function is relatively worthless, favor the constructor
 *
 *  \param cv An optional constant value
 */
template<size_t _MD, size_t _ND, typename _T> MatrixMxN<_MD, _ND, _T>
    constantMatrix(_T cv = 0)
{
    return MatrixMxN<_MD, _ND, _T>(cv);
}

/*!
 *  This function creates an identity matrix of size NxN and type _T
 *  with 1's in the diagonals.
 *
 *  \code
        MatrixMxN<4, 4> = identityMatrix<4, 4, double>();
 *  \endcode
 *
 */
template<size_t _ND, typename _T> MatrixMxN<_ND, _ND, _T>
    identityMatrix()
{
    MatrixMxN<_ND, _ND, _T> mx{};
    for (size_t i = 0; i < _ND; i++)
    {
        for (size_t j = 0; j < _ND; j++)
        {
            mx(i, j) = (i == j) ? static_cast<_T>(1): static_cast<_T>(0);
        }
    }
    return mx;
}


/*!
 *  Solve  Ax = b using LU decomposed matrix and the permutation vector.
 *  Method based on TNT
 *
 */
template<size_t _MD, size_t _ND, size_t _PD, typename _T>
    math::linear::MatrixMxN<_ND, _PD, _T> solveLU(const std::vector<size_t>& pivotsM,
                                                  const MatrixMxN<_MD, _ND, _T> &lu,
                                                  const MatrixMxN<_ND, _PD, _T> &b)
{
    // If we dont have something in the diagonal, we can't solve this
    math::linear::MatrixMxN<_ND, _PD, _T> x = b.permute(pivotsM, _PD);

    for (size_t k = 0; k < _ND; k++) 
    {
        for (size_t i = k + 1; i < _ND; i++) 
        {
            for (size_t j = 0; j < _PD; j++) 
            {
                x(i, j) -= x(k, j)*lu(i, k);
            }
        }
    }
    for (sys::SSize_T k = _ND - 1; k >= 0; k--) 
    {
        for (size_t j = 0; j < _PD; j++) 
        {
            x(k, j) /= lu(k, k);
        }

        for (sys::SSize_T i = 0; i < k; i++) 
        {
            // This one could be _Q
            for (size_t j = 0; j < _PD; j++) 
            {
                x(i, j) -= x(k, j)*lu(i, k);
            }
        }
    }

    return x;
}

/*!
 *  Generalized inverse method using LU decomposition
 *
 *  \param mx A matrix to invert
 *
 *  \code      
         Matrix<3, 3> Ainv = inverseLU<3, double>(A);
 *  \endcode
 *
 */
template<size_t _ND, typename _T> inline
    MatrixMxN<_ND, _ND, _T> inverseLU(const MatrixMxN<_ND, _ND, _T>& mx)
{
    MatrixMxN<_ND, _ND, _T> a(static_cast<_T>(0));

    // Identity
    for (size_t i = 0; i < _ND; i++)
        a(i, i) = 1;

    std::vector<size_t> pivots(_ND);
    MatrixMxN<_ND, _ND, _T> lu = mx.decomposeLU(pivots);
    
    for (size_t i = 0; i < _ND; i++)
    {
        if (almostZero(lu(i, i)))
        {
            throw except::Exception(Ctxt("Non-invertible matrix!"));
        }
    }

    return solveLU<_ND, _ND, _ND, _T>(pivots, lu, a);
}

/*!
 *  Generalized inverse function.  This function is specialized for 2x2s
 *  and 3x3s for type double and float.
 *
 *  \code      
         Matrix<3, 3> Ainv = inverse<3, double>(A);
 *  \endcode
 */
template<size_t _ND, typename _T> inline
    MatrixMxN<_ND, _ND, _T> inverse(const MatrixMxN<_ND, _ND, _T>& mx)
{
    return inverseLU<_ND, _T>(mx);
}

/*!
 *  Full specialization for 2x2 matrices.  Way faster than
 *  doing the generalized inverse.  This function just magically
 *  gets called if you try to invert a 2x2 double matrix (you
 *  dont have to do anything different than usual)
 *
 *  \param mx A 2x2 double matrix
 *  \return A 2x2 double matrix
 *
 */
template<> inline
    MatrixMxN<2, 2, double> inverse<2, double>(const MatrixMxN<2, 2, double>& mx);


template<> inline
    MatrixMxN<3, 3, double> inverse<3, double>(const MatrixMxN<3, 3, double>& mx);

template<> inline
    MatrixMxN<2, 2, float> inverse<2, float>(const MatrixMxN<2, 2, float>& mx);


template<> inline
    MatrixMxN<3, 3, float> inverse<3, float>(const MatrixMxN<3, 3, float>& mx);

/*!
 *  Could possibly be more clever here, and template the actual matrix
 */
template<size_t _MD, size_t _ND, typename _T> MatrixMxN<_MD, _ND, _T>
operator*(_T scalar, const MatrixMxN<_MD, _ND, _T>& m)
{
    return m.multiply(scalar);
}

/*!
 *  Try to pretty print the Matrix to an ostream.
 *  \return Reference to ostream
 */
template<size_t _MD, size_t _ND, typename _T>
std::ostream& operator<<(std::ostream& os, const MatrixMxN<_MD, _ND, _T>& m)
{
    os << "(" << _MD << ',' << _ND << ")" << std::endl;

    for (size_t i = 0; i < _MD; ++i)
    {
        for (size_t j = 0; j < _ND; ++j)
        {
            os << std::setw(10) << m(i, j) << " ";
        }
        os << std::endl;
    }

    return os;
}
}
}


template<> inline
math::linear::MatrixMxN<2, 2, double> 
math::linear::inverse<2, double>(const math::linear::MatrixMxN<2, 2, double>& mx)
{
    const double determinant = mx[1][1] * mx[0][0] - mx[1][0]*mx[0][1];
    
    if (math::linear::almostZero(determinant))
    {
        throw except::Exception(Ctxt("Non-invertible matrix!"));
    }

    // Standard 2x2 inverse
    math::linear::MatrixMxN<2, 2, double> inv;
    inv[0][0] =  mx[1][1];
    inv[0][1] = -mx[0][1];
    inv[1][0] = -mx[1][0];
    inv[1][1] =  mx[0][0];

    inv.scale( 1.0 / determinant );
    return inv;
}

template<> inline
math::linear::MatrixMxN<3, 3, double> 
math::linear::inverse<3, double>(const math::linear::MatrixMxN<3, 3, double>& mx)
{
    const auto& a = mx[0][0];
    const auto& b = mx[0][1];
    const auto& c = mx[0][2];

    const auto& d = mx[1][0];
    const auto& e = mx[1][1];
    const auto& f = mx[1][2];

    const auto& g = mx[2][0];
    const auto& h = mx[2][1];
    const auto& i = mx[2][2];

    const auto g1 = e * i - f * h;
    const auto g2 = d * i - f * g;
    const auto g3 = d * h - e * g;

    const auto determinant = a * g1 - b * g2 + c * g3;
    
    if (math::linear::almostZero(determinant))
    {
        throw except::Exception(Ctxt("Non-invertible matrix!"));
    }

    math::linear::MatrixMxN<3, 3> inv;
    inv[0][0] =  g1; inv[0][1] =  c*h - b*i; inv[0][2] =  b*f - c*e;
    inv[1][0] = -g2; inv[1][1] =  a*i - c*g; inv[1][2] =  c*d - a*f;
    inv[2][0] =  g3; inv[2][1] =  b*g - a*h; inv[2][2] =  a*e - b*d;
    inv.scale( 1.0 / determinant );
    
    return inv;
}


template<> inline
math::linear::MatrixMxN<2, 2, float> 
math::linear::inverse<2, float>(const math::linear::MatrixMxN<2, 2, float>& mx)
{
    const float determinant = mx[1][1] * mx[0][0] - mx[1][0]*mx[0][1];
    
    if (math::linear::almostZero(determinant))
    {
        throw except::Exception(Ctxt("Non-invertible matrix!"));
    }

    // Standard 2x2 inverse
    math::linear::MatrixMxN<2, 2, float> inv;
    inv[0][0] =  mx[1][1];
    inv[0][1] = -mx[0][1];
    inv[1][0] = -mx[1][0];
    inv[1][1] =  mx[0][0];

    inv.scale( 1.0f / determinant );
    return inv;
}

template<> inline
math::linear::MatrixMxN<3, 3, float> 
math::linear::inverse<3, float>(const math::linear::MatrixMxN<3, 3, float>& mx)
{
    const auto& a = mx[0][0];
    const auto& b = mx[0][1];
    const auto& c = mx[0][2];

    const auto& d = mx[1][0];
    const auto& e = mx[1][1];
    const auto& f = mx[1][2];

    const auto& g = mx[2][0];
    const auto& h = mx[2][1];
    const auto& i = mx[2][2];

    const auto g1 = e * i - f * h;
    const auto g2 = d * i - f * g;
    const auto g3 = d * h - e * g;

    const auto determinant = a * g1 - b * g2 + c * g3;
    
    if (math::linear::almostZero(determinant))
    {
        throw except::Exception(Ctxt("Non-invertible matrix!"));
    }

    math::linear::MatrixMxN<3, 3, float> inv;
    inv[0][0] =  g1; inv[0][1] =  c*h - b*i; inv[0][2] =  b*f - c*e;
    inv[1][0] = -g2; inv[1][1] =  a*i - c*g; inv[1][2] =  c*d - a*f;
    inv[2][0] =  g3; inv[2][1] =  b*g - a*h; inv[2][2] =  a*e - b*d;
    inv.scale( 1.0f / determinant );
    
    return inv;
}

/*!
 *  This method "cleans" a Matrix of unknown type.  Concrete instantiations could
 *  include MatrixMxN or Matrix2D, or any other type that has a rows() and cols(),
 *  a proper assignment operator from the same type, and an overloaded (i, j)
 *  operator.
 *
 *  Any value that is within the given epsilon of an integer is rounded to that
 *  integer value.  The parameter is unmodified
 *
 *  \param constMatrix A matrix to tidy
 *  \param The epsilon fudge factor
 *  \return 
 */
template<typename Matrix_T> Matrix_T tidy(const Matrix_T& constMatrix,
                                          double eps = std::numeric_limits<float>::epsilon())
{
    Matrix_T mx = constMatrix;
    for (size_t i = 0; i < mx.rows(); i++)
    {
        for (size_t j = 0; j < mx.cols(); j++)
        {
            double lower = std::floor(mx(i,j));
            double higher = std::ceil(mx(i,j));

            // If the floor is within epsilon, floor this
            if (math::linear::equals(std::abs(mx(i, j) - lower), 0.0, eps))
                mx(i, j) = lower;

            else if (math::linear::equals(std::abs(higher - mx(i, j)), 0.0, eps))
                mx(i, j) = higher;
            
            if (mx(i, j) == -0)
                mx(i, j) = 0;
        }
    }
    return mx;
}

#endif  // CODA_OSS_math_linear_MatrixMxN_h_INCLUDED_
