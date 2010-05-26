/* =========================================================================
 * This file is part of six-c++ 
 * =========================================================================
 * 
 * (C) Copyright 2004 - 2009, General Dynamics - Advanced Information Systems
 *
 * six-c++ is free software; you can redistribute it and/or modify
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
#include "six/Types.h"
#include "six/Utilities.h"
#include <import/str.h>

#include "six/sidd/DerivedXMLControl.h"
#include "six/sidd/DerivedData.h"
#include "six/sidd/DerivedDataBuilder.h"

using namespace six;
using namespace six::sidd;

void DerivedXMLControl::xmlToProductCreation(
                                             xml::lite::Element* productCreationXML,
                                             ProductCreation* productCreation)
{
    xml::lite::Element* informationXML =
            getFirstAndOnly(productCreationXML, "ProcessorInformation");
    productCreation->processorInformation->application
            = getFirstAndOnly(informationXML, "Application")->getCharacterData();

    parseDateTime(getFirstAndOnly(informationXML, "ProcessingDateTime"),
                  productCreation->processorInformation->processingDateTime);

    productCreation->processorInformation->site
            = getFirstAndOnly(informationXML, "Site")->getCharacterData();

    xml::lite::Element* profileXML = getOptional(informationXML, "Profile");
    if (profileXML)
    {
        productCreation->processorInformation->profile
                = profileXML->getCharacterData();
    }

    //Classification
    xml::lite::Element* classificationXML = getFirstAndOnly(productCreationXML,
                                                            "Classification");
    productCreation->classification.level
            = getFirstAndOnly(classificationXML, "Level")->getCharacterData();

    std::vector<xml::lite::Element*> secMarkingsXML;
    classificationXML->getElementsByTagName("SecurityMarkings", secMarkingsXML);
    for (std::vector<xml::lite::Element*>::iterator it = secMarkingsXML.begin(); it
            != secMarkingsXML.end(); ++it)
    {
        productCreation->classification.securityMarkings. push_back(
                                                                    (*it)->getCharacterData());
    }

    xml::lite::Element* classGuidanceXML = getOptional(classificationXML,
                                                       "Guidance");
    if (classGuidanceXML)
    {
        ClassificationGuidance* classGuidance = new ClassificationGuidance();
        productCreation->classification.guidance = classGuidance;

        parseDateTime(getFirstAndOnly(classGuidanceXML, "Date"),
                      productCreation->classification.guidance->date);
        productCreation->classification.guidance->authority
                = getFirstAndOnly(classGuidanceXML, "Authority")->getCharacterData();
    }

    productCreation->productName
            = getFirstAndOnly(productCreationXML, "ProductName")->getCharacterData();
    productCreation->productClass
            = getFirstAndOnly(productCreationXML, "ProductClass")->getCharacterData();

    xml::lite::Element* productTypeXML = getOptional(productCreationXML,
                                                     "ProductType");
    if (productTypeXML)
    {
        productCreation->productType = productTypeXML->getCharacterData();
    }

    try
    {
        parseParameters(productCreationXML, "ProductCreationExtension",
                        productCreation->productCreationExtensions);
    }
    catch (except::Exception&)
    {
    }
}

void DerivedXMLControl::xmlToDisplay(xml::lite::Element* displayXML,
                                     Display* display)
{
    display->pixelType
            = six::toType<PixelType>(
                                     getFirstAndOnly(displayXML, "PixelType")->getCharacterData());

    //RemapInformation
    xml::lite::Element* remapInformationXML = getOptional(displayXML,
                                                          "RemapInformation");
    if (remapInformationXML)
    {
        if (display->remapInformation->displayType
                == DisplayType::DISPLAY_COLOR)
        {

            xml::lite::Element* remapXML = getFirstAndOnly(remapInformationXML,
                                                           "ColorDisplayRemap");

            xml::lite::Element* remapLUTXML = getFirstAndOnly(remapXML,
                                                              "RemapLUT");

            //get size attribute
            int
                    size =
                            str::toType<int>(
                                             remapLUTXML->getAttributes().getValue(
                                                                                   "size"));
            display->remapInformation->remapLUT = new LUT(size, 3);

            std::string lutStr =
                    getFirstAndOnly(remapXML, "RemapLUT")->getCharacterData();
            std::vector<std::string> lutVals = str::split(lutStr, " ");

            int k = 0;
            for (unsigned int i = 0; i < lutVals.size(); i++)
            {
                std::vector<std::string> rgb = str::split(lutVals[i], ",");
                for (unsigned int j = 0; j < rgb.size(); j++)
                {
                    display->remapInformation->remapLUT->table[k++]
                            = rgb[j].at(0);
                }
            }
        }
        else if (display->remapInformation->displayType
                == DisplayType::DISPLAY_MONO)
        {
            xml::lite::Element* remapXML =
                    getFirstAndOnly(remapInformationXML,
                                    "MonochromeDisplayRemap");

            xml::lite::Element* remapLUTXML = getFirstAndOnly(remapXML,
                                                              "RemapLUT");

            //get size attribute
            int
                    size =
                            str::toType<int>(
                                             remapLUTXML->getAttributes().getValue(
                                                                                   "size"));
            display->remapInformation->remapLUT = new LUT(size, 1);

            std::string lutStr =
                    getFirstAndOnly(remapXML, "RemapLUT")->getCharacterData();
            std::vector<std::string> lutVals = str::split(lutStr, " ");
            for (unsigned int i = 0; i < lutVals.size(); i++)
            {
                display->remapInformation->remapLUT->table[i]
                        = lutVals[i].at(0);
            }

            ((MonochromeDisplayRemap*) display->remapInformation)->remapType
                    = getFirstAndOnly(remapXML, "RemapType")->getCharacterData();

            //RemapParameters
            try
            {
                parseParameters(
                                remapXML,
                                "RemapParameter",
                                ((MonochromeDisplayRemap*) display-> remapInformation)->remapParameters);
            }
            catch (except::Exception&)
            {
            }
        }
    }

    display->magnificationMethod
            = six::toType<MagnificationMethod>(
                                               getFirstAndOnly(displayXML,
                                                               "MagnificationMethod")->getCharacterData());
    display->decimationMethod
            = six::toType<DecimationMethod>(
                                            getFirstAndOnly(displayXML,
                                                            "DecimationMethod")->getCharacterData());

    xml::lite::Element* histogramOverridesXML =
            getOptional(displayXML, "DRAHistogramOverrides");
    if (histogramOverridesXML)
    {
        display->histogramOverrides = new DRAHistogramOverrides();
        parseInt(getFirstAndOnly(histogramOverridesXML, "ClipMin"),
                 display->histogramOverrides->clipMin);
        parseInt(getFirstAndOnly(histogramOverridesXML, "ClipMax"),
                 display->histogramOverrides->clipMax);
    }

    //MonitorCompensationApplied
    xml::lite::Element* monitorCompensationXML =
            getOptional(displayXML, "MonitorCompensationApplied");
    if (monitorCompensationXML)
    {
        display->monitorCompensationApplied = new MonitorCompensationApplied();
        parseDouble(getFirstAndOnly(monitorCompensationXML, "Gamma"),
                    display->monitorCompensationApplied->gamma);
        parseDouble(getFirstAndOnly(monitorCompensationXML, "XMin"),
                    display->monitorCompensationApplied->xMin);
    }

    //DisplayExtension
    try
    {
        parseParameters(displayXML, "DisplayExtension",
                        display->displayExtensions);
    }
    catch (except::Exception&)
    {
    }

}

void DerivedXMLControl::xmlToGeographicAndTarget(
                                                 xml::lite::Element* geographicAndTargetXML,
                                                 GeographicAndTarget* geographicAndTarget)
{
    xml::lite::Element* geographicCoverageXML =
            getFirstAndOnly(geographicAndTargetXML, "GeographicCoverage");

    xmlToGeographicCoverage(geographicCoverageXML,
                            geographicAndTarget->geographicCoverage);

    //TargetInformation
    std::vector<xml::lite::Element*> targetInfosXML;
    geographicAndTargetXML->getElementsByTagName("TargetInformation",
                                                 targetInfosXML);

    for (std::vector<xml::lite::Element*>::iterator it = targetInfosXML.begin(); it
            != targetInfosXML.end(); ++it)
    {
        TargetInformation* ti = new TargetInformation();
        geographicAndTarget->targetInformation.push_back(ti);

        parseParameters(*it, "Identifier", ti->identifiers);

        //Footprint
        std::vector<xml::lite::Element*> footprintsXML;
        (*it)->getElementsByTagName("Footprint", footprintsXML);
        for (std::vector<xml::lite::Element*>::iterator it2 =
                footprintsXML.begin(); it2 != footprintsXML.end(); ++it2)
        {
            std::vector<LatLon> fp;
            parseFootprint(*it2, "Vertex", fp, false);
            ti->footprints.push_back(fp);
        }

        //TargetInformationExtension
        try
        {
            parseParameters(*it, "TargetInformationExtension",
                            ti->targetInformationExtensions);
        }
        catch (except::Exception&)
        {
        }

    }

}

void DerivedXMLControl::xmlToGeographicCoverage(
                                                xml::lite::Element* geographicCoverageXML,
                                                GeographicCoverage* geographicCoverage)
{
    std::vector<xml::lite::Element*> georegionIdsXML;
    geographicCoverageXML->getElementsByTagName("GeoregionIdentifier",
                                                georegionIdsXML);
    for (std::vector<xml::lite::Element*>::iterator it =
            georegionIdsXML.begin(); it != georegionIdsXML.end(); ++it)
    {
        geographicCoverage->georegionIdentifiers.push_back(
                                                           (*it)->getCharacterData());
    }

    xml::lite::Element* footprintXML = getFirstAndOnly(geographicCoverageXML,
                                                       "Footprint");

    //Footprint
    parseFootprint(footprintXML, "Vertex", geographicCoverage->footprint, false);

    //If there are subregions, recurse
    std::vector<xml::lite::Element*> subRegionsXML;
    geographicCoverageXML->getElementsByTagName("SubRegion", subRegionsXML);

    int i = 0;
    for (std::vector<xml::lite::Element*>::iterator it = subRegionsXML.begin(); it
            != subRegionsXML.end(); ++it)
    {
        geographicCoverage->subRegion.push_back(
                                                new GeographicCoverage(
                                                                       RegionType::REGION_SUB_REGION));
        xmlToGeographicCoverage(*it, geographicCoverage->subRegion[i++]);
    }

    //Otherwise read the GeographicInfo
    if (subRegionsXML.size() == 0)
    {
        xml::lite::Element* geographicInfoXML =
                getFirstAndOnly(geographicCoverageXML, "GeographicInfo");

        geographicCoverage->geographicInformation = new GeographicInformation();

        std::vector<xml::lite::Element*> countryCodes;
        geographicInfoXML->getElementsByTagName("CountryCode", countryCodes);
        for (std::vector<xml::lite::Element*>::iterator it =
                countryCodes.begin(); it != countryCodes.end(); ++it)
        {
            xml::lite::Element* countryCode = *it;
            geographicCoverage->geographicInformation->countryCodes.push_back(
                                                                              countryCode->getCharacterData());
        }

        xml::lite::Element* securityInformationXML =
                getOptional(geographicInfoXML, "SecurityInformation");
        if (securityInformationXML)
        {
            geographicCoverage->geographicInformation->securityInformation
                    = securityInformationXML->getCharacterData();
        }

        //GeographicInfoExtension
        try
        {
            parseParameters(
                            geographicInfoXML,
                            "GeographicInfoExtension",
                            geographicCoverage->geographicInformation-> geographicInformationExtensions);
        }
        catch (except::Exception&)
        {
        }

    }
}

// This function ASSUMES that the measurement projection has already been set!
void DerivedXMLControl::xmlToMeasurement(xml::lite::Element* measurementXML,
                                         Measurement* measurement)
{
    parseRowColInt(getFirstAndOnly(measurementXML, "PixelFootprint"),
                   measurement->pixelFootprint);

    xml::lite::Element* projXML;
    if (measurement->projection->projectionType
            == ProjectionType::PROJECTION_PLANE)
    {
        projXML = getFirstAndOnly(measurementXML, "PlaneProjection");
    }
    else if (measurement->projection->projectionType
            == ProjectionType::PROJECTION_CYLINDRICAL)
    {
        projXML = getFirstAndOnly(measurementXML, "CylindricalProjection");
    }
    else if (measurement->projection->projectionType
            == ProjectionType::PROJECTION_GEOGRAPHIC)
    {
        projXML = getFirstAndOnly(measurementXML, "GeographicProjection");
    }

    xml::lite::Element* refXML = getFirstAndOnly(projXML, "ReferencePoint");

    parseVector3D(getFirstAndOnly(refXML, "ECEF"),
                  measurement->projection->referencePoint.ecef);

    parseRowColDouble(getFirstAndOnly(refXML, "Point"),
                      measurement->projection->referencePoint.rowCol);

    if (measurement->projection->projectionType
            == ProjectionType::PROJECTION_PLANE)
    {
        PlaneProjection* planeProj = (PlaneProjection*) measurement->projection;
        parsePoly2D(getFirstAndOnly(projXML, "TimeCOAPoly"),
                    planeProj->timeCOAPoly);

        parseRowColDouble(getFirstAndOnly(projXML, "SampleSpacing"),
                          planeProj->sampleSpacing);

        xml::lite::Element* prodPlaneXML = getFirstAndOnly(projXML,
                                                           "ProductPlane");

        parseVector3D(getFirstAndOnly(prodPlaneXML, "RowUnitVector"),
                      planeProj->productPlane.rowUnitVector);
        parseVector3D(getFirstAndOnly(prodPlaneXML, "ColUnitVector"),
                      planeProj->productPlane.colUnitVector);
    }
    else if (measurement->projection->projectionType
            == ProjectionType::PROJECTION_GEOGRAPHIC)
    {
        GeographicProjection* geographicProj =
                (GeographicProjection*) measurement->projection;

        parsePoly2D(getFirstAndOnly(projXML, "TimeCOAPoly"),
                    geographicProj->timeCOAPoly);

        parseRowColLatLon(getFirstAndOnly(projXML, "SampleSpacing"),
                          geographicProj->sampleSpacing);
    }
    else if (measurement->projection->projectionType
            == ProjectionType::PROJECTION_CYLINDRICAL)
    {
        // Now, we go TimeCOA, SampleSpacing, CurvatureRadius
        CylindricalProjection* cylindricalProj =
                (CylindricalProjection*) measurement->projection;

        parsePoly2D(getFirstAndOnly(projXML, "TimeCOAPoly"),
                    cylindricalProj->timeCOAPoly);

        parseRowColDouble(getFirstAndOnly(projXML, "SampleSpacing"),
                          cylindricalProj->sampleSpacing);

        xml::lite::Element* curvRadiusXML = getOptional(projXML,
                                                        "CurvatureRadius");
        if (curvRadiusXML)
        {
            parseDouble(curvRadiusXML, cylindricalProj->curvatureRadius);
        }
    }
    else if (measurement->projection->projectionType
            == ProjectionType::PROJECTION_POLYNOMIAL)
    {
        PolynomialProjection* polyProj =
                (PolynomialProjection*) measurement->projection;

        // Get a bunch of 2D polynomials
        parsePoly2D(getFirstAndOnly(projXML, "RowColToLat"),
                    polyProj->rowColToLat);

        parsePoly2D(getFirstAndOnly(projXML, "RowColToLon"),
                    polyProj->rowColToLat);

        xml::lite::Element* optionalAltPolyXML = getOptional(projXML,
                                                             "RowColToAlt");
        if (optionalAltPolyXML)
        {
            parsePoly2D(optionalAltPolyXML, polyProj->rowColToAlt);
        }

        parsePoly2D(getFirstAndOnly(projXML, "LatLonToRow"),
                    polyProj->latLonToRow);

        parsePoly2D(getFirstAndOnly(projXML, "LatLonToCol"),
                    polyProj->latLonToCol);

    }
    else
        throw except::Exception(Ctxt("Unknown projection type"));

    xml::lite::Element* tmpElem = getFirstAndOnly(measurementXML, "ARPPoly");
    parsePolyXYZ(tmpElem, measurement->arpPoly);

}

void DerivedXMLControl::xmlToExploitationFeatures(
                                                  xml::lite::Element* exploitationFeaturesXML,
                                                  ExploitationFeatures* exploitationFeatures)
{
    xml::lite::Element* tmpElem;

    std::vector<xml::lite::Element*> collectionsXML;
    exploitationFeaturesXML->getElementsByTagName("Collection", collectionsXML);

    unsigned int idx = 0;
    for (std::vector<xml::lite::Element*>::iterator it = collectionsXML.begin(); it
            != collectionsXML.end(); ++it)
    {
        xml::lite::Element* collectionXML = *it;
        Collection* coll;
        if (exploitationFeatures->collections.size() <= idx)
        {
            coll = new Collection();
            exploitationFeatures->collections.push_back(coll);
        }
        else
        {
            coll = exploitationFeatures->collections[idx];
        }

        // Information
        Information* info = coll->information;
        xml::lite::Element* informationXML = getFirstAndOnly(collectionXML,
                                                             "Information");

        info->sensorName
                = getFirstAndOnly(informationXML, "SensorName")->getCharacterData();

        xml::lite::Element* radarModeXML = getFirstAndOnly(informationXML,
                                                           "RadarMode");
        info->radarMode
                = six::toType<RadarModeType>(
                                             getFirstAndOnly(radarModeXML,
                                                             "ModeType")->getCharacterData());
        tmpElem = getOptional(radarModeXML, "ModeID");
        if (tmpElem)
        {
            info->radarModeID = tmpElem->getCharacterData();
        }

        parseDateTime(getFirstAndOnly(informationXML, "CollectionDateTime"),
                      info->collectionDateTime);

        tmpElem = getOptional(informationXML, "LocalDateTime");
        if (tmpElem)
        {
            info->localDateTime = tmpElem->getCharacterData();
        }

        parseDouble(getFirstAndOnly(informationXML, "CollectionDuration"),
                    info->collectionDuration);

        parseRangeAzimuth(getFirstAndOnly(informationXML, "Resolution"),
                          info->resolution);

        xml::lite::Element* roiXML = getOptional(informationXML, "InputROI");
        if (roiXML)
        {
            info->inputROI = new InputROI();

            parseRowColDouble(getFirstAndOnly(roiXML, "Size"),
                              info->inputROI->size);
            parseRowColDouble(getFirstAndOnly(roiXML, "UpperLeft"),
                              info->inputROI->upperLeft);
        }

        xml::lite::Element* polXML =
                getOptional(informationXML, "Polarization");
        if (polXML)
        {
            info->polarization = new TxRcvPolarization();

            tmpElem = getFirstAndOnly(polXML, "TxPolarization");
            info->polarization->txPolarization
                    = six::toType<PolarizationType>(tmpElem->getCharacterData());

            tmpElem = getFirstAndOnly(polXML, "RcvPolarization");
            parseDouble(tmpElem, info->polarization->rcvPolarization);
        }

        // Geometry
        xml::lite::Element* geometryXML =
                getOptional(collectionXML, "Geometry");

        if (geometryXML)
        {
            Geometry* geom = new Geometry();
            coll->geometry = geom;

            tmpElem = getOptional(geometryXML, "Azimuth");
            if (tmpElem)
            {
                parseDouble(tmpElem, geom->azimuth);
            }

            tmpElem = getOptional(geometryXML, "Slope");
            if (tmpElem)
            {
                parseDouble(tmpElem, geom->slope);
            }

            tmpElem = getOptional(geometryXML, "Squint");
            if (tmpElem)
            {
                parseDouble(tmpElem, geom->squint);
            }

            tmpElem = getOptional(geometryXML, "Graze");
            if (tmpElem)
            {
                parseDouble(tmpElem, geom->graze);
            }

            tmpElem = getOptional(geometryXML, "Tilt");
            if (tmpElem)
            {
                parseDouble(tmpElem, geom->tilt);
            }
        }

        // Phenomenology

        xml::lite::Element* phenomenologyXML = getOptional(collectionXML,
                                                           "Phenomenology");

        if (phenomenologyXML)
        {

            Phenomenology* phenom = new Phenomenology();
            coll->phenomenology = phenom;

            tmpElem = getOptional(phenomenologyXML, "Shadow");
            if (tmpElem)
            {
                parseDouble(getFirstAndOnly(tmpElem, "Angle"),
                            phenom->shadow.angle);
                parseDouble(getFirstAndOnly(tmpElem, "Magnitude"),
                            phenom->shadow.magnitude);
            }

            tmpElem = getOptional(phenomenologyXML, "Layover");
            if (tmpElem)
            {
                parseDouble(getFirstAndOnly(tmpElem, "Angle"),
                            phenom->layover.angle);
                parseDouble(getFirstAndOnly(tmpElem, "Magnitude"),
                            phenom->layover.magnitude);
            }

            tmpElem = getOptional(phenomenologyXML, "MultiPath");
            if (tmpElem)
            {
                parseDouble(tmpElem, phenom->multiPath);
            }

            tmpElem = getOptional(phenomenologyXML, "GroundTrack");
            if (tmpElem)
            {
                parseDouble(tmpElem, phenom->groundTrack);
            }
        }
    }

    xml::lite::Element* productXML = getFirstAndOnly(exploitationFeaturesXML,
                                                     "Product");
    Product prod = exploitationFeatures->product;

    parseRowColDouble(getFirstAndOnly(productXML, "Resolution"),
                      exploitationFeatures->product.resolution);

    tmpElem = getOptional(productXML, "North");
    if (tmpElem)
        parseDouble(tmpElem, prod.north);
}

Data* DerivedXMLControl::fromXML(xml::lite::Document* doc)
{
    xml::lite::Element *root = doc->getRootElement();

    xml::lite::Element* productCreationXML = getFirstAndOnly(root,
                                                             "ProductCreation");
    xml::lite::Element* displayXML = getFirstAndOnly(root, "Display");
    xml::lite::Element* measurementXML = getFirstAndOnly(root, "Measurement");
    xml::lite::Element* exploitationFeaturesXML =
            getFirstAndOnly(root, "ExploitationFeatures");
    xml::lite::Element* geographicAndTargetXML =
            getFirstAndOnly(root, "GeographicAndTarget");

    DerivedDataBuilder builder;
    DerivedData *data = builder.steal(); //steal it

    // see if PixelType has MONO or RGB
    PixelType
            pixelType =
                    six::toType<PixelType>(
                                           getFirstAndOnly(displayXML,
                                                           "PixelType")->getCharacterData());
    builder.addDisplay(pixelType);

    RegionType regionType = RegionType::REGION_SUB_REGION;
    xml::lite::Element* tmpElem = getFirstAndOnly(geographicAndTargetXML,
                                                  "GeographicCoverage");
    // see if GeographicCoverage contains SubRegion or GeographicInfo

    if (getOptional(tmpElem, "SubRegion"))
    {
        regionType = RegionType::REGION_SUB_REGION;
    }
    else if (getOptional(tmpElem, "GeographicInfo"))
    {
        regionType = RegionType::REGION_GEOGRAPHIC_INFO;
    }
    builder.addGeographicAndTarget(regionType);

    six::ProjectionType projType = ProjectionType::PROJECTION_NOT_SET;

    if (getOptional(measurementXML, "GeographicProjection"))
        projType = ProjectionType::PROJECTION_GEOGRAPHIC;
    else if (getOptional(measurementXML, "CylindricalProjection"))
        projType = ProjectionType::PROJECTION_CYLINDRICAL;
    else if (getOptional(measurementXML, "PlaneProjection"))
        projType = ProjectionType::PROJECTION_PLANE;
    else if (getOptional(measurementXML, "PolynomialProjection"))
        projType = ProjectionType::PROJECTION_POLYNOMIAL;

    builder.addMeasurement(projType);

    std::vector<xml::lite::Element*> elements;
    exploitationFeaturesXML->getElementsByTagName("ExploitationFeatures",
                                                  elements);
    builder.addExploitationFeatures(elements.size());

    xmlToProductCreation(productCreationXML, data->productCreation);
    xmlToDisplay(displayXML, data->display);
    xmlToGeographicAndTarget(geographicAndTargetXML, data->geographicAndTarget);
    xmlToMeasurement(measurementXML, data->measurement);
    xmlToExploitationFeatures(exploitationFeaturesXML,
                              data->exploitationFeatures);

    xml::lite::Element *productProcessingXML = getOptional(root,
                                                           "ProductProcessing");
    if (productProcessingXML)
    {
        builder.addProductProcessing();
        xmlToProductProcessing(productProcessingXML, data->productProcessing);
    }

    xml::lite::Element *downstreamReprocessingXML =
            getOptional(root, "DownstreamReprocessing");
    if (downstreamReprocessingXML)
    {
        builder.addDownstreamReprocessing();
        xmlToDownstreamReprocessing(downstreamReprocessingXML,
                                    data->downstreamReprocessing);
    }

    xml::lite::Element *errorStatisticsXML = getOptional(root,
                                                         "ErrorStatistics");
    if (errorStatisticsXML)
    {
        builder.addErrorStatistics();
        xmlToErrorStatistics(errorStatisticsXML, data->errorStatistics);
    }

    xml::lite::Element *radiometricXML = getOptional(root, "Radiometric");
    if (radiometricXML)
    {
        builder.addRadiometric();
        xmlToRadiometric(radiometricXML, data->radiometric);
    }

    return data;
}

xml::lite::Element* DerivedXMLControl::productCreationToXML(
                                                            ProductCreation* productCreation,
                                                            xml::lite::Element* parent)
{
    // Make the XML node
    xml::lite::Element* productCreationXML = newElement("ProductCreation",
                                                        parent);

    // Processor Info
    xml::lite::Element* procInfoXML = newElement("ProcessorInformation",
                                                 productCreationXML);

    createString("Application",
                 productCreation->processorInformation->application,
                 procInfoXML);

    createDateTime("ProcessingDateTime",
                   productCreation->processorInformation->processingDateTime,
                   procInfoXML);

    createString("Site", productCreation->processorInformation->site,
                 procInfoXML);

    //Profile - optional
    if (productCreation->processorInformation->profile != Init::undefined<
            std::string>())
    {
        createString("Profile", productCreation->processorInformation->profile,
                     procInfoXML);
    }

    //Classification
    xml::lite::Element* classXML = newElement("Classification",
                                              productCreationXML);
    createString("Level", productCreation->classification.level, classXML);

    for (std::vector<std::string>::iterator it =
            productCreation->classification.securityMarkings.begin(); it
            != productCreation->classification.securityMarkings.end(); ++it)
    {
        std::string marking = *it;
        str::trim(marking);
        if (!marking.empty())
            createString("SecurityMarkings", *it, classXML);
    }

    if (productCreation->classification.guidance)
    {
        xml::lite::Element* guidanceXML = newElement("Guidance", classXML);
        createString("Authority",
                     productCreation->classification.guidance->authority,
                     guidanceXML);
        createDate("Date", productCreation->classification.guidance->date,
                   guidanceXML);
    }

    //ProductName
    createString("ProductName", productCreation->productName,
                 productCreationXML);

    //ProductClass
    createString("ProductClass", productCreation->productClass,
                 productCreationXML);

    //ProductType - optional
    if (productCreation->productType != Init::undefined<std::string>())
    {
        createString("ProductType", productCreation->productType,
                     productCreationXML);
    }

    //product extensions
    addParameters("ProductCreationExtension",
                  productCreation->productCreationExtensions,
                  productCreationXML);
    return productCreationXML;
}

xml::lite::Element*
DerivedXMLControl::displayToXML(Display* display, xml::lite::Element* parent)
{
    xml::lite::Element* displayXML = newElement("Display", parent);

    createString("PixelType", str::toString(display->pixelType), displayXML);

    //RemapInformation - optional
    if (display->remapInformation && (display->pixelType == PixelType::RGB8LU
            || display->pixelType == PixelType::MONO8LU))
    {
        xml::lite::Element* remapInfoXML = newElement("RemapInformation",
                                                      displayXML);

        xml::lite::Element* remapXML;
        if (display->pixelType == PixelType::RGB8LU)
        {
            remapXML = newElement("ColorDisplayRemap", remapInfoXML);
            createLUT("RemapLUT", display->remapInformation->remapLUT, remapXML);
        }
        else
        {
            remapXML = newElement("MonochromeDisplayRemap", remapInfoXML);
            createString(
                         "RemapType",
                         str::toString(
                                       ((MonochromeDisplayRemap*) display->remapInformation)->remapType),
                         remapXML);
            createLUT("RemapLUT", display->remapInformation->remapLUT, remapXML);
            //should we do this cast (i.e. assume the pixelType is correct)?
            addParameters(
                          "RemapParameter",
                          ((MonochromeDisplayRemap*) display->remapInformation)->remapParameters,
                          remapXML);
        }
    }

    //MagnificationMethod - optional
    if (display->magnificationMethod != MagnificationMethod::MAG_NOT_SET)
    {
        createString("MagnificationMethod",
                     str::toString(display->magnificationMethod), displayXML);
    }

    //DecimationMethod - optional
    if (display->decimationMethod != DecimationMethod::DEC_NOT_SET)
    {
        createString("DecimationMethod",
                     str::toString(display->decimationMethod), displayXML);
    }

    //DRAHistogramOverrides - optional
    if (display->histogramOverrides)
    {
        xml::lite::Element *histo = newElement("DRAHistogramOverrides",
                                               displayXML);
        createInt("ClipMin", display->histogramOverrides->clipMin, histo);
        createInt("ClipMax", display->histogramOverrides->clipMax, histo);
    }

    if (display->monitorCompensationApplied)
    {
        //MonitorCompensationApplied - optional
        xml::lite::Element *monComp = newElement("MonitorCompensationApplied",
                                                 displayXML);
        createDouble("Gamma", display->monitorCompensationApplied->gamma,
                     monComp);
        createDouble("XMin", display->monitorCompensationApplied->xMin, monComp);
    }

    addParameters("DisplayExtension", display->displayExtensions, displayXML);
    return displayXML;
}

xml::lite::Element*
DerivedXMLControl::geographicAndTargetToXML(
                                            GeographicAndTarget* geographicAndTarget,
                                            xml::lite::Element* parent)
{
    xml::lite::Element* geographicAndTargetXML =
            newElement("GeographicAndTarget", parent);

    geographicCoverageToXML(geographicAndTarget->geographicCoverage,
                            geographicAndTargetXML);

    //loop over TargetInformation
    for (std::vector<TargetInformation*>::iterator it =
            geographicAndTarget->targetInformation.begin(); it
            != geographicAndTarget->targetInformation.end(); ++it)
    {
        TargetInformation* ti = *it;
        xml::lite::Element* tiXML = newElement("TargetInformation",
                                               geographicAndTargetXML);

        addParameters("Identifier", ti->identifiers, tiXML);
        for (unsigned int i = 0; i < ti->footprints.size(); i++)
        {
            createFootprint("Footprint", "Vertex", ti->footprints[i], tiXML);
        }
        addParameters("TargetInformationExtension",
                      ti->targetInformationExtensions, tiXML);
    }
    return geographicAndTargetXML;
}

xml::lite::Element* DerivedXMLControl::geographicCoverageToXML(
                                                               GeographicCoverage* geoCoverage,
                                                               xml::lite::Element* parent)
{
    //GeographicAndTarget
    xml::lite::Element* geoCoverageXML = newElement("GeographicCoverage",
                                                    parent);

    //Georegion Identifiers
    addParameters("GeoregionIdentifier", geoCoverage->georegionIdentifiers,
                  geoCoverageXML);

    //Footprint
    createFootprint("Footprint", "Vertex", geoCoverage->footprint,
                    geoCoverageXML);

    // GeographicInfo
    if (geoCoverage->geographicInformation)
    {
        xml::lite::Element* geoInfoXML = newElement("GeographicInfo",
                                                    geoCoverageXML);

        for (unsigned int i = 0, numCC =
                geoCoverage->geographicInformation->countryCodes.size(); i
                < numCC; ++i)
        {
            createString("CountryCode",
                         geoCoverage->geographicInformation->countryCodes[i],
                         geoInfoXML);
        }

        createString("SecurityInfo",
                     geoCoverage->geographicInformation->securityInformation,
                     geoInfoXML);
        addParameters(
                      "GeographicInfoExtension",
                      geoCoverage->geographicInformation->geographicInformationExtensions,
                      geoInfoXML);
    }
    else
    {
        //loop over SubRegions
        for (std::vector<GeographicCoverage*>::iterator it =
                geoCoverage->subRegion.begin(); it
                != geoCoverage->subRegion.end(); ++it)
        {
            xml::lite::Element* subRegionXML =
                    geographicCoverageToXML(*it, geoCoverageXML);
            subRegionXML->setQName("SubRegion");
        }
    }
    return geoCoverageXML;
}

xml::lite::Element*
DerivedXMLControl::measurementToXML(Measurement* measurement,
                                    xml::lite::Element* parent)
{
    xml::lite::Element* measurementXML = newElement("Measurement", parent);

    Projection* projection = measurement->projection;
    xml::lite::Element* projectionXML = newElement("", measurementXML);
    xml::lite::Element* referencePointXML = newElement("ReferencePoint",
                                                       projectionXML);
    setAttribute(referencePointXML, "name", projection->referencePoint.name);

    //ECEF
    createVector3D("ECEF", projection->referencePoint.ecef, referencePointXML);
    //Point
    createRowCol("Point", projection->referencePoint.rowCol, referencePointXML);

    switch (projection->projectionType)
    {
    case ProjectionType::PROJECTION_PLANE:
    {
        projectionXML->setLocalName("PlaneProjection");
        PlaneProjection* planeProj = (PlaneProjection*) projection;

        //TimeCOAPoly
        xml::lite::Element* timeCOAPolyXML =
                createPoly2D("TimeCOAPoly", planeProj->timeCOAPoly,
                             projectionXML);

        createRowCol("SampleSpacing", planeProj->sampleSpacing, projectionXML);

        xml::lite::Element* productPlaneXML = newElement("ProductPlane",
                                                         projectionXML);

        //RowBasis
        createVector3D("RowUnitVector", planeProj->productPlane.rowUnitVector,
                       productPlaneXML);

        //ColBasis
        createVector3D("ColUnitVector", planeProj->productPlane.colUnitVector,
                       productPlaneXML);

    }
        break;

    case ProjectionType::PROJECTION_GEOGRAPHIC:
    {
        projectionXML->setLocalName("GeographicProjection");
        GeographicProjection* geographicProj =
                (GeographicProjection*) projection;

        //TimeCOAPoly
        xml::lite::Element* timeCOAPolyXML =
                createPoly2D("TimeCOAPoly", geographicProj->timeCOAPoly,
                             projectionXML);

        //TODO set a parent
        xml::lite::Element* sampleSpacingXML =
                createRowCol("SampleSpacing", geographicProj->sampleSpacing);

    }
        break;

    case ProjectionType::PROJECTION_CYLINDRICAL:
    {
        projectionXML->setLocalName("CylindricalProjection");

        CylindricalProjection* cylindricalProj =
                (CylindricalProjection*) projection;

        //TimeCOAPoly
        xml::lite::Element* timeCOAPolyXML =
                createPoly2D("TimeCOAPoly", cylindricalProj->timeCOAPoly,
                             projectionXML);

        createRowCol("SampleSpacing", cylindricalProj->sampleSpacing,
                     projectionXML);

        if (cylindricalProj->curvatureRadius != Init::undefined<double>())
        {
            createDouble("CurvatureRadius", cylindricalProj->curvatureRadius,
                         projectionXML);
        }

    }
        break;

    case ProjectionType::PROJECTION_POLYNOMIAL:
    {
        projectionXML->setLocalName("PolynomialProjection");
        PolynomialProjection* polyProj = (PolynomialProjection*) projection;
        createPoly2D("RowColToLat", polyProj->rowColToLat, projectionXML);
        createPoly2D("RowColToLon", polyProj->rowColToLon, projectionXML);

        if (polyProj->rowColToAlt.orderX() >= 0
                && polyProj->rowColToAlt.orderY() >= 0)
        {
            createPoly2D("RowColToAlt", polyProj->rowColToAlt, projectionXML);
        }
        createPoly2D("LatLonToRow", polyProj->latLonToRow, projectionXML);
        createPoly2D("LatLonToCol", polyProj->latLonToCol, projectionXML);
    }
        break;

    default:
        throw except::Exception(Ctxt("Unknown projection type!"));
    }

    //Pixel Footprint
    createRowCol("PixelFootprint", measurement->pixelFootprint, measurementXML);

    //ARPPoly
    createPolyXYZ("ARPPoly", measurement->arpPoly, measurementXML);

    return measurementXML;
}

xml::lite::Element*
DerivedXMLControl::exploitationFeaturesToXML(
                                             ExploitationFeatures* exploitationFeatures,
                                             xml::lite::Element* parent)
{

    xml::lite::Element* exploitationFeaturesXML =
            newElement("ExploitationFeatures", parent);

    for (unsigned int i = 0; i < exploitationFeatures->collections.size(); ++i)
    {
        Collection* collection = exploitationFeatures->collections[i];
        xml::lite::Element* collectionXML = newElement("Collection",
                                                       exploitationFeaturesXML);
        setAttribute(collectionXML, "identifier", collection->identifier);

        xml::lite::Element* informationXML = newElement("Information",
                                                        collectionXML);

        createString("SensorName", collection->information->sensorName,
                     informationXML);

        xml::lite::Element* radarModeXML = newElement("RadarMode",
                                                      informationXML);
        //ModeType - Incomplete?
        createString("ModeType",
                     str::toString(collection->information->radarMode),
                     radarModeXML);
        if (collection->information->radarModeID
                != Init::undefined<std::string>())
        {
            createString("ModeID", collection->information->radarModeID,
                         radarModeXML);
        }

        //CollectionDateTime
        createDateTime("CollectionDateTime",
                       collection->information->collectionDateTime,
                       informationXML);

        //LocalDateTime
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

        //Resolution
        createRangeAzimuth("Resolution", collection->information->resolution,
                           informationXML);

        //InputROI
        if (collection->information->inputROI != NULL)
        {
            xml::lite::Element* roiXML = newElement("InputROI", informationXML);
            createRowCol("Size", collection->information->inputROI->size,
                         roiXML);
            createRowCol("UpperLeft",
                         collection->information->inputROI->upperLeft, roiXML);
        }

        if (collection->information->polarization != NULL)
        {
            //TODO add parent - informationXML?
            xml::lite::Element* polXML = newElement("Polarization");
            createString(
                         "TxPolarization",
                         str::toString(
                                       collection->information->polarization->txPolarization),
                         polXML);
            createDouble(
                         "RcvPolarization",
                         collection->information->polarization->rcvPolarization,
                         polXML);
        }

        //Geometry
        Geometry* geom = collection->geometry;

        if (geom != NULL)
        {
            xml::lite::Element* geometryXML = newElement("Geometry",
                                                         collectionXML);

            if (geom->azimuth != Init::undefined<double>())
            {
                createDouble("Azimuth", geom->azimuth, geometryXML);
            }

            if (geom->slope != Init::undefined<double>())
            {
                createDouble("Slope", geom->slope, geometryXML);
            }

            if (geom->squint != Init::undefined<double>())
            {
                createDouble("Squint", geom->squint, geometryXML);
            }

            if (geom->graze != Init::undefined<double>())
            {
                createDouble("Graze", geom->graze, geometryXML);
            }

            if (geom->tilt != Init::undefined<double>())
            {
                createDouble("Tilt", geom->tilt, geometryXML);
            }
        }

        //Phenomenology
        Phenomenology* phenom = collection->phenomenology;
        if (phenom != NULL)
        {
            xml::lite::Element* phenomenologyXML = newElement("Phenomenology",
                                                              collectionXML);

            if (phenom->shadow != Init::undefined<AngleMagnitude>())
            {
                xml::lite::Element* shadow = newElement("Shadow",
                                                        phenomenologyXML);
                createDouble("Angle", phenom->shadow.angle, shadow);
                createDouble("Magnitude", phenom->shadow.magnitude, shadow);
            }

            if (phenom->layover != Init::undefined<AngleMagnitude>())
            {
                xml::lite::Element* layover = newElement("Layover",
                                                         phenomenologyXML);
                createDouble("Angle", phenom->layover.angle, layover);
                createDouble("Magnitude", phenom->layover.magnitude, layover);
            }

            if (phenom->multiPath != Init::undefined<double>())
            {
                createDouble("MultiPath", phenom->multiPath, phenomenologyXML);
            }

            if (phenom->groundTrack != Init::undefined<double>())
            {
                createDouble("GroundTrack", phenom->groundTrack,
                             phenomenologyXML);
            }
        }
    }

    //Product
    xml::lite::Element* productXML = newElement("Product",
                                                exploitationFeaturesXML);

    //Resolution
    createRowCol("Resolution", exploitationFeatures->product.resolution,
                 productXML);

    //North - optional
    if (exploitationFeatures->product.north != Init::undefined<double>())
    {
        createDouble("North", exploitationFeatures->product.north, productXML);
    }
    return exploitationFeaturesXML;
}

xml::lite::Document* DerivedXMLControl::toXML(Data* data)
{
    if (data->getDataClass() != DataClass::DATA_DERIVED)
    {
        throw except::Exception("Data must be derived");
    }
    xml::lite::Document* doc = new xml::lite::Document();
    xml::lite::Element* root = newElement("SIDD");
    doc->setRootElement(root);

    DerivedData *derived = (DerivedData*) data;

    productCreationToXML(derived->productCreation, root);
    displayToXML(derived->display, root);
    geographicAndTargetToXML(derived->geographicAndTarget, root);
    measurementToXML(derived->measurement, root);
    exploitationFeaturesToXML(derived->exploitationFeatures, root);

    if (derived->productProcessing)
    {
        productProcessingToXML(derived->productProcessing, root);
    }
    if (derived->errorStatistics)
    {
        errorStatisticsToXML(derived->errorStatistics, root);
    }
    if (derived->radiometric)
    {
        radiometricToXML(derived->radiometric, root);
    }
    return doc;
}

xml::lite::Element* DerivedXMLControl::createLUT(std::string name, LUT *lut,
                                                 xml::lite::Element* parent)
{
    //     unsigned char* table;
    //     unsigned int numEntries;
    //     unsigned int elementSize;

    xml::lite::Element* lutElement = newElement(name, parent);
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
            //must add 128 to print ASCII
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

xml::lite::Element* DerivedXMLControl::productProcessingToXML(
                                                              ProductProcessing* productProcessing,
                                                              xml::lite::Element* parent)
{
    xml::lite::Element* productProcessingXML = newElement("ProductProcessing",
                                                          parent);

    for (std::vector<ProcessingModule*>::iterator it =
            productProcessing->processingModules.begin(); it
            != productProcessing->processingModules.end(); ++it)
    {
        processingModuleToXML(*it, productProcessingXML);
    }
    return productProcessingXML;
}

xml::lite::Element* DerivedXMLControl::processingModuleToXML(
                                                             ProcessingModule* procMod,
                                                             xml::lite::Element* parent)
{
    xml::lite::Element* procModXML = newElement("ProcessingModule", parent);
    xml::lite::Element *modNameXML = createString("ModuleName",
                                                  procMod->moduleName.str(),
                                                  procModXML);
    setAttribute(modNameXML, "name", procMod->moduleName.getName());

    if (!procMod->processingModules.empty())
    {
        for (std::vector<ProcessingModule*>::iterator it =
                procMod->processingModules.begin(); it
                != procMod->processingModules.end(); ++it)
        {
            processingModuleToXML(*it, procModXML);
        }
    }
    else
    {
        addParameters("ModuleParameter", procMod->moduleParameters, procModXML);
    }
    return procModXML;

}

xml::lite::Element* DerivedXMLControl::downstreamReprocessingToXML(
                                                                   DownstreamReprocessing* downstreamReproc,
                                                                   xml::lite::Element* parent)
{
    xml::lite::Element* epXML = newElement("DownstreamReprocessing", parent);

    GeometricChip *geoChip = downstreamReproc->geometricChip;
    if (geoChip)
    {
        xml::lite::Element* geoChipXML = newElement("GeometricChip", epXML);
        createRowCol("ChipSize", geoChip->chipSize, geoChipXML);
        createRowCol("OriginalUpperLeftCoordinate",
                     geoChip->originalUpperLeftCoordinate, geoChipXML);
        createRowCol("OriginalUpperRightCoordinate",
                     geoChip->originalUpperRightCoordinate, geoChipXML);
        createRowCol("OriginalLowerLeftCoordinate",
                     geoChip->originalLowerLeftCoordinate, geoChipXML);
        createRowCol("OriginalLowerRightCoordinate",
                     geoChip->originalLowerRightCoordinate, geoChipXML);
    }
    if (!downstreamReproc->processingEvents.empty())
    {
        for (std::vector<ProcessingEvent*>::iterator it =
                downstreamReproc->processingEvents.begin(); it
                != downstreamReproc->processingEvents.end(); ++it)
        {
            ProcessingEvent *procEvent = *it;
            xml::lite::Element* procEventXML = newElement("ProcessingEvent",
                                                          epXML);
            createString("ApplicationName", procEvent->applicationName,
                         procEventXML);
            createDateTime("AppliedDateTime", procEvent->appliedDateTime,
                           procEventXML);

            if (procEvent->interpolationMethod.empty())
            {
                createString("InterpolationMethod",
                             procEvent->interpolationMethod, procEventXML);
            }
            addParameters("Descriptor", procEvent->descriptor, procEventXML);
        }
    }
    return epXML;
}

void DerivedXMLControl::xmlToProcessingModule(xml::lite::Element* procXML,
                                              ProcessingModule* procMod)
{
    xml::lite::Element *moduleName = getFirstAndOnly(procXML, "ModuleName");
    procMod->moduleName = Parameter(moduleName->getCharacterData());
    procMod->moduleName.setName(moduleName->getAttributes().getValue("name"));

    parseParameters(procXML, "ModuleParameter", procMod->moduleParameters);

    std::vector<xml::lite::Element*> procModuleXML;
    procXML->getElementsByTagName("ProcessingModule", procModuleXML);

    for (unsigned int i = 0, size = procModuleXML.size(); i < size; ++i)
    {
        ProcessingModule *pm = new ProcessingModule();
        xmlToProcessingModule(procModuleXML[i], pm);
        procMod->processingModules.push_back(pm);
    }
}

void DerivedXMLControl::xmlToProductProcessing(
                                               xml::lite::Element* elem,
                                               ProductProcessing* productProcessing)
{
    std::vector<xml::lite::Element*> procModuleXML;
    elem->getElementsByTagName("ProcessingModule", procModuleXML);

    for (unsigned int i = 0, size = procModuleXML.size(); i < size; ++i)
    {
        ProcessingModule *procMod = new ProcessingModule();
        xmlToProcessingModule(procModuleXML[i], procMod);
        productProcessing->processingModules.push_back(procMod);
    }
}

void DerivedXMLControl::xmlToDownstreamReprocessing(
                                                    xml::lite::Element* elem,
                                                    DownstreamReprocessing* downstreamReproc)
{
    xml::lite::Element *geometricChipXML = getOptional(elem, "GeometricChip");
    std::vector<xml::lite::Element*> procEventXML;
    elem->getElementsByTagName("ProcessingEvent", procEventXML);
    xml::lite::Element *processingEventXML = getOptional(elem,
                                                         "ProcessingEvent");

    if (geometricChipXML)
    {
        GeometricChip *chip = downstreamReproc->geometricChip
                = new GeometricChip();

        parseRowColInt(getFirstAndOnly(geometricChipXML, "ChipSize"),
                       chip->chipSize);

        parseRowColDouble(getFirstAndOnly(geometricChipXML,
                                          "OriginalUpperLeftCoordinate"),
                          chip->originalUpperLeftCoordinate);

        parseRowColDouble(getFirstAndOnly(geometricChipXML,
                                          "OriginalUpperRightCoordinate"),
                          chip->originalUpperRightCoordinate);

        parseRowColDouble(getFirstAndOnly(geometricChipXML,
                                          "OriginalLowerLeftCoordinate"),
                          chip->originalLowerLeftCoordinate);

        parseRowColDouble(getFirstAndOnly(geometricChipXML,
                                          "OriginalLowerRightCoordinate"),
                          chip->originalLowerRightCoordinate);
    }

    for (unsigned int i = 0, size = procEventXML.size(); i < size; ++i)
    {
        ProcessingEvent *procEvent = new ProcessingEvent();
        downstreamReproc->processingEvents.push_back(procEvent);
        xml::lite::Element *peXML = procEventXML[i];

        procEvent->applicationName
                = getFirstAndOnly(peXML, "ApplicationName")->getCharacterData();
        parseDateTime(getFirstAndOnly(peXML, "AppliedDateTime"),
                      procEvent->appliedDateTime);

        xml::lite::Element *tmpElem = getOptional(peXML, "InterpolationMethod");
        if (tmpElem)
        {
            procEvent->interpolationMethod = tmpElem->getCharacterData();
        }
        parseParameters(peXML, "Descriptor", procEvent->descriptor);
    }
}
