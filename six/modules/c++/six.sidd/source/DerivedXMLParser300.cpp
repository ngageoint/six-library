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

#include <sstream>

#include <six/SICommonXMLParser10x.h>
#include <six/sidd/DerivedDataBuilder.h>
#include <six/sidd/DerivedXMLParser300.h>

namespace
{
typedef xml::lite::Element* XMLElem;
typedef xml::lite::Attributes XMLAttributes;

template <typename T>
bool isDefined(const T& enumVal)
{
    return six::Init::isDefined(enumVal.value);
}

template <typename T>
bool isUndefined(const T& enumVal)
{
    return six::Init::isUndefined(enumVal.value);
}

template <typename SmartPtrT>
void confirmNonNull(const SmartPtrT& ptr,
                    const std::string& name,
                    const std::string& suffix = "")
{
    if (ptr.get() == nullptr)
    {
        std::string msg = name + " is required";
        if (!suffix.empty())
        {
            msg += " " + suffix;
        }

        throw except::Exception(Ctxt(msg));
    }
}

void validateDRAFields(const six::sidd::DRAType& algorithmType,
                       bool hasDraParameters,
                       bool hasDraOverrides)
{
    if (algorithmType == six::sidd::DRAType::AUTO &&
        !hasDraParameters)
    {
        throw except::Exception(Ctxt(
            "DRAParameters required for algorithmType AUTO"));
    }

    if (algorithmType != six::sidd::DRAType::AUTO && hasDraParameters)
    {
        throw except::Exception(Ctxt(
            "DRAParameters invalid for algorithmType " +
            algorithmType.toString()));
    }

    if (algorithmType == six::sidd::DRAType::NONE && hasDraOverrides)
    {
        throw except::Exception(Ctxt(
            "DRAOverrides invalid for algorithmType " +
            algorithmType.toString()));
    }
}
}

namespace six
{
namespace sidd
{
static const char VERSION[] = "3.0.0";
static const char SI_COMMON_URI[] = "urn:SICommon:1.0";
static const char ISM_URI[] = "urn:us:gov:ic:ism:13";

DerivedXMLParser300::DerivedXMLParser300(logging::Logger* log,
                                         bool ownLog) :
    DerivedXMLParser(VERSION,
                     std::make_unique<six::SICommonXMLParser10x>(versionToURI(VERSION), false, SI_COMMON_URI, log),
                     log, ownLog)
{
}

DerivedData* DerivedXMLParser300::fromXML(const xml::lite::Document* doc) const
{
    XMLElem root = doc->getRootElement();

    XMLElem productCreationElem        = getFirstAndOnly(root, "ProductCreation");
    XMLElem displayElem                = getFirstAndOnly(root, "Display");
    XMLElem geoDataElem                = getFirstAndOnly(root, "GeoData");
    XMLElem measurementElem            = getFirstAndOnly(root, "Measurement");
    XMLElem exploitationFeaturesElem   = getFirstAndOnly(root, "ExploitationFeatures");
    XMLElem productProcessingElem      = getOptional(root, "ProductProcessing");
    XMLElem downstreamReprocessingElem = getOptional(root, "DownstreamReprocessing");
    XMLElem errorStatisticsElem        = getOptional(root, "ErrorStatistics");
    XMLElem radiometricElem            = getOptional(root, "Radiometric");
    XMLElem matchInfoElem              = getOptional(root, "MatchInfo");
    XMLElem compressionElem            = getOptional(root, "Compression");
    XMLElem dedElem                    = getOptional(root, "DigitalElevationData");
    XMLElem annotationsElem            = getOptional(root, "Annotations");


    DerivedDataBuilder builder;
    DerivedData *data = builder.steal(); //steal it

    // see if PixelType has MONO or RGB
    PixelType pixelType = six::toType<PixelType>(
            getFirstAndOnly(displayElem, "PixelType")->getCharacterData());
    builder.addDisplay(pixelType);

    // create GeoData
    builder.addGeoData();

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
    parseDisplayFromXML(displayElem, *data->display);
    parseGeoDataFromXML(geoDataElem, data->geoData.get());
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
    if (matchInfoElem)
    {
        builder.addMatchInformation();
        common().parseMatchInformationFromXML(matchInfoElem, data->matchInformation.get());
    }
    if (compressionElem)
    {
        builder.addCompression();
        parseCompressionFromXML(compressionElem, *data->compression);
    }
    if (dedElem)
    {
        builder.addDigitalElevationData();
        parseDigitalElevationDataFromXML(dedElem, *data->digitalElevationData);
    }
    if (annotationsElem)
    {
        // 1 to unbounded
        std::vector<XMLElem> annChildren;
        annotationsElem->getElementsByTagName("Annotation", annChildren);
        data->annotations.resize(annChildren.size());
        for (size_t i = 0, size = annChildren.size(); i < size; ++i)
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
    convertGeoDataToXML(derived->geoData.get(), root);
    convertMeasurementToXML(derived->measurement.get(), root);
    convertExploitationFeaturesToXML(derived->exploitationFeatures.get(),
                                     root);

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
    if (derived->matchInformation.get())
    {
        common().convertMatchInformationToXML(*derived->matchInformation,
                                              root);
    }
    // optional
    if (derived->radiometric.get())
    {
        common().convertRadiometryToXML(derived->radiometric.get(), root);
    }
    // optional
    if (derived->compression.get())
    {
       convertCompressionToXML(*derived->compression, root);
    }
    // optional
    if (derived->digitalElevationData.get())
    {
        convertDigitalElevationDataToXML(*derived->digitalElevationData,
                                         root);
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
        for (size_t i = 0, num = derived->annotations.size(); i < num; ++i)
        {
            convertAnnotationToXML(derived->annotations[i].get(),
                                   annotationsElem);
        }
    }

    //set the ElemNS
    root->setNamespacePrefix("", getDefaultURI());
    root->setNamespacePrefix("si", SI_COMMON_URI);
    root->setNamespacePrefix("sfa", SFA_URI);
    root->setNamespacePrefix("ism", ISM_URI);

    return doc;
}

void DerivedXMLParser300::parseDerivedClassificationFromXML(
        const xml::lite::Element* classificationElem,
        DerivedClassification& classification) const
{
    DerivedXMLParser::parseDerivedClassificationFromXML(classificationElem, classification);
    const XMLAttributes& classificationAttributes
        = classificationElem->getAttributes();

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

void DerivedXMLParser300::parseCompressionFromXML(const xml::lite::Element* compressionElem,
                                                 Compression& compression) const
{
    XMLElem j2kElem = getFirstAndOnly(compressionElem, "J2K");
    XMLElem originalElem = getFirstAndOnly(j2kElem, "Original");
    XMLElem parsedElem   = getOptional(j2kElem, "Parsed");

    parseJ2KCompression(originalElem, compression.original);
    if (parsedElem)
    {
        compression.parsed.reset(new J2KCompression());
        parseJ2KCompression(parsedElem, *compression.parsed);
    }
}

void DerivedXMLParser300::parseJ2KCompression(const xml::lite::Element* j2kElem,
                                              J2KCompression& j2k) const
{
    parseInt(getFirstAndOnly(j2kElem, "NumWaveletLevels"),
            j2k.numWaveletLevels);
    parseInt(getFirstAndOnly(j2kElem, "NumBands"),
            j2k.numBands);

    XMLElem layerInfoElems = getFirstAndOnly(j2kElem, "LayerInfo");
    std::vector<XMLElem> layerElems;
    layerInfoElems->getElementsByTagName("Layer", layerElems);

    size_t numLayers = layerElems.size();
    j2k.layerInfo.resize(numLayers);

    for (size_t ii = 0; ii < layerElems.size(); ++ii)
    {
        parseDouble(getFirstAndOnly(layerElems[ii], "Bitrate"),
                    j2k.layerInfo[ii].bitRate);
    }
}

void DerivedXMLParser300::parseDisplayFromXML(const xml::lite::Element* displayElem,
                                              Display& display) const
{
    //pixelType previously set
    parseUInt(getFirstAndOnly(displayElem, "NumBands"),
              display.numBands);
    XMLElem bandDisplayElem = getOptional(displayElem, "DefaultBandDisplay");
    if (bandDisplayElem)
    {
        parseUInt(bandDisplayElem, display.defaultBandDisplay);
    }

    std::vector<XMLElem> nonInteractiveProcessingElems;
    displayElem->getElementsByTagName("NonInteractiveProcessing",
            nonInteractiveProcessingElems);

    display.nonInteractiveProcessing.resize(nonInteractiveProcessingElems.size());
    for (size_t ii = 0; ii < nonInteractiveProcessingElems.size(); ++ii)
    {
        display.nonInteractiveProcessing[ii].reset(new NonInteractiveProcessing());
        parseNonInteractiveProcessingFromXML(nonInteractiveProcessingElems[ii],
                *display.nonInteractiveProcessing[ii]);
    }

    std::vector<XMLElem> interactiveProcessingElems;
    displayElem->getElementsByTagName("InteractiveProcessing",
            interactiveProcessingElems);

    display.interactiveProcessing.resize(interactiveProcessingElems.size());
    for (size_t ii = 0; ii < interactiveProcessingElems.size(); ++ii)
    {
        display.interactiveProcessing[ii].reset(new InteractiveProcessing());
        parseInteractiveProcessingFromXML(interactiveProcessingElems[ii],
                *display.interactiveProcessing[ii]);
    }

    std::vector<XMLElem> extensions;
    displayElem->getElementsByTagName("DisplayExtention", extensions);
    for (size_t ii = 0; ii < extensions.size(); ++ii)
    {
        std::string name;
        getAttributeIfExists(extensions[ii]->getAttributes(), "name", name);
        std::string value;
        parseString(extensions[ii], value);
        Parameter parameter(value);
        parameter.setName(name);
        display.displayExtensions.push_back(parameter);
    }
}

void DerivedXMLParser300::parseNonInteractiveProcessingFromXML(
            const xml::lite::Element* procElem,
            NonInteractiveProcessing& nonInteractiveProcessing) const
{
    XMLElem productGenerationOptions = getFirstAndOnly(procElem,
            "ProductGenerationOptions");
    XMLElem rrdsElem = getFirstAndOnly(procElem, "RRDS");

    parseProductGenerationOptionsFromXML(productGenerationOptions,
        nonInteractiveProcessing.productGenerationOptions);
    parseRRDSFromXML(rrdsElem, nonInteractiveProcessing.rrds);
}

void DerivedXMLParser300::parseProductGenerationOptionsFromXML(
            const xml::lite::Element* optionsElem,
            ProductGenerationOptions& options) const
{
    XMLElem bandElem = getOptional(optionsElem, "BandEqualization");
    XMLElem restoration = getOptional(optionsElem,
            "ModularTransferFunctionRestoration");
    XMLElem remapElem = getOptional(optionsElem, "DataRemapping");
    XMLElem correctionElem = getOptional(optionsElem,
            "AsymmetricPixelCorrection");

    if (bandElem)
    {
        options.bandEqualization.reset(new BandEqualization());
        parseBandEqualizationFromXML(bandElem, *options.bandEqualization);
    }

    if (restoration)
    {
        options.modularTransferFunctionRestoration.reset(new Filter());
        parseFilterFromXML(restoration,
                           *options.modularTransferFunctionRestoration);
    }

    if (remapElem)
    {
        options.dataRemapping.reset(new LookupTable());
        parseLookupTableFromXML(remapElem, *options.dataRemapping);
    }

    if (correctionElem)
    {
        options.asymmetricPixelCorrection.reset(new Filter());
        parseFilterFromXML(correctionElem, *options.asymmetricPixelCorrection);
    }
}

void DerivedXMLParser300::parseLookupTableFromXML(
            const xml::lite::Element* lookupElem,
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
            const XMLAttributes& attributes = lutInfoElem->getAttributes();
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

void DerivedXMLParser300::parseBandEqualizationFromXML(const xml::lite::Element* bandElem,
                                                       BandEqualization& band) const
{
    std::string bandAlgo;
    parseString(getFirstAndOnly(bandElem, "Algorithm"), bandAlgo);

    if (bandAlgo == "1DLUT")
    {
        band.algorithm = BandEqualizationAlgorithm("LUT 1D");
    }
    else
    {
        band.algorithm = BandEqualizationAlgorithm(bandAlgo);
    }

    std::vector<XMLElem> lutElems;
    bandElem->getElementsByTagName("BandLUT", lutElems);
    band.bandLUTs.resize(lutElems.size());
    for (size_t ii = 0; ii < lutElems.size(); ++ii)
    {
        band.bandLUTs[ii].reset(new LookupTable());
        parseLookupTableFromXML(lutElems[ii], *band.bandLUTs[ii]);
    }
}

void DerivedXMLParser300::parseRRDSFromXML(const xml::lite::Element* rrdsElem,
            RRDS& rrds) const
{
    parseEnum(getFirstAndOnly(rrdsElem, "DownsamplingMethod"), rrds.downsamplingMethod);

    XMLElem antiAliasElem = getOptional(rrdsElem, "AntiAlias");
    if (antiAliasElem)
    {
        rrds.antiAlias.reset(new Filter());
        parseFilterFromXML(antiAliasElem, *rrds.antiAlias);
    }

    XMLElem interpolationElem = getOptional(rrdsElem, "Interpolation");
    if (interpolationElem)
    {
        rrds.interpolation.reset(new Filter());
        parseFilterFromXML(interpolationElem, *rrds.interpolation);
    }
}

void DerivedXMLParser300::parseFilterFromXML(const xml::lite::Element* filterElem,
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
        const XMLAttributes& attributes = filterCoef->getAttributes();
        getAttributeIfExists(attributes, "numRows", kernel.custom->size.row);
        getAttributeIfExists(attributes, "numCols", kernel.custom->size.col);

        if (six::Init::isUndefined(kernel.custom->size.row) || six::Init::isUndefined(kernel.custom->size.col))
        {
            throw except::Exception("Expected row and col attributes in FilterCoefficients element of Custom");
        }

        std::vector<XMLElem> coefficients;
        filterCoef->getElementsByTagName("Coef", coefficients);
        size_t numCoefs = coefficients.size();
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
        const XMLAttributes& attributes = filterCoef->getAttributes();
        getAttributeIfExists(attributes, "numPhasings", bank.custom->numPhasings);
        getAttributeIfExists(attributes, "numPoints", bank.custom->numPoints);

        std::vector<XMLElem> coefficients;
        filterCoef->getElementsByTagName("Coef", coefficients);
        size_t numCoefs = coefficients.size();
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
            const xml::lite::Element* interactiveElem,
            InteractiveProcessing& interactive) const
{
    XMLElem geomElem = getFirstAndOnly(interactiveElem, "GeometricTransform");
    XMLElem sharpnessElem = getFirstAndOnly(interactiveElem,
            "SharpnessEnhancement");
    XMLElem colorElem = getOptional(interactiveElem, "ColorSpaceTransform");
    XMLElem dynamicElem = getFirstAndOnly(interactiveElem, "DynamicRangeAdjustment");
    XMLElem ttcElem = getOptional(interactiveElem, "TonalTransferCurve");

    interactive.geometricTransform = GeometricTransform();
    parseGeometricTransformFromXML(geomElem, interactive.geometricTransform);
    parseSharpnessEnhancementFromXML(sharpnessElem,
                                     interactive.sharpnessEnhancement);

    if (colorElem)
    {
        interactive.colorSpaceTransform.reset(new ColorSpaceTransform());
        parseColorSpaceTransformFromXML(colorElem,
                                        *interactive.colorSpaceTransform);
    }

    parseDynamicRangeAdjustmentFromXML(dynamicElem,
                                       interactive.dynamicRangeAdjustment);

    if (ttcElem)
    {
        interactive.tonalTransferCurve.reset(new LookupTable());
        parseLookupTableFromXML(ttcElem, *interactive.tonalTransferCurve);
    }
}

void DerivedXMLParser300::parseGeometricTransformFromXML(const xml::lite::Element* geomElem,
             GeometricTransform& transform) const
{
    XMLElem scalingElem = getFirstAndOnly(geomElem, "Scaling");
    parseFilterFromXML(getFirstAndOnly(scalingElem, "AntiAlias"),
        transform.scaling.antiAlias);
    parseFilterFromXML(getFirstAndOnly(scalingElem, "Interpolation"),
        transform.scaling.interpolation);

    XMLElem orientationElem = getFirstAndOnly(geomElem, "Orientation");
    parseEnum(getFirstAndOnly(orientationElem, "ShadowDirection"), transform.orientation.shadowDirection);
}

void DerivedXMLParser300::parseSharpnessEnhancementFromXML(
             const xml::lite::Element* sharpElem,
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
            parseFilterFromXML(mTFCElem,
                               *sharpness.modularTransferFunctionCompensation);
        }
    }
    else if (mTFRElem)
    {
        ok = true;
        sharpness.modularTransferFunctionEnhancement.reset(new Filter());
        parseFilterFromXML(mTFRElem,
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
            const xml::lite::Element* colorElem, ColorSpaceTransform& transform) const
{
    XMLElem manageElem = getFirstAndOnly(colorElem, "ColorManagementModule");

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
        transform.colorManagementModule.renderingIntent = RenderingIntent(renderIntentStr);
    }
    parseString(getFirstAndOnly(manageElem, "SourceProfile"),
                transform.colorManagementModule.sourceProfile);
    parseOptionalString(manageElem, "DisplayProfile", transform.colorManagementModule.displayProfile);
    parseOptionalString(manageElem, "ICCProfileSignature", transform.colorManagementModule.iccProfile);
}

void DerivedXMLParser300::parseDynamicRangeAdjustmentFromXML(
            const xml::lite::Element* rangeElem,
            DynamicRangeAdjustment& rangeAdjustment) const
{
    parseEnum(getFirstAndOnly(rangeElem, "AlgorithmType"), rangeAdjustment.algorithmType);
    parseInt(getFirstAndOnly(rangeElem, "BandStatsSource"), rangeAdjustment.bandStatsSource);

    XMLElem parameterElem = getOptional(rangeElem, "DRAParameters");
    XMLElem overrideElem = getOptional(rangeElem, "DRAOverrides");

    validateDRAFields(rangeAdjustment.algorithmType,
                      parameterElem ? true : false, overrideElem ? true : false);

    if (parameterElem)
    {
        rangeAdjustment.draParameters.reset(new DynamicRangeAdjustment::DRAParameters());
        parseDouble(getFirstAndOnly(parameterElem, "Pmin"), rangeAdjustment.draParameters->pMin);
        parseDouble(getFirstAndOnly(parameterElem, "Pmax"), rangeAdjustment.draParameters->pMax);
        parseDouble(getFirstAndOnly(parameterElem, "EminModifier"), rangeAdjustment.draParameters->eMinModifier);
        parseDouble(getFirstAndOnly(parameterElem, "EmaxModifier"), rangeAdjustment.draParameters->eMaxModifier);
    }

    if (overrideElem)
    {
        rangeAdjustment.draOverrides.reset(new DynamicRangeAdjustment::DRAOverrides());
        parseDouble(getFirstAndOnly(overrideElem, "Subtractor"),
            rangeAdjustment.draOverrides->subtractor);
        parseDouble(getFirstAndOnly(overrideElem, "Multiplier"),
            rangeAdjustment.draOverrides->multiplier);
    }
}

XMLElem DerivedXMLParser300::convertDerivedClassificationToXML(
        const DerivedClassification& classification,
        XMLElem parent) const
{
    XMLElem classElem = newElement("Classification", parent);

    common().addParameters("SecurityExtension",
                    classification.securityExtensions,
                           classElem);

    //! from ism:ISMRootNodeAttributeGroup
    // SIDD 2.0 is tied to IC-ISM v13
    setAttribute(classElem, "DESVersion", "13", ISM_URI);

    // So far as I can tell this should just be 1
    setAttribute(classElem, "ISMCATCESVersion", "1", ISM_URI);

    //! from ism:ResourceNodeAttributeGroup
    setAttribute(classElem, "resourceElement", "true", ISM_URI);
    setAttribute(classElem, "createDate",
                 classification.createDate.format("%Y-%m-%d"), ISM_URI);
    // required (was optional in SIDD 1.0)
    setAttributeList(classElem, "compliesWith", classification.compliesWith,
                     ISM_URI);

    // optional
    setAttributeIfNonEmpty(classElem,
                           "exemptFrom",
                           classification.exemptFrom,
                           ISM_URI);

    //! from ism:SecurityAttributesGroup
    //  -- referenced in ism::ResourceNodeAttributeGroup
    setAttribute(classElem, "classification", classification.classification,
                 ISM_URI);
    setAttributeList(classElem, "ownerProducer", classification.ownerProducer,
                     ISM_URI, true);

    // optional
    setAttributeIfNonEmpty(classElem, "joint", classification.joint, ISM_URI);

    // optional
    setAttributeList(classElem, "SCIcontrols", classification.sciControls,
                     ISM_URI);
    // optional
    setAttributeList(classElem, "SARIdentifier", classification.sarIdentifier,
                     ISM_URI);
    // optional
    setAttributeList(classElem,
                     "atomicEnergyMarkings",
                     classification.atomicEnergyMarkings,
                     ISM_URI);
    // optional
    setAttributeList(classElem,
                     "disseminationControls",
                     classification.disseminationControls,
                     ISM_URI);
    // optional
    setAttributeList(classElem,
                     "displayOnlyTo",
                     classification.displayOnlyTo,
                     ISM_URI);
    // optional
    setAttributeList(classElem, "FGIsourceOpen", classification.fgiSourceOpen,
                     ISM_URI);
    // optional
    setAttributeList(classElem,
                     "FGIsourceProtected",
                     classification.fgiSourceProtected,
                     ISM_URI);
    // optional
    setAttributeList(classElem, "releasableTo", classification.releasableTo,
                     ISM_URI);
    // optional
    setAttributeList(classElem, "nonICmarkings", classification.nonICMarkings,
                     ISM_URI);
    // optional
    setAttributeIfNonEmpty(classElem,
                           "classifiedBy",
                           classification.classifiedBy,
                           ISM_URI);
    // optional
    setAttributeIfNonEmpty(classElem,
                           "compilationReason",
                           classification.compilationReason,
                           ISM_URI);
    // optional
    setAttributeIfNonEmpty(classElem,
                           "derivativelyClassifiedBy",
                           classification.derivativelyClassifiedBy,
                           ISM_URI);
    // optional
    setAttributeIfNonEmpty(classElem,
                           "classificationReason",
                           classification.classificationReason,
                           ISM_URI);
    // optional
    setAttributeList(classElem, "nonUSControls", classification.nonUSControls,
                     ISM_URI);
    // optional
    setAttributeIfNonEmpty(classElem,
                           "derivedFrom",
                           classification.derivedFrom,
                           ISM_URI);
    // optional
    if (classification.declassDate.get())
    {
        setAttributeIfNonEmpty(
                classElem, "declassDate",
                classification.declassDate->format("%Y-%m-%d"),
                ISM_URI);
    }
    // optional
    setAttributeIfNonEmpty(classElem,
                           "declassEvent",
                           classification.declassEvent,
                           ISM_URI);
    // optional
    setAttributeIfNonEmpty(classElem,
                           "declassException",
                           classification.declassException,
                           ISM_URI);

    //! from ism:NoticeAttributesGroup
    // optional
    setAttributeIfNonEmpty(classElem,
                           "noticeType",
                           classification.noticeType,
                           ISM_URI);
    // optional
    setAttributeIfNonEmpty(classElem,
                           "noticeReason",
                           classification.noticeReason,
                           ISM_URI);
    // optional
    if (classification.noticeDate.get())
    {
        setAttributeIfNonEmpty(
                classElem, "noticeDate",
                classification.noticeDate->format("%Y-%m-%d"),
                ISM_URI);
    }
    // optional
    setAttributeIfNonEmpty(classElem,
                           "unregisteredNoticeType",
                           classification.unregisteredNoticeType,
                           ISM_URI);
    // optional
    setAttributeIfNonEmpty(classElem,
                           "externalNotice",
                           classification.externalNotice,
                           ISM_URI);

    return classElem;
}

XMLElem DerivedXMLParser300::convertLookupTableToXML(
        const std::string& name,
        const LookupTable& table,
        XMLElem parent) const
{
    XMLElem lookupElem = newElement(name, parent);
    createString("LUTName", table.lutName, lookupElem);

    bool ok = false;
    if (table.predefined.get())
    {
        if (table.custom.get() == nullptr)
        {
            ok = true;
            XMLElem predefElem = newElement("Predefined", lookupElem);

            //exactly one of databaseName or (remapFamily and remapMember) can be set
            bool innerOk = false;
            if (table.predefined->databaseName.empty())
            {
                if (six::Init::isDefined(table.predefined->remapFamily) &&
                    six::Init::isDefined(table.predefined->remapMember))
                {
                    innerOk = true;
                    createInt("RemapFamily", table.predefined->remapFamily, predefElem);
                    createInt("RemapMember", table.predefined->remapMember, predefElem);
                }
            }
            else if (six::Init::isUndefined(table.predefined->remapFamily) &&
                     six::Init::isUndefined(table.predefined->remapMember))
            {
                innerOk = true;
                createString("DatabaseName", table.predefined->databaseName, predefElem);
            }
            if (innerOk == false)
            {
                throw except::Exception(Ctxt("Exactly one of databaseName or remapFamiy+remapMember must be set"));
            }
        }
    }
    else if (table.custom.get())
    {
        ok = true;
        std::vector<LUT>& lutValues = table.custom->lutValues;

        XMLElem customElem = newElement("Custom", lookupElem);
        XMLElem lutInfoElem = newElement("LUTInfo", customElem);
        setAttribute(lutInfoElem, "numLuts", lutValues.size());
        setAttribute(lutInfoElem, "size", lutValues[0].table.size());

        for (size_t ii = 0; ii < lutValues.size(); ++ii)
        {
            XMLElem lutElem = createLUT("LUTValues", &lutValues[ii], lutInfoElem);
            setAttribute(lutElem, "lut", ii + 1);
        }
    }
    if (!ok)
    {
        throw except::Exception(Ctxt("Exactly one of Predefined or Custom must be defined"));
    }
    return lookupElem;
}

XMLElem DerivedXMLParser300::convertNonInteractiveProcessingToXML(
        const NonInteractiveProcessing& processing,
        XMLElem parent) const
{
    XMLElem processingElem = newElement("NonInteractiveProcessing", parent);

    // ProductGenerationOptions
    XMLElem prodGenElem = newElement("ProductGenerationOptions",
                                    processingElem);

    const ProductGenerationOptions& prodGen =
            processing.productGenerationOptions;

    if (prodGen.bandEqualization.get())
    {
        const BandEqualization& bandEq = *prodGen.bandEqualization;
        XMLElem bandEqElem = newElement("BandEqualization", prodGenElem);
        if (int(bandEq.algorithm) == 0)
        {
            createString("Algorithm", "1DLUT", bandEqElem);
        }
        else
        {
            createStringFromEnum("Algorithm", bandEq.algorithm, bandEqElem);
        }
        for (size_t ii = 0; ii < bandEq.bandLUTs.size(); ++ii)
        {
            convertLookupTableToXML("BandLUT", *bandEq.bandLUTs[ii], bandEqElem);
        }

        //add the attribute to each of the LUTs
        std::vector<XMLElem> LUTElems;
        bandEqElem->getElementsByTagName("BandLUT", LUTElems);
        for (size_t ii = 0; ii < LUTElems.size(); ++ii)
        {
            setAttribute(LUTElems[ii], "k", ii+1);
        }
    }

    if (prodGen.modularTransferFunctionRestoration.get())
    {
        convertFilterToXML("ModularTransferFunctionRestoration",
                           *prodGen.modularTransferFunctionRestoration,
                           prodGenElem);
    }

    if (prodGen.dataRemapping.get())
    {
        convertLookupTableToXML("DataRemapping", *prodGen.dataRemapping,
                                prodGenElem);
    }

    if (prodGen.asymmetricPixelCorrection.get())
    {
        convertFilterToXML("AsymmetricPixelCorrection",
                           *prodGen.asymmetricPixelCorrection, prodGenElem);
    }

    // RRDS
    XMLElem rrdsElem = newElement("RRDS", processingElem);

    const RRDS& rrds = processing.rrds;
    createStringFromEnum("DownsamplingMethod", rrds.downsamplingMethod,
                         rrdsElem);

    if (rrds.downsamplingMethod != DownsamplingMethod::DECIMATE && rrds.downsamplingMethod != DownsamplingMethod::MAX_PIXEL)
    {
        confirmNonNull(rrds.antiAlias, "antiAlias",
                       "for DECIMATE downsampling");
        convertFilterToXML("AntiAlias", *rrds.antiAlias, rrdsElem);

        confirmNonNull(rrds.interpolation, "interpolation",
                       "for DECIMATE downsampling");
        convertFilterToXML("Interpolation", *rrds.interpolation, rrdsElem);
    }
    return processingElem;
}

XMLElem DerivedXMLParser300::convertInteractiveProcessingToXML(
        const InteractiveProcessing& processing,
        XMLElem parent) const
{
    XMLElem processingElem = newElement("InteractiveProcessing", parent);

    // GeometricTransform
    const GeometricTransform& geoTransform(processing.geometricTransform);
    XMLElem geoTransformElem = newElement("GeometricTransform", processingElem);

    XMLElem scalingElem = newElement("Scaling", geoTransformElem);
    convertFilterToXML("AntiAlias", geoTransform.scaling.antiAlias,
                       scalingElem);
    convertFilterToXML("Interpolation", geoTransform.scaling.interpolation,
                       scalingElem);

    XMLElem orientationElem = newElement("Orientation", geoTransformElem);
    createStringFromEnum("ShadowDirection",
        geoTransform.orientation.shadowDirection,
        orientationElem);

    // SharpnessEnhancement
    const SharpnessEnhancement& sharpness(processing.sharpnessEnhancement);
    XMLElem sharpElem = newElement("SharpnessEnhancement", processingElem);

    bool ok = false;
    if (sharpness.modularTransferFunctionCompensation.get())
    {
        if (sharpness.modularTransferFunctionEnhancement.get() == nullptr)
        {
            ok = true;
            convertFilterToXML("ModularTransferFunctionCompensation",
                               *sharpness.modularTransferFunctionCompensation,
                               sharpElem);
        }
    }
    else if (sharpness.modularTransferFunctionEnhancement.get())
    {
        ok = true;
        convertFilterToXML("ModularTransferFunctionEnhancement",
                           *sharpness.modularTransferFunctionEnhancement,
                           sharpElem);
    }

    if (!ok)
    {
        throw except::Exception(Ctxt(
                "Exactly one of modularTransferFunctionCompensation or "
                "modularTransferFunctionEnhancement must be set"));
    }

    // ColorSpaceTransform
    if (processing.colorSpaceTransform.get())
    {
        const ColorManagementModule& cmm =
                processing.colorSpaceTransform->colorManagementModule;

        XMLElem colorSpaceTransformElem =
                newElement("ColorSpaceTransform", processingElem);
        XMLElem cmmElem =
                newElement("ColorManagementModule", colorSpaceTransformElem);

        if (int(cmm.renderingIntent) == 2)
        {
            createString("RenderingIntent", "RELATIVE", cmmElem);
        }
        else if (int(cmm.renderingIntent) == 3)
        {
            createString("RenderingIntent", "ABSOLUTE", cmmElem);
        }
        else
        {
            createStringFromEnum("RenderingIntent", cmm.renderingIntent, cmmElem);
        }

        // TODO: Not sure what this'll actually look like
        createString("SourceProfile", cmm.sourceProfile, cmmElem);
        if (!cmm.displayProfile.empty())
        {
            createString("DisplayProfile", cmm.displayProfile, cmmElem);
        }

        if (!cmm.iccProfile.empty())
        {
            createString("ICCProfileSignature", cmm.iccProfile, cmmElem);
        }
    }

    // DynamicRangeAdjustment

    const DynamicRangeAdjustment& adjust =
            processing.dynamicRangeAdjustment;

    XMLElem adjustElem =
        newElement("DynamicRangeAdjustment", processingElem);

    createStringFromEnum("AlgorithmType", adjust.algorithmType,
        adjustElem);
    createInt("BandStatsSource", adjust.bandStatsSource, adjustElem);

    validateDRAFields(adjust.algorithmType,
                      adjust.draParameters.get() ? true : false,
                      adjust.draOverrides.get() ? true : false);
    if (adjust.draParameters.get())
    {
        XMLElem paramElem = newElement("DRAParameters", adjustElem);
        createDouble("Pmin", adjust.draParameters->pMin, paramElem);
        createDouble("Pmax", adjust.draParameters->pMax, paramElem);
        createDouble("EminModifier", adjust.draParameters->eMinModifier, paramElem);
        createDouble("EmaxModifier", adjust.draParameters->eMinModifier, paramElem);
    }
    if (adjust.draOverrides.get())
    {
        XMLElem overrideElem = newElement("DRAOverrides", adjustElem);
        createDouble("Subtractor", adjust.draOverrides->subtractor, overrideElem);
        createDouble("Multiplier", adjust.draOverrides->multiplier, overrideElem);
    }

    if (processing.tonalTransferCurve.get())
    {
        convertLookupTableToXML("TonalTransferCurve", *processing.tonalTransferCurve, processingElem);
    }
    return processingElem;
}

XMLElem DerivedXMLParser300::convertPredefinedFilterToXML(
        const Filter::Predefined& predefined,
        XMLElem parent) const
{
    XMLElem predefinedElem = newElement("Predefined", parent);

    // Make sure either DBName or FilterFamily+FilterMember are defined
    bool ok = false;
    if (isDefined(predefined.databaseName))
    {
        if (six::Init::isUndefined(predefined.filterFamily) &&
            six::Init::isUndefined(predefined.filterMember))
        {
            ok = true;

            createStringFromEnum("DatabaseName", predefined.databaseName, predefinedElem);
        }
    }
    else if (six::Init::isDefined(predefined.filterFamily) &&
             six::Init::isDefined(predefined.filterMember))
    {
        ok = true;

        createInt("FilterFamily", predefined.filterFamily, predefinedElem);
        createInt("FilterMember", predefined.filterMember, predefinedElem);
    }

    if (!ok)
    {
        throw except::Exception(Ctxt(
                "Exactly one of either dbName or FilterFamily and "
                "FilterMember must be defined"));
    }

    return predefinedElem;
}

XMLElem DerivedXMLParser300::convertKernelToXML(
        const Filter::Kernel& kernel,
        XMLElem parent) const
{
    XMLElem kernelElem = newElement("FilterKernel", parent);

    bool ok = false;
    if (kernel.predefined.get())
    {
        if (kernel.custom.get() == nullptr)
        {
            ok = true;
            convertPredefinedFilterToXML(*kernel.predefined, kernelElem);
        }
    }
    else if (kernel.custom.get())
    {
        ok = true;

        XMLElem customElem = newElement("Custom", kernelElem);

        if (kernel.custom->filterCoef.size() !=
            static_cast<size_t>(kernel.custom->size.row) * kernel.custom->size.col)
        {
            std::ostringstream ostr;
            ostr << "Filter size is " << kernel.custom->size.row << " rows x "
                << kernel.custom->size.col << " cols but have "
                << kernel.custom->filterCoef.size() << " coefficients";
            throw except::Exception(Ctxt(ostr.str()));
        }

        XMLElem filterCoef = newElement("FilterCoefficients", customElem);
        setAttribute(filterCoef, "numRows", static_cast<size_t>(kernel.custom->size.row));
        setAttribute(filterCoef, "numCols", static_cast<size_t>(kernel.custom->size.col));

        for (ptrdiff_t row = 0, idx = 0; row < kernel.custom->size.row; ++row)
        {
            for (ptrdiff_t col = 0; col < kernel.custom->size.col; ++col, ++idx)
            {
                XMLElem coefElem = createDouble("Coef", kernel.custom->filterCoef[static_cast<size_t>(idx)],
                    filterCoef);
                setAttribute(coefElem, "row", static_cast<size_t>(row));
                setAttribute(coefElem, "col", static_cast<size_t>(col));
            }
        }
    }
    if (!ok)
    {
        throw except::Exception(Ctxt("Exactly one of Custom or Predefined must be set"));
    }

    return kernelElem;
}

XMLElem DerivedXMLParser300::convertBankToXML(const Filter::Bank& bank,
    XMLElem parent) const
{
    XMLElem bankElem = newElement("FilterBank", parent);

    bool ok = false;
    if (bank.predefined.get())
    {
        if (bank.custom.get() == nullptr)
        {
            ok = true;
            convertPredefinedFilterToXML(*bank.predefined, bankElem);
        }
    }
    else if (bank.custom.get())
    {
        ok = true;

        XMLElem customElem = newElement("Custom", bankElem);

        if (bank.custom->filterCoef.size() !=
            static_cast<size_t>(bank.custom->numPhasings) * bank.custom->numPoints)
        {
            std::ostringstream ostr;
            ostr << "Filter size is " << bank.custom->numPhasings << " x "
                << bank.custom->numPoints << " but have "
                << bank.custom->filterCoef.size() << " coefficients";
            throw except::Exception(Ctxt(ostr.str()));
        }

        XMLElem filterCoef = newElement("FilterCoefficients", customElem);
        setAttribute(filterCoef, "numPhasings", bank.custom->numPhasings);
        setAttribute(filterCoef, "numPoints", bank.custom->numPoints);

        for (size_t row = 0, idx = 0; row < bank.custom->numPhasings; ++row)
        {
            for (size_t col = 0; col < bank.custom->numPoints; ++col, ++idx)
            {
                XMLElem coefElem = createDouble("Coef", bank.custom->filterCoef[idx],
                    filterCoef);
                setAttribute(coefElem, "phasing", row);
                setAttribute(coefElem, "point", col);
            }
        }
    }
    if (!ok)
    {
        throw except::Exception(Ctxt("Exactly one of Custom or Predefined must be set"));
    }

    return bankElem;
}

XMLElem DerivedXMLParser300::convertFilterToXML(const std::string& name,
                                                const Filter& filter,
                                                XMLElem parent) const
{
    XMLElem filterElem = newElement(name, parent);

    createString("FilterName", filter.filterName, filterElem);

    // Exactly one of Kernel or Bank should be set
    bool ok = false;
    if (filter.filterKernel.get())
    {
        if (filter.filterBank.get() == nullptr)
        {
            ok = true;
            convertKernelToXML(*filter.filterKernel, filterElem);
        }
    }
    else if (filter.filterBank.get())
    {
        ok = true;
        convertBankToXML(*filter.filterBank, filterElem);
    }

    if (!ok)
    {
        throw except::Exception(Ctxt(
                "Exactly one of kernel or bank must be set"));
    }

    createStringFromEnum("Operation", filter.operation, filterElem);

    return filterElem;
}

XMLElem DerivedXMLParser300::convertCompressionToXML(
        const Compression& compression,
        XMLElem parent) const
{
    XMLElem compressionElem = newElement("Compression", parent);
    XMLElem j2kElem = newElement("J2K", compressionElem);
    XMLElem originalElem = newElement("Original", j2kElem);
    convertJ2KToXML(compression.original, originalElem);

    if (compression.parsed.get())
    {
        XMLElem parsedElem = newElement("Parsed", j2kElem);
        convertJ2KToXML(*compression.parsed, parsedElem);
    }
    return compressionElem;
}

void DerivedXMLParser300::convertJ2KToXML(const J2KCompression& j2k,
                                          XMLElem& parent) const
{
    createInt("NumWaveletLevels", j2k.numWaveletLevels, parent);
    createInt("NumBands", j2k.numBands, parent);

    size_t numLayers = j2k.layerInfo.size();
    XMLElem layerInfoElem = newElement("LayerInfo", parent);
    setAttribute(layerInfoElem, "numLayers", numLayers);

    for (size_t ii = 0; ii < numLayers; ++ii)
    {
        XMLElem layerElem = newElement("Layer", layerInfoElem);
        setAttribute(layerElem, "index", ii + 1);
        createDouble("Bitrate", j2k.layerInfo[ii].bitRate, layerElem);
    }
}

XMLElem DerivedXMLParser300::convertMeasurementToXML(const Measurement* measurement,
    XMLElem parent) const
{
    XMLElem measurementElem = DerivedXMLParser::convertMeasurementToXML(measurement, parent);

    if (measurement->arpFlag != AppliedType::NOT_SET)
    {
        createStringFromEnum("ARPFlag", measurement->arpFlag, measurementElem);
    }

    common().createPolyXYZ("ARPPoly",
        measurement->arpPoly,
        measurementElem);

    //only if 3+ vertices
    const size_t numVertices = measurement->validData.size();
    if (numVertices >= 3)
    {
        XMLElem vElem = newElement("ValidData", measurementElem);
        setAttribute(vElem, "size", numVertices);

        for (size_t ii = 0; ii < numVertices; ++ii)
        {
            XMLElem vertexElem = common().createRowCol(
                "Vertex", measurement->validData[ii], vElem);
            setAttribute(vertexElem, "index", ii + 1);
        }
    }
    else
    {
        throw except::Exception(Ctxt(
            "ValidData must have at least 3 vertices"));
    }
    return measurementElem;
}

XMLElem DerivedXMLParser300::convertExploitationFeaturesToXML(
    const ExploitationFeatures* exploitationFeatures,
    XMLElem parent) const
{
    XMLElem exploitationFeaturesElem =
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
        if (collection->information.radarModeID
            != Init::undefined<std::string>())
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
        for (size_t n = 0, nElems =
            collection->information.polarization.size(); n < nElems; ++n)
        {
            TxRcvPolarization *p = collection->information.polarization[n].get();
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
            // removed in 2.0
            if (!Init::isUndefined(p->processed))
            {
                throw except::Exception(Ctxt(
                    "Polarization.Processed is not a valid field for SIDD 2.0"));
            }
        }

        // create Geometry -- optional
        Geometry* geom = collection->geometry.get();
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
            // optional
            if (geom->dopplerConeAngle != Init::undefined<double>())
                createDouble("DopplerConeAngle", geom->dopplerConeAngle, geometryElem);
            // optional to unbounded
            common().addParameters("Extension", geom->extensions,
                geometryElem);
        }

        // create Phenomenology -- optional
        Phenomenology* phenom = collection->phenomenology.get();
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
    if (exploitationFeatures->product.empty())
    {
        throw except::Exception(Ctxt(
            "ExploitationFeatures must have at least one Product"));
    }

    for (size_t ii = 0; ii < exploitationFeatures->product.size(); ++ii)
    {
        XMLElem productElem = newElement("Product", exploitationFeaturesElem);
        const Product& product = exploitationFeatures->product[ii];

        common().createRowCol("Resolution",
                              product.resolution,
                              productElem);

        createDouble("Ellipticity", product.ellipticity, productElem);

        if (product.polarization.empty())
        {
            throw except::Exception(Ctxt(
                "Product must have at least one Polarization"));
        }

        for (size_t jj = 0; jj < product.polarization.size(); ++jj)
        {
            XMLElem polarizationElem = newElement("Polarization", productElem);
            createStringFromEnum("TxPolarizationProc",
                                 product.polarization[jj].txPolarizationProc,
                                 polarizationElem);
            createStringFromEnum("RcvPolarizationProc",
                                 product.polarization[jj].rcvPolarizationProc,
                                 polarizationElem);
        }

        // optional
        if (product.north != Init::undefined<double>())
            createDouble("North", product.north, productElem);

        // optional to unbounded
        common().addParameters("Extension",
                               product.extensions,
                               productElem);
    }

    return exploitationFeaturesElem;
}

XMLElem DerivedXMLParser300::convertDisplayToXML(
        const Display& display,
        XMLElem parent) const
{
    // NOTE: In several spots here, there are fields which are required in
    //       SIDD 2.0 but a pointer in the Display class since it didn't exist
    //       in SIDD 1.0, so need to confirm it's allocated
    XMLElem displayElem = newElement("Display", parent);

    createString("PixelType", display.pixelType, displayElem);

    createInt("NumBands", display.numBands, displayElem);
    if (six::Init::isDefined(display.defaultBandDisplay))
    {
        createInt("DefaultBandDisplay", display.defaultBandDisplay, displayElem);
    }

    // NonInteractiveProcessing
    for (size_t ii = 0; ii < display.nonInteractiveProcessing.size(); ++ii)
    {
        confirmNonNull(display.nonInteractiveProcessing[ii],
                "nonInteractiveProcessing");
        XMLElem temp = convertNonInteractiveProcessingToXML(
                *display.nonInteractiveProcessing[ii],
                displayElem);
        setAttribute(temp, "band", ii + 1);
    }

    for (size_t ii = 0; ii < display.interactiveProcessing.size(); ++ii)
    {
        // InteractiveProcessing
        confirmNonNull(display.interactiveProcessing[ii],
                "interactiveProcessing");
        XMLElem temp = convertInteractiveProcessingToXML(
                *display.interactiveProcessing[ii],
                displayElem);
        setAttribute(temp, "band", ii + 1);
    }

    // optional to unbounded
    common().addParameters("DisplayExtension", display.displayExtensions,
                           displayElem);
    return displayElem;
}

XMLElem DerivedXMLParser300::convertGeoDataToXML(
        const GeoDataBase* geoData,
        XMLElem parent) const
{
    XMLElem geoDataXML = newElement("GeoData", parent);

    common().createEarthModelType("EarthModel", geoData->earthModel, geoDataXML);

    common().createLatLonFootprint("ImageCorners", "ICP", geoData->imageCorners, geoDataXML);

    //only if 3+ vertices
    const size_t numVertices = geoData->validData.size();
    if (numVertices >= 3)
    {
        XMLElem vXML = newElement("ValidData", geoDataXML);
        setAttribute(vXML, "size", numVertices);

        for (size_t ii = 0; ii < numVertices; ++ii)
        {
            XMLElem vertexXML = common().createLatLon("Vertex", geoData->validData[ii],
                                                      vXML);
            setAttribute(vertexXML, "index", ii + 1);
        }
    }

    for (size_t ii = 0; ii < geoData->geoInfos.size(); ++ii)
    {
        common().convertGeoInfoToXML(*geoData->geoInfos[ii].get(), true, geoDataXML);
    }

    return geoDataXML;
}


XMLElem DerivedXMLParser300::convertDigitalElevationDataToXML(
        const DigitalElevationData& ded,
        XMLElem parent) const
{
    XMLElem dedElem = newElement("DigitalElevationData", parent);

    // GeographicCoordinates
    XMLElem geoCoordElem = newElement("GeographicCoordinates", dedElem);
    createDouble("LongitudeDensity",
                 ded.geographicCoordinates.longitudeDensity,
                 geoCoordElem);
    createDouble("LatitudeDensity",
                 ded.geographicCoordinates.latitudeDensity,
                 geoCoordElem);
    common().createLatLon("ReferenceOrigin",
                          ded.geographicCoordinates.referenceOrigin,
                          geoCoordElem);

    // Geopositioning
    XMLElem geoposElem = newElement("Geopositioning", dedElem);
    createStringFromEnum("CoordinateSystemType",
                         ded.geopositioning.coordinateSystemType,
                         geoposElem);
    createString("GeodeticDatum", ded.geopositioning.geodeticDatum,
                 geoposElem);
    createString("ReferenceEllipsoid", ded.geopositioning.referenceEllipsoid,
                 geoposElem);
    createString("VerticalDatum", ded.geopositioning.verticalDatum,
                 geoposElem);
    createString("SoundingDatum", ded.geopositioning.soundingDatum,
                 geoposElem);
    createInt("FalseOrigin", ded.geopositioning.falseOrigin, geoposElem);
    if (ded.geopositioning.coordinateSystemType == CoordinateSystemType::UTM)
    {
        createInt("UTMGridZoneNumber", ded.geopositioning.utmGridZoneNumber, geoposElem);
    }

    // PositionalAccuracy
    XMLElem posAccElem = newElement("PositionalAccuracy", dedElem);
    createInt("NumRegions", ded.positionalAccuracy.numRegions, posAccElem);

    XMLElem absAccElem = newElement("AbsoluteAccuracy", posAccElem);
    createDouble("Horizontal",
                 ded.positionalAccuracy.absoluteAccuracyHorizontal,
                 absAccElem);
    createDouble("Vertical",
                 ded.positionalAccuracy.absoluteAccuracyVertical,
                 absAccElem);

    XMLElem p2pAccElem = newElement("PointToPointAccuracy", posAccElem);
    createDouble("Horizontal",
                 ded.positionalAccuracy.pointToPointAccuracyHorizontal,
                 p2pAccElem);
    createDouble("Vertical",
                 ded.positionalAccuracy.pointToPointAccuracyVertical,
                 p2pAccElem);

    if (six::Init::isDefined(ded.nullValue))
    {
        createInt("NullValue", ded.nullValue, dedElem);
    }

    return dedElem;
}

void DerivedXMLParser300::parseGeoDataFromXML(
    const xml::lite::Element* geoDataXML, GeoDataBase* geoData) const
{
    common().parseEarthModelType(getFirstAndOnly(geoDataXML, "EarthModel"),
            geoData->earthModel);

    common().parseFootprint(getFirstAndOnly(geoDataXML, "ImageCorners"), "ICP",
            geoData->imageCorners);

    XMLElem tmpElem = getOptional(geoDataXML, "ValidData");
    if (tmpElem != nullptr)
    {
        common().parseLatLons(tmpElem, "Vertex", geoData->validData);
    }

    std::vector <XMLElem> geoInfosXML;
    geoDataXML->getElementsByTagName("GeoInfo", geoInfosXML);

    //optional
    size_t idx(geoData->geoInfos.size());
    geoData->geoInfos.resize(idx + geoInfosXML.size());

    for (std::vector<XMLElem>::const_iterator it = geoInfosXML.begin(); it
            != geoInfosXML.end(); ++it, ++idx)
    {
        geoData->geoInfos[idx].reset(new GeoInfo());
        common().parseGeoInfoFromXML(*it, geoData->geoInfos[idx].get());
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
        parseEnum(arpFlagElem, measurement->arpFlag);
    }

    common().parsePolyXYZ(getFirstAndOnly(measurementElem, "ARPPoly"),
        measurement->arpPoly);

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
        XMLElem collectionElem = collectionsElem[i];
        XMLElem geometryElem = getOptional(collectionElem, "Geometry");
        Collection& coll = *exploitationFeatures->collections[i];

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
        throw except::Exception(Ctxt(
            "ExploitationFeatures requires at least one Product"));
    }

    exploitationFeatures->product.resize(productElems.size());
    for (size_t ii = 0; ii < productElems.size(); ++ii)
    {
        XMLElem productElem = productElems[ii];
        Product& product = exploitationFeatures->product[ii];

        common().parseRowColDouble(getFirstAndOnly(productElem, "Resolution"),
                                   product.resolution);

        parseDouble(getFirstAndOnly(productElem, "Ellipticity"),
                    product.ellipticity);

        std::vector<XMLElem> polarizationElems;
        productElem->getElementsByTagName("Polarization", polarizationElems);
        if (polarizationElems.empty())
        {
            throw except::Exception(Ctxt(
                "Product requires at least one polarization"));
        }
        product.polarization.resize(polarizationElems.size());
        for (size_t jj = 0; jj < product.polarization.size(); ++jj)
        {
            XMLElem polarizationElem = polarizationElems[jj];
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

void DerivedXMLParser300::parseDigitalElevationDataFromXML(
        const xml::lite::Element* elem,
        DigitalElevationData& ded) const
{
    XMLElem coordElem = getFirstAndOnly(elem, "GeographicCoordinates");
    parseDouble(getFirstAndOnly(coordElem, "LongitudeDensity"), ded.geographicCoordinates.longitudeDensity);
    parseDouble(getFirstAndOnly(coordElem, "LatitudeDensity"), ded.geographicCoordinates.latitudeDensity);
    common().parseLatLon(getFirstAndOnly(coordElem, "ReferenceOrigin"), ded.geographicCoordinates.referenceOrigin);

    XMLElem posElem = getFirstAndOnly(elem, "Geopositioning");
    std::string coordSystemType;
    parseString(getFirstAndOnly(posElem, "CoordinateSystemType"), coordSystemType);
    ded.geopositioning.coordinateSystemType = CoordinateSystemType(coordSystemType);
    parseUInt(getFirstAndOnly(posElem, "FalseOrigin"), ded.geopositioning.falseOrigin);
    if (ded.geopositioning.coordinateSystemType == CoordinateSystemType::UTM)
    {
        parseInt(getFirstAndOnly(posElem, "UTMGridZoneNumber"), ded.geopositioning.utmGridZoneNumber);
    }
    XMLElem posAccuracyElem = getFirstAndOnly(elem, "PositionalAccuracy");
    parseUInt(getFirstAndOnly(posAccuracyElem, "NumRegions"), ded.positionalAccuracy.numRegions);
    XMLElem absoluteElem = getFirstAndOnly(posAccuracyElem, "AbsoluteAccuracy");
    parseDouble(getFirstAndOnly(absoluteElem, "Horizontal"), ded.positionalAccuracy.absoluteAccuracyHorizontal);
    parseDouble(getFirstAndOnly(absoluteElem, "Vertical"), ded.positionalAccuracy.absoluteAccuracyVertical);
    XMLElem pointElem = getFirstAndOnly(posAccuracyElem, "PointToPointAccuracy");
    parseDouble(getFirstAndOnly(pointElem, "Horizontal"), ded.positionalAccuracy.pointToPointAccuracyHorizontal);
    parseDouble(getFirstAndOnly(pointElem, "Vertical"), ded.positionalAccuracy.pointToPointAccuracyVertical);
}

std::unique_ptr<LUT> DerivedXMLParser300::parseSingleLUT(const xml::lite::Element& elem,
        size_t size) const
{
    std::string lutStr;
    parseString(&elem, lutStr);
    std::vector<std::string> lutVals = str::split(lutStr, " ");
    auto lut = std::make_unique<LUT>(size, sizeof(short));

    for (size_t ii = 0; ii < lutVals.size(); ++ii)
    {
        const short lutVal = str::toType<short>(lutVals[ii]);
        ::memcpy(&(lut->table[ii * lut->elementSize]),
            &lutVal, sizeof(short));
    }
    return lut;
}

XMLElem DerivedXMLParser300::createLUT(const std::string& name, const LUT *lut,
        XMLElem parent) const
{
    XMLElem lutElement = newElement(name, parent);
    return createLUTImpl(lut, lutElement);
}
}
}
