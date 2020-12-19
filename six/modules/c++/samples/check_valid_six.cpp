/* =========================================================================
 * This file is part of six-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
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
#include <sys/OS.h>
#include <io/FileInputStream.h>
#include <import/six.h>
#include <import/six/sicd.h>
#include <import/six/sidd.h>
#include "utils.h"

#include <sys/Filesystem.h>
namespace fs = std::filesystem;

namespace
{
// TODO: Does this belong in sys?
class ExtensionsPredicate : public sys::FileOnlyPredicate
{
public:
    ExtensionsPredicate(const std::vector<std::string>& extensions,
                        bool ignoreCase = true) :
        mExtensions(extensions),
        mIgnoreCase(ignoreCase)
    {
        if (mIgnoreCase)
        {
            for (size_t ii = 0; ii < mExtensions.size(); ++ii)
            {
                str::lower(mExtensions[ii]);
            }
        }
    }

    virtual bool operator()(const std::string& pathname) const
    {
        if (!sys::FileOnlyPredicate::operator()(pathname))
        {
            return false;
        }

        std::string ext = fs::path(pathname).extension().string();
        if (mIgnoreCase)
        {
            str::lower(ext);
        }

        for (size_t ii = 0; ii < mExtensions.size(); ++ii)
        {
            if (ext == mExtensions[ii])
            {
                return true;
            }
        }

        return false;
    }

private:
    std::vector<std::string> mExtensions;
    const bool mIgnoreCase;
};

std::vector<std::string> getPathnames(const std::string& dirname)
{
    std::vector<std::string> extensions;
    extensions.push_back(".nitf");
    extensions.push_back(".ntf");
    extensions.push_back(".xml");

    return sys::FileFinder::search(ExtensionsPredicate(extensions),
                                   std::vector<std::string>(1, dirname),
                                   false);
}

bool runValidation(const std::unique_ptr<six::Data>& data,
        std::unique_ptr<logging::Logger>& log)
{
    if (data->getDataType() == six::DataType::COMPLEX)
    {
        six::sicd::ComplexData* complexData =
                static_cast<six::sicd::ComplexData*>(
                        data.get());

        if (complexData->validate(*log))
        {
            log->info(Ctxt("Successful: No Errors Found!"));
            return true;
        }
        return false;
    }
    else
    {
        // Nothing to validate
        log->info(Ctxt("Nothing done: not a SICD."));
        return true;
    }
}
}

// make it a little easier to use from MSVC
#if defined(_DEBUG) && defined(_MSC_VER)
#include <sys/Filesystem.h>
namespace fs = std::filesystem;

static fs::path getNitfPath()
{
    const auto cwd = fs::current_path();
    const auto root_dir = cwd.parent_path().parent_path();
    return root_dir / "six" / "tests" / "nitf" / "sicd_1.1.0.nitf";
}

static void setNitfPluginPath()
{
    const auto cwd = fs::current_path();
    const auto root_dir = cwd.parent_path().parent_path().parent_path().parent_path().parent_path();

    const std::string configuration =
#if defined(NDEBUG) // i.e., release
        "Release";
#else
        "Debug";
#endif
    const std::string platform = "x64";

    const auto path = root_dir / "externals" / "nitro" / platform / configuration / "share" / "nitf" / "plugins";
    sys::OS().setEnv("NITF_PLUGIN_PATH", path.string(), true /*overwrite*/);
}
#endif

static int main_(int argc, char** argv)
{
    // make it a little easier to use from MSVC
    #if defined(_DEBUG) && defined(_MSC_VER)
    if (argc == 1)
    {
        setNitfPluginPath();

        // ./six/modules/c++/six/tests/nitf/sidd_1.0.0.nitf
        static const std::string nitf_path = getNitfPath().string();
        argc = 2;
        auto argv_ = static_cast<char**>(malloc(argc * sizeof(char*)));
        argv_[0] = argv[0];
        argv_[1] = const_cast<char*>(nitf_path.c_str());
        //argv[1]
        argv = argv_;
    }
    #endif


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
                        "Specify a schema or directory of schemas",
                        cli::STORE, "schema", "FILE");
    parser.addArgument("input", "Input SICD/SIDD file or directory of files", cli::STORE, "input",
                        "INPUT", 1, 1);

    const std::unique_ptr<cli::Results>
        options(parser.parse(argc, (const char**) argv));

    const std::string inputPath(options->get<std::string>("input"));
    std::vector<std::string> inputPathnames = getPathnames(inputPath);
    const std::string logFile(options->get<std::string>("log"));
    std::string level(options->get<std::string>("level"));
    std::vector<std::string> schemaPaths;
    getSchemaPaths(*options, "--schema", "schema", schemaPaths);

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
    std::unique_ptr<logging::Logger> log =
        logging::setupLogger(fs::path(argv[0]).filename().string(), level, logFile);

    // this validates the DES of the input against the
    // best available schema
    six::NITFReadControl reader;
    reader.setLogger(log.get());
    reader.setXMLControlRegistry(&xmlRegistry);
    bool allValid = true;
    for (size_t ii = 0; ii < inputPathnames.size(); ++ii)
    {
        const std::string& inputPathname(inputPathnames[ii]);
        log->info(Ctxt("Reading " + inputPathname));
        std::unique_ptr<six::Data> data;
        try
        {
            if (nitf::Reader::getNITFVersion(inputPathname) ==
                    NITF_VER_UNKNOWN)
            {
                data = six::parseDataFromFile(xmlRegistry,
                                                inputPathname,
                                                schemaPaths,
                                                *log);
                allValid = allValid && runValidation(data, log);
            }
            else
            {
                reader.load(inputPathname, schemaPaths);
                auto container = reader.getContainer();
                for (size_t jj = 0; jj < container->getNumData(); ++jj)
                {
                    data.reset(container->getData(jj)->clone());
                    allValid = allValid && runValidation(data, log);
                }
            }

        }
        catch (const six::DESValidationException& ex)
        {
            log->error(ex);
            log->error(Ctxt("Unsuccessful: Please contact your product "
                            "vendor with these details!"));
            allValid = false;
        }
        catch (const except::Exception& ex)
        {
            log->error(ex);
            allValid = false;
        }
    }

    return allValid ? EXIT_SUCCESS : EXIT_FAILURE;
}
int main(int argc, char** argv)
{
    try
    {
        return main_(argc, argv);
    }
    catch (const std::exception& ex)
    {
        std::cerr << ex.what() << std::endl;
    }
    catch (...)
    {
        std::cerr << "Unknown exception\n";
    }

    return EXIT_FAILURE;
}