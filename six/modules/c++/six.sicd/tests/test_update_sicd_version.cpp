/* =========================================================================
 * This file is part of six.sicd-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2020, MDA Information Systems LLC
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
 */

#include <iostream>
#include <cli/ArgumentParser.h>
#include <except/Exception.h>
#include <six/Data.h>
#include <six/NITFWriteControl.h>
#include <six/sicd/ComplexData.h>
#include <six/sicd/ComplexXMLControl.h>
#include <six/sicd/Utilities.h>
#include <logging/Logger.h>

namespace
{
void writeSicd(std::auto_ptr<six::Data> complexData,
               const std::vector<std::complex<float>>& widebandData,
               const std::vector<std::string>& schemaPaths,
               const std::string& pathname)
{
    six::XMLControlFactory::getInstance().addCreator(
        six::DataType::COMPLEX,
        new six::XMLControlCreatorT<six::sicd::ComplexXMLControl>());

    mem::SharedPtr<six::Container> container(new six::Container(
            six::DataType::COMPLEX));
    container->addData(complexData);

    six::NITFWriteControl writer(container);

    six::BufferList buffers;
    buffers.push_back(reinterpret_cast<const six::UByte*>(widebandData.data()));
    writer.save(buffers, pathname, schemaPaths);
}

const std::vector<std::string> SICD_VERSIONS =
{
    "0.4.1", "0.5.0", "1.0.0", "1.0.1", "1.1.0", "1.2.0"
};
}

int main(int argc, char **argv)
{
    try
    {
        cli::ArgumentParser parser;
        parser.setDescription("This program is a command-line utility for "
                              "upgrading the SICD metadata. This makes various "
                              "assumptions and is intended to aide in quickly "
                              "generating test products for new standards. It "
                              "is not encouraged for production use.");
        parser.addArgument("-s --schema",
                           "Specify a schema or directory of schemas",
                           cli::STORE, "schema", "FILE");
        parser.addArgument("input", "Input SICD for version upgrade", cli::STORE,
                           "input", "INPUT", 1, 1);
        parser.addArgument("version", "New version to target",
                           cli::STORE, "version")->setChoices(SICD_VERSIONS);
        parser.addArgument("output", "Pathname of output SICD", cli::STORE,
                           "output", "OUTPUT", 1, 1);
        const auto options = parser.parse(argc, argv);

        const std::string pathname = options->get<std::string>("input");
        const std::string version = options->get<std::string>("version");
        std::vector<std::string> schemaPaths;
        if (options->hasValue("schema"))
        {
            schemaPaths.push_back(options->get<std::string>("schema"));
        }

        std::auto_ptr<six::sicd::ComplexData> complexData;
        std::vector<std::complex<float>> widebandData;
        six::sicd::Utilities::readSicd(pathname, schemaPaths,
                                       complexData, widebandData);

        logging::DefaultLogger log("SICD Update");
        six::sicd::Utilities::updateVersion(*complexData, version, log);

        std::auto_ptr<six::Data> data(complexData.release());
        writeSicd(data, widebandData, schemaPaths,
                  options->get<std::string>("output"));
        return 0;
    }
    catch (const except::Exception& ex)
    {
        std::cerr << ex.toString() << "\n";
    }
    catch (const std::exception& ex)
    {
        std::cerr << ex.what() << "\n";
    }
    catch (...)
    {
        std::cerr << "An unknown exception occured\n";
    }
    return 1;
}