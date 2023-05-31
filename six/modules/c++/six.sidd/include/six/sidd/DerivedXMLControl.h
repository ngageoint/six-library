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
#ifndef __SIX_DERIVED_XML_CONTROL_H__
#define __SIX_DERIVED_XML_CONTROL_H__

#include <std/optional>
#include <std/filesystem>

#include <six/XMLControl.h>
#include <six/Enums.h>

#include <six/sidd/DerivedXMLParser.h>

namespace six
{

// Emphasize that this is for SIDD 3.0.0
namespace sidd300
{
    // We have to support two ISM versions with SIDD 3.0 :-(
    enum class ISMVersion
    {
        v201609, // the "newer" version; default
        v13, // the "original" version

        current = v201609
    };
    std::string to_string(ISMVersion); // "v201609" or "v13"

    ISMVersion get(ISMVersion defaultIfNotSet); // overloaded on ISMVersion
    std::optional<ISMVersion> set(ISMVersion); // returns previous value, if any
    std::optional<ISMVersion> getISMVersion();
    std::optional<ISMVersion> clearISMVersion(); // returns previous value, if any

    std::vector<std::filesystem::path> find_SIDD_schema_V_files(const std::vector<std::filesystem::path>& schemaPaths);
}

namespace sidd
{
    // six.sidd only currently supports --
    //   SIDD 1.0.0
    //   SIDD 2.0.0
    //   SIDD 3.0.0
    enum class Version
    {
        v100,
        v200,
        v300,
    };
    std::string to_string(Version); // "v100", "v200", "v300"

/*!
 *  \class DerivedXMLControl
 *  \brief Turns an DerivedData object into XML and vice versa
 *
 *  Derived XMLControl object for reading and writing DerivedData*
 *  The XMLFactory should be used to create this object, and only
 *  if necessary.  A best practice is to use the six::toXMLCharArray
 *  and six::toXMLString functions to turn Data* objects into XML
 */
struct DerivedXMLControl : public XMLControl
{
    DerivedXMLControl(logging::Logger* log = nullptr, bool ownLog = false);
    DerivedXMLControl(std::unique_ptr<logging::Logger>&&);
    DerivedXMLControl(logging::Logger&);
    DerivedXMLControl(const DerivedXMLControl&) = delete;
    DerivedXMLControl& operator=(const DerivedXMLControl&) = delete;
    DerivedXMLControl(DerivedXMLControl&&) = delete;
    DerivedXMLControl& operator=(DerivedXMLControl&&) = delete;

    static const six::DataType dataType;

    static std::unique_ptr<DerivedXMLParser> getParser_(const std::string& strVersion); // for unit-testing

    std::unique_ptr<Data> fromXML(const xml::lite::Document&, std::optional<six::sidd300::ISMVersion>) const;
    std::unique_ptr<xml::lite::Document> toXML(const Data&, std::optional<six::sidd300::ISMVersion>) const;

protected:
    /*!
     *  Returns a new allocated DOM document, created from the DerivedData*
     */
    virtual xml::lite::Document* toXMLImpl(const Data* data);
    virtual std::unique_ptr<xml::lite::Document> toXMLImpl(const Data&) const override;
    /*!
     *  Returns a new allocated DerivedData*, created from the DOM Document*
     *
     */
    virtual Data* fromXMLImpl(const xml::lite::Document* doc);
    virtual std::unique_ptr<Data> fromXMLImpl(const xml::lite::Document&) const override;

    virtual std::unique_ptr<Data> validateXMLImpl(const xml::lite::Document&,
        const std::vector<std::filesystem::path>&, logging::Logger&) const override;

private:
    std::unique_ptr<DerivedXMLParser>
    getParser(Version version, std::optional<six::sidd300::ISMVersion>) const;
};
}
}

#endif

