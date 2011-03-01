#include <import/io.h>
#include <import/six.h>
#include <import/six/sicd.h>
#include <import/six/sidd.h>

typedef std::complex<float> ComplexFloat;

enum
{
    CONV_OK = 0,
    CONV_EX_USAGE,
    CONV_EX_VER,
    CONV_EX_INPUT_FILE_NOT_FOUND,
    CONV_EX_OUTPUT_DIR_NOT_FOUND,
    CONV_EX_UNKNOWN
};

void usageAndDie(const char* prog)
{
    std::cerr << "Usage: " << prog
            << " [options] <sicd/sidd-file> <output-file>" << std::endl;
    std::cerr << "Optional fields:" << std::endl;
    std::cerr << "\t-h\t--help\t\tPrint usage" << std::endl;
    std::cerr << "\t-e\t--expand\tExpand RE16I_IM16I to RE32F_IM32F"
            << std::endl;
    std::cerr << "\t--log <file|console>\tLog to a file or console"
            << std::endl;
    std::cerr << "\t-l\t--level <debug|info|warn|error>\tLevel at which to log"
            << std::endl;
    exit(CONV_EX_USAGE);
}

int main(int argc, char** argv)
{

    std::string inputFile;
    std::string outputFile;
    bool expand = false;
    std::string logFile;
    logging::LogLevel logLevel = logging::LogLevel::LOG_INFO;

    for (int i = 1; i < argc; ++i)
    {
        std::string arg(argv[i]);

        if (arg == "-e" || arg == "--expand")
            expand = true;
        else if (arg == "-h" || arg == "--help")
            usageAndDie(argv[0]);
        else if (arg == "--log")
        {
            if (argc == (i + 1))
                usageAndDie(argv[0]);
            logFile = argv[++i];
        }
        else if (arg == "-l" || arg == "--level")
        {
            if (argc == (i + 1))
                usageAndDie(argv[0]);

            std::string level = argv[++i];
            str::lower(level);
            str::trim(level);
            logLevel = logging::LogLevel(level);
        }
        else if (inputFile.empty())
            inputFile = arg;
        else if (outputFile.empty())
            outputFile = arg;
    }

    if (inputFile.empty() || outputFile.empty())
        usageAndDie(argv[0]);

    try
    {
        six::XMLControlFactory::getInstance().addCreator(
                                                         six::DataType::COMPLEX,
                                                         new six::XMLControlCreatorT<
                                                                 six::sicd::ComplexXMLControl>());
        six::XMLControlFactory::getInstance().addCreator(
                                                         six::DataType::DERIVED,
                                                         new six::XMLControlCreatorT<
                                                                 six::sidd::DerivedXMLControl>());

        logging::Logger log;
        if (!logFile.empty())
        {
            if (logFile == "console")
                log.addHandler(new logging::StreamHandler(logLevel), true);
            else
                log.addHandler(new logging::FileHandler(logFile, logLevel),
                               true);
        }
        else
            log.addHandler(new logging::NullHandler, true);

        six::NITFReadControl reader;
        reader.setLogger(&log);

        reader.load(inputFile);
        six::Container* container = reader.getContainer();

        six::BufferList images;
        for (size_t i = 0, num = container->getNumData(); i < num; ++i)
        {
            six::Data *data = container->getData(i);

            // read the entire image into memory
            six::Region region;
            sys::ubyte *imageBuffer = reader.interleaved(region, 0);

            //only expand if we have a 16i image
            if (expand && data->getPixelType() == six::PixelType::RE16I_IM16I)
            {
                //compute the buf size and only allocate that
                unsigned long expandedSize = data->getNumRows()
                        * data->getNumCols() * sizeof(ComplexFloat);
                sys::ubyte* expandedBuffer =
                        (sys::ubyte*) sys::alignedAlloc(expandedSize);

                //expand the image
                short* shorts = (short*) &imageBuffer[0];
                ComplexFloat* cfloats = (ComplexFloat*) &expandedBuffer[0];
                for (unsigned long i = 0, sz = data->getNumRows()
                        * data->getNumCols() * 2; i < sz; i += 2)
                {
                    *cfloats++ = ComplexFloat((float) shorts[i],
                                              (float) shorts[i + 1]);
                }

                delete[] imageBuffer;
                imageBuffer = expandedBuffer;

                //set the new pixel type
                data->setPixelType(six::PixelType::RE32F_IM32F);
            }
            images.push_back(imageBuffer);
        }

        six::WriteControl *writer = new six::NITFWriteControl;
        writer->setLogger(&log);
        writer->initialize(container);
        writer->save(images, outputFile);

        for (six::BufferList::iterator it = images.begin(); it != images.end(); ++it)
            delete[] *it;

        delete container;
        delete writer;
    }
    catch (except::Exception& ex)
    {
        std::cout << ex.toString() << std::endl;
    }
    return 0;
}
