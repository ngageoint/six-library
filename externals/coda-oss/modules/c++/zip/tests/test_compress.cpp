#include <import/sys.h>
#include <import/io.h>
#include <import/zip.h>

int main(int argc, char** argv)
{
    if (argc != 2)
        die_printf("Usage: %s <file>\n", argv[0]);

    try
    {
        std::string inputName(argv[1]);
        std::string outputName = sys::Path::basename(inputName);
        outputName += ".gz";
        
        std::cout << "Attempting to zip: " 
                  << std::endl << "\tInput: " << inputName << std::endl
                  << "\tTarget: " << outputName << std::endl;;
        
        io::FileInputStream input(inputName);
        
        zip::GZipOutputStream output(outputName);
        
        input.streamTo(output);
        input.close();
        output.close();
    }
    catch (except::Exception& ex)
    {
        std::cout << ex.toString() << std::endl;
        exit(EXIT_FAILURE);
    }
    return 0;
}
