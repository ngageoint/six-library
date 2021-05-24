/* =========================================================================
 * This file is part of six.sidd-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2018, MDA Information Systems LLC
 *
 * six.sidd-c++ is free software; you can redistribute it and/or modify
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

#include <six/sidd/DerivedXMLControl.h>
#include <six/sidd/SIDDByteProvider.h>

namespace six
{
namespace sidd
{
SIDDByteProvider::SIDDByteProvider(
        const DerivedData& data,
        const std::vector<std::string>& schemaPaths,
        size_t numRowsPerBlock,
        size_t numColsPerBlock,
        size_t maxProductSize)
{
    XMLControlRegistry xmlRegistry;
    xmlRegistry.addCreator<DerivedXMLControl>();

    mem::SharedPtr<Container> container(new Container(
        DataType::DERIVED));

    // The container wants to take ownership of the data
    // To avoid memory problems, we'll just clone it
    container->addData(data.clone());

    initialize(container, xmlRegistry, schemaPaths,
               maxProductSize, numRowsPerBlock, numColsPerBlock);
}

SIDDByteProvider::SIDDByteProvider(const NITFWriteControl& writer,
                                   const std::vector<std::string>& schemaPaths)
{
    initialize(writer, schemaPaths);
}
}
}
