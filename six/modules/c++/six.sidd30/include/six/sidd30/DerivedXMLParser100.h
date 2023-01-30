/* =========================================================================
 * This file is part of six.sidd30-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2015, MDA Information Systems LLC
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

#ifndef __SIX_SIDD_DERIVED_XML_PARSER_100_H__
#define __SIX_SIDD_DERIVED_XML_PARSER_100_H__

#include <six/sidd30/DerivedXMLParser.h>

namespace six
{
namespace sidd
{
class DerivedXMLParser100 : public DerivedXMLParser
{
public:
    DerivedXMLParser100(logging::Logger* log = nullptr,
                        bool ownLog = false);

    DerivedXMLParser100(const DerivedXMLParser100&) = delete;
    DerivedXMLParser100& operator=(const DerivedXMLParser100&) = delete;

    virtual xml::lite::Document* toXML(const DerivedData* data) const override;
    std::unique_ptr<xml::lite::Document> toXML(const DerivedData&) const override;

    virtual DerivedData* fromXML(const xml::lite::Document* doc) const override;
    std::unique_ptr<DerivedData> fromXML(const xml::lite::Document&) const override;

protected:
    virtual void parseDerivedClassificationFromXML(
            const xml::lite::Element* classificationElem,
            DerivedClassification& classification) const;

    virtual XMLElem convertDerivedClassificationToXML(
            const DerivedClassification& classification,
            XMLElem parent = nullptr) const;

    virtual XMLElem convertMeasurementToXML(
            const Measurement* measurement,
            XMLElem parent = nullptr) const;

    virtual void parseMeasurementFromXML(
            const xml::lite::Element* measruementElem,
            Measurement* measurement) const;

    virtual XMLElem convertExploitationFeaturesToXML(
        const ExploitationFeatures* exploitationFeatures,
        XMLElem parent = nullptr) const;

    virtual void parseProductFromXML(
        const xml::lite::Element* exploitationFeaturesElem,
        ExploitationFeatures* exploitationFeatures) const;

    virtual XMLElem convertDisplayToXML(const Display& display,
                                        XMLElem parent = nullptr) const;

private:
    static const char VERSION[];
    static const char SI_COMMON_URI[];
    static const char ISM_URI[];

    XMLElem convertGeographicTargetToXML(const GeographicAndTarget& g,
                                         XMLElem parent = nullptr) const;

    void parseGeographicTargetFromXML(
            const xml::lite::Element* elem,
            GeographicAndTarget* geographicAndTarget) const;

    void parseGeographicCoverageFromXML(
            const xml::lite::Element* elem,
            GeographicCoverage* geoCoverage) const;
};
}
}

#endif
