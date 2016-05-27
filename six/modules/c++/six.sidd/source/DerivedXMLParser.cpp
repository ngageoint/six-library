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

#include <string.h>

#include <sys/Conf.h>
#include <str/Manip.h>
#include <except/Exception.h>
#include <six/sidd/DerivedXMLParser.h>
#include <six/sidd/DerivedDataBuilder.h>
#include <six/SICommonXMLParser01x.h>

namespace
{
typedef xml::lite::Element* XMLElem;
typedef xml::lite::Attributes XMLAttributes;
}

namespace six
{
namespace sidd
{
const char DerivedXMLParser::SI_COMMON_URI[] = "urn:SICommon:0.1";
const char DerivedXMLParser::SFA_URI[] = "urn:SFA:1.2.0";
const char DerivedXMLParser::ISM_URI[] = "urn:us:gov:ic:ism";

DerivedXMLParser::DerivedXMLParser(const std::string& version,
                                   logging::Logger* log,
                                   bool ownLog) :
    XMLParser(versionToURI(version), false, log, ownLog),
    mCommon(new six::SICommonXMLParser01x(
                versionToURI(version), false,
                SI_COMMON_URI, log))
{
}

DerivedData* DerivedXMLParser::fromXML(
        const xml::lite::Document* doc) const
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
    builder.addGeographicAndTarget(regionType);

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
        for (size_t i = 0, size = annChildren.size(); i < size; ++i)
        {
            data->annotations[i].reset(new Annotation());
            parseAnnotationFromXML(annChildren[i], data->annotations[i].get());
        }
    }

    return data;
}

xml::lite::Document* DerivedXMLParser::toXML(const DerivedData* derived) const
{
    xml::lite::Document* doc = new xml::lite::Document();
    XMLElem root = newElement("SIDD");
    doc->setRootElement(root);

    convertProductCreationToXML(derived->productCreation.get(), root);
    convertDisplayToXML(derived->display.get(), root);
    convertGeographicTargetToXML(derived->geographicAndTarget.get(), root);
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

void DerivedXMLParser::getAttributeList(
        const xml::lite::Attributes& attributes,
        const std::string& attributeName,
        std::vector<std::string>& values)
{
    values = str::split(attributes.getValue(attributeName));
}

void DerivedXMLParser::getAttributeListIfExists(
        const xml::lite::Attributes& attributes,
        const std::string& attributeName,
        std::vector<std::string>& values)
{
    if (attributes.contains(attributeName))
    {
        values = str::split(attributes.getValue(attributeName));
    }
    else
    {
        values.clear();
    }
}

void DerivedXMLParser::getAttributeIfExists(
        const xml::lite::Attributes& attributes,
        const std::string& attributeName,
        std::string& value)
{
    if (attributes.contains(attributeName))
    {
        value = attributes.getValue(attributeName);
    }
    else
    {
        value.clear();
    }
}

void DerivedXMLParser::getAttributeIfExists(
        const xml::lite::Attributes& attributes,
        const std::string& attributeName,
        mem::ScopedCopyablePtr<DateTime>& date)
{
    if (attributes.contains(attributeName))
    {
        date.reset(new DateTime(toType<DateTime>(
            attributes.getValue(attributeName))));
    }
    else
    {
        date.reset();
    }
}

void DerivedXMLParser::setAttributeList(
        XMLElem element,
        const std::string& attributeName,
        const std::vector<std::string>& values,
        const std::string& uri,
        bool setIfEmpty)
{
    std::string value;
    for (size_t ii = 0; ii < values.size(); ++ii)
    {
        std::string thisValue(values[ii]);
        str::trim(thisValue);
        if (!thisValue.empty())
        {
            if (!value.empty())
            {
                value += " ";
            }

            value += thisValue;
        }
    }

    if (!value.empty() || setIfEmpty)
    {
        setAttribute(element, attributeName, value, uri);
    }
}

void DerivedXMLParser::setAttributeIfNonEmpty(XMLElem element,
                                              const std::string& name,
                                              const std::string& value,
                                              const std::string& uri)
{
    if (!value.empty())
    {
        setAttribute(element, name, value, uri);
    }
}

void DerivedXMLParser::setAttributeIfNonNull(XMLElem element,
                                             const std::string& name,
                                             const DateTime* value,
                                             const std::string& uri)
{
    if (value)
    {
        setAttribute(element, name, toString(*value), uri);
    }
}

void DerivedXMLParser::parseProductCreationFromXML(
        const XMLElem informationXML,
        ProcessorInformation* processorInformation) const
{
    parseString(getFirstAndOnly(informationXML, "Application"),
                processorInformation->application);
    parseDateTime(getFirstAndOnly(informationXML, "ProcessingDateTime"),
                  processorInformation->processingDateTime);
    parseString(getFirstAndOnly(informationXML, "Site"),
                processorInformation->site);

    // optional
    XMLElem profileXML = getOptional(informationXML, "Profile");
    if (profileXML)
    {
        parseString(profileXML, processorInformation->profile);
    }
}

void DerivedXMLParser::parseProductCreationFromXML(
        const XMLElem productCreationXML,
        ProductCreation* productCreation) const
{
    parseProductCreationFromXML(
            getFirstAndOnly(productCreationXML, "ProcessorInformation"),
            productCreation->processorInformation.get());

    parseDerivedClassificationFromXML(
            getFirstAndOnly(productCreationXML, "Classification"),
            productCreation->classification);

    parseString(getFirstAndOnly(productCreationXML, "ProductName"),
                productCreation->productName);
    parseString(getFirstAndOnly(productCreationXML, "ProductClass"),
                productCreation->productClass);

    // optional
    XMLElem productTypeXML = getOptional(productCreationXML, "ProductType");
    if (productTypeXML)
    {
        parseString(productTypeXML, productCreation->productType);
    }

    // optional
    common().parseParameters(productCreationXML, "ProductCreationExtension",
                             productCreation->productCreationExtensions);
}

void DerivedXMLParser::parseDerivedClassificationFromXML(
        const XMLElem classificationXML,
        DerivedClassification& classification) const
{
    // optional to unbounded
    common().parseParameters(classificationXML, "SecurityExtension",
                             classification.securityExtensions);

    const XMLAttributes& classificationAttributes
            = classificationXML->getAttributes();

    //! from ism:ISMRootNodeAttributeGroup
    classification.desVersion = toType<sys::Int32_T>(
            classificationAttributes.getValue("ism:DESVersion"));

    //! from ism:ResourceNodeAttributeGroup
    // NOTE: "resouceElement" is fixed to true so it isn't saved here
    classification.createDate = toType<DateTime>(
            classificationAttributes.getValue("ism:createDate"));
    // optional
    getAttributeListIfExists(classificationAttributes,
                             "ism:compliesWith",
                             classification.compliesWith);

    //! from ism:SecurityAttributesGroup
    //  -- referenced in ism::ResourceNodeAttributeGroup
    classification.classification
            = classificationAttributes.getValue("ism:classification");
    getAttributeList(classificationAttributes,
                     "ism:ownerProducer",
                     classification.ownerProducer);
    // optional
    getAttributeListIfExists(classificationAttributes,
                             "ism:SCIcontrols",
                             classification.sciControls);
    // optional
    getAttributeListIfExists(classificationAttributes,
                             "ism:SARIdentifier",
                             classification.sarIdentifier);
    // optional
    getAttributeListIfExists(classificationAttributes,
                             "ism:disseminationControls",
                             classification.disseminationControls);
    // optional
    getAttributeListIfExists(classificationAttributes,
            "ism:FGIsourceOpen", classification.fgiSourceOpen);
    // optional
    getAttributeListIfExists(classificationAttributes,
                             "ism:FGIsourceProtected",
                             classification.fgiSourceProtected);
    // optional
    getAttributeListIfExists(classificationAttributes,
                             "ism:releasableTo",
                             classification.releasableTo);
    // optional
    getAttributeListIfExists(classificationAttributes,
                             "ism:nonICmarkings",
                             classification.nonICMarkings);
    // optional
    getAttributeIfExists(classificationAttributes,
                         "ism:classifiedBy",
                         classification.classifiedBy);
    // optional
    getAttributeIfExists(classificationAttributes,
                         "ism:compilationReason",
                         classification.compilationReason);
    // optional
    getAttributeIfExists(classificationAttributes,
                         "ism:derivativelyClassifiedBy",
                         classification.derivativelyClassifiedBy);
    // optional
    getAttributeIfExists(classificationAttributes,
                         "ism:classificationReason",
                         classification.classificationReason);
    // optional
    getAttributeListIfExists(classificationAttributes,
                             "ism:nonUSControls",
                             classification.nonUSControls);
    // optional
    getAttributeIfExists(classificationAttributes,
                         "ism:derivedFrom",
                         classification.derivedFrom);
    // optional
    getAttributeIfExists(classificationAttributes,
                         "ism:declassDate",
                         classification.declassDate);
    // optional
    getAttributeIfExists(classificationAttributes,
                         "ism:declassEvent",
                         classification.declassEvent);
    // optional
    getAttributeIfExists(classificationAttributes,
                         "ism:declassException",
                         classification.declassException);
    // optional
    getAttributeIfExists(classificationAttributes,
                         "ism:typeOfExemptedSource",
                         classification.exemptedSourceType);
    // optional
    getAttributeIfExists(classificationAttributes,
                         "ism:dateOfExemptedSource",
                         classification.exemptedSourceDate);
}

Remap* DerivedXMLParser::parseRemapChoiceFromXML(
        const XMLElem remapInformationXML) const
{
    if (remapInformationXML)
    {
        XMLElem colorRemapXML =
                getOptional(remapInformationXML, "ColorDisplayRemap");
        XMLElem monoRemapXML =
                getOptional(remapInformationXML, "MonochromeDisplayRemap");

        if (colorRemapXML && !monoRemapXML)
        {
            XMLElem remapLUTXML = getFirstAndOnly(colorRemapXML, "RemapLUT");

            //get size attribute
            int size = str::toType<int>(remapLUTXML->attribute("size"));

            // xs:list is space delimited
            std::string lutStr = remapLUTXML->getCharacterData();
            std::vector<std::string> lutVals = str::split(lutStr, " ");
            std::auto_ptr<LUT> remapLUT(new LUT(size, 3));

            // TripleType is comma delimited
            size_t k = 0;
            for (size_t i = 0; i < lutVals.size(); i++)
            {
                std::vector<std::string> rgb = str::split(lutVals[i], ",");
                for (size_t j = 0; j < rgb.size(); j++)
                {
                    size_t intermediateVal = str::toType<size_t>(rgb[j]);
                    if (intermediateVal > 255)
                    {
                        throw except::Exception(Ctxt(
                            "LUT vals expected to be in [0, 255]."));
                    }

                    sys::ubyte val = static_cast<sys::ubyte>(
                            intermediateVal);

                    ::memcpy(&(remapLUT->getTable()[k++]), &val,
                             sizeof(sys::ubyte));
                }
            }
            return new ColorDisplayRemap(remapLUT.release());
        }
        else if (!colorRemapXML && monoRemapXML)
        {
            std::string remapType = "";
            parseString(getFirstAndOnly(monoRemapXML, "RemapType"), remapType);

            std::auto_ptr<LUT> remapLUT;
            XMLElem remapLUTXML = getOptional(monoRemapXML, "RemapLUT");
            if (remapLUTXML)
            {
                //get size attribute
                int size = str::toType<int>(remapLUTXML->attribute("size"));

                std::string lutStr = "";
                parseString(remapLUTXML, lutStr);
                std::vector<std::string> lutVals = str::split(lutStr, " ");
                remapLUT.reset(new LUT(size, sizeof(short)));

                for (size_t ii = 0; ii < lutVals.size(); ++ii)
                {
                    const short lutVal = str::toType<short>(lutVals[ii]);
                    ::memcpy(&(remapLUT->getTable()[ii * remapLUT->elementSize]),
                             &lutVal, sizeof(short));
                }
            }

            std::auto_ptr<MonochromeDisplayRemap> monoRemap(
                    new MonochromeDisplayRemap(remapType, remapLUT.release()));

            // optional
            common().parseParameters(monoRemapXML, "RemapParameter",
                                     monoRemap->remapParameters);

            return monoRemap.release();
        }
        else if (!colorRemapXML && !monoRemapXML)
        {
            return NULL;
        }
        else
        {
            throw except::Exception(Ctxt(
                    "Cannot contain both ColorDisplayRemap and " \
                    "MonochromeDisplayRemap"));
        }
    }
    else
    {
        return NULL;
    }
}

void DerivedXMLParser::parseDisplayFromXML(
        const XMLElem displayXML,
        Display* display) const
{
    display->pixelType
            = six::toType<PixelType>(getFirstAndOnly(
                    displayXML, "PixelType")->getCharacterData());

    // optional
    display->remapInformation.reset(parseRemapChoiceFromXML(
            getOptional(displayXML, "RemapInformation")));

    //optional
    XMLElem magXML = getOptional(displayXML, "MagnificationMethod");
    display->magnificationMethod = magXML ?
            six::toType<MagnificationMethod>(magXML->getCharacterData())
            : MagnificationMethod::NOT_SET;

    //optional
    XMLElem decXML = getOptional(displayXML, "DecimationMethod");
    display->decimationMethod = decXML ?
            six::toType<DecimationMethod>(decXML->getCharacterData())
            : DecimationMethod::NOT_SET;

    // optional
    XMLElem histogramOverridesXML = getOptional(displayXML,
                                                "DRAHistogramOverrides");
    if (histogramOverridesXML)
    {
        display->histogramOverrides.reset(new DRAHistogramOverrides());
        parseInt(getFirstAndOnly(histogramOverridesXML, "ClipMin"),
                 display->histogramOverrides->clipMin);
        parseInt(getFirstAndOnly(histogramOverridesXML, "ClipMax"),
                 display->histogramOverrides->clipMax);
    }

    // optional
    XMLElem monitorCompensationXML = getOptional(displayXML,
                                                 "MonitorCompensationApplied");
    if (monitorCompensationXML)
    {
        display->monitorCompensationApplied.reset(
                new MonitorCompensationApplied());
        parseDouble(getFirstAndOnly(monitorCompensationXML, "Gamma"),
                    display->monitorCompensationApplied->gamma);
        parseDouble(getFirstAndOnly(monitorCompensationXML, "XMin"),
                    display->monitorCompensationApplied->xMin);
    }

    // optional
    common().parseParameters(displayXML, "DisplayExtension",
                             display->displayExtensions);
}

void DerivedXMLParser::parseGeographicTargetFromXML(
        const XMLElem geographicAndTargetXML,
        GeographicAndTarget* geographicAndTarget) const
{
    parseGeographicCoverageFromXML(
            getFirstAndOnly(geographicAndTargetXML, "GeographicCoverage"),
            &geographicAndTarget->geographicCoverage);

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

void DerivedXMLParser::parseGeographicCoverageFromXML(
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

// This function ASSUMES that the measurement projection has already been set!
void DerivedXMLParser::parseMeasurementFromXML(
        const XMLElem measurementXML,
        Measurement* measurement) const
{
    //  choice: ProjectionType --
    //  this is first parsed in fromXML()
    XMLElem projXML = NULL;
    if (measurement->projection->projectionType == ProjectionType::POLYNOMIAL)
    {
        projXML = getFirstAndOnly(measurementXML, "PolynomialProjection");

        PolynomialProjection* polyProj
                = (PolynomialProjection*) measurement->projection.get();

        // Get a bunch of 2D polynomials
        common().parsePoly2D(getFirstAndOnly(projXML, "RowColToLat"),
                             polyProj->rowColToLat);
        common().parsePoly2D(getFirstAndOnly(projXML, "RowColToLon"),
                             polyProj->rowColToLon);

        XMLElem optionalAltPolyXML = getOptional(projXML, "RowColToAlt");
        if (optionalAltPolyXML)
        {
            common().parsePoly2D(optionalAltPolyXML, polyProj->rowColToAlt);
        }

        common().parsePoly2D(getFirstAndOnly(projXML, "LatLonToRow"),
                             polyProj->latLonToRow);
        common().parsePoly2D(getFirstAndOnly(projXML, "LatLonToCol"),
                             polyProj->latLonToCol);
    }
    else if (measurement->projection->projectionType
            == ProjectionType::GEOGRAPHIC)
    {
        projXML = getFirstAndOnly(measurementXML, "GeographicProjection");

        GeographicProjection* geographicProj
                = (GeographicProjection*) measurement->projection.get();

        // measureableProjectionType
        common().parseRowColDouble(getFirstAndOnly(projXML, "SampleSpacing"),
                                   geographicProj->sampleSpacing);
        common().parsePoly2D(getFirstAndOnly(projXML, "TimeCOAPoly"),
                             geographicProj->timeCOAPoly);
    }
    else if (measurement->projection->projectionType == ProjectionType::PLANE)
    {
        projXML = getFirstAndOnly(measurementXML, "PlaneProjection");

        PlaneProjection* planeProj
                = (PlaneProjection*) measurement->projection.get();

        // measureableProjectionType
        common().parseRowColDouble(getFirstAndOnly(projXML, "SampleSpacing"),
                                   planeProj->sampleSpacing);
        common().parsePoly2D(getFirstAndOnly(projXML, "TimeCOAPoly"),
                             planeProj->timeCOAPoly);

        // productPlaneType
        XMLElem prodPlaneXML = getFirstAndOnly(projXML, "ProductPlane");
        common().parseVector3D(getFirstAndOnly(prodPlaneXML, "RowUnitVector"),
                               planeProj->productPlane.rowUnitVector);
        common().parseVector3D(getFirstAndOnly(prodPlaneXML, "ColUnitVector"),
                               planeProj->productPlane.colUnitVector);
    }
    else if (measurement->projection->projectionType
            == ProjectionType::CYLINDRICAL)
    {
        projXML = getFirstAndOnly(measurementXML, "CylindricalProjection");

        CylindricalProjection* cylindricalProj
                = (CylindricalProjection*) measurement->projection.get();

        // measureableProjectionType
        common().parseRowColDouble(getFirstAndOnly(projXML, "SampleSpacing"),
                                   cylindricalProj->sampleSpacing);
        common().parsePoly2D(getFirstAndOnly(projXML, "TimeCOAPoly"),
                             cylindricalProj->timeCOAPoly);

        common().parseVector3D(getFirstAndOnly(projXML, "StripmapDirection"),
                               cylindricalProj->stripmapDirection);
        // optional
        XMLElem curvRadiusXML = getOptional(projXML, "CurvatureRadius");
        if (curvRadiusXML)
        {
            parseDouble(curvRadiusXML, cylindricalProj->curvatureRadius);
        }
    }
    else
        throw except::Exception(Ctxt("Unknown projection type"));

    //  referencePointType --
    //  this is present in all projections types
    XMLElem refXML = getFirstAndOnly(projXML, "ReferencePoint");
    getAttributeIfExists(refXML->getAttributes(), "name",
                         measurement->projection->referencePoint.name);
    common().parseVector3D(getFirstAndOnly(refXML, "ECEF"),
                           measurement->projection->referencePoint.ecef);
    common().parseRowColDouble(getFirstAndOnly(refXML, "Point"),
                               measurement->projection->referencePoint.rowCol);


    common().parseRowColInt(getFirstAndOnly(measurementXML, "PixelFootprint"),
                            measurement->pixelFootprint);

    common().parsePolyXYZ(getFirstAndOnly(measurementXML, "ARPPoly"),
                          measurement->arpPoly);
}

void DerivedXMLParser::parseExploitationFeaturesFromXML(
        const XMLElem exploitationFeaturesXML,
        ExploitationFeatures* exploitationFeatures) const
{
    XMLElem tmpElem;

    std::vector<XMLElem> collectionsXML;
    exploitationFeaturesXML->getElementsByTagName("Collection", collectionsXML);

    exploitationFeatures->collections.resize(collectionsXML.size());
    for (size_t i = 0; i < collectionsXML.size(); ++i)
    {
        XMLElem collectionXML = collectionsXML[i];

        // At least one was created at construction, so check for NULL
        if (!exploitationFeatures->collections[i].get())
            exploitationFeatures->collections[i].reset(new Collection());
        Collection* coll = exploitationFeatures->collections[i].get();

        coll->identifier
                = collectionXML->getAttributes().getValue("identifier");

        // parse Information
        Information* info = coll->information.get();
        XMLElem informationXML = getFirstAndOnly(collectionXML, "Information");

        parseString(getFirstAndOnly(informationXML, "SensorName"),
                    info->sensorName);

        XMLElem radarModeXML = getFirstAndOnly(informationXML, "RadarMode");
        info->radarMode = six::toType<RadarModeType>(
                getFirstAndOnly(radarModeXML, "ModeType")->getCharacterData());
        tmpElem = getOptional(radarModeXML, "ModeID");
        if (tmpElem)
        {
            parseString(tmpElem, info->radarModeID);
        }

        parseDateTime(getFirstAndOnly(informationXML, "CollectionDateTime"),
                      info->collectionDateTime);

        // optional
        tmpElem = getOptional(informationXML, "LocalDateTime");
        if (tmpElem)
            parseString(tmpElem, info->localDateTime);

        parseDouble(getFirstAndOnly(informationXML, "CollectionDuration"),
                    info->collectionDuration);

        // optional
        XMLElem resXML = getOptional(informationXML, "Resolution");
        if (resXML)
            common().parseRangeAzimuth(resXML, info->resolution);

        // optional
        XMLElem roiXML = getOptional(informationXML, "InputROI");
        if (roiXML)
        {
            info->inputROI.reset(new InputROI());

            common().parseRowColInt(getFirstAndOnly(roiXML, "Size"),
                                    info->inputROI->size);
            common().parseRowColInt(getFirstAndOnly(roiXML, "UpperLeft"),
                                    info->inputROI->upperLeft);
        }

        // optional and unbounded
        std::vector<XMLElem> polarization;
        informationXML->getElementsByTagName("Polarization", polarization);
        info->polarization.resize(polarization.size());
        for (size_t jj = 0, nElems = polarization.size(); jj < nElems; ++jj)
        {
            XMLElem polXML = polarization[jj];
            info->polarization[jj].reset(new TxRcvPolarization());
            TxRcvPolarization* p = info->polarization[jj].get();

            p->txPolarization = six::toType<PolarizationType>(
                    getFirstAndOnly(polXML, "TxPolarization")->
                            getCharacterData());
            p->rcvPolarization = six::toType<PolarizationType>(
                    getFirstAndOnly(polXML, "RcvPolarization")->
                            getCharacterData());

            // optional
            tmpElem = getOptional(polXML, "RcvPolarizationOffset");
            if (tmpElem)
                parseDouble(tmpElem, p->rcvPolarizationOffset);

            // optional
            tmpElem = getOptional(polXML, "Processed");
            if (tmpElem)
            {
                parseBooleanType(tmpElem, p->processed);
            }
        }


        // parse Geometry -- optional
        XMLElem geometryXML = getOptional(collectionXML, "Geometry");
        if (geometryXML)
        {
            coll->geometry.reset(new Geometry());

            // optional
            tmpElem = getOptional(geometryXML, "Azimuth");
            if (tmpElem)
                parseDouble(tmpElem, coll->geometry->azimuth);

            // optional
            tmpElem = getOptional(geometryXML, "Slope");
            if (tmpElem)
                parseDouble(tmpElem, coll->geometry->slope);

            // optional
            tmpElem = getOptional(geometryXML, "Squint");
            if (tmpElem)
                parseDouble(tmpElem, coll->geometry->squint);

            // optional
            tmpElem = getOptional(geometryXML, "Graze");
            if (tmpElem)
                parseDouble(tmpElem, coll->geometry->graze);

            // optional
            tmpElem = getOptional(geometryXML, "Tilt");
            if (tmpElem)
                parseDouble(tmpElem, coll->geometry->tilt);

            // optional to unbounded
            common().parseParameters(geometryXML, "Extension",
                                     coll->geometry->extensions);
        }

        // parse Phenomenology -- optional
        XMLElem phenomenologyXML = getOptional(collectionXML, "Phenomenology");
        if (phenomenologyXML)
        {
            coll->phenomenology.reset(new Phenomenology());

            // optional
            tmpElem = getOptional(phenomenologyXML, "Shadow");
            if (tmpElem)
            {
                parseDouble(getFirstAndOnly(tmpElem, "Angle"),
                            coll->phenomenology->shadow.angle);
                parseDouble(getFirstAndOnly(tmpElem, "Magnitude"),
                            coll->phenomenology->shadow.magnitude);
            }

            // optional
            tmpElem = getOptional(phenomenologyXML, "Layover");
            if (tmpElem)
            {
                parseDouble(getFirstAndOnly(tmpElem, "Angle"),
                            coll->phenomenology->layover.angle);
                parseDouble(getFirstAndOnly(tmpElem, "Magnitude"),
                            coll->phenomenology->layover.magnitude);
            }

            // optional
            tmpElem = getOptional(phenomenologyXML, "MultiPath");
            if (tmpElem)
            {
                parseDouble(tmpElem, coll->phenomenology->multiPath);
            }

            // optional
            tmpElem = getOptional(phenomenologyXML, "GroundTrack");
            if (tmpElem)
            {
                parseDouble(tmpElem, coll->phenomenology->groundTrack);
            }

            // optional to unbounded
            common().parseParameters(phenomenologyXML, "Extension",
                                     coll->phenomenology->extensions);
        }
    }

    XMLElem productXML = getFirstAndOnly(exploitationFeaturesXML, "Product");

    common().parseRowColDouble(getFirstAndOnly(productXML, "Resolution"),
                               exploitationFeatures->product.resolution);
    // optional
    tmpElem = getOptional(productXML, "North");
    if (tmpElem)
    {
        parseDouble(tmpElem, exploitationFeatures->product.north);
    }

    // optional to unbounded
    common().parseParameters(productXML, "Extension",
                             exploitationFeatures->product.extensions);
}

XMLElem DerivedXMLParser::convertProcessorInformationToXML(
        const ProcessorInformation* processorInformation,
        XMLElem parent) const
{
    XMLElem procInfoXML
            = newElement("ProcessorInformation", parent);

    createString("Application",
                 processorInformation->application,
                 procInfoXML);

    createDateTime("ProcessingDateTime",
                   processorInformation->processingDateTime,
                   procInfoXML);

    createString("Site", processorInformation->site,
                 procInfoXML);

    // optional
    if (processorInformation->profile != Init::undefined<
            std::string>())
    {
        createString("Profile", processorInformation->profile,
                     procInfoXML);
    }

    return procInfoXML;
}

XMLElem DerivedXMLParser::convertDerivedClassificationToXML(
        const DerivedClassification& classification,
        XMLElem parent) const
{
    XMLElem classXML = newElement("Classification", parent);

    common().addParameters("SecurityExtension",
                           classification.securityExtensions,
                           classXML);

    //! from ism:ISMRootNodeAttributeGroup
    setAttribute(classXML, "DESVersion", toString(classification.desVersion),
                 ISM_URI);

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

XMLElem DerivedXMLParser::convertProductCreationToXML(
        const ProductCreation* productCreation,
        XMLElem parent) const
{
    // create ProductCreation -- root
    XMLElem productCreationXML = newElement("ProductCreation", parent);

    convertProcessorInformationToXML(
            productCreation->processorInformation.get(), productCreationXML);

    convertDerivedClassificationToXML(
            productCreation->classification, productCreationXML);

    createString("ProductName", productCreation->productName,
                 productCreationXML);

    createString("ProductClass", productCreation->productClass,
                 productCreationXML);

    // optional
    if (productCreation->productType != Init::undefined<std::string>())
    {
        createString("ProductType", productCreation->productType,
                     productCreationXML);
    }

    // optional to unbounded
    common().addParameters("ProductCreationExtension",
                           productCreation->productCreationExtensions,
                           productCreationXML);

    return productCreationXML;
}

XMLElem DerivedXMLParser::convertDisplayToXML(
        const Display* display,
        XMLElem parent) const
{
    XMLElem displayXML = newElement("Display", parent);

    createString("PixelType", six::toString(display->pixelType), displayXML);

    // optional
    if (display->remapInformation.get())
    {
        XMLElem remapInfoXML = newElement("RemapInformation", displayXML);

        if (display->remapInformation->displayType == DisplayType::COLOR)
        {
            XMLElem remapXML = newElement("ColorDisplayRemap", remapInfoXML);
            if (display->remapInformation->remapLUT.get())
                createLUT("RemapLUT",
                          display->remapInformation->remapLUT.get(), remapXML);
        }
        else if (display->remapInformation->displayType == DisplayType::MONO)
        {
            XMLElem remapXML = newElement("MonochromeDisplayRemap",
                                          remapInfoXML);
            // a little risky, but let's assume the displayType is correct
            MonochromeDisplayRemap* mdr =
                    (MonochromeDisplayRemap*) display->remapInformation.get();
            createString("RemapType", mdr->remapType, remapXML);
            if (mdr->remapLUT.get())
                createLUT("RemapLUT", mdr->remapLUT.get(), remapXML);
            common().addParameters("RemapParameter", mdr->remapParameters, remapXML);
        }
    }

    // optional
    if (display->magnificationMethod != MagnificationMethod::NOT_SET)
    {
        createString("MagnificationMethod",
                     six::toString(display->magnificationMethod), displayXML);
    }

    // optional
    if (display->decimationMethod != DecimationMethod::NOT_SET)
    {
        createString("DecimationMethod",
                     six::toString(display->decimationMethod), displayXML);
    }

    // optional
    if (display->histogramOverrides.get())
    {
        XMLElem histo = newElement("DRAHistogramOverrides", displayXML);
        createInt("ClipMin", display->histogramOverrides->clipMin, histo);
        createInt("ClipMax", display->histogramOverrides->clipMax, histo);
    }

    // optional
    if (display->monitorCompensationApplied.get())
    {
        XMLElem monComp = newElement("MonitorCompensationApplied", displayXML);
        createDouble("Gamma", display->monitorCompensationApplied->gamma,
                     monComp);
        createDouble("XMin", display->monitorCompensationApplied->xMin, monComp);
    }

    // optional to unbounded
    common().addParameters("DisplayExtension", display->displayExtensions, displayXML);

    return displayXML;
}

XMLElem DerivedXMLParser::convertGeographicTargetToXML(
        const GeographicAndTarget* geographicAndTarget,
        XMLElem parent) const
{
    XMLElem geographicAndTargetXML = newElement("GeographicAndTarget", parent);

    convertGeographicCoverageToXML(
            "GeographicCoverage",
            &geographicAndTarget->geographicCoverage,
            geographicAndTargetXML);

    // optional to unbounded
    for (std::vector<mem::ScopedCopyablePtr<TargetInformation> >::
            const_iterator it = geographicAndTarget->targetInformation.begin();
            it != geographicAndTarget->targetInformation.end(); ++it)
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

XMLElem DerivedXMLParser::convertGeographicCoverageToXML(
        const std::string& localName,
        const GeographicCoverage* geoCoverage,
        XMLElem parent) const
{
    //GeographicAndTarget
    XMLElem geoCoverageXML = newElement(localName, parent);

    // optional to unbounded
    common().addParameters("GeoregionIdentifier", geoCoverage->georegionIdentifiers,
                  geoCoverageXML);

    createFootprint("Footprint", "Vertex", geoCoverage->footprint,
                    geoCoverageXML);

    // GeographicInfo
    if (geoCoverage->geographicInformation.get())
    {
        XMLElem geoInfoXML = newElement("GeographicInfo", geoCoverageXML);

        // optional to unbounded
        size_t numCC = geoCoverage->geographicInformation->countryCodes.size();
        for (size_t i = 0; i < numCC; ++i)
        {
            createString("CountryCode",
                         geoCoverage->geographicInformation->countryCodes[i],
                         geoInfoXML);
        }

        // optional
        str::trim(geoCoverage->geographicInformation->securityInformation);
        std::string secInfo
                = geoCoverage->geographicInformation->securityInformation;
        if (!secInfo.empty())
            createString("SecurityInfo", secInfo, geoInfoXML);

        // optional to unbounded
        common().addParameters("GeographicInfoExtension", geoCoverage->
                                    geographicInformation->
                                    geographicInformationExtensions,
                               geoInfoXML);
    }
    else
    {
        //loop over SubRegions
        for (std::vector<mem::ScopedCopyablePtr<GeographicCoverage> >::
                const_iterator it = geoCoverage->subRegion.begin();
                it != geoCoverage->subRegion.end(); ++it)
        {
            convertGeographicCoverageToXML("SubRegion",
                                           (*it).get(),
                                           geoCoverageXML);
        }
    }

    return geoCoverageXML;
}

XMLElem DerivedXMLParser::convertMeasurementToXML(
        const Measurement* measurement,
        XMLElem parent) const
{
    XMLElem measurementXML = newElement("Measurement", parent);

    XMLElem projectionXML = newElement("", measurementXML);

    // NOTE: ReferencePoint is present in all of the ProjectionTypes
    //       so its added here for ease
    XMLElem referencePointXML = newElement("ReferencePoint", projectionXML);
    if (measurement->projection->referencePoint.name
            != Init::undefined<std::string>())
    {
        setAttribute(referencePointXML, "name",
                     measurement->projection->referencePoint.name);
    }
    common().createVector3D("ECEF", common().getSICommonURI(),
                            measurement->projection->referencePoint.ecef,
                            referencePointXML);
    common().createRowCol("Point", common().getSICommonURI(),
                          measurement->projection->referencePoint.rowCol,
                          referencePointXML);

    switch (measurement->projection->projectionType)
    {
    case ProjectionType::POLYNOMIAL:
    {
        projectionXML->setLocalName("PolynomialProjection");

        PolynomialProjection* polyProj
                = (PolynomialProjection*) measurement->projection.get();

        common().createPoly2D("RowColToLat", polyProj->rowColToLat, projectionXML);
        common().createPoly2D("RowColToLon", polyProj->rowColToLon, projectionXML);

        // optional
        if (polyProj->rowColToAlt != Init::undefined<Poly2D>())
        {
            common().createPoly2D("RowColToAlt", polyProj->rowColToAlt, projectionXML);
        }

        common().createPoly2D("LatLonToRow", polyProj->latLonToRow, projectionXML);
        common().createPoly2D("LatLonToCol", polyProj->latLonToCol, projectionXML);
    }
        break;

    case ProjectionType::GEOGRAPHIC:
    {
        projectionXML->setLocalName("GeographicProjection");

        GeographicProjection* geographicProj
                = (GeographicProjection*) measurement->projection.get();

        common().createRowCol("SampleSpacing",
                              geographicProj->sampleSpacing,
                              projectionXML);
        common().createPoly2D("TimeCOAPoly",
                              geographicProj->timeCOAPoly,
                              projectionXML);
    }
        break;

    case ProjectionType::PLANE:
    {
        projectionXML->setLocalName("PlaneProjection");

        PlaneProjection* planeProj
                = (PlaneProjection*) measurement->projection.get();

        common().createRowCol("SampleSpacing",
                              planeProj->sampleSpacing,
                              projectionXML);
        common().createPoly2D("TimeCOAPoly",
                              planeProj->timeCOAPoly,
                              projectionXML);

        XMLElem productPlaneXML = newElement("ProductPlane", projectionXML);
        common().createVector3D("RowUnitVector",
                                planeProj->productPlane.rowUnitVector,
                                productPlaneXML);
        common().createVector3D("ColUnitVector",
                                planeProj->productPlane.colUnitVector,
                                productPlaneXML);
    }
        break;

    case ProjectionType::CYLINDRICAL:
    {
        projectionXML->setLocalName("CylindricalProjection");

        CylindricalProjection* cylindricalProj
                = (CylindricalProjection*) measurement->projection.get();

        common().createRowCol("SampleSpacing",
                              cylindricalProj->sampleSpacing,
                              projectionXML);
        common().createPoly2D("TimeCOAPoly",
                              cylindricalProj->timeCOAPoly,
                              projectionXML);
        common().createVector3D("StripmapDirection",
                                cylindricalProj->stripmapDirection,
                                projectionXML);
        // optional
        if (cylindricalProj->curvatureRadius != Init::undefined<double>())
        {
            createDouble("CurvatureRadius",
                         cylindricalProj->curvatureRadius,
                         projectionXML);
        }
    }
        break;

    default:
        throw except::Exception(Ctxt("Unknown projection type!"));
    }

    common().createRowCol("PixelFootprint",
                          measurement->pixelFootprint,
                          measurementXML);
    common().createPolyXYZ("ARPPoly",
                           measurement->arpPoly,
                           measurementXML);

    return measurementXML;
}

XMLElem DerivedXMLParser::convertExploitationFeaturesToXML(
        const ExploitationFeatures* exploitationFeatures,
        XMLElem parent) const
{

    XMLElem exploitationFeaturesXML =
            newElement("ExploitationFeatures", parent);

    if (exploitationFeatures->collections.size() < 1)
    {
        throw except::Exception(Ctxt(FmtX(
                "ExploitationFeatures must have at least [1] Collection, " \
                "only [%d] found", exploitationFeatures->collections.size())));
    }

    // 1 to unbounded
    for (size_t i = 0; i < exploitationFeatures->collections.size(); ++i)
    {
        Collection* collection = exploitationFeatures->collections[i].get();
        XMLElem collectionXML = newElement("Collection",
                                           exploitationFeaturesXML);
        setAttribute(collectionXML, "identifier", collection->identifier);

        // create Information
        XMLElem informationXML = newElement("Information", collectionXML);

        createString("SensorName",
                     collection->information->sensorName,
                     informationXML);
        XMLElem radarModeXML = newElement("RadarMode", informationXML);
        createString("ModeType",
                     common().getSICommonURI(),
                     six::toString(collection->information->radarMode),
                     radarModeXML);
        // optional
        if (collection->information->radarModeID
                != Init::undefined<std::string>())
            createString("ModeID",
                         common().getSICommonURI(),
                         collection->information->radarModeID,
                         radarModeXML);
        createDateTime("CollectionDateTime",
                       collection->information->collectionDateTime,
                       informationXML);
        // optional
        if (collection->information->localDateTime != Init::undefined<
                std::string>())
        {
            createDateTime("LocalDateTime",
                           collection->information->localDateTime,
                           informationXML);
        }
        createDouble("CollectionDuration",
                     collection->information->collectionDuration,
                     informationXML);
        // optional
        if (!Init::isUndefined(collection->information->resolution))
        {
            common().createRangeAzimuth("Resolution",
                                        collection->information->resolution,
                                        informationXML);
        }
        // optional
        if (collection->information->inputROI.get())
        {
            XMLElem roiXML = newElement("InputROI", informationXML);
            common().createRowCol("Size",
                                  collection->information->inputROI->size,
                                  roiXML);
            common().createRowCol("UpperLeft",
                                  collection->information->inputROI->upperLeft,
                                  roiXML);
        }
        // optional to unbounded
        for (size_t n = 0, nElems =
                collection->information->polarization.size(); n < nElems; ++n)
        {
            TxRcvPolarization *p = collection->information->polarization[n].get();
            XMLElem polXML = newElement("Polarization", informationXML);

            createString("TxPolarization",
                         six::toString(p->txPolarization),
                         polXML);
            createString("RcvPolarization",
                         six::toString(p->rcvPolarization),
                         polXML);
            // optional
            if (!Init::isUndefined(p->rcvPolarizationOffset))
            {
                createDouble("RcvPolarizationOffset",
                             p->rcvPolarizationOffset,
                             polXML);
            }
            // optional
            if (!Init::isUndefined(p->processed))
            {
                createString("Processed", six::toString(p->processed), polXML);
            }
        }

        // create Geometry -- optional
        Geometry* geom = collection->geometry.get();
        if (geom != NULL)
        {
            XMLElem geometryXML = newElement("Geometry", collectionXML);

            // optional
            if (geom->azimuth != Init::undefined<double>())
                createDouble("Azimuth", geom->azimuth, geometryXML);
            // optional
            if (geom->slope != Init::undefined<double>())
                createDouble("Slope", geom->slope, geometryXML);
            // optional
            if (geom->squint != Init::undefined<double>())
                createDouble("Squint", geom->squint, geometryXML);
            // optional
            if (geom->graze != Init::undefined<double>())
                createDouble("Graze", geom->graze, geometryXML);
            // optional
            if (geom->tilt != Init::undefined<double>())
                createDouble("Tilt", geom->tilt, geometryXML);
            // optional to unbounded
            common().addParameters("Extension", geom->extensions,
                                   geometryXML);
        }

        // create Phenomenology -- optional
        Phenomenology* phenom = collection->phenomenology.get();
        if (phenom != NULL)
        {
            XMLElem phenomenologyXML = newElement("Phenomenology",
                                                  collectionXML);

            // optional
            if (phenom->shadow != Init::undefined<AngleMagnitude>())
            {
                XMLElem shadow = newElement("Shadow", phenomenologyXML);
                createDouble("Angle", common().getSICommonURI(),
                             phenom->shadow.angle, shadow);
                createDouble("Magnitude", common().getSICommonURI(),
                             phenom->shadow.magnitude, shadow);
            }
            // optional
            if (phenom->layover != Init::undefined<AngleMagnitude>())
            {
                XMLElem layover = newElement("Layover", phenomenologyXML);
                createDouble("Angle", common().getSICommonURI(),
                             phenom->layover.angle, layover);
                createDouble("Magnitude", common().getSICommonURI(),
                             phenom->layover.magnitude, layover);
            }
            // optional
            if (phenom->multiPath != Init::undefined<double>())
                createDouble("MultiPath", phenom->multiPath, phenomenologyXML);
            // optional
            if (phenom->groundTrack != Init::undefined<double>())
                createDouble("GroundTrack", phenom->groundTrack,
                             phenomenologyXML);
            // optional to unbounded
            common().addParameters("Extension", phenom->extensions,
                                   phenomenologyXML);
        }
    }

    // create Product
    XMLElem productXML = newElement("Product", exploitationFeaturesXML);

    common().createRowCol("Resolution",
                          exploitationFeatures->product.resolution,
                          productXML);
    // optional
    if (exploitationFeatures->product.north != Init::undefined<double>())
        createDouble("North", exploitationFeatures->product.north, productXML);
    // optional to unbounded
    common().addParameters("Extension",
                           exploitationFeatures->product.extensions,
                           productXML);

    return exploitationFeaturesXML;
}


XMLElem DerivedXMLParser::createLUT(const std::string& name, const LUT *lut,
        XMLElem parent) const
{
    XMLElem lutElement = newElement(name, parent);
    setAttribute(lutElement, "size", str::toString(lut->numEntries));

    std::ostringstream oss;
    for (unsigned int i = 0; i < lut->numEntries; ++i)
    {
        if (lut->elementSize == 2)
        {
            short* idx = (short*) (*lut)[i];
            oss << *idx;
        }
        else if (lut->elementSize == 3)
        {
            oss << (unsigned int) (*lut)[i][0] << ','
                    << (unsigned int) (*lut)[i][1] << ','
                    << (unsigned int) (*lut)[i][2];
        }
        else
        {
            throw except::Exception(Ctxt(FmtX("Invalid element size [%d]",
                                              lut->elementSize)));
        }
        if ((lut->numEntries - 1) != i)
            oss << ' ';
    }
    lutElement->setCharacterData(oss.str());
    return lutElement;
}

XMLElem DerivedXMLParser::createFootprint(const std::string& name,
                                          const std::string& cornerName,
                                          const LatLonCorners& corners,
                                          XMLElem parent) const
{
    XMLElem footprint = newElement(name, getDefaultURI(), parent);
    xml::lite::AttributeNode node;
    node.setQName("size");
    node.setValue(str::toString(LatLonCorners::NUM_CORNERS));

    footprint->getAttributes().add(node);

    // Write the corners out in CW order
    // The index attribute is 1-based
    node.setQName("index");

    for (size_t corner = 0; corner < LatLonCorners::NUM_CORNERS; ++corner)
    {
        node.setValue(str::toString(corner + 1));
        common().createLatLon(cornerName,
                             corners.getCorner(corner),
                             footprint)->getAttributes().add(node);
    }

    return footprint;
}

XMLElem DerivedXMLParser::createFootprint(const std::string& name,
                                          const std::string& cornerName,
                                          const LatLonAltCorners& corners,
                                          XMLElem parent) const
{
    XMLElem footprint = newElement(name, getDefaultURI(), parent);
    xml::lite::AttributeNode node;
    node.setQName("size");
    node.setValue(str::toString(LatLonAltCorners::NUM_CORNERS));

    footprint->getAttributes().add(node);

    // Write the corners out in CW order
    // The index attribute is 1-based
    node.setQName("index");

    for (size_t corner = 0; corner < LatLonCorners::NUM_CORNERS; ++corner)
    {
        node.setValue(str::toString(corner + 1));
        common().createLatLonAlt(cornerName,
                                corners.getCorner(corner),
                                footprint)->getAttributes().add(node);
    }

    return footprint;
}

XMLElem DerivedXMLParser::createSFADatum(const std::string& name,
                                         const six::sidd::SFADatum& datum,
                                         XMLElem parent) const
{
    XMLElem datumXML = newElement(name, SFA_URI, parent);

    XMLElem spheriodXML = newElement("Spheroid", SFA_URI, datumXML);

    createString("SpheriodName", SFA_URI, datum.spheroid.name, spheriodXML);
    createDouble("SemiMajorAxis", SFA_URI,
                 datum.spheroid.semiMajorAxis, spheriodXML);
    createDouble("InverseFlattening", SFA_URI,
                 datum.spheroid.inverseFlattening, spheriodXML);

    return datumXML;
}

XMLElem DerivedXMLParser::convertProductProcessingToXML(
        const ProductProcessing* productProcessing,
        XMLElem parent) const
{
    XMLElem productProcessingXML = newElement("ProductProcessing", parent);

    // error checking
    if (productProcessing->processingModules.size() < 1)
    {
        throw except::Exception(Ctxt(FmtX(
                "There must be at least [1] ProcessingModule in "\
                "ProductProcessing, [%d] found",
                productProcessing->processingModules.size())));
    }

    // one to unbounded
    for (std::vector<mem::ScopedCloneablePtr<ProcessingModule> >::
            const_iterator it = productProcessing->processingModules.begin();
            it != productProcessing->processingModules.end(); ++it)
    {
        convertProcessingModuleToXML((*it).get(), productProcessingXML);
    }

    return productProcessingXML;
}

XMLElem DerivedXMLParser::convertProcessingModuleToXML(
        const ProcessingModule* procMod,
        XMLElem parent) const
{
    XMLElem procModXML = newElement("ProcessingModule", parent);

    common().createParameter("ModuleName", procMod->moduleName, procModXML);

    // optional choice
    if (!procMod->processingModules.empty())
    {
        // one to unbounded
        for (std::vector<mem::ScopedCloneablePtr<ProcessingModule> >::
                const_iterator it = procMod->processingModules.begin();
                it != procMod->processingModules.end(); ++it)
        {
            convertProcessingModuleToXML((*it).get(), procModXML);
        }
    }
    else if (!procMod->moduleParameters.empty())
    {
        common().addParameters("ModuleParameter",
                               procMod->moduleParameters,
                               procModXML);
    }

    return procModXML;
}

XMLElem DerivedXMLParser::convertDownstreamReprocessingToXML(
        const DownstreamReprocessing* downstreamReproc,
        XMLElem parent) const
{
    XMLElem epXML = newElement("DownstreamReprocessing", parent);

    // optional
    GeometricChip *geoChip = downstreamReproc->geometricChip.get();
    if (geoChip)
    {
        XMLElem geoChipXML = newElement("GeometricChip", epXML);
        common().createRowCol("ChipSize", geoChip->chipSize, geoChipXML);
        common().createRowCol("OriginalUpperLeftCoordinate",
                     geoChip->originalUpperLeftCoordinate, geoChipXML);
        common().createRowCol("OriginalUpperRightCoordinate",
                     geoChip->originalUpperRightCoordinate, geoChipXML);
        common().createRowCol("OriginalLowerLeftCoordinate",
                     geoChip->originalLowerLeftCoordinate, geoChipXML);
        common().createRowCol("OriginalLowerRightCoordinate",
                     geoChip->originalLowerRightCoordinate, geoChipXML);
    }
    // optional to unbounded
    if (!downstreamReproc->processingEvents.empty())
    {
        for (std::vector<mem::ScopedCopyablePtr<ProcessingEvent> >::
                const_iterator it = downstreamReproc->processingEvents.begin();
                it != downstreamReproc->processingEvents.end(); ++it)
        {
            ProcessingEvent *procEvent = (*it).get();
            XMLElem procEventXML = newElement("ProcessingEvent", epXML);

            createString("ApplicationName", procEvent->applicationName,
                         procEventXML);
            createDateTime("AppliedDateTime", procEvent->appliedDateTime,
                           procEventXML);
            // optional
            if (!procEvent->interpolationMethod.empty())
            {
                createString("InterpolationMethod",
                             procEvent->interpolationMethod, procEventXML);
            }
            // optional to unbounded
            common().addParameters("Descriptor", procEvent->descriptor, procEventXML);
        }
    }
    return epXML;
}

void DerivedXMLParser::parseProcessingModuleFromXML(
        const XMLElem procXML,
        ProcessingModule* procMod) const
{
    common().parseParameter(getFirstAndOnly(procXML, "ModuleName"),
                            procMod->moduleName);

    common().parseParameters(procXML, "ModuleParameter", procMod->moduleParameters);

    std::vector<XMLElem> procModuleXML;
    procXML->getElementsByTagName("ProcessingModule", procModuleXML);
    procMod->processingModules.resize(procModuleXML.size());
    for (size_t i = 0, size = procModuleXML.size(); i < size; ++i)
    {
        procMod->processingModules[i].reset(new ProcessingModule());
        parseProcessingModuleFromXML(
                procModuleXML[i], procMod->processingModules[i].get());
    }
}

void DerivedXMLParser::parseProductProcessingFromXML(
        const XMLElem elem,
        ProductProcessing* productProcessing) const
{
    std::vector<XMLElem> procModuleXML;
    elem->getElementsByTagName("ProcessingModule", procModuleXML);
    productProcessing->processingModules.resize(procModuleXML.size());
    for (size_t i = 0, size = procModuleXML.size(); i < size; ++i)
    {
        productProcessing->processingModules[i].reset(new ProcessingModule());
        parseProcessingModuleFromXML(
                procModuleXML[i],
                productProcessing->processingModules[i].get());
    }
}

void DerivedXMLParser::parseDownstreamReprocessingFromXML(
        const XMLElem elem,
        DownstreamReprocessing* downstreamReproc) const
{
    XMLElem geometricChipXML = getOptional(elem, "GeometricChip");
    if (geometricChipXML)
    {
        downstreamReproc->geometricChip.reset(new GeometricChip());
        GeometricChip *chip = downstreamReproc->geometricChip.get();

        common().parseRowColInt(getFirstAndOnly(geometricChipXML, "ChipSize"),
                                chip->chipSize);
        common().parseRowColDouble(getFirstAndOnly(geometricChipXML,
                                   "OriginalUpperLeftCoordinate"),
                                   chip->originalUpperLeftCoordinate);
        common().parseRowColDouble(getFirstAndOnly(geometricChipXML,
                                   "OriginalUpperRightCoordinate"),
                                   chip->originalUpperRightCoordinate);
        common().parseRowColDouble(getFirstAndOnly(geometricChipXML,
                                   "OriginalLowerLeftCoordinate"),
                                   chip->originalLowerLeftCoordinate);
        common().parseRowColDouble(getFirstAndOnly(geometricChipXML,
                                   "OriginalLowerRightCoordinate"),
                                   chip->originalLowerRightCoordinate);
    }

    std::vector<XMLElem> procEventXML;
    elem->getElementsByTagName("ProcessingEvent", procEventXML);
    downstreamReproc->processingEvents.resize(procEventXML.size());
    for (size_t i = 0, size = procEventXML.size(); i < size; ++i)
    {
        downstreamReproc->processingEvents[i].reset(new ProcessingEvent());
        ProcessingEvent* procEvent
                = downstreamReproc->processingEvents[i].get();

        XMLElem peXML = procEventXML[i];
        parseString(getFirstAndOnly(peXML, "ApplicationName"),
                    procEvent->applicationName);
        parseDateTime(getFirstAndOnly(peXML, "AppliedDateTime"),
                      procEvent->appliedDateTime);

        // optional
        XMLElem tmpElem = getOptional(peXML, "InterpolationMethod");
        if (tmpElem)
        {
            parseString(tmpElem, procEvent->interpolationMethod);
        }

        // optional to unbounded
        common().parseParameters(peXML, "Descriptor", procEvent->descriptor);
    }
}

void DerivedXMLParser::parseGeographicCoordinateSystemFromXML(
        const XMLElem coorSysElem,
        SFAGeographicCoordinateSystem* coordSys) const
{
    parseString(getFirstAndOnly(coorSysElem, "Csname"), coordSys->csName);
    parseDatum(getFirstAndOnly(coorSysElem, "Datum"), coordSys->datum);

    XMLElem primeXML = getFirstAndOnly(coorSysElem, "PrimeMeridian");
    parseString(getFirstAndOnly(primeXML, "Name"),
            coordSys->primeMeridian.name);
    parseDouble(getFirstAndOnly(primeXML, "Longitude"),
            coordSys->primeMeridian.longitude);

    parseString(getFirstAndOnly(coorSysElem, "AngularUnit"),
                coordSys->angularUnit);

    // optional
    XMLElem luXML = getOptional(coorSysElem, "LinearUnit");
    if (luXML)
        parseString(luXML, coordSys->linearUnit);
}

void DerivedXMLParser::parseAnnotationFromXML(
        const XMLElem elem,
        Annotation *a) const
{
    parseString(getFirstAndOnly(elem, "Identifier"), a->identifier);

    XMLElem spatialXML = getOptional(elem, "SpatialReferenceSystem");
    if (spatialXML)
    {
        a->spatialReferenceSystem.reset(new six::sidd::SFAReferenceSystem());

        // choice
        XMLElem tmpXML = getOptional(spatialXML,
                                     "ProjectedCoordinateSystem");
        if (tmpXML)
        {
            a->spatialReferenceSystem->coordinateSystem.reset(
                    new SFAProjectedCoordinateSystem());

            SFAProjectedCoordinateSystem* coordSys =
                    (SFAProjectedCoordinateSystem*)
                            a->spatialReferenceSystem->coordinateSystem.get();

            parseString(getFirstAndOnly(tmpXML, "Csname"), coordSys->csName);

            coordSys->geographicCoordinateSystem.reset(
                    new SFAGeographicCoordinateSystem());
            parseGeographicCoordinateSystemFromXML(
                    getFirstAndOnly(tmpXML, "GeographicCoordinateSystem"),
                    coordSys->geographicCoordinateSystem.get());

            XMLElem projXML = getFirstAndOnly(tmpXML, "Projection");
            parseString(getFirstAndOnly(projXML, "ProjectionName"),
                        coordSys->projection.name);

            XMLElem paramXML = getOptional(tmpXML, "Parameter");
            if (paramXML)
            {
                parseString(getFirstAndOnly(paramXML, "ParameterName"),
                        coordSys->parameter.name);
                parseDouble(getFirstAndOnly(paramXML, "Value"),
                        coordSys->parameter.value);
            }

            parseString(getFirstAndOnly(tmpXML, "LinearUnit"),
                        coordSys->linearUnit);
        }

        tmpXML = getOptional(spatialXML, "GeographicCoordinateSystem");
        if (tmpXML)
        {
            a->spatialReferenceSystem->coordinateSystem.reset(
                    new SFAGeographicCoordinateSystem());

            SFAGeographicCoordinateSystem* coordSys =
                    (SFAGeographicCoordinateSystem*)
                            a->spatialReferenceSystem->coordinateSystem.get();

            parseGeographicCoordinateSystemFromXML(tmpXML, coordSys);
        }

        tmpXML = getOptional(spatialXML, "GeocentricCoordinateSystem");
        if (tmpXML)
        {
            a->spatialReferenceSystem->coordinateSystem.reset(
                    new SFAGeocentricCoordinateSystem());

            SFAGeocentricCoordinateSystem* coordSys =
                    (SFAGeocentricCoordinateSystem*)
                            a->spatialReferenceSystem->coordinateSystem.get();

            parseString(getFirstAndOnly(tmpXML, "Csname"), coordSys->csName);
            parseDatum(getFirstAndOnly(tmpXML, "Datum"), coordSys->datum);

            XMLElem primeXML = getFirstAndOnly(tmpXML, "PrimeMeridian");
            parseString(getFirstAndOnly(primeXML, "Name"),
                    coordSys->primeMeridian.name);
            parseDouble(getFirstAndOnly(primeXML, "Longitude"),
                    coordSys->primeMeridian.longitude);

            parseString(getFirstAndOnly(tmpXML, "LinearUnit"),
                        coordSys->linearUnit);
        }

        // 1 to unbounded
        std::vector<XMLElem> axixNamesXML;
        spatialXML->getElementsByTagName("AxisName", axixNamesXML);
        for (size_t i = 0; i < axixNamesXML.size(); ++i)
        {
            std::string axisName = "";
            parseString(axixNamesXML[i], axisName);
            a->spatialReferenceSystem->axisNames.push_back(axisName);
        }
    }
    else
    {
        a->spatialReferenceSystem.reset();
    }

    std::vector<XMLElem> objectsXML;
    elem->getElementsByTagName("Object", objectsXML);
    a->objects.resize(objectsXML.size());
    for (size_t i = 0, size = objectsXML.size(); i < size; ++i)
    {
        XMLElem obj = objectsXML[i];

        //there should be only one child - a choice between types
        std::vector<XMLElem> &children = obj->getChildren();
        if (!children.empty())
        {
            //just get the first one
            XMLElem child = children[0];
            std::string childName = child->getLocalName();
            str::trim(childName);

            if (childName == "Point")
                a->objects[i].reset(new SFAPoint());
            else if (childName == "Line")
                a->objects[i].reset(new SFALine());
            else if (childName == "LinearRing")
                a->objects[i].reset(new SFALinearRing());
            else if (childName == "Polygon")
                a->objects[i].reset(new SFAPolygon());
            else if (childName == "PolyhedralSurface")
                a->objects[i].reset(new SFAPolyhedralSurface());
            else if (childName == "MultiPolygon")
                a->objects[i].reset(new SFAMultiPolygon());
            else if (childName == "MultiLineString")
                a->objects[i].reset(new SFAMultiLineString());
            else if (childName == "MultiPoint")
                a->objects[i].reset(new SFAMultiPoint());

            SFAGeometry* geoType = a->objects[i].get();
            if (geoType)
            {
                parseSFAGeometryFromXML(child, geoType);
            }
        }
    }
}

void DerivedXMLParser::parseDatum(const XMLElem datumXML, SFADatum& datum) const
{
    XMLElem spheroidXML = getFirstAndOnly(datumXML, "Spheroid");

    parseString(getFirstAndOnly(spheroidXML, "SpheriodName"),
                datum.spheroid.name);
    parseDouble(getFirstAndOnly(spheroidXML, "SemiMajorAxis"),
                datum.spheroid.semiMajorAxis);
    parseDouble(getFirstAndOnly(spheroidXML, "InverseFlattening"),
                datum.spheroid.inverseFlattening);
}

XMLElem DerivedXMLParser::convertGeographicCoordinateSystemToXML(
        const SFAGeographicCoordinateSystem* geographicCoordinateSystem,
        XMLElem parent) const
{
    XMLElem geoSysXML = newElement("GeographicCoordinateSystem",
                                   SFA_URI, parent);

    createString("Csname", SFA_URI,
                 geographicCoordinateSystem->csName, geoSysXML);
    createSFADatum("Datum", geographicCoordinateSystem->datum, geoSysXML);

    XMLElem primeMeridianXML = newElement("PrimeMeridian", SFA_URI, geoSysXML);
    createString("Name", SFA_URI,
                 geographicCoordinateSystem->primeMeridian.name,
                 primeMeridianXML);
    createDouble("Longitude", SFA_URI,
                 geographicCoordinateSystem-> primeMeridian.longitude,
                 primeMeridianXML);

    createString("AngularUnit", SFA_URI,
                 geographicCoordinateSystem->angularUnit, geoSysXML);
    createString("LinearUnit", SFA_URI, geographicCoordinateSystem->linearUnit,
                 geoSysXML);

    return geoSysXML;
}

XMLElem DerivedXMLParser::convertAnnotationToXML(
        const Annotation* a,
        XMLElem parent) const
{
    XMLElem annXML = newElement("Annotation", parent);

    createString("Identifier", a->identifier, annXML);

    // optional
    if (a->spatialReferenceSystem.get())
    {
        XMLElem spRefXML = newElement("SpatialReferenceSystem",
                                      getDefaultURI(), annXML);

        if (a->spatialReferenceSystem->coordinateSystem->getType()
                == six::sidd::SFAProjectedCoordinateSystem::TYPE_NAME)
        {
            XMLElem coordXML = newElement("ProjectedCoordinateSystem", SFA_URI, spRefXML);

            SFAProjectedCoordinateSystem* coordSys
                    = (SFAProjectedCoordinateSystem*)a->
                            spatialReferenceSystem->coordinateSystem.get();

            createString("Csname", SFA_URI, coordSys->csName, coordXML);

            convertGeographicCoordinateSystemToXML(
                    coordSys->geographicCoordinateSystem.get(), coordXML);

            XMLElem projectionXML = newElement("Projection", SFA_URI, coordXML);
            createString("ProjectionName", SFA_URI, coordSys->projection.name,
                         projectionXML);

            // optional
            if (!coordSys->parameter.name.empty())
            {
                XMLElem parameterXML = newElement("Parameter", SFA_URI,
                                                  coordXML);
                createString("ParameterName", SFA_URI,
                             coordSys->parameter.name, parameterXML);
                createDouble("Value", SFA_URI,
                             coordSys->parameter.value, parameterXML);
            }

            createString("LinearUnit", SFA_URI,
                         coordSys->linearUnit, coordXML);
        }
        else if (a->spatialReferenceSystem->coordinateSystem->getType()
                    == six::sidd::SFAGeographicCoordinateSystem::TYPE_NAME)
        {
            SFAGeographicCoordinateSystem* coordSys
                    = (SFAGeographicCoordinateSystem*)a->
                            spatialReferenceSystem->coordinateSystem.get();
            convertGeographicCoordinateSystemToXML(coordSys, spRefXML);
        }
        else if (a->spatialReferenceSystem->coordinateSystem->getType()
                    == six::sidd::SFAGeocentricCoordinateSystem::TYPE_NAME)
        {
            XMLElem coordXML = newElement("GeocentricCoordinateSystem",
                                          SFA_URI, spRefXML);

            SFAGeocentricCoordinateSystem* coordSys
                    = (SFAGeocentricCoordinateSystem*)a->
                            spatialReferenceSystem->coordinateSystem.get();

            createString("Csname", SFA_URI, coordSys->csName, coordXML);
            createSFADatum("Datum", coordSys->datum, coordXML);

            XMLElem primeMeridianXML = newElement("PrimeMeridian", SFA_URI,
                                                  coordXML);
            createString("Name", SFA_URI,
                         coordSys->primeMeridian.name,
                         primeMeridianXML);
            createDouble("Longitude", SFA_URI,
                         coordSys->primeMeridian.longitude,
                         primeMeridianXML);

            createString("LinearUnit", SFA_URI,
                         coordSys->linearUnit, coordXML);
        }

        // one to unbounded
        for(size_t ii = 0; ii < a->spatialReferenceSystem->axisNames.size();
                ++ii)
        {
            createString("AxisName", SFA_URI,
                         a->spatialReferenceSystem->axisNames[ii],
                         spRefXML);
        }
    }

    // one to unbounded
    for (size_t i = 0, num = a->objects.size(); i < num; ++i)
    {
        XMLElem objXML = newElement("Object", annXML);
        convertSFAGeometryToXML(a->objects[i].get(), objXML);
    }
    return annXML;
}

void DerivedXMLParser::parseSFAGeometryFromXML(const XMLElem elem, SFAGeometry *g) const
{
    std::string geoType = g->getType();
    if (geoType == SFAPoint::TYPE_NAME)
    {
        SFAPoint* p = (SFAPoint*) g;
        parseDouble(getFirstAndOnly(elem, "X"), p->x);
        parseDouble(getFirstAndOnly(elem, "Y"), p->y);

        XMLElem tmpElem = getOptional(elem, "Z");
        if (tmpElem)
            parseDouble(tmpElem, p->z);

        tmpElem = getOptional(elem, "M");
        if (tmpElem)
            parseDouble(tmpElem, p->m);
    }
    //for now, line, linearring, and linestring are parsed the same
    else if (geoType == SFALine::TYPE_NAME || geoType
            == SFALinearRing::TYPE_NAME || geoType == SFALineString::TYPE_NAME)
    {
        //cast to the common base - LineString
        SFALineString* p = (SFALineString*) g;
        std::vector<XMLElem> vXML;
        elem->getElementsByTagName("Vertex", vXML);
        p->vertices.resize(vXML.size());
        for (size_t i = 0, size = vXML.size(); i < size; ++i)
        {
            p->vertices[i].reset(new SFAPoint());
            parseSFAGeometryFromXML(vXML[i], p->vertices[i].get());
        }
    }
    else if (geoType == SFAPolygon::TYPE_NAME)
    {
        SFAPolygon* p = (SFAPolygon*) g;
        std::vector<XMLElem> ringXML;
        elem->getElementsByTagName("Ring", ringXML);
        p->rings.resize(ringXML.size());
        for (size_t i = 0, size = ringXML.size(); i < size; ++i)
        {
            p->rings[i].reset(new SFALinearRing());
            parseSFAGeometryFromXML(ringXML[i], p->rings[i].get());
        }
    }
    else if (geoType == SFAPolyhedralSurface::TYPE_NAME)
    {
        SFAPolyhedralSurface* p = (SFAPolyhedralSurface*) g;
        std::vector<XMLElem> polyXML;
        elem->getElementsByTagName("Patch", polyXML);
        p->patches.resize(polyXML.size());
        for (size_t i = 0, size = polyXML.size(); i < size; ++i)
        {
            p->patches[i].reset(new SFAPolygon());
            parseSFAGeometryFromXML(polyXML[i], p->patches[i].get());
        }
    }
    else if (geoType == SFAMultiPolygon::TYPE_NAME)
    {
        SFAMultiPolygon* p = (SFAMultiPolygon*) g;
        std::vector<XMLElem> polyXML;
        elem->getElementsByTagName("Element", polyXML);
        p->elements.resize(polyXML.size());
        for (size_t i = 0, size = polyXML.size(); i < size; ++i)
        {
            p->elements[i].reset(new SFAPolygon());
            parseSFAGeometryFromXML(polyXML[i], p->elements[i].get());
        }
    }
    else if (geoType == SFAMultiLineString::TYPE_NAME)
    {
        SFAMultiLineString* p = (SFAMultiLineString*) g;
        std::vector<XMLElem> lineXML;
        elem->getElementsByTagName("Element", lineXML);
        p->elements.resize(lineXML.size());
        for (size_t i = 0, size = lineXML.size(); i < size; ++i)
        {
            p->elements[i].reset(new SFALineString());
            parseSFAGeometryFromXML(lineXML[i], p->elements[i].get());
        }
    }
    else if (geoType == SFAMultiPoint::TYPE_NAME)
    {
        SFAMultiPoint* p = (SFAMultiPoint*) g;
        std::vector<XMLElem> vXML;
        elem->getElementsByTagName("Vertex", vXML);
        p->vertices.resize(vXML.size());
        for (size_t i = 0, size = vXML.size(); i < size; ++i)
        {
            p->vertices[i].reset(new SFAPoint());
            parseSFAGeometryFromXML(vXML[i], p->vertices[i].get());
        }
    }
    else
    {
        log()->warn(Ctxt("Unable to parse unknown geometry type"));
    }
}

XMLElem DerivedXMLParser::createSFAPoint(
        const std::string& localName,
        const SFAPoint* p,
        XMLElem parent) const
{
    XMLElem pointXML
            = newElement(localName,
                         (localName == "Vertex")
                         ? SFA_URI : getDefaultURI(), parent);

    createDouble("X", SFA_URI, p->x, pointXML);
    createDouble("Y", SFA_URI, p->y, pointXML);

    // optional
    if (!Init::isUndefined(p->z))
        createDouble("Z", SFA_URI, p->z, pointXML);
    // optional
    if (!Init::isUndefined(p->m))
        createDouble("M", SFA_URI, p->m, pointXML);

    return pointXML;
}

XMLElem DerivedXMLParser::createSFALine(
        const std::string& localName,
        const SFALineString* l,
        XMLElem parent) const
{
    XMLElem lineXML
            = newElement(localName,
                         (localName == "Ring")
                         ? SFA_URI : getDefaultURI(), parent);

    // error check the vertices
    if (l->vertices.size() < 2)
        throw except::Exception(Ctxt(FmtX(
                "Must be at least two Vertices in LineString. Only [%d] " \
                "found", l->vertices.size())));

    // two to unbounded
    for (size_t ii = 0; ii < l->vertices.size(); ++ii)
    {
        createSFAPoint("Vertex", l->vertices[ii].get(), lineXML);
    }

    return lineXML;
}

XMLElem DerivedXMLParser::convertSFAGeometryToXML(
        const SFAGeometry* g,
        XMLElem parent) const
{
    XMLElem geoXML = NULL;

    std::string geoType = g->getType();
    if (geoType == SFAPoint::TYPE_NAME)
    {
        SFAPoint* p = (SFAPoint*) g;
        createSFAPoint("Point", p, parent);
    }
    //  LineType, linearRingType, and LineStringType
    //  all derive from LineStringType
    else if (geoType == SFALine::TYPE_NAME
                || geoType == SFALinearRing::TYPE_NAME
                || geoType == SFALineString::TYPE_NAME)
    {
        geoXML = createSFALine(geoType, (SFALineString*) g, parent);
    }
    else if (geoType == SFAPolygon::TYPE_NAME)
    {
        geoXML = newElement("Polygon", getDefaultURI(), parent);

        SFAPolygon* p = (SFAPolygon*) g;

        // one to unbounded
        for (size_t ii = 0; ii < p->rings.size(); ++ii)
        {
            createSFALine("Ring", p->rings[ii].get(), geoXML);
        }
    }
    else if (geoType == SFAPolyhedralSurface::TYPE_NAME)
    {
        geoXML = newElement("PolyhedralSurface", getDefaultURI(), parent);

        SFAPolyhedralSurface* p = (SFAPolyhedralSurface*) g;

        for (size_t ii = 0; ii < p->patches.size(); ++ii)
        {
            XMLElem patchXML = newElement("Patch", SFA_URI, geoXML);
            for (size_t jj = 0; jj < p->patches[ii]->rings.size(); ++jj)
            {
                createSFALine("Ring", p->patches[ii]->rings[jj].get(), patchXML);
            }
        }
    }
    else if (geoType == SFAMultiPolygon::TYPE_NAME)
    {
        geoXML = newElement("MultiPolygon", getDefaultURI(), parent);

        SFAMultiPolygon* p = (SFAMultiPolygon*) g;

        // optional to unbounded
        for (size_t ii = 0; ii < p->elements.size(); ++ii)
        {
            XMLElem elemXML = newElement("Element", SFA_URI, geoXML);
            for (size_t jj = 0; jj < p->elements[ii]->rings.size(); ++jj)
            {
                createSFALine("Ring", p->elements[ii]->rings[jj].get(), elemXML);
            }
        }
    }
    else if (geoType == SFAMultiLineString::TYPE_NAME)
    {
        geoXML = newElement("MultiLineString", getDefaultURI(), parent);

        SFAMultiLineString* p = (SFAMultiLineString*) g;

        // optional to unbounded
        for (size_t ii = 0; ii < p->elements.size(); ++ii)
        {
            XMLElem elemXML = newElement("Element", SFA_URI, geoXML);
            for (size_t jj = 0; jj < p->elements[ii]->vertices.size(); ++jj)
            {
                createSFAPoint("Vertex", p->elements[ii]->vertices[jj].get(),
                               elemXML);
            }
        }
    }
    else if (geoType == SFAMultiPoint::TYPE_NAME)
    {
        geoXML = newElement("MultiPoint", getDefaultURI(), parent);

        SFAMultiPoint* p = (SFAMultiPoint*) g;

        // error check the vertices
        if (p->vertices.size() < 2)
            throw except::Exception(Ctxt(FmtX(
                    "Must be at least two Vertices in LineString. Only [%d] " \
                    "found", p->vertices.size())));

        // two to unbounded
        for (size_t ii = 0; ii < p->vertices.size(); ++ii)
        {
            createSFAPoint("Vertex", p->vertices[ii].get(), geoXML);
        }
    }
    else
    {
        throw except::InvalidArgumentException(Ctxt(FmtX(
                "Invalid geo type: [%s]",
                geoType.c_str())));
    }

    return geoXML;
}
}
}

