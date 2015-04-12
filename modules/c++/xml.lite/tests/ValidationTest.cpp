#include <import/cli.h>
#include <import/xml/lite.h>
#include <import/io.h>
#include <import/mem.h>
#include <import/sys.h>
#include <import/logging.h>

int main(int argc, char** argv)
{
    try 
    {
        // create a parser and add our options to it
        cli::ArgumentParser parser;
        parser.setDescription("The detected image processor.");
        parser.addArgument("-s --schema", "path to schema directory", 
                           cli::STORE)->setDefault(".");
        parser.addArgument("-r --recursive", "recursively search for schemas", 
                           cli::STORE_TRUE)->setDefault(false);
        parser.addArgument("-x --xml", "xml document to validate", 
                           cli::STORE);
        // parse!
        const std::auto_ptr<cli::Results>
            options(parser.parse(argc, (const char**) argv));
        std::auto_ptr<logging::Logger> log(
            logging::setupLogger("ValidationTest"));

        std::vector<std::string> schemaPaths;
        schemaPaths.push_back(options->get<std::string> ("schema"));
        xml::lite::Validator validator(schemaPaths,
                                       log.get(),
                                       options->get<bool> ("recursive"));

        std::vector<xml::lite::ValidationInfo> errors;
        sys::Path path(options->get<std::string> ("xml"));

        io::FileInputStream fis(path.getPath());
        if (validator.validate(fis, path.getPath(), errors))
        {
            for (size_t i = 0; i < errors.size(); ++i)
            {
                std::cout << errors[i] << std::endl;
            }
        }
        fis.close();
    }
    catch (const except::Exception& ex)
    {
        std::cout << "CODA Exception: " << ex.getMessage() << std::endl;
    }
    catch (const std::exception& ex)
    {
        std::cout << "STD Exception: " << ex.what() << std::endl;
    }
    catch (...)
    {
        std::cout << "Unknown Exception: " << "System Error!" << std::endl;
    }

    return 0;
}
