/* =========================================================================
 * This file is part of str-c++ 
 * =========================================================================
 * 
 * (C) Copyright 2004 - 2011, General Dynamics - Advanced Information Systems
 *
 * str-c++ is free software; you can redistribute it and/or modify
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

#include <iostream>
#include <import/str.h>

#if defined(TEST_FUNTOR)
#   define FORMAT_FUNC (std::string)str::Format
#else
#   define FORMAT_FUNC str::format
#endif

int main()
{
    std::cout << FORMAT_FUNC("Your version of str is %d.%d.%d\n",
            STR_MAJOR_VERSION, STR_MINOR_VERSION, STR_MICRO_VERSION);
    std::cout << "Specialization for string test..." << std::endl;
    std::string ok("This test passes");
    std::cout << str::toType<std::string>(ok) << std::endl;

    std::cout << "Testing the trim function..." << std::endl;
    std::string s = "  test   ";
    std::cout << "'" << s << "', length: " << s.length() << std::endl;
    str::trim(s);
    std::cout << "'" << s << "', length: " << s.length() << std::endl;
    str::upper(s);
    std::cout << "Upper: " << s << std::endl;
    str::lower(s);
    std::cout << "Lower: " << s << std::endl;
    return 0;
}
