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
#include <sstream>

#include <sys/Conf.h>
#include <str/Manip.h>
#include <except/Exception.h>
#include <six/sidd/DerivedDataBuilder.h>
#include <six/sidd/DerivedXMLParser.h>

namespace
{
typedef xml::lite::Element* XMLElem;
typedef xml::lite::Attributes XMLAttributes;
}

namespace six
{
namespace sidd
{
const char DerivedXMLParser::SFA_URI[] = "urn:SFA:1.2.0";

DerivedXMLParser::DerivedXMLParser(
        const std::string& version,
        std::unique_ptr<six::SICommonXMLParser>&& comParser,
        logging::Logger* log,
        bool ownLog) :
    XMLParser(versionToURI(version), false, log, ownLog),
    mCommon(std::move(comParser))
{
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
    ptrdiff_t& value)
{
    if (attributes.contains(attributeName))
    {
        value = str::toType<ptrdiff_t>(attributes.getValue(attributeName));
    }
    else
    {
        value = six::Init::undefined<ptrdiff_t>();
    }
}

void DerivedXMLParser::getAttributeIfExists(
    const xml::lite::Attributes& attributes,
    const std::string& attributeName,
    size_t& value)
{
    if (attributes.contains(attributeName))
    {
        value = str::toType<size_t>(attributes.getValue(attributeName));
    }
    else
    {
        value = six::Init::undefined<size_t>();
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

void DerivedXMLParser::getAttributeIfExists(
    const xml::lite::Attributes& attributes,
    const std::string& attributeName,
    BooleanType& boolean)
{
    if (attributes.contains(attributeName))
    {
        std::string name = attributes.getValue(attributeName);
        //I can't imagine we'd actually see this, but 0 and 1 are valid xsd:boolean values,
        //So I'd think we should be able to parse that, too
        if (name == "true" || name == "1")
        {
            boolean = BooleanType("IS_TRUE");
        }
        else if (name == "false" || name == "0")
        {
            boolean = BooleanType("IS_FALSE");
        }
        else
        {
            //This allows the function to continue parsing IS_TRUE, IS_FALSE as desired,
            //as well as throwing an exception for undesired input
            boolean = BooleanType(name);
        }
    }
    else
    {
        boolean = BooleanType("NOT_SET");
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

void DerivedXMLParser::setAttributeIfNonEmpty(XMLElem element,
                                              const std::string& name,
                                              BooleanType value,
                                              const std::string& uri)
{
    if (!Init::isUndefined(value))
    {
        setAttribute(element, name, value == BooleanType::IS_TRUE ? "true" : "false", uri);
    }
}

void DerivedXMLParser::setAttributeIfNonNull(XMLElem element,
                                             const std::string& name,
                                             const DateTime* value,
                                             const std::string& uri)
{
    if (value)
    {
        setAttribute(element, name, six::toString(*value), uri);
    }
}

void DerivedXMLParser::parseProductCreationFromXML(
        const XMLElem informationElem,
        ProcessorInformation* processorInformation) const
{
    parseString(getFirstAndOnly(informationElem, "Application"),
                processorInformation->application);
    parseDateTime(getFirstAndOnly(informationElem, "ProcessingDateTime"),
                  processorInformation->processingDateTime);
    parseString(getFirstAndOnly(informationElem, "Site"),
                processorInformation->site);

    // optional
    XMLElem profileElem = getOptional(informationElem, "Profile");
    if (profileElem)
    {
        parseString(profileElem, processorInformation->profile);
    }
}

void DerivedXMLParser::parseProductCreationFromXML(
        const XMLElem productCreationElem,
        ProductCreation* productCreation) const
{
    parseProductCreationFromXML(
            getFirstAndOnly(productCreationElem, "ProcessorInformation"),
            &productCreation->processorInformation);

    parseDerivedClassificationFromXML(
            getFirstAndOnly(productCreationElem, "Classification"),
            productCreation->classification);

    parseString(getFirstAndOnly(productCreationElem, "ProductName"),
                productCreation->productName);
    parseString(getFirstAndOnly(productCreationElem, "ProductClass"),
                productCreation->productClass);

    // optional
    XMLElem productTypeElem = getOptional(productCreationElem, "ProductType");
    if (productTypeElem)
    {
        parseString(productTypeElem, productCreation->productType);
    }

    // optional
    common().parseParameters(productCreationElem, "ProductCreationExtension",
                             productCreation->productCreationExtensions);
}

void DerivedXMLParser::parseDerivedClassificationFromXML(
    const XMLElem classificationElem,
    DerivedClassification& classification) const
{
    // optional to unbounded
    common().parseParameters(classificationElem, "SecurityExtension",
        classification.securityExtensions);

    const XMLAttributes& classificationAttributes
        = classificationElem->getAttributes();

    //! from ism:ISMRootNodeAttributeGroup
    // Could do
    // toType<int32_t>(
    //        classificationAttributes.getValue("ism:DESVersion"));
    // here if we needed to verify this for any reason

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
}


Remap* DerivedXMLParser::parseRemapChoiceFromXML(
        const XMLElem remapInformationElem) const
{
    if (remapInformationElem)
    {
        XMLElem colorRemapElem =
                getOptional(remapInformationElem, "ColorDisplayRemap");
        XMLElem monoRemapElem =
                getOptional(remapInformationElem, "MonochromeDisplayRemap");

        if (colorRemapElem && !monoRemapElem)
        {
            XMLElem remapLUTElem = getFirstAndOnly(colorRemapElem, "RemapLUT");

            //get size attribute
            int size = str::toType<int>(remapLUTElem->attribute("size"));

            // xs:list is space delimited
            std::string lutStr = remapLUTElem->getCharacterData();
            std::vector<std::string> lutVals = str::split(lutStr, " ");
            std::unique_ptr<LUT> remapLUT(new LUT(size, 3));

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

                    std::byte val = static_cast<std::byte>(
                            intermediateVal);

                    ::memcpy(&(remapLUT->getTable()[k++]), &val,
                             sizeof(std::byte));
                }
            }
            return new ColorDisplayRemap(remapLUT.release());
        }
        else if (!colorRemapElem && monoRemapElem)
        {
            std::string remapType = "";
            parseString(getFirstAndOnly(monoRemapElem, "RemapType"), remapType);

            std::unique_ptr<LUT> remapLUT;
            XMLElem remapLUTElem = getOptional(monoRemapElem, "RemapLUT");
            if (remapLUTElem)
            {
                remapLUT = parseSingleLUT(remapLUTElem);
            }

            std::unique_ptr<MonochromeDisplayRemap> monoRemap(
                    new MonochromeDisplayRemap(remapType, remapLUT.release()));

            // optional
            common().parseParameters(monoRemapElem, "RemapParameter",
                                     monoRemap->remapParameters);

            return monoRemap.release();
        }
        else if (!colorRemapElem && !monoRemapElem)
        {
            return nullptr;
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
        return nullptr;
    }
}

std::unique_ptr<LUT> DerivedXMLParser::parseSingleLUT(const XMLElem elem) const
{
    //get size attribute
    int size = str::toType<int>(elem->attribute("size"));

    std::string lutStr = "";
    parseString(elem, lutStr);
    std::vector<std::string> lutVals = str::split(lutStr, " ");
    std::unique_ptr<LUT> lut(new LUT(size, sizeof(short)));

    for (size_t ii = 0; ii < lutVals.size(); ++ii)
    {
        const short lutVal = str::toType<short>(lutVals[ii]);
        ::memcpy(&(lut->table[ii * lut->elementSize]),
                 &lutVal, sizeof(short));
    }
    return lut;
}

void DerivedXMLParser::parseDisplayFromXML(
        const XMLElem displayElem,
        Display* display) const
{
    display->pixelType
            = six::toType<PixelType>(getFirstAndOnly(
                    displayElem, "PixelType")->getCharacterData());

    // optional
    display->remapInformation.reset(parseRemapChoiceFromXML(
            getOptional(displayElem, "RemapInformation")));

    //optional
    XMLElem magElem = getOptional(displayElem, "MagnificationMethod");
    display->magnificationMethod = magElem ?
            six::toType<MagnificationMethod>(magElem->getCharacterData())
            : MagnificationMethod::NOT_SET;

    //optional
    XMLElem decElem = getOptional(displayElem, "DecimationMethod");
    display->decimationMethod = decElem ?
            six::toType<DecimationMethod>(decElem->getCharacterData())
           : DecimationMethod::NOT_SET;

    // optional
    XMLElem histogramOverridesElem = getOptional(displayElem,
                                                "DRAHistogramOverrides");
    if (histogramOverridesElem)
    {
        display->histogramOverrides.reset(new DRAHistogramOverrides());
        parseInt(getFirstAndOnly(histogramOverridesElem, "ClipMin"),
                 display->histogramOverrides->clipMin);
        parseInt(getFirstAndOnly(histogramOverridesElem, "ClipMax"),
                 display->histogramOverrides->clipMax);
    }

    // optional
    XMLElem monitorCompensationElem = getOptional(displayElem,
                                                 "MonitorCompensationApplied");
    if (monitorCompensationElem)
    {
        display->monitorCompensationApplied.reset(
                new MonitorCompensationApplied());
        parseDouble(getFirstAndOnly(monitorCompensationElem, "Gamma"),
                    display->monitorCompensationApplied->gamma);
        parseDouble(getFirstAndOnly(monitorCompensationElem, "XMin"),
                    display->monitorCompensationApplied->xMin);
    }

    // optional
    common().parseParameters(displayElem, "DisplayExtension",
                             display->displayExtensions);
}

XMLElem DerivedXMLParser::parsePolynomialProjection(
        XMLElem measurementElem,
        Measurement& measurement) const
{
    XMLElem projElem = getFirstAndOnly(measurementElem, "PolynomialProjection");

    PolynomialProjection* polyProj
        = (PolynomialProjection*)measurement.projection.get();

    // Get a bunch of 2D polynomials
    common().parsePoly2D(getFirstAndOnly(projElem, "RowColToLat"),
        polyProj->rowColToLat);
    common().parsePoly2D(getFirstAndOnly(projElem, "RowColToLon"),
        polyProj->rowColToLon);

    XMLElem optionalAltPolyElem = getOptional(projElem, "RowColToAlt");
    if (optionalAltPolyElem)
    {
        common().parsePoly2D(optionalAltPolyElem, polyProj->rowColToAlt);
    }

    common().parsePoly2D(getFirstAndOnly(projElem, "LatLonToRow"),
        polyProj->latLonToRow);
    common().parsePoly2D(getFirstAndOnly(projElem, "LatLonToCol"),
        polyProj->latLonToCol);

    return projElem;
}

XMLElem DerivedXMLParser::parseGeographicProjection(
        XMLElem measurementElem,
        Measurement& measurement) const
{
    XMLElem projElem = getFirstAndOnly(measurementElem, "GeographicProjection");

    GeographicProjection* geographicProj
        = (GeographicProjection*)measurement.projection.get();

    // measureableProjectionType
    common().parseRowColDouble(getFirstAndOnly(projElem, "SampleSpacing"),
        geographicProj->sampleSpacing);
    common().parsePoly2D(getFirstAndOnly(projElem, "TimeCOAPoly"),
        geographicProj->timeCOAPoly);

    return projElem;
}

XMLElem DerivedXMLParser::parsePlaneProjection(
        XMLElem measurementElem,
    Measurement& measurement) const
{
    XMLElem projElem = getFirstAndOnly(measurementElem, "PlaneProjection");

    PlaneProjection* planeProj
        = (PlaneProjection*)measurement.projection.get();

    // measureableProjectionType
    common().parseRowColDouble(getFirstAndOnly(projElem, "SampleSpacing"),
        planeProj->sampleSpacing);
    common().parsePoly2D(getFirstAndOnly(projElem, "TimeCOAPoly"),
        planeProj->timeCOAPoly);

    // productPlaneType
    XMLElem prodPlaneElem = getFirstAndOnly(projElem, "ProductPlane");
    common().parseVector3D(getFirstAndOnly(prodPlaneElem, "RowUnitVector"),
        planeProj->productPlane.rowUnitVector);
    common().parseVector3D(getFirstAndOnly(prodPlaneElem, "ColUnitVector"),
        planeProj->productPlane.colUnitVector);

    return projElem;
}

XMLElem DerivedXMLParser::parseCylindricalProjection(
    XMLElem measurementElem,
    Measurement& measurement) const
{
    XMLElem projElem = getFirstAndOnly(measurementElem, "CylindricalProjection");

    CylindricalProjection* cylindricalProj
        = (CylindricalProjection*)measurement.projection.get();

    // measureableProjectionType
    common().parseRowColDouble(getFirstAndOnly(projElem, "SampleSpacing"),
        cylindricalProj->sampleSpacing);
    common().parsePoly2D(getFirstAndOnly(projElem, "TimeCOAPoly"),
        cylindricalProj->timeCOAPoly);
    common().parseVector3D(getFirstAndOnly(projElem, "StripmapDirection"),
        cylindricalProj->stripmapDirection);
    // optional
    XMLElem curvRadiusElem = getOptional(projElem, "CurvatureRadius");
    if (curvRadiusElem)
    {
        parseDouble(curvRadiusElem, cylindricalProj->curvatureRadius);
    }

    return projElem;
}

// This function ASSUMES that the measurement projection has already been set!
void DerivedXMLParser::parseMeasurementFromXML(
        const XMLElem measurementElem,
        Measurement* measurement) const
{
    XMLElem projElem = nullptr;
    //  choice: ProjectionType --
    //  this is first parsed in fromXML()
    if (measurement->projection->projectionType == ProjectionType::POLYNOMIAL)
    {
        projElem = parsePolynomialProjection(measurementElem, *measurement);
    }
    else if (measurement->projection->projectionType
            == ProjectionType::GEOGRAPHIC)
    {
        projElem = parseGeographicProjection(measurementElem, *measurement);
    }
    else if (measurement->projection->projectionType == ProjectionType::PLANE)
    {
        projElem = parsePlaneProjection(measurementElem, *measurement);
    }
    else if (measurement->projection->projectionType
            == ProjectionType::CYLINDRICAL)
    {
        projElem = parseCylindricalProjection(measurementElem, *measurement);
    }
    else
    {
        throw except::Exception(Ctxt("Unknown projection type"));
    }

    //  referencePointType --
    //  this is present in all projections types
    XMLElem refElem = getFirstAndOnly(projElem, "ReferencePoint");
    getAttributeIfExists(refElem->getAttributes(), "name",
                         measurement->projection->referencePoint.name);
    common().parseVector3D(getFirstAndOnly(refElem, "ECEF"),
                           measurement->projection->referencePoint.ecef);
    common().parseRowColDouble(getFirstAndOnly(refElem, "Point"),
                               measurement->projection->referencePoint.rowCol);
    common().parseRowColInt(getFirstAndOnly(measurementElem, "PixelFootprint"),
                            measurement->pixelFootprint);
    common().parsePolyXYZ(getFirstAndOnly(measurementElem, "ARPPoly"),
                          measurement->arpPoly);
}

void DerivedXMLParser::parseExploitationFeaturesFromXML(
        const XMLElem exploitationFeaturesElem,
        ExploitationFeatures* exploitationFeatures) const
{
    XMLElem tmpElem;

    std::vector<XMLElem> collectionsElem;
    exploitationFeaturesElem->getElementsByTagName("Collection", collectionsElem);

    exploitationFeatures->collections.resize(collectionsElem.size());
    for (size_t i = 0; i < collectionsElem.size(); ++i)
    {
        XMLElem collectionElem = collectionsElem[i];

        // At least one was created at construction, so check for nullptr
        if (!exploitationFeatures->collections[i].get())
            exploitationFeatures->collections[i].reset(new Collection());
        Collection* coll = exploitationFeatures->collections[i].get();

        coll->identifier
                = collectionElem->getAttributes().getValue("identifier");

        // parse Information
        Information* info = &coll->information;
        XMLElem informationElem = getFirstAndOnly(collectionElem, "Information");

        parseString(getFirstAndOnly(informationElem, "SensorName"),
                    info->sensorName);

        XMLElem radarModeElem = getFirstAndOnly(informationElem, "RadarMode");
        info->radarMode = six::toType<RadarModeType>(
            getFirstAndOnly(radarModeElem, "ModeType")->getCharacterData());
        tmpElem = getOptional(radarModeElem, "ModeID");
        if (tmpElem)
        {
            parseString(tmpElem, info->radarModeID);
        }

        parseDateTime(getFirstAndOnly(informationElem, "CollectionDateTime"),
            info->collectionDateTime);

        // optional
        tmpElem = getOptional(informationElem, "LocalDateTime");
        if (tmpElem)
            parseDateTime(tmpElem, info->localDateTime);

        parseDouble(getFirstAndOnly(informationElem, "CollectionDuration"),
            info->collectionDuration);

        // optional
        XMLElem resElem = getOptional(informationElem, "Resolution");
        if (resElem)
            common().parseRangeAzimuth(resElem, info->resolution);

        // optional
        XMLElem roiElem = getOptional(informationElem, "InputROI");
        if (roiElem)
        {
            info->inputROI.reset(new InputROI());

            common().parseRowColInt(getFirstAndOnly(roiElem, "Size"),
                info->inputROI->size);
            common().parseRowColInt(getFirstAndOnly(roiElem, "UpperLeft"),
                info->inputROI->upperLeft);
        }

        // optional and unbounded
        std::vector<XMLElem> polarization;
        informationElem->getElementsByTagName("Polarization", polarization);
        info->polarization.resize(polarization.size());
        for (size_t jj = 0, nElems = polarization.size(); jj < nElems; ++jj)
        {
            XMLElem polElem = polarization[jj];
            info->polarization[jj].reset(new TxRcvPolarization());
            TxRcvPolarization* p = info->polarization[jj].get();

            p->txPolarization = six::toType<PolarizationType>(
                getFirstAndOnly(polElem, "TxPolarization")->
                getCharacterData());
            p->rcvPolarization = six::toType<PolarizationType>(
                getFirstAndOnly(polElem, "RcvPolarization")->
                getCharacterData());

            // optional
            tmpElem = getOptional(polElem, "RcvPolarizationOffset");
            if (tmpElem)
                parseDouble(tmpElem, p->rcvPolarizationOffset);

            // optional
            tmpElem = getOptional(polElem, "Processed");
            if (tmpElem)
            {
                parseBooleanType(tmpElem, p->processed);
            }
        }

        // parse Geometry -- optional
        XMLElem geometryElem = getOptional(collectionElem, "Geometry");
        if (geometryElem)
        {
            coll->geometry.reset(new Geometry());

            // optional
            tmpElem = getOptional(geometryElem, "Azimuth");
            if (tmpElem)
                parseDouble(tmpElem, coll->geometry->azimuth);

            // optional
            tmpElem = getOptional(geometryElem, "Slope");
            if (tmpElem)
                parseDouble(tmpElem, coll->geometry->slope);

            // optional
            tmpElem = getOptional(geometryElem, "Squint");
            if (tmpElem)
                parseDouble(tmpElem, coll->geometry->squint);

            // optional
            tmpElem = getOptional(geometryElem, "Graze");
            if (tmpElem)
                parseDouble(tmpElem, coll->geometry->graze);

            // optional
            tmpElem = getOptional(geometryElem, "Tilt");
            if (tmpElem)
                parseDouble(tmpElem, coll->geometry->tilt);

            // optional to unbounded
            common().parseParameters(geometryElem, "Extension",
                coll->geometry->extensions);
        }

        // parse Phenomenology -- optional
        XMLElem phenomenologyElem = getOptional(collectionElem, "Phenomenology");
        if (phenomenologyElem)
        {
            coll->phenomenology.reset(new Phenomenology());

            // optional
            tmpElem = getOptional(phenomenologyElem, "Shadow");
            if (tmpElem)
            {
                parseDouble(getFirstAndOnly(tmpElem, "Angle"),
                    coll->phenomenology->shadow.angle);
                parseDouble(getFirstAndOnly(tmpElem, "Magnitude"),
                    coll->phenomenology->shadow.magnitude);
            }

            // optional
            tmpElem = getOptional(phenomenologyElem, "Layover");
            if (tmpElem)
            {
                parseDouble(getFirstAndOnly(tmpElem, "Angle"),
                    coll->phenomenology->layover.angle);
                parseDouble(getFirstAndOnly(tmpElem, "Magnitude"),
                    coll->phenomenology->layover.magnitude);
            }

            // optional
            tmpElem = getOptional(phenomenologyElem, "MultiPath");
            if (tmpElem)
            {
                parseDouble(tmpElem, coll->phenomenology->multiPath);
            }

            // optional
            tmpElem = getOptional(phenomenologyElem, "GroundTrack");
            if (tmpElem)
            {
                parseDouble(tmpElem, coll->phenomenology->groundTrack);
            }

            // optional to unbounded
            common().parseParameters(phenomenologyElem, "Extension",
                coll->phenomenology->extensions);
        }
    }

    parseProductFromXML(exploitationFeaturesElem, exploitationFeatures);
}

XMLElem DerivedXMLParser::convertProcessorInformationToXML(
        const ProcessorInformation* processorInformation,
        XMLElem parent) const
{
    XMLElem procInfoElem
            = newElement("ProcessorInformation", parent);

    createString("Application",
                 processorInformation->application,
                 procInfoElem);

    createDateTime("ProcessingDateTime",
                   processorInformation->processingDateTime,
                   procInfoElem);

    createString("Site", processorInformation->site,
                 procInfoElem);

    // optional
    if (processorInformation->profile != Init::undefined<
            std::string>())
    {
        createString("Profile", processorInformation->profile,
                     procInfoElem);
    }

    return procInfoElem;
}

XMLElem DerivedXMLParser::convertProductCreationToXML(
        const ProductCreation* productCreation,
        XMLElem parent) const
{
    // create ProductCreation -- root
    XMLElem productCreationElem = newElement("ProductCreation", parent);

    convertProcessorInformationToXML(
            &productCreation->processorInformation, productCreationElem);

    convertDerivedClassificationToXML(
            productCreation->classification, productCreationElem);

    createString("ProductName", productCreation->productName,
                 productCreationElem);

    createString("ProductClass", productCreation->productClass,
                 productCreationElem);

    // optional
    if (productCreation->productType != Init::undefined<std::string>())
    {
        createString("ProductType", productCreation->productType,
                     productCreationElem);
    }

    // optional to unbounded
    common().addParameters("ProductCreationExtension",
                           productCreation->productCreationExtensions,
                           productCreationElem);

    return productCreationElem;
}

void DerivedXMLParser::convertRemapToXML(const Remap& remap,
                                         XMLElem parent) const
{
    if (remap.displayType == DisplayType::COLOR)
    {
        XMLElem remapElem = newElement("ColorDisplayRemap", parent);
        if (remap.remapLUT.get())
            createLUT("RemapLUT",
                      remap.remapLUT.get(), remapElem);
    }
    else if (remap.displayType == DisplayType::MONO)
    {
        XMLElem remapElem = newElement("MonochromeDisplayRemap",
                                      parent);
        // a little risky, but let's assume the displayType is correct
        const MonochromeDisplayRemap& mdr =
                static_cast<const MonochromeDisplayRemap&>(remap);
        createString("RemapType", mdr.remapType, remapElem);
    /* TODO: Where does this actually go??
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
        */
        if (mdr.remapLUT.get())
        {
            createLUT("RemapLUT", mdr.remapLUT.get(), remapElem);
        }

        common().addParameters("RemapParameter", mdr.remapParameters,
                               remapElem);
    }
}

XMLElem DerivedXMLParser::convertGeographicCoverageToXML(
        const std::string& localName,
        const GeographicCoverage* geoCoverage,
        XMLElem parent) const
{
    //GeographicAndTarget
    XMLElem geoCoverageElem = newElement(localName, parent);

    // optional to unbounded
    common().addParameters("GeoregionIdentifier", geoCoverage->georegionIdentifiers,
                  geoCoverageElem);

    createFootprint("Footprint", "Vertex", geoCoverage->footprint,
                    geoCoverageElem);

    // GeographicInfo
    if (geoCoverage->geographicInformation.get())
    {
        XMLElem geoInfoElem = newElement("GeographicInfo", geoCoverageElem);

        // optional to unbounded
        size_t numCC = geoCoverage->geographicInformation->countryCodes.size();
        for (size_t i = 0; i < numCC; ++i)
        {
            createString("CountryCode",
                         geoCoverage->geographicInformation->countryCodes[i],
                         geoInfoElem);
        }

        // optional
        str::trim(geoCoverage->geographicInformation->securityInformation);
        std::string secInfo
                = geoCoverage->geographicInformation->securityInformation;
        if (!secInfo.empty())
            createString("SecurityInfo", secInfo, geoInfoElem);

        // optional to unbounded
        common().addParameters("GeographicInfoExtension", geoCoverage->
                                    geographicInformation->
                                    geographicInformationExtensions,
                               geoInfoElem);
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
                                           geoCoverageElem);
        }
    }

    return geoCoverageElem;
}

XMLElem DerivedXMLParser::convertMeasurementToXML(
        const Measurement* measurement,
        XMLElem parent) const
{
    XMLElem measurementElem = newElement("Measurement", parent);

    XMLElem projectionElem = newElement("", measurementElem);

    // NOTE: ReferencePoint is present in all of the ProjectionTypes
    //       so its added here for ease
    XMLElem referencePointElem = newElement("ReferencePoint", projectionElem);
    if (measurement->projection->referencePoint.name
            != Init::undefined<std::string>())
    {
        setAttribute(referencePointElem, "name",
                    measurement->projection->referencePoint.name);
    }
    common().createVector3D("ECEF", common().getSICommonURI(),
                            measurement->projection->referencePoint.ecef,
                            referencePointElem);
    common().createRowCol("Point", common().getSICommonURI(),
                          measurement->projection->referencePoint.rowCol,
                          referencePointElem);

    switch (measurement->projection->projectionType)
    {
    case ProjectionType::POLYNOMIAL:
    {
        projectionElem->setLocalName("PolynomialProjection");

        PolynomialProjection* polyProj
                = (PolynomialProjection*) measurement->projection.get();

        common().createPoly2D("RowColToLat", polyProj->rowColToLat, projectionElem);
        common().createPoly2D("RowColToLon", polyProj->rowColToLon, projectionElem);

        // optional
        if (polyProj->rowColToAlt != Init::undefined<Poly2D>())
        {
            common().createPoly2D("RowColToAlt", polyProj->rowColToAlt, projectionElem);
        }

        common().createPoly2D("LatLonToRow", polyProj->latLonToRow, projectionElem);
        common().createPoly2D("LatLonToCol", polyProj->latLonToCol, projectionElem);
    }
        break;

    case ProjectionType::GEOGRAPHIC:
    {
        projectionElem->setLocalName("GeographicProjection");

        GeographicProjection* geographicProj
                = (GeographicProjection*) measurement->projection.get();

        common().createRowCol("SampleSpacing",
                              geographicProj->sampleSpacing,
                              projectionElem);
        common().createPoly2D("TimeCOAPoly",
                              geographicProj->timeCOAPoly,
                              projectionElem);
    }
        break;

    case ProjectionType::PLANE:
    {
        projectionElem->setLocalName("PlaneProjection");

        PlaneProjection* planeProj
                = (PlaneProjection*) measurement->projection.get();

        common().createRowCol("SampleSpacing",
                              planeProj->sampleSpacing,
                              projectionElem);
        common().createPoly2D("TimeCOAPoly",
                              planeProj->timeCOAPoly,
                              projectionElem);

        XMLElem productPlaneElem = newElement("ProductPlane", projectionElem);
        common().createVector3D("RowUnitVector",
                                planeProj->productPlane.rowUnitVector,
                                productPlaneElem);
        common().createVector3D("ColUnitVector",
                                planeProj->productPlane.colUnitVector,
                                productPlaneElem);
    }
        break;

    case ProjectionType::CYLINDRICAL:
    {
        projectionElem->setLocalName("CylindricalProjection");

        CylindricalProjection* cylindricalProj
                = (CylindricalProjection*) measurement->projection.get();

        common().createRowCol("SampleSpacing",
                              cylindricalProj->sampleSpacing,
                              projectionElem);
        common().createPoly2D("TimeCOAPoly",
                              cylindricalProj->timeCOAPoly,
                              projectionElem);
        common().createVector3D("StripmapDirection",
                                cylindricalProj->stripmapDirection,
                                projectionElem);
        // optional
        if (cylindricalProj->curvatureRadius != Init::undefined<double>())
        {
            createDouble("CurvatureRadius",
                         cylindricalProj->curvatureRadius,
                         projectionElem);
        }
    }
        break;

    default:
        throw except::Exception(Ctxt("Unknown projection type!"));
    }

    common().createRowCol("PixelFootprint",
                          measurement->pixelFootprint,
                          measurementElem);

    return measurementElem;
}

XMLElem DerivedXMLParser::createLUT(const std::string& name, const LUT *lut,
        XMLElem parent) const
{
    XMLElem lutElement = newElement(name, parent);
    setAttribute(lutElement, "size", lut->numEntries);
    return createLUTImpl(lut, lutElement);
}

XMLElem DerivedXMLParser::createLUTImpl(const LUT *lut, XMLElem lutElem) const
{
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
            oss << (unsigned int)(*lut)[i][0] << ','
                    << (unsigned int)(*lut)[i][1] << ','
                    << (unsigned int)(*lut)[i][2];
        }
        else
        {
            std::ostringstream ostr;
            ostr << "Invalid element size [" << lut->elementSize << "]";
            throw except::Exception(Ctxt(ostr.str()));
        }
        if ((lut->numEntries - 1) != i)
            oss << ' ';
    }
    lutElem->setCharacterData(oss.str());
    return lutElem;
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

XMLElem DerivedXMLParser::createSFADatum(const std::string& name,
                                         const six::sidd::SFADatum& datum,
                                         XMLElem parent) const
{
    XMLElem datumElem = newElement(name, SFA_URI, parent);

    XMLElem spheriodElem = newElement("Spheroid", SFA_URI, datumElem);

    createString("SpheriodName", SFA_URI, datum.spheroid.name, spheriodElem);
    createDouble("SemiMajorAxis", SFA_URI,
                 datum.spheroid.semiMajorAxis, spheriodElem);
    createDouble("InverseFlattening", SFA_URI,
                 datum.spheroid.inverseFlattening, spheriodElem);

    return datumElem;
}

XMLElem DerivedXMLParser::convertProductProcessingToXML(
        const ProductProcessing* productProcessing,
        XMLElem parent) const
{
    XMLElem productProcessingElem = newElement("ProductProcessing", parent);

    // error checking
    if (productProcessing->processingModules.size() < 1)
    {
        throw except::Exception(Ctxt(FmtX(
                "There must be at least [1] ProcessingModule in "\
                "ProductProcessing, [%d] found",
                productProcessing->processingModules.size())));
    }

    // one to unbounded
    for (std::vector<mem::ScopedCopyablePtr<ProcessingModule> >::
            const_iterator it = productProcessing->processingModules.begin();
            it != productProcessing->processingModules.end(); ++it)
    {
        convertProcessingModuleToXML((*it).get(), productProcessingElem);
    }

    return productProcessingElem;
}

XMLElem DerivedXMLParser::convertProcessingModuleToXML(
        const ProcessingModule* procMod,
        XMLElem parent) const
{
    XMLElem procModElem = newElement("ProcessingModule", parent);

    common().createParameter("ModuleName", procMod->moduleName, procModElem);

    // optional choice
    if (!procMod->processingModules.empty())
    {
        // one to unbounded
        for (std::vector<mem::ScopedCopyablePtr<ProcessingModule> >::
                const_iterator it = procMod->processingModules.begin();
                it != procMod->processingModules.end(); ++it)
        {
            convertProcessingModuleToXML((*it).get(), procModElem);
        }
    }
    else if (!procMod->moduleParameters.empty())
    {
        common().addParameters("ModuleParameter",
                               procMod->moduleParameters,
                               procModElem);
    }

    return procModElem;
}

XMLElem DerivedXMLParser::convertDownstreamReprocessingToXML(
        const DownstreamReprocessing* downstreamReproc,
        XMLElem parent) const
{
    XMLElem epElem = newElement("DownstreamReprocessing", parent);

    // optional
    GeometricChip *geoChip = downstreamReproc->geometricChip.get();
    if (geoChip)
    {
        XMLElem geoChipElem = newElement("GeometricChip", epElem);
        common().createRowCol("ChipSize", geoChip->chipSize, geoChipElem);
        common().createRowCol("OriginalUpperLeftCoordinate",
                     geoChip->originalUpperLeftCoordinate, geoChipElem);
        common().createRowCol("OriginalUpperRightCoordinate",
                     geoChip->originalUpperRightCoordinate, geoChipElem);
        common().createRowCol("OriginalLowerLeftCoordinate",
                     geoChip->originalLowerLeftCoordinate, geoChipElem);
        common().createRowCol("OriginalLowerRightCoordinate",
                     geoChip->originalLowerRightCoordinate, geoChipElem);
    }
    // optional to unbounded
    if (!downstreamReproc->processingEvents.empty())
    {
        for (std::vector<mem::ScopedCopyablePtr<ProcessingEvent> >::
                const_iterator it = downstreamReproc->processingEvents.begin();
                it != downstreamReproc->processingEvents.end(); ++it)
        {
            ProcessingEvent *procEvent = (*it).get();
            XMLElem procEventElem = newElement("ProcessingEvent", epElem);

            createString("ApplicationName", procEvent->applicationName,
                         procEventElem);
            createDateTime("AppliedDateTime", procEvent->appliedDateTime,
                           procEventElem);
            // optional
            if (!procEvent->interpolationMethod.empty())
            {
                createString("InterpolationMethod",
                             procEvent->interpolationMethod, procEventElem);
            }
            // optional to unbounded
            common().addParameters("Descriptor", procEvent->descriptor, procEventElem);
        }
    }
    return epElem;
}

void DerivedXMLParser::parseProcessingModuleFromXML(
        const XMLElem procElem,
        ProcessingModule* procMod) const
{
    common().parseParameter(getFirstAndOnly(procElem, "ModuleName"),
                            procMod->moduleName);

    common().parseParameters(procElem, "ModuleParameter", procMod->moduleParameters);

    std::vector<XMLElem> procModuleElem;
    procElem->getElementsByTagName("ProcessingModule", procModuleElem);
    procMod->processingModules.resize(procModuleElem.size());
    for (size_t i = 0, size = procModuleElem.size(); i < size; ++i)
    {
        procMod->processingModules[i].reset(new ProcessingModule());
        parseProcessingModuleFromXML(
                procModuleElem[i], procMod->processingModules[i].get());
    }
}

void DerivedXMLParser::parseProductProcessingFromXML(
        const XMLElem elem,
        ProductProcessing* productProcessing) const
{
    std::vector<XMLElem> procModuleElem;
    elem->getElementsByTagName("ProcessingModule", procModuleElem);
    productProcessing->processingModules.resize(procModuleElem.size());
    for (size_t i = 0, size = procModuleElem.size(); i < size; ++i)
    {
        productProcessing->processingModules[i].reset(new ProcessingModule());
        parseProcessingModuleFromXML(
                procModuleElem[i],
                productProcessing->processingModules[i].get());
    }
}

void DerivedXMLParser::parseDownstreamReprocessingFromXML(
        const XMLElem elem,
        DownstreamReprocessing* downstreamReproc) const
{
    XMLElem geometricChipElem = getOptional(elem, "GeometricChip");
    if (geometricChipElem)
    {
        downstreamReproc->geometricChip.reset(new GeometricChip());
        GeometricChip *chip = downstreamReproc->geometricChip.get();

        common().parseRowColInt(getFirstAndOnly(geometricChipElem, "ChipSize"),
                                chip->chipSize);
        common().parseRowColDouble(getFirstAndOnly(geometricChipElem,
                                   "OriginalUpperLeftCoordinate"),
                                   chip->originalUpperLeftCoordinate);
        common().parseRowColDouble(getFirstAndOnly(geometricChipElem,
                                   "OriginalUpperRightCoordinate"),
                                   chip->originalUpperRightCoordinate);
        common().parseRowColDouble(getFirstAndOnly(geometricChipElem,
                                   "OriginalLowerLeftCoordinate"),
                                   chip->originalLowerLeftCoordinate);
        common().parseRowColDouble(getFirstAndOnly(geometricChipElem,
                                   "OriginalLowerRightCoordinate"),
                                   chip->originalLowerRightCoordinate);
    }

    std::vector<XMLElem> procEventElem;
    elem->getElementsByTagName("ProcessingEvent", procEventElem);
    downstreamReproc->processingEvents.resize(procEventElem.size());
    for (size_t i = 0, size = procEventElem.size(); i < size; ++i)
    {
        downstreamReproc->processingEvents[i].reset(new ProcessingEvent());
        ProcessingEvent* procEvent
                = downstreamReproc->processingEvents[i].get();

        XMLElem peElem = procEventElem[i];
        parseString(getFirstAndOnly(peElem, "ApplicationName"),
                    procEvent->applicationName);
        parseDateTime(getFirstAndOnly(peElem, "AppliedDateTime"),
                      procEvent->appliedDateTime);

        // optional
        XMLElem tmpElem = getOptional(peElem, "InterpolationMethod");
        if (tmpElem)
        {
            parseString(tmpElem, procEvent->interpolationMethod);
        }

        // optional to unbounded
        common().parseParameters(peElem, "Descriptor", procEvent->descriptor);
    }
}

void DerivedXMLParser::parseGeographicCoordinateSystemFromXML(
        const XMLElem coorSysElem,
        SFAGeographicCoordinateSystem* coordSys) const
{
    parseString(getFirstAndOnly(coorSysElem, "Csname"), coordSys->csName);
    parseDatum(getFirstAndOnly(coorSysElem, "Datum"), coordSys->datum);

    XMLElem primeElem = getFirstAndOnly(coorSysElem, "PrimeMeridian");
    parseString(getFirstAndOnly(primeElem, "Name"),
            coordSys->primeMeridian.name);
    parseDouble(getFirstAndOnly(primeElem, "Longitude"),
            coordSys->primeMeridian.longitude);

    parseString(getFirstAndOnly(coorSysElem, "AngularUnit"),
                coordSys->angularUnit);

    // optional
    XMLElem luElem = getOptional(coorSysElem, "LinearUnit");
    if (luElem)
        parseString(luElem, coordSys->linearUnit);
}

void DerivedXMLParser::parseAnnotationFromXML(
        const XMLElem elem,
        Annotation *a) const
{
    parseString(getFirstAndOnly(elem, "Identifier"), a->identifier);

    XMLElem spatialElem = getOptional(elem, "SpatialReferenceSystem");
    if (spatialElem)
    {
        a->spatialReferenceSystem.reset(new six::sidd::SFAReferenceSystem());

        // choice
        XMLElem tmpElem = getOptional(spatialElem,
                                     "ProjectedCoordinateSystem");
        if (tmpElem)
        {
            a->spatialReferenceSystem->coordinateSystem.reset(
                    new SFAProjectedCoordinateSystem());

            SFAProjectedCoordinateSystem* coordSys =
                    (SFAProjectedCoordinateSystem*)
                            a->spatialReferenceSystem->coordinateSystem.get();

            parseString(getFirstAndOnly(tmpElem, "Csname"), coordSys->csName);

            coordSys->geographicCoordinateSystem.reset(
                    new SFAGeographicCoordinateSystem());
            parseGeographicCoordinateSystemFromXML(
                    getFirstAndOnly(tmpElem, "GeographicCoordinateSystem"),
                    coordSys->geographicCoordinateSystem.get());

            XMLElem projElem = getFirstAndOnly(tmpElem, "Projection");
            parseString(getFirstAndOnly(projElem, "ProjectionName"),
                        coordSys->projection.name);

            XMLElem paramElem = getOptional(tmpElem, "Parameter");
            if (paramElem)
            {
                parseString(getFirstAndOnly(paramElem, "ParameterName"),
                        coordSys->parameter.name);
                parseDouble(getFirstAndOnly(paramElem, "Value"),
                        coordSys->parameter.value);
            }

            parseString(getFirstAndOnly(tmpElem, "LinearUnit"),
                        coordSys->linearUnit);
        }

        tmpElem = getOptional(spatialElem, "GeographicCoordinateSystem");
        if (tmpElem)
        {
            a->spatialReferenceSystem->coordinateSystem.reset(
                    new SFAGeographicCoordinateSystem());

            SFAGeographicCoordinateSystem* coordSys =
                    (SFAGeographicCoordinateSystem*)
                            a->spatialReferenceSystem->coordinateSystem.get();

            parseGeographicCoordinateSystemFromXML(tmpElem, coordSys);
        }

        tmpElem = getOptional(spatialElem, "GeocentricCoordinateSystem");
        if (tmpElem)
        {
            a->spatialReferenceSystem->coordinateSystem.reset(
                    new SFAGeocentricCoordinateSystem());

            SFAGeocentricCoordinateSystem* coordSys =
                    (SFAGeocentricCoordinateSystem*)
                            a->spatialReferenceSystem->coordinateSystem.get();

            parseString(getFirstAndOnly(tmpElem, "Csname"), coordSys->csName);
            parseDatum(getFirstAndOnly(tmpElem, "Datum"), coordSys->datum);

            XMLElem primeElem = getFirstAndOnly(tmpElem, "PrimeMeridian");
            parseString(getFirstAndOnly(primeElem, "Name"),
                    coordSys->primeMeridian.name);
            parseDouble(getFirstAndOnly(primeElem, "Longitude"),
                    coordSys->primeMeridian.longitude);

            parseString(getFirstAndOnly(tmpElem, "LinearUnit"),
                        coordSys->linearUnit);
        }

        // 1 to unbounded
        std::vector<XMLElem> axixNamesElem;
        spatialElem->getElementsByTagName("AxisName", axixNamesElem);
        for (size_t i = 0; i < axixNamesElem.size(); ++i)
        {
            std::string axisName = "";
            parseString(axixNamesElem[i], axisName);
            a->spatialReferenceSystem->axisNames.push_back(axisName);
        }
    }
    else
    {
        a->spatialReferenceSystem.reset();
    }

    std::vector<XMLElem> objectsElem;
    elem->getElementsByTagName("Object", objectsElem);
    a->objects.resize(objectsElem.size());
    for (size_t i = 0, size = objectsElem.size(); i < size; ++i)
    {
        XMLElem obj = objectsElem[i];

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

void DerivedXMLParser::parseDatum(const XMLElem datumElem, SFADatum& datum) const
{
    XMLElem spheroidElem = getFirstAndOnly(datumElem, "Spheroid");

    parseString(getFirstAndOnly(spheroidElem, "SpheriodName"),
                datum.spheroid.name);
    parseDouble(getFirstAndOnly(spheroidElem, "SemiMajorAxis"),
                datum.spheroid.semiMajorAxis);
    parseDouble(getFirstAndOnly(spheroidElem, "InverseFlattening"),
                datum.spheroid.inverseFlattening);
}

XMLElem DerivedXMLParser::convertGeographicCoordinateSystemToXML(
        const SFAGeographicCoordinateSystem* geographicCoordinateSystem,
        XMLElem parent) const
{
    XMLElem geoSysElem = newElement("GeographicCoordinateSystem",
                                   SFA_URI, parent);

    createString("Csname", SFA_URI,
                 geographicCoordinateSystem->csName, geoSysElem);
    createSFADatum("Datum", geographicCoordinateSystem->datum, geoSysElem);

    XMLElem primeMeridianElem = newElement("PrimeMeridian", SFA_URI, geoSysElem);
    createString("Name", SFA_URI,
                 geographicCoordinateSystem->primeMeridian.name,
                 primeMeridianElem);
    createDouble("Longitude", SFA_URI,
                 geographicCoordinateSystem-> primeMeridian.longitude,
                 primeMeridianElem);

    createString("AngularUnit", SFA_URI,
                 geographicCoordinateSystem->angularUnit, geoSysElem);
    createString("LinearUnit", SFA_URI, geographicCoordinateSystem->linearUnit,
                 geoSysElem);

    return geoSysElem;
}

XMLElem DerivedXMLParser::convertAnnotationToXML(
        const Annotation* a,
        XMLElem parent) const
{
    XMLElem annElem = newElement("Annotation", parent);

    createString("Identifier", a->identifier, annElem);

    // optional
    if (a->spatialReferenceSystem.get())
    {
        XMLElem spRefElem = newElement("SpatialReferenceSystem",
                                      getDefaultURI(), annElem);

        if (a->spatialReferenceSystem->coordinateSystem->getType()
                == six::sidd::SFAProjectedCoordinateSystem::TYPE_NAME)
        {
            XMLElem coordElem = newElement("ProjectedCoordinateSystem", SFA_URI, spRefElem);

            SFAProjectedCoordinateSystem* coordSys
                    = (SFAProjectedCoordinateSystem*)a->
                            spatialReferenceSystem->coordinateSystem.get();

            createString("Csname", SFA_URI, coordSys->csName, coordElem);

            convertGeographicCoordinateSystemToXML(
                    coordSys->geographicCoordinateSystem.get(), coordElem);

            XMLElem projectionElem = newElement("Projection", SFA_URI, coordElem);
            createString("ProjectionName", SFA_URI, coordSys->projection.name,
                         projectionElem);

            // optional
            if (!coordSys->parameter.name.empty())
            {
                XMLElem parameterElem = newElement("Parameter", SFA_URI,
                                                  coordElem);
                createString("ParameterName", SFA_URI,
                             coordSys->parameter.name, parameterElem);
                createDouble("Value", SFA_URI,
                             coordSys->parameter.value, parameterElem);
            }

            createString("LinearUnit", SFA_URI,
                         coordSys->linearUnit, coordElem);
        }
        else if (a->spatialReferenceSystem->coordinateSystem->getType()
                    == six::sidd::SFAGeographicCoordinateSystem::TYPE_NAME)
        {
            SFAGeographicCoordinateSystem* coordSys
                    = (SFAGeographicCoordinateSystem*)a->
                            spatialReferenceSystem->coordinateSystem.get();
            convertGeographicCoordinateSystemToXML(coordSys, spRefElem);
        }
        else if (a->spatialReferenceSystem->coordinateSystem->getType()
                    == six::sidd::SFAGeocentricCoordinateSystem::TYPE_NAME)
        {
            XMLElem coordElem = newElement("GeocentricCoordinateSystem",
                                          SFA_URI, spRefElem);

            SFAGeocentricCoordinateSystem* coordSys
                    = (SFAGeocentricCoordinateSystem*)a->
                            spatialReferenceSystem->coordinateSystem.get();

            createString("Csname", SFA_URI, coordSys->csName, coordElem);
            createSFADatum("Datum", coordSys->datum, coordElem);

            XMLElem primeMeridianElem = newElement("PrimeMeridian", SFA_URI,
                                                  coordElem);
            createString("Name", SFA_URI,
                         coordSys->primeMeridian.name,
                         primeMeridianElem);
            createDouble("Longitude", SFA_URI,
                         coordSys->primeMeridian.longitude,
                         primeMeridianElem);

            createString("LinearUnit", SFA_URI,
                         coordSys->linearUnit, coordElem);
        }

        // one to unbounded
        for(size_t ii = 0; ii < a->spatialReferenceSystem->axisNames.size();
                ++ii)
        {
            createString("AxisName", SFA_URI,
                         a->spatialReferenceSystem->axisNames[ii],
                         spRefElem);
        }
    }

    // one to unbounded
    for (size_t i = 0, num = a->objects.size(); i < num; ++i)
    {
        XMLElem objElem = newElement("Object", annElem);
        convertSFAGeometryToXML(a->objects[i].get(), objElem);
    }
    return annElem;
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
        std::vector<XMLElem> vElem;
        elem->getElementsByTagName("Vertex", vElem);
        p->vertices.resize(vElem.size());
        for (size_t i = 0, size = vElem.size(); i < size; ++i)
        {
            p->vertices[i].reset(new SFAPoint());
            parseSFAGeometryFromXML(vElem[i], p->vertices[i].get());
        }
    }
    else if (geoType == SFAPolygon::TYPE_NAME)
    {
        SFAPolygon* p = (SFAPolygon*) g;
        std::vector<XMLElem> ringElem;
        elem->getElementsByTagName("Ring", ringElem);
        p->rings.resize(ringElem.size());
        for (size_t i = 0, size = ringElem.size(); i < size; ++i)
        {
            p->rings[i].reset(new SFALinearRing());
            parseSFAGeometryFromXML(ringElem[i], p->rings[i].get());
        }
    }
    else if (geoType == SFAPolyhedralSurface::TYPE_NAME)
    {
        SFAPolyhedralSurface* p = (SFAPolyhedralSurface*) g;
        std::vector<XMLElem> polyElem;
        elem->getElementsByTagName("Patch", polyElem);
        p->patches.resize(polyElem.size());
        for (size_t i = 0, size = polyElem.size(); i < size; ++i)
        {
            p->patches[i].reset(new SFAPolygon());
            parseSFAGeometryFromXML(polyElem[i], p->patches[i].get());
        }
    }
    else if (geoType == SFAMultiPolygon::TYPE_NAME)
    {
        SFAMultiPolygon* p = (SFAMultiPolygon*) g;
        std::vector<XMLElem> polyElem;
        elem->getElementsByTagName("Element", polyElem);
        p->elements.resize(polyElem.size());
        for (size_t i = 0, size = polyElem.size(); i < size; ++i)
        {
            p->elements[i].reset(new SFAPolygon());
            parseSFAGeometryFromXML(polyElem[i], p->elements[i].get());
        }
    }
    else if (geoType == SFAMultiLineString::TYPE_NAME)
    {
        SFAMultiLineString* p = (SFAMultiLineString*) g;
        std::vector<XMLElem> lineElem;
        elem->getElementsByTagName("Element", lineElem);
        p->elements.resize(lineElem.size());
        for (size_t i = 0, size = lineElem.size(); i < size; ++i)
        {
            p->elements[i].reset(new SFALineString());
            parseSFAGeometryFromXML(lineElem[i], p->elements[i].get());
        }
    }
    else if (geoType == SFAMultiPoint::TYPE_NAME)
    {
        SFAMultiPoint* p = (SFAMultiPoint*) g;
        std::vector<XMLElem> vElem;
        elem->getElementsByTagName("Vertex", vElem);
        p->vertices.resize(vElem.size());
        for (size_t i = 0, size = vElem.size(); i < size; ++i)
        {
            p->vertices[i].reset(new SFAPoint());
            parseSFAGeometryFromXML(vElem[i], p->vertices[i].get());
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
    XMLElem pointElem
            = newElement(localName,
                         (localName == "Vertex")
                         ? SFA_URI : getDefaultURI(), parent);

    createDouble("X", SFA_URI, p->x, pointElem);
    createDouble("Y", SFA_URI, p->y, pointElem);

    // optional
    if (!Init::isUndefined(p->z))
        createDouble("Z", SFA_URI, p->z, pointElem);
    // optional
    if (!Init::isUndefined(p->m))
        createDouble("M", SFA_URI, p->m, pointElem);

    return pointElem;
}

XMLElem DerivedXMLParser::createSFALine(
        const std::string& localName,
        const SFALineString* l,
        XMLElem parent) const
{
    XMLElem lineElem
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
        createSFAPoint("Vertex", l->vertices[ii].get(), lineElem);
    }

    return lineElem;
}

XMLElem DerivedXMLParser::convertSFAGeometryToXML(
        const SFAGeometry* g,
        XMLElem parent) const
{
    XMLElem geoElem = nullptr;

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
        geoElem = createSFALine(geoType, (SFALineString*) g, parent);
    }
    else if (geoType == SFAPolygon::TYPE_NAME)
    {
        geoElem = newElement("Polygon", getDefaultURI(), parent);

        SFAPolygon* p = (SFAPolygon*) g;

        // one to unbounded
        for (size_t ii = 0; ii < p->rings.size(); ++ii)
        {
            createSFALine("Ring", p->rings[ii].get(), geoElem);
        }
    }
    else if (geoType == SFAPolyhedralSurface::TYPE_NAME)
    {
        geoElem = newElement("PolyhedralSurface", getDefaultURI(), parent);

        SFAPolyhedralSurface* p = (SFAPolyhedralSurface*) g;

        for (size_t ii = 0; ii < p->patches.size(); ++ii)
        {
            XMLElem patchElem = newElement("Patch", SFA_URI, geoElem);
            for (size_t jj = 0; jj < p->patches[ii]->rings.size(); ++jj)
            {
                createSFALine("Ring", p->patches[ii]->rings[jj].get(), patchElem);
            }
        }
    }
    else if (geoType == SFAMultiPolygon::TYPE_NAME)
    {
        geoElem = newElement("MultiPolygon", getDefaultURI(), parent);

        SFAMultiPolygon* p = (SFAMultiPolygon*) g;

        // optional to unbounded
        for (size_t ii = 0; ii < p->elements.size(); ++ii)
        {
            XMLElem elemElem = newElement("Element", SFA_URI, geoElem);
            for (size_t jj = 0; jj < p->elements[ii]->rings.size(); ++jj)
            {
                createSFALine("Ring", p->elements[ii]->rings[jj].get(), elemElem);
            }
        }
    }
    else if (geoType == SFAMultiLineString::TYPE_NAME)
    {
        geoElem = newElement("MultiLineString", getDefaultURI(), parent);

        SFAMultiLineString* p = (SFAMultiLineString*) g;

        // optional to unbounded
        for (size_t ii = 0; ii < p->elements.size(); ++ii)
        {
            XMLElem elemElem = newElement("Element", SFA_URI, geoElem);
            for (size_t jj = 0; jj < p->elements[ii]->vertices.size(); ++jj)
            {
                createSFAPoint("Vertex", p->elements[ii]->vertices[jj].get(),
                               elemElem);
            }
        }
    }
    else if (geoType == SFAMultiPoint::TYPE_NAME)
    {
        geoElem = newElement("MultiPoint", getDefaultURI(), parent);

        SFAMultiPoint* p = (SFAMultiPoint*) g;

        // error check the vertices
        if (p->vertices.size() < 2)
            throw except::Exception(Ctxt(FmtX(
                    "Must be at least two Vertices in LineString. Only [%d] " \
                    "found", p->vertices.size())));

        // two to unbounded
        for (size_t ii = 0; ii < p->vertices.size(); ++ii)
        {
            createSFAPoint("Vertex", p->vertices[ii].get(), geoElem);
        }
    }
    else
    {
        throw except::InvalidArgumentException(Ctxt(FmtX(
                "Invalid geo type: [%s]",
                geoType.c_str())));
    }

    return geoElem;
}
}
}
