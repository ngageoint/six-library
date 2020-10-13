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

#include <cstdlib>
#include <cctype>

#include <import/str.h>
#include <cphd/Utilities.h>
#include <six/Init.h>

namespace cphd
{

// Return bytes/sample, either 2, 4, or 8 (or 0 if not initialized)
size_t getNumBytesPerSample(const SignalArrayFormat& signalArrayFormat)
{
    switch (signalArrayFormat)
    {
        case SignalArrayFormat::CI2:
            return 2;
        case SignalArrayFormat::CI4:
            return 4;
        case SignalArrayFormat::CF8:
            return 8;
        default:
            return 0;
    }
}

void validateFormat(const std::string& format)
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
    // Else if Check for string type S[1-9][0-9]
    else if (!six::Init::isUndefined<size_t>(isFormatStr(format)))
    {
        return;
    }
    else if (isMultipleParam(format))
    {
        return;
    }
    throw except::Exception(Ctxt("Invalid format provided"));
}

size_t getFormatSize(const std::string& format)
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
     // Else if Check for string type S[1-9][0-9]
    else if (!six::Init::isUndefined<size_t>(isFormatStr(format)))
    {
        return isFormatStr(format);
    }
    throw except::Exception(Ctxt("Invalid format provided"));
}

bool isMultipleParam(const std::string& format)
{
    std::string eqDelimiter = "=";
    std::string colDelimiter = ";";
    size_t idx = 0;
    while (format.find(colDelimiter, idx) != std::string::npos)
    {
        size_t pos = format.find(colDelimiter, idx);
        try
        {
            std::pair<std::string,std::string> keyVal = keyValueFinder(format, idx, pos);
            validateFormat(keyVal.second);
        }
        catch(const std::exception&)
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

std::vector<std::pair<std::string,std::string> > parseMultipleParams(const std::string& format)
{
    std::vector<std::pair<std::string,std::string> > keyValPairs;
    std::string colDelimiter = ";";
    size_t idx = 0;
    while (format.find(colDelimiter, idx) != std::string::npos)
    {
        size_t pos = format.find(colDelimiter, idx);
        std::pair<std::string,std::string>  keyVal = keyValueFinder(format, idx, pos);
        validateFormat(keyVal.second);
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

std::vector<std::pair<std::string,size_t> > getMultipleParamSizes(const std::string& format)
{
    std::vector<std::pair<std::string,std::string> > params = parseMultipleParams(format);
    std::vector<std::pair<std::string,size_t> > paramVec(params.size());
    for (size_t ii = 0; ii < params.size(); ++ii)
    {
        paramVec[ii] =
            std::pair<std::string, size_t>(params[ii].first, getFormatSize(params[ii].second));
    }
    return paramVec;
}

std::pair<std::string,std::string>  keyValueFinder(const std::string& format, size_t startPos, size_t endPos)
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
    return std::pair<std::string,std::string> (key, val);
}

size_t isFormatStr(const std::string& format)
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
}
