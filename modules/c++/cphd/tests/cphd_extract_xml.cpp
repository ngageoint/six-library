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
#include <mem/ScopedArray.h>
#include <io/FileInputStream.h>
#include <xml/lite/MinidomParser.h>
#include <cphd/FileHeader.h>

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

        // Read the file header to determine where the XML section is
        io::FileInputStream inStream(pathname);
        cphd::FileHeader fileHeader;
        fileHeader.read(inStream);

        // Read in the XML section, parsing it through xml.lite as we go
        inStream.seek(fileHeader.getXMLoffset(), io::Seekable::START);
        xml::lite::MinidomParser parser;
        parser.parse(inStream, fileHeader.getXMLsize());

        // Pretty print it out to stdout
        io::StandardOutStream outStream;
        parser.getDocument()->getRootElement()->prettyPrint(outStream);

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
