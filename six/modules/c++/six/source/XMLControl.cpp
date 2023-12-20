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

#include <assert.h>

#include <std/filesystem>
#include <algorithm>
#include <iterator>

#include <logging/NullLogger.h>
#include <sys/Path.h>

#include <six/XMLControl.h>
#include <six/Utilities.h>
#include <six/Types.h>
#include <six/Data.h>

namespace fs = std::filesystem;

namespace six
{
    // Have mLogger manage mLog and mOwnLog
    XMLControl::XMLControl(logging::Logger* log, bool ownLog) : mLogger(mLog, mOwnLog, nullptr)
    {
        setLogger(log, ownLog);
    }
    XMLControl::XMLControl(std::unique_ptr<logging::Logger>&& log) : XMLControl()
    {
        setLogger(std::move(log));
    }
    XMLControl::XMLControl(logging::Logger& log) : XMLControl(&log, false /*ownLog*/) { }

    XMLControl::~XMLControl() = default;

static void loadDefaultSchemaPath(std::vector<std::string>& schemaPaths)
{
        static const sys::OS os;

// prefer SIX_DEFAULT_SCHEMA_PATH, existing scripts use DEFAULT_SCHEMA_PATH
#if defined(DEFAULT_SCHEMA_PATH) && !defined(SIX_DEFAULT_SCHEMA_PATH)
#define SIX_DEFAULT_SCHEMA_PATH DEFAULT_SCHEMA_PATH 
#endif
#ifndef SIX_DEFAULT_SCHEMA_PATH
// Don't want to set a dummy schema path to a directory that exists as that causes
// the code to check for valid schemas and validate.
#if defined(_WIN32)
#define SIX_DEFAULT_SCHEMA_PATH "Z:\\s 0 m e\\p at h" // just to compile ...
#else
#define SIX_DEFAULT_SCHEMA_PATH "/s 0 m e/p at h" // just to compile ...
#endif
#endif

        std::string envPath;
        os.getEnvIfSet(six::SCHEMA_PATH, envPath);
        str::trim(envPath);
        if (!envPath.empty())
        {
            // SIX_SCHEMA_PATH might be a search path
	        if (!os.splitEnv(six::SCHEMA_PATH, schemaPaths, std::filesystem::file_type::directory))
            {
                // Nope; assume the caller can figure things out (existing behavior).
                schemaPaths.push_back(envPath);
            }
        }
        else if (os.exists(SIX_DEFAULT_SCHEMA_PATH))
        {
            schemaPaths.push_back(SIX_DEFAULT_SCHEMA_PATH);
        }
}

void XMLControl::loadSchemaPaths(std::vector<std::string>& schemaPaths)
{
    if (schemaPaths.empty())
    {
        loadDefaultSchemaPath(schemaPaths);
    }
}
std::vector<std::filesystem::path> XMLControl::loadSchemaPaths(const std::vector<std::filesystem::path>* pSchemaPaths)
{
    std::vector<std::filesystem::path> retval;

    // a NULL pointer indicates that we don't want to validate against a schema
    if (pSchemaPaths != nullptr)
    {
        std::vector<std::string> paths = sys::convertPaths(*pSchemaPaths);

        // If *pSchemaPaths is empty, this will use a default value.  To avoid all validation against a schema,
        // pass NULL for pSchemaPaths.
        loadSchemaPaths(paths);

        retval = sys::convertPaths(paths);
    }
    return retval;
}

static auto check_whether_paths_exist(const std::vector<std::filesystem::path>& paths)
{
    // If the paths we have don't exist, throw
    std::filesystem::path does_not_exist_path;
    std::vector<std::filesystem::path> exist_paths;
    for (const auto& path : paths)
    {
        if (!fs::exists(path))
        {
            // record the first "not found" path
            if (does_not_exist_path.empty())
            {
                does_not_exist_path = path;
            }
        }
        else
        {
            exist_paths.push_back(path);
        }
    }
    // If none of the paths exist, throw
    if (exist_paths.empty() && !does_not_exist_path.empty())
    {
        std::ostringstream msg;
        msg << does_not_exist_path << " does not exist!";
        throw except::Exception(Ctxt(msg));
    }
    return exist_paths;
}

// Generate a detaled INVALID XML message
static auto getInvalidXmlErrorMessage(const std::vector<std::filesystem::path>& paths)
{
    static const std::string invalidXML = "INVALID XML: Check both the XML being produced and schemas available at ";
    auto message = invalidXML;
    message += (paths.size() > 1 ? "these paths:" : "this path:");
    for (const auto& p : paths)
    {
        message += "\n\t" + p.string();
    }
    return message;
}

static void log_any_errors_and_throw(const std::vector<xml::lite::ValidationInfo>& errors,
    const std::vector<std::filesystem::path>& paths, logging::Logger& log)
{
    if (errors.empty())
    {
        return; // no errors, nothing to do
    }

    auto ctx(Ctxt(getInvalidXmlErrorMessage(paths)));
    for (auto&& e : errors)
    {
        log.critical(e.toString());

        ctx.mMessage += "\n" + e.toString();
    }

    //! this is a unique error thrown only in this location --
    //  if the user wants a file written regardless of the consequences
    //  they can catch this error, clear the vector and SIX_SCHEMA_PATH
    //  and attempt to rewrite the file. Continuing in this manner is
    //  highly discouraged
    throw six::DESValidationException(ctx);
}

//  NOTE: Errors are treated as detriments to valid processing
//        and fail accordingly
static void validate_(const xml::lite::Element& rootElement,
    const std::vector<coda_oss::filesystem::path>& foundSchemas, logging::Logger& log)
{
    xml::lite::Uri uri;
    rootElement.getUri(uri);

    // Pretty-print so that lines numbers are useful
    io::U8StringStream xmlStream;
    rootElement.prettyPrint(xmlStream);
    const auto strPrettyXml = xmlStream.stream().str();

    // Process schema paths one at a time.  This will reduce the "noise" from XML validation failures
    // and could also make instantiating an xml::lite::ValidatorXerces faster.
    std::vector<xml::lite::ValidationInfo> all_errors;
    for (auto&& foundSchema : foundSchemas)
    {
        const std::vector<coda_oss::filesystem::path> foundSchemas_{ { foundSchema } };
        const xml::lite::ValidatorXerces validator(foundSchemas_, &log);

        // validate against any specified schemas
        std::vector<xml::lite::ValidationInfo> errors;
        validator.validate(strPrettyXml, uri.value, errors);

        // Looks like we validated; be sure there aren't any errors
        if (errors.empty())
        {
            return; // success!
        }

        // This schema path failed; save away my errors in case none of them work
        all_errors.insert(all_errors.end(), errors.begin(), errors.end());
    }

    // log any error found and throw
    log_any_errors_and_throw(all_errors, foundSchemas, log);
}

static void validate_(const xml::lite::Document& doc,
    const std::vector<coda_oss::filesystem::path>& foundSchemas, logging::Logger& log)
{
    auto rootElement = doc.getRootElement();
    if (rootElement->getUri().empty())
    {
        throw six::DESValidationException(Ctxt("INVALID XML: URI is empty so document version cannot be determined to use for validation"));
    }

    // validate against any specified schemas
    validate_(*rootElement, foundSchemas, log);
}

static std::vector<coda_oss::filesystem::path> findValidSchemaPaths(const std::vector<std::string>&, logging::Logger*);
static std::vector<coda_oss::filesystem::path> findValidSchemaPaths(const std::vector<std::filesystem::path>*, logging::Logger*);

void XMLControl::validate(const xml::lite::Document* doc,
                          const std::vector<std::string>& schemaPaths,
                          logging::Logger* log)
{
    assert(doc != nullptr);

    // Existing code in xml::lite requires that the Logger be non-NULL
    assert(log != nullptr);

    // validate against any specified schemas
    const auto foundSchemas = findValidSchemaPaths(schemaPaths, log); // If the paths we have don't exist, throw
    validate_(*doc, foundSchemas, *log);
}
void XMLControl::validate(const xml::lite::Document& doc,
    const std::vector<std::filesystem::path>* pSchemaPaths,
    logging::Logger* log)
{
    // Existing code in xml::lite requires that the Logger be non-NULL
    assert(log != nullptr);

    // validate against any specified schemas
    const auto foundSchemas = findValidSchemaPaths(pSchemaPaths, log);
    validate_(doc, foundSchemas, *log);
}

static auto findValidSchemas(const std::vector<std::filesystem::path>& paths_)
{
    // If the paths we have don't exist, throw
    const auto paths = check_whether_paths_exist(paths_);
    return xml::lite::ValidatorXerces::loadSchemas(paths, true /*recursive*/);
}
static std::vector<coda_oss::filesystem::path> findValidSchemaPaths(const std::vector<std::string>& schemaPaths,
    logging::Logger* log)
{
    // attempt to get the schema location from the
    // environment if nothing is specified
    std::vector<std::string> paths(schemaPaths);
    XMLControl::loadSchemaPaths(paths);
    if (paths.empty())
    {
        if (log != nullptr)
        {
            std::ostringstream oss;
            oss << "Coudn't validate XML - no schemas paths provided "
                << " and " << six::SCHEMA_PATH << " not set.";
            log->warn(oss);
        }
    }

    return findValidSchemas(sys::convertPaths(paths)); // If the paths we have don't exist, throw
}
static std::vector<coda_oss::filesystem::path> findValidSchemaPaths(const std::vector<std::filesystem::path>* pSchemaPaths,
    logging::Logger* log)
{
    // attempt to get the schema location from the environment if nothing is specified
    auto paths = XMLControl::loadSchemaPaths(pSchemaPaths);
    if ((pSchemaPaths != nullptr) && paths.empty())
    {
        if (log != nullptr)
        {
            std::ostringstream oss;
            oss << "Coudn't validate XML - no schemas paths provided "
                << " and " << six::SCHEMA_PATH << " not set.";
            log->warn(oss);
        }
    }
    return findValidSchemas(paths); // If the paths we have don't exist, throw
}

std::string XMLControl::getDefaultURI(const Data& data)
{
    const std::string dataTypeStr = dataTypeToString(data.getDataType(), false);

    return ("urn:" + dataTypeStr + ":" + data.getVersion());
}

std::string XMLControl::getVersionFromURI(const xml::lite::Document* doc)
{
    assert(doc != nullptr);
    const auto uri = doc->getRootElement()->getUri();
    if (!(str::startsWith(uri, "urn:SICD:") ||
          str::startsWith(uri, "urn:SIDD:")))
    {
        throw except::Exception(Ctxt(
                "Unable to transform XML DES: Invalid XML namespace URI: " +
                uri));
    }

    return uri.substr(9);  // remove "urn:SIxx:" from beginning
}

void XMLControl::getVersionFromURI(const xml::lite::Document* doc,
                                   std::vector<std::string>& version)
{
    splitVersion(getVersionFromURI(doc), version);
}

void XMLControl::splitVersion(const std::string& versionStr,
                              std::vector<std::string>& version)
{
    version = str::split(versionStr, ".");

    if (version.size() < 2)
    {
        throw except::Exception(Ctxt("Invalid version string: " + versionStr));
    }
}

std::unique_ptr<xml::lite::Document> XMLControl::toXMLImpl(const Data& data) const
{
    auto pThis = const_cast<XMLControl*>(this);
    std::unique_ptr<xml::lite::Document> retval(pThis->toXMLImpl(&data)); // store raw pointer right away
    assert(retval.get() != nullptr);
    return retval;
}

xml::lite::Document* XMLControl::toXML(
        const Data* data, const std::vector<std::string>& schemaPaths)
{
    xml::lite::Document* doc = toXMLImpl(data);
    validate(doc, schemaPaths, mLog);
    return doc;
}
std::unique_ptr<xml::lite::Document> XMLControl::toXML(
    const Data& data, const std::vector<std::string>& schemaPaths)
{
    return std::unique_ptr<xml::lite::Document>(toXML(&data, schemaPaths));
}
std::unique_ptr<xml::lite::Document> XMLControl::toXML(
    const Data& data, const std::vector<std::filesystem::path>* pSchemaPaths)
{
    auto doc = toXMLImpl(data);
    validate(*doc, pSchemaPaths, mLog);
    return doc;
}

std::unique_ptr<Data> XMLControl::fromXMLImpl(const xml::lite::Document& doc) const
{
    auto pThis = const_cast<XMLControl*>(this);
    std::unique_ptr<Data> retval(pThis->fromXMLImpl(&doc)); // store raw pointer right away
    assert(retval.get() != nullptr);
    return retval;
}

Data* XMLControl::fromXML(const xml::lite::Document* doc,
                          const std::vector<std::string>& schemaPaths_)
{
    const auto schemaPaths = sys::convertPaths(schemaPaths_);

    assert(doc != nullptr);
    auto data = fromXML(*doc, &schemaPaths);
    return data.release();
}
std::unique_ptr<Data> XMLControl::fromXML(const xml::lite::Document& doc,
    const std::vector<std::filesystem::path>* pSchemaPaths)
{
    std::unique_ptr<Data> data;
    if ((pSchemaPaths != nullptr) && (mLog != nullptr))
    {
        data = validateXMLImpl(doc, *pSchemaPaths, *mLog);
    }
    else
    {
        // existing code, let validate() handle NULLs
        validate(doc, pSchemaPaths, mLog);
        data = fromXMLImpl(doc);
    }
    data->setVersion(getVersionFromURI(&doc));
    return data;
}

std::unique_ptr<Data> XMLControl::validateXMLImpl_(const xml::lite::Document& doc,
    const std::vector<std::filesystem::path>& schemaPaths, logging::Logger& log) const
{
    validate(doc, &schemaPaths, &log);
    return fromXMLImpl(doc);
}
std::unique_ptr<Data> XMLControl::validateXMLImpl(const xml::lite::Document& doc,
    const std::vector<std::filesystem::path>& schemaPaths, logging::Logger& log) const
{
    return validateXMLImpl_(doc, schemaPaths, log);
}

std::string XMLControl::dataTypeToString(DataType dataType, bool appendXML)
{
    std::string str;
    switch (dataType)
    {
    case DataType::COMPLEX:
        str = "SICD";
        break;
    case DataType::DERIVED:
        str = "SIDD";
        break;
    default:
        throw except::Exception(Ctxt("Invalid data type " + str::toString(dataType)));
    }

    if (appendXML)
    {
        str += "_XML";
    }

    return str;
}
}

std::string six::getSchemaPath(std::vector<std::string>& schemaPaths, bool tryToExpandIfNotFound)
{
    loadDefaultSchemaPath(schemaPaths);

    // This is hacky; the whole point of having "schemaPaths" be a vector is that there could
    // be MULTIPLE valid directories, not just one.
    auto schemaPath = schemaPaths.empty() ? "" : schemaPaths[0]; // TODO: use all directories in schemaPaths
    if (fs::is_directory(schemaPath))
    {
        return schemaPath;
    }

    if (tryToExpandIfNotFound)
    {
        // schemaPath might contain special enviroment variables
        schemaPath = sys::Path::expandEnvironmentVariables(schemaPath, fs::file_type::directory);
        if (fs::is_directory(schemaPath))
        {
            schemaPath = fs::absolute(schemaPath).string(); // get rid of embedded ".."
            schemaPaths[0] = schemaPath;
            return schemaPath;
        }
    }

    throw except::IOException(Ctxt(str::Format("Directory does not exist: '%s'", schemaPath)));
}
