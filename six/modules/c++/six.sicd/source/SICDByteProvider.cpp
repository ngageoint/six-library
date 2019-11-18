/* =========================================================================
 * This file is part of six.sicd-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2017, MDA Information Systems LLC
 *
 * six.sicd-c++ is free software; you can redistribute it and/or modify
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

#include <six/sicd/ComplexXMLControl.h>
#include <six/sicd/SICDByteProvider.h>

namespace six
{
namespace sicd
{
SICDByteProvider::SICDByteProvider(
        const ComplexData& data,
        const std::vector<std::string>& schemaPaths,
        size_t maxProductSize)
{
    XMLControlRegistry xmlRegistry;
    xmlRegistry.addCreator(six::DataType::COMPLEX,
                           new six::XMLControlCreatorT<ComplexXMLControl>());

    mem::SharedPtr<Container> container(new Container(
            DataType::COMPLEX));

    // The container wants to take ownership of the data
    // To avoid memory problems, we'll just clone it
    container->addData(data.clone());

    initialize(container, xmlRegistry, schemaPaths, maxProductSize);
}

SICDByteProvider::SICDByteProvider(const NITFWriteControl& writer,
                                   const std::vector<std::string>& schemaPaths)
{
    initialize(writer, schemaPaths);
}

SICDByteProvider::SICDByteProvider(const NITFWriteControl& writer,
                                   const std::vector<std::string>& schemaPaths,
                                   const std::vector<PtrAndLength>& desBuffers)
{
    initialize(writer, schemaPaths, desBuffers);
}
}
}
