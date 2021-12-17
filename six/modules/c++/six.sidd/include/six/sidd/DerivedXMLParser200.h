/* =========================================================================
 * This file is part of six.sidd-c++
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

#ifndef __SIX_SIDD_DERIVED_XML_PARSER_200_H__
#define __SIX_SIDD_DERIVED_XML_PARSER_200_H__

#include <six/sidd/DerivedXMLParser.h>
#include <six/GeoDataBase.h>

namespace six
{
namespace sidd
{
struct DerivedXMLParser200 : public DerivedXMLParser
{
    DerivedXMLParser200(logging::Logger* log = nullptr,
                        bool ownLog = false);


    DerivedXMLParser200(const DerivedXMLParser200&) = delete;
    DerivedXMLParser200& operator=(const DerivedXMLParser200&) = delete;

    virtual xml_lite::Document* toXML(const DerivedData* data) const override;
    std::unique_ptr<xml_lite::Document> toXML(const DerivedData&) const override;

    virtual DerivedData* fromXML(const xml_lite::Document* doc) const override;
    std::unique_ptr<DerivedData> fromXML(const xml_lite::Document&) const override;

    static void validateDRAFields(const six::sidd::DRAType&, bool hasDraParameters, bool hasDraOverrides);
    static void validateDRAFields(const six::sidd::DynamicRangeAdjustment&);

    static ProjectionType getProjectionType(const xml_lite::Element& measurementElem);
    static std::unique_ptr<LUT> parseSingleLUT(const std::string& lutStr, size_t size);

    static xml_lite::Element& convertCompressionToXML(const DerivedXMLParser&,
        const Compression& compression, xml_lite::Element& parent);

    static xml_lite::Element& convertDigitalElevationDataToXML(const DerivedXMLParser&, 
        const DigitalElevationData&, xml_lite::Element& parent);

    static xml_lite::Element& convertGeoDataToXML(const DerivedXMLParser&,
        const GeoDataBase&, xml_lite::Element& parent);

    static xml_lite::Element& convertDerivedClassificationToXML(const DerivedXMLParser&,
        const DerivedClassification&, xml_lite::Element& parent);

    static xml_lite::Element& convertMeasurementToXML(const DerivedXMLParser&,
        const Measurement&, xml_lite::Element& parent);

    static xml_lite::Element& convertExploitationFeaturesToXML(const DerivedXMLParser&,
        const ExploitationFeatures&, xml_lite::Element& parent);

    static xml_lite::Element& createLUT(const DerivedXMLParser&,
        const std::string& name, const LUT&, xml_lite::Element& parent);

    static xml_lite::Element& convertDisplayToXML(const DerivedXMLParser&,
        const Display&, xml_lite::Element& parent);

protected:
    virtual void parseDerivedClassificationFromXML(
            const xml_lite::Element* classificationElem,
            DerivedClassification& classification) const;

    virtual XMLElem convertDerivedClassificationToXML(
            const DerivedClassification& classification,
            XMLElem parent = nullptr) const override;

    virtual void parseProductFromXML(
        const xml_lite::Element* exploitationFeaturesElem,
        ExploitationFeatures* exploitationFeatures) const;

    virtual void parseCompressionFromXML(const xml_lite::Element* compressionElem,
                                         Compression& compression) const;

    virtual XMLElem convertCompressionToXML(const Compression& compression,
                                    XMLElem parent = nullptr) const;

    virtual XMLElem convertDisplayToXML(const Display& display,
                                        XMLElem parent = nullptr) const override;

    virtual void parseDisplayFromXML(const xml_lite::Element* displayElem,
                                     Display& display) const;

    virtual XMLElem convertMeasurementToXML(
            const Measurement* measurement,
            XMLElem parent = nullptr) const override;

    virtual void parseMeasurementFromXML(
            const xml_lite::Element* measurementElem,
            Measurement* measurement) const;

    virtual void parseExploitationFeaturesFromXML(
        const xml_lite::Element* exploitationFeaturesElem,
        ExploitationFeatures* exploitationFeatures) const;

    virtual XMLElem convertExploitationFeaturesToXML(
        const ExploitationFeatures* exploitationFeatures,
        XMLElem parent = nullptr) const override;

    virtual XMLElem createLUT(const std::string& name, const LUT *l,
        XMLElem parent = nullptr) const override;

private:
    static const char VERSION[];
    static const char SI_COMMON_URI[];
    static const char ISM_URI[];

    static xml_lite::Element& convertLookupTableToXML(const DerivedXMLParser&,
        const std::string& name, const LookupTable&, xml_lite::Element& parent);

    static xml_lite::Element& convertNonInteractiveProcessingToXML(const DerivedXMLParser&,
        const NonInteractiveProcessing&, xml_lite::Element& parent);

    static xml_lite::Element& convertInteractiveProcessingToXML(const DerivedXMLParser&,
        const InteractiveProcessing&, xml_lite::Element& parent);

    static xml_lite::Element& convertPredefinedFilterToXML(const DerivedXMLParser&,
        const Filter::Predefined&, xml_lite::Element& parent);
    static xml_lite::Element& convertKernelToXML(const DerivedXMLParser&,
        const Filter::Kernel&, xml_lite::Element& parent);
    static xml_lite::Element& convertBankToXML(const DerivedXMLParser&,
        const Filter::Bank&, xml_lite::Element& parent);

    static xml_lite::Element& convertFilterToXML(const DerivedXMLParser&,
        const std::string& name, const Filter&, xml_lite::Element& parent);

    static void convertJ2KToXML(const DerivedXMLParser&,
        const J2KCompression&, xml_lite::Element& parent);

    void parseJ2KCompression(const xml_lite::Element* j2kElem,
                             J2KCompression& j2k) const;

    void parseGeoDataFromXML(
            const xml_lite::Element* elem, GeoDataBase* geoData) const;

    void parseDigitalElevationDataFromXML(const xml_lite::Element* elem,
                                          DigitalElevationData& ded) const;

    void parseNonInteractiveProcessingFromXML(const xml_lite::Element* procElem,
         NonInteractiveProcessing& nonInteractiveProcessing) const;

    void parseProductGenerationOptionsFromXML(const xml_lite::Element* optionsElem,
         ProductGenerationOptions& options) const;

    void parseBandEqualizationFromXML(const xml_lite::Element* bandElem,
         BandEqualization& band) const;

    void parseRRDSFromXML(const xml_lite::Element* rrdsElem, RRDS& rrds) const;

    void parsePredefinedFilterFromXML(const xml_lite::Element* predefinedElem,
         Filter::Predefined& predefined) const;

    void parseKernelFromXML(const xml_lite::Element* kernelElem,
         Filter::Kernel& kernel) const;

    void parseBankFromXML(const xml_lite::Element* bankElem, Filter::Bank& bank) const;

    void parseFilterFromXML(const xml_lite::Element* filterELem, Filter& filter) const;

    void parseInteractiveProcessingFromXML(const xml_lite::Element* interactiveElem,
         InteractiveProcessing& interactive) const;

    void parseGeometricTransformFromXML(const xml_lite::Element* geomElem,
         GeometricTransform& transform) const;

    void parseSharpnessEnhancementFromXML(const xml_lite::Element* sharpElem,
         SharpnessEnhancement& sharpness) const;

    void parseColorSpaceTransformFromXML(const xml_lite::Element* colorElem,
         ColorSpaceTransform& transform) const;

    void parseDynamicRangeAdjustmentFromXML(const xml_lite::Element* rangeElem,
         DynamicRangeAdjustment& rangeAdjustment) const;

    void parseLookupTableFromXML(const xml_lite::Element* lookupElem,
          LookupTable& lookupTable) const;

    mem::auto_ptr<LUT> parseSingleLUT(const xml_lite::Element* elem,
            size_t size) const;
};
}
}

#endif
