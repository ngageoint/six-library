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
}

namespace six
{
namespace sidd
{
DerivedXMLParser110::DerivedXMLParser110(logging::Logger* log,
                                         bool ownLog) :
    DerivedXMLParser("1.1.0", log, ownLog)
{
}

xml::lite::Document*
DerivedXMLParser110::toXML(const DerivedData* derived) const
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

void DerivedXMLParser110::parseDerivedClassificationFromXML(
        const XMLElem classificationXML,
        DerivedClassification& classification) const
{
    throw except::Exception(Ctxt("IMPLEMENT ME"));
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
        if (rrds.antiAlias.get() == NULL)
        {
            throw except::Exception(Ctxt(
                    "antiAlias must be populated for DECIMATE downsampling"));
        }
        convertKernelToXML("AntiAlias", *rrds.antiAlias, rrdsXML);

        if (rrds.interpolation.get() == NULL)
        {
            throw except::Exception(Ctxt(
                    "interpoliation must be populated for DECIMATE downsampling"));
        }
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
}
}
