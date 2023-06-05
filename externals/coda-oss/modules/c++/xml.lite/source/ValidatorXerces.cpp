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

#include <algorithm>
#include <iterator>
#include <std/filesystem>
#include <std/memory>
#include <std/string>
#include <regex>
#include <tuple> // std::ignore

#include <sys/OS.h>
#include <io/StringStream.h>
#include <mem/ScopedArray.h>
#include <str/EncodedStringView.h>
#include <str/utf8.h>

namespace fs = std::filesystem;

#include <xml/lite/xml_lite_config.h>
#ifdef USE_XERCES
#include <xml/lite/ValidatorXerces.h>

namespace xml
{
namespace lite
{
std::ostream& operator<< (std::ostream& out, 
                          const ValidationErrorHandler& errorHandler)
{
    out << errorHandler.toString() << std::endl;
    return out;
}

bool ValidationErrorHandler::handleError(
        const ValidationError& err)
{
    std::string level;
    switch(err.getSeverity())
    {
    case xercesc::DOMError::DOM_SEVERITY_WARNING : 
        level = "WARNING"; 
        break;
    case xercesc::DOMError::DOM_SEVERITY_ERROR : 
        level = "ERROR"; 
        break;
    case xercesc::DOMError::DOM_SEVERITY_FATAL_ERROR : 
        level = "FATAL"; 
        break;
    default :
        level = "WARNING"; 
        break;
    }

    // transcode the file and message
    XercesLocalString message(err.getMessage());

    // create o
    ValidationInfo info (
        message.str(), level, mID, 
        (size_t)err.getLocation()->getLineNumber());
    mErrorLog.push_back(info);

    return true;
}

inline std::vector<std::string> convert(const std::vector<fs::path>& schemaPaths)
{
    std::vector<std::string> retval;
    std::transform(schemaPaths.begin(), schemaPaths.end(), std::back_inserter(retval),
                   [](const fs::path& p) { return p.string(); });
    return retval;
}
inline auto convert(const std::vector<std::string>& paths)
{
    std::vector<fs::path> retval;
    std::transform(paths.begin(), paths.end(), std::back_inserter(retval),
                   [](const auto& p) { return p; });
    return retval;
}

ValidatorXerces::ValidatorXerces(
        const std::vector<fs::path>& schemaPaths,
        logging::Logger* log,
        bool recursive) :
    ValidatorXerces(convert(schemaPaths), log, recursive)
{
}
ValidatorXerces::ValidatorXerces(
    const std::vector<std::string>& schemaPaths, 
    logging::Logger* log,
    bool recursive) :
    ValidatorInterface(schemaPaths, log, recursive)
{
    initialize();

    // load our schemas --
    // search each directory for schemas
    const auto schemas = loadSchemas(convert(schemaPaths), recursive);

    //  add the schema to the validator
    for (auto&& schema : schemas)
    {
        loadGrammar(schema, *log);
    }

    //! no additional schemas will be loaded after this point!
    mSchemaPool->lockPool();
}
ValidatorXerces::ValidatorXerces(const coda_oss::filesystem::path& schema, logging::Logger& log) :
    ValidatorXerces(std::vector<fs::path>{schema}, &log, false /*recursive*/)
{
    initialize();

    //  add the schema to the validator
    loadGrammar(schema, log);

    //! no additional schemas will be loaded after this point!
    mSchemaPool->lockPool();
}

void ValidatorXerces::initialize()
 {
     // add each schema into a grammar pool --
    // this allows reuse
    mSchemaPool.reset(
        new xercesc::XMLGrammarPoolImpl(
            xercesc::XMLPlatformUtils::fgMemoryManager));

    const XMLCh ls_id [] = {xercesc::chLatin_L, 
                            xercesc::chLatin_S, 
                            xercesc::chNull};

    // create the validator
    mValidator.reset(
        xercesc::DOMImplementationRegistry::
            getDOMImplementation (ls_id)->createLSParser(
                xercesc::DOMImplementationLS::MODE_SYNCHRONOUS,
                0, 
                xercesc::XMLPlatformUtils::fgMemoryManager,
                mSchemaPool.get()));

    // set the configuration settings
    xercesc::DOMConfiguration* config = mValidator->getDomConfig();
    config->setParameter(xercesc::XMLUni::fgDOMComments, false);
    config->setParameter(xercesc::XMLUni::fgDOMDatatypeNormalization, true);
    config->setParameter(xercesc::XMLUni::fgDOMEntities, false);
    config->setParameter(xercesc::XMLUni::fgDOMNamespaces, true);
    config->setParameter(xercesc::XMLUni::fgDOMElementContentWhitespace, false);

    // validation settings
    config->setParameter(xercesc::XMLUni::fgDOMValidate, true);
    config->setParameter(xercesc::XMLUni::fgXercesSchema, true);
    config->setParameter(xercesc::XMLUni::fgXercesSchemaFullChecking, false); // this affects performance

    // definitely use cache grammar -- this is the cached schema
    config->setParameter(xercesc::XMLUni::fgXercesUseCachedGrammarInParse, true);

    // explicitly skip loading schema referenced in the xml docs
    config->setParameter(xercesc::XMLUni::fgXercesLoadSchema, false);

    // load additional schema referenced within schemas
    config->setParameter(xercesc::XMLUni::fgXercesHandleMultipleImports, true);

    // it's up to the user to clear the cached schemas
    config->setParameter(xercesc::XMLUni::fgXercesUserAdoptsDOMDocument, true);

    // add a error handler we still have control over
    mErrorHandler.reset(
            new ValidationErrorHandler());
    config->setParameter(xercesc::XMLUni::fgDOMErrorHandler, 
                         mErrorHandler.get());
}

 void ValidatorXerces::loadGrammar(const coda_oss::filesystem::path& schema, logging::Logger& log)
{
     //  add the schema to the validator
     if (!mValidator->loadGrammar(schema.c_str(),
                                  xercesc::Grammar::SchemaGrammarType,
                                  true))
     {
         std::ostringstream oss;
         oss << "Error: Failure to load schema " << schema;
         log.warn(Ctxt(oss.str()));
     }
 }

std::vector<coda_oss::filesystem::path> ValidatorXerces::loadSchemas(const std::vector<coda_oss::filesystem::path>& schemaPaths, bool recursive)
{
    // load our schemas --
    // search each directory for schemas
    sys::OS os;
    return os.search(schemaPaths, "", ".xsd", recursive);
}

// From config.h.in: Define to the 16 bit type used to represent Xerces UTF-16 characters
// On Windows, this needs to be wchar_t so that various "wide character" Win32 APIs can be called.
static_assert(sizeof(XMLCh) == 2, "XMLCh should be two bytes for UTF-16.");

#ifdef _WIN32
// On other platforms, char16_t is used; only wchar_t on Windows.
using XMLCh_t = wchar_t;
static_assert(std::is_same<::XMLCh, XMLCh_t>::value, "XMLCh should be wchar_t");
inline void reset(str::EncodedStringView xmlView, std::unique_ptr<std::wstring>& pWString)
{
    pWString = std::make_unique<std::wstring>(xmlView.wstring());
}
#else
using XMLCh_t = char16_t;
static_assert(std::is_same<::XMLCh, XMLCh_t>::value, "XMLCh should be char16_t");
#endif

inline void reset(str::EncodedStringView xmlView, std::unique_ptr<std::u16string>& pWString)
{
    pWString = std::make_unique<std::u16string>(xmlView.u16string());
}

using XMLCh_string = std::basic_string<XMLCh_t>;
static std::unique_ptr<XMLCh_string> setStringData(xercesc::DOMLSInputImpl& input, const std::u8string& xml)
{
    // expand to the wide character data for use with xerces
    std::unique_ptr<XMLCh_string> retval;
    reset(str::EncodedStringView(xml), retval);
    input.setStringData(retval->c_str());
    return retval;
}

bool ValidatorXerces::validate_(const std::u8string& xml, 
                               const std::string& xmlID,
                               std::vector<ValidationInfo>& errors) const
{
    // clear the log before its use -- 
    // however we do not clear the users 'errors' because 
    // they might want an accumulation of errors
    mErrorHandler->clearErrorLog();

    // set the id so all errors coming from this session 
    // get a matching id
    mErrorHandler->setID(xmlID);

    // get a vehicle to validate data
    xercesc::DOMLSInputImpl input(
        xercesc::XMLPlatformUtils::fgMemoryManager);

    // expand to the wide character data for use with xerces
    auto pWString = setStringData(input, xml);

    // validate the document
    mValidator->parse(&input)->release();

    // add the new errors to the vector 
    errors.insert(errors.end(), 
                  mErrorHandler->getErrorLog().begin(), 
                  mErrorHandler->getErrorLog().end());

    // reset the id
    mErrorHandler->setID("");

    return (!mErrorHandler->getErrorLog().empty());
}

static str::EncodedStringView encodeXml(const std::string& xml)
{
    // The XML might contain a specific encoding, if it does;
    // we want to use it, otherwise we'll corrupt the data.

    // UTF-8 is the normal case, so check it first
    const std::regex reUtf8("<\?.*encoding=.*['\"]?.*utf-8.*['\"]?.*\?>", std::regex::icase);
    std::cmatch m;
    if (std::regex_search(xml.c_str(), m, reUtf8))
    {
        return str::EncodedStringView::fromUtf8(xml);
    }

    // Maybe this is poor XML with Windows-1252 encoding :-(
    const std::regex reWindows1252("<\?.*encoding=.*['\"]?.*windows-1252.*['\"]?.*\?>", std::regex::icase);
    if (std::regex_search(xml.c_str(), m, reWindows1252))
    {
        return str::EncodedStringView::fromWindows1252(xml);
    }

    // No "... encoding= ..."; let EncodedStringView deal with it   
    return str::EncodedStringView(xml);
}

bool ValidatorXerces::validate(const std::string& xml,
                               const std::string& xmlID,
                               std::vector<ValidationInfo>& errors) const
{
    const auto view = encodeXml(xml);
    try
    {
      return validate(view.u8string(), xmlID, errors);
    }
    catch (const utf8::invalid_utf8&) { }

    // Can't process as "native" (UTF-8 on Linux, Windows-1252 on Windows).
    // Must be Windows-1252 on Linux.
    return validate(str::c_str<str::W1252string>(xml), xmlID, errors);
}
bool ValidatorXerces::validate(const coda_oss::u8string& xml,
                               const std::string& xmlID,
                               std::vector<ValidationInfo>& errors) const
{
    return validate_(xml, xmlID, errors);
}
bool ValidatorXerces::validate(const str::W1252string& xml,
                               const std::string& xmlID,
                               std::vector<ValidationInfo>& errors) const
{
    const str::EncodedStringView xmlView(xml);
    return validate(xmlView.u8string(), xmlID, errors);
}

}
}
#endif
