/* =========================================================================
 * This file is part of six-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
 *
 * six-c++ is free software; you can redistribute it and/or modify
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

#include <io/FileInputStream.h>
#include <import/six.h>
#include <import/six/sicd.h>
#include <import/six/sidd.h>

namespace
{
#include <import/nitf.hpp>

//NITF_TRE_STATIC_HANDLER_REF(XML_DATA_CONTENT);
NITF_TRE_STATIC_HANDLER_REF(PIAPEA);
}

int main(int argc, char** argv)
{
    try
    {
        std::cout << "Exists: " << nitf::PluginRegistry::treHandlerExists("PIAPEA") << std::endl;

        nitf::PluginRegistry::registerTREHandler(PIAPEA_init, PIAPEA_handler);

        std::cout << "Exists: " << nitf::PluginRegistry::treHandlerExists("PIAPEA") << std::endl;

        nitf::PluginRegistry::registerTREHandler(PIAPEA_init, PIAPEA_handler);

        std::cout << "Exists: " << nitf::PluginRegistry::treHandlerExists("PIAPEA") << std::endl;

        nitf::TRE tre("PIAPEA");

        std::cout << "== PIAPEA TRE ==" << std::endl;
        for (nitf::TRE::Iterator it = tre.begin(); it != tre.end(); ++it)
        {
            std::cout << (*it).getKey() << ": [" <<
                    nitf::Field((*it).getData()).toString() << "]" << std::endl;
        }

        return 0;
    }
    catch (const std::exception& ex)
    {
        std::cerr << ex.what() << std::endl;
        return 1;
    }
    catch (const except::Exception& ex)
    {
        std::cerr << ex.toString() << std::endl;
        return 1;
    }
    catch (...)
    {
        std::cerr << "Unknown exception\n";
        return 1;
    }
}
