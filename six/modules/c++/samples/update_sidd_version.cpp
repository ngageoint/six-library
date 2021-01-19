/* =========================================================================
 * This file is part of six-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2020, MDA Information Systems LLC
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

#include <cli/ArgumentParser.h>
#include <except/Exception.h>
#include <logging/Logger.h>
#include <six/Data.h>
#include <six/NITFWriteControl.h>
#include <six/sidd/DerivedData.h>
#include <six/sidd/DerivedXMLControl.h>
#include <six/sidd/SIDDVersionUpdater.h>
#include <six/sidd/Utilities.h>
#include <iostream>

namespace
{
void readSidd(const std::string& pathname,
              std::vector<std::string> schemaPaths,
              std::auto_ptr<six::sidd::DerivedData>& derivedData,
              std::vector<six::UByte>& widebandData)
{
    logging::NullLogger log;

    six::NITFReadControl reader;
    reader.load(pathname);
    auto container = reader.getContainer();
    if (container->getDataType() != six::DataType::DERIVED)
    {
        throw except::Exception(Ctxt(pathname + " is not a SIDD"));
    }
    derivedData.reset(reinterpret_cast<six::sidd::DerivedData*>(
            container->getData(0)->clone()));

    const types::RowCol<size_t> extent(derivedData->getNumRows(),
                                       derivedData->getNumCols());
    widebandData.resize(extent.area() * derivedData->getNumBytesPerPixel());
    six::Region region;
    region.setStartRow(0);
    region.setStartCol(0);
    region.setNumRows(extent.row);
    region.setNumCols(extent.col);
    region.setBuffer(widebandData.data());

    reader.interleaved(region, 0);
}

void writeSidd(std::auto_ptr<six::Data> derivedData,
               const std::vector<six::UByte>& widebandData,
               const std::vector<std::string>& schemaPaths,
               const std::string& pathname)
{
    mem::SharedPtr<six::Container> container(
            new six::Container(six::DataType::DERIVED));
    container->addData(derivedData);

    six::NITFWriteControl writer(container);

    six::BufferList buffers;
    buffers.push_back(reinterpret_cast<const six::UByte*>(widebandData.data()));
    writer.save(buffers, pathname, schemaPaths);
}
}

int main(int argc, char** argv)
{
    try
    {
        static const auto& versions =
                six::sidd::SIDDVersionUpdater::getValidVersions();
        cli::ArgumentParser parser;
        parser.setDescription("This program is a command-line utility for "
                              "upgrading the SIDD metadata. This makes various "
                              "assumptions and is intended to aide in quickly "
                              "generating test products for new standards. It "
                              "is not encouraged for production use.");
        parser.addArgument("-s --schema",
                           "Specify a schema or directory of schemas",
                           cli::STORE,
                           "schema",
                           "FILE");
        parser.addArgument("input",
                           "Input SIDD for version upgrade",
                           cli::STORE,
                           "input",
                           "INPUT",
                           1,
                           1);
        parser.addArgument("version",
                           "New version to target",
                           cli::STORE,
                           "version")
                ->setChoices(versions);
        parser.addArgument("output",
                           "Pathname of output SIDD",
                           cli::STORE,
                           "output",
                           "OUTPUT",
                           1,
                           1);
        const auto options = parser.parse(argc, argv);

        const std::string pathname = options->get<std::string>("input");
        const std::string version = options->get<std::string>("version");
        std::vector<std::string> schemaPaths;
        if (options->hasValue("schema"))
        {
            schemaPaths.push_back(options->get<std::string>("schema"));
        }

        six::XMLControlFactory::getInstance().addCreator(
                six::DataType::DERIVED,
                new six::XMLControlCreatorT<six::sidd::DerivedXMLControl>());

        std::auto_ptr<six::sidd::DerivedData> derivedData;
        std::vector<six::UByte> widebandData;
        readSidd(pathname, schemaPaths, derivedData, widebandData);

        logging::DefaultLogger log("SIDD Update");
        six::sidd::SIDDVersionUpdater(*derivedData, version, log).update();

        std::auto_ptr<six::Data> data(derivedData.release());
        writeSidd(data,
                  widebandData,
                  schemaPaths,
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
