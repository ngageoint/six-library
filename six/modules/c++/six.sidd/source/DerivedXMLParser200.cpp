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

#include <sstream>

#include <six/SICommonXMLParser10x.h>
#include <six/sidd/DerivedDataBuilder.h>
#include <six/sidd/DerivedXMLParser200.h>

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
}

namespace six
{
namespace sidd
{

void DerivedXMLParser200::validateDRAFields(const six::sidd::DRAType& algorithmType,
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
void DerivedXMLParser200::validateDRAFields(const six::sidd::DynamicRangeAdjustment& adjust)
{
    validateDRAFields(adjust.algorithmType,
        adjust.draParameters.get() ? true : false,
        adjust.draOverrides.get() ? true : false);
}

ProjectionType DerivedXMLParser200::getProjectionType(const xml::lite::Element& measurementElem)
{
    six::ProjectionType projType = ProjectionType::NOT_SET;
    if (getOptional(measurementElem, "GeographicProjection"))
        projType = ProjectionType::GEOGRAPHIC;
    else if (getOptional(measurementElem, "CylindricalProjection"))
        projType = ProjectionType::CYLINDRICAL;
    else if (getOptional(measurementElem, "PlaneProjection"))
        projType = ProjectionType::PLANE;
    else if (getOptional(measurementElem, "PolynomialProjection"))
        projType = ProjectionType::POLYNOMIAL;
    return projType;
}


const char DerivedXMLParser200::VERSION[] = "2.0.0";
const char DerivedXMLParser200::SI_COMMON_URI[] = "urn:SICommon:1.0";
const char DerivedXMLParser200::ISM_URI[] = "urn:us:gov:ic:ism:13";

DerivedXMLParser200::DerivedXMLParser200(logging::Logger* log,
                                         bool ownLog) :
    DerivedXMLParser(VERSION,
                     std::unique_ptr<six::SICommonXMLParser>(
                         new six::SICommonXMLParser10x(versionToURI(VERSION),
                                                       false,
                                                       SI_COMMON_URI,
                                                       log)),
                     log,
                     ownLog)
{
}

DerivedData* DerivedXMLParser200::fromXML(
        const xml::lite::Document* doc) const
{
    const xml::lite::Element* const root = doc->getRootElement();

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
    const PixelType pixelType = six::toType<PixelType>(
            getFirstAndOnly(displayElem, "PixelType")->getCharacterData());
    builder.addDisplay(pixelType);

    // create GeoData
    builder.addGeoData();

    // create Measurement
    const auto projType = getProjectionType(*measurementElem);
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
        for (size_t i = 0; i < annChildren.size(); ++i)
        {
            data->annotations[i].reset(new Annotation());
            parseAnnotationFromXML(annChildren[i], data->annotations[i].get());
        }
    }
    return data;
}
std::unique_ptr<DerivedData> DerivedXMLParser200::fromXML(const xml::lite::Document& doc) const
{
    return std::unique_ptr<DerivedData>(fromXML(&doc));
}

xml::lite::Document* DerivedXMLParser200::toXML(const DerivedData* derived) const
{
    xml::lite::Document* doc = new xml::lite::Document();
    XMLElem root = newElement("SIDD");
    doc->setRootElement(root);

    convertProductCreationToXML(derived->productCreation.get(), root);
    convertDisplayToXML(*derived->display, root);
    convertGeoDataToXML(*this, *derived->geoData, *root);
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
        convertDigitalElevationDataToXML(*this, *derived->digitalElevationData,
                                         *root);
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
std::unique_ptr<xml::lite::Document> DerivedXMLParser200::toXML(const DerivedData& data) const
{
    return std::unique_ptr<xml::lite::Document>(toXML(&data));
}

void DerivedXMLParser200::parseDerivedClassificationFromXML(
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

void DerivedXMLParser200::parseCompressionFromXML(const xml::lite::Element* compressionElem,
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

void DerivedXMLParser200::parseJ2KCompression(const xml::lite::Element* j2kElem,
                                              J2KCompression& j2k) const
{
    parseInt(getFirstAndOnly(j2kElem, "NumWaveletLevels"),
            j2k.numWaveletLevels);
    parseInt(getFirstAndOnly(j2kElem, "NumBands"),
            j2k.numBands);

    XMLElem layerInfoElems = getFirstAndOnly(j2kElem, "LayerInfo");
    std::vector<XMLElem> layerElems;
    layerInfoElems->getElementsByTagName("Layer", layerElems);

    const auto numLayers = layerElems.size();
    j2k.layerInfo.resize(numLayers);

    for (size_t ii = 0; ii < layerElems.size(); ++ii)
    {
        parseDouble(getFirstAndOnly(layerElems[ii], "Bitrate"),
                    j2k.layerInfo[ii].bitRate);
    }
}

void DerivedXMLParser200::parseDisplayFromXML(const xml::lite::Element* displayElem,
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

void DerivedXMLParser200::parseNonInteractiveProcessingFromXML(
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

void DerivedXMLParser200::parseProductGenerationOptionsFromXML(
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

void DerivedXMLParser200::parseLookupTableFromXML(
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
                std::unique_ptr<LUT> lut = parseSingleLUT(lutElems[ii], size);
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

void DerivedXMLParser200::parseBandEqualizationFromXML(const xml::lite::Element* bandElem,
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

void DerivedXMLParser200::parseRRDSFromXML(const xml::lite::Element* rrdsElem,
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

void DerivedXMLParser200::parseFilterFromXML(const xml::lite::Element* filterElem,
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

void DerivedXMLParser200::parsePredefinedFilterFromXML(const xml::lite::Element* predefinedElem,
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

void DerivedXMLParser200::parseKernelFromXML(const xml::lite::Element* kernelElem,
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
void DerivedXMLParser200::parseBankFromXML(const xml::lite::Element* bankElem,
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

void DerivedXMLParser200::parseInteractiveProcessingFromXML(
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

void DerivedXMLParser200::parseGeometricTransformFromXML(const xml::lite::Element* geomElem,
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

void DerivedXMLParser200::parseSharpnessEnhancementFromXML(
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

void DerivedXMLParser200::parseColorSpaceTransformFromXML(
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

void DerivedXMLParser200::parseDynamicRangeAdjustmentFromXML(
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

XMLElem DerivedXMLParser200::convertDerivedClassificationToXML(
        const DerivedClassification& classification,
        XMLElem parent) const
{
    assert(parent != nullptr);
    return &convertDerivedClassificationToXML(*this, classification, *parent);
}
xml::lite::Element& DerivedXMLParser200::convertDerivedClassificationToXML(const DerivedXMLParser& parser,
    const DerivedClassification& classification, xml::lite::Element& parent)
{
    auto& classElem_ = parser.newElement("Classification", parent);
    auto classElem = &classElem_;

    parser.common().addParameters("SecurityExtension",
                    classification.securityExtensions,
                           classElem);

    //! from ism:ISMRootNodeAttributeGroup
    // SIDD 2.0 is tied to IC-ISM v13
    parser.setAttribute(classElem, "DESVersion", "13", ISM_URI);

    // So far as I can tell this should just be 1
    parser.setAttribute(classElem, "ISMCATCESVersion", "1", ISM_URI);

    //! from ism:ResourceNodeAttributeGroup
    parser.setAttribute(classElem, "resourceElement", "true", ISM_URI);
    parser.setAttribute(classElem, "createDate",
                 classification.createDate.format("%Y-%m-%d"), ISM_URI);
    // required (was optional in SIDD 1.0)
    parser.setAttributeList(classElem, "compliesWith", classification.compliesWith,
                     ISM_URI);

    // optional
    parser.setAttributeIfNonEmpty(classElem,
                           "exemptFrom",
                           classification.exemptFrom,
                           ISM_URI);

    //! from ism:SecurityAttributesGroup
    //  -- referenced in ism::ResourceNodeAttributeGroup
    parser.setAttribute(classElem, "classification", classification.classification,
                 ISM_URI);
    parser.setAttributeList(classElem, "ownerProducer", classification.ownerProducer,
                     ISM_URI, true);

    // optional
    parser.setAttributeIfNonEmpty(classElem, "joint", classification.joint, ISM_URI);

    // optional
    parser.setAttributeList(classElem, "SCIcontrols", classification.sciControls,
                     ISM_URI);
    // optional
    parser.setAttributeList(classElem, "SARIdentifier", classification.sarIdentifier,
                     ISM_URI);
    // optional
    parser.setAttributeList(classElem,
                     "atomicEnergyMarkings",
                     classification.atomicEnergyMarkings,
                     ISM_URI);
    // optional
    parser.setAttributeList(classElem,
                     "disseminationControls",
                     classification.disseminationControls,
                     ISM_URI);
    // optional
    parser.setAttributeList(classElem,
                     "displayOnlyTo",
                     classification.displayOnlyTo,
                     ISM_URI);
    // optional
    parser.setAttributeList(classElem, "FGIsourceOpen", classification.fgiSourceOpen,
                     ISM_URI);
    // optional
    parser.setAttributeList(classElem,
                     "FGIsourceProtected",
                     classification.fgiSourceProtected,
                     ISM_URI);
    // optional
    parser.setAttributeList(classElem, "releasableTo", classification.releasableTo,
                     ISM_URI);
    // optional
    parser.setAttributeList(classElem, "nonICmarkings", classification.nonICMarkings,
                     ISM_URI);
    // optional
    parser.setAttributeIfNonEmpty(classElem,
                           "classifiedBy",
                           classification.classifiedBy,
                           ISM_URI);
    // optional
    parser.setAttributeIfNonEmpty(classElem,
                           "compilationReason",
                           classification.compilationReason,
                           ISM_URI);
    // optional
    parser.setAttributeIfNonEmpty(classElem,
                           "derivativelyClassifiedBy",
                           classification.derivativelyClassifiedBy,
                           ISM_URI);
    // optional
    parser.setAttributeIfNonEmpty(classElem,
                           "classificationReason",
                           classification.classificationReason,
                           ISM_URI);
    // optional
    parser.setAttributeList(classElem, "nonUSControls", classification.nonUSControls,
                     ISM_URI);
    // optional
    parser.setAttributeIfNonEmpty(classElem,
                           "derivedFrom",
                           classification.derivedFrom,
                           ISM_URI);
    // optional
    if (classification.declassDate.get())
    {
        parser.setAttributeIfNonEmpty(
                classElem, "declassDate",
                classification.declassDate->format("%Y-%m-%d"),
                ISM_URI);
    }
    // optional
    parser.setAttributeIfNonEmpty(classElem,
                           "declassEvent",
                           classification.declassEvent,
                           ISM_URI);
    // optional
    parser.setAttributeIfNonEmpty(classElem,
                           "declassException",
                           classification.declassException,
                           ISM_URI);

    //! from ism:NoticeAttributesGroup
    // optional
    parser.setAttributeIfNonEmpty(classElem,
                           "noticeType",
                           classification.noticeType,
                           ISM_URI);
    // optional
    parser.setAttributeIfNonEmpty(classElem,
                           "noticeReason",
                           classification.noticeReason,
                           ISM_URI);
    // optional
    if (classification.noticeDate.get())
    {
        parser.setAttributeIfNonEmpty(
                classElem, "noticeDate",
                classification.noticeDate->format("%Y-%m-%d"),
                ISM_URI);
    }
    // optional
    parser.setAttributeIfNonEmpty(classElem,
                           "unregisteredNoticeType",
                           classification.unregisteredNoticeType,
                           ISM_URI);
    // optional
    parser.setAttributeIfNonEmpty(classElem,
                           "externalNotice",
                           classification.externalNotice,
                           ISM_URI);

    return classElem_;
}

xml::lite::Element& DerivedXMLParser200::convertLookupTableToXML(const DerivedXMLParser& parser,
    const std::string& name, const LookupTable& table, xml::lite::Element& parent)
{
    auto& lookupElem = parser.newElement(name, parent);
    parser.createString("LUTName", table.lutName, lookupElem);

    bool ok = false;
    if (table.predefined.get())
    {
        if (table.custom.get() == nullptr)
        {
            ok = true;
            auto& predefElem = parser.newElement("Predefined", lookupElem);

            //exactly one of databaseName or (remapFamily and remapMember) can be set
            bool innerOk = false;
            if (table.predefined->databaseName.empty())
            {
                if (six::Init::isDefined(table.predefined->remapFamily) &&
                    six::Init::isDefined(table.predefined->remapMember))
                {
                    innerOk = true;
                    parser.createInt("RemapFamily", table.predefined->remapFamily, predefElem);
                    parser.createInt("RemapMember", table.predefined->remapMember, predefElem);
                }
            }
            else if (six::Init::isUndefined(table.predefined->remapFamily) &&
                     six::Init::isUndefined(table.predefined->remapMember))
            {
                innerOk = true;
                parser.createString("DatabaseName", table.predefined->databaseName, predefElem);
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

        auto& customElem = parser.newElement("Custom", lookupElem);
        auto& lutInfoElem = parser.newElement("LUTInfo", customElem);
        parser.setAttribute(lutInfoElem, "numLuts", lutValues.size());
        parser.setAttribute(lutInfoElem, "size", lutValues[0].table.size());

        for (size_t ii = 0; ii < lutValues.size(); ++ii)
        {
            auto lutElem = parser.createLUT("LUTValues", &lutValues[ii], &lutInfoElem);
            parser.setAttribute(lutElem, "lut", ii + 1);
        }
    }
    if (!ok)
    {
        throw except::Exception(Ctxt("Exactly one of Predefined or Custom must be defined"));
    }
    return lookupElem;
}

xml::lite::Element& DerivedXMLParser200::convertNonInteractiveProcessingToXML(const DerivedXMLParser& parser,
    const NonInteractiveProcessing& processing, xml::lite::Element& parent)
{
    auto& processingElem = parser.newElement("NonInteractiveProcessing", parent);

    // ProductGenerationOptions
    auto& prodGenElem = parser.newElement("ProductGenerationOptions",
                                    processingElem);

    const ProductGenerationOptions& prodGen =
            processing.productGenerationOptions;

    if (prodGen.bandEqualization.get())
    {
        const BandEqualization& bandEq = *prodGen.bandEqualization;
        auto& bandEqElem = parser.newElement("BandEqualization", prodGenElem);
        if (int(bandEq.algorithm) == 0)
        {
            parser.createString("Algorithm", "1DLUT", &bandEqElem);
        }
        else
        {
            parser.createStringFromEnum("Algorithm", bandEq.algorithm, bandEqElem);
        }
        for (size_t ii = 0; ii < bandEq.bandLUTs.size(); ++ii)
        {
            convertLookupTableToXML(parser, "BandLUT", *bandEq.bandLUTs[ii], bandEqElem);
        }

        //add the attribute to each of the LUTs
        std::vector<XMLElem> LUTElems;
        bandEqElem.getElementsByTagName("BandLUT", LUTElems);
        for (size_t ii = 0; ii < LUTElems.size(); ++ii)
        {
            setAttribute(LUTElems[ii], "k", ii+1);
        }
    }

    if (prodGen.modularTransferFunctionRestoration.get())
    {
        convertFilterToXML(parser, "ModularTransferFunctionRestoration",
                           *prodGen.modularTransferFunctionRestoration,
                           prodGenElem);
    }

    if (prodGen.dataRemapping.get())
    {
        convertLookupTableToXML(parser, "DataRemapping", *prodGen.dataRemapping,
                                prodGenElem);
    }

    if (prodGen.asymmetricPixelCorrection.get())
    {
        convertFilterToXML(parser, "AsymmetricPixelCorrection",
                           *prodGen.asymmetricPixelCorrection, prodGenElem);
    }

    // RRDS
    auto& rrdsElem = parser.newElement("RRDS", processingElem);

    const RRDS& rrds = processing.rrds;
    parser.createStringFromEnum("DownsamplingMethod", rrds.downsamplingMethod,
                         rrdsElem);

    if (rrds.downsamplingMethod != DownsamplingMethod::DECIMATE && rrds.downsamplingMethod != DownsamplingMethod::MAX_PIXEL)
    {
        confirmNonNull(rrds.antiAlias, "antiAlias",
                       "for DECIMATE downsampling");
        convertFilterToXML(parser, "AntiAlias", *rrds.antiAlias, rrdsElem);

        confirmNonNull(rrds.interpolation, "interpolation",
                       "for DECIMATE downsampling");
        convertFilterToXML(parser, "Interpolation", *rrds.interpolation, rrdsElem);
    }
    return processingElem;
}

xml::lite::Element& DerivedXMLParser200::convertInteractiveProcessingToXML(const DerivedXMLParser& parser,
    const InteractiveProcessing& processing, xml::lite::Element& parent)
{
    auto& processingElem = parser.newElement("InteractiveProcessing", parent);

    // GeometricTransform
    const GeometricTransform& geoTransform(processing.geometricTransform);
    auto& geoTransformElem = parser.newElement("GeometricTransform", processingElem);

    auto& scalingElem = parser.newElement("Scaling", geoTransformElem);
    convertFilterToXML(parser, "AntiAlias", geoTransform.scaling.antiAlias,
                       scalingElem);
    convertFilterToXML(parser, "Interpolation", geoTransform.scaling.interpolation,
                       scalingElem);

    auto& orientationElem = parser.newElement("Orientation", geoTransformElem);
    parser.createStringFromEnum("ShadowDirection",
        geoTransform.orientation.shadowDirection,
        orientationElem);

    // SharpnessEnhancement
    const SharpnessEnhancement& sharpness(processing.sharpnessEnhancement);
    auto& sharpElem = parser.newElement("SharpnessEnhancement", processingElem);

    bool ok = false;
    if (sharpness.modularTransferFunctionCompensation.get())
    {
        if (sharpness.modularTransferFunctionEnhancement.get() == nullptr)
        {
            ok = true;
            convertFilterToXML(parser, "ModularTransferFunctionCompensation",
                               *sharpness.modularTransferFunctionCompensation,
                               sharpElem);
        }
    }
    else if (sharpness.modularTransferFunctionEnhancement.get())
    {
        ok = true;
        convertFilterToXML(parser, "ModularTransferFunctionEnhancement",
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

        auto& colorSpaceTransformElem =
            parser.newElement("ColorSpaceTransform", processingElem);
        auto& cmmElem =
            parser.newElement("ColorManagementModule", colorSpaceTransformElem);

        if (int(cmm.renderingIntent) == 2)
        {
            parser.createString("RenderingIntent", "RELATIVE", &cmmElem);
        }
        else if (int(cmm.renderingIntent) == 3)
        {
            parser.createString("RenderingIntent", "ABSOLUTE", &cmmElem);
        }
        else
        {
            parser.createStringFromEnum("RenderingIntent", cmm.renderingIntent, cmmElem);
        }

        // TODO: Not sure what this'll actually look like
        parser.createString("SourceProfile", cmm.sourceProfile, cmmElem);
        if (!cmm.displayProfile.empty())
        {
            parser.createString("DisplayProfile", cmm.displayProfile, cmmElem);
        }

        if (!cmm.iccProfile.empty())
        {
            parser.createString("ICCProfileSignature", cmm.iccProfile, cmmElem);
        }
    }

    // DynamicRangeAdjustment

    const DynamicRangeAdjustment& adjust =
            processing.dynamicRangeAdjustment;

    auto& adjustElem =
        parser.newElement("DynamicRangeAdjustment", processingElem);

    parser.createStringFromEnum("AlgorithmType", adjust.algorithmType,
        adjustElem);
    parser.createInt("BandStatsSource", adjust.bandStatsSource, adjustElem);

    validateDRAFields(adjust);
    if (adjust.draParameters.get())
    {
        auto& paramElem = parser.newElement("DRAParameters", adjustElem);
        parser.createDouble("Pmin", adjust.draParameters->pMin, paramElem);
        parser.createDouble("Pmax", adjust.draParameters->pMax, paramElem);
        parser.createDouble("EminModifier", adjust.draParameters->eMinModifier, paramElem);
        parser.createDouble("EmaxModifier", adjust.draParameters->eMinModifier, paramElem);
    }
    if (adjust.draOverrides.get())
    {
        auto& overrideElem = parser.newElement("DRAOverrides", adjustElem);
        parser.createDouble("Subtractor", adjust.draOverrides->subtractor, overrideElem);
        parser.createDouble("Multiplier", adjust.draOverrides->multiplier, overrideElem);
    }

    if (processing.tonalTransferCurve.get())
    {
        convertLookupTableToXML(parser, "TonalTransferCurve", *processing.tonalTransferCurve, processingElem);
    }
    return processingElem;
}

xml::lite::Element& DerivedXMLParser200::convertPredefinedFilterToXML(const DerivedXMLParser& parser,
    const Filter::Predefined& predefined, xml::lite::Element& parent)
{
    auto& predefinedElem = parser.newElement("Predefined", parent);

    // Make sure either DBName or FilterFamily+FilterMember are defined
    bool ok = false;
    if (isDefined(predefined.databaseName))
    {
        if (six::Init::isUndefined(predefined.filterFamily) &&
            six::Init::isUndefined(predefined.filterMember))
        {
            ok = true;

            parser.createStringFromEnum("DatabaseName", predefined.databaseName, predefinedElem);
        }
    }
    else if (six::Init::isDefined(predefined.filterFamily) &&
             six::Init::isDefined(predefined.filterMember))
    {
        ok = true;

        parser.createInt("FilterFamily", predefined.filterFamily, predefinedElem);
        parser.createInt("FilterMember", predefined.filterMember, predefinedElem);
    }

    if (!ok)
    {
        throw except::Exception(Ctxt(
                "Exactly one of either dbName or FilterFamily and "
                "FilterMember must be defined"));
    }

    return predefinedElem;
}

xml::lite::Element& DerivedXMLParser200::convertKernelToXML(const DerivedXMLParser& parser,
    const Filter::Kernel& kernel, xml::lite::Element& parent)
{
    auto& kernelElem = parser.newElement("FilterKernel", parent);

    bool ok = false;
    if (kernel.predefined.get())
    {
        if (kernel.custom.get() == nullptr)
        {
            ok = true;
            convertPredefinedFilterToXML(parser, *kernel.predefined, kernelElem);
        }
    }
    else if (kernel.custom.get())
    {
        ok = true;

        auto& customElem = parser.newElement("Custom", kernelElem);

        if (kernel.custom->filterCoef.size() !=
            static_cast<size_t>(kernel.custom->size.row) * kernel.custom->size.col)
        {
            std::ostringstream ostr;
            ostr << "Filter size is " << kernel.custom->size.row << " rows x "
                << kernel.custom->size.col << " cols but have "
                << kernel.custom->filterCoef.size() << " coefficients";
            throw except::Exception(Ctxt(ostr.str()));
        }

        auto& filterCoef = parser.newElement("FilterCoefficients", customElem);
        setAttribute(filterCoef, "numRows", static_cast<size_t>(kernel.custom->size.row));
        setAttribute(filterCoef, "numCols", static_cast<size_t>(kernel.custom->size.col));

        for (ptrdiff_t row = 0, idx = 0; row < kernel.custom->size.row; ++row)
        {
            for (ptrdiff_t col = 0; col < kernel.custom->size.col; ++col, ++idx)
            {
                auto& coefElem = parser.createDouble("Coef", kernel.custom->filterCoef[static_cast<size_t>(idx)],
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

xml::lite::Element& DerivedXMLParser200::convertBankToXML(const DerivedXMLParser& parser,
    const Filter::Bank& bank, xml::lite::Element& parent)
{
    auto& bankElem = parser.newElement("FilterBank", parent);

    bool ok = false;
    if (bank.predefined.get())
    {
        if (bank.custom.get() == nullptr)
        {
            ok = true;
            convertPredefinedFilterToXML(parser, *bank.predefined, bankElem);
        }
    }
    else if (bank.custom.get())
    {
        ok = true;

        auto& customElem = parser.newElement("Custom", bankElem);

        if (bank.custom->filterCoef.size() !=
            static_cast<size_t>(bank.custom->numPhasings) * bank.custom->numPoints)
        {
            std::ostringstream ostr;
            ostr << "Filter size is " << bank.custom->numPhasings << " x "
                << bank.custom->numPoints << " but have "
                << bank.custom->filterCoef.size() << " coefficients";
            throw except::Exception(Ctxt(ostr.str()));
        }

        auto& filterCoef = parser.newElement("FilterCoefficients", customElem);
        parser.setAttribute(filterCoef, "numPhasings", bank.custom->numPhasings);
        parser.setAttribute(filterCoef, "numPoints", bank.custom->numPoints);

        for (size_t row = 0, idx = 0; row < bank.custom->numPhasings; ++row)
        {
            for (size_t col = 0; col < bank.custom->numPoints; ++col, ++idx)
            {
                auto& coefElem = parser.createDouble("Coef", bank.custom->filterCoef[idx],
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

xml::lite::Element& DerivedXMLParser200::convertFilterToXML(const DerivedXMLParser& parser,
    const std::string& name, const Filter& filter, xml::lite::Element& parent)
{
    auto& filterElem = parser.newElement(name, parent);

    parser.createString("FilterName", filter.filterName, filterElem);

    // Exactly one of Kernel or Bank should be set
    bool ok = false;
    if (filter.filterKernel.get())
    {
        if (filter.filterBank.get() == nullptr)
        {
            ok = true;
            convertKernelToXML(parser, *filter.filterKernel, filterElem);
        }
    }
    else if (filter.filterBank.get())
    {
        ok = true;
        convertBankToXML(parser, *filter.filterBank, filterElem);
    }

    if (!ok)
    {
        throw except::Exception(Ctxt(
                "Exactly one of kernel or bank must be set"));
    }

    parser.createStringFromEnum("Operation", filter.operation, filterElem);

    return filterElem;
}

XMLElem DerivedXMLParser200::convertCompressionToXML(
        const Compression& compression,
        XMLElem parent) const
{
    assert(parent != nullptr);
    return &convertCompressionToXML(*this, compression, *parent);
}
xml::lite::Element& DerivedXMLParser200::convertCompressionToXML(const DerivedXMLParser& parser,
    const Compression& compression, xml::lite::Element& parent)
{
    auto& compressionElem = parser.newElement("Compression", parent);
    auto& j2kElem = parser.newElement("J2K", compressionElem);
    auto& originalElem = parser.newElement("Original", j2kElem);
    convertJ2KToXML(parser, compression.original, originalElem);

    if (compression.parsed.get())
    {
        auto& parsedElem = parser.newElement("Parsed", j2kElem);
        convertJ2KToXML(parser, *compression.parsed, parsedElem);
    }
    return compressionElem;
}

void DerivedXMLParser200::convertJ2KToXML(const DerivedXMLParser& parser,
    const J2KCompression& j2k, xml::lite::Element& parent)
{
    parser.createInt("NumWaveletLevels", j2k.numWaveletLevels, parent);
    parser.createInt("NumBands", j2k.numBands, parent);

    const auto numLayers = j2k.layerInfo.size();
    auto& layerInfoElem = parser.newElement("LayerInfo", parent);
    parser.setAttribute(layerInfoElem, "numLayers", numLayers);

    for (size_t ii = 0; ii < numLayers; ++ii)
    {
        auto& layerElem = parser.newElement("Layer", layerInfoElem);
        parser.setAttribute(layerElem, "index", ii + 1);
        parser.createDouble("Bitrate", j2k.layerInfo[ii].bitRate, layerElem);
    }
}

XMLElem DerivedXMLParser200::convertMeasurementToXML(const Measurement* measurement,
    XMLElem parent) const
{
    assert(parent != nullptr);
    assert(measurement != nullptr);
    return &convertMeasurementToXML(*this, *measurement, *parent);
}
xml::lite::Element& DerivedXMLParser200::convertMeasurementToXML(const DerivedXMLParser& parser,
    const Measurement& measurement, xml::lite::Element& parent)
{
    auto& measurementElem = DerivedXMLParser::convertMeasurementToXML(parser, measurement, parent);

    if (measurement.arpFlag != AppliedType::NOT_SET)
    {
        parser.createStringFromEnum("ARPFlag", measurement.arpFlag, measurementElem);
    }

    parser.common().createPolyXYZ("ARPPoly",
        measurement.arpPoly,
        &measurementElem);

    //only if 3+ vertices
    const size_t numVertices = measurement.validData.size();
    if (numVertices >= 3)
    {
        auto& vElem = parser.newElement("ValidData", measurementElem);
        setAttribute(vElem, "size", numVertices);

        for (size_t ii = 0; ii < numVertices; ++ii)
        {
            auto vertexElem = parser.common().createRowCol(
                "Vertex", measurement.validData[ii], &vElem);
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

XMLElem DerivedXMLParser200::convertExploitationFeaturesToXML(
    const ExploitationFeatures* exploitationFeatures,
    XMLElem parent) const
{
    assert(exploitationFeatures != nullptr);
    assert(parent != nullptr);
    return &convertExploitationFeaturesToXML(*this, *exploitationFeatures, *parent);
}
xml::lite::Element& DerivedXMLParser200::convertExploitationFeaturesToXML(const DerivedXMLParser& parser,
    const ExploitationFeatures& exploitationFeatures, xml::lite::Element& parent)
{
    auto& exploitationFeaturesElem =
        parser.newElement("ExploitationFeatures", parent);

    if (exploitationFeatures.collections.empty())
    {
        throw except::Exception(Ctxt(FmtX(
            "ExploitationFeatures must have at least [1] Collection, " \
            "only [%d] found", exploitationFeatures.collections.size())));
    }

    // 1 to unbounded
    for (auto& pCollections : exploitationFeatures.collections)
    {
        Collection* collection = pCollections.get();
        auto& collectionElem = parser.newElement("Collection",
            exploitationFeaturesElem);
        setAttribute(&collectionElem, "identifier", collection->identifier);

        // create Information
        auto& informationElem = parser.newElement("Information", collectionElem);

        parser.createString("SensorName",
            collection->information.sensorName,
            informationElem);
        auto& radarModeElem = parser.newElement("RadarMode", informationElem);
        parser.createSixString("ModeType",
            parser.common().getSICommonURI(),
            collection->information.radarMode,
            &radarModeElem);
        // optional
        if (!collection->information.radarModeID.empty())
            parser.createString("ModeID",
                parser.common().getSICommonURI(),
                collection->information.radarModeID,
                &radarModeElem);
        parser.createDateTime("CollectionDateTime",
            collection->information.collectionDateTime,
            &informationElem);
        // optional
        if (collection->information.localDateTime != Init::undefined<
            six::DateTime>())
        {
            parser.createDateTime("LocalDateTime",
                collection->information.localDateTime,
                &informationElem);
        }
        parser.createDouble("CollectionDuration",
            collection->information.collectionDuration,
            informationElem);
        // optional
        if (!Init::isUndefined(collection->information.resolution))
        {
            parser.common().createRangeAzimuth("Resolution",
                collection->information.resolution,
                &informationElem);
        }
        // optional
        if (collection->information.inputROI.get())
        {
            auto& roiElem = parser.newElement("InputROI", informationElem);
            parser.common().createRowCol("Size",
                collection->information.inputROI->size,
                &roiElem);
            parser.common().createRowCol("UpperLeft",
                collection->information.inputROI->upperLeft,
                &roiElem);
        }
        // optional to unbounded
        for (const auto& pPolarization : collection->information.polarization)
        {
            const TxRcvPolarization *p = pPolarization.get();
            auto& polElem = parser.newElement("Polarization", informationElem);

            parser.createString("TxPolarization",
                p->txPolarization,
                polElem);
            parser.createString("RcvPolarization",
                p->rcvPolarization,
                polElem);
            // optional
            if (!Init::isUndefined(p->rcvPolarizationOffset))
            {
                parser.createDouble("RcvPolarizationOffset",
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
        const Geometry* geom = collection->geometry.get();
        if (geom != nullptr)
        {
            auto& geometryElem = parser.newElement("Geometry", collectionElem);

            // optional
            if (geom->azimuth != Init::undefined<double>())
                parser.createDouble("Azimuth", geom->azimuth, geometryElem);
            // optional
            if (geom->slope != Init::undefined<double>())
                parser.createDouble("Slope", geom->slope, geometryElem);
            // optional
            if (geom->squint != Init::undefined<double>())
                parser.createDouble("Squint", geom->squint, geometryElem);
            // optional
            if (geom->graze != Init::undefined<double>())
                parser.createDouble("Graze", geom->graze, geometryElem);
            // optional
            if (geom->tilt != Init::undefined<double>())
                parser.createDouble("Tilt", geom->tilt, geometryElem);
            // optional
            if (geom->dopplerConeAngle != Init::undefined<double>())
                parser.createDouble("DopplerConeAngle", geom->dopplerConeAngle, geometryElem);
            // optional to unbounded
            parser.common().addParameters("Extension", geom->extensions,
                &geometryElem);
        }

        // create Phenomenology -- optional
        const Phenomenology* phenom = collection->phenomenology.get();
        if (phenom != nullptr)
        {
            auto& phenomenologyElem = parser.newElement("Phenomenology",
                collectionElem);

            // optional
            if (phenom->shadow != Init::undefined<AngleMagnitude>())
            {
                auto& shadow = parser.newElement("Shadow", phenomenologyElem);
                parser.createDouble("Angle", parser.common().getSICommonURI(),
                    phenom->shadow.angle, &shadow);
                parser.createDouble("Magnitude", parser.common().getSICommonURI(),
                    phenom->shadow.magnitude, &shadow);
            }
            // optional
            if (phenom->layover != Init::undefined<AngleMagnitude>())
            {
                auto& layover = parser.newElement("Layover", phenomenologyElem);
                parser.createDouble("Angle", parser.common().getSICommonURI(),
                    phenom->layover.angle, &layover);
                parser.createDouble("Magnitude", parser.common().getSICommonURI(),
                    phenom->layover.magnitude, &layover);
            }
            // optional
            if (phenom->multiPath != Init::undefined<double>())
                parser.createDouble("MultiPath", phenom->multiPath, phenomenologyElem);
            // optional
            if (phenom->groundTrack != Init::undefined<double>())
                parser.createDouble("GroundTrack", phenom->groundTrack,
                    phenomenologyElem);
            // optional to unbounded
            parser.common().addParameters("Extension", phenom->extensions,
                &phenomenologyElem);
        }
    }

    // create Product
    if (exploitationFeatures.product.empty())
    {
        throw except::Exception(Ctxt(
            "ExploitationFeatures must have at least one Product"));
    }

    for (size_t ii = 0; ii < exploitationFeatures.product.size(); ++ii)
    {
        auto& productElem = parser.newElement("Product", exploitationFeaturesElem);
        const Product& product = exploitationFeatures.product[ii];

        parser.common().createRowCol("Resolution",
                              product.resolution,
                              &productElem);

        parser.createDouble("Ellipticity", product.ellipticity, productElem);

        if (product.polarization.empty())
        {
            throw except::Exception(Ctxt(
                "Product must have at least one Polarization"));
        }

        for (size_t jj = 0; jj < product.polarization.size(); ++jj)
        {
            auto& polarizationElem = parser.newElement("Polarization", productElem);
            parser.createStringFromEnum("TxPolarizationProc",
                                 product.polarization[jj].txPolarizationProc,
                                 polarizationElem);
            parser.createStringFromEnum("RcvPolarizationProc",
                                 product.polarization[jj].rcvPolarizationProc,
                                 polarizationElem);
        }

        // optional
        if (product.north != Init::undefined<double>())
            parser.createDouble("North", product.north, productElem);

        // optional to unbounded
        parser.common().addParameters("Extension",
                               product.extensions,
                               &productElem);
    }

    return exploitationFeaturesElem;
}

XMLElem DerivedXMLParser200::convertDisplayToXML(
        const Display& display,
        XMLElem parent) const
{
    assert(parent != nullptr);
    return &convertDisplayToXML(*this, display, *parent);
}
xml::lite::Element& DerivedXMLParser200::convertDisplayToXML(const DerivedXMLParser& parser,
    const Display& display, xml::lite::Element& parent)
{
    // NOTE: In several spots here, there are fields which are required in
    //       SIDD 2.0 but a pointer in the Display class since it didn't exist
    //       in SIDD 1.0, so need to confirm it's allocated
    auto& displayElem = parser.newElement("Display", parent);

    parser.createString("PixelType", display.pixelType, displayElem);

    parser.createInt("NumBands", display.numBands, displayElem);
    if (six::Init::isDefined(display.defaultBandDisplay))
    {
        parser.createInt("DefaultBandDisplay", display.defaultBandDisplay, displayElem);
    }

    // NonInteractiveProcessing
    for (size_t ii = 0; ii < display.nonInteractiveProcessing.size(); ++ii)
    {
        confirmNonNull(display.nonInteractiveProcessing[ii],
                "nonInteractiveProcessing");
        auto& temp = convertNonInteractiveProcessingToXML(parser,
                *display.nonInteractiveProcessing[ii],
                displayElem);
        setAttribute(temp, "band", ii + 1);
    }

    for (size_t ii = 0; ii < display.interactiveProcessing.size(); ++ii)
    {
        // InteractiveProcessing
        confirmNonNull(display.interactiveProcessing[ii],
                "interactiveProcessing");
        auto& temp = convertInteractiveProcessingToXML(parser,
                *display.interactiveProcessing[ii],
                displayElem);
        setAttribute(temp, "band", ii + 1);
    }

    // optional to unbounded
    parser.common().addParameters("DisplayExtension", display.displayExtensions,
                           &displayElem);
    return displayElem;
}

xml::lite::Element& DerivedXMLParser200::convertGeoDataToXML(const DerivedXMLParser& parser,
    const GeoDataBase& geoData, xml::lite::Element& parent)
{
    auto& geoDataXML = parser.newElement("GeoData", parent);

    parser.common().createEarthModelType("EarthModel", geoData.earthModel, &geoDataXML);
    parser.common().createLatLonFootprint("ImageCorners", "ICP", geoData.imageCorners, &geoDataXML);

    //only if 3+ vertices
    const size_t numVertices = geoData.validData.size();
    if (numVertices >= 3)
    {
        auto& vXML = parser.newElement("ValidData", geoDataXML);
        setAttribute(vXML, "size", numVertices);

        for (size_t ii = 0; ii < numVertices; ++ii)
        {
            auto vertexXML = parser.common().createLatLon("Vertex", geoData.validData[ii], &vXML);
            parser.setAttribute(vertexXML, "index", ii + 1);
        }
    }

    for (size_t ii = 0; ii < geoData.geoInfos.size(); ++ii)
    {
        parser.common().convertGeoInfoToXML(*geoData.geoInfos[ii].get(), true, &geoDataXML);
    }

    return geoDataXML;
}


xml::lite::Element& DerivedXMLParser200::convertDigitalElevationDataToXML(const DerivedXMLParser& parser,
    const DigitalElevationData& ded, xml::lite::Element& parent)
{
    auto& dedElem = parser.newElement("DigitalElevationData", parent);

    // GeographicCoordinates
    auto& geoCoordElem = parser.newElement("GeographicCoordinates", dedElem);
    parser.createDouble("LongitudeDensity",
                 ded.geographicCoordinates.longitudeDensity,
                 geoCoordElem);
    parser.createDouble("LatitudeDensity",
                 ded.geographicCoordinates.latitudeDensity,
                 geoCoordElem);
    parser.common().createLatLon("ReferenceOrigin",
                          ded.geographicCoordinates.referenceOrigin,
                          &geoCoordElem);

    // Geopositioning
    auto& geoposElem = parser.newElement("Geopositioning", dedElem);
    parser.createStringFromEnum("CoordinateSystemType",
                         ded.geopositioning.coordinateSystemType,
                         geoposElem);
    parser.createString("GeodeticDatum", ded.geopositioning.geodeticDatum,
                 geoposElem);
    parser.createString("ReferenceEllipsoid", ded.geopositioning.referenceEllipsoid,
                 geoposElem);
    parser.createString("VerticalDatum", ded.geopositioning.verticalDatum,
                 geoposElem);
    parser.createString("SoundingDatum", ded.geopositioning.soundingDatum,
                 geoposElem);
    parser.createInt("FalseOrigin", ded.geopositioning.falseOrigin, geoposElem);
    if (ded.geopositioning.coordinateSystemType == CoordinateSystemType::UTM)
    {
        parser.createInt("UTMGridZoneNumber", ded.geopositioning.utmGridZoneNumber, geoposElem);
    }

    // PositionalAccuracy
    auto& posAccElem = parser.newElement("PositionalAccuracy", dedElem);
    parser.createInt("NumRegions", ded.positionalAccuracy.numRegions, posAccElem);

    auto& absAccElem = parser.newElement("AbsoluteAccuracy", posAccElem);
    parser.createDouble("Horizontal",
                 ded.positionalAccuracy.absoluteAccuracyHorizontal,
                 absAccElem);
    parser.createDouble("Vertical",
                 ded.positionalAccuracy.absoluteAccuracyVertical,
                 absAccElem);

    auto& p2pAccElem = parser.newElement("PointToPointAccuracy", posAccElem);
    parser.createDouble("Horizontal",
                 ded.positionalAccuracy.pointToPointAccuracyHorizontal,
                 p2pAccElem);
    parser.createDouble("Vertical",
                 ded.positionalAccuracy.pointToPointAccuracyVertical,
                 p2pAccElem);

    if (six::Init::isDefined(ded.nullValue))
    {
        parser.createInt("NullValue", ded.nullValue, dedElem);
    }

    return dedElem;
}

void DerivedXMLParser200::parseGeoDataFromXML(
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


void DerivedXMLParser200::parseMeasurementFromXML(
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

void DerivedXMLParser200::parseExploitationFeaturesFromXML(
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

void DerivedXMLParser200::parseProductFromXML(
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
        const xml::lite::Element* const productElem = productElems[ii];
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

void DerivedXMLParser200::parseDigitalElevationDataFromXML(
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

std::unique_ptr<LUT> DerivedXMLParser200::parseSingleLUT(const std::string& lutStr, size_t size)
{
    const auto lutVals = str::split(lutStr, " ");
    auto lut = std::make_unique<LUT>(size, sizeof(short));

    for (size_t ii = 0; ii < lutVals.size(); ++ii)
    {
        const short lutVal = str::toType<short>(lutVals[ii]);
        ::memcpy(&(lut->table[ii * lut->elementSize]),
            &lutVal, sizeof(short));
    }
    return lut;
}
mem::auto_ptr<LUT> DerivedXMLParser200::parseSingleLUT(const xml::lite::Element* elem,
        size_t size) const
{
    std::string lutStr = "";
    parseString(elem, lutStr);
    auto result = parseSingleLUT(lutStr, size);
    return mem::auto_ptr<LUT>(result.release());
}

XMLElem DerivedXMLParser200::createLUT(const std::string& name, const LUT *lut,
        XMLElem parent) const
{
    assert(lut != nullptr);
    assert(parent != nullptr);
    return &createLUT(*this, name, *lut, *parent);
}
xml::lite::Element& DerivedXMLParser200::createLUT(const DerivedXMLParser& parser,
    const std::string& name, const LUT& lut, xml::lite::Element& parent)
{
    auto& lutElement = parser.newElement(name, parent);
    return * parser.createLUTImpl(&lut, &lutElement);
}
}
}
