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
#pragma once
#ifndef SIX_six_sidd_DerivedXMLControl_h_INCLUDED_
#define SIX_six_sidd_DerivedXMLControl_h_INCLUDED_

#include <std/optional>
#include <std/filesystem>

#include <six/XMLControl.h>
#include <six/Enums.h>

#include <six/sidd/DerivedData.h>
#include <six/sidd/DerivedXMLParser.h>
#include <six/sidd/Exports.h>

namespace six
{
namespace sidd
{
/*!
 *  \class DerivedXMLControl
 *  \brief Turns an DerivedData object into XML and vice versa
 *
 *  Derived XMLControl object for reading and writing DerivedData*
 *  The XMLFactory should be used to create this object, and only
 *  if necessary.  A best practice is to use the six::toXMLCharArray
 *  and six::toXMLString functions to turn Data* objects into XML
 */
struct SIX_SIDD_API DerivedXMLControl : public XMLControl
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

#endif // SIX_six_sidd_DerivedXMLControl_h_INCLUDED_
