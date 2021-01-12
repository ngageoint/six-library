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

#include <import/except.h>
#include <import/io.h>
using namespace except;
using namespace io;
using namespace std;

class Copy
{
public:
    static void run(const char* src, const char* dest)
    {
        FileInputStream in(src);
        FileOutputStream out(dest);
        int c;
        unsigned char buf[25] = "";

        while ((c = in.read((sys::byte*) buf, 25)) != FileInputStream::IS_EOF)
            out.write((sys::byte*) buf, c);
        in.close();
        out.close();
    }
};

int main(int argc, char **argv)
{
    try
    {
        if (argc != 3)
            throw Exception(Ctxt("argc != 3"));

        Copy::run(argv[1], argv[2]);
    }
    catch (Exception& e)
    {
        cout << e.toString() << endl;
    }

}
