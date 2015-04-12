/* =========================================================================
 * This file is part of math.linear-c++ 
 * =========================================================================
 * 
 * (C) Copyright 2004 - 2009, General Dynamics - Advanced Information Systems
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
#include <import/math/linear.h>

namespace mx=math::linear;

typedef std::complex<float> _Cf;
typedef mx::MatrixMxN<2, 2, _Cf> _2x2;
typedef mx::MatrixMxN<3, 2, _Cf> _3x2;
typedef mx::MatrixMxN<3, 3, _Cf> _3x3;
typedef mx::MatrixMxN<4, 4, _Cf> _4x4;
typedef mx::VectorN<3, _Cf> _V3;
typedef mx::VectorN<2, _Cf> _V2;

int main()
{
    _3x3 A = mx::identityMatrix<3, _Cf>();
    std::cout << A << std::endl;

    //_Cf cf12(1.2);
    _V3 v3 = mx::constantVector<3, _Cf>(1.2);

    //_Cf cf2(2);
    v3[2] = 2; //cf2;
    
    std::cout << A * v3 << std::endl;


    std::cout << v3 + v3 << std::endl;

//     v3.normalize();
//     std::cout << v3 << std::endl;
//     std::cout << v3.norm() << std::endl;

    std::cout << v3 * 4.0 << std::endl;
    
    // {1, 0, 0}
    v3 = mx::constantVector<3, _Cf>(0);
    v3[0] = 1;

    // {0, 1, 0}
    _V3 y = mx::constantVector<3, _Cf>(0);
    y[1] = 1;

    std::cout << cross(v3, y) << std::endl;
}
