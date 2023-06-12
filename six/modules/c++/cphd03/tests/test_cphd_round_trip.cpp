/* =========================================================================
 * This file is part of cphd03-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
 *
 * cphd03-c++ is free software; you can redistribute it and/or modify
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
#include <stdexcept>
#include <string>
#include <memory>
#include <thread>

#include <cphd03/CPHDWriter.h>
#include <cphd03/CPHDReader.h>
#include <types/RowCol.h>
#include <cli/ArgumentParser.h>
#include <cphd/Wideband.h>

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
                           "NUM")->setDefault(std::thread::hardware_concurrency());
        parser.addArgument("input", "Input pathname", cli::STORE, "input",
                           "CPHD", 1, 1);
        parser.addArgument("output", "Output pathname", cli::STORE, "output",
                           "CPHD", 1, 1);
        const std::unique_ptr<cli::Results> options(parser.parse(argc, argv));
        const std::string inPathname(options->get<std::string>("input"));
        const std::string outPathname(options->get<std::string>("output"));
        const size_t numThreads(options->get<size_t>("threads"));

        //! Open the CPHD file
        std::cout << "Reading file: " << inPathname << "\n";
        cphd03::CPHDReader reader(inPathname, numThreads);

        cphd03::CPHDWriter writer(reader.getMetadata(), outPathname, numThreads);
        const cphd::SampleType sampleType =
                reader.getMetadata().data.sampleType;
        const cphd03::VBM& vbm = reader.getVBM();
        cphd::Wideband& wideband = reader.getWideband();

        std::cout << "Writing file: " << outPathname << "\n";
        writer.writeMetadata(vbm);

        for (size_t channel = 0;
             channel < reader.getNumChannels();
             ++channel)
        {
            const types::RowCol<size_t> dims(reader.getNumVectors(channel),
                                             reader.getNumSamples(channel));

            auto data = wideband.read(channel,
                          0, cphd::Wideband::ALL,
                          0, cphd::Wideband::ALL,
                          numThreads);

            switch (sampleType)
            {
            case cphd::SampleType::RE08I_IM08I:
                writer.writeCPHDData<std::complex<int8_t> >(
                    reinterpret_cast<const std::complex<int8_t>* >(data.get()),
                    dims.area());
                break;
            case cphd::SampleType::RE16I_IM16I:
                writer.writeCPHDData<std::complex<int16_t> >(
                    reinterpret_cast<const std::complex<int16_t>* >(data.get()),
                    dims.area());
                break;
            case cphd::SampleType::RE32F_IM32F:
                writer.writeCPHDData<cphd::zfloat >(
                    reinterpret_cast<const cphd::zfloat* >(data.get()),
                    dims.area());
                break;
            }
        }

        return 0;
    }
    catch (const std::exception& ex)
    {
        std::cerr << ex.what() << std::endl;
        return 1;
    }
    catch (const except::Exception& ex)
    {
        std::cerr << ex.toString() << std::endl;
        return 1;
    }
    catch (...)
    {
        std::cerr << "Unknown exception\n";
        return 1;
    }
}

