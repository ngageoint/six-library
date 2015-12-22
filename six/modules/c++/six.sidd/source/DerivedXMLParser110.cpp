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

#include <six/sidd/DerivedXMLParser110.h>

namespace
{
typedef xml::lite::Element* XMLElem;
typedef xml::lite::Attributes XMLAttributes;
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
    XMLElem display = convertDisplayToXML(derived->display.get(), root);

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



    return processingXML;
}

XMLElem DerivedXMLParser110::convertKernelToXML(const Kernel& kernel,
                                             XMLElem parent) const
{
    XMLElem kernelXML = newElement("Kernel", parent);

    createString("KernelName", kernel.kernelName, kernelXML);

    createStringFromEnum("Operation", kernel.operation, kernelXML);

    return kernelXML;
}
}
}
