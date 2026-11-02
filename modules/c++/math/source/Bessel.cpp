/* =========================================================================
 * This file is part of math-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2016, MDA Information Systems LLC
 *
 * math-c++ is free software; you can redistribute it and/or modify
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
#include <tuple>

#include <math/Bessel.h>

namespace math
{
/*!
 * Modified Bessel function of the first kind
 * This function wraps the following order-specific functions
 */
double besselI(size_t order, double x)
{
    switch (order)
    {
        case 0:
            return besselIOrderZero(x);

        case 1:
            return besselIOrderOne(x);

        default:
            return besselIOrderN(order, x);
    }
}

/*!
 * Modified Bessel function of the first kind, order 0
 */
double besselIOrderZero(double x)
{
    double ax = std::abs(x);
    double ans;
    double y;

    if (ax < 3.75)
    {
        //Polynomial fit
        y = x / 3.75;
        y *= y;
        ans = 1.0 + y * (3.5156229 +
            y * (3.0899424 +
                y*(1.2067492 +
                    y * (0.2659732 +
                        y * (0.360768e-1 +
                            y * (0.45813e-2))))));
    }
    else
    {
        y = 3.75 / ax;
        ans = (std::exp(ax) / std::sqrt(ax)) *
            (0.39894228 +
                y * (0.1328592e-1 +
                    y * (0.225319e-2 +
                        y * (-0.157565e-2 +
                            y * (0.916281e-2 +
                                y * (-0.2057706e-1 +
                                    y * (0.2635537e-1 +
                                        y * (-0.1647633e-1 +
                                            y * (0.392377e-2)))))))));
    }
    return ans;
}

/*!
 * Modified Bessel function of the first kind, order 1
 */
double besselIOrderOne(double x)
{
    double ax = std::abs(x);
    double ans;
    double y;

    if (ax < 3.75)
    {
        // polynomial fit
        y = x / 3.75;
        y *= y;
        ans = ax * (0.5 +
            y * (0.87890594 +
                 y * (0.51498869 +
                     y * (0.15084934 +
                         y * (0.2658733e-1 +
                             y * (0.301532e-2 +
                                 y * (0.32411e-3)))))));
     }
     else
     {
         y = 3.75 / ax;
         ans = 0.2282967e-1 +
             y * (-0.2895312e-1 +
                 y * (0.1787654e-1 -
                     y * 0.420059e-2));

         // Note that ans is a term at the very end of this expression
         ans = 0.39894228 +
             y * (-0.3988024e-1 +
                 y * (-0.362018e-2 +
                     y * (0.163801e-2 +
                         y * (-0.1031555e-1 +
                             y * ans))));

         ans *= (std::exp(ax) / std::sqrt(ax));
     }
     return x < 0.0 ? -ans : ans;
}

/*!
 * Modified Bessel function of the first kind, order n > 1
 */
double besselIOrderN(size_t order, double x)
{
    const double ACC = 200;
    const int IEXP = std::numeric_limits<double>::max_exponent / 2;
    int k;

    if (x * x <= 8.0 * std::numeric_limits<double>::min())
    {
        return 0;
    }

    double tox = 2.0 / std::abs(x);
    double bip = 0;
    double ans = 0;
    double bi = 1.0;

    //Downward recurrence from even n
    for (size_t jj = 2 * (order + int(std::sqrt(ACC * static_cast<double>(order)))); jj > 0; jj--)
    {
        double bim = bip + (static_cast<double>(jj) * tox * bi);
        bip = bi;
        bi = bim;
        std::ignore = std::frexp(bi, &k);

        //Renormalize to prevent overflow
        if (k > IEXP)
        {
            ans = std::ldexp(ans, -IEXP);
            bi = std::ldexp(bi, -IEXP);
            bip = std::ldexp(bip, -IEXP);
        }
        if (jj == order)
        {
            ans = bip;
        }
    }
    //Normalize
    ans *= besselIOrderZero(x) / bi;
    return x < 0 && (order & 1) ? -ans : ans;
}
}

