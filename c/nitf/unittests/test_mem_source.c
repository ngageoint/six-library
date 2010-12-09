/* =========================================================================
 * This file is part of NITRO
 * =========================================================================
 * 
 * (C) Copyright 2004 - 2010, General Dynamics - Advanced Information Systems
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

#include <import/nitf.h>
#include "Test.h"

#define MEMBUF "ABCABCABCABCABCABCABCABCABCABCABCABCABCABCABCABC"
#define BAND1 "AAAAAAAAAAAAAAAA"
#define BAND2 "BBBBBBBBBBBBBBBB"
#define BAND3 "CCCCCCCCCCCCCCCC"
#define MEMSIZE strlen(MEMBUF)
#define NUM_BANDS 3

TEST_CASE(testMemorySource)
{
    /*  Get the error object       */
    nitf_Error error;
    int bandSize = MEMSIZE / NUM_BANDS;
    int numBytesPerPix = 1;
    char *band_0 = NULL, *band_1 = NULL, *band_2 = NULL, *all_bands = NULL;

    /*  Construct the band sources  */
    nitf_BandSource *bs0 = nitf_MemorySource_construct(MEMBUF, MEMSIZE, 0,
                                                       numBytesPerPix,
                                                       NUM_BANDS - 1, &error);
    nitf_BandSource *bs1 = nitf_MemorySource_construct(MEMBUF, MEMSIZE, 1,
                                                       numBytesPerPix,
                                                       NUM_BANDS - 1, &error);
    nitf_BandSource *bs2 = nitf_MemorySource_construct(MEMBUF, MEMSIZE, 2,
                                                       numBytesPerPix,
                                                       NUM_BANDS - 1, &error);
    nitf_BandSource *all = nitf_MemorySource_construct(MEMBUF, MEMSIZE, 0,
                                                       numBytesPerPix, 0,
                                                       &error);

    TEST_ASSERT(bs0);
    TEST_ASSERT(bs1);
    TEST_ASSERT(bs2);
    TEST_ASSERT(all);

    /*  Construct in memory band buffers for testing -- 0 terminate strings */
    band_0 = (char *) NITF_MALLOC(bandSize + 1);
    band_1 = (char *) NITF_MALLOC(bandSize + 1);
    band_2 = (char *) NITF_MALLOC(bandSize + 1);
    all_bands = (char *) NITF_MALLOC(MEMSIZE + 1);
    band_0[bandSize] = 0;
    band_1[bandSize] = 0;
    band_2[bandSize] = 0;
    all_bands[MEMSIZE] = 0;

    /*  Read half of the info for one band.  This makes sure that we  */
    /*  are capable of picking up where we left off                   */
    bs0->iface->read(bs0->data, band_0, (MEMSIZE / NUM_BANDS / 2), &error);
    bs1->iface->read(bs1->data, band_1, (MEMSIZE / NUM_BANDS / 2), &error);
    bs2->iface->read(bs2->data, band_2, (MEMSIZE / NUM_BANDS / 2), &error);

    /*  Pick up where we left off and keep going  */
    bs0->iface->read(bs0->data, &band_0[MEMSIZE / NUM_BANDS / 2], (MEMSIZE
            / NUM_BANDS / 2), &error);
    bs1->iface->read(bs1->data, &band_1[MEMSIZE / NUM_BANDS / 2], (MEMSIZE
            / NUM_BANDS / 2), &error);
    bs2->iface->read(bs2->data, &band_2[MEMSIZE / NUM_BANDS / 2], (MEMSIZE
            / NUM_BANDS / 2), &error);
    all->iface->read(all->data, all_bands, MEMSIZE, &error);

    TEST_ASSERT_EQ_STR(band_0, BAND1);
    TEST_ASSERT_EQ_STR(band_1, BAND2);
    TEST_ASSERT_EQ_STR(band_2, BAND3);
    TEST_ASSERT_EQ_STR(all_bands, MEMBUF);

    NITF_FREE(band_0);
    NITF_FREE(band_1);
    NITF_FREE(band_2);
    NITF_FREE(all_bands);
    nitf_BandSource_destruct(&bs0);
    nitf_BandSource_destruct(&bs1);
    nitf_BandSource_destruct(&bs2);

    TEST_ASSERT_NULL(bs0);
    TEST_ASSERT_NULL(bs1);
    TEST_ASSERT_NULL(bs2);
}

int main(int argc, char **argv)
{
    CHECK(testMemorySource);
    return 0;
}
