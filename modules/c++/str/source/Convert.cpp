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

#include "str/Convert.h"
#include "str/Manip.h"

template<> std::string str::toType<std::string>(const std::string& s)
{
    return s;
}
template<> bool str::toType<bool>(const std::string& s)
{
    std::string ss = s;
    str::lower(ss);

    if (ss == "true")
    {
        return true;
    }
    else if (ss == "false")
    {
        return false;
    }
    else if (str::isNumeric(ss))
    {
        int value(0);
        std::stringstream buf(ss);
        buf >> value;
        return (value != 0);
    }
    else
    {
        throw except::BadCastException(except::Context(__FILE__, __LINE__,
            std::string(""), std::string(""),
            std::string("Invalid bool: '") + s + std::string("'")));
    }

    return false;
}

template<> int str::getPrecision(const float& type)
{
    return std::numeric_limits<float>::digits10 + 1;
}
template<> int str::getPrecision(const double& type)
{
    return std::numeric_limits<double>::digits10 + 1;
}
template<> int str::getPrecision(const long double& type)
{
    return std::numeric_limits<long double>::digits10 + 1;
}
