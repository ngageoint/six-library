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

#pragma once
#ifndef CODA_OSS_xml_lite_ValidatorXerces_h_INCLUDED_
#define CODA_OSS_xml_lite_ValidatorXerces_h_INCLUDED_

#include <memory>
#include <vector>
#include <coda_oss/string.h>

#include "config/Exports.h"

#include <xml/lite/xml_lite_config.h>
#ifdef USE_XERCES
#include "xerces_.h"

#include <xml/lite/UtilitiesXerces.h>
#include <xml/lite/ValidatorInterface.h>

namespace xml
{
namespace lite
{

typedef xercesc::DOMError ValidationError;

struct ValidationErrorHandler final : public xercesc::DOMErrorHandler
{
    ValidationErrorHandler() = default;

    ValidationErrorHandler(const ValidationErrorHandler&) = delete;
    ValidationErrorHandler& operator=(const ValidationErrorHandler&) = delete;
    ValidationErrorHandler(ValidationErrorHandler&&) = delete;
    ValidationErrorHandler& operator=(ValidationErrorHandler&&) = delete;

    //! handle the errors during validation
    bool handleError (const ValidationError& err) override;

    //! get the raw information
    const std::vector<ValidationInfo>& getErrorLog() const
    {
        return mErrorLog;
    }

    void clearErrorLog() { mErrorLog.clear(); }

    //! set the id to differentiate between errors
    void setID(const std::string& id) { mID = id; }

    //! stream to a string
    std::string toString() const
    {
        std::ostringstream oss;
        for (size_t i = 0; i < mErrorLog.size(); ++i)
        {
            oss << mErrorLog[i] << std::endl;
        }
        return oss.str();
    }

private:
    std::vector<ValidationInfo> mErrorLog;
    std::string mID;
};

/*!
 * \class ValidatorXerces
 * \brief Schema validation is done here.
 *
 * This class is the Xercesc schema validator
 */
struct CODA_OSS_API ValidatorXerces : public ValidatorInterface
{
    /*! 
     *  Constructor
     *  \param schemaPaths  Vector of both paths and singular schemas
     *                      Note: All schemas must end in *.xsd
     *  \param log          Logger for reporting errors
     *  \param recursive    Do a recursive search for schemas on directory 
     *                      input
     */
    ValidatorXerces(const std::vector<std::string>& schemaPaths, 
                    logging::Logger* log = nullptr,
                    bool recursive = true);
    ValidatorXerces(const std::vector<coda_oss::filesystem::path>&,
                    logging::Logger* log = nullptr,
                    bool recursive = true);

    ValidatorXerces(const ValidatorXerces&) = delete;
    ValidatorXerces& operator=(const ValidatorXerces&) = delete;
    ValidatorXerces(ValidatorXerces&&) = default;
    ValidatorXerces& operator=(ValidatorXerces&&) = default;

    using ValidatorInterface::validate;

    /*!
     *  Validation against the internal schema pool
     *  \param xml     The xml document string to validate
     *  \param xmlID   Identifier for this input xml within the error log
     *  \param errors  Object for returning errors found (errors are appended)
     */
    virtual bool validate(const std::string& xml,
                          const std::string& xmlID,
                          std::vector<ValidationInfo>& errors) const override;
    bool validate(const coda_oss::u8string&, const std::string& xmlID, std::vector<ValidationInfo>&) const override;
    bool validate(const str::W1252string&, const std::string& xmlID, std::vector<ValidationInfo>&) const override;

    // Search each directory for XSD files
    static std::vector<coda_oss::filesystem::path> loadSchemas(const std::vector<coda_oss::filesystem::path>& schemaPaths, bool recursive=true);

private:
    XercesContext mCtxt;

    bool validate_(const coda_oss::u8string& xml, 
                   const std::string& xmlID,
                   std::vector<ValidationInfo>& errors) const;

    std::unique_ptr<xercesc::XMLGrammarPool> mSchemaPool;
    std::unique_ptr<xml::lite::ValidationErrorHandler> mErrorHandler;
    std::unique_ptr<xercesc::DOMLSParser> mValidator;

};

//! stream the entire log -- newline separated
std::ostream& operator<< (std::ostream& out,
                          const ValidationErrorHandler& errorHandler);
}
}

#endif

#endif  // CODA_OSS_xml_lite_ValidatorXerces_h_INCLUDED_
