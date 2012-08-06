/* =========================================================================
 * This file is part of six-c++ 
 * =========================================================================
 * 
 * (C) Copyright 2004 - 2009, General Dynamics - Advanced Information Systems
 *
 * six-c++ is free software; you can redistribute it and/or modify
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
#ifndef __SIX_XML_CONTROL_FACTORY_H__
#define __SIX_XML_CONTROL_FACTORY_H__

#include <memory>

#include <import/mt.h>

#include "six/XMLControl.h"

namespace six
{

/*!
 *  \class XMLControlCreator
 *  \brief Creator design pattern for producing an XMLControl
 *
 *  These objects are registered to the XMLControlFactory singleton
 *  prior to data requests for objects of that type.  This allows
 *  the application developer to enable the proper driver for the
 *  data
 *
 */
struct XMLControlCreator
{
    //!  Constructor
    XMLControlCreator() {}

    //!  Destructor
    virtual ~XMLControlCreator() {}

    /*!
     *  This method creates a new XMLControl object of derived type
     *  on demand
     *
     */
    virtual six::XMLControl* newXMLControl() const = 0;

};

/*!
 *  \class XMLControlCreatorT
 *  \brief Templated implementation of XMLControlCreator
 *
 *  This utility Creator simply uses a default constructor to instantiate
 *  an object T.  This is usually all that a derived implementation will
 *  need
 *
 */
template <typename T> struct XMLControlCreatorT : public XMLControlCreator
{
    ~XMLControlCreatorT() {}
    six::XMLControl* newXMLControl() const { return new T(); }
};

/*!
 *  \class XMLControlRegistry
 *  \brief Dynamic registry pattern for generating a new XML reader/writer
 *
 *  This class generates the proper reader/writer for the Data content.
 *  There are two methods currently, one that uses the DataType to
 *  identify which reader/writer to create, and one that uses a
 *  string (the same one that identifies the type in the container)
 */
class XMLControlRegistry
{
public:
    //!  Constructor
    XMLControlRegistry()
    {
    }

    //!  Destructor
    virtual ~XMLControlRegistry();

    void addCreator(const std::string& identifier,
                    std::auto_ptr<XMLControlCreator> creator);

    /*!
     * Takes ownership of creator
     */
    void addCreator(const std::string& identifier,
                    XMLControlCreator* creator)
    {
        std::auto_ptr<XMLControlCreator> scopedCreator(creator);
        addCreator(identifier, scopedCreator);
    }

    void addCreator(DataType dataType,
                    std::auto_ptr<XMLControlCreator> creator)
    {
        addCreator(dataType.toString(), creator);
    }

    /*!
     * Takes ownership of creator
     */
    void addCreator(DataType dataType, XMLControlCreator* creator)
    {
        std::auto_ptr<XMLControlCreator> scopedCreator(creator);
        addCreator(dataType, scopedCreator);
    }

    /*!
     *  Method to create a new XMLControl from a string.
     *  The created control must be deleted by the caller.
     *
     *  \param identifier what type of XML control to create.
     *  \return XMLControl the produced XML bridge
     *
     */
    XMLControl* newXMLControl(const std::string& identifier) const;

    XMLControl* newXMLControl(DataType dataType) const
    {
        return newXMLControl(dataType.toString());
    }

private:
    typedef std::map<std::string, XMLControlCreator*> RegistryMap;
    RegistryMap mRegistry;
};

/*!
 *  Method to convert from a ComplexData or DerivedData into a C-style
 *  array containing XML.  It is up to the caller to delete this array.
 * 
 *  \param a ComplexData or DerivedData object
 *  \return A new allocated string containing the XML representation of the
 *  data
 */
char* toXMLCharArray(const Data* data,
                     const XMLControlRegistry *xmlRegistry = NULL);

/*!
 *  Convenience method to convert from a ComplexData or DerivedData
 *  into a C++ style string containing XML.
 *
 *  \param a ComplexData or DerivedData object
 *  \return A C++ string object containing the XML
 *
 */
std::string toXMLString(const Data* data,
                        const XMLControlRegistry *xmlRegistry = NULL);


//!  Singleton declaration of our XMLControlRegistry
typedef mt::Singleton<XMLControlRegistry, true> XMLControlFactory;

}

#endif
