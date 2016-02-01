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

#include <six/SICommonXMLParser01x.h>
#include <six/sidd/DerivedXMLParser100.h>
#include <six/sidd/DerivedDataBuilder.h>

namespace
{
typedef xml::lite::Element* XMLElem;
typedef xml::lite::Attributes XMLAttributes;
}

namespace six
{
namespace sidd
{
const char DerivedXMLParser100::VERSION[] = "1.0.0";
const char DerivedXMLParser100::SI_COMMON_URI[] = "urn:SICommon:0.1";
const char DerivedXMLParser100::ISM_URI[] = "urn:us:gov:ic:ism";

DerivedXMLParser100::DerivedXMLParser100(logging::Logger* log,
                                         bool ownLog) :
    DerivedXMLParser(VERSION,
                     std::auto_ptr<six::SICommonXMLParser>(
                         new six::SICommonXMLParser01x(versionToURI(VERSION),
                                                       false,
                                                       SI_COMMON_URI,
                                                       log)),
                     log,
                     ownLog)
{
}

DerivedData* DerivedXMLParser100::fromXML(const xml::lite::Document* doc) const
{
    XMLElem root = doc->getRootElement();

    XMLElem productCreationXML        = getFirstAndOnly(root, "ProductCreation");
    XMLElem displayXML                = getFirstAndOnly(root, "Display");
    XMLElem measurementXML            = getFirstAndOnly(root, "Measurement");
    XMLElem exploitationFeaturesXML   = getFirstAndOnly(root, "ExploitationFeatures");
    XMLElem geographicAndTargetXML    = getFirstAndOnly(root, "GeographicAndTarget");
    XMLElem productProcessingXML      = getOptional(root, "ProductProcessing");
    XMLElem downstreamReprocessingXML = getOptional(root, "DownstreamReprocessing");
    XMLElem errorStatisticsXML        = getOptional(root, "ErrorStatistics");
    XMLElem radiometricXML            = getOptional(root, "Radiometric");
    XMLElem annotationsXML            = getOptional(root, "Annotations");

    DerivedDataBuilder builder;
    DerivedData *data = builder.steal(); //steal it

    // see if PixelType has MONO or RGB
    PixelType pixelType = six::toType<PixelType>(
            getFirstAndOnly(displayXML, "PixelType")->getCharacterData());
    builder.addDisplay(pixelType);

    RegionType regionType = RegionType::SUB_REGION;
    XMLElem tmpElem = getFirstAndOnly(geographicAndTargetXML,
                                      "GeographicCoverage");

    // create GeographicAndTarget
    if (getOptional(tmpElem, "SubRegion"))
        regionType = RegionType::SUB_REGION;
    else if (getOptional(tmpElem, "GeographicInfo"))
        regionType = RegionType::GEOGRAPHIC_INFO;
    builder.addGeographicAndTargetOld(regionType);

    // create Measurement
    six::ProjectionType projType = ProjectionType::NOT_SET;
    if (getOptional(measurementXML, "GeographicProjection"))
        projType = ProjectionType::GEOGRAPHIC;
    else if (getOptional(measurementXML, "CylindricalProjection"))
        projType = ProjectionType::CYLINDRICAL;
    else if (getOptional(measurementXML, "PlaneProjection"))
        projType = ProjectionType::PLANE;
    else if (getOptional(measurementXML, "PolynomialProjection"))
        projType = ProjectionType::POLYNOMIAL;
    builder.addMeasurement(projType);

    // create ExploitationFeatures
    std::vector<XMLElem> elements;
    exploitationFeaturesXML->getElementsByTagName("ExploitationFeatures",
                                                  elements);
    builder.addExploitationFeatures(elements.size());

    parseProductCreationFromXML(productCreationXML, data->productCreation.get());
    parseDisplayFromXML(displayXML, data->display.get());
    parseGeographicTargetFromXML(geographicAndTargetXML, data->geographicAndTarget.get());
    parseMeasurementFromXML(measurementXML, data->measurement.get());
    parseExploitationFeaturesFromXML(exploitationFeaturesXML, data->exploitationFeatures.get());

    if (productProcessingXML)
    {
        builder.addProductProcessing();
        parseProductProcessingFromXML(productProcessingXML,
                                      data->productProcessing.get());
    }
    if (downstreamReprocessingXML)
    {
        builder.addDownstreamReprocessing();
        parseDownstreamReprocessingFromXML(downstreamReprocessingXML,
                                           data->downstreamReprocessing.get());
    }
    if (errorStatisticsXML)
    {
        builder.addErrorStatistics();
        common().parseErrorStatisticsFromXML(errorStatisticsXML,
                                             data->errorStatistics.get());
    }
    if (radiometricXML)
    {
        builder.addRadiometric();
        common().parseRadiometryFromXML(radiometricXML,
                                        data->radiometric.get());
    }
    if (annotationsXML)
    {
        // 1 to unbounded
        std::vector<XMLElem> annChildren;
        annotationsXML->getElementsByTagName("Annotation", annChildren);
        data->annotations.resize(annChildren.size());
        for (unsigned int i = 0, size = annChildren.size(); i < size; ++i)
        {
            data->annotations[i].reset(new Annotation());
            parseAnnotationFromXML(annChildren[i], data->annotations[i].get());
        }
    }

    return data;
}

void DerivedXMLParser100::parseDerivedClassificationFromXML(
        const XMLElem classificationXML,
        DerivedClassification& classification) const
{
    DerivedXMLParser::parseDerivedClassificationFromXML(classificationXML, classification);

    const XMLAttributes& classificationAttributes
        = classificationXML->getAttributes();
 
    getAttributeListIfExists(classificationAttributes,
                             "ism:compliesWith",
                             classification.compliesWith);
    // optional
    getAttributeIfExists(classificationAttributes,
                         "ism:typeOfExemptedSource",
                         classification.exemptedSourceType);
    // optional
    getAttributeIfExists(classificationAttributes,
                         "ism:dateOfExemptedSource",
                         classification.exemptedSourceDate);
}

XMLElem DerivedXMLParser100::convertDerivedClassificationToXML(
        const DerivedClassification& classification,
        XMLElem parent) const
{
    XMLElem classXML = newElement("Classification", parent);

    common().addParameters("SecurityExtension",
                           classification.securityExtensions,
                           classXML);

    //! from ism:ISMRootNodeAttributeGroup
    // SIDD 1.0 is tied to IC-ISM v4
    setAttribute(classXML, "DESVersion", "4", ISM_URI);

    //! from ism:ResourceNodeAttributeGroup
    setAttribute(classXML, "resourceElement", "true", ISM_URI);
    setAttribute(classXML, "createDate",
                 classification.createDate.format("%Y-%m-%d"), ISM_URI);
    // optional
    setAttributeList(classXML, "compliesWith", classification.compliesWith,
                     ISM_URI);

    //! from ism:SecurityAttributesGroup
    //  -- referenced in ism::ResourceNodeAttributeGroup
    setAttribute(classXML, "classification", classification.classification,
                 ISM_URI);
    setAttributeList(classXML, "ownerProducer", classification.ownerProducer,
                     ISM_URI, true);
    // optional
    setAttributeList(classXML, "SCIcontrols", classification.sciControls,
                     ISM_URI);
    // optional
    setAttributeList(classXML, "SARIdentifier", classification.sarIdentifier,
                     ISM_URI);
    // optional
    setAttributeList(classXML,
                     "disseminationControls",
                     classification.disseminationControls,
                     ISM_URI);
    // optional
    setAttributeList(classXML, "FGIsourceOpen", classification.fgiSourceOpen,
                     ISM_URI);
    // optional
    setAttributeList(classXML,
                     "FGIsourceProtected",
                     classification.fgiSourceProtected,
                     ISM_URI);
    // optional
    setAttributeList(classXML, "releasableTo", classification.releasableTo,
                     ISM_URI);
    // optional
    setAttributeList(classXML, "nonICmarkings", classification.nonICMarkings,
                     ISM_URI);
    // optional
    setAttributeIfNonEmpty(classXML,
                           "classifiedBy",
                           classification.classifiedBy,
                           ISM_URI);
    // optional
    setAttributeIfNonEmpty(classXML,
                           "compilationReason",
                           classification.compilationReason,
                           ISM_URI);
    // optional
    setAttributeIfNonEmpty(classXML,
                           "derivativelyClassifiedBy",
                           classification.derivativelyClassifiedBy,
                           ISM_URI);
    // optional
    setAttributeIfNonEmpty(classXML,
                           "classificationReason",
                           classification.classificationReason,
                           ISM_URI);
    // optional
    setAttributeList(classXML, "nonUSControls", classification.nonUSControls,
                     ISM_URI);
    // optional
    setAttributeIfNonEmpty(classXML,
                           "derivedFrom",
                           classification.derivedFrom,
                           ISM_URI);
    // optional
    if (classification.declassDate.get())
    {
        setAttributeIfNonEmpty(
                classXML, "declassDate",
                classification.declassDate->format("%Y-%m-%d"),
                ISM_URI);
    }
    // optional
    setAttributeIfNonEmpty(classXML,
                           "declassEvent",
                           classification.declassEvent,
                           ISM_URI);
    // optional
    setAttributeIfNonEmpty(classXML,
                           "declassException",
                           classification.declassException,
                           ISM_URI);
    // optional
    setAttributeIfNonEmpty(classXML,
                           "typeOfExemptedSource",
                           classification.exemptedSourceType,
                           ISM_URI);
    // optional
    if (classification.exemptedSourceDate.get())
    {
        setAttributeIfNonEmpty(
                classXML, "dateOfExemptedSource",
                classification.exemptedSourceDate->format("%Y-%m-%d"),
                ISM_URI);
    }

    return classXML;
}

xml::lite::Document*
DerivedXMLParser100::toXML(const DerivedData* derived) const
{
    xml::lite::Document* doc = new xml::lite::Document();
    XMLElem root = newElement("SIDD");
    doc->setRootElement(root);

    convertProductCreationToXML(derived->productCreation.get(), root);
    convertDisplayToXML(*derived->display, root);
    convertGeographicTargetToXML(*derived->geographicAndTarget, root);
    convertMeasurementToXML(derived->measurement.get(), root);
    convertExploitationFeaturesToXML(derived->exploitationFeatures.get(),
                                     root);

    // optional
    if (derived->productProcessing.get())
    {
        convertProductProcessingToXML(derived->productProcessing.get(), root);
    }
    // optional
    if (derived->downstreamReprocessing.get())
    {
        convertDownstreamReprocessingToXML(
                derived->downstreamReprocessing.get(), root);
    }
    // optional
    if (derived->errorStatistics.get())
    {
        common().convertErrorStatisticsToXML(derived->errorStatistics.get(),
                                             root);
    }
    // optional
    if (derived->radiometric.get())
    {
        common().convertRadiometryToXML(derived->radiometric.get(), root);
    }
    // optional
    if (!derived->annotations.empty())
    {
        XMLElem annotationsElem = newElement("Annotations", root);
        for (size_t i = 0, num = derived->annotations.size(); i < num; ++i)
        {
            convertAnnotationToXML(derived->annotations[i].get(),
                                   annotationsElem);
        }
    }

    //set the XMLNS
    root->setNamespacePrefix("", getDefaultURI());
    root->setNamespacePrefix("si", SI_COMMON_URI);
    root->setNamespacePrefix("sfa", SFA_URI);
    root->setNamespacePrefix("ism", ISM_URI);

    return doc;
}

XMLElem DerivedXMLParser100::convertDisplayToXML(
        const Display& display,
        XMLElem parent) const
{
    XMLElem displayXML = newElement("Display", parent);

    createString("PixelType", six::toString(display.pixelType), displayXML);

    // optional
    if (display.remapInformation.get())
    {
        XMLElem remapInfoXML = newElement("RemapInformation", displayXML);
        convertRemapToXML(*display.remapInformation, remapInfoXML);
    }

    // optional
    if (display.magnificationMethod != MagnificationMethod::NOT_SET)
    {
        createString("MagnificationMethod",
                     six::toString(display.magnificationMethod), displayXML);
    }

    // optional
    if (display.decimationMethod != DecimationMethod::NOT_SET)
    {
        createString("DecimationMethod",
                     six::toString(display.decimationMethod), displayXML);
    }

    // optional
    if (display.histogramOverrides.get())
    {
        XMLElem histo = newElement("DRAHistogramOverrides", displayXML);
        createInt("ClipMin", display.histogramOverrides->clipMin, histo);
        createInt("ClipMax", display.histogramOverrides->clipMax, histo);
    }

    // optional
    if (display.monitorCompensationApplied.get())
    {
        XMLElem monComp = newElement("MonitorCompensationApplied", displayXML);
        createDouble("Gamma", display.monitorCompensationApplied->gamma,
                     monComp);
        createDouble("XMin", display.monitorCompensationApplied->xMin, monComp);
    }

    // optional to unbounded
    common().addParameters("DisplayExtension", display.displayExtensions, displayXML);

    return displayXML;
}

XMLElem DerivedXMLParser100::convertGeographicTargetToXML(
        const GeographicAndTarget& geographicAndTarget,
        XMLElem parent) const
{
    XMLElem geographicAndTargetXML = newElement("GeographicAndTarget", parent);

    if (geographicAndTarget.geographicCoverage.get() == NULL)
    {
        throw except::Exception(Ctxt("geographicCoverage is required"));
    }

    convertGeographicCoverageToXML(
            "GeographicCoverage",
            geographicAndTarget.geographicCoverage.get(),
            geographicAndTargetXML);

    // optional to unbounded
    for (std::vector<mem::ScopedCopyablePtr<TargetInformation> >::
            const_iterator it = geographicAndTarget.targetInformation.begin();
            it != geographicAndTarget.targetInformation.end(); ++it)
    {
        TargetInformation* ti = (*it).get();
        XMLElem tiXML = newElement("TargetInformation", geographicAndTargetXML);

        // 1 to unbounded
        common().addParameters("Identifier", ti->identifiers, tiXML);

        // optional
        if (ti->footprint.get())
        {
            createFootprint("Footprint", "Vertex", *ti->footprint, tiXML);
        }

        // optional to unbounded
        common().addParameters("TargetInformationExtension",
                               ti->targetInformationExtensions, tiXML);
    }

    return geographicAndTargetXML;
}

void DerivedXMLParser100::parseGeographicTargetFromXML(
        const XMLElem geographicAndTargetXML,
        GeographicAndTarget* geographicAndTarget) const
{
    parseGeographicCoverageFromXML(
            getFirstAndOnly(geographicAndTargetXML, "GeographicCoverage"),
            geographicAndTarget->geographicCoverage.get());

    // optional to unbounded
    std::vector<XMLElem> targetInfosXML;
    geographicAndTargetXML->getElementsByTagName("TargetInformation",
                                                 targetInfosXML);
    geographicAndTarget->targetInformation.resize(targetInfosXML.size());
    for (size_t i = 0; i < targetInfosXML.size(); ++i)
    {
        geographicAndTarget->targetInformation[i].reset(
                new TargetInformation());

        TargetInformation* ti
                = geographicAndTarget->targetInformation[i].get();

        // unbounded
        common().parseParameters(targetInfosXML[i], "Identifier",
                                 ti->identifiers);

        // optional
        XMLElem tmpXML = getOptional(targetInfosXML[i], "Footprint");
        if (tmpXML)
        {
            ti->footprint.reset(new six::LatLonCorners());
            common().parseFootprint(tmpXML, "Vertex", *ti->footprint);
        }

        // optional
        common().parseParameters(targetInfosXML[i],
                                 "TargetInformationExtension",
                                 ti->targetInformationExtensions);
    }
}

void DerivedXMLParser100::parseGeographicCoverageFromXML(
        const XMLElem geographicCoverageXML,
        GeographicCoverage* geographicCoverage) const
{
    // optional and unbounded
    common().parseParameters(geographicCoverageXML, "GeoregionIdentifier",
                             geographicCoverage->georegionIdentifiers);

    common().parseFootprint(getFirstAndOnly(geographicCoverageXML, "Footprint"),
                            "Vertex", geographicCoverage->footprint);

    // If there are subregions, recurse
    std::vector<XMLElem> subRegionsXML;
    geographicCoverageXML->getElementsByTagName("SubRegion", subRegionsXML);

    geographicCoverage->subRegion.resize(subRegionsXML.size());
    for (size_t i = 0; i < subRegionsXML.size(); ++i)
    {
        geographicCoverage->subRegion[i].reset(
                new GeographicCoverage(RegionType::SUB_REGION));
        parseGeographicCoverageFromXML(
                subRegionsXML[i], geographicCoverage->subRegion[i].get());
    }

    // Otherwise read the GeographicInfo
    if (subRegionsXML.size() == 0)
    {
        XMLElem geographicInfoXML = getFirstAndOnly(geographicCoverageXML,
                                                    "GeographicInfo");

        geographicCoverage->geographicInformation.reset(
            new GeographicInformation());

        // optional to unbounded
        std::vector<XMLElem> countryCodes;
        geographicInfoXML->getElementsByTagName("CountryCode", countryCodes);
        for (std::vector<XMLElem>::const_iterator it = countryCodes.begin(); it
                != countryCodes.end(); ++it)
        {
            geographicCoverage->geographicInformation->
                    countryCodes.push_back((*it)->getCharacterData());
        }

        // optional
        XMLElem securityInformationXML = getOptional(geographicInfoXML,
                                                     "SecurityInfo");
        if (securityInformationXML)
        {
            parseString(securityInformationXML, geographicCoverage->
                    geographicInformation->securityInformation);
        }

        // optional to unbounded
        common().parseParameters(geographicInfoXML, "GeographicInfoExtension",
                geographicCoverage->geographicInformation->
                        geographicInformationExtensions);
    }
}

XMLElem DerivedXMLParser100::convertMeasurementToXML(
    const Measurement* measurement,
    XMLElem parent) const
{
    XMLElem measurementXML = DerivedXMLParser::convertMeasurementToXML(measurement, parent);

    common().createPolyXYZ("ARPPoly",
        measurement->arpPoly,
        measurementXML);

    return measurementXML;
}

void DerivedXMLParser100::parseMeasurementFromXML(
    const XMLElem measurementXML,
    Measurement* measurement) const
{
    DerivedXMLParser::parseMeasurementFromXML(measurementXML, measurement);

    common().parsePolyXYZ(getFirstAndOnly(measurementXML, "ARPPoly"),
        measurement->arpPoly);
}
}
}
