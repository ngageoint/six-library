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

#include <xml/lite/xml_lite_config.h>
#ifdef USE_XERCES
#include <xml/lite/ValidatorXerces.h>

#include <algorithm>
#include <iterator>

#include <sys/OS.h>
#include <io/StringStream.h>
#include <mem/ScopedArray.h>

namespace fs = coda_oss::filesystem;

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

bool ValidatorXerces::validate(const std::string& xml,
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
    XercesLocalString xmlWide(xml);
    input.setStringData(xmlWide.toXMLCh());

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
}
}
#endif
