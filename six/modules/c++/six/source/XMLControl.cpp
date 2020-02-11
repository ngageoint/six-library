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

#include <logging/NullLogger.h>
#include <six/XMLControl.h>

namespace six
{
XMLControl::XMLControl(logging::Logger* log, bool ownLog) :
    mLog(NULL),
    mOwnLog(false)
{
    setLogger(log, ownLog);
}

XMLControl::~XMLControl()
{
    if (mLog && mOwnLog)
    {
        delete mLog;
    }
}

void XMLControl::loadSchemaPaths(std::vector<std::string>& schemaPaths)
{
    if (schemaPaths.empty())
    {
        std::string envPath;
        sys::OS().getEnvIfSet(six::SCHEMA_PATH, envPath);
        str::trim(envPath);
        if (!envPath.empty())
        {
            schemaPaths.push_back(envPath);
        }
        else
        {
            schemaPaths.push_back(DEFAULT_SCHEMA_PATH);
        }
    }
}

//  NOTE: Errors are treated as detriments to valid processing
//        and fail accordingly
void XMLControl::validate(const xml::lite::Document* doc,
                          const std::vector<std::string>& schemaPaths,
                          logging::Logger* log)
{
    // attempt to get the schema location from the
    // environment if nothing is specified
    std::vector<std::string> paths(schemaPaths);
    loadSchemaPaths(paths);

    sys::OS os;
    // If the paths we have don't exist, throw
    for (size_t ii = 0; ii < paths.size(); ++ii)
    {
        if (!os.exists(paths[ii]))
        {
            std::ostringstream msg;
            msg << paths[ii] << " does not exist!";
            throw except::Exception(Ctxt(msg.str()));
        }
    }

    // validate against any specified schemas
    if (!paths.empty())
    {
        xml::lite::Validator validator(paths, log, true);

        std::vector<xml::lite::ValidationInfo> errors;

        if (doc->getRootElement()->getUri().empty())
        {
            throw six::DESValidationException(Ctxt(
                    "INVALID XML: URI is empty so document version cannot be "
                    "determined to use for validation"));
        }

        // Pretty-print so that lines numbers are useful
        io::StringStream xmlStream;
        doc->getRootElement()->prettyPrint(xmlStream);

        validator.validate(xmlStream, doc->getRootElement()->getUri(), errors);

        // log any error found and throw
        if (!errors.empty())
        {
            for (size_t i = 0; i < errors.size(); ++i)
            {
                log->critical(errors[i].toString());
            }

            //! this is a unique error thrown only in this location --
            //  if the user wants a file written regardless of the consequences
            //  they can catch this error, clear the vector and SIX_SCHEMA_PATH
            //  and attempt to rewrite the file. Continuing in this manner is
            //  highly discouraged
            throw six::DESValidationException(
                    Ctxt("INVALID XML: Check both the XML being "
                         "produced and the schemas available"));
        }
    }
}

void XMLControl::setLogger(logging::Logger* log, bool own)
{
    if (mLog && mOwnLog && log != mLog)
    {
        delete mLog;
        mLog = NULL;
    }

    if (log)
    {
        mLog = log;
        mOwnLog = own;
    }
    else
    {
        mLog = new logging::NullLogger;
        mOwnLog = true;
    }
}

std::string XMLControl::getDefaultURI(const Data& data)
{
    const std::string dataTypeStr = dataTypeToString(data.getDataType(), false);

    return ("urn:" + dataTypeStr + ":" + data.getVersion());
}

std::string XMLControl::getVersionFromURI(const xml::lite::Document* doc)
{
    const std::string uri = doc->getRootElement()->getUri();
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

xml::lite::Document* XMLControl::toXML(
        const Data* data, const std::vector<std::string>& schemaPaths)
{
    xml::lite::Document* doc = toXMLImpl(data);
    validate(doc, schemaPaths, mLog);
    return doc;
}

Data* XMLControl::fromXML(const xml::lite::Document* doc,
                          const std::vector<std::string>& schemaPaths)
{
    validate(doc, schemaPaths, mLog);
    Data* const data = fromXMLImpl(doc);
    data->setVersion(getVersionFromURI(doc));
    return data;
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
            throw except::Exception(
                    Ctxt("Invalid data type " + str::toString(dataType)));
    }

    if (appendXML)
    {
        str += "_XML";
    }

    return str;
}
}
