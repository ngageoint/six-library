/* =========================================================================
 * This file is part of io-c++ 
 * =========================================================================
 * 
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
 *
 * io-c++ is free software; you can redistribute it and/or modify
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
#include <import/io.h>
#include <import/except.h>

using namespace sys;
using namespace io;
using namespace except;
using namespace std;

class Copy
{
public:
    static void run(const char* src, const char* dest)
    {
        io::FileInputStream in(src);
        io::FileOutputStream out(dest);

        in.streamTo(out);

        in.close();
        out.close();

    }
};

int main(int argc, char **argv)
{
    try
    {

        if (argc != 3)
            throw except::Error(
                                Ctxt(
                                     FmtX(
                                          "Usage: %s <input file> <output file>",
                                          argv[0])));

        Copy::run(argv[1], argv[2]);
    }
    catch (except::Throwable& e)
    {
        cout << e.toString() << endl;
    }

}
