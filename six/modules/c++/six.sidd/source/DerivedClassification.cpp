/* =========================================================================
 * This file is part of six.sidd-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
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
#include "six/NITFImageInfo.h"
#include "six/sidd/DerivedClassification.h"

namespace six
{
namespace sidd
{
void DerivedClassification::setSecurity(const std::string& prefix,
                                        logging::Logger& log,
                                        nitf::FileSecurity security) const
{
    if (classification != "U")
    {
        if (!releasableTo.empty())
        {
            std::string releasableToStr;
            for (size_t ii = 0; ii < releasableTo.size(); ++ii)
            {
                if (ii > 0)
                {
                    releasableToStr += " ";
                }
                releasableToStr += releasableTo[ii];
            }

            security.getReleasingInstructions().set(releasableToStr);

            const std::string fieldKey =
                NITFImageInfo::generateFieldKey(NITFImageInfo::REL, prefix);
            log.debug(Ctxt(FmtX("Setting NITF [%s] from sicd/sidd: [%s]", fieldKey, releasableToStr)));
        }

        if (!classifiedBy.empty())
        {
            security.getClassificationAuthority().set(classifiedBy);

            std::string fieldKey =
                NITFImageInfo::generateFieldKey(NITFImageInfo::CAUT, prefix);
            log.debug(Ctxt(FmtX("Setting NITF [%s] from sicd/sidd: [%s]", fieldKey, classifiedBy)));

            // classifiedBy attribute represents the name of the original
            // classification authority
            const std::string classAuthorityType("O");
            security.getClassificationAuthorityType().set(classAuthorityType);

            fieldKey =
                NITFImageInfo::generateFieldKey(NITFImageInfo::CATP, prefix);
            log.debug(Ctxt(FmtX("Setting NITF [%s] from sicd/sidd: [%s]", fieldKey, classAuthorityType)));
        }

        if (!classificationReason.empty())
        {
            security.getClassificationReason().set(classificationReason);

            const std::string fieldKey =
                NITFImageInfo::generateFieldKey(NITFImageInfo::CRSN, prefix);
            log.debug(Ctxt(FmtX("Setting NITF [%s] from sicd/sidd: [%s]", fieldKey, classificationReason)));
        }

        // By default, mark as exempt from automatic declassification
        std::string declassType("X");

        if (declassDate.get() != nullptr)
        {
            std::string declassDateStr;
            declassDate->format("%Y%m%d", declassDateStr); // note: timezone is explicitly excluded in ISM-201609
            str::trim(declassDateStr);
            security.getDeclassificationDate().set(declassDateStr);

            const std::string fieldKey =
                NITFImageInfo::generateFieldKey(NITFImageInfo::DCDT, prefix);
            log.debug(Ctxt(FmtX("Setting NITF [%s] from sicd/sidd: [%s]", fieldKey, declassDateStr)));

            // Declassify on a specific date
            declassType = "DD";
        }

        if (!declassEvent.empty())
        {
            security.getClassificationText().set(declassEvent);

            const std::string fieldKey =
                NITFImageInfo::generateFieldKey(NITFImageInfo::CLTX, prefix);
            log.debug(Ctxt(FmtX("Setting NITF [%s] from sicd/sidd: [%s]", fieldKey, declassEvent)));

            // Declassify upon occurrence of an event
            declassType = "DE";
        }

        if (!exemptedSourceType.empty())
        {
            // Attributes are 'OADR' or 'X[1-8]'
            // TODO  Should the NITF field get set to something if this is set
            // to 'OADR'?
            if (exemptedSourceType.length() == 2 &&
                exemptedSourceType[0] == 'X' &&
                exemptedSourceType[1] >= '1' &&
                exemptedSourceType[1] <= '8')
            {
                security.getDeclassificationExemption().set(
                    exemptedSourceType);

                const std::string fieldKey =
                    NITFImageInfo::generateFieldKey(NITFImageInfo::DCXM, prefix);
                log.debug(Ctxt(FmtX("Setting NITF [%s] from sicd/sidd: [%s]", fieldKey, exemptedSourceType)));

                // Exempt from automatic declassification
                declassType = "X";
            }
        }

        if (!declassException.empty())
        {
            // declassException attributes are named
            // '25X[1-9]' or '25X1-human'
            // Corresponding NITF Declassification Exemption field values are
            // named 'X25[1-9]'
            std::string declassExemption;
            if (declassException == "25X1-human")
            {
                declassExemption = "X251";
            }
            else if (declassException.length() == 4 &&
                     declassException[0] == '2' &&
                     declassException[1] == '5' &&
                     declassException[2] == 'X')
            {
                const char ch(declassException[3]);
                if (ch >= '1' && ch <= '9')
                {
                    declassExemption = "X25" + ch;
                }
            }

            if (!declassExemption.empty())
            {
                security.getDeclassificationExemption().set(declassExemption);

                const std::string fieldKey =
                    NITFImageInfo::generateFieldKey(NITFImageInfo::DCXM, prefix);
                log.debug(Ctxt(FmtX("Setting NITF [%s] from sicd/sidd: [%s]", fieldKey, declassExemption)));

                // Exempt from automatic declassification
                declassType = "X";
            }
        }

        // Now that we've gone through all the things that could modify the
        // declassification type, set it
        security.getDeclassificationType().set(declassType);
        log.debug(Ctxt(FmtX("Setting NITF [%s] from sicd/sidd: [%s]",
            NITFImageInfo::generateFieldKey(NITFImageInfo::DCTP, prefix), declassType)));
    }
}

void DerivedClassification::putImpl(const std::string& name,
                                    const std::vector<std::string>& strs,
                                    std::ostream& os)
{
    os << name << ":\n";
    for (size_t ii = 0; ii < strs.size(); ++ii)
    {
        os << "    [" << strs[ii] << "]\n";
    }
}

void DerivedClassification::putImpl(const std::string& name,
                                    const ParameterCollection& params,
                                    std::ostream& os)
{
    os << name << ":\n";
    for (size_t ii = 0; ii < params.size(); ++ii)
    {
        os << "    [" << params[ii].getName() << " = "
           << params[ii].str() << "]\n";
    }
}

void DerivedClassification::putImpl(const std::string& name,
                                    const DateTime* date,
                                    std::ostream& os)
{
    os << name << ":";
    if (date)
    {
        os << " " << date->format("%Y%m%d");
    }
    os << "\n";
}

std::ostream& DerivedClassification::put(std::ostream& os) const
{
    putImpl("Security Extensions", securityExtensions, os);

    putImpl("Create Date", &createDate, os);

    putImpl("Complies With", compliesWith, os);

    os << "Level: " << classification << "\n";
    putImpl("Owner/producer", ownerProducer, os);
    putImpl("SCI Controls", sciControls, os);
    putImpl("SAR Identifier", sarIdentifier, os);
    putImpl("Dissemination Controls", disseminationControls, os);
    putImpl("FGI Source Open", fgiSourceOpen, os);
    putImpl("FGI Source Protected", fgiSourceProtected, os);
    putImpl("Releasable To", releasableTo, os);
    putImpl("Non IC Markings", nonICMarkings, os);

    os << "Classified By: " << classifiedBy
       << "\nCompilation Reason: " << compilationReason
       << "\nDerivatively Classified By: " << derivativelyClassifiedBy
       << "\nClassification Reason: " << classificationReason << "\n";

    putImpl("Non US Controls", nonUSControls, os);

    os << "Derived From: " << derivedFrom << "\n";

    putImpl("Declass Date", declassDate.get(), os);

    os << "Declass Event: " << declassEvent
       << "\nDeclass Exception: " << declassException
       << "\nExempted Source Type: " << exemptedSourceType << "\n";

    putImpl("Exempted Source Date", exemptedSourceDate.get(), os);

    os << "Exempt From: " << exemptFrom
       << "Joint: " << joint << "\n";

    putImpl("Display Only To", displayOnlyTo, os);

    os << "Notice: " << noticeType
       << "\nNotice Reason: " << noticeReason << "\n";

    putImpl("Notice Date", noticeDate.get(), os);

    os << "Unregistered Notice: " << unregisteredNoticeType
       << "\nExternal Notice: " << externalNotice << "\n";

    return os;
}

bool DerivedClassification::operator_eq(const DerivedClassification& rhs) const
{
    return (securityExtensions == rhs.securityExtensions &&
        desVersion == rhs.desVersion &&
        createDate == rhs.createDate &&
        compliesWith == rhs.compliesWith &&
        classification == rhs.classification &&
        ownerProducer == rhs.ownerProducer &&
        sciControls == rhs.sciControls &&
        sarIdentifier == rhs.sarIdentifier &&
        disseminationControls == rhs.disseminationControls &&
        fgiSourceOpen == rhs.fgiSourceOpen &&
        fgiSourceProtected == rhs.fgiSourceProtected &&
        releasableTo == rhs.releasableTo &&
        nonICMarkings == rhs.nonICMarkings &&
        classifiedBy == rhs.classifiedBy &&
        compilationReason == rhs.compilationReason &&
        derivativelyClassifiedBy == rhs.derivativelyClassifiedBy &&
        classificationReason == rhs.classificationReason &&
        nonUSControls == rhs.nonUSControls &&
        derivedFrom == rhs.derivedFrom &&
        declassDate == rhs.declassDate &&
        declassEvent == rhs.declassEvent &&
        declassException == rhs.declassException &&
        exemptedSourceType == rhs.exemptedSourceType &&
        exemptedSourceDate == rhs.exemptedSourceDate &&
        fileOptions == rhs.fileOptions);
}

bool DerivedClassification::equalTo(const Classification& rhs) const
{
    auto derivedClassification = dynamic_cast<const DerivedClassification*>(&rhs);
    if (derivedClassification != nullptr)
    {
        return this->operator_eq(*derivedClassification);
    }
    return false;
}

}
}
