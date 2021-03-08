/* =========================================================================
 * This file is part of math.poly-c++ 
 * =========================================================================
 * 
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
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

typedef Fixed1D<1> Linear1D;
typedef Fixed1D<2> Quadratic1D;
typedef Fixed1D<3> Cubic1D;
typedef Fixed1D<4> Quartic1D;

typedef Fixed2D<1, 1> Linear2D;
typedef Fixed2D<2, 2> Quadratic2D;
typedef Fixed2D<3, 3> Cubic2D;
typedef Fixed2D<4, 4> Quartic2D;


int main(int, char**)
{
    // first, lets do a one-d polynomial test...
    Cubic1D lOneDTst;
    lOneDTst[0] = -3;
    lOneDTst[1] = 1;
    lOneDTst[2] = 2;
    std::cout << "Test 1-D Polynomial implementation..." << std::endl;
    std::cout << "lOneDTstPoly: " << lOneDTst << std::endl;
    std::cout << "lOneDTst(" << (-1) << ") = " << lOneDTst(-1) << std::endl;
    std::cout << "lOneDTst(" << (-.5) << ") = " << lOneDTst(-.5) << std::endl;
    std::cout << "lOneDTst(" << (0) << ") = " << lOneDTst(0) << std::endl;
    std::cout << "lOneDTst(" << (.5) << ") = " << lOneDTst(.5) << std::endl;
    std::cout << "lOneDTst(" << (1) << ") = " << lOneDTst(1) << std::endl;
    std::cout << std::endl;
    std::cout << "Test 1-D Polynomial Derivative implemenation..." << std::endl;
    std::cout << "lOneDTstPoly: " << lOneDTst.derivative() << std::endl;
    std::cout << std::endl;
    std::cout << "Test 1-D Polynomial Integral implementation..." << std::endl;
    Quadratic1D lDeriv(lOneDTst.derivative());
    std::cout << "Integrate: " << lDeriv << " over the interval from -1 to 1" << std::endl;
    std::cout << "Integral(-1,1) = " << lDeriv.integrate(-1,1) << std::endl;

    std::cout << "Expected Res   = " << lOneDTst(1)-lOneDTst(-1) << std::endl;
    
    std::cout << "Test 1-D Polynomial * implementation..." << std::endl;
    Linear1D lOneDTst2;
    lOneDTst2[0] = 1;
    lOneDTst2[1] = 1;
    std::cout << "lOneDTstPoly: " << lOneDTst2 << std::endl;
    std::cout << "lOneDTstPoly^2: " << lOneDTst2*lOneDTst2 << std::endl;
    std::cout << std::endl;
    
    std::cout << "Test 1-D Polynomial + implementation..." << std::endl;
    std::cout << "lOneDTstPoly1: " << lOneDTst << std::endl;
    std::cout << "lOneDTstPoly2: " << lOneDTst2 << std::endl;
    Quadratic1D plus = lOneDTst2 + lOneDTst;

    std::cout << "lOneDTstPoly2 + lOneDTstPoly1 (quad): " << plus << std::endl;
    std::cout << "lOneDTstPoly2 + lOneDTstPoly1: " <<  lOneDTst2 + lOneDTst << std::endl;
    std::cout << std::endl;
    
    std::cout << "Test 1-D Polynomial - implementation..." << std::endl;

    std::cout << "lOneDTstPoly1: " << lOneDTst << std::endl;
    std::cout << "N: " << lOneDTst * -1 << std::endl;
    std::cout << "lOneDTstPoly2: " << lOneDTst2 << std::endl;
    plus = lOneDTst2-lOneDTst;
    std::cout << "lOneDTstPoly2 - lOneDTstPoly1 (quad): " << plus << std::endl;
    std::cout << "lOneDTstPoly2 - lOneDTstPoly1: " << lOneDTst2-lOneDTst << std::endl;
    std::cout << std::endl;
    

    // now, lets do a two-d polynomial test...
    Fixed2D<3, 1, double> lTwoDTst;
    lTwoDTst[0][0] = -3;
    lTwoDTst[0][1] = 1;
    lTwoDTst[1][0] = -1;
    lTwoDTst[1][1] = 0;
    lTwoDTst[2][0] = -2;
    lTwoDTst[2][1] = -1;
    std::cout << "Test 2-D Polynomial implementation..." << std::endl;
    std::cout << "lTwoDTstPoly: " << std::endl << lTwoDTst << std::endl;
    std::cout << "lTwoDTst(" << 0 << "," << 0 << ") = " << lTwoDTst(0,0) << std::endl;
    std::cout << "lTwoDTst(" << 0 << "," << 2 << ") = " << lTwoDTst(0,2) << std::endl;
    std::cout << "lTwoDTst(" << 2 << "," << 0 << ") = " << lTwoDTst(2,0) << std::endl;
    std::cout << "lTwoDTst(" << 2 << "," << -2 << ") = " << lTwoDTst(2,-2) << std::endl;
    std::cout << std::endl;


    std::cout << "Test 2-D Polynomial atX implementation..." << std::endl;
    std::cout << "lTwoDTstPoly: " << std::endl << lTwoDTst << std::endl;

    math::poly::Fixed1D<3> lAt = lTwoDTst.atY(2);

    std::cout << "lAt(" << 2 << ") = " << lAt << std::endl;
    std::cout << std::endl;



    std::cout << "Test 2-D Polynomial DerivativeY implementation..." << std::endl;
    std::cout << "lTwoDTstPoly: " << lTwoDTst.derivativeY() << std::endl;
    std::cout << std::endl;
    /*try
    {
    std::cout << "Test 2-D Polynomial DerivativeY.DerivativeY implementation..." << std::endl;
    std::cout << "lTwoDTstPoly: " << lTwoDTst.derivativeY().derivativeY() << std::endl;
    std::cout << std::endl;
    }
    catch (except::Exception& ex)
    {
        std::cout << ex.getMessage() << std::endl;
    }*/

    std::cout << "Test 2-D Polynomial DerivativeX implementation..." << std::endl;
    std::cout << "lTwoDTstPoly: " << lTwoDTst.derivativeX() << std::endl;
    std::cout << std::endl;
    std::cout << "Test 2-D Polynomial DerivativeX.DerivativeX implementation..." << std::endl;
    std::cout << "lTwoDTstPoly: " << lTwoDTst.derivativeX().derivativeX() << std::endl;
    std::cout << std::endl;
    std::cout << "Test 2-D Polynomial DerivativeXY implementation..." << std::endl;
    std::cout << "lTwoDTstPoly: " << lTwoDTst.derivativeXY() << std::endl;
    std::cout << std::endl;
    std::cout << "Test 2-D Polynomial DerivativeXY.DerivativeXY implemenation..." << std::endl;
    /*try
    {
    std::cout << "lTwoDTstPoly: " << lTwoDTst.derivativeXY().derivativeXY() << std::endl;
    std::cout << std::endl;
    }
    catch (except::Exception& ex)
    {
        std::cout << ex.getMessage() << std::endl;
    }*/
    std::cout << "Test 2-D Polynomial Integral implementation..." << std::endl;
    math::poly::Fixed2D<2, 0, double> lDeriv2(lTwoDTst.derivativeXY());
    std::cout << "Integrate: " << lDeriv2 << " over the interval... " << std::endl;
    std::cout << "Integral(-2,3,-1,2) = " << lDeriv2.integrate(-2,3,-1,2) << std::endl;
    std::cout << "Expected Res   = " << (lTwoDTst(3,2)-lTwoDTst(3,-1))-(lTwoDTst(-2,2)-lTwoDTst(-2,-1)) << std::endl;
    
    std::cout << "Test 2-D Polynomial * implementation..." << std::endl;

    Fixed2D<2, 1> lTwoDTst2;
    lTwoDTst2[0][0] = 1;
    lTwoDTst2[0][1] = 1;
    lTwoDTst2[1][0] = -1;
    lTwoDTst2[1][1] = 2;
    std::cout << "lTwoDTstPoly: " << lTwoDTst << std::endl;
    std::cout << "lTwoDTstPoly2: " << lTwoDTst2 << std::endl;

    std::cout << "lTwoDTstPoly*lTwoDTstPoly2: " << lTwoDTst*lTwoDTst2 << std::endl;
    std::cout << std::endl;
    
    std::cout << "Test 2-D Polynomial + implementation..." << std::endl;
    std::cout << "lTwoDTstPoly1: " << lTwoDTst << std::endl;
    std::cout << "lTwoDTstPoly2: " << lTwoDTst2 << std::endl;
    Fixed2D<3, 1> mix =  lTwoDTst2+lTwoDTst;

    std::cout << "lTwoDTstPoly2 + lTwoDTstPoly1: (cubic) " << mix << std::endl;
    std::cout << "lTwoDTstPoly2 + lTwoDTstPoly1: " << lTwoDTst2+lTwoDTst << std::endl;
    std::cout << std::endl;


    std::cout << "Test 2-D Polynomial - implementation..." << std::endl;
    std::cout << "lTwoDTstPoly1: " << lTwoDTst << std::endl;
    std::cout << "lTwoDTstPoly2: " << lTwoDTst2 << std::endl;
     mix = lTwoDTst2-lTwoDTst;

     std::cout << "lOneDTstPoly2 - lOneDTstPoly1: (cubic) " << mix << std::endl;

    std::cout << "lOneDTstPoly2 - lOneDTstPoly1: " << lTwoDTst2-lTwoDTst << std::endl;
    std::cout << std::endl;
}
