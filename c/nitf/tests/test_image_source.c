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

nitf_IOHandle prepare_io()
{
    /*  First we'll create the file for them...  */
    nitf_Error error;
    nitf_IOHandle handle = nitf_IOHandle_create(FILENM, NITF_ACCESS_WRITEONLY, NITF_CREATE, &error);
    if (NITF_INVALID_HANDLE(handle))
    {
        nitf_Error_print(&error, stdout, "Quitting!");
        exit(EXIT_FAILURE);
    }
    /*  And we'll write our buffer out  */
    nitf_IOHandle_write(handle, MEMBUF, MEMSIZE, &error);
    nitf_IOHandle_close(handle);


    /*  Now we'll reopen in readonly fashion  */
    handle = nitf_IOHandle_create(FILENM, NITF_ACCESS_READONLY, NITF_CREATE, &error);
    if (NITF_INVALID_HANDLE(handle))
    {
        nitf_Error_print(&error, stdout, "Quitting!");
        exit(EXIT_FAILURE);
    }
    return handle;
}

int main(int argc, char **argv)
{
    /*  Get the error object       */
    nitf_Error     error;
    int bandSize = MEMSIZE / NUM_BANDS;
    nitf_IOHandle handle = prepare_io();
    nitf_ImageSource* is = nitf_ImageSource_construct(&error);
    char *band_0; char* band_1; char* band_2;

    int numBytesPerPix = 1;

    /*  Construct the band sources  */


    nitf_BandSource* bs0 = nitf_FileSource_construct(handle, 0, numBytesPerPix,
                           NUM_BANDS - 1, &error);
    nitf_BandSource* bs1 = nitf_FileSource_construct(handle, 1, numBytesPerPix,
                           NUM_BANDS - 1, &error);
    nitf_BandSource* bs2 = nitf_FileSource_construct(handle, 2, numBytesPerPix,
                           NUM_BANDS - 1, &error);


    assert( nitf_IOHandle_getSize(handle, &error) == MEMSIZE );

    if (!is) goto CATCH_ERROR;

    assert( nitf_ImageSource_addBand(is, bs0, &error));
    assert( nitf_ImageSource_addBand(is, bs1, &error));
    assert( nitf_ImageSource_addBand(is, bs2, &error));

    /*  Construct in memory band buffers -- for testing -- 0 terminate strings */
    band_0 = (char*)NITF_MALLOC(bandSize + 1); band_0[bandSize] = 0;
    band_1 = (char*)NITF_MALLOC(bandSize + 1); band_1[bandSize] = 1;
    band_2 = (char*)NITF_MALLOC(bandSize + 1); band_2[bandSize] = 2;


    bs0 = nitf_ImageSource_getBand(is, 0, &error); assert(bs0);
    bs1 = nitf_ImageSource_getBand(is, 1, &error); assert(bs1);
    bs2 = nitf_ImageSource_getBand(is, 2, &error); assert(bs2);

    /*  Read half of the info for one band.  This makes sure that we  */
    /*  are capable of picking up where we left off                   */
    bs0->iface->read(bs0->data, band_0, (MEMSIZE / NUM_BANDS / 2), &error);
    bs1->iface->read(bs1->data, band_1, (MEMSIZE / NUM_BANDS / 2), &error);
    bs2->iface->read(bs2->data, band_2, (MEMSIZE / NUM_BANDS / 2), &error);
    print_band(band_0, "1: half", MEMSIZE / NUM_BANDS / 2);
    print_band(band_1, "2: half", MEMSIZE / NUM_BANDS / 2);
    print_band(band_2, "3: half", MEMSIZE / NUM_BANDS / 2);
    /*  Pick up where we left off and keep going  */
    bs0->iface->read(bs0->data, &band_0[MEMSIZE / NUM_BANDS / 2], (MEMSIZE / NUM_BANDS / 2), &error);
    bs1->iface->read(bs1->data, &band_1[MEMSIZE / NUM_BANDS / 2], (MEMSIZE / NUM_BANDS / 2), &error);
    bs2->iface->read(bs2->data, &band_2[MEMSIZE / NUM_BANDS / 2], (MEMSIZE / NUM_BANDS / 2), &error);

    /*  Now we would like to verify the results of our reading  */

    /*  The first three bands should be all of the same letter B1=A, B2=B, B3=C*/
    print_band(band_0, "1", MEMSIZE / NUM_BANDS);
    print_band(band_1, "2", MEMSIZE / NUM_BANDS);
    print_band(band_2, "3", MEMSIZE / NUM_BANDS);



    NITF_FREE(band_0);
    NITF_FREE(band_1);
    NITF_FREE(band_2);

    nitf_BandSource_destruct(&bs0);
    nitf_BandSource_destruct(&bs1);
    nitf_BandSource_destruct(&bs2);
    return 0;

CATCH_ERROR:
    nitf_Error_print(&error, stdout, "Quitting");
    exit(EXIT_FAILURE);

}
