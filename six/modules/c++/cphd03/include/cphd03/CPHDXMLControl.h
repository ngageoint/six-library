/* =========================================================================
 * This file is part of cphd03-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
 *
 * cphd03-c++ is free software; you can redistribute it and/or modify
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


#ifndef __CPHD03_CPHD03_XML_CONTROL_H__
#define __CPHD03_CPHD03_XML_CONTROL_H__

#include <memory>

#include <logging/Logger.h>
#include <xml/lite/Element.h>
#include <xml/lite/Document.h>
#include <six/XMLParser.h>
#include <six/SICommonXMLParser10x.h>
#include <cphd03/Metadata.h>

namespace cphd03
{
/*!
 *  This class converts a Metadata object into a CPHD XML
 *  Document Object Model (DOM).
 */
class CPHDXMLControl : public six::XMLParser
{
public:
    //!  Constructor
    CPHDXMLControl();

    CPHDXMLControl(logging::Logger* log, bool ownLog = false);

    /*!
     *  This function takes in a Metadata object and converts
     *  it to a new-allocated XML DOM.
     */
    mem::auto_ptr<xml::lite::Document> toXML(const Metadata& metadata);

    /*!
     *  Function takes a DOM Document* node and creates a new-allocated
     *  CPHDData* populated by the DOM.
     */
    mem::auto_ptr<Metadata> fromXML(const xml::lite::Document* doc);
   Metadata fromXML(const xml::lite::Document& doc);

    mem::auto_ptr<Metadata> fromXML(const std::string& xmlString);

    std::u8string toXMLString(const Metadata& metadata);
    size_t getXMLsize(const Metadata& metadata);

private:
    typedef xml::lite::Element* XMLElem;

    // TODO: These are copies from some of the six.sicd XMLParsers
    XMLElem createLatLonAltFootprint(const std::string& name,
                                     const std::string& cornerName,
                                     const cphd::LatLonAltCorners& corners,
                                     XMLElem parent = nullptr) const;

private:
    static const char CPHD03_URI[];

    //! Returns the default URI
    std::string getDefaultURI() const;

    //! Returns the URI to use with SI Common types
    std::string getSICommonURI() const;

    // Write functions
    XMLElem toXML(const Data& obj, XMLElem parent = nullptr);
    XMLElem toXML(const Global& obj, XMLElem parent = nullptr);
    XMLElem toXML(const Channel& obj, XMLElem parent = nullptr);
    XMLElem toXML(const SRP& obj, XMLElem parent = nullptr);
    XMLElem toXML(const Antenna& obj, XMLElem parent = nullptr);
    XMLElem toXML(const std::string& name, const AntennaParameters &ap,
            XMLElem parent = nullptr);
    XMLElem toXML(const VectorParameters& obj, XMLElem parent = nullptr);

    XMLElem areaLineDirectionParametersToXML(const std::string& name,
            const AreaDirectionParameters& obj,
            XMLElem parent = nullptr);
    XMLElem areaSampleDirectionParametersToXML(const std::string& name,
            const AreaDirectionParameters& obj,
            XMLElem parent = nullptr);

    // Read functions
    void fromXML(const xml::lite::Element* dataXML, Data& obj);
    void fromXML(const xml::lite::Element* globalXML, Global& obj);
    void fromXML(const xml::lite::Element* channelXML, Channel& obj);
    void fromXML(const xml::lite::Element* srpXML, SRP& obj);
    void fromXML(const xml::lite::Element* antennaXML, Antenna& obj);
    void fromXML(const xml::lite::Element* antennaParamsXML, AntennaParameters& params);
    void fromXML(const xml::lite::Element* vectorParametersXML, VectorParameters& obj);

private:
    six::SICommonXMLParser10x mCommon;
};
}

#endif
