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
typedef xml::lite::Attributes ElemAttributes;
}

namespace six
{
namespace sidd
{
constexpr Version VERSION = Version::v1_0_0;
const char SI_COMMON_URI[] = "urn:SICommon:0.1";
const char ISM_URI[] = "urn:us:gov:ic:ism";
inline static xml::lite::Uri getISMUri()
{
    return xml::lite::Uri(ISM_URI);
}

DerivedXMLParser100::DerivedXMLParser100(logging::Logger* log,
                                         bool ownLog) :
    DerivedXMLParser(VERSION,
                     std::unique_ptr<six::SICommonXMLParser>(
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
    const xml::lite::Element* const root = doc->getRootElement();

    XMLElem productCreationElem        = getFirstAndOnly(root, "ProductCreation");
    XMLElem displayElem                = getFirstAndOnly(root, "Display");
    XMLElem measurementElem            = getFirstAndOnly(root, "Measurement");
    XMLElem exploitationFeaturesElem   = getFirstAndOnly(root, "ExploitationFeatures");
    XMLElem geographicAndTargetElem    = getFirstAndOnly(root, "GeographicAndTarget");
    XMLElem productProcessingElem      = getOptional(root, "ProductProcessing");
    XMLElem downstreamReprocessingElem = getOptional(root, "DownstreamReprocessing");
    XMLElem errorStatisticsElem        = getOptional(root, "ErrorStatistics");
    XMLElem radiometricElem            = getOptional(root, "Radiometric");
    XMLElem annotationsElem            = getOptional(root, "Annotations");

    DerivedDataBuilder builder;
    DerivedData *data = builder.steal(); //steal it

    // see if PixelType has MONO or RGB
    const auto pixelType = six::toType<PixelType>(
            getFirstAndOnly(displayElem, "PixelType")->getCharacterData());
    builder.addDisplay(pixelType);

    RegionType regionType = RegionType::SUB_REGION;
    XMLElem tmpElem = getFirstAndOnly(geographicAndTargetElem,
                                      "GeographicCoverage");

    // create GeographicAndTarget
    if (getOptional(tmpElem, "SubRegion"))
        regionType = RegionType::SUB_REGION;
    else if (getOptional(tmpElem, "GeographicInfo"))
        regionType = RegionType::GEOGRAPHIC_INFO;
    builder.addGeographicAndTarget(regionType);

    // create Measurement
    six::ProjectionType projType = ProjectionType::NOT_SET;
    if (getOptional(measurementElem, "GeographicProjection"))
        projType = ProjectionType::GEOGRAPHIC;
    else if (getOptional(measurementElem, "CylindricalProjection"))
        projType = ProjectionType::CYLINDRICAL;
    else if (getOptional(measurementElem, "PlaneProjection"))
        projType = ProjectionType::PLANE;
    else if (getOptional(measurementElem, "PolynomialProjection"))
        projType = ProjectionType::POLYNOMIAL;
    builder.addMeasurement(projType);

    // create ExploitationFeatures
    std::vector<XMLElem> elements;
    exploitationFeaturesElem->getElementsByTagName("ExploitationFeatures",
                                                  elements);
    builder.addExploitationFeatures(static_cast<unsigned int>(elements.size()));

    parseProductCreationFromXML(productCreationElem, data->productCreation.get());
    parseDisplayFromXML(displayElem, data->display.get());
    parseGeographicTargetFromXML(geographicAndTargetElem, data->geographicAndTarget.get());
    parseMeasurementFromXML(measurementElem, data->measurement.get());
    parseExploitationFeaturesFromXML(exploitationFeaturesElem, data->exploitationFeatures.get());

    if (productProcessingElem)
    {
        builder.addProductProcessing();
        parseProductProcessingFromXML(productProcessingElem,
                                      data->productProcessing.get());
    }
    if (downstreamReprocessingElem)
    {
        builder.addDownstreamReprocessing();
        parseDownstreamReprocessingFromXML(downstreamReprocessingElem,
                                           data->downstreamReprocessing.get());
    }
    if (errorStatisticsElem)
    {
        builder.addErrorStatistics();
        common().parseErrorStatisticsFromXML(errorStatisticsElem,
                                             data->errorStatistics.get());
    }
    if (radiometricElem)
    {
        builder.addRadiometric();
        common().parseRadiometryFromXML(radiometricElem,
                                        data->radiometric.get());
    }
    if (annotationsElem)
    {
        // 1 to unbounded
        std::vector<XMLElem> annChildren;
        annotationsElem->getElementsByTagName("Annotation", annChildren);
        data->annotations.resize(annChildren.size());
        for (size_t i = 0; i < annChildren.size(); ++i)
        {
            data->annotations[i].reset(new Annotation());
            parseAnnotationFromXML(annChildren[i], data->annotations[i].get());
        }
    }

    return data;
}

void DerivedXMLParser100::parseDerivedClassificationFromXML(
        const xml::lite::Element* classificationElem,
        DerivedClassification& classification) const
{
    DerivedXMLParser::parseDerivedClassificationFromXML(classificationElem, classification);

    const ElemAttributes& classificationAttributes
        = classificationElem->getAttributes();

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
    XMLElem classElem_ = newElement("Classification", parent);
    auto& classElem = *classElem_;

    common().addParameters("SecurityExtension",
                           classification.securityExtensions,
                           classElem_);

    //! from ism:ISMRootNodeAttributeGroup
    // SIDD 1.0 is tied to IC-ISM v4
    setAttribute(classElem_, "DESVersion", "4", ISM_URI);

    const auto ismUri = getISMUri();

    //! from ism:ResourceNodeAttributeGroup
    setAttribute(classElem, "resourceElement", "true", ismUri);
    setAttribute(classElem, "createDate",
                 classification.createDate.format("%Y-%m-%d"), ismUri);
    // optional
    setAttributeList(classElem, "compliesWith", classification.compliesWith, ismUri);

    //! from ism:SecurityAttributesGroup
    //  -- referenced in ism::ResourceNodeAttributeGroup
    setAttribute(classElem, "classification", classification.classification, ismUri);
    setAttributeList(classElem, "ownerProducer", classification.ownerProducer, ismUri, true);
    // optional
    setAttributeList(classElem, "SCIcontrols", classification.sciControls, ismUri);
    // optional
    setAttributeList(classElem, "SARIdentifier", classification.sarIdentifier, ismUri);
    // optional
    setAttributeList(classElem,
                     "disseminationControls",
                     classification.disseminationControls,
                     ismUri);
    // optional
    setAttributeList(classElem, "FGIsourceOpen", classification.fgiSourceOpen, ismUri);
    // optional
    setAttributeList(classElem,
                     "FGIsourceProtected",
                     classification.fgiSourceProtected,
                     ismUri);
    // optional
    setAttributeList(classElem, "releasableTo", classification.releasableTo, ismUri);
    // optional
    setAttributeList(classElem, "nonICmarkings", classification.nonICMarkings, ismUri);
    // optional
    setAttributeIfNonEmpty(classElem,
                           "classifiedBy",
                           classification.classifiedBy,
                           ismUri);
    // optional
    setAttributeIfNonEmpty(classElem,
                           "compilationReason",
                           classification.compilationReason,
                           ismUri);
    // optional
    setAttributeIfNonEmpty(classElem,
                           "derivativelyClassifiedBy",
                           classification.derivativelyClassifiedBy,
                           ismUri);
    // optional
    setAttributeIfNonEmpty(classElem,
                           "classificationReason",
                           classification.classificationReason,
                           ismUri);
    // optional
    setAttributeList(classElem, "nonUSControls", classification.nonUSControls, ismUri);
    // optional
    setAttributeIfNonEmpty(classElem,
                           "derivedFrom",
                           classification.derivedFrom,
                           ismUri);
    // optional
    if (classification.declassDate.get())
    {
        setAttributeIfNonEmpty(
                classElem, "declassDate",
                classification.declassDate->format("%Y-%m-%d"),
                ismUri);
    }
    // optional
    setAttributeIfNonEmpty(classElem,
                           "declassEvent",
                           classification.declassEvent,
                           ismUri);
    // optional
    setAttributeIfNonEmpty(classElem,
                           "declassException",
                           classification.declassException,
                           ismUri);
    // optional
    setAttributeIfNonEmpty(classElem,
                           "typeOfExemptedSource",
                           classification.exemptedSourceType,
                           ismUri);
    // optional
    if (classification.exemptedSourceDate.get())
    {
        setAttributeIfNonEmpty(
                classElem, "dateOfExemptedSource",
                classification.exemptedSourceDate->format("%Y-%m-%d"),
                ismUri);
    }

    return classElem_;
}
std::unique_ptr<DerivedData> DerivedXMLParser100::fromXML(const xml::lite::Document& doc) const
{
    return std::unique_ptr<DerivedData>(fromXML(&doc));
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
        for (size_t i = 0; i < derived->annotations.size(); ++i)
        {
            convertAnnotationToXML(derived->annotations[i].get(),
                                   annotationsElem);
        }
    }

    //set the ElemNS
    root->setNamespacePrefix("", getDefaultURI());
    root->setNamespacePrefix("si", xml::lite::Uri(SI_COMMON_URI));
    root->setNamespacePrefix("sfa", xml::lite::Uri(SFA_URI));
    root->setNamespacePrefix("ism", getISMUri());

    return doc;
}
std::unique_ptr<xml::lite::Document> DerivedXMLParser100::toXML(const DerivedData& data) const
{
    return std::unique_ptr<xml::lite::Document>(toXML(&data));
}

XMLElem DerivedXMLParser100::convertDisplayToXML(
        const Display& display,
        XMLElem parent) const
{
    XMLElem displayElem = newElement("Display", parent);

    createString("PixelType", display.pixelType, displayElem);

    // optional
    if (display.remapInformation.get())
    {
        XMLElem remapInfoElem = newElement("RemapInformation", displayElem);
        convertRemapToXML(*display.remapInformation, remapInfoElem);
    }

    // optional
    if (display.magnificationMethod != MagnificationMethod::NOT_SET)
    {
        createString("MagnificationMethod",
                     display.magnificationMethod, displayElem);
    }

    // optional
    if (display.decimationMethod != DecimationMethod::NOT_SET)
    {
        createString("DecimationMethod",
                     display.decimationMethod, displayElem);
    }

    // optional
    if (display.histogramOverrides.get())
    {
        XMLElem histo = newElement("DRAHistogramOverrides", displayElem);
        createInt("ClipMin", display.histogramOverrides->clipMin, histo);
        createInt("ClipMax", display.histogramOverrides->clipMax, histo);
    }

    // optional
    if (display.monitorCompensationApplied.get())
    {
        XMLElem monComp = newElement("MonitorCompensationApplied", displayElem);
        createDouble("Gamma", display.monitorCompensationApplied->gamma,
                     monComp);
        createDouble("XMin", display.monitorCompensationApplied->xMin, monComp);
    }

    // optional to unbounded
    common().addParameters("DisplayExtension", display.displayExtensions, displayElem);

    return displayElem;
}

XMLElem DerivedXMLParser100::convertGeographicTargetToXML(
        const GeographicAndTarget& geographicAndTarget,
        XMLElem parent) const
{
    XMLElem geographicAndTargetElem = newElement("GeographicAndTarget", parent);

    if (geographicAndTarget.geographicCoverage.get() == nullptr)
    {
        throw except::Exception(Ctxt("geographicCoverage is required"));
    }

    convertGeographicCoverageToXML(
            "GeographicCoverage",
            geographicAndTarget.geographicCoverage.get(),
            geographicAndTargetElem);

    // optional to unbounded
    for (std::vector<mem::ScopedCopyablePtr<TargetInformation> >::
            const_iterator it = geographicAndTarget.targetInformation.begin();
            it != geographicAndTarget.targetInformation.end(); ++it)
    {
        const TargetInformation* ti = (*it).get();
        XMLElem tiElem = newElement("TargetInformation", geographicAndTargetElem);

        // 1 to unbounded
        common().addParameters("Identifier", ti->identifiers, tiElem);

        // optional
        if (ti->footprint.get())
        {
            createFootprint("Footprint", "Vertex", *ti->footprint, tiElem);
        }

        // optional to unbounded
        common().addParameters("TargetInformationExtension",
                               ti->targetInformationExtensions, tiElem);
    }

    return geographicAndTargetElem;
}

void DerivedXMLParser100::parseGeographicTargetFromXML(
        const xml::lite::Element* geographicAndTargetElem,
        GeographicAndTarget* geographicAndTarget) const
{
    parseGeographicCoverageFromXML(
            getFirstAndOnly(geographicAndTargetElem, "GeographicCoverage"),
            geographicAndTarget->geographicCoverage.get());

    // optional to unbounded
    std::vector<XMLElem> targetInfosElem;
    geographicAndTargetElem->getElementsByTagName("TargetInformation",
                                                 targetInfosElem);
    geographicAndTarget->targetInformation.resize(targetInfosElem.size());
    for (size_t i = 0; i < targetInfosElem.size(); ++i)
    {
        geographicAndTarget->targetInformation[i].reset(
                new TargetInformation());

        TargetInformation* ti
                = geographicAndTarget->targetInformation[i].get();

        // unbounded
        common().parseParameters(targetInfosElem[i], "Identifier",
                                 ti->identifiers);

        // optional
        XMLElem tmpElem = getOptional(targetInfosElem[i], "Footprint");
        if (tmpElem)
        {
            ti->footprint.reset(new six::LatLonCorners());
            common().parseFootprint(tmpElem, "Vertex", *ti->footprint);
        }

        // optional
        common().parseParameters(targetInfosElem[i],
                                 "TargetInformationExtension",
                                 ti->targetInformationExtensions);
    }
}

void DerivedXMLParser100::parseGeographicCoverageFromXML(
        const xml::lite::Element* geographicCoverageElem,
        GeographicCoverage* geographicCoverage) const
{
    // optional and unbounded
    common().parseParameters(geographicCoverageElem, "GeoregionIdentifier",
                             geographicCoverage->georegionIdentifiers);

    common().parseFootprint(getFirstAndOnly(geographicCoverageElem, "Footprint"),
                            "Vertex", geographicCoverage->footprint);

    // If there are subregions, recurse
    std::vector<XMLElem> subRegionsElem;
    geographicCoverageElem->getElementsByTagName("SubRegion", subRegionsElem);

    geographicCoverage->subRegion.resize(subRegionsElem.size());
    for (size_t i = 0; i < subRegionsElem.size(); ++i)
    {
        geographicCoverage->subRegion[i].reset(
                new GeographicCoverage(RegionType::SUB_REGION));
        parseGeographicCoverageFromXML(
                subRegionsElem[i], geographicCoverage->subRegion[i].get());
    }

    // Otherwise read the GeographicInfo
    if (subRegionsElem.size() == 0)
    {
        XMLElem geographicInfoElem = getFirstAndOnly(geographicCoverageElem,
                                                    "GeographicInfo");

        geographicCoverage->geographicInformation.reset(
            new GeographicInformation());

        // optional to unbounded
        std::vector<XMLElem> countryCodes;
        geographicInfoElem->getElementsByTagName("CountryCode", countryCodes);
        for (const auto& countryCode : countryCodes)
        {
            geographicCoverage->geographicInformation->
                    countryCodes.push_back(countryCode->getCharacterData());
        }

        parseOptionalString(geographicInfoElem, "SecurityInfo",
            geographicCoverage->geographicInformation->securityInformation);

        // optional to unbounded
        common().parseParameters(geographicInfoElem, "GeographicInfoExtension",
                geographicCoverage->geographicInformation->
                        geographicInformationExtensions);
    }
}

XMLElem DerivedXMLParser100::convertMeasurementToXML(
    const Measurement* measurement,
    XMLElem parent) const
{
    XMLElem measurementElem = DerivedXMLParser::convertMeasurementToXML(measurement, parent);

    common().createPolyXYZ("ARPPoly",
        measurement->arpPoly,
        measurementElem);

    return measurementElem;
}

void DerivedXMLParser100::parseMeasurementFromXML(
    const xml::lite::Element* measurementElem,
    Measurement* measurement) const
{
    DerivedXMLParser::parseMeasurementFromXML(measurementElem, measurement);

    common().parsePolyXYZ(getFirstAndOnly(measurementElem, "ARPPoly"),
        measurement->arpPoly);
}

XMLElem DerivedXMLParser100::convertExploitationFeaturesToXML(
    const ExploitationFeatures* exploitationFeatures,
    XMLElem parent) const
{
    XMLElem exploitationFeaturesElem =
        newElement("ExploitationFeatures", parent);

    if (exploitationFeatures->collections.empty())
    {
        throw except::Exception(Ctxt(str::Format(
            "ExploitationFeatures must have at least [1] Collection, " \
            "only [%d] found", exploitationFeatures->collections.size())));
    }

    // 1 to unbounded
    for (auto& pCollections : exploitationFeatures->collections)
    {
        Collection* collection = pCollections.get();
        XMLElem collectionElem = newElement("Collection",
            exploitationFeaturesElem);
        setAttribute(collectionElem, "identifier", collection->identifier);

        // create Information
        XMLElem informationElem = newElement("Information", collectionElem);

        createString("SensorName",
            collection->information.sensorName,
            informationElem);
        XMLElem radarModeElem = newElement("RadarMode", informationElem);
        createSixString("ModeType",
            common().getSICommonURI(),
            collection->information.radarMode,
            radarModeElem);
        // optional
        if (!collection->information.radarModeID.empty())
            createString("ModeID",
                common().getSICommonURI(),
                collection->information.radarModeID,
                radarModeElem);
        createDateTime("CollectionDateTime",
            collection->information.collectionDateTime,
            informationElem);
        // optional
        if (collection->information.localDateTime != Init::undefined<
            six::DateTime>())
        {
            createDateTime("LocalDateTime",
                collection->information.localDateTime,
                informationElem);
        }
        createDouble("CollectionDuration",
            collection->information.collectionDuration,
            informationElem);
        // optional
        if (!Init::isUndefined(collection->information.resolution))
        {
            common().createRangeAzimuth("Resolution",
                collection->information.resolution,
                informationElem);
        }
        // optional
        if (collection->information.inputROI.get())
        {
            XMLElem roiElem = newElement("InputROI", informationElem);
            common().createRowCol("Size",
                collection->information.inputROI->size,
                roiElem);
            common().createRowCol("UpperLeft",
                collection->information.inputROI->upperLeft,
                roiElem);
        }
        // optional to unbounded
        for (const auto& pPolarization : collection->information.polarization)
        {
            const TxRcvPolarization *p = pPolarization.get();
            XMLElem polElem = newElement("Polarization", informationElem);

            createString("TxPolarization",
                p->txPolarization,
                polElem);
            createString("RcvPolarization",
                p->rcvPolarization,
                polElem);
            // optional
            if (!Init::isUndefined(p->rcvPolarizationOffset))
            {
                createDouble("RcvPolarizationOffset",
                    p->rcvPolarizationOffset,
                    polElem);
            }
            // optional
            if (!Init::isUndefined(p->processed))
            {
                createSixString("Processed", p->processed, polElem);
            }
        }

        // create Geometry -- optional
        const Geometry* geom = collection->geometry.get();
        if (geom != nullptr)
        {
            XMLElem geometryElem = newElement("Geometry", collectionElem);

            // optional
            if (geom->azimuth != Init::undefined<double>())
                createDouble("Azimuth", geom->azimuth, geometryElem);
            // optional
            if (geom->slope != Init::undefined<double>())
                createDouble("Slope", geom->slope, geometryElem);
            // optional
            if (geom->squint != Init::undefined<double>())
                createDouble("Squint", geom->squint, geometryElem);
            // optional
            if (geom->graze != Init::undefined<double>())
                createDouble("Graze", geom->graze, geometryElem);
            // optional
            if (geom->tilt != Init::undefined<double>())
                createDouble("Tilt", geom->tilt, geometryElem);
            // optional to unbounded
            common().addParameters("Extension", geom->extensions,
                geometryElem);
        }

        // create Phenomenology -- optional
        const Phenomenology* phenom = collection->phenomenology.get();
        if (phenom != nullptr)
        {
            XMLElem phenomenologyElem = newElement("Phenomenology",
                collectionElem);

            // optional
            if (phenom->shadow != Init::undefined<AngleMagnitude>())
            {
                XMLElem shadow = newElement("Shadow", phenomenologyElem);
                createDouble("Angle", common().getSICommonURI(),
                    phenom->shadow.angle, shadow);
                createDouble("Magnitude", common().getSICommonURI(),
                    phenom->shadow.magnitude, shadow);
            }
            // optional
            if (phenom->layover != Init::undefined<AngleMagnitude>())
            {
                XMLElem layover = newElement("Layover", phenomenologyElem);
                createDouble("Angle", common().getSICommonURI(),
                    phenom->layover.angle, layover);
                createDouble("Magnitude", common().getSICommonURI(),
                    phenom->layover.magnitude, layover);
            }
            // optional
            if (phenom->multiPath != Init::undefined<double>())
                createDouble("MultiPath", phenom->multiPath, phenomenologyElem);
            // optional
            if (phenom->groundTrack != Init::undefined<double>())
                createDouble("GroundTrack", phenom->groundTrack,
                    phenomenologyElem);
            // optional to unbounded
            common().addParameters("Extension", phenom->extensions,
                phenomenologyElem);
        }
    }

    // create Product
    XMLElem productElem = newElement("Product", exploitationFeaturesElem);

    if (exploitationFeatures->product.size() != 1)
    {
        throw except::Exception(Ctxt(
                "Expected exactly one Product for ExploitationFeatures"));
    }
    common().createRowCol("Resolution",
        exploitationFeatures->product[0].resolution,
        productElem);
    // optional
    if (exploitationFeatures->product[0].north != Init::undefined<double>())
        createDouble("North", exploitationFeatures->product[0].north, productElem);

    // optional to unbounded
    common().addParameters("Extension",
        exploitationFeatures->product[0].extensions,
        productElem);

    return exploitationFeaturesElem;
}

void DerivedXMLParser100::parseProductFromXML(
    const xml::lite::Element* exploitationFeaturesElem,
    ExploitationFeatures* exploitationFeatures) const
{
    XMLElem productElem = getFirstAndOnly(exploitationFeaturesElem, "Product");

    exploitationFeatures->product.resize(1);
    Product& product = exploitationFeatures->product[0];
    common().parseRowColDouble(getFirstAndOnly(productElem, "Resolution"),
                               product.resolution);

    parseOptionalDouble(productElem, "North", product.north);

    // optional to unbounded
    common().parseParameters(productElem, "Extension",
                             product.extensions);
}
}
}
