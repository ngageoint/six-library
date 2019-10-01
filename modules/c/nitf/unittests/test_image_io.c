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
    const char* pixelValueType;
    nitf_Uint32 bitsPerPixel;
    const char* imageRepresentation;
    char* pixels;
    nitf_Uint64 imageSize;
    size_t numBands;

    // Read request
    nitf_Uint32 startRow;
    nitf_Uint32 numRows;
    nitf_Uint32 startCol;
    nitf_Uint32 numCols;

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
    size_t numBands;
    nitf_Uint32* bandList;
    nitf_BandInfo** band;
} TestState;

const nitf_Uint32 NUM_ROWS = 16;
const nitf_Uint32 NUM_COLS = 16;
const nitf_Uint32 ROWS_PER_BLOCK = 4;
const nitf_Uint32 COLS_PER_BLOCK = 4;

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

    state->band = malloc(sizeof(nitf_BandInfo) * spec->numBands);
    assert(state->band);

    size_t band;
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
            spec->pixelValueType,
            spec->bitsPerPixel, spec->bitsPerPixel,
            "R",
            spec->imageRepresentation,
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
    state->bandList = malloc(state->numBands * sizeof(nitf_Uint32));
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
    char pixels[] =
    {
        "AAAABBBBCCCCDDDD"
        "AAAABBBBCCCCDDDD"
        "AAAABBBBCCCCDDDD"
        "AAAABBBBCCCCDDDD"
        "EEEEFFFFGGGGHHHH"
        "EEEEFFFFGGGGHHHH"
        "EEEEFFFFGGGGHHHH"
        "EEEEFFFFGGGGHHHH"
        "IIIIJJJJKKKKLLLL"
        "IIIIJJJJKKKKLLLL"
        "IIIIJJJJKKKKLLLL"
        "IIIIJJJJKKKKLLLL"
        "MMMMNNNNOOOOPPPP"
        "MMMMNNNNOOOOPPPP"
        "MMMMNNNNOOOOPPPP"
        "MMMMNNNNOOOOPPPP"
    };
    nitf_Error error;
    TestSpec pTypeTests[] =
    {
        {
            "P",
            "INT",
            8,
            "MONO",
            pixels,
            sizeof(pixels),
            1,

            0, 2,
            4, 4,

            "AAAABBBB"
        },
        {
            "P",
            "INT",
            8,
            "MONO",
            pixels,
            sizeof(pixels),
            1,

            0, 4,
            0, NUM_COLS,

            "AAAAAAAAAAAAAAAA"
            "BBBBBBBBBBBBBBBB"
            "CCCCCCCCCCCCCCCC"
            "DDDDDDDDDDDDDDDD"
        },
        {
            "P",
            "INT",
            8,
            "MONO",
            pixels,
            sizeof(pixels),
            1,

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

    const size_t numTests = sizeof(pTypeTests) / sizeof(pTypeTests[0]);
    size_t testIndex;
    for (testIndex = 0; testIndex < numTests; ++testIndex)
    {
        TestSpec* spec = &pTypeTests[testIndex];
        TestState* test = constructTestSubheader(spec);

        int padded;
        nitf_Uint8* user = (nitf_Uint8*)calloc(1, strlen(spec->expectedRead) + 1);
        TEST_ASSERT(user);
        nitf_ImageIO_read(test->imageIO, test->interface, test->subwindow,
                          &user, &padded, &error);

        TEST_ASSERT(strcmp((char *)user, spec->expectedRead) == 0);
        free(user);
        freeTestState(test);
    }
}

TEST_CASE(testPBlockTwoBands)
{
    static const size_t NUM_BANDS = 2;
    char pixels[] =
    {
        "AaAaAaAaBbBbBbBbCcCcCcCcDdDdDdDd"
        "AaAaAaAaBbBbBbBbCcCcCcCcDdDdDdDd"
        "AaAaAaAaBbBbBbBbCcCcCcCcDdDdDdDd"
        "AaAaAaAaBbBbBbBbCcCcCcCcDdDdDdDd"
        "EeEeEeEeFfFfFfFfGgGgGgGgHhHhHhHh"
        "EeEeEeEeFfFfFfFfGgGgGgGgHhHhHhHh"
        "EeEeEeEeFfFfFfFfGgGgGgGgHhHhHhHh"
        "EeEeEeEeFfFfFfFfGgGgGgGgHhHhHhHh"
        "IiIiIiIiJjJjJjJjKkKkKkKkLlLlLlLl"
        "IiIiIiIiJjJjJjJjKkKkKkKkLlLlLlLl"
        "IiIiIiIiJjJjJjJjKkKkKkKkLlLlLlLl"
        "IiIiIiIiJjJjJjJjKkKkKkKkLlLlLlLl"
        "MmMmMmMmNnNnNnNnOoOoOoOoPpPpPpPp"
        "MmMmMmMmNnNnNnNnOoOoOoOoPpPpPpPp"
        "MmMmMmMmNnNnNnNnOoOoOoOoPpPpPpPp"
        "MmMmMmMmNnNnNnNnOoOoOoOoPpPpPpPp"
    };
    nitf_Error error;
    TestSpec pTypeTests[] =
    {

        {
            "P",
            "INT",
            8,
            "NODISPLY",
            pixels,
            sizeof(pixels),
            NUM_BANDS,

            0, 2,
            4, 4,

            "AAAABBBBaaaabbbb"
        },
        {
            "P",
            "INT",
            8,
            "NODISPLY",
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
            "INT",
            8,
            "NODISPLY",
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

        int padded;
        nitf_Uint8* user[2];
        user[0] = (nitf_Uint8*)calloc(1, strlen(spec->expectedRead) + 1);
        user[1] = (nitf_Uint8*)calloc(1, strlen(spec->expectedRead) + 1);
        TEST_ASSERT(user[0]);
        TEST_ASSERT(user[1])
        nitf_ImageIO_read(test->imageIO, test->interface, test->subwindow,
                          user, &padded, &error);

        char *catUser = malloc(strlen(spec->expectedRead) + 1);
        strcpy(catUser, user[0]);
        strcat(catUser, user[1]);
        TEST_ASSERT(strcmp((char *)catUser, spec->expectedRead) == 0);
        free(user[0]);
        free(user[1]);
        free(catUser);
        freeTestState(test);
    }
}

TEST_CASE(testPBlockOffsetBand)
{
    static const size_t NUM_BANDS = 2;
    char pixels[] =
    {
        "AaAaAaAaBbBbBbBbCcCcCcCcDdDdDdDd"
        "AaAaAaAaBbBbBbBbCcCcCcCcDdDdDdDd"
        "AaAaAaAaBbBbBbBbCcCcCcCcDdDdDdDd"
        "AaAaAaAaBbBbBbBbCcCcCcCcDdDdDdDd"
        "EeEeEeEeFfFfFfFfGgGgGgGgHhHhHhHh"
        "EeEeEeEeFfFfFfFfGgGgGgGgHhHhHhHh"
        "EeEeEeEeFfFfFfFfGgGgGgGgHhHhHhHh"
        "EeEeEeEeFfFfFfFfGgGgGgGgHhHhHhHh"
        "IiIiIiIiJjJjJjJjKkKkKkKkLlLlLlLl"
        "IiIiIiIiJjJjJjJjKkKkKkKkLlLlLlLl"
        "IiIiIiIiJjJjJjJjKkKkKkKkLlLlLlLl"
        "IiIiIiIiJjJjJjJjKkKkKkKkLlLlLlLl"
        "MmMmMmMmNnNnNnNnOoOoOoOoPpPpPpPp"
        "MmMmMmMmNnNnNnNnOoOoOoOoPpPpPpPp"
        "MmMmMmMmNnNnNnNnOoOoOoOoPpPpPpPp"
        "MmMmMmMmNnNnNnNnOoOoOoOoPpPpPpPp"
    };
    nitf_Error error;
    TestSpec pTypeTests[] =
    {

        {
            "P",
            "INT",
            8,
            "NODISPLY",
            pixels,
            sizeof(pixels),
            NUM_BANDS,

            0, 2,
            4, 4,

            "aaaabbbb"
        },

        {
            "P",
            "INT",
            8,
            "NODISPLY",
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
            "INT",
            8,
            "NODISPLY",
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

    const size_t numTests = sizeof(pTypeTests) / sizeof(pTypeTests[0]);
    size_t testIndex;
    for (testIndex = 0; testIndex < numTests; ++testIndex)
    {
        TestSpec* spec = &pTypeTests[testIndex];
        TestState* test = constructTestSubheader(spec);

        /* Adjust subwindow to only read second band */
        test->subwindow->numBands = 1;
        test->subwindow->bandList = &test->bandList[1];

        int padded;
        nitf_Uint8* user[2];
        user[0] = (nitf_Uint8*)calloc(1, strlen(spec->expectedRead) + 1);
        user[1] = (nitf_Uint8*)calloc(1, strlen(spec->expectedRead) + 1);
        TEST_ASSERT(user[0]);
        TEST_ASSERT(user[1])
        nitf_ImageIO_read(test->imageIO, test->interface, test->subwindow,
                          user, &padded, &error);

        char *catUser = malloc(strlen(spec->expectedRead) + 1);
        strcpy(catUser, user[0]);
        strcat(catUser, user[1]);
        TEST_ASSERT(strcmp((char *)catUser, spec->expectedRead) == 0);
        free(user[0]);
        free(user[1]);
        free(catUser);
        freeTestState(test);
    }
}

TEST_CASE(testPBlockThreeBandsWithOffset)
{
    static const size_t NUM_BANDS = 3;
    char pixels[] =
    {
        "Aa1Aa1Aa1Aa1Bb2Bb2Bb2Bb2Cc3Cc3Cc3Cc3Dd4Dd4Dd4Dd4"
        "Aa1Aa1Aa1Aa1Bb2Bb2Bb2Bb2Cc3Cc3Cc3Cc3Dd4Dd4Dd4Dd4"
        "Aa1Aa1Aa1Aa1Bb2Bb2Bb2Bb2Cc3Cc3Cc3Cc3Dd4Dd4Dd4Dd4"
        "Aa1Aa1Aa1Aa1Bb2Bb2Bb2Bb2Cc3Cc3Cc3Cc3Dd4Dd4Dd4Dd4"
        "Ee5Ee5Ee5Ee5Ff6Ff6Ff6Ff6Gg7Gg7Gg7Gg7Hh8Hh8Hh8Hh8"
        "Ee5Ee5Ee5Ee5Ff6Ff6Ff6Ff6Gg7Gg7Gg7Gg7Hh8Hh8Hh8Hh8"
        "Ee5Ee5Ee5Ee5Ff6Ff6Ff6Ff6Gg7Gg7Gg7Gg7Hh8Hh8Hh8Hh8"
        "Ee5Ee5Ee5Ee5Ff6Ff6Ff6Ff6Gg7Gg7Gg7Gg7Hh8Hh8Hh8Hh8"
        "Ii9Ii9Ii9Ii9Jj0Jj0Jj0Jj0Kk!Kk!Kk!Kk!Ll@Ll@Ll@Ll@"
        "Ii9Ii9Ii9Ii9Jj0Jj0Jj0Jj0Kk!Kk!Kk!Kk!Ll@Ll@Ll@Ll@"
        "Ii9Ii9Ii9Ii9Jj0Jj0Jj0Jj0Kk!Kk!Kk!Kk!Ll@Ll@Ll@Ll@"
        "Ii9Ii9Ii9Ii9Jj0Jj0Jj0Jj0Kk!Kk!Kk!Kk!Ll@Ll@Ll@Ll@"
        "Mm#Mm#Mm#Mm#Nn$Nn$Nn$Nn$Oo%Oo%Oo%Oo%Pp^Pp^Pp^Pp^"
        "Mm#Mm#Mm#Mm#Nn$Nn$Nn$Nn$Oo%Oo%Oo%Oo%Pp^Pp^Pp^Pp^"
        "Mm#Mm#Mm#Mm#Nn$Nn$Nn$Nn$Oo%Oo%Oo%Oo%Pp^Pp^Pp^Pp^"
        "Mm#Mm#Mm#Mm#Nn$Nn$Nn$Nn$Oo%Oo%Oo%Oo%Pp^Pp^Pp^Pp^"
    };
    nitf_Error error;
    TestSpec pTypeTests[] =
    {
        {
            "P",
            "INT",
            8,
            "NODISPLY",
            pixels,
            sizeof(pixels),
            NUM_BANDS,

            0, 2,
            4, 4,

            "aaaabbbb11112222"
        },

        {
            "P",
            "INT",
            8,
            "NODISPLY",
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
            "INT",
            8,
            "NODISPLY",
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

    const size_t numTests = sizeof(pTypeTests) / sizeof(pTypeTests[0]);
    size_t testIndex;
    for (testIndex = 0; testIndex < numTests; ++testIndex)
    {
        TestSpec* spec = &pTypeTests[testIndex];
        TestState* test = constructTestSubheader(spec);

        /* Adjust subwindow to only read from second band */
        test->subwindow->numBands = 2;
        test->subwindow->bandList = &test->bandList[1];

        int padded;
        nitf_Uint8* user[2];
        user[0] = (nitf_Uint8*)calloc(1, strlen(spec->expectedRead) + 1);
        user[1] = (nitf_Uint8*)calloc(1, strlen(spec->expectedRead) + 1);
        TEST_ASSERT(user[0]);
        TEST_ASSERT(user[1])
        nitf_ImageIO_read(test->imageIO, test->interface, test->subwindow,
                user, &padded, &error);

        char *catUser = malloc(strlen(spec->expectedRead) + 1);
        strcpy(catUser, user[0]);
        strcat(catUser, user[1]);
        TEST_ASSERT(strcmp((char *)catUser, spec->expectedRead) == 0);
        free(user[0]);
        free(user[1]);
        free(catUser);
        freeTestState(test);
    }
}

TEST_CASE(testInvalidReadOrderFailsGracefully)
{
    static const size_t NUM_BANDS = 3;
    char pixels[] =
    {
        "Aa1Aa1Aa1Aa1Bb2Bb2Bb2Bb2Cc3Cc3Cc3Cc3Dd4Dd4Dd4Dd4"
        "Aa1Aa1Aa1Aa1Bb2Bb2Bb2Bb2Cc3Cc3Cc3Cc3Dd4Dd4Dd4Dd4"
        "Aa1Aa1Aa1Aa1Bb2Bb2Bb2Bb2Cc3Cc3Cc3Cc3Dd4Dd4Dd4Dd4"
        "Aa1Aa1Aa1Aa1Bb2Bb2Bb2Bb2Cc3Cc3Cc3Cc3Dd4Dd4Dd4Dd4"
        "Ee5Ee5Ee5Ee5Ff6Ff6Ff6Ff6Gg7Gg7Gg7Gg7Hh8Hh8Hh8Hh8"
        "Ee5Ee5Ee5Ee5Ff6Ff6Ff6Ff6Gg7Gg7Gg7Gg7Hh8Hh8Hh8Hh8"
        "Ee5Ee5Ee5Ee5Ff6Ff6Ff6Ff6Gg7Gg7Gg7Gg7Hh8Hh8Hh8Hh8"
        "Ee5Ee5Ee5Ee5Ff6Ff6Ff6Ff6Gg7Gg7Gg7Gg7Hh8Hh8Hh8Hh8"
        "Ii9Ii9Ii9Ii9Jj0Jj0Jj0Jj0Kk!Kk!Kk!Kk!Ll@Ll@Ll@Ll@"
        "Ii9Ii9Ii9Ii9Jj0Jj0Jj0Jj0Kk!Kk!Kk!Kk!Ll@Ll@Ll@Ll@"
        "Ii9Ii9Ii9Ii9Jj0Jj0Jj0Jj0Kk!Kk!Kk!Kk!Ll@Ll@Ll@Ll@"
        "Ii9Ii9Ii9Ii9Jj0Jj0Jj0Jj0Kk!Kk!Kk!Kk!Ll@Ll@Ll@Ll@"
        "Mm#Mm#Mm#Mm#Nn$Nn$Nn$Nn$Oo%Oo%Oo%Oo%Pp^Pp^Pp^Pp^"
        "Mm#Mm#Mm#Mm#Nn$Nn$Nn$Nn$Oo%Oo%Oo%Oo%Pp^Pp^Pp^Pp^"
        "Mm#Mm#Mm#Mm#Nn$Nn$Nn$Nn$Oo%Oo%Oo%Oo%Pp^Pp^Pp^Pp^"
        "Mm#Mm#Mm#Mm#Nn$Nn$Nn$Nn$Oo%Oo%Oo%Oo%Pp^Pp^Pp^Pp^"
    };

    nitf_Error error;
    TestSpec spec =
    {
        "P",
        "INT",
        8,
        "NODISPLY",
        pixels,
        sizeof(pixels),
        NUM_BANDS,

        0, 2,
        4, 4,

        ""
    };

    TestState* test = constructTestSubheader(&spec);

    test->subwindow->numBands = 2;
    test->subwindow->bandList = &test->bandList[1];
    test->subwindow->bandList[0] = 2;
    test->subwindow->bandList[1] = 0;

    int padded;
    nitf_Uint8* user[2] = { 0, 0 };
    TEST_ASSERT(!nitf_ImageIO_read(test->imageIO, test->interface, test->subwindow,
                user, &padded, &error));
    freeTestState(test);
}

TEST_CASE(testPBlock4BytePixels)
{
    static const size_t NUM_BANDS = 2;
    char pixels[] =
    {
        "**Aa**Aa**Aa**Aa**Bb**Bb**Bb**Bb**Cc**Cc**Cc**Cc**Dd**Dd**Dd**Dd"
        "**Aa**Aa**Aa**Aa**Bb**Bb**Bb**Bb**Cc**Cc**Cc**Cc**Dd**Dd**Dd**Dd"
        "**Aa**Aa**Aa**Aa**Bb**Bb**Bb**Bb**Cc**Cc**Cc**Cc**Dd**Dd**Dd**Dd"
        "**Aa**Aa**Aa**Aa**Bb**Bb**Bb**Bb**Cc**Cc**Cc**Cc**Dd**Dd**Dd**Dd"
        "**Ee**Ee**Ee**Ee**Ff**Ff**Ff**Ff**Gg**Gg**Gg**Gg**Hh**Hh**Hh**Hh"
        "**Ee**Ee**Ee**Ee**Ff**Ff**Ff**Ff**Gg**Gg**Gg**Gg**Hh**Hh**Hh**Hh"
        "**Ee**Ee**Ee**Ee**Ff**Ff**Ff**Ff**Gg**Gg**Gg**Gg**Hh**Hh**Hh**Hh"
        "**Ee**Ee**Ee**Ee**Ff**Ff**Ff**Ff**Gg**Gg**Gg**Gg**Hh**Hh**Hh**Hh"
        "**Ii**Ii**Ii**Ii**Jj**Jj**Jj**Jj**Kk**Kk**Kk**Kk**Ll**Ll**Ll**Ll"
        "**Ii**Ii**Ii**Ii**Jj**Jj**Jj**Jj**Kk**Kk**Kk**Kk**Ll**Ll**Ll**Ll"
        "**Ii**Ii**Ii**Ii**Jj**Jj**Jj**Jj**Kk**Kk**Kk**Kk**Ll**Ll**Ll**Ll"
        "**Ii**Ii**Ii**Ii**Jj**Jj**Jj**Jj**Kk**Kk**Kk**Kk**Ll**Ll**Ll**Ll"
        "**Mm**Mm**Mm**Mm**Nn**Nn**Nn**Nn**Oo**Oo**Oo**Oo**Pp**Pp**Pp**Pp"
        "**Mm**Mm**Mm**Mm**Nn**Nn**Nn**Nn**Oo**Oo**Oo**Oo**Pp**Pp**Pp**Pp"
        "**Mm**Mm**Mm**Mm**Nn**Nn**Nn**Nn**Oo**Oo**Oo**Oo**Pp**Pp**Pp**Pp"
        "**Mm**Mm**Mm**Mm**Nn**Nn**Nn**Nn**Oo**Oo**Oo**Oo**Pp**Pp**Pp**Pp"
    };
    nitf_Error error;
    TestSpec pTypeTests[] =
    {
        {
            "P",
            "INT",
            16,
            "NODISPLY",
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

    const size_t numTests = sizeof(pTypeTests) / sizeof(pTypeTests[0]);
    size_t testIndex;
    for (testIndex = 0; testIndex < numTests; ++testIndex)
    {
        TestSpec* spec = &pTypeTests[testIndex];
        TestState* test = constructTestSubheader(spec);

        /* Adjust subwindow to only read second band */
        test->subwindow->numBands = 1;
        test->subwindow->bandList = &test->bandList[1];

        int padded;
        nitf_Uint8* user = (nitf_Uint8*)calloc(1, strlen(spec->expectedRead) + 1);
        TEST_ASSERT(user);
        nitf_ImageIO_read(test->imageIO, test->interface, test->subwindow,
                &user, &padded, &error);

        TEST_ASSERT(strcmp((char *)user, spec->expectedRead) == 0);
        free(user);
        freeTestState(test);
    }
}

int main(int argc, char** argv)
{
    (void) argc;
    (void) argv;
    CHECK(testPBlockOneBand);
    CHECK(testPBlockTwoBands);
    CHECK(testPBlockOffsetBand);
    CHECK(testPBlockThreeBandsWithOffset);
    CHECK(testInvalidReadOrderFailsGracefully);
    CHECK(testPBlock4BytePixels);
    return 0;
}
