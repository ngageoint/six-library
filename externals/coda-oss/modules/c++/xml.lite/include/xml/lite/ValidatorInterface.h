/* =========================================================================
 * This file is part of xml.lite-c++ 
 * =========================================================================
 * 
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
 *
 * xml.lite-c++ is free software; you can redistribute it and/or modify
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

#ifndef CODA_OSS_xml_lite_ValidatorInterface_h_INCLUDED_
#define CODA_OSS_xml_lite_ValidatorInterface_h_INCLUDED_
#pragma once

/*!
 * \file ValidatorInterface.h
 * \brief This is the API for the validator interface.
 *
 * This object is the holder of a pool of schemas in memory
 * and gives the ability to validate any xml against this
 * pool.
 */

#include <string>
#include <vector>
#include <io/InputStream.h>
#include <str/Convert.h>
#include <logging/Logger.h>
#include <sys/filesystem.h>
#include <xml/lite/Element.h>
#include <xml/lite/QName.h>
#include <coda_oss/string.h>
#include <str/Encoding.h>

namespace xml
{
namespace lite
{

/*!
 * \class ValidationInfo
 * \brief This is the information for one 
 *        schema validation error.
 */
struct ValidationInfo final
{
    ValidationInfo(const std::string& message,
                   const std::string& level,
                   const std::string& file,
                   size_t line) :
        mMessage(message), mLevel(level), 
        mFile(file), mLine(line) 
    {
    }

    std::string getMessage() const { return mMessage; }
    std::string getLevel() const { return mLevel; }
    std::string getFile() const { return mFile; }
    size_t getLine() const { return mLine; }

    //! stream to a string
    std::string toString() const
    {
        std::ostringstream oss;
        oss << "[" << this->getLevel() << "]" << 
            " from File: " << this->getFile() << 
            " on Line: " << str::toString(this->getLine()) << 
            " with Message: " << this->getMessage();
        return oss.str();
    }

private:
    std::string mMessage;
    std::string mLevel;
    std::string mFile;
    size_t mLine;
};

/*!
 * \class ValidatorInterface
 * \brief Schema validation is done here.
 *
 * This class is the interface for schema validators
 */
class ValidatorInterface
{
public:

    enum ValidationErrorType
    {
        VALIDATION_WARNING = 0,
        VALIDATION_ERROR,
        VALIDATION_FATAL
    };

    /*! 
     *  Constructor
     *  \param schemaPaths  Vector of both paths and singular schemas
     *                      Note: All schemas must end in *.xsd
     *  \param log          Logger for reporting errors
     *  \param recursive    Do a recursive search for schemas on directory 
     *                      input
     */
    ValidatorInterface(const std::vector<std::string>& /*schemaPaths*/,
                       logging::Logger* /*log*/,
                       bool /*recursive*/ = true) {}
    ValidatorInterface(const std::vector<coda_oss::filesystem::path>&,
                       logging::Logger* /*log*/,
                       bool /*recursive*/ = true) { }

    //! Destructor.
    virtual ~ValidatorInterface() = default;

    /*!
     *  Validation against the internal schema pool
     *  \param xml     Input stream to the xml document to validate
     *  \param xmlID   Identifier for this input xml within the error log
     *  \param errors  Object for returning errors found (errors are appended)
     */
    template <typename TStringStream>
    bool vallidateT(io::InputStream& xml,
                    TStringStream&& oss,
                    const std::string& xmlID,
                    std::vector<ValidationInfo>& errors) const
    {
        // convert to std::string
        xml.streamTo(oss);
        return validate(oss.stream().str(), xmlID, errors);
    }
    bool validate(io::InputStream& xml,
                  const std::string& xmlID,
                  std::vector<ValidationInfo>& errors) const
    {
        return vallidateT(xml, io::StringStream(), xmlID, errors);
    }

    /*!
     *  Validation against the internal schema pool
     *  \param xml     Input stream to the xml document to validate
     *  \param xmlID   Identifier for this input xml within the error log
     *  \param errors  Object for returning errors found (errors are appended)
     */
    bool validate(const Element* xml,
                  const std::string& xmlID,
                  std::vector<ValidationInfo>& errors) const
    {
        // convert to stream
        io::U8StringStream oss;
        xml->print(oss);
        return validate(oss.stream().str(), xmlID, errors);
    }

    /*!
     *  Validation against the internal schema pool
     *  \param xml     The xml document string to validate
     *  \param xmlID   Identifier for this input xml within the error log
     *  \param errors  Object for returning errors found (errors are appended)
     */
    virtual bool validate(const std::string& xml,
                          const std::string& xmlID,
                          std::vector<ValidationInfo>& errors) const = 0;
    virtual bool validate(const coda_oss::u8string&, const std::string& /*xmlID*/, std::vector<ValidationInfo>&) const = 0;
    virtual bool validate(const str::W1252string&, const std::string& /*xmlID*/, std::vector<ValidationInfo>&) const = 0;
};

inline std::ostream& operator<< (std::ostream& out,
                                 const ValidationInfo& info)
{
    out << info.toString();
    return out;
}
}
}

#endif  // CODA_OSS_xml_lite_ValidatorInterface_h_INCLUDED_