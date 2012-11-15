/* =========================================================================
 * This file is part of six.sidd-c++ 
 * =========================================================================
 * 
 * (C) Copyright 2004 - 2009, General Dynamics - Advanced Information Systems
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
#include <six/sidd/DerivedData.h>
#include <six/sidd/DerivedXMLParser.h>

namespace six
{
namespace sidd
{
DerivedXMLControl::DerivedXMLControl(logging::Logger* log, bool ownLog) :
    XMLControl(log, ownLog)
{
}

Data* DerivedXMLControl::fromXMLImpl(const xml::lite::Document* doc)
{
    DerivedXMLParser parser(getVersionFromURI(doc), mLog, false);
    return parser.fromXML(doc);
}

xml::lite::Document* DerivedXMLControl::toXMLImpl(const Data* data)
{
    if (data->getDataType() != DataType::DERIVED)
    {
        throw except::Exception(Ctxt("Data must be SIDD"));
    }

    DerivedXMLParser parser(data->getVersion(), mLog, false);
    return parser.toXML(reinterpret_cast<const DerivedData*>(data));
}
}
}
