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

    // PA = LU
    const unsigned int seed =
        static_cast<unsigned int>(sys::LocalDateTime().getTimeInMillis());
    ::srand(seed);
    A(0, 0) = _Cf((float)rand()/RAND_MAX, (float)rand()/RAND_MAX);
    A(0, 1) = _Cf((float)rand()/RAND_MAX, (float)rand()/RAND_MAX);
    A(0, 2) = _Cf((float)rand()/RAND_MAX, (float)rand()/RAND_MAX);
    A(1, 0) = _Cf((float)rand()/RAND_MAX, (float)rand()/RAND_MAX);
    A(1, 1) = _Cf((float)rand()/RAND_MAX, (float)rand()/RAND_MAX);
    A(1, 2) = _Cf((float)rand()/RAND_MAX, (float)rand()/RAND_MAX);
    A(2, 0) = _Cf((float)rand()/RAND_MAX, (float)rand()/RAND_MAX);
    A(2, 1) = _Cf((float)rand()/RAND_MAX, (float)rand()/RAND_MAX);
    A(2, 2) = _Cf((float)rand()/RAND_MAX, (float)rand()/RAND_MAX);
    A.scale(10);

    std::cout << "A: " << A << std::endl;

    std::vector<size_t> p;
    p.resize(3);
    _3x3 lu = A.decomposeLU(p);
    _3x3 PA = A.permute(p);

    _3x3 L = mx::identityMatrix<3, _Cf>();
    _3x3 U = mx::identityMatrix<3, _Cf>();

    U(0, 0) = lu(0, 0); U(0, 1) = lu(0, 1); U(0, 2) = lu(0, 2);
    L(1, 0) = lu(1, 0); U(1, 1) = lu(1, 1); U(1, 2) = lu(1, 2);
    L(2, 0) = lu(2, 0); L(2, 1) = lu(2, 1); U(2, 2) = lu(2, 2);

    std::cout << "L: " << L << std::endl;
    std::cout << "U: " << U << std::endl;
    std::cout << "LU: " << L * U << std::endl;
    std::cout << "PA: " << PA << std::endl;
}
