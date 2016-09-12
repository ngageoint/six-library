#ifndef __MATH_POLY_FIT_H__
#define __MATH_POLY_FIT_H__

#include <math/poly/OneD.h>
#include <math/poly/TwoD.h>
#include <math/linear/Matrix2D.h>
#include <math/linear/VectorN.h>
#include <sys/Conf.h>
#include <except/Exception.h>

#include <numeric>
#include <sstream>

namespace math
{
namespace poly
{

/*!
 *  Templated function to perform a linear least squares fit for the data.
 *  This algorithm is fairly straightforward.
 *
 *  To fit an order N polynomial, we need to solve
 *  Ax=b, for x.
 *
 *  A is a system of polynomials, e.g.
 *
 *  f(x) = c0 + c1*x + c2*x^2 + c3*x^3 = y
 *  
 *  Each observed point in the data sets is computed
 *  for our A matrix.
 *
 *  e.g.: f(1) = 3, f(-1) = 13, f(2) 1, f(-2) = 33
 *
 *  | 1  1  1  1 || c0 |   |  3 |
 *  | 1 -1  1 -1 || c1 | = | 13 |
 *  | 1  2  4  8 || c2 |   |  1 |
 *  | 1 -2  4 -8 || c3 |   | 33 |
 *
 *  
 *  Linear least squares solution for system where
 *  ker(A) = {0} (IOW, there are free variables)
 *
 *  x = inv(A' * A) * A' * b
 *
 *  \param x The observable x points
 *  \param y The observable y solutions
 *  \param order The desired order of the polynomial fit
 *  \return A one dimensional polynomial that fits the curve
 */
 
template<typename Vector_T> OneD<double> fit(const Vector_T& x,
                                             const Vector_T& y,
                                             size_t order)
{
    math::linear::Vector<double> vx(x);
    math::linear::Vector<double> vy(y);
    // n is polynomial order
    size_t sizeX = vx.size();

    if (sizeX <= order)
    {
        std::ostringstream excSS;
        excSS << "Not enough points for a unique fit solution ("
              << sizeX << " points for an order-" << order
              << "fit)!  You should really have at least (order+1) = "
              << (order+1) << " points for this to do what you expect.";
        throw except::Exception(Ctxt(excSS.str()));
    }
    
    // Compute mean value
    double mean = std::accumulate(vx.get(), vx.get() + sizeX, 0.0) / sizeX;

    // Shift the vector values by mean to center around zero
    math::linear::Vector<double> offv(sizeX, mean);
    math::linear::Vector<double> xp = vx - offv;

    // Normalize the values in the vector using standard deviation
    double rxrms = 1 / std::sqrt(xp.normSq() / sizeX);
    xp.scale(rxrms);

    math::linear::Matrix2D<double> A(sizeX, order + 1);

    for (size_t i = 0; i < sizeX; i++)
    {
        // The c0 coefficient is a freebie
        A(i, 0) = 1;
        double v = xp[i];
        A(i, 1) = v;
        for (size_t j = 2; j <= order; j++)
        {
            A(i, j) = std::pow(v, (double)j);
        }
    }
    
    math::linear::Matrix2D<double> At = A.transpose();
    math::linear::Matrix2D<double> inv = inverse(At * A);
    math::linear::Matrix2D<double> B = inv * At;
    math::linear::Vector<double> c(B * vy.matrix());

    // Now we need the order+1 components out for our poly
    math::poly::OneD<double> poly(c.size());

    // Remove the normalization scaling
    double xacc = 1;
    for (size_t i = 0; i < c.size(); i++)
    {
        poly[i] = c[i] * xacc;
        xacc *= rxrms;
    }

    // Shift the polynomial back from its centered offset
    math::poly::OneD<double> shift(1);
    shift[0] = -mean;
    shift[1] = 1;

    return poly.transformInput(shift);
}


/*!
 *  This method allows us to fit a set of observations using raw
 *  pointers
 */
inline OneD<double> fit(size_t numObs, const double* x, const double* y, 
            size_t order)
{
    math::linear::Vector<double> xv(numObs, x);
    math::linear::Vector<double> yv(numObs, y);
    return math::poly::fit<math::linear::Vector<double> >(xv, yv, order);
}


/*!
 *  Two-dimensional linear least squares fit
 *  To make sure that one dimension does not dominate the other,
 *  we normalize the x and y matrices.
 *
 *  The x, y and z matrices must all be the same size, and the
 *  x(i, j) point in X must correspond to y(i, j) in Y
 *
 *  \param x Input x coordinate
 *  \param y Input y coordinates
 *  \param z Observed outputs
 *  \param nx The requested order X of the output poly
 *  \param ny The requested order Y of the output poly
 *  \throw Exception if matrices are not equally sized
 *  \return A polynomial, f(x, y) = z
 */

inline math::poly::TwoD<double> fit(const math::linear::Matrix2D<double>& x,
                    const math::linear::Matrix2D<double>& y,
                    const math::linear::Matrix2D<double>& z,
                    size_t nx,
                    size_t ny)
{
    size_t m = x.rows();
    size_t n = x.cols();
    size_t mxn = x.size();

    if (m != y.rows())
        throw except::Exception(Ctxt("Matrices must be equally sized"));

    if (n != y.cols())
        throw except::Exception(Ctxt("Matrices must be equally sized"));

    // Compute mean values
    double xoff = std::accumulate(x.get(), x.get() + mxn, 0.0) / mxn;
    double yoff = std::accumulate(y.get(), y.get() + mxn, 0.0) / mxn;

    // Shift the matrix values by mean to center around zero
    math::linear::Matrix2D<double> xoffm(m, n, xoff);
    math::linear::Matrix2D<double> yoffm(m, n, yoff);

    math::linear::Matrix2D<double> xp = x - xoffm;
    math::linear::Matrix2D<double> yp = y - yoffm;

    // Normalize the values in the matrix using standard deviation
    double rxrms = 1 / std::sqrt(xp.normSq() / mxn);
    double ryrms = 1 / std::sqrt(yp.normSq() / mxn);
    xp.scale(rxrms);
    yp.scale(ryrms);

    size_t acols = (nx+1) * (ny+1);

    if (mxn < acols)
    {
        std::ostringstream excSS;
        excSS << "Not enough points for a unique fit solution ("
              << mxn << " points for a " << acols << "-coefficient fit)!"
              << " You should really have at least (orderX+1)*(orderY+1) = "
              << acols << " points for this to do what you expect.";
        throw except::Exception(Ctxt(excSS.str()));
    }
    

    // R = M x N
    // C = NX+1 x NY+1

    // size(A) = R x P
    math::linear::Matrix2D<double> A(mxn, acols);
    
    for (size_t i = 0; i < m; i++)
    {
        size_t xidx = i*n;
        for (size_t j = 0; j < n; j++, xidx++)
        {
            // We are doing an accumulation of pow()s to get this

            // Pre-calculate these
            double xij = xp(i, j);
            double yij = yp(i, j);

            double xacc = 1;

            for (size_t k = 0; k <= nx; k++)
            {
                size_t yidx = k * (ny + 1);
                double yacc = 1;
                
                for (size_t l = 0; l <= ny; l++)
                {

                    A(xidx, yidx) = xacc * yacc;
                    yacc *= yij;
                    ++yidx;
                }
                xacc *= xij;
            }
        }
    }
    
    // size(tmp) = R x 1
    math::linear::Matrix2D<double> tmp(mxn, 1);

    for (size_t i = 0; i < m; i++)
    {
        size_t xidx = i*n;
        for (size_t j = 0; j < n; j++, xidx++)
        {
            tmp(xidx, 0) = z(i, j);
        }
    }
    
    math::linear::Matrix2D<double> At = A.transpose();

    //       T
    // size(A  A) = (P x R) (R x P) = (P x P)
    // size(inv) = (P x P)
    math::linear::Matrix2D<double> inv = math::linear::inverse<double>(At * A);

    // size(C) = ((P x P) (P x R))(R x 1)
    //         =   (P x R)(R x 1)
    //         =   (P x 1)
    //         =   (NX+1xNY+1 x 1)

    math::linear::Matrix2D<double> C = inv * At * tmp;

    // Now we need the NX+1 components out for our x coeffs
    // and NY+1 components out for our y coeffs
    math::poly::TwoD<double> coeffs(nx, ny);

    // Remove the normalization scaling
    double xacc = 1;
    size_t p = 0;
    for (size_t i = 0; i <= nx; i++)
    {
        double yacc = 1;
        for (size_t j = 0; j <= ny; j++)
        {
            coeffs[i][j] = C(p, 0)*(xacc * yacc);
            ++p;
            yacc *= ryrms;
        }
        xacc *= rxrms;
    }

    // Shift the polynomial back from its centered offset
    math::poly::TwoD<double> xShift(1, 1);
    math::poly::TwoD<double> yShift(1, 1);
    xShift[0][0] = -xoff;
    xShift[1][0] = 1;
    yShift[0][0] = -yoff;
    yShift[0][1] = 1;

    return coeffs.transformInput(xShift, yShift);
}

inline math::poly::TwoD<double> fit(size_t numRows,
                    size_t numCols,
                    const double* x,
                    const double* y,
                    const double* z,
                    size_t nx,
                    size_t ny)
{
    math::linear::Matrix2D<double> xm(numRows, numCols, x);
    math::linear::Matrix2D<double> ym(numRows, numCols, y);
    math::linear::Matrix2D<double> zm(numRows, numCols, z);

    return fit(xm, ym, zm, nx, ny);
}

/*!
 *  Perform three 1D fits using the same set of observed X values, and return a 1D polynomial with
 *  a Vector of 3 coefficients for each term.
 *
 *  \param xObs Observed x values
 *  \param yObs0 First set of observed y values
 *  \param yObs1 Second set of observed y values
 *  \param yObs2 Third set of observed y values
 *  \param order The desired order of the polynomial fit
 *  \throw Exception if all input Vectors are not equally sized
 *  \return A polynomial (B01, B02, B03)x^0 + (B11, B12, B13)x^1 + ... + (Bn1, Bn2, Bn3)x^n
 */

inline math::poly::OneD< math::linear::VectorN< 3, double > > fit(
    const math::linear::Vector<double>& xObs,
    const math::linear::Vector<double>& yObs0,
    const math::linear::Vector<double>& yObs1,
    const math::linear::Vector<double>& yObs2,
    size_t order)
{
    size_t numObs = xObs.size();
    if (yObs0.size() != numObs || yObs1.size() != numObs || yObs2.size() != numObs)
    {
        throw except::Exception(Ctxt("Must have the same number of observed y values as observed x values"));
    }

    const math::poly::OneD<double> fit0 = fit(xObs, yObs0, order);
    const math::poly::OneD<double> fit1 = fit(xObs, yObs1, order);
    const math::poly::OneD<double> fit2 = fit(xObs, yObs2, order);

    math::poly::OneD< math::linear::VectorN< 3, double > > polyVector3 =
        math::poly::OneD< math::linear::VectorN< 3, double > >(order);
    for (size_t term = 0; term <= order; term++)
    {
        math::linear::VectorN< 3, double >& coeffs = polyVector3[term];
        coeffs[0] = fit0[term];
        coeffs[1] = fit1[term];
        coeffs[2] = fit2[term];
    }

    return polyVector3;
}

/*!
 *  Perform three 1D fits using the same set of observed X values, and return a 1D polynomial with
 *  a Vector of 3 coefficients for each term.
 *
 *  \param xObsVector Observed x values
 *  \param yObsMatrix Matrix with each row as a set of observed y values
 *  \param order The desired order of the polynomial fit
 *  \throw Exception if the matrix doesn't have 3 sets of values, i.e. 3 rows
 *  \return A polynomial (B01, B02, B03)x^0 + (B11, B12, B13)x^1 + ... + (Bn1, Bn2, Bn3)x^n
 */

inline math::poly::OneD< math::linear::VectorN< 3, double > > fit(
    const math::linear::Vector<double>& xObsVector,
    const math::linear::Matrix2D<double>& yObsMatrix,
    size_t order)
{
    if (yObsMatrix.rows() != 3)
    {
        throw except::Exception(Ctxt("Matrix of observed Y values must have 3 rows"));
    }

    // Vector size error checking will be done by the base fit() function
    const size_t numObs = yObsMatrix.cols();
    math::linear::Vector<double> yObsVector0 = math::linear::Vector<double>(numObs, yObsMatrix.row(0));
    math::linear::Vector<double> yObsVector1 = math::linear::Vector<double>(numObs, yObsMatrix.row(1));
    math::linear::Vector<double> yObsVector2 = math::linear::Vector<double>(numObs, yObsMatrix.row(2));

    math::poly::OneD< math::linear::VectorN< 3, double > > polyVector3 = fit(
        xObsVector,
        yObsVector0,
        yObsVector1,
        yObsVector2,
        order);
    return polyVector3;
}

/*!
 *  Perform three 1D fits using the same set of observed X values, and return a 1D polynomial with
 *  a Vector of 3 coefficients for each term.
 *
 *  \param xObs Observed x values
 *  \param yObs0 First set of observed y values
 *  \param yObs1 Second set of observed y values
 *  \param yObs2 Third set of observed y values
 *  \param order The desired order of the polynomial fit
 *  \return A polynomial (B01, B02, B03)x^0 + (B11, B12, B13)x^1 + ... + (Bn1, Bn2, Bn3)x^n
 */

inline math::poly::OneD< math::linear::VectorN< 3, double > > fit(
        const std::vector<double>& xObs,
        const std::vector<double>& yObs0,
        const std::vector<double>& yObs1,
        const std::vector<double>& yObs2,
        size_t order)
{
    // Vector size error checking will be done by the base fit() function
    math::poly::OneD< math::linear::VectorN< 3, double > > polyVector3 = fit(
        math::linear::Vector<double>(xObs),
        math::linear::Vector<double>(yObs0),
        math::linear::Vector<double>(yObs1),
        math::linear::Vector<double>(yObs2),
        order);
    return polyVector3;
}
}
}

#endif
