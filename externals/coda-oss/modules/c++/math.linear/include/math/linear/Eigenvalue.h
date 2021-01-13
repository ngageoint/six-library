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
#ifndef __MATH_LINEAR_EIGENVALUE_H__
#define __MATH_LINEAR_EIGENVALUE_H__

#include <cmath>
#include <math.h>

#include <sys/Conf.h>
#include <except/Exception.h>
#include <str/Convert.h>
#include <math/linear/Matrix2D.h>
#include <math/linear/Vector.h>

namespace math
{
namespace linear
{
/*!
 *   \class Eigenvalue
 *   \brief Computes eigenvalues and eigenvectors of a real matrix
 *
 *   If A is symmetric, then A = V*D*V' where the eigenvalue matrix D is
 *   diagonal and the eigenvector matrix V is orthogonal. That is,
 *   the diagonal values of D are the eigenvalues, and
 *   V*V' = I, where I is the identity matrix.  The columns of V
 *   represent the eigenvectors in the sense that A*V = V*D.
 *
 *   If A is not symmetric, then the eigenvalue matrix D is block diagonal
 *   with the real eigenvalues in 1-by-1 blocks and any complex eigenvalues,
 *   a + i*b, in 2-by-2 blocks, [a, b; -b, a].  That is, if the complex
 *   eigenvalues look like

          u + iv     .        .          .      .    .
            .      u - iv     .          .      .    .
            .        .      a + ib       .      .    .
            .        .        .        a - ib   .    .
            .        .        .          .      x    .
            .        .        .          .      .    y

 *   then D looks like

            u        v        .          .      .    .
           -v        u        .          .      .    .
            .        .        a          b      .    .
            .        .       -b          a      .    .
            .        .        .          .      x    .
            .        .        .          .      .    y
 *
 *   This keeps V a real matrix in both symmetric and non-symmetric
 *   cases, and A*V = V*D.
 *
 *   The matrix V may be badly conditioned, or even singular, so the validity
 *   of the equation A = V*D*inverse(V) depends upon the condition number of V.
 *
 *   This is based on the Eigenvalue class from TNT's tnt_linalg.h which
 *   says it was adapted from JAMA, a Java Matrix Library, developed jointly
 *   by the Mathworks and NIST (see  http://math.nist.gov/javanumerics/jama),
 *   which in turn, were based on original EISPACK routines.
 *
 *   RealT must be a real (non-complex) type
 */
template<typename RealT>
class Eigenvalue
{
public:
    /*
     * Construct the eigenvalue decomposition
     * \param A Square matrix
     */
    Eigenvalue(const Matrix2D<RealT>& A) :
        mN_(static_cast<int>(A.cols())),
        mD(mN_),
        mE(mN_),
        mV(mN_, mN_)
    {
        if (A.rows() != A.cols())
        {
            throw except::Exception(Ctxt(
                "Expected square matrix but got rows = " +
                str::toString(A.rows()) + ", cols = " +
                str::toString(A.cols())));
        }

        if (isSymmetric(A))
        {
            mV = A;

            // Tridiagonalize.
            tred2();

            // Diagonalize.
            tql2();
        }
        else
        {
            mH = Matrix2D<RealT>(mN_, mN_);
            mOrt = Vector<RealT>(mN_);

            mH = A;

            // Reduce to Hessenberg form.
            orthes();

            // Reduce Hessenberg to real Schur form.
            hqr2();
        }
    }

    // Return the eigenvector matrix
    const Matrix2D<RealT>& getV() const
    {
        return mV;
    }

    // Return the real parts of the eigenvalues
    const Vector<RealT>& getRealEigenvalues() const
    {
        return mD;
    }

    // Return the imaginary parts of the eigenvalues
    const Vector<RealT>& getImagEigenvalues() const
    {
        return mE;
    }

    /**
     Computes the block diagonal eigenvalue matrix.
     If the original matrix A is not symmetric, then the eigenvalue
     matrix D is block diagonal with the real eigenvalues in 1-by-1
     blocks and any complex eigenvalues,
     a + i*b, in 2-by-2 blocks, [a, b; -b, a].  That is, if the complex
     eigenvalues look like
     <pre>

     u + iv     .        .          .      .    .
     .      u - iv     .          .      .    .
     .        .      a + ib       .      .    .
     .        .        .        a - ib   .    .
     .        .        .          .      x    .
     .        .        .          .      .    y
     </pre>
     then D looks like
     <pre>

     u        v        .          .      .    .
     -v        u        .          .      .    .
     .        .        a          b      .    .
     .        .       -b          a      .    .
     .        .        .          .      x    .
     .        .        .          .      .    y
     </pre>
     This keeps V a real matrix in both symmetric and non-symmetric
     cases, and A*V = V*D.

     \param D: upon return, the matrix is filled with the block diagonal
     eigenvalue matrix.
     */
    void getD(Matrix2D<RealT>& D) const
    {
        D = Matrix2D<RealT>(mN_, mN_, static_cast<RealT>(0));
        for (int i = 0; i < mN_; i++)
        {
            D[i][i] = mD[i];
            if (mE[i] > 0)
            {
                D[i][i + 1] = mE[i];
            }
            else if (mE[i] < 0)
            {
                D[i][i - 1] = mE[i];
            }
        }
    }

private:
    // Symmetric Householder reduction to tridiagonal form.
    void tred2()
    {
        //  This is derived from the Algol procedures tred2 by
        //  Bowdler, Martin, Reinsch, and Wilkinson, Handbook for
        //  Auto. Comp., Vol.ii-Linear Algebra, and the corresponding
        //  Fortran subroutine in EISPACK.

        for (int j = 0; j < mN_; j++)
        {
            mD[j] = mV[mN_ - 1][j];
        }

        // Householder reduction to tridiagonal form.

        for (int i = mN_ - 1; i > 0; i--)
        {

            // Scale to avoid under/overflow.

            RealT scale = RealT(0.0);
            RealT h = RealT(0.0);
            for (int k = 0; k < i; k++)
            {
                scale = scale + std::abs(mD[k]);
            }
            if (scale == RealT(0.0))
            {
                mE[i] = mD[i - 1];
                for (int j = 0; j < i; j++)
                {
                    mD[j] = mV[i - 1][j];
                    mV[i][j] = RealT(0.0);
                    mV[j][i] = RealT(0.0);
                }
            }
            else
            {

                // Generate Householder vector.

                for (int k = 0; k < i; k++)
                {
                    mD[k] /= scale;
                    h += mD[k] * mD[k];
                }
                RealT f = mD[i - 1];
                RealT g = sqrt(h);
                if (f > 0)
                {
                    g = -g;
                }
                mE[i] = scale * g;
                h = h - f * g;
                mD[i - 1] = f - g;
                for (int j = 0; j < i; j++)
                {
                    mE[j] = RealT(0.0);
                }

                // Apply similarity transformation to remaining columns.

                for (int j = 0; j < i; j++)
                {
                    f = mD[j];
                    mV[j][i] = f;
                    g = mE[j] + mV[j][j] * f;
                    for (int k = j + 1; k <= i - 1; k++)
                    {
                        g += mV[k][j] * mD[k];
                        mE[k] += mV[k][j] * f;
                    }
                    mE[j] = g;
                }
                f = RealT(0.0);
                for (int j = 0; j < i; j++)
                {
                    mE[j] /= h;
                    f += mE[j] * mD[j];
                }
                RealT hh = f / (h + h);
                for (int j = 0; j < i; j++)
                {
                    mE[j] -= hh * mD[j];
                }
                for (int j = 0; j < i; j++)
                {
                    f = mD[j];
                    g = mE[j];
                    for (int k = j; k <= i - 1; k++)
                    {
                        mV[k][j] -= (f * mE[k] + g * mD[k]);
                    }
                    mD[j] = mV[i - 1][j];
                    mV[i][j] = RealT(0.0);
                }
            }
            mD[i] = h;
        }

        // Accumulate transformations.

        for (int i = 0; i < mN_ - 1; i++)
        {
            mV[mN_ - 1][i] = mV[i][i];
            mV[i][i] = RealT(1.0);
            RealT h = mD[i + 1];
            if (h != RealT(0.0))
            {
                for (int k = 0; k <= i; k++)
                {
                    mD[k] = mV[k][i + 1] / h;
                }
                for (int j = 0; j <= i; j++)
                {
                    RealT g = RealT(0.0);
                    for (int k = 0; k <= i; k++)
                    {
                        g += mV[k][i + 1] * mV[k][j];
                    }
                    for (int k = 0; k <= i; k++)
                    {
                        mV[k][j] -= g * mD[k];
                    }
                }
            }
            for (int k = 0; k <= i; k++)
            {
                mV[k][i + 1] = RealT(0.0);
            }
        }
        for (int j = 0; j < mN_; j++)
        {
            mD[j] = mV[mN_ - 1][j];
            mV[mN_ - 1][j] = RealT(0.0);
        }
        mV[mN_ - 1][mN_ - 1] = RealT(1.0);
        mE[0] = RealT(0.0);
    }

    // Symmetric tridiagonal QL algorithm.

    void tql2()
    {

        //  This is derived from the Algol procedures tql2, by
        //  Bowdler, Martin, Reinsch, and Wilkinson, Handbook for
        //  Auto. Comp., Vol.ii-Linear Algebra, and the corresponding
        //  Fortran subroutine in EISPACK.

        for (int i = 1; i < mN_; i++)
        {
            mE[i - 1] = mE[i];
        }
        mE[mN_ - 1] = RealT(0.0);

        RealT f = RealT(0.0);
        RealT tst1 = RealT(0.0);
        RealT eps = pow(2.0, -52.0);
        for (int l = 0; l < mN_; l++)
        {

            // Find small subdiagonal element

            tst1 = std::max<RealT>(tst1, std::abs(mD[l]) + std::abs(mE[l]));
            int m = l;

            // Original while-loop from Java code
            while (m < mN_)
            {
                if (std::abs(mE[m]) <= eps * tst1)
                {
                    break;
                }
                m++;
            }

            // If m == l, mD[l] is an eigenvalue,
            // otherwise, iterate.

            if (m > l)
            {
                int iter = 0;
                do
                {
                    iter = iter + 1; // (Could check iteration count here.)

                    // Compute implicit shift

                    RealT g = mD[l];
                    RealT p = (mD[l + 1] - g) / (2.0 * mE[l]);
                    RealT r = hypot(p, static_cast<RealT>(RealT(1.0)));
                    if (p < 0)
                    {
                        r = -r;
                    }
                    mD[l] = mE[l] / (p + r);
                    mD[l + 1] = mE[l] * (p + r);
                    RealT dl1 = mD[l + 1];
                    RealT h = g - mD[l];
                    for (int i = l + 2; i < mN_; i++)
                    {
                        mD[i] -= h;
                    }
                    f = f + h;

                    // Implicit QL transformation.

                    p = mD[m];
                    RealT c = RealT(1.0);
                    RealT c2 = c;
                    RealT c3 = c;
                    RealT el1 = mE[l + 1];
                    RealT s = RealT(0.0);
                    RealT s2 = RealT(0.0);
                    for (int i = m - 1; i >= l; i--)
                    {
                        c3 = c2;
                        c2 = c;
                        s2 = s;
                        g = c * mE[i];
                        h = c * p;
                        r = hypot(p, mE[i]);
                        mE[i + 1] = s * r;
                        s = mE[i] / r;
                        c = p / r;
                        p = c * mD[i] - s * g;
                        mD[i + 1] = h + s * (c * g + s * mD[i]);

                        // Accumulate transformation.

                        for (int k = 0; k < mN_; k++)
                        {
                            h = mV[k][i + 1];
                            mV[k][i + 1] = s * mV[k][i] + c * h;
                            mV[k][i] = c * mV[k][i] - s * h;
                        }
                    }
                    p = -s * s2 * c3 * el1 * mE[l] / dl1;
                    mE[l] = s * p;
                    mD[l] = c * p;

                    // Check for convergence.

                }
                while (std::abs(mE[l]) > eps * tst1);
            }
            mD[l] = mD[l] + f;
            mE[l] = RealT(0.0);
        }

        // Sort eigenvalues and corresponding vectors.

        for (int i = 0; i < mN_ - 1; i++)
        {
            int k = i;
            RealT p = mD[i];
            for (int j = i + 1; j < mN_; j++)
            {
                if (mD[j] < p)
                {
                    k = j;
                    p = mD[j];
                }
            }
            if (k != i)
            {
                mD[k] = mD[i];
                mD[i] = p;
                for (int j = 0; j < mN_; j++)
                {
                    p = mV[j][i];
                    mV[j][i] = mV[j][k];
                    mV[j][k] = p;
                }
            }
        }
    }

    // Nonsymmetric reduction to Hessenberg form.

    void orthes()
    {

        //  This is derived from the Algol procedures orthes and ortran,
        //  by Martin and Wilkinson, Handbook for Auto. Comp.,
        //  Vol.ii-Linear Algebra, and the corresponding
        //  Fortran subroutines in EISPACK.

        int low = 0;
        int high = mN_ - 1;

        for (int m = low + 1; m <= high - 1; m++)
        {

            // Scale column.

            RealT scale = RealT(0.0);
            for (int i = m; i <= high; i++)
            {
                scale = scale + std::abs(mH[i][m - 1]);
            }
            if (scale != RealT(0.0))
            {

                // Compute Householder transformation.

                RealT h = RealT(0.0);
                for (int i = high; i >= m; i--)
                {
                    mOrt[i] = mH[i][m - 1] / scale;
                    h += mOrt[i] * mOrt[i];
                }
                RealT g = sqrt(h);
                if (mOrt[m] > 0)
                {
                    g = -g;
                }
                h = h - mOrt[m] * g;
                mOrt[m] = mOrt[m] - g;

                // Apply Householder similarity transformation
                // mH = (I-u*u'/h)*mH*(I-u*u')/h)

                for (int j = m; j < mN_; j++)
                {
                    RealT f = RealT(0.0);
                    for (int i = high; i >= m; i--)
                    {
                        f += mOrt[i] * mH[i][j];
                    }
                    f = f / h;
                    for (int i = m; i <= high; i++)
                    {
                        mH[i][j] -= f * mOrt[i];
                    }
                }

                for (int i = 0; i <= high; i++)
                {
                    RealT f = RealT(0.0);
                    for (int j = high; j >= m; j--)
                    {
                        f += mOrt[j] * mH[i][j];
                    }
                    f = f / h;
                    for (int j = m; j <= high; j++)
                    {
                        mH[i][j] -= f * mOrt[j];
                    }
                }
                mOrt[m] = scale * mOrt[m];
                mH[m][m - 1] = scale * g;
            }
        }

        // Accumulate transformations (Algol's ortran).

        for (int i = 0; i < mN_; i++)
        {
            for (int j = 0; j < mN_; j++)
            {
                mV[i][j] = (i == j ? RealT(1.0) : RealT(0.0));
            }
        }

        for (int m = high - 1; m >= low + 1; m--)
        {
            if (mH[m][m - 1] != RealT(0.0))
            {
                for (int i = m + 1; i <= high; i++)
                {
                    mOrt[i] = mH[i][m - 1];
                }
                for (int j = m; j <= high; j++)
                {
                    RealT g = RealT(0.0);
                    for (int i = m; i <= high; i++)
                    {
                        g += mOrt[i] * mV[i][j];
                    }
                    // Double division avoids possible underflow
                    g = (g / mOrt[m]) / mH[m][m - 1];
                    for (int i = m; i <= high; i++)
                    {
                        mV[i][j] += g * mOrt[i];
                    }
                }
            }
        }
    }

    // Complex scalar division.
    static
    void cdiv(RealT xr,
              RealT xi,
              RealT yr,
              RealT yi,
              RealT& cdivr,
              RealT& cdivi)
    {
        RealT r, mD;
        if (std::abs(yr) > std::abs(yi))
        {
            r = yi / yr;
            mD = yr + r * yi;
            cdivr = (xr + r * xi) / mD;
            cdivi = (xi - r * xr) / mD;
        }
        else
        {
            r = yr / yi;
            mD = yi + r * yr;
            cdivr = (r * xr + xi) / mD;
            cdivi = (r * xi - xr) / mD;
        }
    }

    // Nonsymmetric reduction from Hessenberg to real Schur form.

    void hqr2()
    {

        //  This is derived from the Algol procedure hqr2,
        //  by Martin and Wilkinson, Handbook for Auto. Comp.,
        //  Vol.ii-Linear Algebra, and the corresponding
        //  Fortran subroutine in EISPACK.

        // Initialize

        int nn = this->mN_;
        int mN = nn - 1;
        int low = 0;
        int high = nn - 1;
        RealT eps = pow(2.0, -52.0);
        RealT exshift = RealT(0.0);
        RealT p = 0, q = 0, r = 0, s = 0, z = 0, t, w, x, y;

        // Store roots isolated by balanc and compute matrix norm

        RealT norm = RealT(0.0);
        for (int i = 0; i < nn; i++)
        {
            if ((i < low) || (i > high))
            {
                mD[i] = mH[i][i];
                mE[i] = RealT(0.0);
            }
            for (int j = std::max<int>(i - 1, 0); j < nn; j++)
            {
                norm = norm + std::abs(mH[i][j]);
            }
        }

        // Outer loop over eigenvalue index

        int iter = 0;
        while (mN >= low)
        {

            // Look for single small sub-diagonal element

            int l = mN;
            while (l > low)
            {
                s = std::abs(mH[l - 1][l - 1]) + std::abs(mH[l][l]);
                if (s == RealT(0.0))
                {
                    s = norm;
                }
                if (std::abs(mH[l][l - 1]) < eps * s)
                {
                    break;
                }
                l--;
            }

            // Check for convergence
            // One root found

            if (l == mN)
            {
                mH[mN][mN] = mH[mN][mN] + exshift;
                mD[mN] = mH[mN][mN];
                mE[mN] = RealT(0.0);
                mN--;
                iter = 0;

                // Two roots found

            }
            else if (l == mN - 1)
            {
                w = mH[mN][mN - 1] * mH[mN - 1][mN];
                p = (mH[mN - 1][mN - 1] - mH[mN][mN]) / 2.0;
                q = p * p + w;
                z = sqrt(std::abs(q));
                mH[mN][mN] = mH[mN][mN] + exshift;
                mH[mN - 1][mN - 1] = mH[mN - 1][mN - 1] + exshift;
                x = mH[mN][mN];

                // RealT pair

                if (q >= 0)
                {
                    if (p >= 0)
                    {
                        z = p + z;
                    }
                    else
                    {
                        z = p - z;
                    }
                    mD[mN - 1] = x + z;
                    mD[mN] = mD[mN - 1];
                    if (z != RealT(0.0))
                    {
                        mD[mN] = x - w / z;
                    }
                    mE[mN - 1] = RealT(0.0);
                    mE[mN] = RealT(0.0);
                    x = mH[mN][mN - 1];
                    s = std::abs(x) + std::abs(z);
                    p = x / s;
                    q = z / s;
                    r = sqrt(p * p + q * q);
                    p = p / r;
                    q = q / r;

                    // Row modification

                    for (int j = mN - 1; j < nn; j++)
                    {
                        z = mH[mN - 1][j];
                        mH[mN - 1][j] = q * z + p * mH[mN][j];
                        mH[mN][j] = q * mH[mN][j] - p * z;
                    }

                    // Column modification

                    for (int i = 0; i <= mN; i++)
                    {
                        z = mH[i][mN - 1];
                        mH[i][mN - 1] = q * z + p * mH[i][mN];
                        mH[i][mN] = q * mH[i][mN] - p * z;
                    }

                    // Accumulate transformations

                    for (int i = low; i <= high; i++)
                    {
                        z = mV[i][mN - 1];
                        mV[i][mN - 1] = q * z + p * mV[i][mN];
                        mV[i][mN] = q * mV[i][mN] - p * z;
                    }

                    // Complex pair

                }
                else
                {
                    mD[mN - 1] = x + p;
                    mD[mN] = x + p;
                    mE[mN - 1] = z;
                    mE[mN] = -z;
                }
                mN = mN - 2;
                iter = 0;

                // No convergence yet

            }
            else
            {

                // Form shift

                x = mH[mN][mN];
                y = RealT(0.0);
                w = RealT(0.0);
                if (l < mN)
                {
                    y = mH[mN - 1][mN - 1];
                    w = mH[mN][mN - 1] * mH[mN - 1][mN];
                }

                // Wilkinson's original ad hoc shift

                if (iter == 10)
                {
                    exshift += x;
                    for (int i = low; i <= mN; i++)
                    {
                        mH[i][i] -= x;
                    }
                    s = std::abs(mH[mN][mN - 1]) + std::abs(mH[mN - 1][mN - 2]);
                    x = y = 0.75 * s;
                    w = -0.4375 * s * s;
                }

                // MATLAB's new ad hoc shift

                if (iter == 30)
                {
                    s = (y - x) / 2.0;
                    s = s * s + w;
                    if (s > 0)
                    {
                        s = sqrt(s);
                        if (y < x)
                        {
                            s = -s;
                        }
                        s = x - w / ((y - x) / 2.0 + s);
                        for (int i = low; i <= mN; i++)
                        {
                            mH[i][i] -= s;
                        }
                        exshift += s;
                        x = y = w = 0.964;
                    }
                }

                iter = iter + 1; // (Could check iteration count here.)

                // Look for two consecutive small sub-diagonal elements

                int m = mN - 2;
                while (m >= l)
                {
                    z = mH[m][m];
                    r = x - z;
                    s = y - z;
                    p = (r * s - w) / mH[m + 1][m] + mH[m][m + 1];
                    q = mH[m + 1][m + 1] - z - r - s;
                    r = mH[m + 2][m + 1];
                    s = std::abs(p) + std::abs(q) + std::abs(r);
                    p = p / s;
                    q = q / s;
                    r = r / s;
                    if (m == l)
                    {
                        break;
                    }
                    if (std::abs(mH[m][m - 1]) * (std::abs(q) + std::abs(r)) <
                        eps * (std::abs(p) *
                            (std::abs(mH[m - 1][m - 1]) + std::abs(z) +
                             std::abs(mH[m + 1][m + 1]))))
                    {
                        break;
                    }
                    m--;
                }

                for (int i = m + 2; i <= mN; i++)
                {
                    mH[i][i - 2] = RealT(0.0);
                    if (i > m + 2)
                    {
                        mH[i][i - 3] = RealT(0.0);
                    }
                }

                // Double QR step involving rows l:mN and columns m:mN

                for (int k = m; k <= mN - 1; k++)
                {
                    int notlast = (k != mN - 1);
                    if (k != m)
                    {
                        p = mH[k][k - 1];
                        q = mH[k + 1][k - 1];
                        r = (notlast ? mH[k + 2][k - 1] : RealT(0.0));
                        x = std::abs(p) + std::abs(q) + std::abs(r);
                        if (x != RealT(0.0))
                        {
                            p = p / x;
                            q = q / x;
                            r = r / x;
                        }
                    }
                    if (x == RealT(0.0))
                    {
                        break;
                    }
                    s = sqrt(p * p + q * q + r * r);
                    if (p < 0)
                    {
                        s = -s;
                    }
                    if (s != 0)
                    {
                        if (k != m)
                        {
                            mH[k][k - 1] = -s * x;
                        }
                        else if (l != m)
                        {
                            mH[k][k - 1] = -mH[k][k - 1];
                        }
                        p = p + s;
                        x = p / s;
                        y = q / s;
                        z = r / s;
                        q = q / p;
                        r = r / p;

                        // Row modification

                        for (int j = k; j < nn; j++)
                        {
                            p = mH[k][j] + q * mH[k + 1][j];
                            if (notlast)
                            {
                                p = p + r * mH[k + 2][j];
                                mH[k + 2][j] = mH[k + 2][j] - p * z;
                            }
                            mH[k][j] = mH[k][j] - p * x;
                            mH[k + 1][j] = mH[k + 1][j] - p * y;
                        }

                        // Column modification

                        for (int i = 0; i <= std::min<int>(mN, k + 3); i++)
                        {
                            p = x * mH[i][k] + y * mH[i][k + 1];
                            if (notlast)
                            {
                                p = p + z * mH[i][k + 2];
                                mH[i][k + 2] = mH[i][k + 2] - p * r;
                            }
                            mH[i][k] = mH[i][k] - p;
                            mH[i][k + 1] = mH[i][k + 1] - p * q;
                        }

                        // Accumulate transformations

                        for (int i = low; i <= high; i++)
                        {
                            p = x * mV[i][k] + y * mV[i][k + 1];
                            if (notlast)
                            {
                                p = p + z * mV[i][k + 2];
                                mV[i][k + 2] = mV[i][k + 2] - p * r;
                            }
                            mV[i][k] = mV[i][k] - p;
                            mV[i][k + 1] = mV[i][k + 1] - p * q;
                        }
                    } // (s != 0)
                } // k loop
            } // check convergence
        } // while (mN >= low)

        // Backsubstitute to find vectors of upper triangular form

        if (norm == RealT(0.0))
        {
            return;
        }

        for (mN = nn - 1; mN >= 0; mN--)
        {
            p = mD[mN];
            q = mE[mN];

            // RealT vector

            if (q == 0)
            {
                int l = mN;
                mH[mN][mN] = RealT(1.0);
                for (int i = mN - 1; i >= 0; i--)
                {
                    w = mH[i][i] - p;
                    r = RealT(0.0);
                    for (int j = l; j <= mN; j++)
                    {
                        r = r + mH[i][j] * mH[j][mN];
                    }
                    if (mE[i] < RealT(0.0))
                    {
                        z = w;
                        s = r;
                    }
                    else
                    {
                        l = i;
                        if (mE[i] == RealT(0.0))
                        {
                            if (w != RealT(0.0))
                            {
                                mH[i][mN] = -r / w;
                            }
                            else
                            {
                                mH[i][mN] = -r / (eps * norm);
                            }

                            // Solve real equations

                        }
                        else
                        {
                            x = mH[i][i + 1];
                            y = mH[i + 1][i];
                            q = (mD[i] - p) * (mD[i] - p) + mE[i] * mE[i];
                            t = (x * s - z * r) / q;
                            mH[i][mN] = t;
                            if (std::abs(x) > std::abs(z))
                            {
                                mH[i + 1][mN] = (-r - w * t) / x;
                            }
                            else
                            {
                                mH[i + 1][mN] = (-s - y * t) / z;
                            }
                        }

                        // Overflow control

                        t = std::abs(mH[i][mN]);
                        if ((eps * t) * t > 1)
                        {
                            for (int j = i; j <= mN; j++)
                            {
                                mH[j][mN] = mH[j][mN] / t;
                            }
                        }
                    }
                }

                // Complex vector

            }
            else if (q < 0)
            {
                int l = mN - 1;

                // Last vector component imaginary so matrix is triangular

                if (std::abs(mH[mN][mN - 1]) > std::abs(mH[mN - 1][mN]))
                {
                    mH[mN - 1][mN - 1] = q / mH[mN][mN - 1];
                    mH[mN - 1][mN] = -(mH[mN][mN] - p) / mH[mN][mN - 1];
                }
                else
                {
                    RealT cdivr;
                    RealT cdivi;
                    cdiv(RealT(0.0),
                         -mH[mN - 1][mN],
                         mH[mN - 1][mN - 1] - p,
                         q,
                         cdivr,
                         cdivi);
                    mH[mN - 1][mN - 1] = cdivr;
                    mH[mN - 1][mN] = cdivi;
                }
                mH[mN][mN - 1] = RealT(0.0);
                mH[mN][mN] = RealT(1.0);
                for (int i = mN - 2; i >= 0; i--)
                {
                    RealT ra, sa, vr, vi;
                    ra = RealT(0.0);
                    sa = RealT(0.0);
                    for (int j = l; j <= mN; j++)
                    {
                        ra = ra + mH[i][j] * mH[j][mN - 1];
                        sa = sa + mH[i][j] * mH[j][mN];
                    }
                    w = mH[i][i] - p;

                    if (mE[i] < RealT(0.0))
                    {
                        z = w;
                        r = ra;
                        s = sa;
                    }
                    else
                    {
                        l = i;
                        if (mE[i] == 0)
                        {
                            RealT cdivr;
                            RealT cdivi;
                            cdiv(-ra, -sa, w, q, cdivr, cdivi);
                            mH[i][mN - 1] = cdivr;
                            mH[i][mN] = cdivi;
                        }
                        else
                        {

                            // Solve complex equations

                            x = mH[i][i + 1];
                            y = mH[i + 1][i];
                            vr = (mD[i] - p) * (mD[i] - p) + mE[i] * mE[i]
                                            - q * q;
                            vi = (mD[i] - p) * 2.0 * q;
                            if ((vr == RealT(0.0)) && (vi == RealT(0.0)))
                            {
                                vr = eps * norm *
                                    (std::abs(w) + std::abs(q) + std::abs(x) +
                                     std::abs(y) + std::abs(z));
                            }

                            RealT cdivr;
                            RealT cdivi;
                            cdiv(x * r - z * ra + q * sa,
                                 x * s - z * sa - q * ra,
                                 vr,
                                 vi,
                                 cdivr,
                                 cdivi);

                            mH[i][mN - 1] = cdivr;
                            mH[i][mN] = cdivi;
                            if (std::abs(x) > (std::abs(z) + std::abs(q)))
                            {
                                mH[i + 1][mN - 1] = (-ra - w * mH[i][mN - 1]
                                                + q * mH[i][mN]) / x;
                                mH[i + 1][mN] = (-sa - w * mH[i][mN]
                                                - q * mH[i][mN - 1]) / x;
                            }
                            else
                            {
                                cdiv(-r - y * mH[i][mN - 1],
                                     -s - y * mH[i][mN],
                                     z,
                                     q,
                                     cdivr,
                                     cdivi);

                                mH[i + 1][mN - 1] = cdivr;
                                mH[i + 1][mN] = cdivi;
                            }
                        }

                        // Overflow control

                        t = std::max<RealT>(std::abs(mH[i][mN - 1]),
                                        std::abs(mH[i][mN]));
                        if ((eps * t) * t > 1)
                        {
                            for (int j = i; j <= mN; j++)
                            {
                                mH[j][mN - 1] = mH[j][mN - 1] / t;
                                mH[j][mN] = mH[j][mN] / t;
                            }
                        }
                    }
                }
            }
        }

        // Vectors of isolated roots

        for (int i = 0; i < nn; i++)
        {
            if (i < low || i > high)
            {
                for (int j = i; j < nn; j++)
                {
                    mV[i][j] = mH[i][j];
                }
            }
        }

        // Back transformation to get eigenvectors of original matrix

        for (int j = nn - 1; j >= low; j--)
        {
            for (int i = low; i <= high; i++)
            {
                z = RealT(0.0);
                for (int k = low; k <= std::min<int>(j, high); k++)
                {
                    z = z + mV[i][k] * mH[k][j];
                }
                mV[i][j] = z;
            }
        }
    }

    static
    bool isSymmetric(const Matrix2D<RealT>& A)
    {
        for (size_t row = 0; row < A.rows(); ++row)
        {
            for (size_t col = 0; col < A.cols(); ++col)
            {
                if (A[row][col] != A[col][row])
                {
                    return false;
                }
            }
        }

        return true;
    }

private:
    // Row and column dimension (square matrix).
    const int mN_;

    // Arrays for internal storage of eigenvalues.
    Vector<RealT> mD; // real part
    Vector<RealT> mE; // imag part

    // Array for internal storage of eigenvectors.
    Matrix2D<RealT> mV;

    // Array for internal storage of nonsymmetric Hessenberg form.
    Matrix2D<RealT> mH;

    // Working storage for nonsymmetric algorithm.
    Vector<RealT> mOrt;
};
}
}

#endif
