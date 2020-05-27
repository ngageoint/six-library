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
                     std::auto_ptr<six::SICommonXMLParser> comParser,
                     logging::Logger* log = NULL,
                     bool ownLog = false);

    virtual xml::lite::Document* toXML(const DerivedData* data) const = 0;

    virtual DerivedData* fromXML(const xml::lite::Document* doc) const = 0;

protected:
    virtual void parseDerivedClassificationFromXML(
            const XMLElem classificationElem,
            DerivedClassification& classification) const;

    virtual XMLElem convertDerivedClassificationToXML(
            const DerivedClassification& classification,
            XMLElem parent = NULL) const = 0;

    virtual void parseProductFromXML(
            const XMLElem exploitationFeaturesElem,
            ExploitationFeatures* exploitationFeatures) const = 0;

    virtual XMLElem convertDisplayToXML(const Display& display,
                                        XMLElem parent = NULL) const = 0;

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
                              sys::SSize_T& value);

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
            XMLElem parent = NULL) const;
    XMLElem createLUTImpl(const LUT *l, XMLElem lutElem) const;
    XMLElem createFootprint(const std::string& name,
                            const std::string& cornerName,
                            const LatLonCorners& corners,
                            XMLElem parent = NULL) const;

    XMLElem createFootprint(const std::string& name,
                            const std::string& cornerName,
                            const LatLonAltCorners& corners,
                            XMLElem parent = NULL) const;

    XMLElem createSFAPoint(const std::string& localName,
                           const SFAPoint* point,
                           XMLElem parent = NULL) const;

    XMLElem createSFALine(const std::string& localName,
                          const SFALineString* lineStr,
                          XMLElem parent) const;

    XMLElem createSFADatum(const std::string& name,
                           const six::sidd::SFADatum& datum,
                           XMLElem parent = NULL) const;

    XMLElem convertProductCreationToXML(const ProductCreation* productCreation,
                                        XMLElem parent = NULL) const;
    XMLElem convertProcessorInformationToXML(const ProcessorInformation* processorInformation,
                                             XMLElem parent) const;
    XMLElem convertProductProcessingToXML(const ProductProcessing* productProcessing,
                                          XMLElem parent = NULL) const;
    XMLElem convertProcessingModuleToXML(const ProcessingModule* procMod,
                                         XMLElem parent = NULL) const;
    XMLElem convertDownstreamReprocessingToXML(const DownstreamReprocessing* d,
                                               XMLElem parent = NULL) const;
    XMLElem convertDisplayToXML(const Display* display,
                                XMLElem parent = NULL) const;
    void convertRemapToXML(const Remap& remap, XMLElem parent = NULL) const;

    XMLElem convertGeographicCoverageToXML(const std::string& localName,
                                           const GeographicCoverage* g,
                                           XMLElem parent = NULL) const;
    virtual XMLElem convertMeasurementToXML(const Measurement* measurement,
                                    XMLElem parent = NULL) const;
    virtual XMLElem convertExploitationFeaturesToXML(const ExploitationFeatures* exploitationFeatures,
                                                     XMLElem parent = NULL) const = 0;
    XMLElem convertAnnotationToXML(const Annotation *a,
                                   XMLElem parent = NULL) const;
    XMLElem convertCompressionToXML(const Compression *c,
                                    XMLElem parent = NULL) const;
    void    convertJ2KToXML(const J2KCompression* c, XMLElem& parent) const;
    XMLElem convertSFAGeometryToXML(const SFAGeometry *g,
                                    XMLElem parent = NULL) const;
    XMLElem convertGeographicCoordinateSystemToXML(
            const SFAGeographicCoordinateSystem* geographicCoordinateSystem,
            XMLElem parent) const;
    void parseProductCreationFromXML(const XMLElem productCreationElem,
                                     ProductCreation* productCreation) const;
    void parseProductCreationFromXML(const XMLElem informationElem,
                                     ProcessorInformation* processorInformation) const;
    void parseProductProcessingFromXML(const XMLElem elem,
                                       ProductProcessing* productProcessing) const;
    void parseProcessingModuleFromXML(const XMLElem elem,
                                      ProcessingModule* procMod) const;
    void parseDownstreamReprocessingFromXML(const XMLElem elem,
                                            DownstreamReprocessing* downstreamReproc) const;
    Remap* parseRemapChoiceFromXML(const XMLElem remapInformationElem) const;
    std::auto_ptr<LUT> parseSingleLUT(const XMLElem elem) const;
    void parseDisplayFromXML(const XMLElem displayElem, Display* display) const;
    virtual void parseMeasurementFromXML(const XMLElem measurementElem,
                                 Measurement* measurement) const;
    virtual void parseExploitationFeaturesFromXML(const XMLElem elem,
                                          ExploitationFeatures* exploitationFeatures) const;
    void parseAnnotationFromXML(const XMLElem annotationElem,
                                Annotation* a) const;
    void parseSFAGeometryFromXML(const XMLElem elem,
                                 SFAGeometry* g) const;
    void parseGeographicCoordinateSystemFromXML(
            const XMLElem coorSysElem,
            SFAGeographicCoordinateSystem* coordSys) const;
    void parseDatum(const XMLElem datumElem, SFADatum& datum) const;
    XMLElem parsePolynomialProjection(XMLElem projElem, Measurement& measurement) const;
    XMLElem parseGeographicProjection(XMLElem projElem, Measurement& measurement) const;
    XMLElem parsePlaneProjection(XMLElem projElem, Measurement& measurement) const;
    XMLElem parseCylindricalProjection(XMLElem projElem, Measurement& measurement) const;

    static
    std::string versionToURI(const std::string& version)
    {
        return ("urn:SIDD:" + version);
    }

private:
    std::auto_ptr<six::SICommonXMLParser> mCommon;
};
}
}

#endif
