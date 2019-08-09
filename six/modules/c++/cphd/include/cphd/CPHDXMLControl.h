/* =========================================================================
 * This file is part of cphd-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2018, MDA Information Systems LLC
 *
 * cphd-c++ is free software; you can redistribute it and/or modify
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


#ifndef __CPHD_CPHD_XML_CONTROL_H__
#define __CPHD_CPHD_XML_CONTROL_H__

#include <memory>

#include <mem/SharedPtr.h>
#include <logging/Logger.h>
#include <xml/lite/Element.h>
#include <xml/lite/Document.h>
#include <six/XMLParser.h>
#include <six/SICommonXMLParser10x.h>
#include <cphd/CollectionID.h>
#include <cphd/SceneCoordinates.h>
#include <cphd/Data.h>
#include <cphd/Global.h>
#include <cphd/Metadata.h>
#include <cphd/PVP.h>

namespace cphd
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

    //! Constructor
    CPHDXMLControl(logging::Logger* log, bool ownLog);

    std::auto_ptr<Metadata> fromXML(const xml::lite::Document* doc);

    std::auto_ptr<Metadata> fromXML(const std::string& xmlString);

    // size_t getXMLsize(const Metadata& metadata);

private:
    typedef xml::lite::Element* XMLElem;

private:
    static const char CPHD10_URI[];

    //! Returns the default URI
    std::string getDefaultURI() const;

    //! Returns the URI to use with SI Common types
    std::string getSICommonURI() const;

    void fromXML(const XMLElem collectionIDXML, CollectionID& collectionID);
    void fromXML(const XMLElem globalXML, Global& global);
    void fromXML(const XMLElem sceneCoordsXML, SceneCoordinates& scene);
    void fromXML(const XMLElem dataXML, Data& data);
    void fromXML(const XMLElem channelXML, Channel& channel);
    void fromXML(const XMLElem pvpXML, Pvp& pvp);

    void parseVector2D(const XMLElem vecXML, Vector2& vec) const;
    void parseAreaType(const XMLElem areaXML, AreaType& area) const;
    void parseLineSample(const XMLElem lsXML, LineSample& ls) const;
    void parseIAExtent(const XMLElem extentXML, ImageAreaExtent& extent) const;
    void parseChannelParameters(const XMLElem paramXML,
                                ChannelParameter& param) const;
    void parsePVPType(const XMLElem paramXML, PVPType& param) const;
    void parsePVPType(const XMLElem paramXML, APVPType& param) const;

private:
    six::SICommonXMLParser10x mCommon;
};
}

#endif
