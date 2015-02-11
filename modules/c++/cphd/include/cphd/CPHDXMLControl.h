/******************************************************************************
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
 *
 * Developed in the performance of one or more U.S. Government contracts.
 * The U.S. Government has Unlimited Rights in this computer software as set
 * forth in the Rights in Technical Data and Compute Software: Noncommercial
 * Items clauses.
 ******************************************************************************/


#ifndef __CPHD_CPHD_XML_CONTROL_H__
#define __CPHD_CPHD_XML_CONTROL_H__

#include <memory>

#include <mem/SharedPtr.h>
#include <logging/Logger.h>
#include <xml/lite/Element.h>
#include <xml/lite/Document.h>
#include <six/XMLParser.h>
#include <six/SICommonXMLParser10x.h>
#include <cphd/Metadata.h>

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

    CPHDXMLControl(logging::Logger* log, bool ownLog = false);

    /*!
     *  This function takes in a Metadata object and converts
     *  it to a new-allocated XML DOM.
     */
    std::auto_ptr<xml::lite::Document> toXML(const Metadata& metadata);

    /*!
     *  Function takes a DOM Document* node and creates a new-allocated
     *  CPHDData* populated by the DOM.
     */
    std::auto_ptr<Metadata> fromXML(const xml::lite::Document* doc);

    std::string toXMLString(const Metadata& metadata);
    size_t getXMLsize(const Metadata& metadata);

private:
    typedef xml::lite::Element* XMLElem;

    // TODO: These are copies from some of the six.sicd XMLParsers
    XMLElem toXML(const CollectionInformation& obj, XMLElem parent = NULL);

    void fromXML(const XMLElem collectionInfoXML, CollectionInformation& obj);

    XMLElem createLatLonAltFootprint(const std::string& name,
                                     const std::string& cornerName,
                                     const LatLonAltCorners& corners,
                                     XMLElem parent = NULL) const;

private:
    static const char CPHD_URI[];

    //! Returns the default URI
    std::string getDefaultURI() const;

    //! Returns the URI to use with SI Common types
    std::string getSICommonURI() const;

    // Write functions
    XMLElem toXML(const Data& obj, XMLElem parent = NULL);
    XMLElem toXML(const Global& obj, XMLElem parent = NULL);
    XMLElem toXML(const Channel& obj, XMLElem parent = NULL);
    XMLElem toXML(const SRP& obj, XMLElem parent = NULL);
    XMLElem toXML(const Antenna& obj, XMLElem parent = NULL);
    XMLElem toXML(const std::string& name, const AntennaParameters &ap,
            XMLElem parent = NULL);
    XMLElem toXML(const VectorParameters& obj, XMLElem parent = NULL);

    XMLElem areaLineDirectionParametersToXML(const std::string& name,
            const AreaDirectionParameters& obj,
            XMLElem parent = NULL);
    XMLElem areaSampleDirectionParametersToXML(const std::string& name,
            const AreaDirectionParameters& obj,
            XMLElem parent = NULL);

    // Read functions
    void fromXML(const XMLElem dataXML, Data& obj);
    void fromXML(const XMLElem globalXML, Global& obj);
    void fromXML(const XMLElem channelXML, Channel& obj);
    void fromXML(const XMLElem srpXML, SRP& obj);
    void fromXML(const XMLElem antennaXML, Antenna& obj);
    void fromXML(const XMLElem antennaParamsXML, AntennaParameters& params);
    void fromXML(const XMLElem vectorParametersXML, VectorParameters& obj);

private:
    six::SICommonXMLParser10x mCommon;
};
}

#endif
