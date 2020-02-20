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

#include <cmath>
#include <import/cli.h>
#include <import/io.h>
#include <import/mem.h>
#include <import/six.h>
#include <import/six/convert.h>
#include <import/six/sicd.h>
#include <import/six/sidd.h>
#include "utils.h"

namespace
{
float round(float num)
{
    float f = num - std::floor(num);
    return (f > 0.5f) ? std::ceil(num) : std::floor(num);
}

class CompressFloat
{
public:
    CompressFloat(float min, float diff) :
        mMin(min),
        mDiff(diff)
    {
    }

    inline sys::Int16_T operator()(float value) const
    {
        const float ret(round((((value - mMin) / mDiff) *
                std::numeric_limits<sys::Uint16_T>::max()) +
                        std::numeric_limits<sys::Int16_T>::min()));
        return static_cast<sys::Int16_T>(ret);
    }

private:
    const float mMin;
    const float mDiff;
};

class Buffers
{
public:
    Buffers()
    {
    }

    ~Buffers()
    {
        for (size_t ii = 0; ii < mBuffers.size(); ++ii)
        {
            delete[] mBuffers[ii];
        }
    }

    sys::ubyte* add(size_t numBytes)
    {
        mem::ScopedArray<sys::ubyte> buffer(new sys::ubyte[numBytes]);
        mBuffers.push_back(buffer.get());
        return buffer.release();
    }

    std::vector<sys::ubyte*> get() const
    {
        return mBuffers;
    }

private:
    std::vector<sys::ubyte*> mBuffers;
};

// We've stored the complex<short> in the second half of the buffer
// We'll expand to complex<float> starting in the first half of the buffer
void expandComplex(size_t numPixels, sys::ubyte* buffer)
{
    const std::complex<short>* const input =
            reinterpret_cast<std::complex<short>*>(
                    buffer + numPixels * sizeof(std::complex<short>));

    std::complex<float>* const output =
            reinterpret_cast<std::complex<float>*>(buffer);

    for (size_t ii = 0; ii < numPixels; ++ii)
    {
        output[ii] = std::complex<float>(input[ii].real(), input[ii].imag());
    }
}

// Because we are compressing, it is safe to write directly on top of the
// buffer. This will end up with a buffer that is twice as big as needed
// because we go from a 32 to 16 bit value, but the "extra" data can
// simply be ignored.
//
// Since our buffer could be in the range of [-1.0f, 1.0f] we cannot simply
// cast to a 16 bit int. Instead we expand the values so they always go from
// [-32K, 32K].
void compressInteger(size_t numPixels, sys::ubyte* buffer)
{
    const float* const floatValues = reinterpret_cast<float*>(buffer);

    // Find the min and max values of either real or imag
    float min = floatValues[0];
    float max = floatValues[0];
    for (size_t ii = 1; ii < numPixels * 2; ++ii)
    {
        if (floatValues[ii] < min)
        {
            min = floatValues[ii];
        }
        if (floatValues[ii] > max)
        {
            max = floatValues[ii];
        }
    }

    const std::complex<float>* const input =
            reinterpret_cast<std::complex<float>*>(buffer);

    std::complex<sys::Int16_T>* const output =
            reinterpret_cast<std::complex<sys::Int16_T>*>(buffer);
    const float diff = max - min;

    // If diff ends up being zero, we will get a division by 0 error.
    // This means that buffer is all the same value so we can just
    // fill it with 0s.
    if (diff == 0.0f)
    {
        std::fill_n(output, numPixels, std::complex<sys::Int16_T>(0, 0));
        return;
    }

    const CompressFloat compressFloat(min, diff);
    for (size_t ii = 0; ii < numPixels; ++ii)
    {
        output[ii] = std::complex<sys::Int16_T>(
                compressFloat(input[ii].real()),
                compressFloat(input[ii].imag()));
    }
}
}

int main(int argc, char** argv)
{
    try
    {
        // create a parser and add our options to it
        cli::ArgumentParser parser;
        parser.setDescription("This program reads a SICD or SIDD into the "\
                              "internal memory model and round-trips it back "\
                              "to file.");
        parser.addArgument("-r --rows", "ILOC rows limit", cli::STORE, "maxRows",
                           "ROWS");
        parser.addArgument("--rowsPerBlock", "Max rows per block", cli::STORE,
                           "rowsPerBlock", "ROWS_PER_BLOCK");
        parser.addArgument("--colsPerBlock", "Max cols per block", cli::STORE,
                           "colsPerBlock", "COLS_PER_BLOCK");
        parser.addArgument("--size", "Max image segment size", cli::STORE,
                           "maxSize", "BYTES");
        parser.addArgument("--version", "Version", cli::STORE,
                           "version", "VERSION");
        parser.addArgument("-e --expand", "Expand RE16I_IM16I to RE32F_IM32F",
                           cli::STORE_TRUE, "expand");
        parser.addArgument("-i --convert-to-int",
                           "Expand RE32F_IM32F to RE16I_IM16I",
                           cli::STORE_TRUE, "convertToInt");
        parser.addArgument("-f --log", "Specify a log file", cli::STORE, "log",
                           "FILE")->setDefault("console");
        parser.addArgument("-l --level", "Specify log level", cli::STORE,
                           "level", "LEVEL")->setChoices(
                           str::split("debug info warn error"))->setDefault(
                           "info");
        parser.addArgument("-p --plugin",
                           "Specify a plugin or directory of plugins for "
                           "converting from external vendor format to SICD",
                           cli::STORE);
        parser.addArgument("-s --schema",
                           "Specify a schema or directory of schemas",
                           cli::STORE);
        parser.addArgument("--retainDateTime",
                           "Keep the original datetime in roundtripped file",
                           cli::STORE_TRUE, "retainDateTime");
        parser.addArgument("input", "Input SICD/SIDD", cli::STORE, "input",
                           "INPUT", 1, 1);
        parser.addArgument("output", "Output filename", cli::STORE, "output",
                           "OUTPUT", 1, 1);

        const std::auto_ptr<cli::Results> options(parser.parse(argc, argv));

        const std::string inputFile(options->get<std::string> ("input"));
        const std::string outputFile(options->get<std::string> ("output"));
        const bool expand(options->get<bool> ("expand"));
        const bool convertToInt(options->get<bool>("convertToInt"));
        const bool retainDateTime(options->get<bool>("retainDateTime"));
        const std::string logFile(options->get<std::string> ("log"));
        std::string level(options->get<std::string> ("level"));
        std::vector<std::string> schemaPaths;
        getSchemaPaths(*options, "--schema", "schema", schemaPaths);

        std::string maxSize;
        if (options->hasValue("maxSize"))
        {
            maxSize = options->get<std::string>("maxSize");
        }

        std::string maxILOC;
        if (options->hasValue("maxRows"))
        {
            maxILOC = options->get<std::string>("maxRows");
        }

        std::string rowsPerBlock;
        if (options->hasValue("rowsPerBlock"))
        {
            rowsPerBlock = options->get<std::string>("rowsPerBlock");
        }

        std::string plugin;
        if (options->hasValue("plugin"))
        {
            plugin = options->get<std::string>("plugin");
        }

        std::string colsPerBlock;
        if (options->hasValue("colsPerBlock"))
        {
            colsPerBlock = options->get<std::string>("colsPerBlock");
        }

        std::string version;
        if (options->hasValue("version"))
        {
            version = options->get<std::string>("version");
        }

        str::upper(level);
        str::trim(level);
        logging::LogLevel logLevel(level);

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

        logging::Logger log;
        if (logFile == "console")
            log.addHandler(new logging::StreamHandler(logLevel), true);
        else
            log.addHandler(new logging::FileHandler(logFile, logLevel), true);

        std::auto_ptr<six::ReadControl> reader;
        const std::string extension = sys::Path::splitExt(inputFile).second;
        if (extension == ".nitf" || extension == ".ntf")
        {
            reader.reset(new six::NITFReadControl());
        }
        else
        {
            reader.reset(new six::convert::ConvertingReadControl(plugin));
        }

        reader->setLogger(&log);
        reader->setXMLControlRegistry(&xmlRegistry);

        reader->load(inputFile, schemaPaths);
        mem::SharedPtr<six::Container> container(reader->getContainer());

        // Update the XML to reflect the creation time as right now
        if (!retainDateTime)
        {
            const six::DateTime now;
            for (size_t ii = 0; ii < container->getNumData(); ++ii)
            {
                container->getData(ii)->setCreationTime(now);
            }
        }

        // For SICD, there's only one image (container->getNumData() == 1)
        // For SIDD, there may be more than one image, but there may also be
        // DES's with SICD XML in them.  So here we want to read every image
        // that's present.
        six::Region region;
        region.setStartRow(0);
        region.setStartCol(0);

        Buffers buffers;
        for (size_t ii = 0, imageNum = 0; ii < container->getNumData(); ++ii)
        {
            six::Data* const data = container->getData(ii);

            if (container->getDataType() == six::DataType::COMPLEX ||
                data->getDataType() == six::DataType::DERIVED)
            {
                const types::RowCol<size_t> extent(data->getNumRows(),
                                                   data->getNumCols());
                const size_t numPixels(extent.row * extent.col);
                const bool expandIt =
                        (expand &&
                         data->getPixelType() == six::PixelType::RE16I_IM16I);
                const bool compressIt =
                        (convertToInt &&
                         data->getPixelType() == six::PixelType::RE32F_IM32F);

                size_t numBytesPerPixel = data->getNumBytesPerPixel();
                size_t offset = 0;
                if (expandIt)
                {
                    // We'll make the buffer twice as large as we need for
                    // reading, read into the second half of it, then start
                    // expanding into the first half of it
                    offset = numPixels * numBytesPerPixel;
                    numBytesPerPixel *= 2;
                }

                sys::ubyte* buffer =
                        buffers.add(numPixels * numBytesPerPixel);

                region.setNumRows(extent.row);
                region.setNumCols(extent.col);
                region.setBuffer(buffer + offset);
                reader->interleaved(region, imageNum++);

                if (expandIt)
                {
                    expandComplex(numPixels, buffer);
                    data->setPixelType(six::PixelType::RE32F_IM32F);
                }
                if (compressIt)
                {
                    compressInteger(numPixels, buffer);
                    data->setPixelType(six::PixelType::RE16I_IM16I);
                }
                if (!version.empty())
                {
                    data->setVersion(version);
                }
            }
        }

        six::Options writerOptions;
        if (!maxILOC.empty())
        {
            writerOptions.setParameter(
                    six::NITFHeaderCreator::OPT_MAX_ILOC_ROWS,
                    maxILOC);
        }
        if (!maxSize.empty())
        {
            writerOptions.setParameter(
                    six::NITFHeaderCreator::OPT_MAX_PRODUCT_SIZE,
                    maxSize);
        }

        if (!rowsPerBlock.empty())
        {
            writerOptions.setParameter(
                    six::NITFHeaderCreator::OPT_NUM_ROWS_PER_BLOCK,
                    rowsPerBlock);
        }

        if (!colsPerBlock.empty())
        {
            writerOptions.setParameter(
                    six::NITFHeaderCreator::OPT_NUM_COLS_PER_BLOCK,
                    colsPerBlock);
        }

        six::NITFWriteControl writer(writerOptions, container, &xmlRegistry);
        writer.setLogger(&log);
        writer.save(buffers.get(), outputFile, schemaPaths);
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
