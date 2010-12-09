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

NITF_TRE_STATIC_HANDLER_REF(PIAPEA);

int main(int argc, char** argv)
{
    try
    {
        nitf::PluginRegistry::registerTREHandler(PIAPEA_init, PIAPEA_handler);
        nitf::TRE tre("PIAPEA");

        std::cout << "== PIAPEA TRE ==" << std::endl;
        for (nitf::TRE::Iterator it = tre.begin(); it != tre.end(); ++it)
        {
            std::cout << (*it).getKey() << ": [" <<
                    nitf::Field((*it).getData()).toString() << "]" << std::endl;
        }
        return 0;
    }
    catch (except::Throwable& t)
    {
        std::cout << t.getTrace() << std::endl;
    }
    return 1;
}
