/* =========================================================================
 * This file is part of six.sidd-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2009, General Dynamics - Advanced Information Systems
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
                     logging::Logger* log = NULL,
                     bool ownLog = false);

    xml::lite::Document* toXML(const DerivedData* data) const;

    DerivedData* fromXML(const xml::lite::Document* doc) const;

protected:

    const six::SICommonXMLParser& common() const
    {
        return *(mCommon.get());
    }

private:
    static const char SI_COMMON_URI[];
    static const char SFA_URI[];
    static const char ISM_URI[];

    XMLElem createLUT(const std::string& name, const LUT *l,
            XMLElem parent = NULL) const;
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
    XMLElem convertDerivedClassificationToXML(const DerivedClassification& classification, 
                                              XMLElem parent = NULL) const;
    XMLElem convertProductProcessingToXML(const ProductProcessing* productProcessing, 
                                          XMLElem parent = NULL) const;
    XMLElem convertProcessingModuleToXML(const ProcessingModule* procMod, 
                                         XMLElem parent = NULL) const;
    XMLElem convertDownstreamReprocessingToXML(const DownstreamReprocessing* d, 
                                               XMLElem parent = NULL) const;
    XMLElem convertDisplayToXML(const Display* display, 
                                XMLElem parent = NULL) const;
    XMLElem convertGeographicTargetToXML(const GeographicAndTarget* g, 
                                         XMLElem parent = NULL) const;
    XMLElem convertGeographicCoverageToXML(const std::string& localName,
                                           const GeographicCoverage* g, 
                                           XMLElem parent = NULL) const;
    XMLElem convertMeasurementToXML(const Measurement* measurement, 
                                    XMLElem parent = NULL) const;
    XMLElem convertExploitationFeaturesToXML(const ExploitationFeatures* exFeatures,
                                             XMLElem parent = NULL) const;
    XMLElem convertAnnotationToXML(const Annotation *a, 
                                   XMLElem parent = NULL) const;
    XMLElem convertSFAGeometryToXML(const SFAGeometry *g,
                                    XMLElem parent = NULL) const;
    XMLElem convertGeographicCoordinateSystemToXML(
            const SFAGeographicCoordinateSystem* geographicCoordinateSystem,
            XMLElem parent) const;


    void parseProductCreationFromXML(const XMLElem productCreationXML,
                                     ProductCreation* productCreation) const;
    void parseProductCreationFromXML(const XMLElem informationXML,
                                     ProcessorInformation* processorInformation) const;
    void parseDerivedClassificationFromXML(const XMLElem classificationXML,
                                           DerivedClassification& classification) const;
    void parseProductProcessingFromXML(const XMLElem elem, 
                                       ProductProcessing* productProcessing) const;
    void parseProcessingModuleFromXML(const XMLElem elem, 
                                      ProcessingModule* procMod) const;
    void parseDownstreamReprocessingFromXML(const XMLElem elem, 
                                            DownstreamReprocessing* downstreamReproc) const;
    Remap* parseRemapChoiceFromXML(const XMLElem remapInformationXML) const;
    void parseDisplayFromXML(const XMLElem displayXML, Display* display) const;
    void parseGeographicTargetFromXML(const XMLElem elem, 
                                      GeographicAndTarget* geographicAndTarget) const;
    void parseGeographicCoverageFromXML(const XMLElem elem, 
                                        GeographicCoverage* geoCoverage) const;
    void parseMeasurementFromXML(const XMLElem measurementXML, 
                                 Measurement* measurement) const;
    void parseExploitationFeaturesFromXML(const XMLElem elem, 
                                          ExploitationFeatures* exFeatures) const;
    void parseAnnotationFromXML(const XMLElem annotationXML, 
                                Annotation* a) const;
    void parseSFAGeometryFromXML(const XMLElem elem, 
                                 SFAGeometry* g) const;
    void parseGeographicCoordinateSystemFromXML(
            const XMLElem coorSysElem, 
            SFAGeographicCoordinateSystem* coordSys) const;

    void parseDatum(const XMLElem datumXML, SFADatum& datum) const;

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
                              mem::ScopedCopyablePtr<DateTime>& date);

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
    void setAttributeIfNonNull(XMLElem element,
                               const std::string& name,
                               const DateTime* value,
                               const std::string& uri = "");

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
