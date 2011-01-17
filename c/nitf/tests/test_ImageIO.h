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

/*!

  \file
  \brief Include file for support functions for the NITF ImageIO library tests

  This include file defines support functions and structures used by
  stand-alone test programs for the NITF ImageIO libary. Stand-alone means
  programs that do not require any NITF header interpretation functionality.

*/

#ifndef TEST_NITF_IMAGE_IO_INCLUDED
#define TEST_NITF_IMAGE_IO_INCLUDED

#include <import/nitf.h>

#ifdef __cplusplus
extern "C"
{
#endif

    /*! \def TEST_NITF_IMAGE_IO_MAX_STRING - Longest string in test setup
    structures */
#define TEST_NITF_IMAGE_IO_MAX_STRING 256

    /*! \def TEST_NITF_IMAGE_IO_MAX_BANDS - Upper limit on band count */
#define TEST_NITF_IMAGE_IO_MAX_BANDS 32

    /*!

      \brief test_nitf_ImageIOConstructArgs - Structure that holds arguments for
      nitf_ImageIOConstruct

      This structure is used to facilitate reading files that contain argument
      values (e,g., a test vector). The error buffer argument is not included

      The pad value array has storage for the largest posisble pad value (8). The
      value is stored in native order.
    */

    typedef struct
    {
        nitf_Uint32 nRows;          /*!< Number of rows in the image */
        nitf_Uint32 nColumns;       /*!< Number of columns in the image */
        nitf_Uint32 nBands;         /*!< Number of bands */
        nitf_Uint32 nMultiBands;    /*!< Number of mutli-spectral bands */
        char pixelType[TEST_NITF_IMAGE_IO_MAX_STRING+2];  /*!< Pixel type */
        nitf_Uint32 nBits;          /*!< Number of bits per pixel */
        nitf_Uint32 nBitsActual;    /*!< Actual number of bits per pixel */
        char justify[TEST_NITF_IMAGE_IO_MAX_STRING+2];  /*!< ;Pixels justification */
        nitf_Uint32 nBlksPerRow;    /*!< Number of blocks per row */
        nitf_Uint32 nBlksPerColumn; /*!< Number of blocks per Columns */
        nitf_Uint32 nRowsPerBlk;    /*!< Number of rows per block */
        nitf_Uint32 nColumnsPerBlk; /*!< Number of columns per block */
        char mode[TEST_NITF_IMAGE_IO_MAX_STRING+2];  /*!< Blocking mode */
        char compression[TEST_NITF_IMAGE_IO_MAX_STRING+2];  /*!< Compression type */
        nitf_Uint8 padValue[8];     /*!< Pad value */
        nitf_Uint32 offset;         /*!< Byte offset in file to image data segment */
        char dataPattern[TEST_NITF_IMAGE_IO_MAX_STRING+2]; /*!< Pattern gen method */
    }
    test_nitf_ImageIOConstructArgs;

    /*!

      \brief test_nitf_ImageIOReadArgs - Structure that holds arguments for
      nitf_ImageIORead

      The nitf", "fd", "user", and "padded" arguments are not represented in the
      structure.

    */

    typedef struct
    {
        char name[TEST_NITF_IMAGE_IO_MAX_STRING+2]; /*!< File name to read */
        nitf_Uint32 row;                            /*!< Start row */
        nitf_Uint32 nRows;                          /*!< Number of rows to read */
        nitf_Uint32 rowSkip;                        /*!< Row skip factor */
        nitf_Uint32 column;                         /*!< Start column */
        nitf_Uint32 nColumns;                       /*!< Number of columns to read */
        nitf_Uint32 columnSkip;                     /*!< Column skip factor */
        char
        downSample[TEST_NITF_IMAGE_IO_MAX_STRING+2];/*!< Down-sample method */
        nitf_Uint32 nBands;                         /*!< Number of bands to read */
        nitf_Uint32 bands[TEST_NITF_IMAGE_IO_MAX_BANDS]; /*!< Bands to read */
    }
    test_nitf_ImageIOReadArgs;

    /*!
      \brief test_nitf_ImageIOReadConstructArgs - Read argument file for
       nitf_ImageIOConstruct call

      Read values from the file stream and initialize a argument structure.
      One value is read from each line. The lines in the file should give the
      values in the order they appear in the test_nitf_ImageIOConstructArgs
      structure. Each line can contain a comment following the value. The comment
      should be separated from the value by white space.

      On error, the error string argument is set. Error messages are not
      terminated by \\n

      \return The initialized structure or NULL on error

    */

    NITFAPI(test_nitf_ImageIOConstructArgs *) test_nitf_ImageIOReadConstructArgs
    (
        FILE *file,   /*!< The file to read */
        char **error  /*!< Returns an error message on error */
    );

    /*!
      \brief test_nitf_ImageIOReadReadArgs - Read argument file for
       nitf_ImageIORead call

      Read values from the file stream and initialize a argument structure.
      One value is read from each line. The lines in the file should give the
      values in the order they appear in the test_nitf_ImageIOReadArgs structure.
      Each line can contain a comment following the value. The comment should be
      separated from the value by white space. The band numbers in the "bands"
      array should be one per line

      On error, the error string argument is set. Error messages are not
      terminated by \\n

      \return Returns an intialized test_nitf_ImageIOReadArgs structure or NULL

    */

    NITFAPI(test_nitf_ImageIOReadArgs *) test_nitf_ImageIOReadReadArgs
    (
        FILE *file,   /*!< The file to read */
        char **error  /*!< Returns an error message on error */
    );


    /*!
      \brief test_nitf_ImageIO_mkArray - Create image array

      test_nitf_ImageIO_mkArray creates an image array with dimensions
      [bands][rows][columns]. The dimensions are taken from the construct
      arguments structure as is the pixel size in bytes

      test_nitf_ImageIO_freeArray should be used to free this array

      /return Returns the array. On error, NULL is returned and the error
      string is set
    */

    NITFAPI(void *) test_nitf_ImageIO_mkArray
    (
        test_nitf_ImageIOConstructArgs *args,  /*!< Supplies dimensions and size */
        char **error  /*!< Returns an error message on error */
    );

    /*!
      \brief test_nitf_ImageIO_freeArray - Free image array

      \return None
    */

    NITFAPI(void) test_nitf_ImageIO_freeArray
    (
        nitf_Uint8 ***data     /*!< The array to free */
    );

    /*!
      \brief test_nitf_ImageIO_brcI4 - Create a band/row/colum I4 image array

      test_nitf_ImageIO_brcI4 creates a three dimensional image array of 32 bit
      integers where:

           data[band][row][col] = (band << 16) + (row << 8) + col

      /return Returns the array. On error, NULL is returned and the error
      string is set
    */

    NITFAPI(void *) test_nitf_ImageIO_brcI4
    (
        test_nitf_ImageIOConstructArgs *args,  /*!< Supplies dimensions and size */
        char **error                   /*!< Returns an error message on error */
    );

    /*!
      \brief test_nitf_ImageIO_brcC8 - Create a band/row/colum C8 image array

      test_nitf_ImageIO_brcI4 creates a thredimensional image array of single
      precision complex (float real followed by float imaginary) where:

           data[band][row][col].r = band*100 + row
           data[band][row][col].i = band*100 + col

      /return Returns the array. On error, NULL is returned and the error
      string is set
    */

    NITFAPI(void *) test_nitf_ImageIO_brcC8
    (
        test_nitf_ImageIOConstructArgs *args,  /*!< Supplies dimensions and size */
        char **error                   /*!< Returns an error message on error */
    );

    /*!
      \brief test_nitf_ImageIO_Block - Create a block pattern

      test_nitf_ImageIO_block creates a three dimensional image array of
      one byte data. The data is:

           data[band][row][col] = band+1;

      Block pattern generator. The data is generated based on the blocking
      structure and a mask. The mask specifies which blocks are present and
      which are missing. For missing blocks the data is 0 for present blocks
      the data is the band number

      The mask is determined by the data generator tag. The tag always starts
      with "block_". The rest of the tag is the specification

      The specification is a single string composed of the row strings of the
      desired result. Each row sub-string is separated by an _. Each row string
      contains a character for each block in the row. If the character is "M"
      the block is missing, if it is "P" the block is present

      /return Returns the array. On error, NULL is returned and the error
      string is set
    */

    NITFAPI(void *) test_nitf_ImageIO_block
    (
        test_nitf_ImageIOConstructArgs *args,  /*!< Supplies dimensions and size */
        char **error  /*!< Returns an error message on error */
    );

    /*!
       \brief test_nitf_ImageIO_bigEndian - Test for Big endian system

       \return TRUE when called on a big-endian system
    */

    NITFAPI(int) test_nitf_ImageIO_bigEndian(void);

    /*========================= test_nitf_ImageIO_fakeSubheader ==================*/

    /*!
       \brief test_nitf_fakeSubheader - Create partially initialized image
      subheader object.

       \b test_nitf_fakeSubheader creates partially initialized image subheader
      object. This object can be used to construct a nitf_ImageIO object

      /return Returns the object. On error, NULL is returned and the error
      string is set.
    */

    NITFAPI(nitf_ImageSubheader *) test_nitf_ImageIO_fakeSubheader
    (
        test_nitf_ImageIOConstructArgs *args,  /*!< Supplies dimensions and size */
        char **error  /*!< Returns an error message on error */
    );

#ifdef __cplusplus
}
#endif

#endif

