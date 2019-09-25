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

#ifndef __CPHD_SUPPORT_ARRAY_H__
#define __CPHD_SUPPORT_ARRAY_H__

#include <ostream>
#include <vector>
#include <stddef.h>

#include <cphd/Enums.h>
#include <cphd/Types.h>


namespace cphd
{

struct SupportArrayParameter
{
    SupportArrayParameter();

    bool operator==(const SupportArrayParameter& other) const
    {
        return identifier == other.identifier &&
                elementFormat == other.elementFormat &&
                x0 == other.x0 && y0 == other.y0 &&
                xSS == other.xSS && ySS == other.ySS;
    }

    bool operator!=(const SupportArrayParameter& other) const
    {
        return !((*this) == other);
    }

    void setIdentifier(size_t identifierIn)
    {
        identifier = identifierIn;
    }

    inline size_t getIdentifier() const
    {
        return identifier;
    }

    std::string elementFormat;
    double x0;
    double y0;
    double xSS;
    double ySS;
    // hexType noData;

private:
    size_t identifier;
};

struct AdditionalSupportArray : SupportArrayParameter
{

    AdditionalSupportArray();

    bool operator==(const AdditionalSupportArray& other) const
    {
        return identifier == other.identifier &&
                elementFormat == other.elementFormat &&
                x0 == other.x0 && y0 == other.y0 &&
                xSS == other.xSS && ySS == other.ySS &&
                xUnits == other.xUnits && yUnits == other.yUnits &&
                zUnits == other.zUnits && parameter == other.parameter;
    }

    bool operator!=(const AdditionalSupportArray& other) const
    {
        return !((*this) == other);
    }

    std::string identifier;
    std::string xUnits;
    std::string yUnits;
    std::string zUnits;
    six::ParameterCollection parameter;

};

/*
 * (Optional) Parameters that describe the binary support
 * array(s) content and grid coordinates.
 * See section 2.3
 */
struct SupportArray
{
    bool operator==(const SupportArray& other) const
    {
        return iazArray == other.iazArray &&
                antGainPhase == other.antGainPhase &&
                addedSupportArray == other.addedSupportArray;
    }

    bool operator!=(const SupportArray& other) const
    {
        return !((*this) == other);
    }

    std::vector<SupportArrayParameter> iazArray;
    std::vector<SupportArrayParameter> antGainPhase;
    std::vector<AdditionalSupportArray> addedSupportArray;

};

std::ostream& operator<< (std::ostream& os, const SupportArrayParameter& s);
std::ostream& operator<< (std::ostream& os, const AdditionalSupportArray& a);
std::ostream& operator<< (std::ostream& os, const SupportArray& s);
}

#endif
