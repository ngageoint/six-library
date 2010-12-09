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


#define MAX_DATE_STRING    1024

int main(int argc, char** argv)
{
    try
    {
        char dateBuf[MAX_DATE_STRING];
        nitf::DateTime now;

        now.setYear(2009);

        std::cout << "Year: " << now.getYear() << std::endl;
        std::cout << "Month: " << now.getMonth() << std::endl;
        std::cout << "Day Of Month: " << now.getDayOfMonth() << std::endl;
        std::cout << "Day Of Week: " << now.getDayOfWeek() << std::endl;
        std::cout << "Day Of Year: " << now.getDayOfYear() << std::endl;
        std::cout << "Hour: " << now.getHour() << std::endl;
        std::cout << "Minute: " << now.getMinute() << std::endl;
        std::cout << "Second: " << now.getSecond() << std::endl;
        std::cout << "Millis: " << now.getTimeInMillis() << std::endl;

        now.format(NITF_DATE_FORMAT_21, dateBuf, NITF_FDT_SZ + 1);
        std::cout << "The Current NITF 2.1 Formatted Date: " << dateBuf << std::endl;

#ifndef WIN32
        nitf::DateTime dolly(dateBuf, NITF_DATE_FORMAT_21);
        std::cout << "Roundtripped: " << dolly.getTimeInMillis() << std::endl;
#endif

        now.format(NITF_DATE_FORMAT_20, dateBuf, NITF_FDT_SZ + 1);
        std::cout << "The Current NITF 2.0 Formatted Date: " << dateBuf << std::endl;

#ifndef WIN32
        dolly = nitf::DateTime(dateBuf, NITF_DATE_FORMAT_20);
        std::cout << "Roundtripped: " << dolly.getTimeInMillis() << std::endl;
#endif

        exit(EXIT_SUCCESS);
    }
    catch (except::Throwable & t)
    {
        std::cout << t.getTrace() << std::endl;
    }
}
