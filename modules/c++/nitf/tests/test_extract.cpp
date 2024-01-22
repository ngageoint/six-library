/* =========================================================================
 * This file is part of NITRO
 * =========================================================================
 *
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
 *
 * NITRO is free software; you can redistribute it and/or modify
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
 * License along with this program; if not, If not,
 * see <http://www.gnu.org/licenses/>.
 *
 */

// modeled after nitf_extract.py

#include <stdio.h>

#include <string>
#include <vector>
#include <numeric> // std::iota

#include <import/str.h>
#include <import/sys.h>

#include <import/nitf.hpp>

static  std::vector<std::string> extract_image(const nitf::ImageSubheader& subheader, uint32_t index, nitf::ImageReader& imageReader,
    const std::string& outDir="", const std::string& baseName="")
{
    nitf::SubWindow window(subheader);
    const auto nbpp = subheader.actualBitsPerPixel(); //const auto nbpp = subheader.numBitsPerPixel();
    const auto bandData = imageReader.read(window, nbpp);

    std::vector<std::string> outNames;
    size_t band = 0;
    for (const auto& data : bandData) // for band, data in enumerate(bandData):
    {
        auto outName = str::Format("%s_%d__", baseName, index);
	outName += str::Format("%d_x_%d_", window.getNumRows(), window.getNumCols());
	outName += str::Format("%d_band_%d.out", static_cast<int>(nbpp), static_cast<int>(band));
        outName = sys::Path::joinPaths(outDir, outName);
        auto f = fopen(outName.c_str(), "wb"); // f = open(outName, "wb");
        nitf::write(f, data); // fwrite(data.data(), sizeof(data[0]), data.size() / sizeof(data[0]), f);
        fclose(f); // f.close();
        outNames.push_back(outName);
        band++;
    }

    return outNames;
}

static void extract_images(const std::string& fileName, const std::string& outDir = "")
{
    nitf::IOHandle handle(fileName);
    nitf::Reader reader;
    nitf::Record record = reader.read(handle);

    uint32_t i = 0;
    for (nitf::ImageSegment segment : record.getImages())
    {
        auto imReader = reader.newImageReader(i);
        extract_image(segment.getSubheader(), i, imReader, outDir, sys::Path::basename(fileName));

        i++;
    }

    handle.close();
}

int main(int argc, char **argv)
{
    try
    {
        //  Check argv and make sure we are happy
        if (argc != 3)
        {
            std::cout << "Usage: %s <input-file> <output-file> \n" << argv[0]
                    << std::endl;
            exit( EXIT_FAILURE);
        }

        // Check that wew have a valid NITF
        if (nitf::Reader::getNITFVersion(argv[1]) == nitf::Version::NITF_VER_UNKNOWN)
        {
            std::cout << "Invalid NITF: " << argv[1] << std::endl;
            exit( EXIT_FAILURE);
        }

        extract_images(argv[1]);
        return 0;
    }
    catch (const std::exception& ex)
    {
        std::cerr << "ERROR!: " << ex.what() << "\n";
        return 1;
    }
}

