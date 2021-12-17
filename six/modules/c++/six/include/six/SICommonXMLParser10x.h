/* =========================================================================
 * This file is part of six-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
 *
 * six-c++ is free software; you can redistribute it and/or modify
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

#ifndef SIX_six_SICommonXMLParser10x_h_INCLUDED_
#define SIX_six_SICommonXMLParser10x_h_INCLUDED_
#pragma once

#include <six/SICommonXMLParser.h>

namespace six
{

struct SICommonXMLParser10x : public SICommonXMLParser
{
    SICommonXMLParser10x(const std::string& defaultURI, bool addClassAttributes, const std::string& siCommonURI,
        logging::Logger* log = nullptr, bool ownLog = false);
    SICommonXMLParser10x(const std::string& defaultURI, bool addClassAttributes, const std::string& siCommonURI,
        std::unique_ptr<logging::Logger>&&);
    SICommonXMLParser10x(const std::string& defaultURI, bool addClassAttributes, const std::string& siCommonURI,
        logging::Logger&);
    SICommonXMLParser10x(const SICommonXMLParser10x&) = delete;
    SICommonXMLParser10x(SICommonXMLParser10x&&) = delete;
    SICommonXMLParser10x& operator=(const SICommonXMLParser10x&) = delete;
    SICommonXMLParser10x& operator=(SICommonXMLParser10x&&) = delete;
    virtual ~SICommonXMLParser10x() = default;

    XMLElem convertRadiometryToXML(
        const Radiometric *obj,
        XMLElem parent = nullptr) const override;

    void parseRadiometryFromXML(
        const xml_lite::Element* radiometricXML,
        Radiometric *obj) const override;

    XMLElem convertMatchInformationToXML(
        const MatchInformation& matchInfo,
        XMLElem parent) const override;

    void parseMatchInformationFromXML(
        const xml_lite::Element* matchInfoXML,
        MatchInformation* info) const override;

protected:

    XMLElem convertCompositeSCPToXML(
        const ErrorStatistics* errorStatistics,
        XMLElem parent = nullptr) const override;

    void parseCompositeSCPFromXML(
        const xml_lite::Element* errorStatsXML,
        ErrorStatistics* errorStatistics) const override;

};
}

#endif // SIX_six_SICommonXMLParser10x_h_INCLUDED_
