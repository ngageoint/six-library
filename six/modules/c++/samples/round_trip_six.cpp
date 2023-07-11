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

#include <stdint.h>

#include <cmath>
#include <std/filesystem>
#include <vector>
#include <std/span>

#include <types/Complex.h>
#include <import/six.h>
#include <import/cli.h>
#include <import/io.h>
#include <import/mem.h>

#include <import/six.h>
#include <import/six/convert.h>
#include <import/six/sicd.h>
#include <import/six/sidd.h>
#include "utils.h"

namespace fs = std::filesystem;

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

    inline int16_t operator()(float value) const
    {
        const float ret(round((((value - mMin) / mDiff) *
                std::numeric_limits<uint16_t>::max()) +
                        std::numeric_limits<int16_t>::min()));
        return static_cast<int16_t>(ret);
    }

private:
    const float mMin;
    const float mDiff;
};

struct Buffers final
{
    std::byte* add(size_t numBytes)
    {
        mBuffers.push_back(std::vector<std::byte>(numBytes));
        return mBuffers.back().data();
    }

    six::BufferList get()
    {
        six::BufferList retval;
        for (auto& buffer : mBuffers)
        {
            const void* pBuffer = buffer.data();
            retval.push_back(static_cast<const six::UByte*>(pBuffer));
        }
        return retval;
    }

private:
    std::vector<std::vector<std::byte>> mBuffers;
};

// We've stored the complex<short> in the second half of the buffer
// We'll expand to complex<float> starting in the first half of the buffer
void expandComplex(size_t numPixels, std::byte* buffer)
{
    const six::zint16_t* const input =
            reinterpret_cast<six::zint16_t*>(
                    buffer + numPixels * sizeof(six::zint16_t));

    six::zfloat* const output =
            reinterpret_cast<six::zfloat*>(buffer);

    for (size_t ii = 0; ii < numPixels; ++ii)
    {
        output[ii] = six::zfloat(input[ii].real(), input[ii].imag());
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
void compressInteger(size_t numPixels, std::byte* buffer)
{
    auto const floatValues = reinterpret_cast<const float*>(buffer);

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

    auto const input = reinterpret_cast<const six::zfloat*>(buffer);
    auto const output = reinterpret_cast<six::zint16_t*>(buffer);
    const float diff = max - min;

    // If diff ends up being zero, we will get a division by 0 error.
    // This means that buffer is all the same value so we can just
    // fill it with 0s.
    if (diff == 0.0f)
    {
        std::fill_n(output, numPixels, six::zint16_t(0, 0));
        return;
    }

    const CompressFloat compressFloat(min, diff);
    for (size_t ii = 0; ii < numPixels; ++ii)
    {
        output[ii] = six::zint16_t(
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

        const std::unique_ptr<cli::Results> options(parser.parse(argc, argv));

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
        xmlRegistry.addCreator<six::sicd::ComplexXMLControl>();
        xmlRegistry.addCreator<six::sidd::DerivedXMLControl>();

        logging::Logger log;
        if (logFile == "console")
            log.addHandler(new logging::StreamHandler(logLevel), true);
        else
            log.addHandler(new logging::FileHandler(logFile, logLevel), true);

        std::unique_ptr<six::ReadControl> reader;
        std::string extension = fs::path(inputFile).extension().string();
        str::lower(extension);
        if (extension == ".nitf" || extension == ".ntf")
        {
            reader.reset(new six::NITFReadControl());
        }
        else
        {
            reader.reset(new six::convert::ConvertingReadControl(plugin));
        }

        reader->setLogger(log);
        reader->setXMLControlRegistry(&xmlRegistry);

        reader->load(inputFile, schemaPaths);
        auto container(reader->getContainer());

        // Update the XML to reflect the creation time as right now
        if (!retainDateTime)
        {
            const six::DateTime now;
            for (size_t ii = 0; ii < container->size(); ++ii)
            {
                container->getData(ii)->setCreationTime(now);
            }
        }

        // For SICD, there's only one image (container->size() == 1)
        // For SIDD, there may be more than one image, but there may also be
        // DES's with SICD XML in them.  So here we want to read every image
        // that's present.
        six::Region region;

        Buffers buffers;
        for (size_t ii = 0, imageNum = 0; ii < container->size(); ++ii)
        {
            six::Data* const data = container->getData(ii);

            if (container->getDataType() == six::DataType::COMPLEX ||
                data->getDataType() == six::DataType::DERIVED)
            {
                const auto extent = getExtent(*data);
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

                auto buffer = buffers.add(numPixels * numBytesPerPixel);

                setDims(region, extent);
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
        writer.setLogger(log);
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
