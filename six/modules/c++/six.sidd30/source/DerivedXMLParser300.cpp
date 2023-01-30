/* =========================================================================
 * This file is part of six.sidd30-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2015, MDA Information Systems LLC
* (C) Copyright 2021, Maxar Technologies, Inc.
 *
 * six.sidd30-c++ is free software; you can redistribute it and/or modify
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

#include <assert.h>

#include <sstream>

#include <gsl/gsl.h>

#include <six/SICommonXMLParser10x.h>
#include <six/sidd30/DerivedDataBuilder.h>
#include <six/sidd30/DerivedXMLParser300.h>
#include <six/sidd30/DerivedXMLParser200.h>

using XMLElem = xml::lite::Element*;

namespace six
{
namespace sidd30
{
static const char VERSION[] = "3.0.0";
static const char SI_COMMON_URI[] = "urn:SICommon:1.0";
static const char ISM_URI[] = "urn:us:gov:ic:ism:13";

//DerivedXMLParser300::DerivedXMLParser300(std::unique_ptr<logging::Logger>&& log) :
//    DerivedXMLParser(VERSION,
//        std::make_unique<six::SICommonXMLParser10x>(versionToURI(VERSION), false, SI_COMMON_URI, *log),
//        std::move(log)) {  }
DerivedXMLParser300::DerivedXMLParser300(logging::Logger& log) :
    DerivedXMLParser(VERSION,
        std::make_unique<six::SICommonXMLParser10x>(versionToURI(VERSION), false, SI_COMMON_URI, log),
        log) {  }

DerivedData* DerivedXMLParser300::fromXML(
        const xml::lite::Document* doc) const
{
    assert(doc != nullptr);
    return fromXML(*doc).release();
}
std::unique_ptr<DerivedData> DerivedXMLParser300::fromXML(const xml::lite::Document& doc) const
{
    const xml::lite::Element* const pRoot = doc.getRootElement();
    const auto& root = *pRoot;

    auto& productCreationElem        = getFirstAndOnly(root, "ProductCreation");
    auto& displayElem                = getFirstAndOnly(root, "Display");
    auto& geoDataElem                = getFirstAndOnly(root, "GeoData");
    auto& measurementElem            = getFirstAndOnly(root, "Measurement");
    auto& exploitationFeaturesElem   = getFirstAndOnly(root, "ExploitationFeatures");
    XMLElem productProcessingElem      = getOptional(root, "ProductProcessing");
    XMLElem downstreamReprocessingElem = getOptional(root, "DownstreamReprocessing");
    XMLElem errorStatisticsElem        = getOptional(root, "ErrorStatistics");
    XMLElem radiometricElem            = getOptional(root, "Radiometric");
    XMLElem matchInfoElem              = getOptional(root, "MatchInfo");
    XMLElem compressionElem            = getOptional(root, "Compression");
    XMLElem dedElem                    = getOptional(root, "DigitalElevationData");
    XMLElem annotationsElem            = getOptional(root, "Annotations");


    DerivedDataBuilder builder;
    std::unique_ptr<DerivedData> data(builder.steal()); //steal it

    // see if PixelType has MONO or RGB
    const PixelType pixelType = six::toType<PixelType>(
            getFirstAndOnly(displayElem, "PixelType").getCharacterData());
    builder.addDisplay(pixelType);

    // create GeoData
    builder.addGeoData();

    // create Measurement
    const auto projType = DerivedXMLParser200::getProjectionType(measurementElem);
    builder.addMeasurement(projType);

    // create ExploitationFeatures
    std::vector<XMLElem> elements;
    exploitationFeaturesElem.getElementsByTagName("ExploitationFeatures", elements);
    builder.addExploitationFeatures(gsl::narrow<unsigned int>(elements.size()));

    parseProductCreationFromXML(productCreationElem, *data->productCreation);
    parseDisplayFromXML(displayElem, *data->display);
    parseGeoDataFromXML(geoDataElem, *data->geoData);
    parseMeasurementFromXML(&measurementElem, data->measurement.get());
    parseExploitationFeaturesFromXML(&exploitationFeaturesElem, data->exploitationFeatures.get());

    if (productProcessingElem)
    {
        builder.addProductProcessing();
        parseProductProcessingFromXML(*productProcessingElem, *data->productProcessing);
    }
    if (downstreamReprocessingElem)
    {
        builder.addDownstreamReprocessing();
        parseDownstreamReprocessingFromXML(*downstreamReprocessingElem, *data->downstreamReprocessing);
    }
    if (errorStatisticsElem)
    {
        builder.addErrorStatistics();
        common().parseErrorStatisticsFromXML(*errorStatisticsElem, *data->errorStatistics);
    }
    if (radiometricElem)
    {
        builder.addRadiometric();
        common().parseRadiometryFromXML(radiometricElem, data->radiometric.get());
    }
    if (matchInfoElem)
    {
        builder.addMatchInformation();
        common().parseMatchInformationFromXML(matchInfoElem, data->matchInformation.get());
    }
    if (compressionElem)
    {
        builder.addCompression();
        parseCompressionFromXML(*compressionElem, *data->compression);
    }
    if (dedElem)
    {
        builder.addDigitalElevationData();
        parseDigitalElevationDataFromXML(*dedElem, *data->digitalElevationData);
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

xml::lite::Document* DerivedXMLParser300::toXML(const DerivedData* derived) const
{
    xml::lite::Document* doc = new xml::lite::Document();
    XMLElem root = newElement("SIDD");
    doc->setRootElement(root);

    convertProductCreationToXML(derived->productCreation.get(), root);
    convertDisplayToXML(*derived->display, root);
    DerivedXMLParser200::convertGeoDataToXML(*this, *derived->geoData, *root);
    convertMeasurementToXML(derived->measurement.get(), root);
    convertExploitationFeaturesToXML(derived->exploitationFeatures.get(), root);

    // optional
    if (derived->downstreamReprocessing.get())
    {
        convertDownstreamReprocessingToXML(
                derived->downstreamReprocessing.get(), root);
    }
    // optional
    if (derived->errorStatistics.get())
    {
        common().convertErrorStatisticsToXML(derived->errorStatistics.get(), root);
    }
    // optional
    if (derived->matchInformation.get())
    {
        common().convertMatchInformationToXML(*derived->matchInformation, root);
    }
    // optional
    if (derived->radiometric.get())
    {
        common().convertRadiometryToXML(derived->radiometric.get(), root);
    }
    // optional
    if (derived->compression.get())
    {
        DerivedXMLParser200::convertCompressionToXML(*this, *derived->compression, *root);
    }
    // optional
    if (derived->digitalElevationData.get())
    {
        DerivedXMLParser200::convertDigitalElevationDataToXML(*this, *derived->digitalElevationData, *root);
    }
    // optional
    if (derived->productProcessing.get())
    {
        convertProductProcessingToXML(derived->productProcessing.get(), root);
    }
    // optional
    if (!derived->annotations.empty())
    {
        XMLElem annotationsElem = newElement("Annotations", root);
        for (const auto& pAnnotation : derived->annotations)
        {
            convertAnnotationToXML(pAnnotation.get(), annotationsElem);
        }
    }

    //set the ElemNS
    root->setNamespacePrefix("", getDefaultURI());
    root->setNamespacePrefix("si", xml::lite::Uri(SI_COMMON_URI));
    root->setNamespacePrefix("sfa", xml::lite::Uri(SFA_URI));
    root->setNamespacePrefix("ism", xml::lite::Uri(ISM_URI));

    return doc;
}
std::unique_ptr<xml::lite::Document> DerivedXMLParser300::toXML(const DerivedData& data) const
{
    return std::unique_ptr<xml::lite::Document>(toXML(&data));
}

void DerivedXMLParser300::parseDerivedClassificationFromXML(
        const xml::lite::Element* classificationElem,
        DerivedClassification& classification) const
{
    DerivedXMLParser::parseDerivedClassificationFromXML(classificationElem, classification);
    const auto& classificationAttributes = classificationElem->getAttributes();

    getAttributeList(classificationAttributes,
        "ism:compliesWith",
        classification.compliesWith);
    // optional
    getAttributeIfExists(classificationAttributes,
        "ism:exemptFrom",
        classification.exemptFrom);
    // optional
    getAttributeIfExists(classificationAttributes,
        "ism:joint",
        classification.joint);
    // optional
    getAttributeListIfExists(classificationAttributes,
        "ism:atomicEnergyMarkings",
        classification.atomicEnergyMarkings);
    // optional
    getAttributeListIfExists(classificationAttributes,
        "ism:displayOnlyTo",
        classification.displayOnlyTo);
    // optional
    getAttributeIfExists(classificationAttributes,
        "ism:noticeType",
        classification.noticeType);
    // optional
    getAttributeIfExists(classificationAttributes,
        "ism:noticeReason",
        classification.noticeReason);
    // optional
    getAttributeIfExists(classificationAttributes,
        "ism:noticeDate",
        classification.noticeDate);
    // optional
    getAttributeIfExists(classificationAttributes,
        "ism:unregisteredNoticeType",
        classification.unregisteredNoticeType);
    // optional
    getAttributeIfExists(classificationAttributes,
        "ism:externalNotice",
        classification.externalNotice);
}

void DerivedXMLParser300::parseCompressionFromXML(const xml::lite::Element& compressionElem,
                                                 Compression& compression) const
{
    auto& j2kElem = getFirstAndOnly(compressionElem, "J2K");
    auto& originalElem = getFirstAndOnly(j2kElem, "Original");
    auto parsedElem   = getOptional(j2kElem, "Parsed");

    parseJ2KCompression(originalElem, compression.original);
    if (parsedElem)
    {
        compression.parsed.reset(new J2KCompression());
        parseJ2KCompression(*parsedElem, *compression.parsed);
    }
}

void DerivedXMLParser300::parseJ2KCompression(const xml::lite::Element& j2kElem,
                                              J2KCompression& j2k) const
{
    parseInt(getFirstAndOnly(j2kElem, "NumWaveletLevels"), j2k.numWaveletLevels);
    parseInt(getFirstAndOnly(j2kElem, "NumBands"), j2k.numBands);

    auto& layerInfoElems = getFirstAndOnly(j2kElem, "LayerInfo");
    std::vector<XMLElem> layerElems;
    layerInfoElems.getElementsByTagName("Layer", layerElems);

    const auto numLayers = layerElems.size();
    j2k.layerInfo.resize(numLayers);

    for (size_t ii = 0; ii < layerElems.size(); ++ii)
    {
        parseDouble(getFirstAndOnly(layerElems[ii], "Bitrate"), j2k.layerInfo[ii].bitRate);
    }
}

void DerivedXMLParser300::parseDisplayFromXML(const xml::lite::Element& displayElem,
                                              Display& display) const
{
    //pixelType previously set
    parseUInt(getFirstAndOnly(displayElem, "NumBands"), display.numBands);
    XMLElem bandDisplayElem = getOptional(displayElem, "DefaultBandDisplay");
    if (bandDisplayElem)
    {
        parseUInt(*bandDisplayElem, display.defaultBandDisplay);
    }

    std::vector<XMLElem> nonInteractiveProcessingElems;
    displayElem.getElementsByTagName("NonInteractiveProcessing", nonInteractiveProcessingElems);
    display.nonInteractiveProcessing.resize(nonInteractiveProcessingElems.size());
    for (size_t ii = 0; ii < nonInteractiveProcessingElems.size(); ++ii)
    {
        display.nonInteractiveProcessing[ii].reset(new NonInteractiveProcessing());
        parseNonInteractiveProcessingFromXML(*nonInteractiveProcessingElems[ii],
                *display.nonInteractiveProcessing[ii]);
    }

    std::vector<XMLElem> interactiveProcessingElems;
    displayElem.getElementsByTagName("InteractiveProcessing", interactiveProcessingElems);
    display.interactiveProcessing.resize(interactiveProcessingElems.size());
    for (size_t ii = 0; ii < interactiveProcessingElems.size(); ++ii)
    {
        display.interactiveProcessing[ii].reset(new InteractiveProcessing());
        parseInteractiveProcessingFromXML(*interactiveProcessingElems[ii],
                *display.interactiveProcessing[ii]);
    }

    std::vector<XMLElem> extensions;
    displayElem.getElementsByTagName("DisplayExtention", extensions);
    for (const auto& extension : extensions)
    {
        std::string name;
        getAttributeIfExists(extension->getAttributes(), "name", name);
        std::string value;
        parseString(*extension, value);
        Parameter parameter(value);
        parameter.setName(name);
        display.displayExtensions.push_back(parameter);
    }
}

void DerivedXMLParser300::parseNonInteractiveProcessingFromXML(
            const xml::lite::Element& procElem,
            NonInteractiveProcessing& nonInteractiveProcessing) const
{
    auto& productGenerationOptions = getFirstAndOnly(procElem, "ProductGenerationOptions");
    auto& rrdsElem = getFirstAndOnly(procElem, "RRDS");

    parseProductGenerationOptionsFromXML(productGenerationOptions, nonInteractiveProcessing.productGenerationOptions);
    parseRRDSFromXML(rrdsElem, nonInteractiveProcessing.rrds);
}

void DerivedXMLParser300::parseProductGenerationOptionsFromXML(
            const xml::lite::Element& optionsElem,
            ProductGenerationOptions& options) const
{
    XMLElem bandElem = getOptional(optionsElem, "BandEqualization");
    XMLElem restoration = getOptional(optionsElem, "ModularTransferFunctionRestoration");
    XMLElem remapElem = getOptional(optionsElem, "DataRemapping");
    XMLElem correctionElem = getOptional(optionsElem, "AsymmetricPixelCorrection");

    if (bandElem)
    {
        options.bandEqualization.reset(new BandEqualization());
        parseBandEqualizationFromXML(*bandElem, *options.bandEqualization);
    }

    if (restoration)
    {
        options.modularTransferFunctionRestoration.reset(new Filter());
        parseFilterFromXML(*restoration, *options.modularTransferFunctionRestoration);
    }

    if (remapElem)
    {
        options.dataRemapping.reset(new LookupTable());
        parseLookupTableFromXML(*remapElem, *options.dataRemapping);
    }

    if (correctionElem)
    {
        options.asymmetricPixelCorrection.reset(new Filter());
        parseFilterFromXML(*correctionElem, *options.asymmetricPixelCorrection);
    }
}

void DerivedXMLParser300::parseLookupTableFromXML(
            const xml::lite::Element& lookupElem,
            LookupTable& lookupTable) const
{
    parseString(getFirstAndOnly(lookupElem, "LUTName"), lookupTable.lutName);
    XMLElem customElem = getOptional(lookupElem, "Custom");
    XMLElem predefinedElem = getOptional(lookupElem, "Predefined");

    bool ok = false;
    if (customElem)
    {
        if (!predefinedElem)
        {
            ok = true;
            XMLElem lutInfoElem = getFirstAndOnly(customElem, "LUTInfo");
            const auto& attributes = lutInfoElem->getAttributes();
            size_t numBands;
            size_t size;
            getAttributeIfExists(attributes, "numLuts", numBands);
            getAttributeIfExists(attributes, "size", size);
            lookupTable.custom.reset(new LookupTable::Custom(size, numBands));
            std::vector<XMLElem> lutElems;
            lutInfoElem->getElementsByTagName("LUTValues", lutElems);

            for (size_t ii = 0; ii < lutElems.size(); ++ii)
            {
                auto lut = parseSingleLUT(*lutElems[ii], size);
                lookupTable.custom->lutValues[ii] = *lut;
            }
        }
    }
    else if (predefinedElem)
    {
        ok = true;
        lookupTable.predefined.reset(new LookupTable::Predefined());
        bool innerOk = false;
        XMLElem dbNameElem = getOptional(predefinedElem, "DatabaseName");
        XMLElem familyElem = getOptional(predefinedElem, "RemapFamily");
        XMLElem memberElem = getOptional(predefinedElem, "RemapMember");

        if (dbNameElem)
        {
            if (!familyElem && !memberElem)
            {
                innerOk = true;
                parseString(dbNameElem, lookupTable.predefined->databaseName);
            }
        }
        else if (familyElem && memberElem)
        {
            innerOk = true;
            parseInt(familyElem, lookupTable.predefined->remapFamily);
            parseInt(memberElem, lookupTable.predefined->remapMember);
        }
        if (innerOk == false)
        {
            throw except::Exception(Ctxt("Exactly one of databaseName or remapFamiy + remapMember must be set"));
        }
    }
    if (ok == false)
    {
        throw except::Exception(Ctxt("Exactly one of Custom or Predefined must be set for LookupTable"));
    }
}

void DerivedXMLParser300::parseBandEqualizationFromXML(const xml::lite::Element& bandElem,
                                                       BandEqualization& band) const
{
    std::string bandAlgo;
    parseString(getFirstAndOnly(bandElem, "Algorithm"), bandAlgo);

    if (bandAlgo == "1DLUT")
    {
        band.algorithm = BandEqualizationAlgorithm::LUT_1D;
    }
    else
    {
        band.algorithm = BandEqualizationAlgorithm::toType(bandAlgo);
    }

    std::vector<XMLElem> lutElems;
    bandElem.getElementsByTagName("BandLUT", lutElems);
    band.bandLUTs.resize(lutElems.size());
    for (size_t ii = 0; ii < lutElems.size(); ++ii)
    {
        band.bandLUTs[ii].reset(new LookupTable());
        parseLookupTableFromXML(*lutElems[ii], *band.bandLUTs[ii]);
    }
}

void DerivedXMLParser300::parseRRDSFromXML(const xml::lite::Element& rrdsElem,
            RRDS& rrds) const
{
    parseEnum(getFirstAndOnly(rrdsElem, "DownsamplingMethod"), rrds.downsamplingMethod);

    XMLElem antiAliasElem = getOptional(rrdsElem, "AntiAlias");
    if (antiAliasElem)
    {
        rrds.antiAlias.reset(new Filter());
        parseFilterFromXML(*antiAliasElem, *rrds.antiAlias);
    }

    XMLElem interpolationElem = getOptional(rrdsElem, "Interpolation");
    if (interpolationElem)
    {
        rrds.interpolation.reset(new Filter());
        parseFilterFromXML(*interpolationElem, *rrds.interpolation);
    }
}

void DerivedXMLParser300::parseFilterFromXML(const xml::lite::Element& filterElem,
    Filter& filter) const
{
    parseString(getFirstAndOnly(filterElem, "FilterName"), filter.filterName);
    XMLElem kernelElem = getOptional(filterElem, "FilterKernel");
    XMLElem bankElem = getOptional(filterElem, "FilterBank");

    bool ok = false;
    if (kernelElem)
    {
        if (!bankElem)
        {
            ok = true;
            filter.filterKernel.reset(new Filter::Kernel());
            parseKernelFromXML(kernelElem, *filter.filterKernel);
        }
    }
    else if (bankElem)
    {
        ok = true;
        filter.filterBank.reset(new Filter::Bank());
        parseBankFromXML(bankElem, *filter.filterBank);
    }
    if (!ok)
    {
        throw except::Exception(Ctxt("Exactly one of FilterKernel or FilterBank must be set"));
    }
    parseEnum(getFirstAndOnly(filterElem, "Operation"), filter.operation);
}

void DerivedXMLParser300::parsePredefinedFilterFromXML(const xml::lite::Element* predefinedElem,
     Filter::Predefined& predefined) const
{
    bool ok = false;
    XMLElem dbNameElem = getOptional(predefinedElem, "DatabaseName");
    XMLElem familyElem = getOptional(predefinedElem, "FilterFamily");
    XMLElem filterMember = getOptional(predefinedElem, "FilterMember");

    if (dbNameElem)
    {
        if (!familyElem && !filterMember)
        {
            ok = true;
            parseEnum(dbNameElem, predefined.databaseName);
        }
    }
    else if (familyElem && filterMember)
    {
        ok = true;

        parseInt(familyElem, predefined.filterFamily);
        parseInt(familyElem, predefined.filterMember);
    }
    if (!ok)
    {
        throw except::Exception(Ctxt(
            "Exactly one of either dbName or FilterFamily and "
            "FilterMember must be defined"));
    }
}

void DerivedXMLParser300::parseKernelFromXML(const xml::lite::Element* kernelElem,
     Filter::Kernel& kernel) const
{
    XMLElem predefinedElem = getOptional(kernelElem, "Predefined");
    XMLElem customElem = getOptional(kernelElem, "Custom");

    bool ok = false;
    if (predefinedElem)
    {
        if (!customElem)
        {
            ok = true;
            kernel.predefined.reset(new Filter::Predefined());
            parsePredefinedFilterFromXML(predefinedElem, *kernel.predefined);
        }
    }
    else if (customElem)
    {
        ok = true;
        kernel.custom.reset(new Filter::Kernel::Custom());
        XMLElem filterCoef = getFirstAndOnly(customElem, "FilterCoefficients");
        const auto& attributes = filterCoef->getAttributes();
        getAttributeIfExists(attributes, "numRows", kernel.custom->size.row);
        getAttributeIfExists(attributes, "numCols", kernel.custom->size.col);

        if (six::Init::isUndefined(kernel.custom->size.row) || six::Init::isUndefined(kernel.custom->size.col))
        {
            throw except::Exception("Expected row and col attributes in FilterCoefficients element of Custom");
        }

        std::vector<XMLElem> coefficients;
        filterCoef->getElementsByTagName("Coef", coefficients);
        const auto numCoefs = coefficients.size();
        kernel.custom->filterCoef.resize(numCoefs);
        for (size_t ii = 0; ii < numCoefs; ++ii)
        {
            parseDouble(coefficients[ii], kernel.custom->filterCoef[ii]);
        }
    }
    if (!ok)
    {
        throw except::Exception(Ctxt("Exactly one of Custom or Predefined must be set for FilterKernel"));
    }
}
void DerivedXMLParser300::parseBankFromXML(const xml::lite::Element* bankElem,
     Filter::Bank& bank) const
{
    XMLElem predefinedElem = getOptional(bankElem, "Predefined");
    XMLElem customElem = getOptional(bankElem, "Custom");

    bool ok = false;
    if (predefinedElem)
    {
        if (!customElem)
        {
            ok = true;
            bank.predefined.reset(new Filter::Predefined());
            parsePredefinedFilterFromXML(predefinedElem, *bank.predefined);
        }
    }
    else if (customElem)
    {
        bank.custom.reset(new Filter::Bank::Custom());
        ok = true;

        XMLElem filterCoef = getFirstAndOnly(customElem, "FilterCoefficients");
        const auto& attributes = filterCoef->getAttributes();
        getAttributeIfExists(attributes, "numPhasings", bank.custom->numPhasings);
        getAttributeIfExists(attributes, "numPoints", bank.custom->numPoints);

        std::vector<XMLElem> coefficients;
        filterCoef->getElementsByTagName("Coef", coefficients);
        const auto numCoefs = coefficients.size();
        bank.custom->filterCoef.resize(numCoefs);
        for (size_t ii = 0; ii < numCoefs; ++ii)
        {
            parseDouble(coefficients[ii], bank.custom->filterCoef[ii]);
        }
    }
    if (!ok)
    {
        throw except::Exception(Ctxt("Exactly one of Custom or Predefined must be set for FilterBank"));
    }
}

void DerivedXMLParser300::parseInteractiveProcessingFromXML(
            const xml::lite::Element& interactiveElem,
            InteractiveProcessing& interactive) const
{
    auto& geomElem = getFirstAndOnly(interactiveElem, "GeometricTransform");
    auto& sharpnessElem = getFirstAndOnly(interactiveElem,
            "SharpnessEnhancement");
    XMLElem colorElem = getOptional(interactiveElem, "ColorSpaceTransform");
    auto& dynamicElem = getFirstAndOnly(interactiveElem, "DynamicRangeAdjustment");
    XMLElem ttcElem = getOptional(interactiveElem, "TonalTransferCurve");

    interactive.geometricTransform = GeometricTransform();
    parseGeometricTransformFromXML(geomElem, interactive.geometricTransform);
    parseSharpnessEnhancementFromXML(sharpnessElem,
                                     interactive.sharpnessEnhancement);

    if (colorElem)
    {
        interactive.colorSpaceTransform.reset(new ColorSpaceTransform());
        parseColorSpaceTransformFromXML(*colorElem, *interactive.colorSpaceTransform);
    }

    parseDynamicRangeAdjustmentFromXML(dynamicElem, interactive.dynamicRangeAdjustment);

    if (ttcElem)
    {
        interactive.tonalTransferCurve.reset(new LookupTable());
        parseLookupTableFromXML(*ttcElem, *interactive.tonalTransferCurve);
    }
}

void DerivedXMLParser300::parseGeometricTransformFromXML(const xml::lite::Element& geomElem,
             GeometricTransform& transform) const
{
    auto& scalingElem = getFirstAndOnly(geomElem, "Scaling");
    parseFilterFromXML(getFirstAndOnly(scalingElem, "AntiAlias"),
        transform.scaling.antiAlias);
    parseFilterFromXML(getFirstAndOnly(scalingElem, "Interpolation"),
        transform.scaling.interpolation);

    auto& orientationElem = getFirstAndOnly(geomElem, "Orientation");
    parseEnum(getFirstAndOnly(orientationElem, "ShadowDirection"), transform.orientation.shadowDirection);
}

void DerivedXMLParser300::parseSharpnessEnhancementFromXML(
             const xml::lite::Element& sharpElem,
             SharpnessEnhancement& sharpness) const
{
    bool ok = false;
    XMLElem mTFCElem = getOptional(sharpElem,
                                   "ModularTransferFunctionCompensation");
    XMLElem mTFRElem = getOptional(sharpElem,
                                   "ModularTransferFunctionEnhancement");
    if (mTFCElem)
    {
        if (!mTFRElem)
        {
            ok = true;
            sharpness.modularTransferFunctionCompensation.reset(new Filter());
            parseFilterFromXML(*mTFCElem,
                               *sharpness.modularTransferFunctionCompensation);
        }
    }
    else if (mTFRElem)
    {
        ok = true;
        sharpness.modularTransferFunctionEnhancement.reset(new Filter());
        parseFilterFromXML(*mTFRElem,
                           *sharpness.modularTransferFunctionEnhancement);
    }
    if (!ok)
    {
        throw except::Exception(Ctxt(
                "Exactly one of modularTransferFunctionCompensation or "
                "modularTransferFunctionEnhancement must be set"));
    }
}

void DerivedXMLParser300::parseColorSpaceTransformFromXML(
            const xml::lite::Element& colorElem, ColorSpaceTransform& transform) const
{
    auto& manageElem = getFirstAndOnly(colorElem, "ColorManagementModule");

    std::string renderIntentStr;
    parseString(getFirstAndOnly(manageElem, "RenderingIntent"), renderIntentStr);
    if (renderIntentStr == "RELATIVE")
    {
        transform.colorManagementModule.renderingIntent = RenderingIntent::RELATIVE_INTENT;
    }
    else if (renderIntentStr == "ABSOLUTE")
    {
        transform.colorManagementModule.renderingIntent = RenderingIntent::ABSOLUTE_INTENT;
    }
    else
    {
        transform.colorManagementModule.renderingIntent = RenderingIntent::toType(renderIntentStr);
    }
    parseString(getFirstAndOnly(manageElem, "SourceProfile"),
                transform.colorManagementModule.sourceProfile);
    parseOptionalString(manageElem, "DisplayProfile", transform.colorManagementModule.displayProfile);
    parseOptionalString(manageElem, "ICCProfileSignature", transform.colorManagementModule.iccProfile);
}

void DerivedXMLParser300::parseDynamicRangeAdjustmentFromXML(
            const xml::lite::Element& rangeElem,
            DynamicRangeAdjustment& rangeAdjustment) const
{
    parseEnum(getFirstAndOnly(rangeElem, "AlgorithmType"), rangeAdjustment.algorithmType);
    parseInt(getFirstAndOnly(rangeElem, "BandStatsSource"), rangeAdjustment.bandStatsSource);

    XMLElem parameterElem = getOptional(rangeElem, "DRAParameters");
    XMLElem overrideElem = getOptional(rangeElem, "DRAOverrides");

    DerivedXMLParser200::validateDRAFields(rangeAdjustment.algorithmType,
                      parameterElem ? true : false, overrideElem ? true : false);

    if (parameterElem)
    {
        rangeAdjustment.draParameters.reset(new DynamicRangeAdjustment::DRAParameters());
        parseDouble(getFirstAndOnly(*parameterElem, "Pmin"), rangeAdjustment.draParameters->pMin);
        parseDouble(getFirstAndOnly(*parameterElem, "Pmax"), rangeAdjustment.draParameters->pMax);
        parseDouble(getFirstAndOnly(*parameterElem, "EminModifier"), rangeAdjustment.draParameters->eMinModifier);
        parseDouble(getFirstAndOnly(*parameterElem, "EmaxModifier"), rangeAdjustment.draParameters->eMaxModifier);
    }

    if (overrideElem)
    {
        rangeAdjustment.draOverrides.reset(new DynamicRangeAdjustment::DRAOverrides());
        parseDouble(getFirstAndOnly(*overrideElem, "Subtractor"), rangeAdjustment.draOverrides->subtractor);
        parseDouble(getFirstAndOnly(*overrideElem, "Multiplier"), rangeAdjustment.draOverrides->multiplier);
    }
}

XMLElem DerivedXMLParser300::convertDerivedClassificationToXML(
        const DerivedClassification& classification,
        XMLElem parent) const
{
    assert(parent != nullptr);
    return & DerivedXMLParser200::convertDerivedClassificationToXML(*this, classification, *parent);
}

XMLElem DerivedXMLParser300::convertMeasurementToXML(const Measurement* measurement,
    XMLElem parent) const
{
    assert(parent != nullptr);
    assert(measurement != nullptr);
    return &DerivedXMLParser200::convertMeasurementToXML(*this, *measurement, *parent);
}

XMLElem DerivedXMLParser300::convertExploitationFeaturesToXML(
    const ExploitationFeatures* exploitationFeatures,
    XMLElem parent) const
{
    assert(exploitationFeatures != nullptr);
    assert(parent != nullptr);
    return &DerivedXMLParser200::convertExploitationFeaturesToXML(*this, *exploitationFeatures, *parent);
}

XMLElem DerivedXMLParser300::convertDisplayToXML(
        const Display& display,
        XMLElem parent) const
{
    assert(parent != nullptr);
    return &DerivedXMLParser200::convertDisplayToXML(*this, display, *parent);
}

void DerivedXMLParser300::parseGeoDataFromXML(
    const xml::lite::Element& geoDataXML, GeoDataBase& geoData) const
{
    common().parseEarthModelType(getFirstAndOnly(geoDataXML, "EarthModel"), geoData.earthModel);

    common().parseFootprint(getFirstAndOnly(geoDataXML, "ImageCorners"), "ICP", geoData.imageCorners);

    XMLElem tmpElem = getOptional(geoDataXML, "ValidData");
    if (tmpElem != nullptr)
    {
        common().parseLatLons(tmpElem, "Vertex", geoData.validData);
    }

    std::vector <XMLElem> geoInfosXML;
    geoDataXML.getElementsByTagName("GeoInfo", geoInfosXML);

    //optional
    size_t idx(geoData.geoInfos.size());
    geoData.geoInfos.resize(idx + geoInfosXML.size());

    for (std::vector<XMLElem>::const_iterator it = geoInfosXML.begin(); it
            != geoInfosXML.end(); ++it, ++idx)
    {
        geoData.geoInfos[idx].reset(new GeoInfo());
        common().parseGeoInfoFromXML(*it, geoData.geoInfos[idx].get());
    }
}


void DerivedXMLParser300::parseMeasurementFromXML(
        const xml::lite::Element* measurementElem,
        Measurement* measurement) const
{
    DerivedXMLParser::parseMeasurementFromXML(measurementElem, measurement);

    XMLElem arpFlagElem = getOptional(measurementElem, "ARPFlag");
    if (arpFlagElem)
    {
        parseEnum(*arpFlagElem, measurement->arpFlag);
    }

    common().parsePolyXYZ(getFirstAndOnly(measurementElem, "ARPPoly"), measurement->arpPoly);

    common().parseRowColInts(getFirstAndOnly(measurementElem, "ValidData"),
                             "Vertex",
                             measurement->validData);
}

void DerivedXMLParser300::parseExploitationFeaturesFromXML(
    const xml::lite::Element* exploitationFeaturesElem,
    ExploitationFeatures* exploitationFeatures) const
{
    // There is a difference between 1.0 and 2.0 in that the processing field
    // is no longer allowed in 2.0. But since schema validation should handle
    // this, it's fine to just delegate to the common case.
    DerivedXMLParser::parseExploitationFeaturesFromXML(exploitationFeaturesElem, exploitationFeatures);

    std::vector<XMLElem> collectionsElem;
    exploitationFeaturesElem->getElementsByTagName("Collection", collectionsElem);
    for (size_t i = 0; i < collectionsElem.size(); ++i)
    {
        const xml::lite::Element* const collectionElem = collectionsElem[i];
        XMLElem geometryElem = getOptional(collectionElem, "Geometry");
        const Collection& coll = *exploitationFeatures->collections[i];

        // optional
        if (geometryElem) {
            parseOptionalDouble(geometryElem, "DopplerConeAngle", coll.geometry->dopplerConeAngle);
        }
    }
}

void DerivedXMLParser300::parseProductFromXML(
    const xml::lite::Element* exploitationFeaturesElem,
    ExploitationFeatures* exploitationFeatures) const
{
    std::vector<XMLElem> productElems;
    exploitationFeaturesElem->getElementsByTagName("Product", productElems);
    if (productElems.empty())
    {
        throw except::Exception(Ctxt("ExploitationFeatures requires at least one Product"));
    }

    exploitationFeatures->product.resize(productElems.size());
    for (size_t ii = 0; ii < productElems.size(); ++ii)
    {
        const xml::lite::Element* const productElem = productElems[ii];
        Product& product = exploitationFeatures->product[ii];

        common().parseRowColDouble(getFirstAndOnly(productElem, "Resolution"), product.resolution);
        parseDouble(getFirstAndOnly(productElem, "Ellipticity"), product.ellipticity);

        std::vector<XMLElem> polarizationElems;
        productElem->getElementsByTagName("Polarization", polarizationElems);
        if (polarizationElems.empty())
        {
            throw except::Exception(Ctxt("Product requires at least one polarization"));
        }
        product.polarization.resize(polarizationElems.size());
        for (size_t jj = 0; jj < product.polarization.size(); ++jj)
        {
            const xml::lite::Element* const polarizationElem = polarizationElems[jj];
            ProcTxRcvPolarization& polarization = product.polarization[jj];

            parseEnum(getFirstAndOnly(polarizationElem, "TxPolarizationProc"),
                      polarization.txPolarizationProc);
            parseEnum(getFirstAndOnly(polarizationElem, "RcvPolarizationProc"),
                      polarization.rcvPolarizationProc);
        }

        parseOptionalDouble(productElem, "North", product.north);

        // optional to unbounded
        common().parseParameters(productElem, "Extension",
                                 product.extensions);
    }
}

void DerivedXMLParser300::parseDigitalElevationDataFromXML(const xml::lite::Element& elem, DigitalElevationData& ded) const
{
    auto& coordElem = getFirstAndOnly(elem, "GeographicCoordinates");
    parseDouble(getFirstAndOnly(coordElem, "LongitudeDensity"), ded.geographicCoordinates.longitudeDensity);
    parseDouble(getFirstAndOnly(coordElem, "LatitudeDensity"), ded.geographicCoordinates.latitudeDensity);
    common().parseLatLon(getFirstAndOnly(coordElem, "ReferenceOrigin"), ded.geographicCoordinates.referenceOrigin);

    auto& posElem = getFirstAndOnly(elem, "Geopositioning");
    std::string coordSystemType;
    parseString(getFirstAndOnly(posElem, "CoordinateSystemType"), coordSystemType);
    ded.geopositioning.coordinateSystemType = CoordinateSystemType::toType(coordSystemType);
    parseUInt(getFirstAndOnly(posElem, "FalseOrigin"), ded.geopositioning.falseOrigin);
    if (ded.geopositioning.coordinateSystemType == CoordinateSystemType::UTM)
    {
        parseInt(getFirstAndOnly(posElem, "UTMGridZoneNumber"), ded.geopositioning.utmGridZoneNumber);
    }
    auto& posAccuracyElem = getFirstAndOnly(elem, "PositionalAccuracy");
    parseUInt(getFirstAndOnly(posAccuracyElem, "NumRegions"), ded.positionalAccuracy.numRegions);
    auto& absoluteElem = getFirstAndOnly(posAccuracyElem, "AbsoluteAccuracy");
    parseDouble(getFirstAndOnly(absoluteElem, "Horizontal"), ded.positionalAccuracy.absoluteAccuracyHorizontal);
    parseDouble(getFirstAndOnly(absoluteElem, "Vertical"), ded.positionalAccuracy.absoluteAccuracyVertical);
    auto& pointElem = getFirstAndOnly(posAccuracyElem, "PointToPointAccuracy");
    parseDouble(getFirstAndOnly(pointElem, "Horizontal"), ded.positionalAccuracy.pointToPointAccuracyHorizontal);
    parseDouble(getFirstAndOnly(pointElem, "Vertical"), ded.positionalAccuracy.pointToPointAccuracyVertical);
}

std::unique_ptr<LUT> DerivedXMLParser300::parseSingleLUT(const xml::lite::Element& elem, size_t size) const
{
    std::string lutStr;
    parseString(&elem, lutStr);
    return DerivedXMLParser200::parseSingleLUT(lutStr, size);
}

XMLElem DerivedXMLParser300::createLUT(const std::string& name, const LUT *lut,
        XMLElem parent) const
{
    assert(lut != nullptr);
    assert(parent != nullptr);
    return &DerivedXMLParser200::createLUT(*this, name, *lut, *parent);
}
}
}
