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
struct DerivedXMLParser300 : public DerivedXMLParser
{
    DerivedXMLParser300(logging::Logger* log = nullptr,
                        bool ownLog = false);


    DerivedXMLParser300(const DerivedXMLParser300&) = delete;
    DerivedXMLParser300& operator=(const DerivedXMLParser300&) = delete;

    xml::lite::Document* toXML(const DerivedData* data) const override;

    DerivedData* fromXML(const xml::lite::Document* doc) const override;

protected:
    void parseDerivedClassificationFromXML(
            const xml::lite::Element* classificationElem,
            DerivedClassification& classification) const override;

    XMLElem convertDerivedClassificationToXML(
            const DerivedClassification& classification,
            XMLElem parent = nullptr) const override;

    void parseProductFromXML(
        const xml::lite::Element* exploitationFeaturesElem,
        ExploitationFeatures* exploitationFeatures) const override;

    virtual void parseCompressionFromXML(const xml::lite::Element* compressionElem,
                                         Compression& compression) const;

    virtual XMLElem convertCompressionToXML(const Compression& compression,
                                    XMLElem parent = nullptr) const;

    XMLElem convertDisplayToXML(const Display& display,
                                        XMLElem parent = nullptr) const override;

    virtual void parseDisplayFromXML(const xml::lite::Element* displayElem,
                                     Display& display) const;

    XMLElem convertMeasurementToXML(
            const Measurement* measurement,
            XMLElem parent = nullptr) const override;

    void parseMeasurementFromXML(
            const xml::lite::Element* measurementElem,
            Measurement* measurement) const override;

    void parseExploitationFeaturesFromXML(
        const xml::lite::Element* exploitationFeaturesElem,
        ExploitationFeatures* exploitationFeatures) const override;

    XMLElem convertExploitationFeaturesToXML(
        const ExploitationFeatures* exploitationFeatures,
        XMLElem parent = nullptr) const override;

    XMLElem createLUT(const std::string& name, const LUT *l,
        XMLElem parent = nullptr) const override;


private:
    XMLElem convertLookupTableToXML(
            const std::string& name,
            const LookupTable& table,
            XMLElem parent = nullptr) const;

    XMLElem convertNonInteractiveProcessingToXML(
            const NonInteractiveProcessing& processing,
            XMLElem parent = nullptr) const;

    XMLElem convertInteractiveProcessingToXML(
            const InteractiveProcessing& processing,
            XMLElem parent = nullptr) const;

    XMLElem convertPredefinedFilterToXML(
            const Filter::Predefined& predefined,
            XMLElem parent = nullptr) const;

    XMLElem convertKernelToXML(
            const Filter::Kernel& kernel,
            XMLElem parent = nullptr) const;

    XMLElem convertBankToXML(
            const Filter::Bank& bank,
            XMLElem parent = nullptr) const;

    XMLElem convertFilterToXML(const std::string& name,
                               const Filter& Filter,
                               XMLElem parent = nullptr) const;

    void convertJ2KToXML(const J2KCompression& j2k,
                         XMLElem& parent) const;

    XMLElem convertGeoDataToXML(const GeoDataBase* g,
                                XMLElem parent = nullptr) const;

    XMLElem convertDigitalElevationDataToXML(const DigitalElevationData& ded,
                                             XMLElem parent = nullptr) const;

    void parseJ2KCompression(const xml::lite::Element* j2kElem,
                             J2KCompression& j2k) const;

    void parseGeoDataFromXML(
            const xml::lite::Element* elem, GeoDataBase* geoData) const;

    void parseDigitalElevationDataFromXML(const xml::lite::Element* elem,
                                          DigitalElevationData& ded) const;

    void parseNonInteractiveProcessingFromXML(const xml::lite::Element* procElem,
         NonInteractiveProcessing& nonInteractiveProcessing) const;

    void parseProductGenerationOptionsFromXML(const xml::lite::Element* optionsElem,
         ProductGenerationOptions& options) const;

    void parseBandEqualizationFromXML(const xml::lite::Element* bandElem,
         BandEqualization& band) const;

    void parseRRDSFromXML(const xml::lite::Element* rrdsElem, RRDS& rrds) const;

    void parsePredefinedFilterFromXML(const xml::lite::Element* predefinedElem,
         Filter::Predefined& predefined) const;

    void parseKernelFromXML(const xml::lite::Element* kernelElem,
         Filter::Kernel& kernel) const;

    void parseBankFromXML(const xml::lite::Element* bankElem, Filter::Bank& bank) const;

    void parseFilterFromXML(const xml::lite::Element* filterELem, Filter& filter) const;

    void parseInteractiveProcessingFromXML(const xml::lite::Element* interactiveElem,
         InteractiveProcessing& interactive) const;

    void parseGeometricTransformFromXML(const xml::lite::Element* geomElem,
         GeometricTransform& transform) const;

    void parseSharpnessEnhancementFromXML(const xml::lite::Element* sharpElem,
         SharpnessEnhancement& sharpness) const;

    void parseColorSpaceTransformFromXML(const xml::lite::Element* colorElem,
         ColorSpaceTransform& transform) const;

    void parseDynamicRangeAdjustmentFromXML(const xml::lite::Element* rangeElem,
         DynamicRangeAdjustment& rangeAdjustment) const;

    void parseLookupTableFromXML(const xml::lite::Element* lookupElem,
          LookupTable& lookupTable) const;

    std::unique_ptr<LUT> parseSingleLUT(const xml::lite::Element& elem,
            size_t size) const;
};
}
}

#endif // SIX_sidd_DerivedXMLParser300_h_INCLUDED_
