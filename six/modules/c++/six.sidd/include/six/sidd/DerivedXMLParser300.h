/* =========================================================================
 * This file is part of six.sidd-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2015, MDA Information Systems LLC
 * (C) Copyright 2021, Maxar Technologies, Inc.
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

#ifndef SIX_sidd_DerivedXMLParser300_h_INCLUDED_
#define SIX_sidd_DerivedXMLParser300_h_INCLUDED_

#include <six/sidd/DerivedXMLParser.h>
#include <six/GeoDataBase.h>

namespace six
{
namespace sidd
{
struct DerivedXMLParser300 final : public DerivedXMLParser
{
    //DerivedXMLParser300(std::unique_ptr<logging::Logger>&&);
    DerivedXMLParser300(logging::Logger&);
    DerivedXMLParser300(const DerivedXMLParser300&) = delete;
    DerivedXMLParser300& operator=(const DerivedXMLParser300&) = delete;
    DerivedXMLParser300(DerivedXMLParser300&&) = delete;
    DerivedXMLParser300& operator=(DerivedXMLParser300&&) = delete;

    xml_lite::Document* toXML(const DerivedData* data) const override;
    std::unique_ptr<xml_lite::Document> toXML(const DerivedData&) const override;

    DerivedData* fromXML(const xml_lite::Document* doc) const override;
    std::unique_ptr<DerivedData> fromXML(const xml_lite::Document&) const override;

private:
    XMLElem convertDerivedClassificationToXML(const DerivedClassification&, XMLElem parent = nullptr) const override;
    void parseDerivedClassificationFromXML(const xml_lite::Element* classificationElem, DerivedClassification&) const override;

    XMLElem convertDisplayToXML(const Display&, XMLElem parent = nullptr) const override;
    void parseDisplayFromXML(const xml_lite::Element& displayElem, Display&) const;

    XMLElem convertMeasurementToXML(const Measurement*, XMLElem parent = nullptr) const override;
    void parseMeasurementFromXML(const xml_lite::Element*, Measurement* measurement) const override;

    XMLElem convertExploitationFeaturesToXML(const ExploitationFeatures*, XMLElem parent = nullptr) const override;
    void parseProductFromXML(const xml_lite::Element* exploitationFeaturesElem, ExploitationFeatures*) const override;
    void parseExploitationFeaturesFromXML(const xml_lite::Element*, ExploitationFeatures*) const override;

    XMLElem createLUT(const std::string& name, const LUT* l, XMLElem parent = nullptr) const override;
    std::unique_ptr<LUT> parseSingleLUT(const xml_lite::Element& elem, size_t size) const;

    void parseCompressionFromXML(const xml_lite::Element& compressionElem, Compression&) const;
    void parseInteractiveProcessingFromXML(const xml_lite::Element& interactiveElem, InteractiveProcessing&) const;
    void parseLookupTableFromXML(const xml_lite::Element& lookupElem, LookupTable&) const;
    void parseNonInteractiveProcessingFromXML(const xml_lite::Element& procElem, NonInteractiveProcessing&) const;
    void parsePredefinedFilterFromXML(const xml_lite::Element* predefinedElem, Filter::Predefined&) const;
    void parseKernelFromXML(const xml_lite::Element* kernelElem, Filter::Kernel&) const;
    void parseBankFromXML(const xml_lite::Element* bankElem, Filter::Bank&) const;
    void parseFilterFromXML(const xml_lite::Element& filterELem, Filter& filter) const;
    void parseJ2KCompression(const xml_lite::Element& j2kElem, J2KCompression& j2k) const;
    void parseGeoDataFromXML(const xml_lite::Element& elem, GeoDataBase&) const;
    void parseDigitalElevationDataFromXML(const xml_lite::Element& elem, DigitalElevationData&) const;
    void parseProductGenerationOptionsFromXML(const xml_lite::Element& optionsElem, ProductGenerationOptions&) const;
    void parseBandEqualizationFromXML(const xml_lite::Element& bandElem, BandEqualization&) const;
    void parseRRDSFromXML(const xml_lite::Element& rrdsElem, RRDS& rrds) const;
    void parseGeometricTransformFromXML(const xml_lite::Element& geomElem, GeometricTransform&) const;
    void parseSharpnessEnhancementFromXML(const xml_lite::Element& sharpElem, SharpnessEnhancement&) const;
    void parseColorSpaceTransformFromXML(const xml_lite::Element& colorElem, ColorSpaceTransform&) const;
    void parseDynamicRangeAdjustmentFromXML(const xml_lite::Element& rangeElem, DynamicRangeAdjustment&) const;
};
}
}

#endif // SIX_sidd_DerivedXMLParser300_h_INCLUDED_
