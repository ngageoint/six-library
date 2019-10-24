/* =========================================================================
 * This file is part of cphd-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2019, MDA Information Systems LLC
 *
 * cphd-c++ is free software; you can redistribute it and/or modify
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
#include <fstream>
#include <memory>
#include <logging/NullLogger.h>
#include <cli/ArgumentParser.h>
#include <io/TempFile.h>
#include <io/FileInputStream.h>
#include <io/FileOutputStream.h>
#include <cphd/Metadata.h>
#include <cphd/PVPBlock.h>
#include <cphd/CPHDReader.h>
#include <cphd/CPHDWriter.h>
#include <str/Convert.h>

/*!
 * Reads in CPHD file from InputFile
 * Writes out CPHD file to OutputFile
 * Files should match
 */

void testRoundTrip(std::string inPathname, std::string outPathname, size_t numThreads, std::vector<std::string>& schemaPathname)
{
    //! Open the CPHD file
    cphd::CPHDReader reader(inPathname, numThreads, schemaPathname);
    std::cout << "Succesfully finished reading from CPHD: " << inPathname << "\n";
    const cphd::FileHeader& header = reader.getFileHeader();

    const cphd::Metadata& metadata = reader.getMetadata();

    cphd::CPHDWriter writer(reader.getMetadata(), numThreads);
    const cphd::SignalArrayFormat signalFormat =
            reader.getMetadata().data.getSignalFormat();
    const cphd::PVPBlock& pvpBlock = reader.getPVPBlock();
    const cphd::Wideband& wideband = reader.getWideband();

    writer.writeMetadata(outPathname, pvpBlock);

    // SupportBlock
    const cphd::SupportBlock& supportBlock = reader.getSupportBlock();
    mem::ScopedArray<sys::ubyte> readPtr;
    supportBlock.readAll(numThreads, readPtr);

    size_t idx = 0;
    auto it = metadata.data.supportOffsetMap.begin();
    for (it = metadata.data.supportOffsetMap.begin(), idx = 0;
         it != metadata.data.supportOffsetMap.end();
         ++it, idx = reader.getMetadata().data.getSupportArrayById(it->first).getSize())
    {
        writer.writeSupportData(readPtr.get() + idx, it->first);
    }

    writer.writePVPData(pvpBlock);

    for (size_t channel = 0;
         channel < reader.getNumChannels();
         ++channel)
    {
        const types::RowCol<size_t> dims(reader.getNumVectors(channel),
                                         reader.getNumSamples(channel));
        mem::ScopedArray<sys::ubyte> data;

        wideband.read(channel,
                      0, cphd::Wideband::ALL,
                      0, cphd::Wideband::ALL,
                      numThreads, data);

        switch (signalFormat)
        {
        case cphd::SignalArrayFormat::CI2:
            writer.writeCPHDData<std::complex<sys::Int8_T> >(
                reinterpret_cast<const std::complex<sys::Int8_T>* >(data.get()),
                dims.area(), channel);
            break;
        case cphd::SignalArrayFormat::CI4:
            writer.writeCPHDData<std::complex<sys::Int16_T> >(
                reinterpret_cast<const std::complex<sys::Int16_T>* >(data.get()),
                dims.area(), channel);
            break;
        case cphd::SignalArrayFormat::CF8:
            writer.writeCPHDData<std::complex<float> >(
                reinterpret_cast<const std::complex<float>* >(data.get()),
                dims.area(), channel);
            break;
        }
    }
    std::cout << "Succesfully finished writing to CPHD: " << outPathname << "\n";
}


int main(int argc, char** argv)
{
    try
    {
        // Parse the command line
        cli::ArgumentParser parser;
        parser.setDescription("Round trip for a CPHD file.");
        parser.addArgument("-t --threads",
                           "Specify the number of threads to use",
                           cli::STORE,
                           "threads",
                           "NUM")->setDefault(sys::OS().getNumCPUs());
        parser.addArgument("input", "Input pathname", cli::STORE, "input",
                           "CPHD", 1, 1);
        parser.addArgument("output", "Output pathname", cli::STORE, "output",
                           "CPHD", 1, 1);
        parser.addArgument("schema", "Schema pathname", cli::STORE, "schema",
                           "XSD", 1, 1);
        const std::auto_ptr<cli::Results> options(parser.parse(argc, argv));
        const std::string inPathname(options->get<std::string>("input"));
        const std::string outPathname(options->get<std::string>("output"));
        const std::string schemaPathname(options->get<std::string>("schema"));
        const size_t numThreads(options->get<size_t>("threads"));

        std::vector<std::string> schemas;
        if (!schemaPathname.empty())
        {
            schemas.push_back(schemaPathname);
        }
        testRoundTrip(inPathname, outPathname, numThreads, schemas);
        return 0;
    }
    catch (const except::Exception& ex)
    {
        std::cerr << ex.toString() << std::endl;
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }
    catch (...)
    {
        std::cerr << "Unknown exception\n";
    }
    return 1;

}
