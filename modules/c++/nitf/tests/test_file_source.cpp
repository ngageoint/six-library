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


#include <import/nitf.hpp>

#define MEMBUF "ABCABCABCABCABCABCABCABCABCABCABCABCABCABCABCABC"
#define MEMSIZE strlen(MEMBUF)
#define NUM_BANDS 3
#define FILENM "test_file.src"
void print_band(char* band, const char* s, int size)
{
    int i;
    printf("Band %s: [", s);
    for (i = 0; i < size; i++)
    {
        printf("%c", band[i]);
    }
    printf("]\n");
}


std::string prepare_io()
{
    /*  First we'll create the file for them...  */
    nitf::IOHandle handle(FILENM, NITF_ACCESS_WRITEONLY, NITF_CREATE);

    /*  And we'll write our buffer out  */
    handle.write(MEMBUF, MEMSIZE);
    handle.close();

    return std::string(FILENM);
}


int main(int argc, char **argv)
{
    /*  Get the error object       */
    int bandSize = MEMSIZE / NUM_BANDS;
    std::string fname = prepare_io();

    int numBytesPerPix = 1;

    /*  Construct the band sources  */
    nitf::BandSource* bs0 = new nitf::FileSource(fname, 0, numBytesPerPix, NUM_BANDS - 1);
    nitf::BandSource* bs1 = new nitf::FileSource(fname, 1, numBytesPerPix, NUM_BANDS - 1);
    nitf::BandSource* bs2 = new nitf::FileSource(fname, 2, numBytesPerPix, NUM_BANDS - 1);
    nitf::BandSource* all = new nitf::FileSource(fname, 0, numBytesPerPix, 0);

    /*  Construct in memory band buffers -- for testing -- 0 terminate strings */
    char *band_0 = (char*)NITF_MALLOC(bandSize + 1);
    char *band_1 = (char*)NITF_MALLOC(bandSize + 1);
    char *band_2 = (char*)NITF_MALLOC(bandSize + 1);
    char *all_bands = (char*)NITF_MALLOC( MEMSIZE + 1);

    band_0[bandSize] = 0;
    band_1[bandSize] = 1;
    band_2[bandSize] = 2;
    all_bands[ MEMSIZE ] = 0;

    /*  Read half of the info for one band.  This makes sure that we  */
    /*  are capable of picking up where we left off                   */
    bs0->read(band_0, (MEMSIZE / NUM_BANDS / 2));
    bs1->read(band_1, (MEMSIZE / NUM_BANDS / 2));
    bs2->read(band_2, (MEMSIZE / NUM_BANDS / 2));

    /*  Pick up where we left off and keep going  */
    bs0->read(&band_0[MEMSIZE / NUM_BANDS / 2], (MEMSIZE / NUM_BANDS / 2));
    bs1->read(&band_1[MEMSIZE / NUM_BANDS / 2], (MEMSIZE / NUM_BANDS / 2));
    bs2->read(&band_2[MEMSIZE / NUM_BANDS / 2], (MEMSIZE / NUM_BANDS / 2));
    all->read(all_bands, MEMSIZE);

    /*  Now we would like to verify the results of our reading  */

    /*  The first three bands should be all of the same letter B1=A, B2=B, B3=C*/
    print_band(band_0, "1", MEMSIZE / NUM_BANDS);
    print_band(band_1, "2", MEMSIZE / NUM_BANDS);
    print_band(band_2, "3", MEMSIZE / NUM_BANDS);

    /*  The last band source was applied to the entire buffer, so it should  */
    /*  look the same as the original memory source                          */
    print_band(all_bands, "ALL", MEMSIZE);

    NITF_FREE(band_0);
    NITF_FREE(band_1);
    NITF_FREE(band_2);
    NITF_FREE(all_bands);
    delete bs0;
    delete bs1;
    delete bs2;
    return 0;
}
