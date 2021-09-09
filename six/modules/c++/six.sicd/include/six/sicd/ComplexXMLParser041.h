/* =========================================================================
 * This file is part of six.sicd-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
 *
 * six.sicd-c++ is free software; you can redistribute it and/or modify
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
#ifndef __SIX_SICD_COMPLEX_XML_PARSER_041_H__
#define __SIX_SICD_COMPLEX_XML_PARSER_041_H__

#include <six/sicd/ComplexXMLParser04x.h>

namespace six
{
namespace sicd
{
class ComplexXMLParser041 : public ComplexXMLParser04x
{
public:
    ComplexXMLParser041(const std::string& version,
                        logging::Logger* log = nullptr,
                        bool ownLog = false);

protected:
    ComplexXMLParser041(const std::string& version,
                        bool addClassAttributes,
                        std::unique_ptr<SICommonXMLParser>&& comParser,
                        logging::Logger* log = nullptr,
                        bool ownLog = false);
#if !CODA_OSS_cpp17
    ComplexXMLParser041(const std::string& version,
                        bool addClassAttributes,
                        mem::auto_ptr<SICommonXMLParser> comParser,
                        logging::Logger* log = nullptr,
                        bool ownLog = false);
#endif

    ComplexXMLParser041(const ComplexXMLParser041&) = delete;
    ComplexXMLParser041& operator=(const ComplexXMLParser041&) = delete;

    virtual XMLElem convertRMATToXML(const RMAT* obj, 
                                     XMLElem parent = nullptr) const;
    virtual void parseRMATFromXML(const xml::lite::Element* rmatElem, RMAT* obj) const;

    virtual XMLElem convertImageFormationAlgoToXML(
            const PFA* pfa, const RMA* rma, 
            const RgAzComp* rgAzComp, 
            XMLElem parent = nullptr) const;
};
}
}

#endif

