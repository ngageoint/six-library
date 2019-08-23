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
ProductInfo::ProductInfo():
    profile(six::Init::undefined<std::string>())
{
}

ProductInfo::CreationInfo::CreationInfo():
    application(six::Init::undefined<std::string>()),
    dateTime(six::Init::undefined<DateTime>()),
    site(six::Init::undefined<std::string>())
{
}

std::ostream& operator<< (std::ostream& os, const ProductInfo& p)
{
    os << "Product Information      : \n"
        << "  Profile                : " << p.profile << "\n";
    for (size_t i = 0; i < p.creationInfo.size(); ++i)
    {
        os << "  Creation Information   : " << "\n"
            << "    Application          : " << p.creationInfo[i].application << "\n"
            << "    DateTime             : " << p.creationInfo[i].dateTime.getMonth() << "/"
                << p.creationInfo[i].dateTime.getDayOfMonth() << "/"
                << p.creationInfo[i].dateTime.getYear() << "\n"
            << "    Site                 : " << p.creationInfo[i].site << "\n";
        for (size_t j = 0; j < p.creationInfo[i].parameter.size(); ++j)
        {
            os << "    Parameter name       : " 
                    << p.creationInfo[i].parameter[j].getName() << "\n"
                << "    Parameter value      : " << p.creationInfo[i].parameter[j].str() << "\n";
        }
    }
    for (size_t i = 0; i < p.parameter.size(); ++i)
    {
        os << "  Parameter name       : " << p.parameter[i].getName() << "\n"
            << "  Parameter value      : " << p.parameter[i].str() << "\n";
    }
    return os;
}

}
