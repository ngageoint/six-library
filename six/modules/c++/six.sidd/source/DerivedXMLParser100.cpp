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

#include <six/sidd/DerivedXMLParser100.h>

namespace
{
typedef xml::lite::Element* XMLElem;
typedef xml::lite::Attributes XMLAttributes;
}

namespace six
{
namespace sidd
{
DerivedXMLParser100::DerivedXMLParser100(logging::Logger* log,
                                         bool ownLog) :
    DerivedXMLParser("1.0.0", log, ownLog)
{
}

void DerivedXMLParser100::parseDerivedClassificationFromXML(
        const XMLElem classificationXML,
        DerivedClassification& classification) const
{
    // optional to unbounded
    common().parseParameters(classificationXML, "SecurityExtension",
                             classification.securityExtensions);

    const XMLAttributes& classificationAttributes
            = classificationXML->getAttributes();

    //! from ism:ISMRootNodeAttributeGroup
    // Could do
    // toType<sys::Int32_T>(
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
    XMLElem classXML = newElement("Classification", parent);

    common().addParameters("SecurityExtension",
                           classification.securityExtensions,
                           classXML);

    //! from ism:ISMRootNodeAttributeGroup
    // SIDD 1.0 is tied to IC-ISM v4
    setAttribute(classXML, "DESVersion", "4", ISM_URI);

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
}
}
