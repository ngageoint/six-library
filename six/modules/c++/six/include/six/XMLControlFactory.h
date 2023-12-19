/* =========================================================================
 * This file is part of six-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
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
#pragma once
#ifndef __SIX_XML_CONTROL_FACTORY_H__
#define __SIX_XML_CONTROL_FACTORY_H__

#include <memory>
#include <vector>
#include <std/filesystem>
#include <std/string>

#ifdef SWIGPYTHON
#include <mem/AutoPtr.h>
#endif

#include <scene/sys_Conf.h>

#include "six/XMLControl.h"
#include "six/Exports.h"

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
    XMLControlCreator() = default;

    //!  Destructor
    virtual ~XMLControlCreator() {}

    /*!
     *  This method creates a new XMLControl object of derived type
     *  on demand
     *
     */
    virtual six::XMLControl* newXMLControl(logging::Logger* log) const = 0;

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
    six::XMLControl* newXMLControl(logging::Logger* log) const 
    { 
        return new T(log, false);
    }
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
struct SIX_SIX_API XMLControlRegistry
{
    //!  Destructor
    virtual ~XMLControlRegistry();

    void addCreator(const std::string& identifier,
        std::unique_ptr<XMLControlCreator>&& creator);
    #ifdef SWIGPYTHON
    void addCreator_(const std::string& identifier,
        mem::AutoPtr<XMLControlCreator> creator)
    {
        std::unique_ptr<XMLControlCreator> scopedCreator(creator.release());
        addCreator(identifier, std::move(scopedCreator));
    }
    #endif

    /*!
     * Takes ownership of creator
     */
    void addCreator(const std::string& identifier,
                    XMLControlCreator* creator)
    {
        std::unique_ptr<XMLControlCreator> scopedCreator(creator);
        addCreator(identifier, std::move(scopedCreator));
    }

    void addCreator(DataType dataType,
        std::unique_ptr<XMLControlCreator>&& creator)
    {
        addCreator(dataType.toString(), std::move(creator));
    }
    #ifdef SWIGPYTHON
    void addCreator_(DataType dataType,
                    mem::AutoPtr<XMLControlCreator> creator_)
    {
        std::unique_ptr<XMLControlCreator> creator(creator_.release());
        addCreator(dataType, std::move(creator));
    }
    #endif

    /*!
     * Takes ownership of creator
     */
    void addCreator(DataType dataType, XMLControlCreator* creator)
    {
        std::unique_ptr<XMLControlCreator> scopedCreator(creator);
        addCreator(dataType, std::move(scopedCreator));
    }

    template<typename TXMLControlCreator>
    void addCreator()
    {
        auto scopedCreator = std::make_unique<XMLControlCreatorT<TXMLControlCreator>>();
        addCreator(TXMLControlCreator::dataType, std::move(scopedCreator));
    }

    /*!
     *  Method to create a new XMLControl from a string.
     *  The created control must be deleted by the caller.
     *
     *  \param identifier what type of XML control to create.
     *  \return XMLControl the produced XML bridge
     *
     */
    XMLControl* newXMLControl(const std::string& identifier,
                              logging::Logger* log) const;

    XMLControl* newXMLControl(DataType dataType,
                              logging::Logger* log) const
    {
        return newXMLControl(dataType.toString(), log);
    }

private:
    typedef std::map<std::string, XMLControlCreator*> RegistryMap;
    RegistryMap mRegistry;
};

/*!
 *  Convenience method to convert from a ComplexData or DerivedData
 *  into a C++ style string containing XML.
 *
 *  \param a ComplexData or DerivedData object
 *  \return A C++ string object containing the XML
 *
 */
std::u8string toXMLString(const Data* data,
                        const XMLControlRegistry *xmlRegistry = nullptr);
SIX_SIX_API std::string toXMLString_(const Data* data,
    const XMLControlRegistry* xmlRegistry = nullptr);

/*!
 *  Additionally performs schema validation --
 *  This function must must receive a valid logger to print validation errors
 */
SIX_SIX_API std::u8string toValidXMLString(
        const Data* data,
        const std::vector<std::string>& schemaPaths,
        logging::Logger* log,
        const XMLControlRegistry *xmlRegistry = nullptr);
SIX_SIX_API std::u8string toValidXMLString(const Data&,
    const std::vector<std::string>& schemaPaths, logging::Logger*, const XMLControlRegistry* xmlRegistry = nullptr);
SIX_SIX_API std::u8string toValidXMLString(const Data&,
    const std::vector<std::filesystem::path>*, logging::Logger*, const XMLControlRegistry* xmlRegistry = nullptr);

//!  Singleton declaration of our XMLControlRegistry
SIX_SIX_API XMLControlRegistry& getXMLControlFactory();
namespace XMLControlFactory
{
  inline auto& getInstance()
  {
    return getXMLControlFactory();
  }
}

}

#endif

