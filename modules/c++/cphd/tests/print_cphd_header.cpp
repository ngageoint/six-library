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

#include <sys/Path.h>
#include <except/Exception.h>
#include <logging/Setup.h>
#include <cphd/CPHDReader.h>

int main(int argc, char** argv)
{
    try
    {
        // Parse the command line
        const std::string progname(sys::Path::basename(argv[0]));
        if (argc != 2)
        {
            std::cerr << "Usage: " << progname << " <CPHD pathname>\n\n";
            return 1;
        }
        const std::string pathname(argv[1]);

        // Set up a logger to log to the console
        mem::SharedPtr<logging::Logger> logger(logging::setupLogger(progname));

        // There are lots of ways we could print this information
        // For now, just use the default ways each object knows to print itself
        static const size_t NUM_THREADS = 1;
        const cphd::CPHDReader reader(pathname, NUM_THREADS, logger);

        std::cout << reader.getFileHeader() << "\n"
                  << reader.getMetadata() << "\n"
                  << reader.getVBM() << "\n";

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
