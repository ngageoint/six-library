/******************************************************************************
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
 *
 * Developed in the performance of one or more U.S. Government contracts.
 * The U.S. Government has Unlimited Rights in this computer software as set
 * forth in the Rights in Technical Data and Compute Software: Noncommercial
 * Items clauses.
 ******************************************************************************/
#include <iostream>
#include <stdexcept>
#include <string>
#include <memory>

#include <cphd/CPHDWriter.h>
#include <cphd/CPHDReader.h>
#include <types/RowCol.h>
#include <cli/ArgumentParser.h>

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
        const std::auto_ptr<cli::Results> options(parser.parse(argc, argv));
        const std::string inPathname(options->get<std::string>("input"));
        const std::string outPathname(options->get<std::string>("output"));
        const size_t numThreads(options->get<size_t>("threads"));

        //! Open the CPHD file
        std::cout << "Reading file: " << inPathname << "\n";
        cphd::CPHDReader reader(inPathname, numThreads);

        cphd::CPHDWriter writer(reader.getMetadata(), numThreads);
        const cphd::SampleType sampleType =
                reader.getMetadata().data.sampleType;
        const cphd::VBM& vbm = reader.getVBM();
        cphd::Wideband& wideband = reader.getWideband();

        std::cout << "Writing file: " << outPathname << "\n";
        writer.writeMetadata(outPathname, vbm);

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

            switch (sampleType)
            {
            case cphd::SampleType::RE08I_IM08I:
                writer.writeCPHDData<std::complex<sys::Int8_T> >(
                    reinterpret_cast<const std::complex<sys::Int8_T>* >(data.get()),
                    dims.normL1());
                break;
            case cphd::SampleType::RE16I_IM16I:
                writer.writeCPHDData<std::complex<sys::Int16_T> >(
                    reinterpret_cast<const std::complex<sys::Int16_T>* >(data.get()),
                    dims.normL1());
                break;
            case cphd::SampleType::RE32F_IM32F:
                writer.writeCPHDData<std::complex<float> >(
                    reinterpret_cast<const std::complex<float>* >(data.get()),
                    dims.normL1());
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

