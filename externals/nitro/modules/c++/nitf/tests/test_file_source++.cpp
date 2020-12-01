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

#include <vector>
#include <string.h>

#include <import/nitf.hpp>

namespace
{
static const char* const MEMBUF = "ABCABCABCABCABCABCABCABCABCABCABCABCABCABCABCABC";
static const size_t MEMSIZE = strlen(MEMBUF);
static const size_t NUM_BANDS = 3;
static const char* const FILENAME = "test_file.src";

void printBand(const char* band, const char* s, size_t size)
{
	std::cout << "Band " << s << ": [";
    for (size_t i = 0; i < size; ++i)
    {
    	std::cout << band[i];
    }
    std::cout << "]\n";
}


std::string prepareIO()
{
    /*  First we'll create the file for them...  */
    nitf::IOHandle handle(FILENAME, NITF_ACCESS_WRITEONLY, NITF_CREATE);

    /*  And we'll write our buffer out  */
    handle.write(MEMBUF, MEMSIZE);
    handle.close();

    return std::string(FILENAME);
}
}

int main(int, char **)
{
    const size_t bandSize = MEMSIZE / NUM_BANDS;
    const std::string fname = prepareIO();

    const int numBytesPerPix = 1;

    /*  Construct the band sources  */
    nitf::FileSource bs0(fname, 0, numBytesPerPix, NUM_BANDS - 1);
    nitf::FileSource bs1(fname, 1, numBytesPerPix, NUM_BANDS - 1);
    nitf::FileSource bs2(fname, 2, numBytesPerPix, NUM_BANDS - 1);
    nitf::FileSource all(fname, 0, numBytesPerPix, 0);

    /*  Construct in memory band buffers -- for testing -- 0 terminate strings */
    std::vector<char> band0Vec(bandSize + 1);
    char* const band0 = &band0Vec[0];

    std::vector<char> band1Vec(bandSize + 1);
    char* const band1 = &band1Vec[0];

    std::vector<char> band2Vec(bandSize + 1);
    char* const band2 = &band2Vec[0];

    std::vector<char> allBandsVec(MEMSIZE + 1);
    char* const allBands = &allBandsVec[0];

    band0[bandSize] = band1[bandSize] = band2[bandSize] = allBands[MEMSIZE] =
            '\0';

    /*  Read half of the info for one band.  This makes sure that we  */
    /*  are capable of picking up where we left off                   */
    bs0.read(band0, (MEMSIZE / NUM_BANDS / 2));
    bs1.read(band1, (MEMSIZE / NUM_BANDS / 2));
    bs2.read(band2, (MEMSIZE / NUM_BANDS / 2));

    /*  Pick up where we left off and keep going  */
    bs0.read(&band0[MEMSIZE / NUM_BANDS / 2], (MEMSIZE / NUM_BANDS / 2));
    bs1.read(&band1[MEMSIZE / NUM_BANDS / 2], (MEMSIZE / NUM_BANDS / 2));
    bs2.read(&band2[MEMSIZE / NUM_BANDS / 2], (MEMSIZE / NUM_BANDS / 2));
    all.read(allBands, MEMSIZE);

    /*  Now we would like to verify the results of our reading  */

    /*  The first three bands should be all of the same letter B1=A, B2=B, B3=C*/
    printBand(band0, "1", MEMSIZE / NUM_BANDS);
    printBand(band1, "2", MEMSIZE / NUM_BANDS);
    printBand(band2, "3", MEMSIZE / NUM_BANDS);

    /*  The last band source was applied to the entire buffer, so it should  */
    /*  look the same as the original memory source                          */
    printBand(allBands, "ALL", MEMSIZE);

    return 0;
}
