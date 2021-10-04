/* =========================================================================
 * This file is part of six.sidd-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
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
#ifndef __SIX_SIDD_DERIVED_XML_PARSER_H__
#define __SIX_SIDD_DERIVED_XML_PARSER_H__

#include <six/XMLControl.h>
#include <six/sidd/DerivedData.h>
#include <six/sidd/SFA.h>
#include <six/SICommonXMLParser.h>

namespace six
{
namespace sidd
{
class DerivedXMLParser : public six::XMLParser
{
public:
    DerivedXMLParser(const std::string& version,
                     std::unique_ptr<six::SICommonXMLParser>&& comParser,
                     logging::Logger* log = nullptr,
                     bool ownLog = false);
#if !CODA_OSS_cpp17
    DerivedXMLParser(const std::string& version,
                     mem::auto_ptr<six::SICommonXMLParser> comParser,
                     logging::Logger* log = nullptr,
                     bool ownLog = false);
#endif

    DerivedXMLParser(const DerivedXMLParser&) = delete;
    DerivedXMLParser& operator=(const DerivedXMLParser&) = delete;

    virtual xml::lite::Document* toXML(const DerivedData* data) const = 0;

    virtual DerivedData* fromXML(const xml::lite::Document* doc) const = 0;

protected:
    virtual void parseDerivedClassificationFromXML(
            const xml::lite::Element* classificationElem,
            DerivedClassification& classification) const;

    virtual XMLElem convertDerivedClassificationToXML(
            const DerivedClassification& classification,
            XMLElem parent = nullptr) const = 0;

    virtual void parseProductFromXML(
            const xml::lite::Element* exploitationFeaturesElem,
            ExploitationFeatures* exploitationFeatures) const = 0;

    virtual XMLElem convertDisplayToXML(const Display& display,
                                        XMLElem parent = nullptr) const = 0;

    static const char SFA_URI[];

    const six::SICommonXMLParser& common() const
    {
        return *(mCommon.get());
    }

    static
    void getAttributeList(const xml::lite::Attributes& attributes,
                          const std::string& attributeName,
                          std::vector<std::string>& values);

    static
    void getAttributeListIfExists(const xml::lite::Attributes& attributes,
                                  const std::string& attributeName,
                                  std::vector<std::string>& values);

    static
    void getAttributeIfExists(const xml::lite::Attributes& attributes,
                              const std::string& attributeName,
                              std::string& value);
    static
    void getAttributeIfExists(const xml::lite::Attributes& attributes,
                              const std::string& attributeName,
                              ptrdiff_t& value);

    static
    void getAttributeIfExists(const xml::lite::Attributes& attributes,
                              const std::string& attributeName,
                              size_t& value);

    static
    void getAttributeIfExists(const xml::lite::Attributes& attributes,
                              const std::string& attributeName,
                              mem::ScopedCopyablePtr<DateTime>& date);

    static
    void getAttributeIfExists(const xml::lite::Attributes& attributes,
                              const std::string& attributeName,
                              BooleanType& boolean);

    static
    void setAttributeList(XMLElem element,
                          const std::string& attributeName,
                          const std::vector<std::string>& values,
                          const std::string& uri = "",
                          bool setIfEmpty = false);

    static
    void setAttributeIfNonEmpty(XMLElem element,
                                const std::string& name,
                                const std::string& value,
                                const std::string& uri = "");

    static
    void setAttributeIfNonEmpty(XMLElem element,
                                const std::string& name,
                                six::BooleanType value,
                                const std::string& uri = "");

    static
    void setAttributeIfNonNull(XMLElem element,
                               const std::string& name,
                               const DateTime* value,
                               const std::string& uri = "");

    virtual XMLElem createLUT(const std::string& name, const LUT *l,
            XMLElem parent = nullptr) const;
    XMLElem createLUTImpl(const LUT *l, XMLElem lutElem) const;
    XMLElem createFootprint(const std::string& name,
                            const std::string& cornerName,
                            const LatLonCorners& corners,
                            XMLElem parent = nullptr) const;

    XMLElem createFootprint(const std::string& name,
                            const std::string& cornerName,
                            const LatLonAltCorners& corners,
                            XMLElem parent = nullptr) const;

    XMLElem createSFAPoint(const std::string& localName,
                           const SFAPoint* point,
                           XMLElem parent = nullptr) const;

    XMLElem createSFALine(const std::string& localName,
                          const SFALineString* lineStr,
                          XMLElem parent) const;

    XMLElem createSFADatum(const std::string& name,
                           const six::sidd::SFADatum& datum,
                           XMLElem parent = nullptr) const;

    XMLElem convertProductCreationToXML(const ProductCreation* productCreation,
                                        XMLElem parent = nullptr) const;
    XMLElem convertProcessorInformationToXML(const ProcessorInformation* processorInformation,
                                             XMLElem parent) const;
    XMLElem convertProductProcessingToXML(const ProductProcessing* productProcessing,
                                          XMLElem parent = nullptr) const;
    XMLElem convertProcessingModuleToXML(const ProcessingModule* procMod,
                                         XMLElem parent = nullptr) const;
    XMLElem convertDownstreamReprocessingToXML(const DownstreamReprocessing* d,
                                               XMLElem parent = nullptr) const;
    XMLElem convertDisplayToXML(const Display* display,
                                XMLElem parent = nullptr) const;
    void convertRemapToXML(const Remap& remap, XMLElem parent = nullptr) const;

    XMLElem convertGeographicCoverageToXML(const std::string& localName,
                                           const GeographicCoverage* g,
                                           XMLElem parent = nullptr) const;
    virtual XMLElem convertMeasurementToXML(const Measurement* measurement,
                                    XMLElem parent = nullptr) const;
    virtual XMLElem convertExploitationFeaturesToXML(const ExploitationFeatures* exploitationFeatures,
                                                     XMLElem parent = nullptr) const = 0;
    XMLElem convertAnnotationToXML(const Annotation *a,
                                   XMLElem parent = nullptr) const;
    XMLElem convertCompressionToXML(const Compression *c,
                                    XMLElem parent = nullptr) const;
    void    convertJ2KToXML(const J2KCompression* c, XMLElem& parent) const;
    XMLElem convertSFAGeometryToXML(const SFAGeometry *g,
                                    XMLElem parent = nullptr) const;
    XMLElem convertGeographicCoordinateSystemToXML(
            const SFAGeographicCoordinateSystem* geographicCoordinateSystem,
            XMLElem parent) const;
    void parseProductCreationFromXML(const xml::lite::Element* productCreationElem,
                                     ProductCreation* productCreation) const;
    void parseProductCreationFromXML(const xml::lite::Element* informationElem,
                                     ProcessorInformation* processorInformation) const;
    void parseProductProcessingFromXML(const xml::lite::Element* elem,
                                       ProductProcessing* productProcessing) const;
    void parseProcessingModuleFromXML(const xml::lite::Element* elem,
                                      ProcessingModule* procMod) const;
    void parseDownstreamReprocessingFromXML(const xml::lite::Element* elem,
                                            DownstreamReprocessing* downstreamReproc) const;
    Remap* parseRemapChoiceFromXML(const xml::lite::Element* remapInformationElem) const;
    mem::auto_ptr<LUT> parseSingleLUT(const xml::lite::Element* elem) const;
    void parseDisplayFromXML(const xml::lite::Element* displayElem, Display* display) const;
    virtual void parseMeasurementFromXML(const xml::lite::Element* measurementElem,
                                 Measurement* measurement) const;
    virtual void parseExploitationFeaturesFromXML(const xml::lite::Element* elem,
                                          ExploitationFeatures* exploitationFeatures) const;
    void parseAnnotationFromXML(const xml::lite::Element* annotationElem,
                                Annotation* a) const;
    void parseSFAGeometryFromXML(const xml::lite::Element* elem,
                                 SFAGeometry* g) const;
    void parseGeographicCoordinateSystemFromXML(
            const xml::lite::Element* coorSysElem,
            SFAGeographicCoordinateSystem* coordSys) const;
    void parseDatum(const xml::lite::Element* datumElem, SFADatum& datum) const;
    XMLElem parsePolynomialProjection(const xml::lite::Element* projElem, const Measurement& measurement) const;
    XMLElem parseGeographicProjection(const xml::lite::Element* projElem, const Measurement& measurement) const;
    XMLElem parsePlaneProjection(const xml::lite::Element* projElem, const Measurement& measurement) const;
    XMLElem parseCylindricalProjection(const xml::lite::Element* projElem, const Measurement& measurement) const;

    static
    std::string versionToURI(const std::string& strVersion)
    {
        return ("urn:SIDD:" + strVersion);
    }

private:
    std::unique_ptr<six::SICommonXMLParser> mCommon;
};
}
}

#endif
