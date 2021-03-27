/* =========================================================================
 * This file is part of six.sicd-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
 *
 * six.sicd-c++ is free software; you can redistribute it and/or modify
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
 * Simple program to round-trip SICD or SIDD XML and verify the resulting XML matches
 */
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

#include <std/filesystem>

#include <sys/OS.h>
#include <sys/Path.h>
#include <except/Exception.h>
#include <six/XMLControl.h>
#include <six/sicd/ComplexXMLControl.h>
#include <six/sidd/DerivedXMLControl.h>
#include <six/sidd/DerivedData.h>
#include <six/Utilities.h>
#include <io/FileInputStream.h>
#include <logging/StreamHandler.h>

namespace fs = std::filesystem;

namespace
{
class XMLVerifier
{
public:
    XMLVerifier();

    void verify(const std::string& pathname) const;

private:
    void readFile(const std::string& pathname, std::string& contents) const;

private:
    six::XMLControlRegistry mXmlRegistry;
    std::vector<std::string> mSchemaPaths;
    mutable logging::Logger mLog;

    mutable std::vector<char> mScratch;
};

XMLVerifier::XMLVerifier()
{
    // Verify schema path is set
    six::XMLControl::loadSchemaPaths(mSchemaPaths);

    mXmlRegistry.addCreator(six::DataType::COMPLEX,
            new six::XMLControlCreatorT<
                    six::sicd::ComplexXMLControl>());
    mXmlRegistry.addCreator(six::DataType::DERIVED,
            new six::XMLControlCreatorT<
                    six::sidd::DerivedXMLControl>());

    mLog.addHandler(new logging::StreamHandler(logging::LogLevel::LOG_INFO),
                    true);
}

void XMLVerifier::readFile(const std::string& pathname,
                           std::string& contents) const
{
    sys::File inFile(pathname);
    mScratch.resize(inFile.length());

    if (mScratch.empty())
    {
        contents.clear();
    }
    else
    {
        inFile.readInto(mScratch.data(), mScratch.size());
        contents.assign(mScratch.data(), mScratch.size());
    }
}

void XMLVerifier::verify(const std::string& pathname) const
{
    std::cout << "Verifying " << pathname << "...";

    // Read the file in and save it off to a string for comparison later
    std::string inStr;
    readFile(pathname, inStr);

    // Parse the XML - this verifies both that the XML validates against
    // the schema and that our parser reads it without errors
    io::StringStream inStream;
    inStream.write(reinterpret_cast<const std::byte*>(inStr.c_str()),
                   inStr.length());

    std::unique_ptr<six::Data> data(six::parseDataFromFile(mXmlRegistry,
                                                         pathname,
                                                         mSchemaPaths,
                                                         mLog));

    // Write it back out - this verifies both that the XML we write validates
    // against the schema and that our parser writes it without errors
    std::unique_ptr<six::XMLControl> xmlControl(mXmlRegistry.newXMLControl(data->getDataType(), &mLog));
    std::unique_ptr<xml::lite::Document> xmlDoc(xmlControl->toXML(data.get(),
                                              mSchemaPaths));

    const sys::Path::StringPair splitPath = sys::Path::splitExt(pathname);
    const std::string roundTrippedPath = splitPath.first + "_out"
            + splitPath.second;

    io::FileOutputStream outStream(roundTrippedPath);
    xmlDoc->getRootElement()->prettyPrint(outStream);
    outStream.close();

    // Now re-read the output and make sure the Data objects
    // are equal.
    std::unique_ptr<six::Data> readData(six::parseDataFromFile(mXmlRegistry,
        roundTrippedPath,
        mSchemaPaths,
        mLog));

    if (data.get() == nullptr || readData.get() == nullptr || *data != *readData)
    {
        throw except::Exception(Ctxt(
            "Round-tripped Data does not match for '" + pathname + "'"));
    }

    std::cout << " verified" << std::endl;
}
}

int main(int argc, char** argv)
{
    try
    {
        // Parse the command line
        if (argc < 2)
        {
            std::cerr << "Usage: " << fs::path(argv[0]).filename().string()
                      << " <SICD or SIDD XML pathname #1>"
                      << " <SICD or SIDD XML pathname #2> ...\n";
            return 1;
        }

        XMLVerifier verifier;
        for (int ii = 1; ii < argc; ++ii)
        {
            verifier.verify(argv[ii]);
        }

        return 0;
    }
    catch (const std::exception& ex)
    {
        std::cerr << "Caught std::exception: " << ex.what() << std::endl;
        return 1;
    }
    catch (const except::Exception& ex)
    {
        std::cerr << "Caught except::exception: " << ex.getMessage()
            << std::endl;
        return 1;
    }
    catch (...)
    {
        std::cerr << "Caught unknown exception\n";
        return 1;
    }
}
