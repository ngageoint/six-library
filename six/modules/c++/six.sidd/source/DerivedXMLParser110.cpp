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
#include <six/sidd/DerivedXMLParser110.h>

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
    if (ptr.get() == NULL)
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
const char DerivedXMLParser110::VERSION[] = "1.1.0";
const char DerivedXMLParser110::SI_COMMON_URI[] = "urn:SICommon:1.0";

DerivedXMLParser110::DerivedXMLParser110(logging::Logger* log,
                                         bool ownLog) :
    DerivedXMLParser(VERSION,
                     std::auto_ptr<six::SICommonXMLParser>(
                         new six::SICommonXMLParser10x(versionToURI(VERSION),
                                                       false,
                                                       SI_COMMON_URI,
                                                       log)),
                     log,
                     ownLog)
{
}

DerivedData* DerivedXMLParser110::fromXML(
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
    XMLElem compressionXML            = getOptional(root, "Compression");


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
        for (unsigned int i = 0, size = annChildren.size(); i < size; ++i)
        {
            data->annotations[i].reset(new Annotation());
            parseAnnotationFromXML(annChildren[i], data->annotations[i].get());
        }
    }
    if (compressionXML)
    {
        builder.addCompression();
        parseCompressionFromXML(compressionXML, *(data->compression.get()));
    }

    return data;
}

xml::lite::Document* DerivedXMLParser110::toXML(const DerivedData* derived) const
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
    // optional
    if (derived->compression.get())
    {
       convertCompressionToXML(*(derived->compression.get()), root);
    }

    //set the XMLNS
    root->setNamespacePrefix("", getDefaultURI());
    root->setNamespacePrefix("si", SI_COMMON_URI);
    root->setNamespacePrefix("sfa", SFA_URI);
    root->setNamespacePrefix("ism", ISM_URI);

    return doc;
}

void DerivedXMLParser110::parseDerivedClassificationFromXML(
        const XMLElem classificationXML,
        DerivedClassification& classification) const
{
    throw except::Exception(Ctxt("IMPLEMENT ME"));
}

void DerivedXMLParser110::parseCompressionFromXML(const XMLElem compressionXML,
                                                 Compression& compression) const
{
    XMLElem j2kElem = getFirstAndOnly(compressionXML, "J2K");
    XMLElem originalElem = getFirstAndOnly(j2kElem, "Original");
    XMLElem parsedElem   = getOptional(j2kElem, "Parsed");

    parseJ2KCompression(originalElem, compression.original);
    if (parsedElem)
    {
        compression.parsed.reset(new J2KCompression());
        parseJ2KCompression(parsedElem, *(compression.parsed));
    }
}

void DerivedXMLParser110::parseJ2KCompression(const XMLElem j2kXML,
                                           J2KCompression& j2k) const
{
    parseInt(getFirstAndOnly(j2kXML, "NumWaveletLevels"),
            j2k.numWaveletLevels);
    parseInt(getFirstAndOnly(j2kXML, "NumBands"),
            j2k.numBands);

    XMLElem layerInfoXML = getFirstAndOnly(j2kXML, "LayerInfo");
    std::vector<XMLElem> layersXML;
    layerInfoXML->getElementsByTagName("Layer", layersXML);

    size_t numLayers = layersXML.size();
    j2k.layerInfo.resize(numLayers);

    for (size_t ii = 0; ii != layersXML.size(); ++ii)
    {
        double bitRate;
        parseDouble(getFirstAndOnly(layersXML[ii], "Bitrate"),
                    bitRate);
        j2k.layerInfo[ii].bitRate = bitRate;
    }
}

XMLElem DerivedXMLParser110::convertDerivedClassificationToXML(
        const DerivedClassification& classification,
        XMLElem parent) const
{
    XMLElem classXML = newElement("Classification", parent);

    common().addParameters("SecurityExtension",
                    classification.securityExtensions,
                           classXML);

    //! from ism:ISMRootNodeAttributeGroup
    // SIDD 1.1 is tied to IC-ISM v13
    setAttribute(classXML, "DESVersion", "13", ISM_URI);

    // So far as I can tell this should just be 1
    setAttribute(classXML, "ISMCATCESVersion", "1", ISM_URI);

    //! from ism:ResourceNodeAttributeGroup
    setAttribute(classXML, "resourceElement", "true", ISM_URI);
    setAttribute(classXML, "createDate",
                 classification.createDate.format("%Y-%m-%d"), ISM_URI);
    // required (was optional in SIDD 1.0)
    setAttributeList(classXML, "compliesWith", classification.compliesWith,
                     ISM_URI);

    // optional
    setAttributeIfNonEmpty(classXML,
                           "exemptFrom",
                           classification.exemptFrom,
                           ISM_URI);

    //! from ism:SecurityAttributesGroup
    //  -- referenced in ism::ResourceNodeAttributeGroup
    setAttribute(classXML, "classification", classification.classification,
                 ISM_URI);
    setAttributeList(classXML, "ownerProducer", classification.ownerProducer,
                     ISM_URI, true);

    // optional
    setAttributeIfNonEmpty(classXML, "joint", classification.joint, ISM_URI);

    // optional
    setAttributeList(classXML, "SCIcontrols", classification.sciControls,
                     ISM_URI);
    // optional
    setAttributeList(classXML, "SARIdentifier", classification.sarIdentifier,
                     ISM_URI);
    // optional
    setAttributeList(classXML,
                     "atomicEnergyMarkings",
                     classification.atomicEnergyMarkings,
                     ISM_URI);
    // optional
    setAttributeList(classXML,
                     "disseminationControls",
                     classification.disseminationControls,
                     ISM_URI);
    // optional
    setAttributeList(classXML,
                     "displayOnlyTo",
                     classification.displayOnlyTo,
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

    //! from ism:NoticeAttributesGroup
    // optional
    setAttributeIfNonEmpty(classXML,
                           "noticeType",
                           classification.noticeType,
                           ISM_URI);
    // optional
    setAttributeIfNonEmpty(classXML,
                           "noticeReason",
                           classification.noticeReason,
                           ISM_URI);
    // optional
    if (classification.noticeDate.get())
    {
        setAttributeIfNonEmpty(
                classXML, "noticeDate",
                classification.noticeDate->format("%Y-%m-%d"),
                ISM_URI);
    }
    // optional
    setAttributeIfNonEmpty(classXML,
                           "unregisteredNoticeType",
                           classification.unregisteredNoticeType,
                           ISM_URI);
    // optional
    setAttributeIfNonEmpty(classXML,
                           "externalNotice",
                           classification.externalNotice,
                           ISM_URI);

    return classXML;
}

XMLElem DerivedXMLParser110::convertNonInteractiveProcessingToXML(
        const NonInteractiveProcessing& processing,
        XMLElem parent) const
{
    XMLElem processingXML = newElement("NonInteractiveProcessing", parent);

    // ProductGenerationOptions
    XMLElem prodGenXML = newElement("ProductGenerationOptions",
                                    processingXML);

    const ProductGenerationOptions& prodGen =
            processing.productGenerationOptions;

    if (prodGen.bandEqualization.get())
    {
        const BandEqualization& bandEq = *prodGen.bandEqualization;

        XMLElem bandEqXML = newElement("BandEqualization", prodGenXML);
        createStringFromEnum("Algorithm", bandEq.algorithm, bandEqXML);
        createLUT("BandLUT", bandEq.bandLUT.get(), bandEqXML);
    }

    convertKernelToXML("ModularTransferFunctionRestoration",
                       prodGen.modularTransferFunctionRestoration,
                       prodGenXML);

    if (prodGen.dataRemapping.get())
    {
        XMLElem dataRemappingXML = newElement("DataRemapping", prodGenXML);
        convertRemapToXML(*prodGen.dataRemapping, dataRemappingXML);
    }

    if (prodGen.asymmetricPixelCorrection.get())
    {
        convertKernelToXML("AsymmetricPixelCorrection",
                           *prodGen.asymmetricPixelCorrection, prodGenXML);
    }

    // RRDS
    XMLElem rrdsXML = newElement("RRDS", processingXML);

    const RRDS& rrds = processing.rrds;
    createStringFromEnum("DownsamplingMethod", rrds.downsamplingMethod,
                         rrdsXML);

    if (rrds.downsamplingMethod == DownsamplingMethod::DECIMATE)
    {
        confirmNonNull(rrds.antiAlias, "antiAlias",
                       "for DECIMATE downsampling");
        convertKernelToXML("AntiAlias", *rrds.antiAlias, rrdsXML);

        confirmNonNull(rrds.interpolation, "interpolation",
                       "for DECIMATE downsampling");
        convertKernelToXML("Interpolation", *rrds.interpolation, rrdsXML);
    }

    return processingXML;
}

XMLElem DerivedXMLParser110::convertInteractiveProcessingToXML(
        const InteractiveProcessing& processing,
        XMLElem parent) const
{
    XMLElem processingXML = newElement("InteractiveProcessing", parent);

    // GeometricTransform
    const GeometricTransform& geoTransform(processing.geometricTransform);
    XMLElem geoTransformXML = newElement("GeometricTransform", processingXML);

    XMLElem scalingXML = newElement("Scaling", geoTransformXML);
    convertKernelToXML("AntiAlias", geoTransform.scaling.antiAlias,
                       scalingXML);
    convertKernelToXML("Interpolation", geoTransform.scaling.interpolation,
                       scalingXML);

    XMLElem orientationXML = newElement("Orientation", geoTransformXML);
    createStringFromEnum("OrientationType",
                         geoTransform.orientation.orientationType,
                         orientationXML);
    if (geoTransform.orientation.orientationType ==
                DerivedOrientationType::ANGLE)
    {
        createDouble("RotationAngle", geoTransform.orientation.rotationAngle,
                     orientationXML);
    }

    // SharpnessEnhancement
    const SharpnessEnhancement& sharpness(processing.sharpnessEnhancement);
    XMLElem sharpXML = newElement("SharpnessEnhancement", processingXML);

    bool ok = false;
    if (sharpness.modularTransferFunctionCompensation.get())
    {
        if (sharpness.modularTransferFunctionRestoration.get() == NULL)
        {
            ok = true;
            convertKernelToXML("ModularTransferFunctionCompensation",
                               *sharpness.modularTransferFunctionCompensation,
                               sharpXML);
        }
    }
    else if (sharpness.modularTransferFunctionRestoration.get())
    {
        ok = true;
        convertKernelToXML("ModularTransferFunctionRestoration",
                           *sharpness.modularTransferFunctionRestoration,
                           sharpXML);
    }

    if (!ok)
    {
        throw except::Exception(Ctxt(
                "Exactly one of modularTransferFunctionCompensation or "
                "modularTransferFunctionRestoration must be set"));
    }

    // ColorSpaceTransform
    if (processing.colorSpaceTransform.get())
    {
        const ColorManagementModule& cmm =
                processing.colorSpaceTransform->colorManagementModule;

        XMLElem colorSpaceTransformXML =
                newElement("ColorSpaceTransform", processingXML);
        XMLElem cmmXML =
                newElement("ColorManagementModule", colorSpaceTransformXML);

        createStringFromEnum("RenderingIntent", cmm.renderingIntent, cmmXML);

        // TODO: Not sure what this'll actually look like
        createString("SourceProfile", cmm.sourceProfile, cmmXML);
        createString("DisplayProfile", cmm.displayProfile, cmmXML);

        if (!cmm.iccProfile.empty())
        {
            createString("ICCProfile", cmm.iccProfile, cmmXML);
        }
    }

    // DynamicRangeAdjustment
    if (processing.dynamicRangeAdjustment.get())
    {
        const DynamicRangeAdjustment& adjust =
                *processing.dynamicRangeAdjustment;

        XMLElem adjustXML =
                newElement("DynamicRangeAdjustment", processingXML);

        createStringFromEnum("AlgorithmType", adjust.algorithmType,
                             adjustXML);

        createDouble("Pmin", adjust.pMin, adjustXML);
        createDouble("Pmax", adjust.pMax, adjustXML);

        XMLElem modXML = newElement("Modifiers", adjustXML);
        createDouble("Emin", adjust.modifiers.eMin, modXML);
        createDouble("Emax", adjust.modifiers.eMax, modXML);
        if (six::Init::isDefined(adjust.modifiers.subtractor))
        {
            createDouble("Subtractor", adjust.modifiers.subtractor, modXML);
            createDouble("Multiplier", adjust.modifiers.multiplier, modXML);
        }
    }

    if (processing.oneDimensionalLookup.get())
    {
        XMLElem lutXML = newElement("OneDimensionalLookup", processingXML);
        convertKernelToXML("TTC", processing.oneDimensionalLookup->ttc,
                           lutXML);
    }

    return processingXML;
}

XMLElem DerivedXMLParser110::convertPredefinedKernelToXML(
        const Kernel::Predefined& predefined,
        XMLElem parent) const
{
    XMLElem predefinedXML = newElement("Predefined", parent);

    // Make sure either DBName or KernelFamily+KernelMember are defined
    bool ok = false;
    if (isDefined(predefined.dbName))
    {
        if (six::Init::isUndefined(predefined.kernelFamily) &&
            six::Init::isUndefined(predefined.kernelMember))
        {
            ok = true;

            createStringFromEnum("DBName", predefined.dbName, predefinedXML);
        }
    }
    else if (six::Init::isDefined(predefined.kernelFamily) &&
             six::Init::isDefined(predefined.kernelMember))
    {
        ok = true;

        createInt("KernelFamily", predefined.kernelFamily, predefinedXML);
        createInt("KernelMember", predefined.kernelMember, predefinedXML);
    }

    if (!ok)
    {
        throw except::Exception(Ctxt(
                "Exactly one of either dbName or kernelFamily and "
                "kernelMember must be defined"));
    }

    return predefinedXML;
}

XMLElem DerivedXMLParser110::convertCustomKernelToXML(
        const Kernel::Custom& custom,
        XMLElem parent) const
{
    XMLElem customXML = newElement("Custom", parent);

    XMLElem kernelSize = newElement("KernelSize", customXML);
    createInt("Row", custom.kernelSize.row, kernelSize);
    createInt("Col", custom.kernelSize.col, kernelSize);

    if (custom.kernelCoef.size() !=
        static_cast<size_t>(custom.kernelSize.row) * custom.kernelSize.col)
    {
        std::ostringstream ostr;
        ostr << "Kernel size is " << custom.kernelSize.row << " rows x "
             << custom.kernelSize.col << " cols but have "
             << custom.kernelCoef.size() << " coefficients";
        throw except::Exception(Ctxt(ostr.str()));
    }

    XMLElem kernelCoef = newElement("KernelCoef", customXML);
    for (sys::SSize_T row = 0, idx = 0; row < custom.kernelSize.row; ++row)
    {
        for (sys::SSize_T col = 0; col < custom.kernelSize.col; ++col, ++idx)
        {
            XMLElem coefXML = createDouble("Coef", custom.kernelCoef[idx],
                                           kernelCoef);
            setAttribute(coefXML, "row", str::toString(row));
            setAttribute(coefXML, "col", str::toString(col));
        }
    }

    return customXML;
}

XMLElem DerivedXMLParser110::convertKernelToXML(const std::string& name,
                                                const Kernel& kernel,
                                                XMLElem parent) const
{
    XMLElem kernelXML = newElement(name, parent);

    createString("KernelName", kernel.kernelName, kernelXML);

    // Exactly one of Predefined or Custom should be set
    bool ok = false;
    if (kernel.predefined.get())
    {
        if (kernel.custom.get() == NULL)
        {
            ok = true;
            convertPredefinedKernelToXML(*kernel.predefined, kernelXML);
        }
    }
    else if (kernel.custom.get())
    {
        ok = true;
        convertCustomKernelToXML(*kernel.custom, kernelXML);
    }

    if (!ok)
    {
        throw except::Exception(Ctxt(
                "Exactly one of predefined or custom must be set"));
    }

    createStringFromEnum("Operation", kernel.operation, kernelXML);

    return kernelXML;
}

XMLElem DerivedXMLParser110::convertCompressionToXML(
        const Compression& compression,
        XMLElem parent) const
{
    XMLElem compressionXML = newElement("Compression", parent);
    XMLElem j2kXML         = newElement("J2K", compressionXML);
    XMLElem originalXML    = newElement("Original", j2kXML);
    convertJ2KToXML(compression.original, originalXML);

    if (compression.parsed.get())
    {
        XMLElem parsedXML = newElement("Parsed", j2kXML);
        convertJ2KToXML(*(compression.parsed.get()), parsedXML);
    }
    return compressionXML;
}

void DerivedXMLParser110::convertJ2KToXML(const J2KCompression& j2k,
                                       XMLElem& parent) const
{
    createInt("NumWaveletLevels", j2k.numWaveletLevels, parent);
    createInt("NumBands", j2k.numBands, parent);

    size_t numLayers = j2k.layerInfo.size();
    XMLElem layerInfoXML = newElement("LayerInfo", parent);
    setAttribute(layerInfoXML, "numLayers", toString(numLayers));

    for (size_t ii = 0; ii < numLayers; ++ii)
    {
        XMLElem layerXML = newElement("Layer", layerInfoXML);
        setAttribute(layerXML, "index", toString(ii));
        createDouble("Bitrate", j2k.layerInfo[ii].bitRate, layerXML);
    }
}

XMLElem DerivedXMLParser110::convertDisplayToXML(
        const Display& display,
        XMLElem parent) const
{
    // NOTE: In several spots here, there are fields which are required in
    //       SIDD 1.1 but a pointer in the Display class since it didn't exist
    //       in SIDD 1.0, so need to confirm it's allocated

    XMLElem displayXML = newElement("Display", parent);

    createString("PixelType", six::toString(display.pixelType), displayXML);

    // BandInformation
    XMLElem bandInfoXML = newElement("BandInformation", displayXML);
    confirmNonNull(display.bandInformation, "bandInformation");
    createInt("NumBands", display.bandInformation->bands.size());
    for (size_t ii = 0; ii < display.bandInformation->bands.size(); ++ii)
    {
        XMLElem bandXML = createString("Band",
                                       display.bandInformation->bands[ii],
                                       bandInfoXML);
        setAttribute(bandXML, "index", str::toString(ii));
    }
    createInt("BitsPerPixel", display.bandInformation->bitsPerPixel,
              bandInfoXML);
    createInt("DisplayFlag", display.bandInformation->displayFlag,
              bandInfoXML);

    // NonInteractiveProcessing
    confirmNonNull(display.nonInteractiveProcessing,
                   "nonInteractiveProcessing");
    convertNonInteractiveProcessingToXML(*display.nonInteractiveProcessing,
                                         displayXML);

    // InteractiveProcessing
    confirmNonNull(display.interactiveProcessing, "interactiveProcessing");
    convertInteractiveProcessingToXML(*display.interactiveProcessing,
                                      displayXML);

    // optional to unbounded
    common().addParameters("DisplayExtension", display.displayExtensions,
                           displayXML);

    return displayXML;
}

XMLElem DerivedXMLParser110::convertGeographicTargetToXML(
        const GeographicAndTarget& geographicAndTarget,
        XMLElem parent) const
{
    XMLElem geographicAndTargetXML = newElement("GeographicAndTarget", parent);

    common().createLatLonFootprint("ImageCorners", "ICP",
                                   geographicAndTarget.imageCorners,
                                   geographicAndTargetXML);

    //only if 3+ vertices
    const size_t numVertices = geographicAndTarget.validData.size();
    if (numVertices >= 3)
    {
        XMLElem vXML = newElement("ValidData", geographicAndTargetXML);
        setAttribute(vXML, "size", str::toString(numVertices));

        for (size_t ii = 0; ii < numVertices; ++ii)
        {
            XMLElem vertexXML =
                    common().createLatLon("Vertex",
                                          geographicAndTarget.validData[ii],
                                          vXML);
            setAttribute(vertexXML, "index", str::toString(ii + 1));
        }
    }

    for (size_t ii = 0; ii < geographicAndTarget.geoInfos.size(); ++ii)
    {
        common().convertGeoInfoToXML(*geographicAndTarget.geoInfos[ii],
                                     geographicAndTargetXML);
    }

    return geographicAndTargetXML;
}
}
}
