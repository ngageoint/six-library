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
#include <functional>
#include <algorithm>
#include <map>
#include <vector>

#include "test_functional.h"

int main(int argc, char **argv)
{
    try
    {
        nitf::Reader reader;
        if ( argc != 2 )
            throw nitf::NITFException(Ctxt(FmtX("Usage: %s <nitf-file>\n", argv[0])));

        nitf::IOHandle io(argv[1]);
        nitf::Record record = reader.read(io);
        nitf::FileHeader fileHeader = record.getHeader();

        FieldAdapterMap<nitf::FileHeader> map; //create the adapter map
        map.insert("fileHeader", std::mem_fun_ref(&nitf::FileHeader::getFileHeader));
        map.insert("fileDateTime", std::mem_fun_ref(&nitf::FileHeader::getFileDateTime));
        map.insert("fileVersion", std::mem_fun_ref(&nitf::FileHeader::getFileVersion));

        //we can loop over them in order
        for(FieldAdapterMap<nitf::FileHeader>::Iterator it = map.begin(); it != map.end(); ++it)
            std::cout << it->first << ": [" << it->second(fileHeader).toString() << "]" << std::endl;

        map.remove("fileHeader");
        //loop again
        for(FieldAdapterMap<nitf::FileHeader>::Iterator it = map.begin(); it != map.end(); ++it)
            std::cout << it->first << ": [" << it->second(fileHeader).toString() << "]" << std::endl;

        FieldAdapterMap<nitf::FileSecurity> securityMap;
        securityMap.insert("classificationSystem", std::mem_fun_ref(&nitf::FileSecurity::getClassificationSystem));
        securityMap.insert("codewords", std::mem_fun_ref(&nitf::FileSecurity::getCodewords));

        //we can query them by name
        nitf::FileSecurity security = fileHeader.getSecurityGroup();
        std::cout << "[" << securityMap["classificationSystem"](security).toString() << "]" << std::endl;
        std::cout << "[" << securityMap["codewords"](security).toString() << "]" << std::endl;

        io.close();
        return 0;
    }
    catch (except::Throwable& t)
    {
        std::cout << "ERROR: " << t.getTrace() << std::endl;
    }
}
