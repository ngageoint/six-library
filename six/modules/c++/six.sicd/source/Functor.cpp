/* =========================================================================
* This file is part of six.sicd-c++
* =========================================================================
*
* (C) Copyright 2004 - 2016, MDA Information Systems LLC
*
* six.sicd-c++ is free software; you can redistribute it and/or modify
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
#include <iostream>
#include <vector>
//#include <limits>
#include <six/sicd/Functor.h>

namespace six
{
namespace sicd
{

double bessi(size_t order, double x)
{
    switch (order)
    {
        case 0: 
            return bessi0(x);

        case 1:
            return bessi1(x);

        default:
            return bessin(order, x);
    }
}

double bessi0(double x)
{
    double ax = std::fabs(x);
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
double bessi1(double x)
{
    double ax = std::fabs(x);
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

double bessin(size_t order, double x)
{
    const double ACC = 200;
    const int IEXP = std::numeric_limits<double>::max_exponent / 2;
    int k;
    double bm, dum;

    if (x * x <= 8.0 * std::numeric_limits<double>::min())
    {
        return 0;
    }

    double tox = 2.0 / std::fabs(x);
    double bip = 0;
    double ans = 0;
    double bi = 1.0;

    //Downward recurrence from even n
    for (size_t jj = 2 * (order + int(std::sqrt(ACC * order))); jj > 0; jj--)
    {
        double bim = bip + (jj * tox * bi);
        bip = bi;
        bi = bim;
        dum = std::frexp(bi, &k);

        //Renormalize to prevent overflow
        if (k > IEXP)
        {
            ans = std::ldexp(ans, -IEXP);
            bi = std::ldexp(bi, -IEXP);
            bip = ldexp(bip, -IEXP);
        }
        if (jj == order)
        {
            ans = bip;
        }
    }
    //Normalize with bessi0
    ans *= bessi0(x) / bi;  
    return x < 0 && (order & 1) ? -ans : ans;
}

RaisedCos::RaisedCos(double coef) :
    mCoef(coef)
{
}

std::vector<double> RaisedCos::operator()(size_t n) const
{
    std::vector<double> ret;

    for (size_t ii = 0; ii < std::ceil(n/2.0); ++ii)
    {
        ret.push_back((1 - mCoef) * std::cos(2 * M_PI * ii) / (n - 1));
    }
    if (n % 2 == 0)
    {
        std::copy(std::begin(ret), std::end(ret), std::back_inserter(ret));
    }
    else
    {
        std::copy(std::begin(ret), std::end(ret) - 1, std::back_inserter(ret));
    }
    return ret;
}

Kaiser::Kaiser(double beta) :
    beta(beta)
{
}

std::vector<double> Kaiser::operator()(size_t L) const
{
    std::vector<double> ret;

    if (L == 1)
    {
        ret.push_back(1);
        return ret;
    }
    size_t m = L - 1;
    double k;
    for (size_t ii = 0; ii < L; ++ii)
    {
        k = (2 * beta / m * std::sqrt(ii * m - ii));
        ret.push_back(bessi(0, k) / bessi(0, beta));
    }

    return ret;
}
}
}
