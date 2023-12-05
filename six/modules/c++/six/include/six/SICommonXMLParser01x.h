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

#pragma once
#ifndef __SIX_SI_COMPLEX_COMMON_XML_PARSER_01X_H__
#define __SIX_SI_COMPLEX_COMMON_XML_PARSER_01X_H__

#include <six/SICommonXMLParser.h>
#include <six/Exports.h>

namespace six
{

class SIX_SIX_API SICommonXMLParser01x : public SICommonXMLParser
{
public:
    SICommonXMLParser01x(const std::string& defaultURI,
                         bool addClassAttributes,
                         const std::string& siCommonURI,
                         logging::Logger* log = nullptr,
                         bool ownLog = false);

    SICommonXMLParser01x(const SICommonXMLParser01x&) = delete;
    SICommonXMLParser01x& operator=(const SICommonXMLParser01x&) = delete;
    SICommonXMLParser01x& operator=(SICommonXMLParser01x&&) = delete;

    XMLElem convertRadiometryToXML(
        const Radiometric *obj,
        XMLElem parent = nullptr) const override;

    void parseRadiometryFromXML(
        const xml::lite::Element* radiometricXML,
        Radiometric *obj) const override;

    XMLElem convertMatchInformationToXML(
        const MatchInformation& matchInfo,
        XMLElem parent) const override;

    void parseMatchInformationFromXML(
        const xml::lite::Element* matchInfoXML,
        MatchInformation* info) const override;

protected:

    XMLElem convertCompositeSCPToXML(
        const ErrorStatistics* errorStatistics,
        XMLElem parent = nullptr) const override;

    void parseCompositeSCPFromXML(
        const xml::lite::Element* errorStatsXML,
        ErrorStatistics* errorStatistics) const override;

};

}

#endif

