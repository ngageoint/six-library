/* =========================================================================
 * This file is part of tiff-c++ 
 * =========================================================================
 * 
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
 * © Copyright 2023, Maxar Technologies, Inc.
 *
 * tiff-c++ is free software; you can redistribute it and/or modify
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

#include "tiff/TiffUtils.h"

#include "gsl/gsl.h"
#include "config/compiler_extensions.h"

#include "tiffio.h"
#if _MSC_VER && _WIN32
#pragma comment(lib, "libtiff-c.lib")
#endif

tiff_errorhandler_t tiff_setErrorHandler(tiff_errorhandler_t pHandler)
{
    return TIFFSetErrorHandler(pHandler);
}
tiff_errorhandler_t tiff_setWarningHandler(tiff_errorhandler_t pHandler)
{
    return TIFFSetWarningHandler(pHandler);
}

bool tiff_readData(tiff_stream stream_, coda_oss::byte* buffer, size_t numElements)
{
    void* pStream = stream_;
    auto tif = static_cast<TIFF*>(pStream);

    // Code from PTCOMPUTE SimpleTIFFReader::readTIFFData()

    uint32_t s;
    TIFFGetField(tif, TIFFTAG_BITSPERSAMPLE, &s);

    uint16_t nsamples;
    TIFFGetField(tif, TIFFTAG_SAMPLESPERPIXEL, &nsamples);

    s /= 8;  // bits to bytes
    size_t numBytes = numElements * s * nsamples;
    size_t bytesRead = 0;

    tdata_t buf;

    uint32_t imageWidth, imageLength;
    TIFFGetField(tif, TIFFTAG_IMAGEWIDTH, &imageWidth);
    TIFFGetField(tif, TIFFTAG_IMAGELENGTH, &imageLength);

    uint16_t config;
    TIFFGetField(tif, TIFFTAG_PLANARCONFIG, &config);

    if (TIFFIsTiled(tif))
    {
        uint32_t tileWidth, tileLength, x, y;
        TIFFGetField(tif, TIFFTAG_TILEWIDTH, &tileWidth);
        TIFFGetField(tif, TIFFTAG_TILELENGTH, &tileLength);
        buf = _TIFFmalloc(TIFFTileSize(tif));
        for (y = 0; y < imageLength; y += tileLength)
        {
            for (x = 0; x < imageWidth; x += tileWidth)
            {
                TIFFReadTile(tif, buf, x, y, 0, 0);
                uint32_t imagePtr = s * (y * imageWidth + x);
                uint32_t tilePtr = 0;
                // Now that we have the tile, copy it line-by-line into the
                // output memory buffer (the goofy casting and addressing stuff
                // is done to avoid ugly void* arithmetic compiler warnings)
                // IMPORTANT: Tiles are of a fixed size, and the number of
                // tiles times the tile size may be larger than the actual
                // image. In these cases, the tiles are zero-padded, and we
                // need to copy only the valid range into the output buffer
                int copyLength = std::min(tileLength, imageLength - y);
                int copyWidth = std::min(tileWidth, imageWidth - x);
                for (int l = 0; l < copyLength; l++)
                {
                    memcpy(&buffer[0] + imagePtr, &((char*)buf)[0] + tilePtr, copyWidth * s);
                    imagePtr += imageWidth * s;
                    tilePtr += tileWidth * s;
                }
            }
        }
    }  // end tiled read
    else
    {
        uint32_t row;
        auto scansize = TIFFScanlineSize(tif);
        buf = _TIFFmalloc(scansize);

        if (config == PLANARCONFIG_CONTIG)
        {
            for (row = 0; row < imageLength; row++)
            {
                if (numBytes > bytesRead)
                {
                    TIFFReadScanline(tif, buf, row);
                    auto bytesToRead = gsl::narrow<tmsize_t>(numBytes - bytesRead);
                    if (bytesToRead > scansize)
                    {
                        memcpy(&buffer[0] + bytesRead, buf, scansize);
                    }
                    else
                    {
                        memcpy(&buffer[0] + bytesRead, buf, bytesToRead);
                    }
                    bytesRead += scansize;
                }
            }
        }
        else if (config == PLANARCONFIG_SEPARATE)
        {
            uint16_t sample;
            for (sample = 0; s < nsamples; sample++)
            {
                for (row = 0; row < imageLength; row++)
                {
                    if (numBytes > bytesRead)
                    {
                        TIFFReadScanline(tif, buf, row, sample);
                        auto bytesToRead = gsl::narrow<tmsize_t>(numBytes - bytesRead);
                        if (bytesToRead > scansize)
                        {
                            memcpy(&buffer[0] + bytesRead, buf, scansize);
                        }
                        else
                        {
                            memcpy(&buffer[0] + bytesRead, buf, bytesToRead);
                        }
                        bytesRead += scansize;
                    }
                }
            }
        }
    }  // End scanline (not tiled) read

    _TIFFfree(buf);
    TIFFClose(tif);
    return true;
}
bool tiff_readData(const coda_oss::filesystem::path& fileName, coda_oss::byte* buffer, size_t numElements)
{
    TIFF* tif = TIFFOpen(fileName.string().c_str(), "r");
    if (tif == nullptr)
    {
        return false;
    }

    void* pTif = tif;
    auto tif_ = static_cast<tiff_stream>(pTif);
    return tiff_readData(tif_, buffer, numElements);
}

#include "tiffio.hxx"

CODA_OSS_disable_warning_push
#if _MSC_VER
#pragma warning(disable: 26818) // Switch statement does not cover all cases. Consider adding a '...' label (es.79).
#endif
#include "tif_stream.cxx_"
CODA_OSS_disable_warning_pop

tiff_stream tiff_streamOpen(const std::string& name,  std::istream& is)
{
    void* result = TIFFStreamOpen(name.c_str(), &is);
    return static_cast<tiff_stream>(result);
}
