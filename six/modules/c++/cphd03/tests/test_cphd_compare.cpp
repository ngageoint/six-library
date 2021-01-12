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
#include <thread>

#include <cphd03/CPHDReader.h>
#include <cphd/Wideband.h>
#include <types/RowCol.h>
#include <cli/ArgumentParser.h>

namespace
{
template <typename T>
bool compareCPHDData(const std::byte* data1,
                     const std::byte* data2,
                     size_t size,
                     size_t channel)
{
    const T* castData1 = reinterpret_cast<const T*>(data1);
    const T* castData2 = reinterpret_cast<const T*>(data2);

    for (size_t ii = 0; ii < size; ++ii)
    {
        if (castData1[ii] != castData2[ii])
        {
            std::cout << "Data at channel " << channel
                      << " has differing data starting at"
                      << " index " << ii << "\n";
            return false;
        }
    }

    return true;
}

bool compareWideband(cphd03::CPHDReader& reader1,
                     cphd03::CPHDReader& reader2,
                     size_t channelsToProcess,
                     size_t numThreads)
{
    bool dataMatches = true;

    cphd::Wideband& wideband1 = reader1.getWideband();
    cphd::Wideband& wideband2 = reader2.getWideband();

    std::unique_ptr<std::byte[]> cphd03Data1;
    std::unique_ptr<std::byte[]> cphd03Data2;

    for (size_t ii = 0; ii < channelsToProcess; ++ii)
    {
        const types::RowCol<size_t> dims1(
                reader1.getMetadata().getNumVectors(ii),
                reader1.getMetadata().getNumSamples(ii));
        const types::RowCol<size_t> dims2(
                reader2.getMetadata().getNumVectors(ii),
                reader2.getMetadata().getNumSamples(ii));

        if (dims1 == dims2)
        {
            wideband1.read(ii,
                           0, cphd::Wideband::ALL, 0,
                           cphd::Wideband::ALL,
                           numThreads, cphd03Data1);

            wideband2.read(ii,
                           0, cphd::Wideband::ALL, 0,
                           cphd::Wideband::ALL,
                           numThreads, cphd03Data2);

            switch (reader1.getMetadata().getSampleType())
            {
            case cphd::SampleType::RE08I_IM08I:
                if (!compareCPHDData<std::complex<int8_t> >(
                        cphd03Data1.get(),
                        cphd03Data2.get(),
                        dims1.area(),
                        ii))
                {
                    dataMatches = false;
                }
                break;
            case cphd::SampleType::RE16I_IM16I:
                if (!compareCPHDData<std::complex<int16_t> >(
                        cphd03Data1.get(),
                        cphd03Data2.get(),
                        dims1.area(),
                        ii))
                {
                    dataMatches = false;
                }
                break;
            case cphd::SampleType::RE32F_IM32F:
                if (!compareCPHDData<std::complex<float> >(
                        cphd03Data1.get(),
                        cphd03Data2.get(),
                        dims1.area(),
                        ii))
                {
                    dataMatches = false;
                }
                break;
            }
        }
        else
        {
            std::cout << "Data at channel " << ii
                      << " has differing dimensions\n";
            dataMatches = false;
        }
    }

    return dataMatches;
}
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
                           "NUM")->setDefault(std::thread::hardware_concurrency());
        parser.addArgument("input1", "First pathname", cli::STORE, "input1",
                           "CPHD", 1, 1);
        parser.addArgument("input2", "Second pathname", cli::STORE, "input2",
                           "CPHD", 1, 1);
        const std::unique_ptr<cli::Results> options(parser.parse(argc, argv));
        const std::string inPathname1(options->get<std::string>("input1"));
        const std::string inPathname2(options->get<std::string>("input2"));
        const size_t numThreads(options->get<size_t>("threads"));
        bool dataMatches = true;

        //! Open the CPHD file
        std::cout << "Reading file: " << inPathname1 << "\n";
        cphd03::CPHDReader reader1(inPathname1, numThreads);

        std::cout << "Reading file: " << inPathname2 << "\n";
        cphd03::CPHDReader reader2(inPathname2, numThreads);

        if (reader1.getMetadata() != reader2.getMetadata())
        {
            std::cout << "Metadata does not match\n";
            dataMatches = false;
        }

        const size_t channelsToProcess = std::min(
                reader1.getMetadata().getNumChannels(),
                reader2.getMetadata().getNumChannels());
        if (reader1.getMetadata().getNumChannels() !=
            reader2.getMetadata().getNumChannels())
        {
            std::cout << "Files contain a differing number of channels "
                      << "comparison will continue but will only look at "
                      << "the first " << channelsToProcess << " channels\n";
            dataMatches = false;
        }

        if (reader1.getVBM() != reader2.getVBM())
        {
            std::cout << "VBM does not match\n";
            dataMatches = false;
        }

        //! Only process wideband data if the data types are the same
        if (reader1.getMetadata().getSampleType() ==
            reader2.getMetadata().getSampleType())
        {
            const bool cphd03DataMatches = compareWideband(reader1,
                                                         reader2,
                                                         channelsToProcess,
                                                         numThreads);
            if (!cphd03DataMatches)
            {
                dataMatches = false;
            }
        }
        else
        {
            std::cout << "Data has differing sample type\n";
            dataMatches = false;
        }

        if (dataMatches)
        {
            std::cout << "The two CPHD files match\n";
        }

        return 0;
    }
    catch (const std::exception& ex)
    {
        std::cerr << ex.what() << std::endl;
        return 1;
    }
    catch (...)
    {
        std::cerr << "Unknown exception\n";
        return 1;
    }
}

