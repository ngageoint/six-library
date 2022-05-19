/* =========================================================================
 * This file is part of cphd-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2019, MDA Information Systems LLC
 *
 * cphd-c++ is free software; you can redistribute it and/or modify
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

#include <cphd/ProductInfo.h>
#include <six/Init.h>

namespace cphd
{
ProductInfo::ProductInfo()
{
}

ProductInfo::CreationInfo::CreationInfo():
    dateTime(six::Init::undefined<DateTime>())
{
}

std::ostream& operator<< (std::ostream& os, const ProductInfo& p)
{
    os << "Product Information      : \n"
        << "  Profile                : " << p.profile << "\n";
    for (size_t ii = 0; ii < p.creationInfo.size(); ++ii)
    {
        os << "  Creation Information   : " << "\n"
            << "    Application          : " << p.creationInfo[ii].application << "\n"
            << "    DateTime             : " << p.creationInfo[ii].dateTime.getMonth() << "/"
                << p.creationInfo[ii].dateTime.getDayOfMonth() << "/"
                << p.creationInfo[ii].dateTime.getYear() << "\n"
            << "    Site                 : " << p.creationInfo[ii].site << "\n";
        for (size_t jj = 0; jj < p.creationInfo[ii].parameter.size(); ++jj)
        {
            os << "    Parameter name       : "
                    << p.creationInfo[ii].parameter[jj].getName() << "\n"
                << "    Parameter value      : " << p.creationInfo[ii].parameter[jj].str() << "\n";
        }
    }
    for (size_t ii = 0; ii < p.parameter.size(); ++ii)
    {
        os << "  Parameter name   : " << p.parameter[ii].getName() << "\n"
            << "  Parameter value  : " << p.parameter[ii].str() << "\n";
    }
    return os;
}
}
