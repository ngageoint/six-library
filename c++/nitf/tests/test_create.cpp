/* =========================================================================
 * This file is part of NITRO
 * =========================================================================
 *
 * (C) Copyright 2004 - 2010, General Dynamics - Advanced Information Systems
 *
 * NITRO is free software; you can redistribute it and/or modify
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
 * License along with this program; if not, If not,
 * see <http://www.gnu.org/licenses/>.
 *
 */

#include <import/nitf.hpp>

int main(int argc, char** argv)
{
    try
    {
        if (argc != 2)
        {
            throw nitf::NITFException(Ctxt(FmtX("Usage %s <file-to-create>\n",
                                                argv[0])));
        }

        nitf::IOHandle handle(argv[1], NITF_ACCESS_READONLY, NITF_CREATE);
        if (handle.isValid())
        {
            throw nitf::NITFException(Ctxt("Test failed!"));
        }
        printf("Create succeeded.  Check file permissions to make sure they're OK\n");
        handle.close();
        return 0;
    }
    catch (except::Throwable& t)
    {
        std::cout << t.getTrace() << std::endl;
    }
}
