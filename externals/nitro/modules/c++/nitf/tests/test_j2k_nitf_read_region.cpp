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

#ifndef __STDC_FORMAT_MACROS
#define __STDC_FORMAT_MACROS 1 // needed for PRIu64 with old compilers
#endif
#include <inttypes.h>

#include <stdlib.h>
#include <stdio.h>

#include <std/span>

#if _WIN32
#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
#include <windows.h>
#undef min
#undef max
#endif // _WIN32

#include <str/Format.h>
#include <gsl/gsl.h>

#include <import/nitf.hpp>
#include <nitf/J2KReader.hpp>
#include <nitf/J2KWriter.hpp>

void writeFile(uint32_t x0, uint32_t y0,
    uint32_t x1, uint32_t y1, std::span<const uint8_t> buf, const std::string& prefix)
{
    const auto filename = str::format("%s-raw-%d_%d__%d_%d.out", prefix.c_str(), x0, y0, x1, y1);
    nitf::IOHandle outHandle(filename, NRT_ACCESS_WRITEONLY, NRT_CREATE);
    outHandle.write(buf.data(), buf.size());
    printf("Wrote file: %s\n", filename.c_str());
}

void writeJ2K(uint32_t x0, uint32_t y0,
    uint32_t x1, uint32_t y1, std::span<const uint8_t> buf,
    const j2k::Container& inContainer, const std::string& prefix)
{
    const auto outName = str::format("%s-raw-%d_%d__%d_%d.j2k", prefix.c_str(), x0, y0, x1, y1);

    const auto num_x_tiles = inContainer.getTilesX();
    const auto num_y_tiles = inContainer.getTilesY();
    const auto tile_height = inContainer.getTileHeight();
    const auto tile_width = inContainer.getTileWidth();
    const auto num_components = inContainer.getNumComponents();
   
    // TODO: May need to handle this differently for multiple components
    const auto c = inContainer.getComponent(0);
    const uint32_t precision = c.getPrecision();

    j2k::WriterOptions options;
    /* TODO set some options here */
    j2k::Writer writer(inContainer, options);

    nitf::IOHandle outIO(outName, NRT_ACCESS_WRITEONLY, NRT_CREATE);

    const int tileSize = tile_height * tile_width;
    const int bytes = (precision - 1) / 8 + 1;
    // TODO: determine tile range from read region
    for (uint32_t y = 0; y < num_y_tiles; ++y)
    {
        for (uint32_t x = 0; x < num_x_tiles; ++x)
        {
            const int index = y * num_x_tiles + x % num_x_tiles;
            const int offset = index * tile_height * tile_width * bytes * num_components; 

            const std::span<const uint8_t> buf_(buf.data() + offset, tileSize);
            writer.setTile(x, y, buf_);
        }
    }

    writer.write(outIO);
    printf("Wrote file: %s\n", outName.c_str());
}

int main(int argc, char **argv)
{
    int rc = 0;
    bool dump = false;
    char *fname = NULL;
    uint32_t x0 = 0;
    uint32_t y0 = 0;
    uint32_t x1 = 0;
    uint32_t y1 = 0;

    for (int argIt = 1; argIt < argc; ++argIt)
    {
        if (strcmp(argv[argIt], "--dump") == 0)
        {
            dump = true;
        }
        else if (strcmp(argv[argIt], "--x0") == 0)
        {
            if (argIt >= argc - 1)
                return EXIT_FAILURE;
            x0 = atoi(argv[++argIt]);
        }
        else if (strcmp(argv[argIt], "--y0") == 0)
        {
            if (argIt >= argc - 1)
                return EXIT_FAILURE;
            y0 = atoi(argv[++argIt]);
        }
        else if (strcmp(argv[argIt], "--x1") == 0)
        {
            if (argIt >= argc - 1)
                return EXIT_FAILURE;
            x1 = atoi(argv[++argIt]);
        }
        else if (strcmp(argv[argIt], "--y1") == 0)
        {
            if (argIt >= argc - 1)
                return EXIT_FAILURE;
            y1 = atoi(argv[++argIt]);
        }
        else if (!fname)
        {
            fname = argv[argIt];
        }
    }

    if (!fname)
    {
        fprintf(stderr, "Usage: [--x0 --y0 --x1 --y1] <j2k-file>\n");
        return EXIT_FAILURE;
    }

    nitf::IOHandle io(fname, NRT_ACCESS_READONLY, NRT_OPEN_EXISTING);
    nitf::Reader reader;
    const auto version = reader.getNITFVersion(io);
    if (version == NITF_VER_UNKNOWN)
    {
        fprintf(stderr, "This file does not appear to be a valid NITF\n");
        return EXIT_FAILURE;
    }

    const auto record = reader.readIO(io);
    const auto images = record.getImages();

    int i = 0;
    for (auto it = images.begin(); it != images.end(); ++it, ++i)
    {
        const nitf::ImageSegment segment(*it);
        const auto subheader = segment.getSubheader();

        if (subheader.imageCompression() == nitf::ImageCompression::C8)
        {
            printf("Image %d contains J2K compressed data\n", (i + 1));
            printf("Offset: %" PRIu64 "\n", segment.getImageOffset());
            io.seek(gsl::narrow<nrt_Off>(segment.getImageOffset()), NRT_SEEK_SET);

            j2k::Reader j2kReader(io);
            auto container = j2kReader.getContainer();

            if (x1 == 0)
                x1 = container.getWidth();
            if (y1 == 0)
                y1 = container.getHeight();

            printf("grid width:\t%d\n", container.getGridWidth());
            printf("grid height:\t%d\n", container.getGridHeight());
            printf("tile width:\t%d\n", container.getTileWidth());
            printf("tile height:\t%d\n", container.getTileHeight());
            printf("x tiles:\t%d\n", container.getTilesX());
            printf("y tiles:\t%d\n", container.getTilesY());
            printf("image type:\t%d\n", container.getImageType());

            const auto nComponents = container.getNumComponents();
            printf("components:\t%d\n", nComponents);

            for(uint32_t cmpIt = 0; cmpIt < nComponents; ++cmpIt)
            {
                const auto c = container.getComponent(cmpIt);
                printf("===component %d===\n", (cmpIt + 1));
                printf("width:\t\t%d\n", c.getWidth());
                printf("height:\t\t%d\n", c.getHeight());
                printf("precision:\t%d\n", c.getPrecision());
                printf("x0:\t\t%d\n", c.getOffsetX());
                printf("y0:\t\t%d\n", c.getOffsetY());
                printf("x separation:\t%d\n", c.getSeparationX());
                printf("y separation:\t%d\n", c.getSeparationY());
                printf("signed:\t\t%d\n", c.isSigned());
            }

            const auto c = container.getComponent(0);
            auto buf_ = j2k::make_Buffer();
            auto buf = j2kReader.readRegion(x0, y0, x1, y1, buf_);

            if (dump)
            {
                const auto width = container.getWidth();
                const auto height = container.getHeight();

                const auto result_ = j2kReader.readRegion(0, 0, width, height, buf);
                const std::span<const uint8_t> result(result_.data(), result_.size());

                const auto namePrefix = str::format("image-%d", (i + 1));
                // TODO: Update write to only output tiles in read region
                writeFile(0, 0, width, height, result, namePrefix);
                writeJ2K(0, 0, width, height, result, container, namePrefix);
            }
        }
    }

    return rc;
}
