/* =========================================================================
 * This file is part of six.sicd-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
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
#include <six/sicd/ComplexData.h>
#include <six/sicd/ComplexXMLParser040.h>
#include <six/sicd/ComplexXMLParser041.h>
#include <six/sicd/ComplexXMLParser050.h>
#include <six/sicd/ComplexXMLParser100.h>
#include <six/sicd/ComplexXMLParser101.h>

namespace six
{
namespace sicd
{
    const six::DataType ComplexXMLControl::dataType = six::DataType::COMPLEX;

ComplexXMLControl::ComplexXMLControl(logging::Logger* log, bool ownLog) :
    XMLControl(log, ownLog)
{
}

Data* ComplexXMLControl::fromXMLImpl(const xml::lite::Document* doc)
{
    return getParser(getVersionFromURI(doc))->fromXML(doc);
}

xml::lite::Document* ComplexXMLControl::toXMLImpl(const Data* data)
{
    if (data->getDataType() != DataType::COMPLEX)
    {
        throw except::Exception(Ctxt("Data must be SICD"));
    }

    const ComplexData* const sicd(dynamic_cast<const ComplexData*>(data));
    return getParser(data->getVersion())->toXML(sicd);
}

std::unique_ptr<ComplexXMLParser>
ComplexXMLControl::getParser(const std::string& strVersion) const
{
    std::unique_ptr<ComplexXMLParser> parser;

    std::vector<std::string> versionParts;
    splitVersion(strVersion, versionParts);

    if (versionParts.size() != 3)
    {
        throw except::Exception(
            Ctxt("Unsupported SICD Version: " + strVersion));
    }

    const std::string& majorVersion(versionParts.at(0));
    const std::string& minorVersion(versionParts.at(1));
    const std::string& patchVersion(versionParts.at(2));

    // six.sicd only currently supports --
    //   SICD 0.4.0
    //   SICD 0.4.1
    //   SICD 0.5.0
    //   SICD 1.0.0
    //   SICD 1.0.1
    //   SICD 1.1.0
    //   SICD 1.2.0
    //   SICD 1.2.1
    if (majorVersion == "0")
    {
        if (minorVersion == "4")
        {
            if (patchVersion == "0")
            {
                parser.reset(new ComplexXMLParser040(strVersion, mLog));
            }
            else if (patchVersion == "1")
            {
                parser.reset(new ComplexXMLParser041(strVersion, mLog));
            }
        }
        else if (minorVersion == "5" && patchVersion == "0")
        {
            parser.reset(new ComplexXMLParser050(strVersion, mLog));
        }
    }
    else if (majorVersion == "1")
    {
        if (minorVersion == "0")
        {
            if (patchVersion == "0")
            {
                parser.reset(new ComplexXMLParser100(strVersion, mLog));
            }
            else if (patchVersion == "1")
            {
                parser.reset(new ComplexXMLParser101(strVersion, mLog));
            }
        }
        else if (minorVersion == "1" && patchVersion == "0")
        {
            // The only difference between SICD 1.0.1 and 1.1.0 is that in
            // 1.1 there was a schema change that allows MatchCollection to
            // be optional.  We rely on the schema to do this check - the
            // parser itself doesn't - so we can simply reuse the 1.0.1 parser.
            parser.reset(new ComplexXMLParser101(strVersion, mLog));
        }
        else if (minorVersion == "2" && patchVersion == "0")
        {
            // From a SIX standpoint, 1.2.0 is identical to 1.1.0. The only
            // difference between 1.2 and 1.1 is some rules with how FTITLE
            // can be populated, so we can again reuse the 1.0.1 parser.
            parser.reset(new ComplexXMLParser101(strVersion, mLog));
        }
        else if (minorVersion == "2" && patchVersion == "1")
        {
            // The only change is in the DualPolarization enum, which is handled in the schema
            parser.reset(new ComplexXMLParser101(strVersion, mLog));
        }
    }

    // Validation check that we found a suitable Parser
    if (parser.get() == nullptr)
    {
        throw except::Exception(
            Ctxt("Unsupported SICD Version: " + version));
    }

    return parser;
}
}
}
