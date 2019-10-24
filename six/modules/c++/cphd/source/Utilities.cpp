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

#include <cphd/Utilities.h>
#include <import/str.h>
#include <six/Init.h>

namespace cphd
{
void validateFormat(std::string format)
{
    if (format == "F4" || format == "F8")
    {
        return;
    }
    else if (format == "U1" || format == "U2" || format == "U4" ||
                format == "U8")
    {
        return;
    }
    else if (format == "I1" || format == "I2" || format == "I4" ||
                format == "I8")
    {
        return;
    }
    else if (format == "CI2" || format == "CI4" || format == "CI8" ||
                format == "CI16")
    {
        return;
    }
    else if (format == "CF8" || format == "CF16")
    {
        return;
    }
    else if (!six::Init::isUndefined<size_t>(isFormatStr(format))) // Else if Check for string type S[1-9][0-9]
    {
        return;
    }
    else if (isMultipleParam(format))
    {
        return;
    }
    throw except::Exception(Ctxt("Invalid format provided"));
}

size_t getFormatSize(std::string format)
{
    if (format == "U1" || format == "I1")
    {
        return 1;
    }
    else if (format == "U2" || format == "I2" ||
             format == "CI2")
    {
        return 2;
    }
    else if (format == "F4" || format == "U4" ||
             format == "I4" || format == "CI4")
    {
        return 4;
    }
    else if (format == "F8" || format == "U8" ||
             format == "I8" || format == "CI8" ||
             format == "CF8")
    {
        return 8;
    }
    else if (format == "CI16" || format == "CF16")
    {
        return 16;
    }
    else if (!six::Init::isUndefined<size_t>(isFormatStr(format))) // Else if Check for string type S[1-9][0-9]
    {
        return isFormatStr(format);
    }
    throw except::Exception(Ctxt("Invalid format provided"));
}

bool isMultipleParam(std::string format)
{
    std::string eqDelimiter = "=";
    std::string colDelimiter = ";";
    size_t idx = 0;
    while (format.find(colDelimiter, idx) != std::string::npos)
    {
        size_t pos = format.find(colDelimiter, idx);
        try
        {
            FormatType keyVal = keyValueFinder(format, idx, pos);
            validateFormat(keyVal.getVal());
        }
        catch(std::exception& e)
        {
            return false;
        }
        //! Update the current idx
        if (pos + 1 >= format.size())
        {
            return true;
        }
        idx = ++pos;
    }
    return false;
}

std::vector<FormatType> parseMultipleParams(std::string format)
{
    std::vector<FormatType> keyValPairs;
    std::string colDelimiter = ";";
    size_t idx = 0;
    while (format.find(colDelimiter, idx) != std::string::npos)
    {
        size_t pos = format.find(colDelimiter, idx);
        FormatType keyVal = keyValueFinder(format, idx, pos);
        validateFormat(keyVal.getVal());
        keyValPairs.push_back(keyVal);

        //! Update the current idx
        if (pos + 1 >= format.size())
        {
            return keyValPairs;
        }
        idx = ++pos;
    }
    throw except::Exception(Ctxt(
            "Incorrect format \n"));
}

std::vector<std::pair<std::string,size_t> > getMultipleParamSizes(std::string format)
{
    std::vector<FormatType> params = parseMultipleParams(format);
    std::vector<std::pair<std::string,size_t> > paramVec(params.size());
    for (size_t ii = 0; ii < params.size(); ++ii)
    {
        paramVec[ii] =
            std::pair<std::string, size_t>(params[ii].getKey(), getFormatSize(params[ii].getVal()));
    }
    return paramVec;
}

std::map<std::string, size_t> getMultipleParams(std::string format)
{
    std::map<std::string, size_t> paramMap;
    std::vector<FormatType> params = parseMultipleParams(format);
    for (size_t ii = 0; ii < params.size(); ++ii)
    {
        if (paramMap.count(params[ii].getKey()) == 0)
        {
            paramMap[params[ii].getKey()] = getFormatSize(params[ii].getVal());
        }
        else
        {
            throw except::Exception(Ctxt(
                "Param in format is not unique"));
        }
    }
    return paramMap;
}

FormatType keyValueFinder(std::string format, size_t startPos, size_t endPos)
{
    const std::string eqDelimiter = "=";
    size_t eqPos = format.find(eqDelimiter, startPos);
    if(eqPos == std::string::npos || eqPos > endPos)
    {
        throw except::Exception(Ctxt(
                "Incorrect format"));
    }
    //! Update the current idx
    std::string key = format.substr(startPos, eqPos-startPos);
    if (eqPos + 1 >= endPos)
    {
        throw except::Exception(Ctxt(
                "Incorrect format"));
    }
    eqPos += 1;
    std::string val = format.substr(eqPos, endPos-eqPos);
    return FormatType(key, val);
}

size_t isFormatStr(std::string format)
{
    const char* ptr = format.c_str();
    if(format.size() <= 3 && *ptr == 'S')
    {
        std::string numBytes;
        if(format.size() > 1 && std::isdigit(*(ptr+1)))
        {
            numBytes = *(ptr+1);
        }
        else
        {
            return six::Init::undefined<size_t>();
        }
        if(format.size() > 2)
        {
            if(std::isdigit(*(ptr+2)))
            {
                numBytes += *(ptr+2);
            }
            else
            {
                return six::Init::undefined<size_t>();
            }
        }
        return str::toType<size_t>(numBytes);
    }
    return six::Init::undefined<size_t>();
}

std::ostream& operator<< (std::ostream& os, const GeoInfo& g)
{
    os << "GeoInfo:: \n"
        << "  Name             : " << g.name << "\n";
    for (size_t ii = 0; ii < g.desc.size(); ++ii)
    {
        os << "  Description      : " << g.desc[ii].getName() << ": " << g.desc[ii].str() << "\n";
    }
    if (g.geometryLatLon.size() == 1)
    {
        os << "  Point            : " << g.geometryLatLon[0].getLat() << ", " << g.geometryLatLon[0].getLon() << "\n";
    }
    else if (g.geometryLatLon.size() == 2)
    {
        os << "  Line             : " << "\n";
        for (size_t ii = 0; ii < g.geometryLatLon.size(); ++ii)
        {
            os << "  Endpoint         : " << g.geometryLatLon[ii].getLat() << ", " << g.geometryLatLon[ii].getLon() << "\n";
        }
    }
    else if (g.geometryLatLon.size() > 2)
    {
        os << "  Polygon          : " << "\n";
        for (size_t ii = 0; ii < g.geometryLatLon.size(); ++ii)
        {
            os << "  Vertex           : " << g.geometryLatLon[ii].getLat() << ", " << g.geometryLatLon[ii].getLon() << "\n";
        }
    }

    for (size_t ii = 0; ii < g.geoInfos.size(); ++ii)
    {
        os << *g.geoInfos[ii] << "\n";
    }
    return os;
}

std::ostream& operator<< (std::ostream& os, const MatchInformation& m)
{
    os << "MatchInformation:: \n"
        << "  Types:: \n";
    for (size_t ii = 0; ii < m.types.size(); ++ii)
    {
        os << "  TypeID           : " << m.types[ii]->typeID << "\n";
        if (!six::Init::isUndefined(m.types[ii]->currentIndex))
        {
            os << "  CurrentIndex     : " << m.types[ii]->currentIndex << "\n";
        }
        os << " NumMatchCollections : " << m.types[ii]->matchCollects.size() << "\n";

        for (size_t jj = 0; jj < m.types[ii]->matchCollects.size(); ++jj)
        {
            os << "  MatchCollect     : \n"
                << "    CoreName       : " << m.types[ii]->matchCollects[jj].coreName << "\n";
            if (!six::Init::isUndefined(m.types[ii]->matchCollects[jj].matchIndex))
            {
                os << "    MatchIndex     : " << m.types[ii]->matchCollects[jj].matchIndex << "\n";
            }
            for (size_t kk = 0; kk < m.types[ii]->matchCollects[jj].parameters.size(); ++kk)
            {
                os << "    Parameter      : "
                    << m.types[ii]->matchCollects[jj].parameters[kk].getName()
                    << ": " << m.types[ii]->matchCollects[jj].parameters[kk].str() << "\n";
            }
        }
    }
    return os;
}

}
