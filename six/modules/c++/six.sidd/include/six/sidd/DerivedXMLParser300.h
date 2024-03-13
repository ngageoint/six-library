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
#include <six/sidd/DerivedXMLControl.h>

namespace six
{
namespace sidd
{
struct DerivedXMLParser300 final : public DerivedXMLParser
{
    //DerivedXMLParser300(std::unique_ptr<logging::Logger>&&);
    DerivedXMLParser300(logging::Logger&, six::sidd300::ISMVersion);
    DerivedXMLParser300(const DerivedXMLParser300&) = delete;
    DerivedXMLParser300& operator=(const DerivedXMLParser300&) = delete;
    DerivedXMLParser300(DerivedXMLParser300&&) = delete;
    DerivedXMLParser300& operator=(DerivedXMLParser300&&) = delete;

    xml::lite::Document* toXML(const DerivedData* data) const override;
    std::unique_ptr<xml::lite::Document> toXML(const DerivedData&) const override;

    DerivedData* fromXML(const xml::lite::Document* doc) const override;
    std::unique_ptr<DerivedData> fromXML(const xml::lite::Document&) const override;

    six::sidd300::ISMVersion getISMVersion() const
    {
        return mISMVersion;
    }
    void setISMVersion(six::sidd300::ISMVersion value)
    {
        mISMVersion = value;
    }

private:
    six::sidd300::ISMVersion mISMVersion = six::sidd300::ISMVersion::current;

    XMLElem convertDerivedClassificationToXML(const DerivedClassification&, XMLElem parent = nullptr) const override;
    void parseDerivedClassificationFromXML(const xml::lite::Element* classificationElem, DerivedClassification&) const override;

    XMLElem convertDisplayToXML(const Display&, XMLElem parent = nullptr) const override;
    void parseDisplayFromXML(const xml::lite::Element& displayElem, Display&) const;

    XMLElem convertMeasurementToXML(const Measurement*, XMLElem parent = nullptr) const override;
    void parseMeasurementFromXML(const xml::lite::Element*, Measurement* measurement) const override;

    XMLElem convertExploitationFeaturesToXML(const ExploitationFeatures*, XMLElem parent = nullptr) const override;
    void parseProductFromXML(const xml::lite::Element* exploitationFeaturesElem, ExploitationFeatures*) const override;
    void parseExploitationFeaturesFromXML(const xml::lite::Element*, ExploitationFeatures*) const override;

    XMLElem createLUT(const std::string& name, const LUT* l, XMLElem parent = nullptr) const override;
    std::unique_ptr<LUT> parseSingleLUT(const xml::lite::Element& elem, size_t size) const;

    void parseCompressionFromXML(const xml::lite::Element& compressionElem, Compression&) const;
    XMLElem convertCompressionToXML(const Compression*, XMLElem parent = nullptr) const override;

    void parseInteractiveProcessingFromXML(const xml::lite::Element& interactiveElem, InteractiveProcessing&) const;
    void parseLookupTableFromXML(const xml::lite::Element& lookupElem, LookupTable&) const;
    void parseNonInteractiveProcessingFromXML(const xml::lite::Element& procElem, NonInteractiveProcessing&) const;
    void parsePredefinedFilterFromXML(const xml::lite::Element* predefinedElem, Filter::Predefined&) const;
    void parseKernelFromXML(const xml::lite::Element* kernelElem, Filter::Kernel&) const;
    void parseBankFromXML(const xml::lite::Element* bankElem, Filter::Bank&) const;
    void parseFilterFromXML(const xml::lite::Element& filterELem, Filter& filter) const;

    void parseJ2KCompression(const xml::lite::Element& j2kElem, J2KCompression& j2k) const;
    static void convertJ2KToXML(const DerivedXMLParser&,
        const J2KCompression&, xml::lite::Element& parent);

    void parseGeoDataFromXML(const xml::lite::Element& elem, GeoDataBase&) const;
    void parseDigitalElevationDataFromXML(const xml::lite::Element& elem, DigitalElevationData&) const;
    void parseProductGenerationOptionsFromXML(const xml::lite::Element& optionsElem, ProductGenerationOptions&) const;
    void parseBandEqualizationFromXML(const xml::lite::Element& bandElem, BandEqualization&) const;
    void parseRRDSFromXML(const xml::lite::Element& rrdsElem, RRDS& rrds) const;
    void parseGeometricTransformFromXML(const xml::lite::Element& geomElem, GeometricTransform&) const;
    void parseSharpnessEnhancementFromXML(const xml::lite::Element& sharpElem, SharpnessEnhancement&) const;
    void parseColorSpaceTransformFromXML(const xml::lite::Element& colorElem, ColorSpaceTransform&) const;
    void parseDynamicRangeAdjustmentFromXML(const xml::lite::Element& rangeElem, DynamicRangeAdjustment&) const;
};
}
}

#endif // SIX_sidd_DerivedXMLParser300_h_INCLUDED_
