/* =========================================================================
 * This file is part of NITRO
 * =========================================================================
 *
 * (C) Copyright 2004 - 2020, MDA Information Systems LLC
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

#include <nitf/ImageSubheader.hpp>
#include <nitf/ImageWriter.hpp>
#include <nitf/Record.hpp>
#include "TestCase.h"

namespace
{
TEST_CASE(imageWriterThrowsOnFailedConstruction)
{
    nitf::ImageSubheader subheader;
    TEST_EXCEPTION(nitf::ImageWriter(subheader));
}

TEST_CASE(constructValidImageWriter)
{
    nitf::Record record;
    nitf::ImageSegment segment = record.newImageSegment();
    nitf::ImageSubheader subheader = segment.getSubheader();
    std::vector<nitf::BandInfo> bands = {nitf::BandInfo(), nitf::BandInfo()};
    subheader.setPixelInformation("INT", 8, 8, "R", "MONO", "VIS", bands);
    subheader.setBlocking(100, 200, 10, 10, "P");
    nitf::ImageWriter writer(subheader);
}
}

TEST_MAIN(
    TEST_CHECK(imageWriterThrowsOnFailedConstruction);
    TEST_CHECK(constructValidImageWriter);
    )