/* =========================================================================
 * This file is part of except-c++ 
 * =========================================================================
 * 
 * (C) Copyright 2004 - 2009, General Dynamics - Advanced Information Systems
 *
 * except-c++ is free software; you can redistribute it and/or modify
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


#include <stdlib.h>
#include "import/except.h"

#define Ctxt(MESSAGE) Context(__FILE__, __LINE__, "Func", "Time", MESSAGE)

using std::endl; using std::cout;
using namespace except;

DECLARE_EXCEPTION(DivideByZero)
typedef DivideByZeroException DivideByZero;

double Divide(double x, double y);


double callDivide(double x, double y)
{
    double z;
    try
    {
        z = Divide(x, y);
    }
    catch (Throwable& t)
    {
        throw DivideByZero(t, Ctxt("Hello"));
    }
    return z;
}

double Divide(double x, double y)
{
    if (y == 0)
    {
        throw DivideByZero(Ctxt("Hello again"));
    }
    return x / y;
}

int main(int argc, char* argv[])
{
    if (argc != 3)
    {
        cout << "Usage: " << argv[0] << " <x> <y> " << endl;
        exit(1);
    }

    double x = atof(argv[1]);
    double y = atof(argv[2]);

    try
    {
        cout << "Dividing " << x << " and " << y << endl;
        cout << "Answer is: " << callDivide(x, y) << endl;
    }
    catch (Throwable& t)
    {
        cout << "Why did you do that?" << endl;
        cout << t.toString() << endl;
    }
    return 0;
}
