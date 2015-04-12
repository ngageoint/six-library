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

typedef mx::MatrixMxN<2, 2> _2x2;
typedef mx::MatrixMxN<3, 2> _3x2;
typedef mx::MatrixMxN<3, 3> _3x3;
typedef mx::MatrixMxN<4, 4> _4x4;


int main()
{
    _2x2 B = mx::identityMatrix<2, double>();
    B(0, 0) = 1.1;
    std::cout << "B: " << B << std::endl;

    _3x2 A(0.0);
    
    A(0, 0) = 1; A(1, 0) = 3; A(2, 0) = 5;
    A(0, 1) = 2; A(1, 1) = 4; A(2, 1) = 6;

    std::cout << "A: " << A << std::endl;

    _3x2 C = A.multiplyDiagonal(B);

    /*

      >> B = [1.1 0; 0 1.0];
      >> A = [1 2; 3 4; 5 6];
      >> C = A * B

      C =

      1.1000    2.0000
      3.3000    4.0000
      5.5000    6.0000

     */
    std::cout << "C: " << C << std::endl;
    
 
}
