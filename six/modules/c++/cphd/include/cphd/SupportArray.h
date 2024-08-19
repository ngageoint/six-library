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

#pragma once
#ifndef SIX_cphd_SupportArray_h_INCLUDED_
#define SIX_cphd_SupportArray_h_INCLUDED_

#include <stddef.h>

#include <ostream>
#include <vector>
#include <unordered_map>

#include <six/XsElement.h>

#include <cphd/Enums.h>
#include <cphd/Types.h>

namespace cphd
{

/*!
 *  \struct SupportArrayParameter
 *
 *  \brief Parameters for defined support arrays
 *
 *  (Optional) Parameters that describe the binary support
 *  array(s) content and grid coordinates.
 */
struct SupportArrayParameter
{
    //! Constructor
    SupportArrayParameter();

    /*
     *  \func SupportArrayParameter
     *
     *  \brief Constructor initalizes values
     *
     *  \param format Speicifies the binary format of the data
     *   in the support array
     *  \param id Unique identifier of the support array
     *  \param x0_in Row 0 x coordinate
     *  \param y0_in Row 0 y coordinate
     *  \param xSS_in Row coordinate (X) sample spacing
     *  \param ySS_in Col coordinate (Y) sample spacing
     */
    SupportArrayParameter(
        const std::string& format, const std::string& id,
        double x0_in, double y0_in,
        double xSS_in, double ySS_in);

    //! Equality operator
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

    //! Set unique identifier
    void setIdentifier(const std::string& identifierIn)
    {
        identifier = identifierIn;
    }

    //! Get unique identifier
    inline std::string getIdentifier() const
    {
        return identifier;
    }

    //! Data element format
    std::string elementFormat;

    //! Row 0 X coordinate
    double x0;

    //! Column 0 Y coordinate.
    double y0;

    //! Row coordinate (X) sample spacing
    double xSS;

    //! Column coordinate (Y) sample spacing.
    double ySS;

protected:
    void initializeParams();
private:
    std::string identifier;
};

/*!
 *  \struct DwellTimeArray
 *
 *  DTA(m,n) Array of COD times(sec) and Dwell Times(sec) for points
 *  on reference surface. Array coordinates are image area coordinates(IAX, IAY).
 */
using DwellTimeArray = SupportArrayParameter;

/*!
 *  \struct AdditionalSupportArray
 *
 *  \brief Addiitonal support array custom parameters
 *
 *  (Optional) Added Array: Z(m,n) is a two dimensional array
 *  of data elements. Content and format of each
 *  element is user defined. Array coordinates X(m)
 *  and Y(m) are also user defined.
 */
struct AdditionalSupportArray : SupportArrayParameter
{
    //! Constructor
    AdditionalSupportArray();

    /*
     *  \func AdditionalSupportArray
     *
     *  \brief Constructor initalizes values
     *
     *  \param format Speicifies the binary format of the data
     *   in the support array
     *  \param id Unique identifier of the support array
     *  \param x0_in Row 0 x coordinate
     *  \param y0_in Row 0 y coordinate
     *  \param xSS_in Row coordinate (X) sample spacing
     *  \param ySS_in Col coordinate (Y) sample spacing
     *  \param xUnits Defines the X units of the sampled grid
     *  \param yUnits Defines the Y units of the sampled grid
     *  \param zUnits Defines the Z units of the sampled grid
     */
    AdditionalSupportArray(
        const std::string& format, const std::string& id,
        double x0_in, double y0_in, double xSS_in, double ySS_in,
        const std::string& xUnits_in, const std::string& yUnits_in,
        const std::string& zUnits_in);

    //! Equality operator
    bool operator==(const AdditionalSupportArray& other) const
    {
        return elementFormat == other.elementFormat &&
                x0 == other.x0 && y0 == other.y0 &&
                xSS == other.xSS && ySS == other.ySS &&
                identifier == other.identifier &&
                xUnits == other.xUnits && yUnits == other.yUnits &&
                zUnits == other.zUnits && parameter == other.parameter;
    }
    bool operator!=(const AdditionalSupportArray& other) const
    {
        return !((*this) == other);
    }

    //! Unique identifier of support array
    std::string identifier;

    //! Defines the X units of the sampled grid
    std::string xUnits;

    //! Defines the Y units of the sampled grid
    std::string yUnits;

    //! Defines the units of each component of the Data
    //! Element
    std::string zUnits;

    //! (Optional) Text field that can be used to further
    //! describe the added Support Array
    six::ParameterCollection parameter;
};

/*
 *  \struct SupportArray
 *
 *  \brief Contains parameters of all support arrays
 *
 *  (Optional) Parameters that describe the binary support
 *  array(s) content and grid coordinates.
 *  See section 2.3
 */
struct SupportArray final
{
    //! Equality operators
    bool operator==(const SupportArray& other) const
    {
        return (iazArray == other.iazArray)
            && (antGainPhase == other.antGainPhase)
            && (dwellTimeArray == other.dwellTimeArray)
            && (addedSupportArray == other.addedSupportArray);
    }
    bool operator!=(const SupportArray& other) const
    {
        return !((*this) == other);
    }

    //! Get IAZ support array by unique id
    SupportArrayParameter getIAZSupportArray(const std::string& key) const;

    //! Get AGP support array by unique id
    SupportArrayParameter getAGPSupportArray(const std::string& key) const;

    //! Get Dwell Time array by unique id
    SupportArrayParameter getDWTSupportArray(const std::string& key) const;

    //! Get AGP support array by unique id
    AdditionalSupportArray getAddedSupportArray(const std::string& key) const;

    //! Vector of IAZ type arrays
    std::vector<SupportArrayParameter> iazArray;

    //! Vector of AGP type arrays
    std::vector<SupportArrayParameter> antGainPhase;

    //! Dwell Time Array
    std::vector<DwellTimeArray> dwellTimeArray; // new in CPHD 1.1.0

    //! Map of additonally defined support arrays
    std::unordered_map<std::string, AdditionalSupportArray> addedSupportArray;
};

//! Ostream operators
std::ostream& operator<< (std::ostream& os, const SupportArrayParameter& s);
std::ostream& operator<< (std::ostream& os, const AdditionalSupportArray& a);
std::ostream& operator<< (std::ostream& os, const SupportArray& s);
}

#endif // SIX_cphd_SupportArray_h_INCLUDED_
