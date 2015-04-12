/* =========================================================================
 * This file is part of math.poly-c++ 
 * =========================================================================
 * 
 * (C) Copyright 2004 - 2009, General Dynamics - Advanced Information Systems
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

#include <import/math/poly.h>
using namespace math::poly;

TwoD<double>cascade(TwoD<double>oldP, TwoD<double>gx, TwoD<double>gy)
{

    TwoD<double>newP(oldP.orderX(), oldP.orderY());

    for (int i = 0; i <= oldP.orderX(); i++)
    {
        for (int j = 0; j <= oldP.orderY(); j++)
        {
            newP += (gx.power(i) * (gy.power(j) * oldP[i][j]));
        }
    }

    return newP;
}

int main()
{

    try
    {
        TwoD<double>f(1, 1);
        // 5 + xy
        f[0][0] = 5;
        f[1][1] = 1;

        std::cout << "f(x, y):" << std::endl << f << std::endl;
        TwoD<double>h(2, 2);
        h[0][0] = 6;
        h[1][2] = 3;
        std::cout << "h(x, y)" << std::endl << h << std::endl;

        TwoD<double>gx(1, 1);
        gx[0][1] = gx[1][0] = 1;
        std::cout << "gx(x, y):" << std::endl << gx << std::endl;

        TwoD<double>gy(2, 2);
        gy[2][0] = 1;
        std::cout << "gy(x, y):" << std::endl << gy << std::endl;

        TwoD<double>fp = cascade(f, gx, gy);

        std::cout << "fp(x, y):" << std::endl << fp << std::endl;

        TwoD<double>hp = cascade(h, gx, gy);

        std::cout << "hp(x, y):" << std::endl << hp << std::endl;

    }
    catch (except::Exception& ex)
    {
        std::cout << "Exception: " << ex.toString() << std::endl;
    }
    return 0;
}
