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
#ifndef __MATH_LINEAR_MATRIX_2D_H__
#define __MATH_LINEAR_MATRIX_2D_H__

#include <cmath>
#include <algorithm>
#include <functional>
#include <import/sys.h>
#include <mem/ScopedArray.h>
#include <math/linear/MatrixMxN.h>

namespace math
{
namespace linear
{

// Forward declare friend class
template <typename _T>
class Vector;

/*!
 *  \class Matrix2D
 *  \brief Flexible sized Matrix template
 *
 *  This class provides a flexible size class of arbitrary, sizable
 *  dimensions M and N.  Invocations are not required to know the size of these matrices
 *  at compile time.  This class is more flexible then its MatrixMxN counterpart, however
 *  the MatrixMxN should be favored wherever possible (in other words, whenever the
 *  matrix dimensions are known a priori).
 *
 *  This class is assumed to be a mathematical matrix of small to moderate size.  Memory
 *  allocation is managed by the ScopedArray used underneath.
 *
 *
 */
template <typename _T=double>
class Matrix2D
{
    friend class Vector<_T>;

    //!  Matrix dimension in rows
    size_t mM;

    //!  Matrix dimension in cols
    size_t mN;

    //! Length of storage
    size_t mMN;

    //! storage owned by the object
    mem::ScopedArray<_T> mStorage;

    //!  pointer to the raw storage
    _T *mRaw;

public:
    //! Default constructor (does nothing)
    Matrix2D() :
        mM(0), mN(0), mMN(0), mRaw(nullptr)
    {
    }

    /*!
     *  Create a matrix with a constant value for
     *  each element.
     *
     *  \param cv A constant value (defaults to 0)
     *
     *  \code
           Matrix2D<float> mx(2, 2, 4.2f);
     *  \endcode
     *
     */
    Matrix2D(size_t M, size_t N, _T cv = 0) :
        mM(M), mN(N), mMN(M*N),
        mStorage(new _T[mMN]),
        mRaw(mStorage.get())
    {
        std::fill_n(mRaw, mMN, cv);
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
          Matrix2D<> A(3, 3, raw9);
     *  \endcode
     *
     *  \param raw A raw pointer to copy internally
     */
    Matrix2D(size_t M, size_t N, const _T* raw) :
        mM(M), mN(N), mMN(M*N),
        mStorage(new _T[mMN]),
        mRaw(mStorage.get())
    {
        std::copy(raw, raw+mMN, mRaw);
    }
    /*!
     *  Construct a matrix from a 1D M*N vector.
     *
     *  \code
          std::vector<double> vec9(9, 42.0);
          Matrix2D<> A(3, 3, vec9);
     *  \endcode
     *
     *
     */
    Matrix2D(size_t M, size_t N, const std::vector<_T>& raw) :
        mM(M), mN(N), mMN(M*N),
        mStorage(new _T[mMN]),
        mRaw(mStorage.get())
    {
        // use mMN endpoint, since mMN can be less than raw.size()
        std::copy(raw.begin(), raw.begin()+mMN, mRaw);
    }
    /*!
     *  Supports explicit assignment from
     *  one matrix to another
     *
     * \param mx A constant matrix to copy
     *
     *  \code
          Matrix2D<> At(A.transpose());
     *  \endcode
     */
    Matrix2D(const Matrix2D& mx) :
        mM(mx.mM), mN(mx.mN), mMN(mx.mMN),
        mStorage(new _T[mMN]),
        mRaw(mStorage.get())
    {
        std::copy(mx.mRaw, mx.mRaw+mMN, mRaw);
    }
    /*!
     *  Supports use of the class as a decorator
     *  for an existing pointer. The object
     *  may adopt the pointer, taking responsibility
     *  for its destruction.
     *
     *  \code
          std::vector<double> vec9(9, 42.0);
          Matrix2D<> At(3, 3, &vec9[0], false);
     *  \endcode
     */
    Matrix2D(size_t M, size_t N, _T* raw, bool adopt) :
        mM(M), mN(N), mMN(M*N), mRaw(raw)
    {
        if (adopt)
        {
            // pointer will be destroyed by us
            mStorage.reset(raw);
            mRaw = mStorage.get();
        }
    }

    template <size_t _MD, size_t _ND>
    Matrix2D(const MatrixMxN<_MD, _ND>& input) :
        mM(_MD),
        mN(_ND),
        mMN(_MD * _ND),
        mStorage(new _T[mMN]),
        mRaw(mStorage.get())
    {
        for (size_t idx = 0, mm = 0; mm < _MD; ++mm)
        {
            for (size_t nn = 0; nn < _ND; ++nn, ++idx)
            {
                mRaw[idx] = input[mm][nn];
            }
        }
    }

    /*!
     *  Assignment operator from one matrix to another
     *
     *  \code
          Matrix2D<> At = A.transpose();
     *  \endcode
     *
     *  \param mx The source matrix
     *  \return this (the copy)
     */
    Matrix2D& operator=(const Matrix2D& mx)
    {
        if (this != &mx)
        {
            mM  = mx.mM;
            mN  = mx.mN;
            mMN = mx.mMN;

            mStorage.reset(new _T[mMN]),
            mRaw = mStorage.get();

            std::copy(mx.mRaw, mx.mRaw+mMN, mRaw);
        }
        return *this;
    }

    /*!
     *  Set a matrix to a single element containing the contents
     *  of a scalar value.  Note that this behavior differs (drastically)
     *  from the MatrixMxN operator for a scalar value, since here, the
     *  dimensions cannot be known
     *
     *  \code
           Matrix2D<float> mx = 4.3f;
           assert(mx.size() == 1);
     *  \endcode
     *
     */
    Matrix2D& operator=(const _T& sv)
    {
        mM  = 1;
        mN  = 1;
        mMN = 1;
        mStorage.reset(new _T[1]);
        mRaw = mStorage.get();
        mRaw[0] = sv;
        return *this;
    }

    //! Nothing is allocated by us
    ~Matrix2D() {}


    /*!
     *  Get back the value at index i, j
     *  \code
           double Aij = A(i, j);
     *  \endcode
     *
     *  \param i The row index
     *  \param j The column index
     */
    inline _T operator()(size_t i, size_t j) const
    {
#if defined(MATH_LINEAR_BOUNDS)
        assert( i < mM && j < mN );
#endif
        return mRaw[i * mN + j];
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
    inline _T& operator()(size_t i, size_t j)
    {
#if defined(MATH_LINEAR_BOUNDS)
        assert( i < mM && j < mN );
#endif
        return mRaw[i * mN + j];
    }

    /*!
     * Please try to avoid this method.  There is extensive information
     * here about why you should avoid it:
     * http://www.parashift.com/c++-faq-lite/operator-overloading.html#faq-13.10
     * http://www.parashift.com/c++-faq-lite/operator-overloading.html#faq-13.11
     */
    inline const _T* operator[](size_t i) const
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
     *  treating row i as a mutable pointer.
     */
    inline _T* operator[](size_t i)
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
    inline const _T* row(size_t i) const
    {
#if defined(MATH_LINEAR_BOUNDS)
        assert( i < mM);
#endif
        return &mRaw[i * mN];
    }

    /*!
     *  Never use this method!  This method is dangerous
     *  since the user can cause damage by unwittingly
     *  treating row i as a mutable pointer.
     */
    inline _T* row(size_t i)
    {
#if defined(MATH_LINEAR_BOUNDS)
        assert( i < mM);
#endif
        return &mRaw[i * mN];
    }

    /*!
     *  Set the matrix row i to a copy of
     *  the row vector
     *
     *  \code
          Matrix2D<> A(3, 3, 42.0);
          double rowVec[] = { 1, 2, 3 };
          A.row(0, rowVec);
     *  \endcode
     *
     *  \param i The row index
     *  \param vec The row vector to copy from
     */
    inline void row(size_t i, const _T* vec_)
    {
        for (size_t j = 0; j < mN; j++)
        {
            mRaw[i * mN + j] = vec_[j];
        }
    }
    /*!
     *  Set the matrix row i to a copy of
     *  the row vector
     *
     *  \code
          Matrix2D<> A(3, 3, 42.0);
          std::vector<double> rowVec(3, 1.2);
          A.row(0, rowVec);
     *  \endcode
     *
     *  \param i The row index
     *  \param vec The row vector to copy from
     */
    inline void row(size_t i, const std::vector<_T>& vec_)
    {
        if (!vec_.empty())
        {
            row(i, &vec_[0]);
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
        std::vector<_T> jth(mM);
        for (size_t i = 0; i < mM; ++i)
        {
            jth[i] = mRaw[i * mN + j];
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
    void col(size_t j, const _T* vec_)
    {
        for (size_t i = 0; i < mM; ++i)
        {
            mRaw[i * mN + j] = vec_[i];
        }
    }

    /*!
     *  Set column from vector at index j
     *
     *  \code
          Matrix2D<> A(3, 3, 42.0);
          std::vector<double> colVec(3, 1.2);
          A.col(0, colVec);
     *  \endcode
     *
     *  \param j The column index
     *  \param vec The vector to copy from
     */
    void col(size_t j, const std::vector<_T>& vec_)
    {
        if (!vec_.empty())
        {
            col(j, &vec_[0]);
        }
    }

    //!  Return number of rows (M)
    size_t rows() const { return mM; }

    //!  Return number of cols (N)
    size_t cols() const { return mN; }

    //!  Return total size (M x N)
    size_t size() const { return mMN; }

    //! Get a constant ref to the underlying vector
    const _T* get() const { return mRaw; }

    /*!
     *  Equality operator test
     *
     *  \code

          double raw4[] = { 1, 2, 3, 4 };
          Matrix2D<double> A(2, 2, raw4);
          MatrixMxN<2, 2> B(raw4);
          Matrix2D<double> C = A;
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

        size_t M = rows();
        size_t N = cols();
        for (size_t i = 0; i < M; ++i)
        {
            for (size_t j = 0; j < N; ++j)
            {
                if (! equals(mRaw[i * N + j], mx(i, j)))
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
          Matrix2D<double> A(2, 2, raw4);
          MatrixMxN<2, 2> B(raw4);
          B(1, 1) = 42.0;
          Matrix2D<double> C(2, 2, &B.mRaw[0][0]);
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
           Matrix2D<float> mx = createIdentity<float>(3);
           mx.scale(4.2f);
     *  \endcode
     *
     *
     *  \param scalar The value to multiply into mx
     *
     */
    Matrix2D& scale(_T scalar)
    {
        for (size_t i = 0; i < mMN; ++i)
            mRaw[i] *= scalar;
        return *this;
    }

    /*!
     *  Does a matrix-scalar multiplication.  The
     *  output is a matrix of same dimensions.  This
     *  function is identical to the scale() method
     *  except that this matrix is not mutated -
     *  a scale copy is produced and returned. Note that
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
    Matrix2D multiply(_T scalar) const
    {
        Matrix2D mx = *this;

        for (size_t i = 0; i < mMN; ++i)
        {
            mx.mRaw[i] *= scalar;
        }
        return mx;
    }

    /*!
     *  Multiply an NxP matrix to a MxN matrix (this) to
     *  produce an MxP matrix output.
     *
     *  This function accesses the inner arrays for
     *  (potential, though slight) performance reasons.
     *
     *  \param mx An NxP matrix
     *  \return An MxP matrix
     *
     *
     *  \code
           Matrix2D<> a3x1(3, 1, 42.0);
           Matrix2D<> a1x3(1, 3, 1.0);
           Matrix2D<> a3x3(A.multiply(B));
           Matrix2D<> a1x1(B.multiply(A));
     *  \endcode
     *
     */
    Matrix2D
        multiply(const Matrix2D& mx) const
    {
        if (mN != mx.mM)
            throw except::Exception(Ctxt(
                "Invalid inner dimension sizes for multiply"));

        size_t M = mM;
        size_t P = mx.mN;

        Matrix2D newM(M, P);
        multiply(mx, newM);
        return newM;
    }

    /*!
     *  Multiply an NxP matrix to a MxN matrix (this) to
     *  produce an MxP matrix output.
     *
     *  This function accesses the inner arrays for
     *  (potential, though slight) performance reasons.
     *
     *  \param mx An NxP matrix
     *  \param out An MxP matrix
     *
     *  \code
           Matrix2D<> a3x1(3, 1, 42.0);
           Matrix2D<> a1x3(1, 3, 1.0);
           Matrix2D<> a3x3(3, 3, 0);
           A.multiply(B, a3x3);
     *  \endcode
     *
     */
    void
    multiply(const Matrix2D& mx, Matrix2D &out) const
    {
        size_t M(mM);
        size_t N(mN);
        size_t P(mx.mN);

        if (mN != mx.mM)
            throw except::Exception(Ctxt(
                "Invalid inner dimension sizes for multiply"));
        if (out.mM != M)
            throw except::Exception(Ctxt(
                "Invalid output row size for multiply"));
        if (out.mN != P)
            throw except::Exception(Ctxt(
                "Invalid output column size for multiply"));

        size_t i, j, k;
        for (i = 0; i < M; i++)
        {
            for (j = 0; j < P; j++)
            {
                out(i, j) = 0;

                for (k = 0; k < N; k++)
                {
                    out(i, j) += mRaw[i * N + k] * mx(k, j);
                }
            }
        }
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
    Matrix2D& scaleDiagonal(const Matrix2D& mx)
    {
        if (mx.mM != mx.mN || mx.mN != mN)
            throw except::Exception(Ctxt("Invalid size for diagonal multiply"));

        size_t i, j;
        for (i = 0; i < mM; i++)
        {
            for (j = 0; j < mN; j++)
            {
                mRaw[i * mN + j] *= mx(j,j);
            }
        }
        return *this;
    }

    /*!
     * Same as scaleDiagonal() but takes mx in as a row vector
     *
     */
    Matrix2D& scaleDiagonalRowVector(const Matrix2D& mx)
    {
        if (mx.mM != mN || mx.mN != 1)
            throw except::Exception(Ctxt("Invalid size for diagonal multiply"));

        size_t i, j;
        for (i = 0; i < mM; i++)
        {
            for (j = 0; j < mN; j++)
            {
                mRaw[i * mN + j] *= mx(j,0);
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
    Matrix2D
        multiplyDiagonal(const Matrix2D& mx) const
    {
        Matrix2D newM = *this;
        newM.scaleDiagonal(mx);
        return newM;
    }

    /*!
     *  This function is the same as multiplyDiagonal except that mx is a
     *  row vector
     *
     */
    Matrix2D
        multiplyDiagonalRowVector(const Matrix2D& mx) const
    {
        Matrix2D newM = *this;
        newM.scaleDiagonalRowVector(mx);
        return newM;
    }

    /*!
     *  This function does an add and accumulate
     *  operation.  The parameter is add-assigned
     *  element-wise to this
     *
     *  \param mx The matrix to assign (MxN)
     *  \return This
     *
     *  \code
           A += B;
     *  \endcode
     *
     */
    Matrix2D&
    operator+=(const Matrix2D& mx)
    {
        if (mM != mx.mM || mN != mx.mN)
            throw except::Exception(Ctxt("Required to equally size matrices for element-wise add"));

        for (size_t i = 0; i < mMN; ++i)
        {
            mRaw[i] += mx.mRaw[i];
        }
        return *this;

    }

    /*!
     *  This function does a subtraction
     *  operation element wise.
     *
     *  \param mx MxN matrix to subtract from this
     *  \return This
     *
     *  \code
           A -= B;
     *  \endcode
     *
     */
    Matrix2D&
    operator-=(const Matrix2D& mx)
    {
        if (mx.mM != mM || mx.mN != mN)
            throw except::Exception(Ctxt("Matrices must be same size for element-wise subtract"));

        for (size_t i = 0; i < mM; i++)
        {
            for (size_t j = 0; j < mN; j++)
            {
                mRaw[i * mN + j] -= mx(i, j);
            }
        }
        return *this;

    }

    /*!
     *  Add an MxN matrix to another and return a third
     *  that is the sum.  This operation does not mutate this.
     *
     *  \param mx
     *  \return The sum
     *
     *  \code
           C = A.add(B);
     *  \endcode
     *
     */
    Matrix2D add(const Matrix2D& mx) const
    {
        Matrix2D newM = *this;
        newM += mx;
        return newM;
    }

    /*!
     *  Subtract an MxN matrix to another and return a third
     *  that is the sum.  This operation does not mutate this.
     *
     *  \param mx
     *  \return The sum
     *
     *  \code
           C = A.subtract(B);
     *  \endcode
     *
     */

    Matrix2D subtract(const Matrix2D& mx) const
    {
        Matrix2D newM = *this;
        newM -= mx;
        return newM;
    }


    /*!
     *  Create a NxM matrix which is the transpose of this
     *  MxN matrix.
     *
     *  \return An NxM matrix that is the transpose
     *
     *  \code
           B = A.tranpose();
     *  \endcode
     *
     */
    Matrix2D transpose() const
    {

        Matrix2D x(mN, mM);
        for (size_t i = 0; i < mM; i++)
            for (size_t j = 0; j < mN; j++)
                x.mRaw[j * mM + i] = mRaw[i * mN + j];

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
     *  \param [out] pivotsM (pre sized)
     *
     */
    Matrix2D decomposeLU(std::vector<size_t>& pivotsM) const
    {

        Matrix2D lu(mM, mN);

        for (size_t i = 0; i < mM; i++)
        {
            // Start by making our pivots unpermuted
            pivotsM[i] = i;
            for (size_t j = 0; j < mN; j++)
            {
                // And copying elements
                lu.mRaw[i * mN + j] = mRaw[i * mN + j];
            }
        }

        std::vector<_T> colj(mM);
        _T* rowi;

        for (size_t j = 0; j < mN; j++)
        {
            for (size_t i = 0; i < mM; i++)
            {
                colj[i] = lu(i, j);
            }

            for (size_t i = 0; i < mM; i++)
            {
                rowi = lu[i];

                size_t max = std::min<size_t>(i, j);
                _T s(0);
                for (size_t k = 0; k < max; k++)
                {
                    s += rowi[k] * colj[k];
                }
                colj[i] -= s;
                rowi[j] = colj[i];

            }

            size_t p = j;
            for (size_t i = j + 1; i < mM; i++)
            {
                if (std::abs(colj[i]) > std::abs(colj[p]))
                    p = i;

            }
            if (p != j)
            {
                size_t k = 0;
                for (; k < mN; k++)
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
            if (j < mM && std::abs( lu(j, j) ))
            {
                for (size_t i = j + 1; i < mM; i++)
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
           Matrix2D G = F.permute(P);
     *  \endcode
     *
     */
    Matrix2D permute(const std::vector<size_t>& pivotsM, size_t n = 0) const
    {
        if (n == 0) n = mN;
        Matrix2D perm(mM, n);
        for (size_t i = 0; i < mM; i++)
        {
            for (size_t j = 0; j < n; j++)
            {
                perm(i, j) = mRaw[pivotsM[i] * mN + j];
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
        size_t sz = mM * mN;
        _T acc(0);
        for (size_t i = 0; i < sz; ++i)
        {
            acc += mRaw[i] * mRaw[i];
        }
        return acc;
    }

    /*!
     *  Find the L2 norm of the matrix.
     *  \return The norm
     */
    _T norm() const
    {
        return static_cast<_T>(std::sqrt(normSq()));
    }

    /*!
     *  Scale the entire matrix inplace by the L2 norm value.
     *  \return A reference to this
     */
    Matrix2D& normalize()
    {
        return scale(1.0/norm());
    }

    /*!
     *  Scale the matrix by the L2 norm value. This
     *  function performs the same operation as normalize()
     *  except that this matrix is not mutated.
     */
    Matrix2D unit() const
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
    Matrix2D operator+(const Matrix2D& mx) const
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
    Matrix2D operator-(const Matrix2D& mx) const
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
    Matrix2D operator*(_T scalar) const
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
    Matrix2D operator/(_T scalar) const
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
    Matrix2D
    operator*(const Matrix2D& mx) const
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
    Matrix2D operator-() const
    {
        Matrix2D neg(*this);
        std::transform(neg.mRaw,
                       neg.mRaw + neg.mMN,
                       neg.mRaw,
                       std::negate<_T>());
        return neg;
    }

    /*!
     *  serialize out to a boost stream
     */
    template <class Archive>
    void serialize(Archive& ar, const unsigned int  /*version*/)
    {
        ar & mM;
        ar & mN;
        ar & mMN;
        if (Archive::is_loading::value)
        {
            mStorage.reset(new _T[mMN]);
            mRaw = mStorage.get();
        }
        for (size_t ii = 0; ii < mMN; ++ii)
        {
            ar & mRaw[ii];
        }
    }
};

/*!
 *  This function creates an identity matrix of size NxN and type _T
 *  with 1's in the diagonals.
 *
 *  \code
        Matrix2D = identityMatrix<double>(4);
 *  \endcode
 *
 *  \param N the dimension in rows and in cols for the matrix to be produced
 *
 */
template<typename _T> Matrix2D<_T>
    identityMatrix(size_t N)
{
    Matrix2D<_T> mx(N, N);
    for (size_t i = 0; i < N; i++)
    {
        for (size_t j = 0; j < N; j++)
        {
            mx(i, j) = (i == j) ? 1: 0;
        }
    }
    return mx;
}

template<typename _T, typename Vector_T > Matrix2D<_T>
    diagonalMatrix(const Vector_T& diag)
{
    size_t N = diag.size();

    Matrix2D<_T> mx(N, N, 0.0);
    for (size_t i = 0; i < N; ++i)
    {
        mx(i, i) = diag[i];
    }
    return mx;
}

/*!
 *  Solve  Ax = b using LU decomposed matrix and the permutation vector.
 *  Method based on TNT
 *
 */
template<typename _T>
    math::linear::Matrix2D<_T> solveLU(const std::vector<size_t>& pivotsM,
                                       const Matrix2D<_T> &lu,
                                       const Matrix2D<_T> &b)
{


    // If we dont have something in the diagonal, we can't solve this
    math::linear::Matrix2D<_T> x = b.permute(pivotsM);

    const size_t P = b.cols();
    const size_t N = lu.cols();
    for (size_t kk = 0; kk < N; kk++)
    {
        for (size_t ii = kk + 1; ii < N; ii++)
        {
            for (size_t jj = 0; jj < P; jj++)
            {
                x(ii, jj) -= x(kk, jj)*lu(ii, kk);
            }
        }
    }
    for (sys::SSize_T kk = N - 1; kk >= 0; kk--)
    {
        for (size_t jj = 0; jj < P; jj++)
        {
            x(kk, jj) /= lu(kk, kk);
        }

        for (size_t ii = 0; ii < static_cast<size_t>(kk); ii++)
        {
            // This one could be _Q
            for (size_t jj = 0; jj < P; jj++)
            {
                x(ii, jj) -= x(kk, jj)*lu(ii, kk);
            }
        }
    }

    return x;
}

/*!
 *  Perform hard-coded 2x2 matrix inversion.  You can use this method
 *  directly, or the inverse() operator which will automatically call
 *  this for 2x2s
 *
 */
template<typename _T> inline Matrix2D<_T> inverse2x2(const Matrix2D<_T>& mx)
{
    const double determinant = mx(1,1) * mx(0,0) - mx(1,0)*mx(0,1);

    if (almostZero(determinant))
    {
        throw except::Exception(Ctxt("Non-invertible matrix!"));
    }

    // Standard 2x2 inverse
    Matrix2D<_T> inv(2, 2);
    inv(0,0) =  mx(1,1);
    inv(0,1) = -mx(0,1);
    inv(1,0) = -mx(1,0);
    inv(1,1) =  mx(0,0);

    inv.scale( 1.0 / determinant );
    return inv;
}

/*!
 *  Perform hard-coded 3x3 matrix inversion.  You can use this method
 *  directly, or the inverse() operator which will automatically call
 *  this for 3x3s
 *
 */
template<typename _T> inline Matrix2D<_T>
    inverse3x3(const Matrix2D<_T>& mx)
{
    double a = mx(0,0);
    double b = mx(0,1);
    double c = mx(0,2);

    double d = mx(1,0);
    double e = mx(1,1);
    double f = mx(1,2);

    double g = mx(2,0);
    double h = mx(2,1);
    double i = mx(2,2);

    double g1 = e*i - f*h;
    double g2 = d*i - f*g;
    double g3 = d*h - e*g;

    const double determinant = a*g1 - b*g2 + c*g3;

    if (almostZero(determinant))
    {
        throw except::Exception(Ctxt("Non-invertible matrix!"));
    }

    Matrix2D<double> inv(3, 3);
    inv(0,0) =  g1; inv(0,1) =  c*h - b*i; inv(0,2) =  b*f - c*e;
    inv(1,0) = -g2; inv(1,1) =  a*i - c*g; inv(1,2) =  c*d - a*f;
    inv(2,0) =  g3; inv(2,1) =  b*g - a*h; inv(2,2) =  a*e - b*d;
    inv.scale( 1.0 / determinant );

    return inv;
}

/*!
 *  Generalized inverse method (currently uses LU decomposition).
 *
 *  \param mx A matrix to invert
 *
 *  \code
         Matrix2D<> Ainv = inverseLU<double>(A);
 *  \endcode
 *
 */
template<typename _T> inline
    Matrix2D<_T> inverseLU(const Matrix2D<_T>& mx)
{
    size_t M = mx.rows();
    size_t N = mx.cols();
    Matrix2D<_T> a(M, M, (_T)0);

    for (size_t i = 0; i < M; i++)
        a(i, i) = 1;

    std::vector<size_t> pivots(M);
    Matrix2D<_T> lu = mx.decomposeLU(pivots);

    for (size_t i = 0; i < N; i++)
    {
        if (almostZero(lu(i, i)))
        {
            throw except::Exception(Ctxt("Non-invertible matrix!"));
        }
    }

    return solveLU(pivots, lu, a);
}

/*!
 *  Generalized inverse function.  This function has special
 *  cases for 2x2s and 3x3s.  Otherwise, it uses generalized
 *  LU decomposition to solve for the inverse.
 *
 *  \code
         Matrix2D<> Ainv = inverse<double>(A);
 *  \endcode
 */
template<typename _T> inline
    Matrix2D<_T> inverse(const Matrix2D<_T>& mx)
{
    // Try to speed this up
    if (mx.rows() != mx.cols())
        throw except::Exception(Ctxt("Expected a square matrix"));
    if (mx.rows() == 2)
        return inverse2x2<_T>(mx);
    if (mx.rows() == 3)
        return inverse3x3<_T>(mx);
        // TODO Add 4x4

    return inverseLU<_T>(mx);
}

/*!
 * Calculate the left inverse
 * \param mx The matrix to find the inverse of. Mx is assumed to have more rows
 * than columns, and that the columns are linearly independent.
 * \return Left inverse
 * \throws if matrix is not left-invertible
 */
template<typename _T> inline
    Matrix2D<_T> leftInverse(const Matrix2D<_T>& mx)
{
    return inverse(mx.transpose() * mx) * mx.transpose();
}

/*!
 * Calculate the right inverse
 * \param mx The matrix to find the inverse of. Mx is assumed to have more
 * columns than rows, and that the rows are linearly independent.
 * \return Right inverse
 * \throws if matrix is not right-invertible
 */
template<typename _T> inline
    Matrix2D<_T> rightInverse(const Matrix2D<_T>& mx)
{
    return mx.transpose() * inverse(mx * mx.transpose());
}

template<typename _T> Matrix2D<_T>
operator*(_T scalar, const Matrix2D<_T>& m)
{
    return m.multiply(scalar);
}

/*!
 *  Try to pretty print the Matrix to an ostream.
 *  \return Reference to ostream
 */
template<typename _T>
std::ostream& operator<<(std::ostream& os, const Matrix2D<_T>& m)
{
    os << "(" << m.rows() << ',' << m.cols() << ")" << std::endl;
    for (size_t i = 0; i < m.rows(); ++i)
    {
        for (size_t j = 0; j < m.cols(); ++j)
        {
            os << std::setw(10) << m(i, j) << " ";
        }
        os << std::endl;
    }

    return os;
}
}
}

#endif
