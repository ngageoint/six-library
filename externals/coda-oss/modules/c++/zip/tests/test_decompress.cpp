#include <import/sys.h>
#include <import/io.h>
#include <import/zip.h>

int main(int argc, char** argv)
{
    if (argc != 3)
        die_printf("Usage: %s <gz-file> <out-file>\n", argv[0]);

    try
    {
        std::string inputName(argv[1]);
        std::string outputName(argv[2]);

        std::cout << "Attempting to unzip: " 
                  << std::endl << "\tInput: " << inputName << std::endl
                  << "\tTarget: " << outputName << std::endl;;
        
        zip::GZipInputStream input(inputName);
        io::FileOutputStream output(outputName);
        while ( input.streamTo(output, 8192) );

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
