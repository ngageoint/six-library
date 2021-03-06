/* =========================================================================
 * This file is part of NITRO
 * =========================================================================
 *
 * (C) Copyright 2004 - 2019, MDA Information Systems LLC
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

#include "Test.h"
#include <import/nitf.h>

typedef struct TestSpec
{
    // Image spec
    const char* imageMode;
    uint32_t bitsPerPixel;
    char* pixels;
    uint64_t imageSize;
    uint32_t numBands;

    // Read request
    uint32_t startRow;
    uint32_t numRows;
    uint32_t startCol;
    uint32_t numCols;

    const char* expectedRead;
} TestSpec;

// Hold onto any dynamically allocated objects for a test,
// so we can pull all the initialization up into one function,
// but still hold onto everything to free it
typedef struct TestState
{
    nitf_ImageSubheader* subheader;
    nitf_ImageIO* imageIO;
    nitf_IOInterface* interface;
    nitf_SubWindow* subwindow;
    uint32_t numBands;
    uint32_t* bandList;
    nitf_BandInfo** band;
} TestState;

#define NUM_ROWS 16
#define NUM_COLS 16
#define ROWS_PER_BLOCK 4
#define COLS_PER_BLOCK 4

TestState* constructTestSubheader(TestSpec* spec)
{
    nitf_Error error;

    TestState* state = (TestState*)malloc(sizeof(TestState));
    assert(state);

    nitf_ImageSubheader *subheader = nitf_ImageSubheader_construct(&error);
    assert(subheader);

    nitf_ImageSubheader_setBlocking(subheader,
                                    NUM_ROWS, NUM_COLS,
                                    ROWS_PER_BLOCK, COLS_PER_BLOCK,
                                    spec->imageMode,
                                    &error);

    void* band_ = malloc(sizeof(nitf_BandInfo) * spec->numBands);
    state->band =
#ifdef __cplusplus
        static_cast<nitf_BandInfo**>(band_);
#else
        band_;
#endif
    assert(state->band);

    uint32_t band;
    for (band = 0; band < spec->numBands; ++band)
    {
        state->band[band] = nitf_BandInfo_construct(&error);
        assert(state->band[band]);

        if (!nitf_BandInfo_init(state->band[band],
                                "M", " ", "N", "   ", 0, 0, NULL, &error))
        {
            exit(1);
        }
    }

    nitf_ImageSubheader_setPixelInformation(
            subheader,
            "INT",
            spec->bitsPerPixel, spec->bitsPerPixel,
            "R",
            "MONO",
            "VIS",
            spec->numBands,
            state->band,
            &error);
    nitf_ImageSubheader_setCompression(subheader, "NC", "", &error);


    nitf_ImageIO* imageIO = nitf_ImageIO_construct(
            subheader, 0, spec->imageSize, NULL, NULL, NULL, &error);
    assert(imageIO);

    nitf_IOInterface *io = nitf_BufferAdapter_construct(
            spec->pixels, spec->imageSize, 0, &error);
    assert(io);

    nitf_SubWindow *subwindow = nitf_SubWindow_construct(&error);
    assert(subwindow);
    subwindow->startRow = spec->startRow;
    subwindow->numRows = spec->numRows;
    subwindow->startCol = spec->startCol;
    subwindow->numCols = spec->numCols;

    state->subheader = subheader;
    state->imageIO = imageIO;
    state->interface = io;
    state->subwindow = subwindow;
    state->numBands = spec->numBands;
    void* bandList_ = malloc(state->numBands * sizeof(uint32_t));
    state->bandList =
#ifdef __cplusplus
        static_cast<uint32_t*>(bandList_);
#else
        bandList_;
#endif
    for (band = 0; band < state->numBands; ++band)
    {
        state->bandList[band] = band;
    }
    assert(state->bandList);
    subwindow->numBands = state->numBands;
    subwindow->bandList = state->bandList;
    return state;
}

void freeTestState(TestState* state)
{
    nitf_SubWindow_destruct(&state->subwindow);
    nitf_IOInterface_destruct(&state->interface);
    nitf_ImageIO_destruct(&state->imageIO);
    nitf_ImageSubheader_destruct(&state->subheader);
    free(state->bandList);
    free(state);
}

void freeBands(uint8_t** bands, size_t numBands)
{
    size_t bandIndex;
    for (bandIndex = 0; bandIndex < numBands; ++bandIndex)
    {
        if (bands[bandIndex] != NULL)
        {
            free(bands[bandIndex]);
        }
    }
    free(bands);
}

static uint8_t** allocateBands(size_t numBands, size_t bytesPerBand)
{
    size_t bandIndex;
    int success = 1;

    uint8_t** bands = (uint8_t**)calloc(numBands, sizeof(uint8_t *));
    if (bands == NULL)
    {
        return NULL;
    }

    for (bandIndex = 0; bandIndex < numBands; ++bandIndex)
    {
        bands[bandIndex] = (uint8_t*)calloc(sizeof(uint8_t), bytesPerBand + 1);
        if (bands[bandIndex] == NULL)
        {
            success = 0;
        }
    }

    /* Free memory if we failed */
    if (success == 0)
    {
        freeBands(bands, numBands);
        bands = NULL;
    }

    return bands;
}

static NITF_BOOL doReadTest(TestSpec* spec, TestState* test)
{
    NITF_BOOL result = NITF_SUCCESS;

    const uint32_t numBands = test->subwindow->numBands;
    const size_t bandSize = strlen(spec->expectedRead) / numBands;
    uint8_t** bands = allocateBands(numBands, bandSize);
    if (bands == NULL)
    {
        return NITF_FAILURE;
    }

    nitf_Error error;
    int padded;
    nitf_ImageIO_read(test->imageIO, test->interface, test->subwindow,
                      bands, &padded, &error);

    const size_t joinedBands_sz = strlen(spec->expectedRead) + 1;
    void* joinedBands_ = malloc(joinedBands_sz);
    char* joinedBands =
#ifdef __cplusplus
        static_cast<char*>(joinedBands_);
#else
        joinedBands_;
#endif
    if (joinedBands)
    {
        nrt_strcpy_s(joinedBands, joinedBands_sz, (const char*) bands[0]);
        uint32_t bandIdx;
        for (bandIdx = 1; bandIdx < numBands; ++bandIdx)
        {
            nrt_strcat_s(joinedBands, joinedBands_sz, (const char*) bands[bandIdx]);
        }
        if (strcmp((char *)joinedBands, spec->expectedRead) != 0)
        {
            result = NITF_FAILURE;
        }
        free(joinedBands);
    }

    if (bands != NULL)
    {
        freeBands(bands, numBands);
    }

    return result;
}

static NITF_BOOL roundTripTest(TestSpec* spec, TestState* test, char* pixels)
{
    NITF_BOOL result = NITF_SUCCESS;
    nitf_Error error;
    const uint32_t numBands = test->subwindow->numBands;
    nitf_IOInterface* writeIO = NULL;

    const size_t bandSize = strlen(spec->expectedRead) / numBands;
    uint8_t** bands = allocateBands(numBands, bandSize);
    if (bands == NULL)
    {
        return NITF_FAILURE;
    }

    int padded;
    nitf_ImageIO_read(test->imageIO, test->interface, test->subwindow,
                      bands, &padded, &error);

    const size_t bufferSize = strlen(pixels);
    char* buf = (char*)calloc(bufferSize + 1, sizeof(char));
    writeIO = nrt_BufferAdapter_construct(buf, bufferSize, 1, &error);

    if (writeIO != NULL)
    {
        nitf_ImageIO_writeSequential(test->imageIO, writeIO, &error);
        nitf_ImageIO_writeRows(test->imageIO, writeIO, NUM_ROWS, bands, &error);
        nitf_ImageIO_writeDone(test->imageIO, writeIO, &error);

        if (strcmp(buf, pixels) != 0)
        {
            result = NITF_FAILURE;
        }

        nitf_IOInterface_destruct(&writeIO);
    }

    if (bands != NULL)
    {
        freeBands(bands, numBands);
    }

    return result;
}

TEST_CASE(testPBlockOneBand)
{
    /* For image mode P, the pixels are stored one block
     * at a time. This means that the first row of pixels
     * (AAAABBBBCCCCDDDD) is actually the first block of
     * the image. We can find the actual first image row
     * by stitching together the first (numRows / numBlocksPerRow)
     * blocks and taking the first row of that.
     * In this case, "AAAAAAAAAAAAAAAA"
     */
#define NUM_BANDS 1
#define pixels \
        "AAAABBBBCCCCDDDD" \
        "AAAABBBBCCCCDDDD" \
        "AAAABBBBCCCCDDDD" \
        "AAAABBBBCCCCDDDD" \
        "EEEEFFFFGGGGHHHH" \
        "EEEEFFFFGGGGHHHH" \
        "EEEEFFFFGGGGHHHH" \
        "EEEEFFFFGGGGHHHH" \
        "IIIIJJJJKKKKLLLL" \
        "IIIIJJJJKKKKLLLL" \
        "IIIIJJJJKKKKLLLL" \
        "IIIIJJJJKKKKLLLL" \
        "MMMMNNNNOOOOPPPP" \
        "MMMMNNNNOOOOPPPP" \
        "MMMMNNNNOOOOPPPP" \
        "MMMMNNNNOOOOPPPP"

    TestSpec pTypeTests[] =
    {
        {
            "P",
            8,
            pixels,
            sizeof(pixels),
            NUM_BANDS,

            0, 2,
            4, 4,

            "AAAABBBB"
        },
        {
            "P",
            8,
            pixels,
            sizeof(pixels),
            NUM_BANDS,

            0, 4,
            0, NUM_COLS,

            "AAAAAAAAAAAAAAAA"
            "BBBBBBBBBBBBBBBB"
            "CCCCCCCCCCCCCCCC"
            "DDDDDDDDDDDDDDDD"
        },
        {
            "P",
            8,
            pixels,
            sizeof(pixels),
            NUM_BANDS,

            0, NUM_ROWS,
            0, NUM_COLS,

            "AAAAAAAAAAAAAAAA"
            "BBBBBBBBBBBBBBBB"
            "CCCCCCCCCCCCCCCC"
            "DDDDDDDDDDDDDDDD"
            "EEEEEEEEEEEEEEEE"
            "FFFFFFFFFFFFFFFF"
            "GGGGGGGGGGGGGGGG"
            "HHHHHHHHHHHHHHHH"
            "IIIIIIIIIIIIIIII"
            "JJJJJJJJJJJJJJJJ"
            "KKKKKKKKKKKKKKKK"
            "LLLLLLLLLLLLLLLL"
            "MMMMMMMMMMMMMMMM"
            "NNNNNNNNNNNNNNNN"
            "OOOOOOOOOOOOOOOO"
            "PPPPPPPPPPPPPPPP"
        }
    };

#undef NUM_BANDS
#undef pixels

    const size_t numTests = sizeof(pTypeTests) / sizeof(pTypeTests[0]);
    size_t testIndex;
    for (testIndex = 0; testIndex < numTests; ++testIndex)
    {
        TestSpec* spec = &pTypeTests[testIndex];
        TestState* test = constructTestSubheader(spec);

        TEST_ASSERT(doReadTest(spec, test));
        freeTestState(test);
    }
}

TEST_CASE(testPBlockTwoBands)
{
#define NUM_BANDS 2
#define pixels \
        "AaAaAaAaBbBbBbBbCcCcCcCcDdDdDdDd"\
        "AaAaAaAaBbBbBbBbCcCcCcCcDdDdDdDd"\
        "AaAaAaAaBbBbBbBbCcCcCcCcDdDdDdDd"\
        "AaAaAaAaBbBbBbBbCcCcCcCcDdDdDdDd"\
        "EeEeEeEeFfFfFfFfGgGgGgGgHhHhHhHh"\
        "EeEeEeEeFfFfFfFfGgGgGgGgHhHhHhHh"\
        "EeEeEeEeFfFfFfFfGgGgGgGgHhHhHhHh"\
        "EeEeEeEeFfFfFfFfGgGgGgGgHhHhHhHh"\
        "IiIiIiIiJjJjJjJjKkKkKkKkLlLlLlLl"\
        "IiIiIiIiJjJjJjJjKkKkKkKkLlLlLlLl"\
        "IiIiIiIiJjJjJjJjKkKkKkKkLlLlLlLl"\
        "IiIiIiIiJjJjJjJjKkKkKkKkLlLlLlLl"\
        "MmMmMmMmNnNnNnNnOoOoOoOoPpPpPpPp"\
        "MmMmMmMmNnNnNnNnOoOoOoOoPpPpPpPp"\
        "MmMmMmMmNnNnNnNnOoOoOoOoPpPpPpPp"\
        "MmMmMmMmNnNnNnNnOoOoOoOoPpPpPpPp"

    TestSpec pTypeTests[] =
    {

        {
            "P",
            8,
            pixels,
            sizeof(pixels),
            NUM_BANDS,

            0, 2,
            4, 4,

            "AAAABBBBaaaabbbb"
        },
        {
            "P",
            8,
            pixels,
            sizeof(pixels),
            NUM_BANDS,

            0, 4,
            0, NUM_COLS,

            "AAAAAAAAAAAAAAAA"
            "BBBBBBBBBBBBBBBB"
            "CCCCCCCCCCCCCCCC"
            "DDDDDDDDDDDDDDDD"
            "aaaaaaaaaaaaaaaa"
            "bbbbbbbbbbbbbbbb"
            "cccccccccccccccc"
            "dddddddddddddddd"
        },
        {
            "P",
            8,
            pixels,
            sizeof(pixels),
            NUM_BANDS,

            0, NUM_ROWS,
            0, NUM_COLS,

            "AAAAAAAAAAAAAAAA"
            "BBBBBBBBBBBBBBBB"
            "CCCCCCCCCCCCCCCC"
            "DDDDDDDDDDDDDDDD"
            "EEEEEEEEEEEEEEEE"
            "FFFFFFFFFFFFFFFF"
            "GGGGGGGGGGGGGGGG"
            "HHHHHHHHHHHHHHHH"
            "IIIIIIIIIIIIIIII"
            "JJJJJJJJJJJJJJJJ"
            "KKKKKKKKKKKKKKKK"
            "LLLLLLLLLLLLLLLL"
            "MMMMMMMMMMMMMMMM"
            "NNNNNNNNNNNNNNNN"
            "OOOOOOOOOOOOOOOO"
            "PPPPPPPPPPPPPPPP"
            "aaaaaaaaaaaaaaaa"
            "bbbbbbbbbbbbbbbb"
            "cccccccccccccccc"
            "dddddddddddddddd"
            "eeeeeeeeeeeeeeee"
            "ffffffffffffffff"
            "gggggggggggggggg"
            "hhhhhhhhhhhhhhhh"
            "iiiiiiiiiiiiiiii"
            "jjjjjjjjjjjjjjjj"
            "kkkkkkkkkkkkkkkk"
            "llllllllllllllll"
            "mmmmmmmmmmmmmmmm"
            "nnnnnnnnnnnnnnnn"
            "oooooooooooooooo"
            "pppppppppppppppp"
        }
    };

    const size_t numTests = sizeof(pTypeTests) / sizeof(pTypeTests[0]);
    size_t testIndex;
    for (testIndex = 0; testIndex < numTests; ++testIndex)
    {
        TestSpec* spec = &pTypeTests[testIndex];
        TestState* test = constructTestSubheader(spec);

        TEST_ASSERT(doReadTest(spec, test));
        freeTestState(test);
    }
#undef NUM_BANDS
#undef pixels
}


TEST_CASE(testTwoBandRoundTrip)
{
#define NUM_BANDS 2
#define pixels \
        "AaAaAaAaBbBbBbBbCcCcCcCcDdDdDdDd" \
        "AaAaAaAaBbBbBbBbCcCcCcCcDdDdDdDd" \
        "AaAaAaAaBbBbBbBbCcCcCcCcDdDdDdDd" \
        "AaAaAaAaBbBbBbBbCcCcCcCcDdDdDdDd" \
        "EeEeEeEeFfFfFfFfGgGgGgGgHhHhHhHh" \
        "EeEeEeEeFfFfFfFfGgGgGgGgHhHhHhHh" \
        "EeEeEeEeFfFfFfFfGgGgGgGgHhHhHhHh" \
        "EeEeEeEeFfFfFfFfGgGgGgGgHhHhHhHh" \
        "IiIiIiIiJjJjJjJjKkKkKkKkLlLlLlLl" \
        "IiIiIiIiJjJjJjJjKkKkKkKkLlLlLlLl" \
        "IiIiIiIiJjJjJjJjKkKkKkKkLlLlLlLl" \
        "IiIiIiIiJjJjJjJjKkKkKkKkLlLlLlLl" \
        "MmMmMmMmNnNnNnNnOoOoOoOoPpPpPpPp" \
        "MmMmMmMmNnNnNnNnOoOoOoOoPpPpPpPp" \
        "MmMmMmMmNnNnNnNnOoOoOoOoPpPpPpPp" \
        "MmMmMmMmNnNnNnNnOoOoOoOoPpPpPpPp"

    TestSpec spec =
    {
        "P",
        8,
        pixels,
        sizeof(pixels),
        NUM_BANDS,

        0, NUM_ROWS,
        0, NUM_COLS,

        "AAAAAAAAAAAAAAAA"
        "BBBBBBBBBBBBBBBB"
        "CCCCCCCCCCCCCCCC"
        "DDDDDDDDDDDDDDDD"
        "EEEEEEEEEEEEEEEE"
        "FFFFFFFFFFFFFFFF"
        "GGGGGGGGGGGGGGGG"
        "HHHHHHHHHHHHHHHH"
        "IIIIIIIIIIIIIIII"
        "JJJJJJJJJJJJJJJJ"
        "KKKKKKKKKKKKKKKK"
        "LLLLLLLLLLLLLLLL"
        "MMMMMMMMMMMMMMMM"
        "NNNNNNNNNNNNNNNN"
        "OOOOOOOOOOOOOOOO"
        "PPPPPPPPPPPPPPPP"
        "aaaaaaaaaaaaaaaa"
        "bbbbbbbbbbbbbbbb"
        "cccccccccccccccc"
        "dddddddddddddddd"
        "eeeeeeeeeeeeeeee"
        "ffffffffffffffff"
        "gggggggggggggggg"
        "hhhhhhhhhhhhhhhh"
        "iiiiiiiiiiiiiiii"
        "jjjjjjjjjjjjjjjj"
        "kkkkkkkkkkkkkkkk"
        "llllllllllllllll"
        "mmmmmmmmmmmmmmmm"
        "nnnnnnnnnnnnnnnn"
        "oooooooooooooooo"
        "pppppppppppppppp"
    };

    TestState* test = constructTestSubheader(&spec);
    TestSpec* pSpec = &spec;

    TEST_ASSERT(roundTripTest(pSpec, test, pixels));

#undef NUM_BANDS
#undef pixels
    freeTestState(test);
}

TEST_CASE(testPBlockOffsetBand)
{
#define NUM_BANDS 2
#define pixels \
        "AaAaAaAaBbBbBbBbCcCcCcCcDdDdDdDd" \
        "AaAaAaAaBbBbBbBbCcCcCcCcDdDdDdDd" \
        "AaAaAaAaBbBbBbBbCcCcCcCcDdDdDdDd" \
        "AaAaAaAaBbBbBbBbCcCcCcCcDdDdDdDd" \
        "EeEeEeEeFfFfFfFfGgGgGgGgHhHhHhHh" \
        "EeEeEeEeFfFfFfFfGgGgGgGgHhHhHhHh" \
        "EeEeEeEeFfFfFfFfGgGgGgGgHhHhHhHh" \
        "EeEeEeEeFfFfFfFfGgGgGgGgHhHhHhHh" \
        "IiIiIiIiJjJjJjJjKkKkKkKkLlLlLlLl" \
        "IiIiIiIiJjJjJjJjKkKkKkKkLlLlLlLl" \
        "IiIiIiIiJjJjJjJjKkKkKkKkLlLlLlLl" \
        "IiIiIiIiJjJjJjJjKkKkKkKkLlLlLlLl" \
        "MmMmMmMmNnNnNnNnOoOoOoOoPpPpPpPp" \
        "MmMmMmMmNnNnNnNnOoOoOoOoPpPpPpPp" \
        "MmMmMmMmNnNnNnNnOoOoOoOoPpPpPpPp" \
        "MmMmMmMmNnNnNnNnOoOoOoOoPpPpPpPp"
    TestSpec pTypeTests[] =
    {

        {
            "P",
            8,
            pixels,
            sizeof(pixels),
            NUM_BANDS,

            0, 2,
            4, 4,

            "aaaabbbb"
        },

        {
            "P",
            8,
            pixels,
            sizeof(pixels),
            NUM_BANDS,

            0, 4,
            0, NUM_COLS,

            "aaaaaaaaaaaaaaaa"
            "bbbbbbbbbbbbbbbb"
            "cccccccccccccccc"
            "dddddddddddddddd"
        },

        {
            "P",
            8,
            pixels,
            sizeof(pixels),
            NUM_BANDS,

            0, NUM_ROWS,
            0, NUM_COLS,

            "aaaaaaaaaaaaaaaa"
            "bbbbbbbbbbbbbbbb"
            "cccccccccccccccc"
            "dddddddddddddddd"
            "eeeeeeeeeeeeeeee"
            "ffffffffffffffff"
            "gggggggggggggggg"
            "hhhhhhhhhhhhhhhh"
            "iiiiiiiiiiiiiiii"
            "jjjjjjjjjjjjjjjj"
            "kkkkkkkkkkkkkkkk"
            "llllllllllllllll"
            "mmmmmmmmmmmmmmmm"
            "nnnnnnnnnnnnnnnn"
            "oooooooooooooooo"
            "pppppppppppppppp"
        }
    };
#undef NUM_BANDS
#undef pixels

    const size_t numTests = sizeof(pTypeTests) / sizeof(pTypeTests[0]);
    size_t testIndex;
    for (testIndex = 0; testIndex < numTests; ++testIndex)
    {
        TestSpec* spec = &pTypeTests[testIndex];
        TestState* test = constructTestSubheader(spec);

        /* Adjust subwindow to only read second band */
        test->subwindow->numBands = 1;
        test->subwindow->bandList = &test->bandList[1];

        TEST_ASSERT(doReadTest(spec, test));
        freeTestState(test);
    }
}


TEST_CASE(testPBlockThreeBandsWithOffset)
{
#define NUM_BANDS 3
#define pixels \
        "Aa1Aa1Aa1Aa1Bb2Bb2Bb2Bb2Cc3Cc3Cc3Cc3Dd4Dd4Dd4Dd4" \
        "Aa1Aa1Aa1Aa1Bb2Bb2Bb2Bb2Cc3Cc3Cc3Cc3Dd4Dd4Dd4Dd4" \
        "Aa1Aa1Aa1Aa1Bb2Bb2Bb2Bb2Cc3Cc3Cc3Cc3Dd4Dd4Dd4Dd4" \
        "Aa1Aa1Aa1Aa1Bb2Bb2Bb2Bb2Cc3Cc3Cc3Cc3Dd4Dd4Dd4Dd4" \
        "Ee5Ee5Ee5Ee5Ff6Ff6Ff6Ff6Gg7Gg7Gg7Gg7Hh8Hh8Hh8Hh8" \
        "Ee5Ee5Ee5Ee5Ff6Ff6Ff6Ff6Gg7Gg7Gg7Gg7Hh8Hh8Hh8Hh8" \
        "Ee5Ee5Ee5Ee5Ff6Ff6Ff6Ff6Gg7Gg7Gg7Gg7Hh8Hh8Hh8Hh8" \
        "Ee5Ee5Ee5Ee5Ff6Ff6Ff6Ff6Gg7Gg7Gg7Gg7Hh8Hh8Hh8Hh8" \
        "Ii9Ii9Ii9Ii9Jj0Jj0Jj0Jj0Kk!Kk!Kk!Kk!Ll@Ll@Ll@Ll@" \
        "Ii9Ii9Ii9Ii9Jj0Jj0Jj0Jj0Kk!Kk!Kk!Kk!Ll@Ll@Ll@Ll@" \
        "Ii9Ii9Ii9Ii9Jj0Jj0Jj0Jj0Kk!Kk!Kk!Kk!Ll@Ll@Ll@Ll@" \
        "Ii9Ii9Ii9Ii9Jj0Jj0Jj0Jj0Kk!Kk!Kk!Kk!Ll@Ll@Ll@Ll@" \
        "Mm#Mm#Mm#Mm#Nn$Nn$Nn$Nn$Oo%Oo%Oo%Oo%Pp^Pp^Pp^Pp^" \
        "Mm#Mm#Mm#Mm#Nn$Nn$Nn$Nn$Oo%Oo%Oo%Oo%Pp^Pp^Pp^Pp^" \
        "Mm#Mm#Mm#Mm#Nn$Nn$Nn$Nn$Oo%Oo%Oo%Oo%Pp^Pp^Pp^Pp^" \
        "Mm#Mm#Mm#Mm#Nn$Nn$Nn$Nn$Oo%Oo%Oo%Oo%Pp^Pp^Pp^Pp^"

    TestSpec pTypeTests[] =
    {
        {
            "P",
            8,
            pixels,
            sizeof(pixels),
            NUM_BANDS,

            0, 2,
            4, 4,

            "aaaabbbb11112222"
        },

        {
            "P",
            8,
            pixels,
            sizeof(pixels),
            NUM_BANDS,

            0, 4,
            0, NUM_COLS,

            "aaaaaaaaaaaaaaaa"
            "bbbbbbbbbbbbbbbb"
            "cccccccccccccccc"
            "dddddddddddddddd"
            "1111111111111111"
            "2222222222222222"
            "3333333333333333"
            "4444444444444444"
        },

        {
            "P",
            8,
            pixels,
            sizeof(pixels),
            NUM_BANDS,

            0, NUM_ROWS,
            0, NUM_COLS,

            "aaaaaaaaaaaaaaaa"
            "bbbbbbbbbbbbbbbb"
            "cccccccccccccccc"
            "dddddddddddddddd"
            "eeeeeeeeeeeeeeee"
            "ffffffffffffffff"
            "gggggggggggggggg"
            "hhhhhhhhhhhhhhhh"
            "iiiiiiiiiiiiiiii"
            "jjjjjjjjjjjjjjjj"
            "kkkkkkkkkkkkkkkk"
            "llllllllllllllll"
            "mmmmmmmmmmmmmmmm"
            "nnnnnnnnnnnnnnnn"
            "oooooooooooooooo"
            "pppppppppppppppp"
            "1111111111111111"
            "2222222222222222"
            "3333333333333333"
            "4444444444444444"
            "5555555555555555"
            "6666666666666666"
            "7777777777777777"
            "8888888888888888"
            "9999999999999999"
            "0000000000000000"
            "!!!!!!!!!!!!!!!!"
            "@@@@@@@@@@@@@@@@"
            "################"
            "$$$$$$$$$$$$$$$$"
            "%%%%%%%%%%%%%%%%"
            "^^^^^^^^^^^^^^^^"
        }
    };
#undef NUM_BANDS
#undef pixels

    const size_t numTests = sizeof(pTypeTests) / sizeof(pTypeTests[0]);
    size_t testIndex;
    for (testIndex = 0; testIndex < numTests; ++testIndex)
    {
        TestSpec* spec = &pTypeTests[testIndex];
        TestState* test = constructTestSubheader(spec);

        test->subwindow->numBands = 2;
        test->subwindow->bandList = &test->bandList[1];

        TEST_ASSERT(doReadTest(spec, test));
        freeTestState(test);
    }
}

TEST_CASE(testInvalidReadOrderFailsGracefully)
{
#define NUM_BANDS 3
#define pixels \
        "Aa1Aa1Aa1Aa1Bb2Bb2Bb2Bb2Cc3Cc3Cc3Cc3Dd4Dd4Dd4Dd4"\
        "Aa1Aa1Aa1Aa1Bb2Bb2Bb2Bb2Cc3Cc3Cc3Cc3Dd4Dd4Dd4Dd4"\
        "Aa1Aa1Aa1Aa1Bb2Bb2Bb2Bb2Cc3Cc3Cc3Cc3Dd4Dd4Dd4Dd4"\
        "Aa1Aa1Aa1Aa1Bb2Bb2Bb2Bb2Cc3Cc3Cc3Cc3Dd4Dd4Dd4Dd4"\
        "Ee5Ee5Ee5Ee5Ff6Ff6Ff6Ff6Gg7Gg7Gg7Gg7Hh8Hh8Hh8Hh8"\
        "Ee5Ee5Ee5Ee5Ff6Ff6Ff6Ff6Gg7Gg7Gg7Gg7Hh8Hh8Hh8Hh8"\
        "Ee5Ee5Ee5Ee5Ff6Ff6Ff6Ff6Gg7Gg7Gg7Gg7Hh8Hh8Hh8Hh8"\
        "Ee5Ee5Ee5Ee5Ff6Ff6Ff6Ff6Gg7Gg7Gg7Gg7Hh8Hh8Hh8Hh8"\
        "Ii9Ii9Ii9Ii9Jj0Jj0Jj0Jj0Kk!Kk!Kk!Kk!Ll@Ll@Ll@Ll@"\
        "Ii9Ii9Ii9Ii9Jj0Jj0Jj0Jj0Kk!Kk!Kk!Kk!Ll@Ll@Ll@Ll@"\
        "Ii9Ii9Ii9Ii9Jj0Jj0Jj0Jj0Kk!Kk!Kk!Kk!Ll@Ll@Ll@Ll@"\
        "Ii9Ii9Ii9Ii9Jj0Jj0Jj0Jj0Kk!Kk!Kk!Kk!Ll@Ll@Ll@Ll@"\
        "Mm#Mm#Mm#Mm#Nn$Nn$Nn$Nn$Oo%Oo%Oo%Oo%Pp^Pp^Pp^Pp^"\
        "Mm#Mm#Mm#Mm#Nn$Nn$Nn$Nn$Oo%Oo%Oo%Oo%Pp^Pp^Pp^Pp^"\
        "Mm#Mm#Mm#Mm#Nn$Nn$Nn$Nn$Oo%Oo%Oo%Oo%Pp^Pp^Pp^Pp^"\
        "Mm#Mm#Mm#Mm#Nn$Nn$Nn$Nn$Oo%Oo%Oo%Oo%Pp^Pp^Pp^Pp^"\

    nitf_Error error;
    TestSpec spec =
    {
        "P",
        8,
        pixels,
        sizeof(pixels),
        NUM_BANDS,

        0, 2,
        4, 4,

        ""
    };
#undef NUM_BANDS
#undef pixels

    TestState* test = constructTestSubheader(&spec);

    test->subwindow->numBands = 2;
    test->subwindow->bandList = &test->bandList[1];
    test->subwindow->bandList[0] = 2;
    test->subwindow->bandList[1] = 0;

    int padded;
    uint8_t* user[2] = { 0, 0 };
    TEST_ASSERT(!nitf_ImageIO_read(test->imageIO, test->interface, test->subwindow,
                user, &padded, &error));
    freeTestState(test);
}

TEST_CASE(testPBlock4BytePixels)
{
#define NUM_BANDS 2
#define pixels \
        "**Aa**Aa**Aa**Aa**Bb**Bb**Bb**Bb**Cc**Cc**Cc**Cc**Dd**Dd**Dd**Dd"\
        "**Aa**Aa**Aa**Aa**Bb**Bb**Bb**Bb**Cc**Cc**Cc**Cc**Dd**Dd**Dd**Dd"\
        "**Aa**Aa**Aa**Aa**Bb**Bb**Bb**Bb**Cc**Cc**Cc**Cc**Dd**Dd**Dd**Dd"\
        "**Aa**Aa**Aa**Aa**Bb**Bb**Bb**Bb**Cc**Cc**Cc**Cc**Dd**Dd**Dd**Dd"\
        "**Ee**Ee**Ee**Ee**Ff**Ff**Ff**Ff**Gg**Gg**Gg**Gg**Hh**Hh**Hh**Hh"\
        "**Ee**Ee**Ee**Ee**Ff**Ff**Ff**Ff**Gg**Gg**Gg**Gg**Hh**Hh**Hh**Hh"\
        "**Ee**Ee**Ee**Ee**Ff**Ff**Ff**Ff**Gg**Gg**Gg**Gg**Hh**Hh**Hh**Hh"\
        "**Ee**Ee**Ee**Ee**Ff**Ff**Ff**Ff**Gg**Gg**Gg**Gg**Hh**Hh**Hh**Hh"\
        "**Ii**Ii**Ii**Ii**Jj**Jj**Jj**Jj**Kk**Kk**Kk**Kk**Ll**Ll**Ll**Ll"\
        "**Ii**Ii**Ii**Ii**Jj**Jj**Jj**Jj**Kk**Kk**Kk**Kk**Ll**Ll**Ll**Ll"\
        "**Ii**Ii**Ii**Ii**Jj**Jj**Jj**Jj**Kk**Kk**Kk**Kk**Ll**Ll**Ll**Ll"\
        "**Ii**Ii**Ii**Ii**Jj**Jj**Jj**Jj**Kk**Kk**Kk**Kk**Ll**Ll**Ll**Ll"\
        "**Mm**Mm**Mm**Mm**Nn**Nn**Nn**Nn**Oo**Oo**Oo**Oo**Pp**Pp**Pp**Pp"\
        "**Mm**Mm**Mm**Mm**Nn**Nn**Nn**Nn**Oo**Oo**Oo**Oo**Pp**Pp**Pp**Pp"\
        "**Mm**Mm**Mm**Mm**Nn**Nn**Nn**Nn**Oo**Oo**Oo**Oo**Pp**Pp**Pp**Pp"\
        "**Mm**Mm**Mm**Mm**Nn**Nn**Nn**Nn**Oo**Oo**Oo**Oo**Pp**Pp**Pp**Pp"\

    TestSpec pTypeTests[] =
    {
        {
            "P",
            16,
            pixels,
            sizeof(pixels),
            NUM_BANDS,

            0, NUM_ROWS,
            0, NUM_COLS,

            "aAaAaAaAaAaAaAaAaAaAaAaAaAaAaAaA"
            "bBbBbBbBbBbBbBbBbBbBbBbBbBbBbBbB"
            "cCcCcCcCcCcCcCcCcCcCcCcCcCcCcCcC"
            "dDdDdDdDdDdDdDdDdDdDdDdDdDdDdDdD"
            "eEeEeEeEeEeEeEeEeEeEeEeEeEeEeEeE"
            "fFfFfFfFfFfFfFfFfFfFfFfFfFfFfFfF"
            "gGgGgGgGgGgGgGgGgGgGgGgGgGgGgGgG"
            "hHhHhHhHhHhHhHhHhHhHhHhHhHhHhHhH"
            "iIiIiIiIiIiIiIiIiIiIiIiIiIiIiIiI"
            "jJjJjJjJjJjJjJjJjJjJjJjJjJjJjJjJ"
            "kKkKkKkKkKkKkKkKkKkKkKkKkKkKkKkK"
            "lLlLlLlLlLlLlLlLlLlLlLlLlLlLlLlL"
            "mMmMmMmMmMmMmMmMmMmMmMmMmMmMmMmM"
            "nNnNnNnNnNnNnNnNnNnNnNnNnNnNnNnN"
            "oOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoO"
            "pPpPpPpPpPpPpPpPpPpPpPpPpPpPpPpP"
        }
    };

#undef NUM_BANDS
#undef pixels
    const size_t numTests = sizeof(pTypeTests) / sizeof(pTypeTests[0]);
    size_t testIndex;
    for (testIndex = 0; testIndex < numTests; ++testIndex)
    {
        TestSpec* spec = &pTypeTests[testIndex];
        TestState* test = constructTestSubheader(spec);

        /* Adjust subwindow to only read second band */
        test->subwindow->numBands = 1;
        test->subwindow->bandList = &test->bandList[1];

        TEST_ASSERT(doReadTest(spec, test));
        freeTestState(test);
    }
}

TEST_MAIN(
    (void)argc;
    (void)argv;

    CHECK(testPBlockOneBand);
    CHECK(testPBlockTwoBands);
    CHECK(testPBlockOffsetBand);
    CHECK(testPBlockThreeBandsWithOffset);
    CHECK(testInvalidReadOrderFailsGracefully);
    CHECK(testPBlock4BytePixels);
    CHECK(testTwoBandRoundTrip);
    )
