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

#include <sys/OS.h>
#include <io/StringStream.h>
#include <mem/ScopedArray.h>
#include <str/EncodedStringView.h>

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
ValidatorXerces::ValidatorXerces(
        const std::vector<fs::path>& schemaPaths,
        logging::Logger* log,
        bool recursive) :
    ValidatorXerces(convert(schemaPaths), log, recursive)
{
    // The string conversion code in validate() doesn't work right on all platforms
    // for non-ASCII characters.  But changing that to be correct could break
    // existing code someplace; thus, it's enabled only if using the new
    // fs::path overload, std::string retains existing behavior.
    mLegacyStringConversion = false;
}
ValidatorXerces::ValidatorXerces(
    const std::vector<std::string>& schemaPaths, 
    logging::Logger* log,
    bool recursive) :
    ValidatorInterface(schemaPaths, log, recursive)
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

    // definitely use cache grammer -- this is the cached schema
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

    // load our schemas --
    // search each directory for schemas
    sys::OS os;
    std::vector<std::string> schemas = 
        os.search(schemaPaths, "", ".xsd", recursive);

    //  add the schema to the validator
    for (size_t i = 0; i < schemas.size(); ++i)
    {
        if (!mValidator->loadGrammar(schemas[i].c_str(), 
                                     xercesc::Grammar::SchemaGrammarType,
                                     true))
        {
            std::ostringstream oss;
            oss << "Error: Failure to load schema " << schemas[i];
            log->warn(Ctxt(oss.str()));
        }
    }

    //! no additional schemas will be loaded after this point!
    mSchemaPool->lockPool();
}

// From config.h.in: Define to the 16 bit type used to represent Xerces UTF-16 characters
// On Windows, this needs to be wchar_t so that various "wide character" Win32 APIs can be called.
static_assert(sizeof(XMLCh) == 2, "XMLCh should be two bytes for UTF-16.");

#if _WIN32
// On other platforms, char16_t is used; only wchar_t on Windows.
using XMLCh_t = wchar_t;
static_assert(std::is_same<::XMLCh, XMLCh_t>::value, "XMLCh should be wchar_t");
inline void reset(str::EncodedStringView xmlView, std::unique_ptr<std::wstring>& pWString)
{
    pWString.reset(new std::wstring(xmlView.wstring()));
}
#else
using XMLCh_t = char16_t;
static_assert(std::is_same<::XMLCh, XMLCh_t>::value, "XMLCh should be char16_t");
#endif
inline void reset(str::EncodedStringView xmlView, std::unique_ptr<std::u16string>& pWString)
{
    pWString.reset(new std::u16string(xmlView.u16string()));
}

using XMLCh_string = std::basic_string<XMLCh_t>;

template <typename CharT>
static void setStringData_(xercesc::DOMLSInputImpl& input, const std::basic_string<CharT>& xml, std::unique_ptr<XMLCh_string>& pWString)
{
    reset(str::EncodedStringView(xml), pWString);
    input.setStringData(pWString->c_str());
}
static void setStringData(xercesc::DOMLSInputImpl& input, const std::string& xml, bool legacyStringConversion,
                          std::unique_ptr<XercesLocalString>& pXmlWide, std::unique_ptr<XMLCh_string>& pWString)
{
    if (legacyStringConversion)
    {
        // This doesn't work right for UTF-8 or Windows-1252
        pXmlWide.reset(new XercesLocalString(xml));
        input.setStringData(pXmlWide->toXMLCh());
    }
    else
    {
        setStringData_(input, xml, pWString);
    }
}
inline void setStringData(xercesc::DOMLSInputImpl& input, const coda_oss::u8string& xml, bool /*legacyStringConversion*/,
                          std::unique_ptr<XercesLocalString>&, std::unique_ptr<XMLCh_string>& pWString)
{
    setStringData_(input, xml, pWString);
}
inline void setStringData(xercesc::DOMLSInputImpl& input, const str::W1252string& xml, bool /*legacyStringConversion*/,
                          std::unique_ptr<XercesLocalString>&, std::unique_ptr<XMLCh_string>& pWString)
{
    setStringData_(input, xml, pWString);
}

template<typename CharT>
bool ValidatorXerces::validate_(const std::basic_string<CharT>& xml, bool legacyStringConversion,
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
    std::unique_ptr<XercesLocalString> pXmlWide;
    std::unique_ptr<XMLCh_string> pWString;
    setStringData(input, xml, legacyStringConversion, pXmlWide, pWString);

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
bool ValidatorXerces::validate(const std::string& xml,
                               const std::string& xmlID,
                               std::vector<ValidationInfo>& errors) const
{
    return validate_(xml, mLegacyStringConversion, xmlID, errors);
}
bool ValidatorXerces::validate(const coda_oss::u8string& xml,
                               const std::string& xmlID,
                               std::vector<ValidationInfo>& errors) const
{
    return validate_(xml, false /*legacyStringConversion*/, xmlID, errors);
}
bool ValidatorXerces::validate(const str::W1252string& xml,
                               const std::string& xmlID,
                               std::vector<ValidationInfo>& errors) const
{
    return validate_(xml, false /*legacyStringConversion*/, xmlID, errors);
}

}
}
#endif
