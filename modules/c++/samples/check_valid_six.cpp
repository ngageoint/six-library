/* =========================================================================
 * This file is part of six-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2009, General Dynamics - Advanced Information Systems
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

#include <import/cli.h>
#include <import/io.h>
#include <import/mem.h>
#include <import/six.h>
#include <import/six/sicd.h>
#include <import/six/sidd.h>

typedef std::complex<float> ComplexFloat;

int main(int argc, char** argv)
{
    try
    {
        // create a parser and add our options to it
        cli::ArgumentParser parser;
        parser.setDescription("This program reads a SIDD/SICD along with a "\
                              "directory of schemas, and returns any error messages "\
                              "that may be contained in the DES XML");
        parser.addArgument("-f --log", "Specify a log file", cli::STORE, "log",
                           "FILE")->setDefault("console");
        parser.addArgument("-l --level", "Specify log level", cli::STORE,
                           "level", "LEVEL")->setChoices(
                           str::split("debug info warn error"))->setDefault(
                           "info");
        parser.addArgument("-s --schema", 
                           "Specify a schema or directoy of schemas", 
                           cli::STORE, "schema", "FILE", 1, 1, true);
        parser.addArgument("input", "Input SICD/SIDD file or directory of files", cli::STORE, "input",
                           "INPUT", 1, 1);

        const std::auto_ptr<cli::Results>
            options(parser.parse(argc, (const char**) argv));

        sys::OS os;
        std::vector<std::string> inputPaths(1, options->get<std::string> ("input"));
        std::vector<std::string> nitfFiles = os.search(inputPaths, "", ".nitf", false); 
        std::vector<std::string> ntfFiles = os.search(inputPaths, "", ".ntf", false);
        nitfFiles.insert(nitfFiles.end(), ntfFiles.begin(), ntfFiles.end());

        const std::string logFile(options->get<std::string> ("log"));
        std::string level(options->get<std::string> ("level"));
        std::vector<std::string> schemaPaths (
                1, options->get<std::string> ("schema"));

        // create an XML registry
        // The reason to do this is to avoid adding XMLControlCreators to the
        // XMLControlFactory singleton - this way has more fine-grained control
        six::XMLControlRegistry xmlRegistry;
        xmlRegistry.addCreator(six::DataType::COMPLEX,
                               new six::XMLControlCreatorT<
                                       six::sicd::ComplexXMLControl>());
        xmlRegistry.addCreator(six::DataType::DERIVED,
                               new six::XMLControlCreatorT<
                                       six::sidd::DerivedXMLControl>());

        str::upper(level);
        str::trim(level);
        std::auto_ptr<logging::Logger> log = 
            logging::setupLogger("check_valid_six", level, logFile);


        // this validates the DES of the input against the 
        // best available schema
        for (size_t i = 0; i < nitfFiles.size(); ++i)
        {
            try
            {
                six::NITFReadControl reader;
                reader.setLogger(log.get());
                reader.setXMLControlRegistry(&xmlRegistry);
                log->info(Ctxt("Reading " + nitfFiles[i]));
                reader.load(nitfFiles[i], schemaPaths);
                log->info(Ctxt("Successful: No Errors Found!"));
            }
            catch (const except::Exception& ex)
            {
                log->info(Ctxt("Unsuccessful: Please contact your product "\
                               "vendor with these details!"));
            }
        }
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
    return 0;
}
