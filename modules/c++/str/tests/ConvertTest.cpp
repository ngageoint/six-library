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

#include <import/str.h>
#include <import/except.h>


template <typename T> void testType(const std::string& s)
{
    try
    {
        std::cout << str::toType<T>(s);
    }
    catch (except::Exception& e)
    {
        std::cout << e.toString();
    }
    catch (std::exception& ex)
    {
        std::cout << ex.what();
    }
    catch (...)
    {
        std::cout << "Caught something";
    }
}

int main()
{
    std::string val("0");

    std::cout << "int: ";
    testType<int>(val);

    std::cout << std::endl << "long: ";
    testType<long>(val);

    std::cout << std::endl << "bool: ";
    testType<bool>(val);

    std::cout << std::endl << "unsigned char: ";
    testType<unsigned char>(val);

    std::cout << std::endl << "unsigned int: ";
    testType<unsigned int>(val);

    std::cout << std::endl << "unsigned long: ";
    testType<unsigned long>(val);

    std::cout << std::endl << "unsigned long long: ";
    testType<unsigned long long>(val);

    std::cout << std::endl << "unsigned short: ";
    testType<unsigned short>(val);

    std::cout << std::endl << "float: ";
    testType<float>(val);

    std::cout << std::endl << "double: ";
    testType<double>(val);

    std::cout << std::endl << "off_t: ";
    testType<off_t>(val);

    std::cout << std::endl;

    return 0;
}
