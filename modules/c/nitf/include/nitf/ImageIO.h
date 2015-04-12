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
 \brief  NITF Image segment image data I/O object (nitf_ImageIO)

 \todo Implement deep copy clone
 The nitf_ImageIO object is used to manage the pixel data I/O for one image
 segment of a NITF image.

 The pixel type constants are needed for an argument of the down-sample
 plug-in functions. Their odd encoding is because they are used internally
 as part of a mask that contains other private bit definitions.
*/

#ifndef NITF_IMAGE_IO_INCLUDED
#define NITF_IMAGE_IO_INCLUDED

#include "nitf/System.h"
#include "nitf/PluginIdentifier.h"
#include "nitf/ImageSubheader.h"
#include "nitf/SubWindow.h"

/*! \def NITF_IMAGE_IO_NO_OFFSET - No block/mask offset */

#define NITF_IMAGE_IO_NO_OFFSET      ((nitf_Uint32) 0xffffffff)

/*! \def NITF_NBPP_TO_BYTES  - Compute bytes per pixel from NBPP field */

#define NITF_NBPP_TO_BYTES(nbpp) ((((int) (nbpp)) - 1)/8 + 1)

/*! \def NITF_IMAGE_IO_PIXEL_TYPE_INT - Integer */
#define NITF_IMAGE_IO_PIXEL_TYPE_INT ((nitf_Uint32) 0x00080000)

/*! \def NITF_IMAGE_IO_PIXEL_TYPE_B - Bi-valued */
#define NITF_IMAGE_IO_PIXEL_TYPE_B ((nitf_Uint32) 0x00100000)

/*! \def NITF_IMAGE_IO_PIXEL_TYPE_SI - Two's complement signed integer */
#define NITF_IMAGE_IO_PIXEL_TYPE_SI ((nitf_Uint32) 0x00200000)

/*! \def NITF_IMAGE_IO_PIXEL_TYPE_R - Floating point */
#define NITF_IMAGE_IO_PIXEL_TYPE_R ((nitf_Uint32) 0x00400000)

/*! \def NITF_IMAGE_IO_PIXEL_TYPE_C - Complex floating point */
#define NITF_IMAGE_IO_PIXEL_TYPE_C ((nitf_Uint32) 0x00800000)

/* Psuedo type for 12 bit integer (NBPP == 12 and ABPP = 12) */
/*! \def NITF_IMAGE_IO_PIXEL_TYPE_12 - 12 bit integer signed or unsigned */
#define NITF_IMAGE_IO_PIXEL_TYPE_12 ((nitf_Uint32) 0x01000000)

NITF_CXX_GUARD

/*!
  \brief NITF Image segment image data I/O object handle
 
  The \b nitf_ImageIO is the object used to manage one the image data I/O for
  one image segment of one image. There are no user accessible fields
 
*/
typedef void nitf_ImageIO;

/*!
  \brief nitf_BlockingInfo - Blocking information structure
 
  The \b nitf_BlockingInfo provides information about an image segment's block
  organization. This structure is obtained by calling the function
  \b nitf_ImageIO_getBlockingInfo. The information in this structure reflects
  the correct information. Due to the implementation of some compression
  types the information in the image segment header may not be correct.
 
*/

typedef struct _nitf_BlockingInfo
{
    nitf_Uint32 numBlocksPerRow;        /*!< Number of blocks per row */
    nitf_Uint32 numBlocksPerCol;        /*!< Number of blocks per column */
    nitf_Uint32 numRowsPerBlock;        /*!< Number of rows per block */
    nitf_Uint32 numColsPerBlock;        /*!< Number of columns per block */
    size_t length;                      /*!< Total block length in bytes */
}
nitf_BlockingInfo;


/*!
  \brief Constructor for the nitf_BlockingInfo object
 
  \b nitf_BlockingInfo_construct is the constructor for the \em nitf_BlockingInfo object.
  The returned object must be freed via the object's destructor,
  \em nitf_BlockingInfo_destruct
 
  \return The new object or NULL on error
 
  On error, the user supplied error object is set. Possible errors include:
 
      Memory allocation error\n
*/
NITFPROT(nitf_BlockingInfo *) nitf_BlockingInfo_construct(nitf_Error *
        error);

/*!
  \brief Destructor for the nitf_BlockingInfo object
 
  \b nitf_BlockingInfo_destruct deallocates the memory associated with the object.
  The argument is set to NULL on return
 
  \return None
*/
NITFPROT(void) nitf_BlockingInfo_destruct(nitf_BlockingInfo ** info);


/*!
  \brief nitf_ImageIO_writeSequential  - Create write control object for
   sequential writing full rows, all bands
 
  nitf_ImageIO_writeSequential creates a write control object for sequential
  writes of full rows with all bands.
 
  The caller supplied IO handle is retained internally. The subsequent write
  operations position the handle to the correct location before writing. The
  handle is not repositioned after the operation. The caller can use the handle
  between write calls but must not close or destroy it. The write done function
  does not close the handle.
 
  State information about the write operation is mantained in the image I/O
  object. It is an error to try to start another operation before the write
  done function is called.
 
  \param nitf Associated ImageIO object
  \param io The IO interface to use
  \param error [out] return errors
  \return FALSE is returned on error and the error object is set
 
  Possible errors:
 
    Write already in progress
    Memory allocation errors
    I/O errors
 
*/

NITFPROT(NITF_BOOL) nitf_ImageIO_writeSequential(nitf_ImageIO * nitf,
                                                 nitf_IOInterface* io,
                                                 nitf_Error * error
                                                );

/*!
  \brief nitf_ImageIO_writeDone - Cleanup for write
 
  nitf_ImageIOWrite_writeDone does post writing clean-up for the calling
  object.
 
  All memory allocated for the write operation is freed and any deferred write
  operations are complete. The deferred writes are done by calling the flush
  operation.
 
  \param object Object to cleanup
  \param io io interface to use
  \param error [out] The error to return

  \return One error, FALSE is returned and the caller supplied error object
  is set.
 
  Possible errors:
 
    Writer not active
    I/O errors
    Memory allocation errors
    Invalid row count
 
*/

NITFPROT(NITF_BOOL) nitf_ImageIO_writeDone(nitf_ImageIO * object,
                                           nitf_IOInterface* io,
                                           nitf_Error * error);

/*!
  \brief nitf_ImageIO_writeRows - Write rows more rows
 
  nitf_ImageIO_writeRows writes consecutive rows to the image. The rows
  are written after the rows of the previous call. This function is for
  sequential writes of complete rows
 
  The "data" argument is an array of pointers. Each pointer points to the
  row data for one band. When multiple rows are written (numRows != 1),
  Each pointer points to numRows rows of data for that band. The pointers
  should be ordered according to the band order.
 
  The image I/O object must be set-up for write by one of the following
  functions:
 
      nitf_ImageIO_writeSequential
 
  \param object Associated ImageIO object
  \param io Interface for writes
  \param data Row buffers, one per band
  \param erro For error reports
  \return One error, FALSE is returned and the caller supplied error object
  is set.
 
  Possible errors:
 
    Writer not active
    I/O errors
    Memory allocation errors
    Invalid row count
*/

NITFPROT(NITF_BOOL) nitf_ImageIO_writeRows(nitf_ImageIO * object,
                                           nitf_IOInterface* io,
                                           nitf_Uint32 numRows,
                                           nitf_Uint8 ** data,
                                           nitf_Error * error
                                          );

/*!
  \brief nitf_ImageIO_flush - Complete deferred writes
 
  nitf_ImageIO_flush completes any deferred write associated with the
  calling object. An example of something that might be deferred is the
  writing of the block mask.
 
  \param The object to flush
  \param io The interface pointer
  \param error [out] error to return
  \return On error, FALSE is returned and the supplied error object is set
 
  Possible errors include:
 
    Writer not active
    I/O errors
*/

NITFPROT(NITF_BOOL) nitf_ImageIO_flush(nitf_ImageIO * object,
                                       nitf_IOInterface* io,
                                       nitf_Error * error
                                      );


/*!
  \brief nitf_ImageIO_setPadPixel - Set the pad pixel value used for write
 
  nitf_ImageIO_setPadPixel sets the pad pixel value used for write. This
  value is used for write operations.
 
  If after this call, the image object is used for a read and the image
  segment  defines a pad pixel value. The value is set to the segment's
  value
 
  Because the pad pixel's type depends on the pixel type the user must give
  the value as a byte pointer and length.
  \param object to flush
  \param pad pixel value
  \param length of the value
  \return None
 
*/

NITFPROT(NITF_BOOL) 
nitf_ImageIO_setPadPixel(nitf_ImageIO * object,
                         nitf_Uint8 * value,
                         nitf_Uint32 length,
                         nitf_Error* error
    );


/*!
  \brief nitf_CompressionControl - Compression control object

  The nitf_CompressionControl object manages the writing of blocks for
  one image segment. The use of this object is not thread safe but the
  reader objects that are created from it are thread safe if thread safe
  operations are supported.

  The object has no public fields and may be different for each compression
  library
*/

typedef void nitf_CompressionControl;

/*!
    \brief NITF_COMPRESSION_INTERFACE_OPEN_FUNCTION - Image compression
  interface open function

  This function pointer type is the type for the open field in the
  compression interface object. The function creates the interface control
  object and extracts any required values from the supplied image sub-header

  The plugin object should not retain a reference to the subheader object.

  The plugin may modify subheader values to be consistent with the compression
  type's BIIF specification. For example a required block size. It may also
  generate an error if some subheader fields are not the required values.

  \ar subheader          - Associated image subheader
  \ar error              - Error object

  \return The new nitf_CompressionControl structure or NULL on error
*/

typedef nitf_CompressionControl *
(*NITF_COMPRESSION_INTERFACE_OPEN_FUNCTION)
 (nitf_ImageSubheader * subheader,nitf_Error * error);

/*!
    \brief NITF_COMPRESSION_INTERFACE_START_FUNCTION - Image compression
  interface start function

  This function pointer type is the type for the start field in the
  compression interface object. The function initializes the compression

  \ar object             - The compression control from the open function
  \ar offset             - File offset to start of compressed data output
  \ar dataLength         - Length of uncompressed data
  \ar blockMask          - Block mask, possibly updated during compression
  \ar padMask            - Pad mask, possibly updated during compression
  \ar error              - Error object

  \return True on success and false on error

  The mask may be updated as the blocks are written by the write block
  function or after compression is complete by the end function. Copies
  of the pointers are maintained internally. If the masks are NULL, then
  the compression type is unmasked (i.e., C3 v M3)

  In order to avoid major complicated logic in the main library, the start
  function is responsible for verifying any plugin specific requirements such
  as allowed pixel size and type

  The plugin may retain the mask pointers and the IO interface supplied to the
  write block function

  On error, the error object is set
*/

typedef NITF_BOOL
(*NITF_COMPRESSION_INTERFACE_START_FUNCTION)
(nitf_CompressionControl *object,
 nitf_Uint64 offset,nitf_Uint64 dataLength,
 nitf_Uint64 * blockMask,
 nitf_Uint64 * padMask, nitf_Error * error);

/*!
    \brief NITF_COMPRESSION_INTERFACE_WRITE_BLOCK_FUNCTION - Image
  compression interface write block function

  This function pointer type is the type for the writeBlock field in the
  compression interface object. The function writes a block.

  The data is supplied to the compressor in order, starting with the first
  block. If pad is true, then the block contains pad pixels. If noData is true,
  then this is a pad-only,missing block. The compression type may or may not
  support missing blocks.

  \ar object           - Associated compression control
  \ar handle           - The IO handle for image IO
  \ar data             - Block data
  \ar NITF_BOOL pad    - Block contains some pad block
  \ar NITF_BOOL noData - Block has no data (pad only)
  \ar error            - Error object

  \return Error flag, false means error

*/

typedef NITF_BOOL (*NITF_COMPRESSION_INTERFACE_WRITE_BLOCK_FUNCTION)
(nitf_CompressionControl * object, nitf_IOInterface* io,nitf_Uint8 *data,
    NITF_BOOL pad,NITF_BOOL noData,nitf_Error *error);

/*!
    \brief NITF_COMPRESSION_INTERFACE_END_FUNCTION - Image compression
  interface end function

  This function pointer type is the type for the end field in the
  compression interface object. The function completes the writing of
  the compressed data and makes any final changes to the block masks

  \ar object          - Associated compression control
  \ar handle          - The IO handle for image IO
  \ar error           - Error object

  \return Error flag, false means error

*/

typedef NITF_BOOL (*NITF_COMPRESSION_INTERFACE_END_FUNCTION)
(nitf_CompressionControl * object,nitf_IOInterface* io,nitf_Error *error);

/*!
    \brief NITF_COMPRESSION_CONTROL_DESTROY_FUNCTION - Image compression
    interface control object destructor

  This function pointer type is the type for the destroy field in the
  compression interface object. The function destroys a interface control
  object.

  \ar object      - Associated interface control

  \return none

  On error, the error object is set
*/

typedef void (*NITF_COMPRESSION_CONTROL_DESTROY_FUNCTION)
(nitf_CompressionControl ** object);


/*!
  \brief nitf_DecompressionControl - Decompression control object
 
  The nitf_DecompressionControl object manages the reading of blocks for
  one image segment. The use of this object is not thread safe but the
  reader objects that are created from it are thread safe if thread safe
  operations are supported.
 
  The object has no public fields and may be different for each compression
  library
*/

typedef void nitf_DecompressionControl;

/*!
    \brief NITF_DECOMPRESSION_INTERFACE_OPEN_FUNCTION - Image decompression
  interface open function
 
  This function pointer type is the type for the open field in the
  decompression interface object. The function prepare for first image data
  access.
 
  \ar handle             - The IO handle for image IO
  \ar offset             - File offset to start of compressed data
  \ar fileLength         - Length of compressed data in the file
  \ar blockingDefinition - Blocking information for the image
  \ar blockMask          - Block mask
  \ar error              - Error object
 
  \return The new nitf_DecompressionControl structure or NULL on error
 
  On error, the error object is set
*/

typedef nitf_DecompressionControl *
(*NITF_DECOMPRESSION_INTERFACE_OPEN_FUNCTION)
(nitf_IOInterface* io,
 nitf_Uint64 offset,
 nitf_Uint64 fileLength,
 nitf_BlockingInfo * blockingDefinition,
 nitf_Uint64 * blockMask, nitf_Error * error);

/*!
    \brief NITF_DECOMPRESSION_INTERFACE_READ_BLOCK_FUNCTION - Image
  decompression interface read block function
 
  This function pointer type is the type for the readBlock field in the
  decompression interface object. The function reads a block.
 
  The reader allocates the returned buffer which may be obtained by some
  facility other than the NITF library memory allocation interface. This
  buffer must be freed via the freeBlock function entry.
 
  \ar object      - Associated reader
  \ar blockNumber - Block number
  \ar error       - Error object
 
  \return The data in a buffer or NULL on error
 
  On error, the error object is set
*/

typedef nitf_Uint8 *(*NITF_DECOMPRESSION_INTERFACE_READ_BLOCK_FUNCTION)
(nitf_DecompressionControl * object,
 nitf_Uint32 blockNumber, nitf_Error * error);

/*!
    \brief NITF_DECOMPRESSION_INTERFACE_FREE_BLOCK_FUNCTION - Image
  decompression interface free block function
 
  This function pointer type is the type for the freeBlock field in the
  decompression interface object. The function frees a block allocated by
  the reader.
 
  \ar object      - Associated reader
  \ar block       - Block to free
  \ar error       - Error object
 
  \return On error, FALSE is returned
 
  On error, the error object is set
*/

typedef NITF_BOOL(*NITF_DECOMPRESSION_INTERFACE_FREE_BLOCK_FUNCTION)
(nitf_DecompressionControl * object,
 nitf_Uint8 * block, nitf_Error * error);

/*!
    \brief NITF_DECOMPRESSION_CONTROL_DESTROY_FUNCTION - Image decompression
    interface control object destructor
 
  This function pointer type is the type for the destroy field in the
  decompression interface object. The function destroys a interface control
  object.
 
  \ar object      - Associated interface control
  \ar error       - Error object
 
  \return On error, FALSE is returned
 
  On error, the error object is set
*/

typedef void (*NITF_DECOMPRESSION_CONTROL_DESTROY_FUNCTION)
(nitf_DecompressionControl ** object);

/*!
  \brief nitf_CompressionInterface - Interface object for compression
 
  The nitf_CompressionInterface object provides function entry points for
  compressing image data. Each object handles a particular type of
  compression.
 
*/

/*!
  \brief nitf_CompressionInterface - Interface object for compression

  The nitf_CompressionInterface object provides function entry points for
  compressing image data. Each object handles a particular type of compression.

*/

typedef struct _nitf_CompressionInterface
{
    NITF_COMPRESSION_INTERFACE_OPEN_FUNCTION open;
                           /*!< Open compression */
    NITF_COMPRESSION_INTERFACE_START_FUNCTION start;
                           /*!< Start compression */
    NITF_COMPRESSION_INTERFACE_WRITE_BLOCK_FUNCTION writeBlock;
                           /*!< Write a block */
    NITF_COMPRESSION_INTERFACE_END_FUNCTION end;
                           /*!< End compression */
    NITF_COMPRESSION_CONTROL_DESTROY_FUNCTION destroyControl;
                           /*!< Destructor for compression control object */
    void *internal;
                           /*!< Pointer to compression specific internal data */
}
nitf_CompressionInterface;

/*!
  \brief nitf_DecompressionInterface - Interface object for decompression
 
  The nitf_DecompressionInterface object provides function entry points for
  decompressing image data. Each object handles a particular type of
  compression.
 
*/

typedef struct _nitf_DecompressionInterface
{
    NITF_DECOMPRESSION_INTERFACE_OPEN_FUNCTION open;    /*!< Prepare for first image data access */
    NITF_DECOMPRESSION_INTERFACE_READ_BLOCK_FUNCTION readBlock; /*!< Read a block */
    NITF_DECOMPRESSION_INTERFACE_FREE_BLOCK_FUNCTION freeBlock; /*!< Free block returned by readBlock */
    NITF_DECOMPRESSION_CONTROL_DESTROY_FUNCTION destroyControl; /*!< Destructor for decompression control object */
    void *internal;             /*!< Pointer to compression specific internal data */
}
nitf_DecompressionInterface;

/*!
  \brief NITF_DOWN_SAMPLE_FUNCTION - Function pointer for down-sample
  function
 
  The reader supports image down-sampling on read via a function interface.
  The down-sample function supplies the actual functionality.
 
  Down-sample functions are intended to be purely numerical functions that
  operate on pre-allocated buffers but it is conceivable that some more
  complex algorithms might be needed to allocate memory or might not work on
  some data types. Therefore an error return is allowed.
 
  \ar input             - Input data
  \ar output            - Output data
  \ar parameters        - Processing parameters
  \ar numWindowRows     - Number of rows of windows
  \ar numWindowCols     - Number of columns of windows
  \ar subWindow         - Associated sub-window spec
  \ar pixelType         - Pixel type code
  \ar pixelSize         - Pixel size
  \ar rowsInLastWindow  - Number of valid rows in the last window
  \ar colsInLastWindow  - Number of valid columns in the last window
  \ar error             - Error object
 
  The parameters  argument is a user supplied structure that supplies
  parameter information to the down-sample fucntion. An example might be
  a kernel for a convolution down-sample method. The form of the structure
  is method dependent.
 
  \return Returns FALSE on error.
 
  On error, the error object is initialized.
*/

typedef int (*NITF_DOWN_SAMPLE_FUNCTION) (void *input,
                                          void *output,
                                          void *parameters,
                                          nitf_Uint32 numWindowRows,
                                          nitf_Uint32 numWindowCols,
                                          nitf_SubWindow * subWindow,
                                          nitf_Uint32 pixelType,
                                          nitf_Uint32 pixelSize,
                                          nitf_Uint32 rowsInLastWindow,
                                          nitf_Uint32 colsInLastWindow,
                                          nitf_Error * error);
/*!
  \brief Constructor for the nitf_ImageIO object
 
  \b nitf_ImageIO_construct is the constructor for the \em nitf_ImageIO object.
  The returned object must be freed via the object's destructor,
  \em nitf_ImageIO_destruct
 
  This function does not interpret the NITF or image segment header, hence
  the large number of arguments.
 
  The \em offset argument is the file offset to the start of the corresponding
  image data segment (not the header). For some images this is the start of
  the block and pad mask information
 
  The \em compressionRate argument is based on the conditional COMRAT NITF
  image subheader field. If the field is not present the argument should be
  an empty string, not a NULL pointer.
 
  The compressor/decompressor functions should be set to be consistant with
  the compression type specified by the compression argument. NULL pointers
  should be used for uncompressed images
 
  \param subheader From the calling image segment
  \param offset Byte offset in file to image data segment
  \param length Length of image data segment
  \param compressor Compression interface
  \param decompressor Decompression interface
  \param error Error object
  \return The new object or NULL on error
 
  On error, the user supplied error object is set. Possible errors include:
 
      Memory allocation error\n
      Blocking mode not supported\n
      Compression type not supported\n
      Invalid argument\n
 
  The invalid argument error occurs when the block dimensions are
  inconsistant with the image dimensions
 
*/

NITFPROT(nitf_ImageIO *) 
nitf_ImageIO_construct(nitf_ImageSubheader * subheader, 
                       nitf_Uint64 offset,
                       nitf_Uint64 length,
                       nitf_CompressionInterface * compressor,
                       nitf_DecompressionInterface * decompressor,
                       nitf_Error * error
    );

/*!
  \brief nitf_ImageIO_clone - Clone an object
 
  \b nitf_ImageIO_clone creates a copy of the caller. This is a deep copy
  except that any state information in the object is reset th the initial
  values. Calling this function is the same as calling the constructor with
  the same arguments used to create the caller.
  \param image The object to clone
  \return A pointer to the new object is returned. On error, NULL is returned
  and the caller supplied error object is set.
 
  Possible errors:
 
    Memory allocation error
*/

NITFPROT(nitf_ImageIO *) nitf_ImageIO_clone(nitf_ImageIO * image,
                                            nitf_Error * error
                                           );


/*!
  \brief Destructor for the nitf_ImageIO object
 
  \b nitf_ImageIO_destruct deallocates the memory associated with the object.
  The argument is set to NULL on return
 
  \param Pointer to the nitf_ImageIO object to destroy
  \return None
*/

NITFPROT(void) nitf_ImageIO_destruct(nitf_ImageIO ** nitf);

/*!
  \brief nitf_ImageIO_read - Read a sub-window
 
  \b nitf_ImageIO_read reads a sub-window. The user supplies the opened file
  descriptor and data buffers. The file should allow seeks. The file is left in
  the file position of the last read.
 
  If the \em padded argument returns TRUE the request may include pad pixels.
  For blocked images, each block may contain pad pixels. It is possible to
  determine that a given block contains pads but it is not always possible
  to tell which ones. So a request may hit a block with pad pixels but miss
  the actual pixels. Thus the padded flag is only a maybe. A FALSE value
  does mean that there are not pad pixels in the request.
 
  The user must supply one buffer for each band requested in the sub-window.
  Each buffer must have enough storage for one band of the requested
  sub-window size.
 
  Row and column skips of more than one in the sub-window are not currently
  implemented.
 
  \param nitf The associated nitf_ImageIO object
  \param io The IO interface
  \param subWindow Sub-window to read
  \param padded Returns TRUE if pad pixels may have been read
  \param error [out] Error object
  \return Returns FALSE on error
 
  On error, FALSE is returned and the user supplied error object is set.
 
  Possible errors include:
 
    Write in progress
    Invalid sub-set or band
    System I/O or memory allocation errors
*/

NITFPROT(NITF_BOOL) nitf_ImageIO_read(nitf_ImageIO * nitf,
                                      nitf_IOInterface* io,
                                      nitf_SubWindow * subWindow,
                                      nitf_Uint8 ** user,
                                      int *padded,
                                      nitf_Error * error
                                     );

/*!
  \brief  nitf_ImageIO_pixelSize - Return the pixel size
 
  \b nitf_ImageIO_pixelSize returns the pixel size in bytes.
 
  \param nitf The associated nitf_ImageIO object
  \return Returns pixel size in bytes
*/

NITFPROT(nitf_Uint32) nitf_ImageIO_pixelSize(nitf_ImageIO * nitf);

/*!
  \brief  nitf_ImageIO_setFileOffset
 
  \b nitf_ImageIO_setFileOffset sets the file offset to the start of the
  image data. This resets the value given as the \b offset argument of the
  nitf_ImageIO contructor. This value cannot be changed if an I/O operation
  is in progress.
 
  \param nitf 
  \param offset The associated nitf_ImageIO offset
  \param error Error object
  \return FALSE is returned on error and teh error object is set
 
  Possible errors include:
 
    I/O operation in progress
*/

NITFPROT(NITF_BOOL) nitf_ImageIO_setFileOffset(nitf_ImageIO * nitf,
                                               nitf_Uint64 offset,
                                               nitf_Error * error 
                                              );

/*!
  \brief nitf_ImageIO_getBlockingInfo - Get blocking information
 
  \b nitf_ImageIO_getBlockingInfo returns blocking information in a
  \b  nitf_BlockingInfo. The data in this structure reflects the correct
  information. Due to the implementation of some compression types the
  information in the image segment header may not be correct.
 
  The storage for the returned value is allocated for this call via the
  system memory allocation method and should be freed by the caller.
 
  This function may have read values from the file, hence the need for the
  I/O handle. This function can only be called for an existing file.
 
  \param image The associated ImageIO object 
  \param io IO interface for read
  \param error Error object
  \return A pointer to a \b nitf_BlockingInfo structure. On error, the error
  structure is set and NULL is returned.
 
  Possible errors include:
 
    memory allocation error
    I/O error
*/

NITFPROT(nitf_BlockingInfo *) nitf_ImageIO_getBlockingInfo(nitf_ImageIO * image,
                                                           nitf_IOInterface* io,
                                                           nitf_Error * error
                                                          );

/*!
  \brief nitf_ImageIO_setWriteCaching - Enable/disable cached writes
 
  See the documentation for nitf_ImageWriter_setWriteCaching
 
  \return Returns the current enable/disable state
*/

NITFPROT(int) nitf_ImageIO_setWriteCaching
(
    nitf_ImageIO * nitf,      /*!< Object to modify */
    int enable               /*!< Enable cached writes if true */
);

/*!
  \brief nitf_ImageIO_setReadCaching - Enable cached reads
 
  See the documentation for nitf_ImageWriter_setReadCaching
 
  \return None
*/

NITFPROT(void) nitf_ImageIO_setReadCaching
(
    nitf_ImageIO * nitf      /*!< Object to modify */
);

/*!
  \brief nitf_BlockingInfo_print - Print blocking information
 
  \b nitf_BlockingInfo_print doe a formatted print of a nitf_BlockingInfo
  structure.
 
  If the "file" argument is NULL stdout is used.
 
  \return None
*/

NITFPROT(void) nitf_BlockingInfo_print(nitf_BlockingInfo * info,  /*!< The structure to print */
                                       FILE * file       /*!< FILE to use for print */
                                      );

NITF_CXX_ENDGUARD

#endif
