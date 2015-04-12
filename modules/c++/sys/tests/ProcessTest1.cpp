/* =========================================================================
 * This file is part of sys-c++ 
 * =========================================================================
 * 
 * (C) Copyright 2004 - 2009, General Dynamics - Advanced Information Systems
 *
 * sys-c++ is free software; you can redistribute it and/or modify
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


#include <import/sys.h>
#include <import/except.h>
using namespace sys;
using namespace except;

#include <string>
#include <vector>
using namespace std;

class LazyChild : public Process
{
public:
    LazyChild() {}
    ~LazyChild() {}
    void run()
    {
        cout << "In Child" << endl;
        OS os;
        os.millisleep(1000);
    }
};

class ListChild : public Process
{
public:
    ListChild(char **argv) : mArgv(argv) {}
    ~ListChild() {}
    void run()
    {
        cout << "In Child with argv:" << mArgv[0] << endl;
        execv("/usr/bin/ls", mArgv);
    }
    char **mArgv;
};

int main(int argc, char**argv)
{
    ListChild process1(argv);
    process1.start();
    process1.waitFor();

    ListChild process2(argv);
    process2.start();
    process2.waitFor();

    ListChild process3(argv);
    process3.start();
    process3.waitFor();

    ListChild process4(argv);
    process4.start();
    process4.waitFor();

    LazyChild lc;
    lc.start();
    lc.waitFor();

    return 0;
}
