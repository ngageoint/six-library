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

#include <import/except.h>
#include <import/sys.h>
#include <sstream>
#include <vector>
using namespace except;
using namespace sys;
using namespace std;

class NotAddableError : public Error
{
public:
    NotAddableError(int size)
    {
        std::ostringstream m;
        m << "Not addable because size is < 2: " << size;
        mMessage = m.str();
    }
};

class GreaterThan10Ex : public Exception
{
public:
    GreaterThan10Ex(int size)
    {
        std::ostringstream m;
        m << "Not addable because value is > 10: " << size;
        mMessage = m.str();
    }
};
class NegativeNumberEx : public Exception
{
public:

    NegativeNumberEx(int size)
    {
        std::ostringstream m;
        m << "Not addable because value is < 0: " << size;
        mMessage = m.str();
    }
};

int main(int argc, char **argv)
{
    try
    {
        vector<int>v;
        for (int i = 1; i < argc; i++)
        {
            v.push_back(atoi(argv[i]));
        }
        if (v.size() < 2)
        {

            NotAddableError nae(v.size());
            throw(nae);
        }
        int acc = 0;
        for (int i = 0; i < v.size(); i++)
        {
            acc += v[i];
        }

        if (acc < 0)
        {

            NegativeNumberEx nne(acc);
            throw(nne);

        }
        else if (acc > 10)
        {
            GreaterThan10Ex gt10(acc);
            throw(gt10);
        }
        else
        {
            //__status__("Succeeded");
        }
    }
    catch (Exception& ex)
    {
        cout << "Ex: " << ex.toString() << endl;
    }
    catch (Error& err)
    {
        cout << "Err: " << err.toString() << endl;
    }
    catch (Throwable& t)
    {
        cout << "Throwable: " << t.toString() << endl;
    }

    catch (...)
    {
        cout << "No clue what kind of exception" << endl;
    }

    return 0;

}
