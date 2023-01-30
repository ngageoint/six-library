/* =========================================================================
 * This file is part of six.sidd30-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
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
#ifndef SIX_six_sicd30_DerivedClassification_h_INCLUDED_
#define SIX_six_sicd30_DerivedClassification_h_INCLUDED_
#pragma once

#include <std/optional>

#include <mem/ScopedCopyablePtr.h>

#include "six/Classification.h"
#include "six/ParameterCollection.h"

namespace six
{
namespace sidd30
{
/*!
 *  \class DerivedClassification
 *  \brief The implementation of Classification for derived products
 *
 *  SIDD 1.0 uses IC ISM v4
 *  SIDD 2.0 uses IC ISM v13
 *
 *  Compiler-generated copy constructor and assignment operator are sufficient
 */
struct DerivedClassification: public Classification
{
    std::string getLevel() const override
    {
        return classification;
    }

    virtual void setSecurity(const std::string& prefix,
                             logging::Logger& log,
                             nitf::FileSecurity security) const;

    virtual std::ostream& put(std::ostream& os) const;

    //! Extensible parameters used to support profile-specific needs related
    //  to product security.
    ParameterCollection         securityExtensions;

    //! The version number of the DES. Should there be multiple specified in
    //  an instance document the one at the root node is the one that will
    //  apply to the entire document.
    int32_t                     desVersion = 0;

    //! Used to designate what date the document was produced on. This is the
    //  date that will be used by various constraint rules to determine if the
    //  document meets all the business rules.
    DateTime                         createDate;

    //! An indicator of what ISM rule sets the documents complies with. This
    //  allows systems to know that the document claims compliance with these
    //  rule sets and they should be enforced.
    //  This is optional in SIDD 1.0 but required in SIDD 2.0
    std::vector<std::string>         compliesWith;

    // 'classification' and 'ownerProducer' are required
    // The rest are all optional

    //! A single indicator of the highest level of classification applicable
    //  to an information resource or portion within the domain of classified
    //  national security information.
    //  VALID VALUES: 'R', 'C', 'S', 'TS', or 'U'
    std::string                      classification;

    //! One or more indicators identifying the national government or
    //  international organization that have purview over the classification
    //  marking of an information resource or portion therein.
    std::vector<std::string>         ownerProducer;

    //! One or more indicators identifying sensitive compartmented information
    //  control system(s).
    std::vector<std::string>         sciControls;

    //! One or more indicators identifying the defense or intelligence
    //  programs for which special access is required.
    std::vector<std::string>         sarIdentifier;

    //! One or more indicators identifying the expansion or limitation on the
    //  distribution of information.
    std::vector<std::string>         disseminationControls;

    //! One or more indicators identifying information which qualifies as
    //  foreign government information for which the source(s) of the
    //  information is not concealed.
    //  The attribute can indicate that the source of information of foreign
    //  origin is unknown.
    std::vector<std::string>         fgiSourceOpen;

    //! A single indicator that information qualifies as foreign government
    //  information for which the source(s) of the information must be
    //  concealed.
    std::vector<std::string>         fgiSourceProtected;

    //! One or more indicators identifying the country or countries and/or
    //  international organization(s) to which classified information may be
    //  released based on the determination of an originator in accordance
    //  with established foreign disclosure procedures.
    std::vector<std::string>         releasableTo;

    //! One or more indicators of the expansion or limitation on the
    //  distribution of an information resource or portion within the domain
    //  of information originating from non-intelligence components.
    std::vector<std::string>         nonICMarkings;

    //! The identity, by name or personal identifier, and position title of
    // the original classification authority for a resource.
    std::string                      classifiedBy;

    //! A description of the reasons that the classification of this element
    //  is more restrictive than a simple roll-up of the sub elements would
    //  result in.
    std::string                      compilationReason;

    //! The identity, by name or personal identifier, of the derivative
    //  classification authority.
    std::string                      derivativelyClassifiedBy;

    //! One or more reason indicators or explanatory text describing the basis
    //  for an original classification decision.
    std::string                      classificationReason;

    //! One or more indicators of the expansion or limitation on the
    //  distribution of an information resource or portion within the domain
    //  of information originating from non-US components.
    std::vector<std::string>         nonUSControls;

    //! A citation of the authoritative source or reference to multiple
    //  sources of the classification markings used in a classified resource.
    std::string                      derivedFrom;

    //! A specific year, month, and day upon which the information shall be
    //  automatically declassified if not properly exempted from automatic
    //  declassification.
    mem::ScopedCopyablePtr<DateTime> declassDate;

    //! A description of an event upon which the information shall be
    //  automatically declassified if not properly exempted from automatic
    //  declassification.
    std::string                      declassEvent;

    //! A single indicator describing an exemption to the nominal 25-year
    //  point for automatic declassification.  This element is used in
    //  conjunction with the declassDate or declassEvent.
    std::string                      declassException;

    //! A declassification marking of a source document that causes the
    //  current, derivative document to be exempted from automatic
    //  declassification.  This is always used in conjunction with the
    //  exemptedSourceDate.
    //  This only exists in SIDD 1.0
    std::string                      exemptedSourceType;

    //! A specific year, month, and day of publication or release of a source
    //  document, or the most recent source document, that was itself marked
    //  with a declassification constraint.  This is always used in
    // conjunction with the exemptedSourceType.
    //  This only exists in SIDD 1.0
    mem::ScopedCopyablePtr<DateTime> exemptedSourceDate;

    // The following are new in SIDD 2.0 (IC ISM v13)

    //! This attribute is used to declare specific exemptions within a rule
    //  set - for example exemption from ICD 710 FD&R requirements. This
    //  attribute is used on the resource node of a document in conjunction
    //  with compliesWith.
    std::string exemptFrom;

    //! This attribute, when true, is used to signify that multiple values in
    //  the ownerProducer attribute are JOINT owners of the data.
    BooleanType joint;

    //! This attribute is used at both the resource and the portion levels.
    //  One or more indicators identifying DoE markings. It is manifested in
    //  portion marks and security banners.
    std::vector<std::string> atomicEnergyMarkings;

    //! This attribute is used at both the resource and the portion levels.
    //  One or more indicators identifying the country or countries and/or
    //  international organization(s) to which classified information may be
    //  displayed but NOT released based on the determination of an originator
    //  in accordance with established foreign disclosure procedures. This
    // element is used in conjunction with the DisplayOnly Dissemination
    // Controls value. It is manifested in portion marks and security banners.
    std::vector<std::string> displayOnlyTo;

    //! A single Notice that may consist of 1 or more NoticeText
    std::string noticeType;

    //! A Reason (less than 2048 chars) associated with a notice such as the
    //  DoD Distribution reason.
    std::string noticeReason;

    //! A Date associated with a notice such as the DoD Distribution notice
    //  date.
    mem::ScopedCopyablePtr<DateTime> noticeDate;

    //! A notice that is of a category that is not described in the CAPCO
    //  Register and/or is not sufficiently defined to be represented in the
    //  Controlled Value Enumeration CVEnumISMNotice.xml. This attribute can
    //  be used by specifications that import ISM to represent a wider variety
    //  of security-related notices.
    std::string unregisteredNoticeType;

    //! This attribute is an indicator that the element contains a
    //  security-related notice NOT in this document. This flag allows for a
    //  notice to exist in a document without the data that would normally
    //  require the notice. Example: a FISA notice when there is no FISA data
    //  present.  A common use case is source citations where the notice if
    //  for the sourced document and should not impact the requirements for
    //  that type of data in this document.
    BooleanType externalNotice;

    bool operator==(const DerivedClassification& rhs) const // need member-function for SWIG
    {
        return static_cast<const Classification&>(*this) == static_cast<const Classification&>(rhs);
    }
private:
    //! Equality operator
    bool operator_eq(const DerivedClassification& rhs) const;
    bool equalTo(const Classification& rhs) const override;

    static
    void putImpl(const std::string& name,
                 const std::vector<std::string>& strs,
                 std::ostream& os);

    static
    void putImpl(const std::string& name,
                 const ParameterCollection& params,
                 std::ostream& os);

    static
    void putImpl(const DateTime& date, std::ostream& os);

    static
    void putImpl(const std::string& name,
                 const DateTime* date,
                 std::ostream& os);
};
}
}
#endif // SIX_six_sicd30_DerivedClassification_h_INCLUDED_
