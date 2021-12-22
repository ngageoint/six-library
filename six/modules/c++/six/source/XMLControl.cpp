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
        const sys::OS os;

// prefer SIX_DEFAULT_SCHEMA_PATH, existing scripts use DEFAULT_SCHEMA_PATH
#if defined(DEFAULT_SCHEMA_PATH) && !defined(SIX_DEFAULT_SCHEMA_PATH)
#define SIX_DEFAULT_SCHEMA_PATH DEFAULT_SCHEMA_PATH 
#endif
#ifndef SIX_DEFAULT_SCHEMA_PATH
// Don't want to set a dummy schema path to a directory that exists as that causes
// the code to check for valid schemas and validate.
#if defined(_WIN32)
#define SIX_DEFAULT_SCHEMA_PATH R"(C:\s 0 m e\p at h)" // just to compile ...
#else
#define SIX_DEFAULT_SCHEMA_PATH R"(/s 0 m e/p at h)" // just to compile ...
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
std::vector<std::string> XMLControl::loadSchemaPaths(const std::vector<std::filesystem::path>* pSchemaPaths)
{
    std::vector<std::string> retval;

    // a NULL pointer indicates that we don't want to validate against a schema
    if (pSchemaPaths != nullptr)
    {
        std::transform(pSchemaPaths->begin(), pSchemaPaths->end(), std::back_inserter(retval),
            [&](const std::filesystem::path& p) { return p.string();  });

        // If *pSchemaPaths is empty, this will use a default value.  To avoid all validation against a schema,
        // pass NULL for pSchemaPaths.
        loadSchemaPaths(retval);
    }
    return retval;
}

static std::vector<std::string> check_whether_paths_exist(const std::vector<std::string>& paths)
{
    // If the paths we have don't exist, throw
    std::string does_not_exist_path;
    std::vector<std::string> exist_paths;
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
        throw except::Exception(Ctxt(msg.str()));
    }
    return exist_paths;
}

//  NOTE: Errors are treated as detriments to valid processing
//        and fail accordingly
static void do_validate_(const xml_lite::Document& doc,
    const std::vector<std::string>& paths, logging::Logger* log)
{
    // validate against any specified schemas
    xml_lite::Validator validator(paths, log, true);

    const auto& rootElement = doc.getRootElement();
    if (rootElement->getUri().empty())
    {
        throw six::DESValidationException(Ctxt("INVALID XML: URI is empty so document version cannot be determined to use for validation"));
    }

    // Pretty-print so that lines numbers are useful
    io::StringStream xmlStream;
    rootElement->prettyPrint(xmlStream, xml_lite::StringEncoding::Utf8);

    std::vector<xml_lite::ValidationInfo> errors;
    validator.validate(xmlStream, rootElement->getUri(), errors);

    // log any error found and throw
    if (!errors.empty())
    {
        if (log)
        {
            for (size_t i = 0; i < errors.size(); ++i)
            {
                log->critical(errors[i].toString());
            }
        }

        //! this is a unique error thrown only in this location --
        //  if the user wants a file written regardless of the consequences
        //  they can catch this error, clear the vector and SIX_SCHEMA_PATH
        //  and attempt to rewrite the file. Continuing in this manner is
        //  highly discouraged
        throw six::DESValidationException(Ctxt("INVALID XML: Check both the XML being produced and the schemas available"));
    }
}
static void validate_(const xml_lite::Document& doc,
    std::vector<std::string> paths, logging::Logger* log)
{
    // If the paths we have don't exist, throw
    paths = check_whether_paths_exist(paths);

    // validate against any specified schemas
    if (!paths.empty())
    {
        do_validate_(doc, paths, log);
    }
}
void XMLControl::validate(const xml_lite::Document* doc,
                          const std::vector<std::string>& schemaPaths,
                          logging::Logger* log)
{
    assert(doc != nullptr);

    // attempt to get the schema location from the
    // environment if nothing is specified
    std::vector<std::string> paths(schemaPaths);
    loadSchemaPaths(paths);

    if ((log != nullptr) && schemaPaths.empty())
    {
        std::ostringstream oss;
        oss << "Coudn't validate XML - no schemas paths provided "
            << " and " << six::SCHEMA_PATH << " not set.";

        log->warn(oss.str());
    }

    // validate against any specified schemas
    validate_(*doc, paths, log);
}
void XMLControl::validate(const xml_lite::Document& doc,
    const std::vector<std::filesystem::path>* pSchemaPaths,
    logging::Logger* log)
{
    // attempt to get the schema location from the environment if nothing is specified
    auto paths = loadSchemaPaths(pSchemaPaths);
    if ((log != nullptr) && (pSchemaPaths != nullptr) && paths.empty())
    {
        std::ostringstream oss;
        oss << "Coudn't validate XML - no schemas paths provided "
            << " and " << six::SCHEMA_PATH << " not set.";

        log->warn(oss.str());
    }

    // validate against any specified schemas
    validate_(doc, paths, log);
}

std::string XMLControl::getDefaultURI(const Data& data)
{
    const std::string dataTypeStr = dataTypeToString(data.getDataType(), false);

    return ("urn:" + dataTypeStr + ":" + data.getVersion());
}

std::string XMLControl::getVersionFromURI(const xml_lite::Document* doc)
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

void XMLControl::getVersionFromURI(const xml_lite::Document* doc,
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

std::unique_ptr<xml_lite::Document> XMLControl::toXMLImpl(const Data& data) const
{
    auto pThis = const_cast<XMLControl*>(this);
    std::unique_ptr<xml_lite::Document> retval(pThis->toXMLImpl(&data)); // store raw pointer right away
    assert(retval.get() != nullptr);
    return retval;
}

xml_lite::Document* XMLControl::toXML(
        const Data* data, const std::vector<std::string>& schemaPaths)
{
    xml_lite::Document* doc = toXMLImpl(data);
    validate(doc, schemaPaths, mLog);
    return doc;
}
std::unique_ptr<xml_lite::Document> XMLControl::toXML(
    const Data& data, const std::vector<std::string>& schemaPaths)
{
    return std::unique_ptr<xml_lite::Document>(toXML(&data, schemaPaths));
}
std::unique_ptr<xml_lite::Document> XMLControl::toXML(
    const Data& data, const std::vector<std::filesystem::path>* pSchemaPaths)
{
    auto doc = toXMLImpl(data);
    validate(*doc, pSchemaPaths, mLog);
    return doc;
}

std::unique_ptr<Data> XMLControl::fromXMLImpl(const xml_lite::Document& doc) const
{
    auto pThis = const_cast<XMLControl*>(this);
    std::unique_ptr<Data> retval(pThis->fromXMLImpl(&doc)); // store raw pointer right away
    assert(retval.get() != nullptr);
    return retval;
}

Data* XMLControl::fromXML(const xml_lite::Document* doc,
                          const std::vector<std::string>& schemaPaths_)
{
    std::vector<std::filesystem::path> schemaPaths;
    std::transform(schemaPaths_.begin(), schemaPaths_.end(), std::back_inserter(schemaPaths),
        [](const std::string& s) { return s; });

    auto data = fromXML(*doc, &schemaPaths);
    return data.release();
}
std::unique_ptr<Data> XMLControl::fromXML(const xml_lite::Document& doc,
    const std::vector<std::filesystem::path>* pSchemaPaths)
{
    validate(doc, pSchemaPaths, mLog);
    auto data = fromXMLImpl(doc);
    data->setVersion(getVersionFromURI(&doc));
    return data;
}

std::string XMLControl::dataTypeToString(DataType dataType, bool appendXML)
{
    std::string str = dataType;
    if (appendXML)
    {
        str += "_XML";
    }

    return str;
}
}

std::string six::getSchemaPath(std::vector<std::string>& schemaPaths)
{
    loadDefaultSchemaPath(schemaPaths);
    auto schemaPath = schemaPaths.empty() ? "" : schemaPaths[0];
    if (!fs::is_directory(schemaPath))
    {
        throw except::IOException(Ctxt(FmtX("Directory does not exist: '%s'", schemaPath.c_str())));
    }
    return schemaPath;
}
