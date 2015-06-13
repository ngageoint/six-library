#include <import/sys.h>
#include <import/io.h>
#include "zip/ZipFile.h"

int main(int argc, char** argv)
{
    if (argc != 3)
        die_printf("Usage: %s <zip-file> <out-file>\n", argv[0]);

    try
    {
        std::string inputName(argv[1]);
        std::string outputName(argv[2]);

        std::cout << "Attempting to unzip: " 
                  << std::endl << "\tInput: " << inputName << std::endl
                  << "\tTarget: " << outputName << std::endl;;

	io::FileInputStream input(inputName);
	zip::ZipFile zipFile(&input);

	std::cout << zipFile << std::endl;


 	for (zip::ZipFile::Iterator p = zipFile.begin();
 	     p != zipFile.end(); ++p)
        {
 	    zip::ZipEntry* entry = *p;
 	    std::cout << "Entry: " << *entry << std::endl;
        }

        unsigned long numEntries = zipFile.getNumEntries();
        if (numEntries > 1)
            std::cout << "Warning: zip has more than one entry... decompressing the first only" << std::endl;
        
        assert( numEntries );


        zip::ZipFile::Iterator p = zipFile.begin();
        sys::ubyte* uncompressed = (*p)->decompress();

        io::FileOutputStream output(outputName);
        output.write((const sys::byte*)uncompressed, 
                     (*p)->getUncompressedSize());
        input.close();
        output.close();
        delete [] uncompressed;
    }
    catch (except::Exception& ex)
    {
        std::cout << ex.toString() << std::endl;
        exit(EXIT_FAILURE);
    }
    return 0;
}
