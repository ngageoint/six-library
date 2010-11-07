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

#include "nitf/ImageIO.h"


/*!
  \file
  \brief NITF Image segment image data I/O object private data
   (_nitf_ImageIO)

  The _nitf_ImageIO object contains the actual data that defines the object.
  The opaque pointer (nitf_ImageIO) returned by the constructor points to
  an instance of this structure.

  \b Notes on the constants:

The compression, blocking mode and pixel type constants are implemented
as mutualy exclusive bit masks.\n

Although the big-valued ("B") pixel type is included in the constant
set (for completeness), this pixel type will be treated as a type of
compression of a one byte unsigned image.\n

*/

#define TRY_GET_UINT32(VALUE, DATA_PTR ) \
    if (!nitf_Field_get(VALUE, DATA_PTR, NITF_CONV_INT, NITF_INT32_SZ, error)) \
        goto CATCH_ERROR

#define TRY_GET_UINT64(VALUE, DATA_PTR) \
    if (!nitf_Field_get(VALUE, \
                        DATA_PTR, \
                        NITF_CONV_INT, \
                        NITF_INT64_SZ, error)) goto CATCH_ERROR

/*! \def NITF_IMAGE_IO_COMPRESSION_NC - No compression, no blocking */
#define NITF_IMAGE_IO_COMPRESSION_NC            ((nitf_Uint32) 0x00000001)

/*! \def NITF_IMAGE_IO_COMPRESSION_NM - No compression, blocking */
#define NITF_IMAGE_IO_COMPRESSION_NM            ((nitf_Uint32) 0x00000002)

/*! \def NITF_IMAGE_IO_COMPRESSION_C1 - Bi-level compression, no blocking */
#define NITF_IMAGE_IO_COMPRESSION_C1            ((nitf_Uint32) 0x00000004)

/*! \def NITF_IMAGE_IO_COMPRESSION_C3 - JPEG compression, no blocking */
#define NITF_IMAGE_IO_COMPRESSION_C3            ((nitf_Uint32) 0x00000008)

/*! \def NITF_IMAGE_IO_COMPRESSION_C4- Vector quantization  compression,
   no blocking */
#define NITF_IMAGE_IO_COMPRESSION_C4            ((nitf_Uint32) 0x00000010)

/*! \def NITF_IMAGE_IO_COMPRESSION_C5 - Lossless JPEG compression,
   no blocking */
#define NITF_IMAGE_IO_COMPRESSION_C5            ((nitf_Uint32) 0x00000020)

/*! \def NITF_IMAGE_IO_COMPRESSION_C6 - Reserved, no blocking */
#define NITF_IMAGE_IO_COMPRESSION_C6            ((nitf_Uint32) 0x00000040)

/*! \def NITF_IMAGE_IO_COMPRESSION_C8 - JPEG 2000 */
#define NITF_IMAGE_IO_COMPRESSION_C8            ((nitf_Uint32) 0x00000080)

/*! \def NITF_IMAGE_IO_COMPRESSION_I1 - Downsampled JPEG, no blocking */
#define NITF_IMAGE_IO_COMPRESSION_I1            ((nitf_Uint32) 0x00000100)

/*! \def NITF_IMAGE_IO_COMPRESSION_M1 - Bi-level compression, blocking */
#define NITF_IMAGE_IO_COMPRESSION_M1            ((nitf_Uint32) 0x00000200)

/*! \def NITF_IMAGE_IO_COMPRESSION_M3 - JPEG compression, blocking */
#define NITF_IMAGE_IO_COMPRESSION_M3 ((nitf_Uint32) 0x00000400)

/*! \def NITF_IMAGE_IO_COMPRESSION_M4 - Vector quantization  compression,
   blocking */
#define NITF_IMAGE_IO_COMPRESSION_M4 ((nitf_Uint32) 0x00000800)

/*! \def NITF_IMAGE_IO_COMPRESSION_M5 - Lossless JPEG compression, blocking */
#define NITF_IMAGE_IO_COMPRESSION_M5 ((nitf_Uint32) 0x00001000)

/*! \def NITF_IMAGE_IO_COMPRESSION_M8 - JPEG 2000 */
#define NITF_IMAGE_IO_COMPRESSION_M8 ((nitf_Uint32) 0x00004000)

/*! \def NITF_IMAGE_IO_BLOCKING_MODE_B - Band interleaved by block */
#define NITF_IMAGE_IO_BLOCKING_MODE_B ((nitf_Uint32) 0x00008000)

/*! \def NITF_IMAGE_IO_BLOCKING_MODE_P - Band interleaved by pixel */
#define NITF_IMAGE_IO_BLOCKING_MODE_P ((nitf_Uint32) 0x00010000)

/*! \def NITF_IMAGE_IO_BLOCKING_MODE_R - Band interleaved by row */
#define NITF_IMAGE_IO_BLOCKING_MODE_R ((nitf_Uint32) 0x00020000)

/*! \def NITF_IMAGE_IO_BLOCKING_MODE_S - Band sequential */
#define NITF_IMAGE_IO_BLOCKING_MODE_S ((nitf_Uint32) 0x00040000)

/*! \def NITF_IMAGE_IO_BLOCKING_MODE_RGB24 - Special case RGB 24-bit BIP */
#define NITF_IMAGE_IO_BLOCKING_MODE_RGB24 ((nitf_Uint32) 0x00050000)

/*! \def NITF_IMAGE_IO_BLOCKING_MODE_IQ - Special case IQ 2-band BIP */
#define NITF_IMAGE_IO_BLOCKING_MODE_IQ ((nitf_Uint32) 0x00060000)

/*! \def NITF_IMAGE_IO_NO_COMPRESSION - No compression combined mask */
#define NITF_IMAGE_IO_NO_COMPRESSION \
    (NITF_IMAGE_IO_COMPRESSION_NC  | NITF_IMAGE_IO_COMPRESSION_NM)

/*! \def NITF_IMAGE_IO_MASK_HEADER_LEN - Length of fixed part of mask header
   in the file (Does not include pad pixel value length) */
#define NITF_IMAGE_IO_MASK_HEADER_LEN      (10)

/*! \def NITF_IMAGE_IO_NO_BLOCK - Code used for block number when there is
   no block in the corresponding block cache buffer */
#define NITF_IMAGE_IO_NO_BLOCK             ((nitf_Uint32) 0xffffffff)

/*! \def NITF_IMAGE_IO_PAD_MAX_LENGTH - Maximum lenght of a pad pixel
   in bytes */
#define NITF_IMAGE_IO_PAD_MAX_LENGTH (16)

/*!
  \def NITF_IMAGE_IO_PAD_SCANNER - Macro to a create pad scan function

  Most of the logic is to avoid the fill pixels which are at the ends of the
  rows and last rows in blocks on the right and bottom border of the image.

  Notes:

    The padColumnCount is a byte count not a pixel count
    The padRowCount is a row count
    The padRowCount only applies if the block is the last
      block in the block column

  The pad value and data have already been byte swapped if needed,
 */

#define NITF_IMAGE_IO_PAD_SCANNER(name,type) \
    NITFPRIV(void) name \
    (struct _nitf_ImageIOBlock_s *blockIO, \
     NITF_BOOL *padFound,NITF_BOOL *dataFound) \
    { \
        type *pixels = (type *) (blockIO->blockControl.block); \
        type padValue = *((type *) (blockIO->cntl->nitf->pixel.pad)); \
        nitf_Uint32 row; \
        nitf_Uint32 col; \
        nitf_Uint32 rowEndIncr; \
        nitf_Uint32 colLimit; \
        nitf_Uint32 rowLimit; \
        _nitf_ImageIO *nitf = blockIO->cntl->nitf; \
        NITF_BOOL pFound = 0; \
        NITF_BOOL dFound = 0; \
        rowEndIncr = blockIO->padColumnCount/(nitf->pixel.bytes); \
        colLimit = blockIO->cntl->nitf->numColumnsPerBlock - rowEndIncr; \
        rowLimit = blockIO->cntl->nitf->numRowsPerBlock;\
        if(blockIO->currentRow >= (nitf->numRows - 1)) \
            rowLimit -= blockIO->padRowCount; \
        for(row=0;row<rowLimit;row++) \
        { \
            for(col=0;col<colLimit;col++) \
            { \
                if(*(pixels++) == padValue) \
                    pFound = 1; \
                else \
                    dFound = 1; \
            } \
            pixels += rowEndIncr; \
        } \
        *padFound = pFound; \
        *dataFound = dFound; \
        return; \
    }

/* Forward reference */
struct _nitf_ImageIOBlock_s;
struct _nitf_ImageIOControl_s;  /* Forward reference */
struct _nitf_ImageIOWriteControl_s;     /* Forward reference */
struct _nitf_ImageIOReadControl_s;      /* Forward reference */

/*!
  \brief _NITF_IMAGE_IO_CONTROL_FUNC   - Control function pointer

  \ar cntl  - Block I/O control structure
  \ar error - Error object

  \return Returns FALSE on error
*/

typedef int (*_NITF_IMAGE_IO_CONTROL_FUNC)
(struct _nitf_ImageIOControl_s * cntl, nitf_Error * error);

/*!
  \brief _NITF_IMAGE_IO_IO_FUNC - I/O function pointer

  \ar io    - Block I/O structure
  \ar error - Error object

  \return Returns FALSE on error
*/

typedef int (*_NITF_IMAGE_IO_IO_FUNC)(struct _nitf_ImageIOBlock_s * block,
                                      nitf_IOInterface* io, 
                                      nitf_Error * error);

/*!
  \brief _NITF_IMAGE_IO_PACK_FUNC - Pack/unpack function pointer

  \ar io    - Block I/O structure
  \ar error - Error object

  \return None
*/

typedef void (*_NITF_IMAGE_IO_PACK_FUNC)
(struct _nitf_ImageIOBlock_s * block, nitf_Error * error);

/*!
  \brief _NITF_IMAGE_IO_UNFORMAT_FUNC - Pixel unformat function pointer

  \ar buffer     - Pixel buffer
  \ar count      - Pixel (not byte) count
  \ar shiftCount - Bit shift count

  \return None
*/

typedef void (*_NITF_IMAGE_IO_UNFORMAT_FUNC)
(nitf_Uint8 * buffer, size_t count, nitf_Uint32 shiftCount);

/*!
  \brief _NITF_IMAGE_IO_FORMAT_FUNC - Pixel format function pointer

  \ar buffer     - Pixel buffer
  \ar count      - Pixel (not byte) count
  \ar shiftCount - Bit shift count

  \return None
*/

typedef void (*_NITF_IMAGE_IO_FORMAT_FUNC)
(nitf_Uint8 * buffer, size_t count, nitf_Uint32 shiftCount);

/*!
  \brief Pad pixel scan function pointer

  The pad pixel scan functions are a group of macro generated functions that
  are used to check for the presence of pad and non-pad pixels in a block.
  Each function scans a full block of pixels looking for pad and non-pad
  value. Two flags are returned, one indicatin that the pad value was found
  and the other indicating that the data vluse was found.

  The individual variants of the function are generated by a macro that takes
  and integer type as an argument. The type is teh pixel type. Since binary
  conpares are done, using integer types will work for the equivalent float
  type. The only type not covered is colplex double which is listed as a type
  but not possible due an error in the specification.

  \ar blockIO  - Associated _nitf_ImageIOBlock object
  \ar padFound - Returns true if any pad values found
  \ar dataFound - Returns true if any data (non-pad) values found

  \return None
*/

typedef void (*_NITF_IMAGE_IO_PAD_SCAN_FUNC)
(struct _nitf_ImageIOBlock_s *blockIO,
 NITF_BOOL *padFound, NITF_BOOL *dataFound);

/*!
  \brief _nitf_ImageIO_writeMethod - Writing method codes

  The enum _nitf_ImageIO_writeMethod specifies which of various writing methods
  is being used by a particular nitf_ImageIOWriteControl object. It is used
  to prevent a mismatch between the method implied by the function that created
  the control object and the functionis that use it. For example, if the control
  object is created by nitf_ImageIO_writeSequential, only sequential write
  related methods can be used. To avoid mismatch errors, the control structure
  remembers how it was created.

*/

typedef enum
{
    SEQUENTIAL_ALL_BANDS = 1    /*!< Sequential writes, all bands */
} _nitf_ImageIO_writeMethod;

/*!
  \brief _nitf_ImageIOVtbl - Function vector table

  The vector table contains function pointers for functions that perform
  the operations in the read and write loops. The actual functions are
  selected when the nitf_ImageIO object is created. The selection is based
  on image segment characteristics such as blocking mode, compression type,
  and pixel type.

  The unformat and unformat functions handle issues such as byte ordering,
  pixel justification and sign extension (i.e. 12-bit pixel)
*/

typedef struct
{
    /*!< I/O setup function */
    _NITF_IMAGE_IO_CONTROL_FUNC setup;
    _NITF_IMAGE_IO_IO_FUNC reader;      /*!< Read function */
    _NITF_IMAGE_IO_IO_FUNC writer;      /*!< Write function */
    /*!< Unpacking function */
    _NITF_IMAGE_IO_PACK_FUNC unpack;
    _NITF_IMAGE_IO_PACK_FUNC pack;      /*!< Packing function  */
    /*!< Data unformat function */
    _NITF_IMAGE_IO_UNFORMAT_FUNC unformat;
    /*!< Data format function */
    _NITF_IMAGE_IO_FORMAT_FUNC format;
    /*!< I/O completion */
    _NITF_IMAGE_IO_CONTROL_FUNC done;
}
_nitf_ImageIOVtbl;

/*!
  \brief _nitf_ImageIO_MaskHeader - Blocking and pad mask definition

  The _nitf_ImageIO_MaskHeader structure is used to manage the block and pad
  masks which may be at the start of the image data segment for some images.
  This structure is not setup until the first I/O operation, hence the ready
  flag

  The mask structures are always set-up. For images that do not have mask
  information, the mask is created based on the image segment information
  supplied to the nitf_ImageIO constructor. This simplifies the read/write
which can be written to assume that the masks are always present.

*/

typedef struct
{
    int ready;                  /*!< Structure is setup if TRUE */
    nitf_Uint32 imageDataOffset;        /*!< Offset to actual image data past masks */
    nitf_Uint16 blockRecordLength;      /*!< Block mask record length */
    nitf_Uint16 padRecordLength;        /*!< Pad pixel mask record length */
    /*!< Pad pixel value length in bytes */
    nitf_Uint16 padPixelValueLength;
}
_nitf_ImageIO_MaskHeader;

/*!
  \brief _nitf_ImageIOPixelDef - Pixel definition

  The pixel definition strcture holds information desribing the image's
  pixel in terms of size, value type, and justification.

  The pad pixel is stored in the pad field as a formatted pixel in the correct
  byte order, exactly as it appears in the file.

*/

typedef struct
{
    nitf_Uint32 type;           /*!< Pixel type code */
    nitf_Uint32 bytes;          /*!< Pixel (unpacked) size in bytes */
    /*!< Pad value */
    nitf_Uint8 pad[NITF_IMAGE_IO_PAD_MAX_LENGTH];
    int swap;                   /*!< Byte swap if TRUE */
    nitf_Uint32 shift;          /*!< Shift count (in bits) for left justified pixels */
}
_nitf_ImageIOPixelDef;

/*!
  \brief _nitf_Offsets32 - Buffer offset 32-bit

  The buffer offset stucture maintains the current and original values of
  a 32 bit offset. The current value advances through the processing and is
  ocasionally returned to the original or base value
*/

typedef struct
{
    nitf_Uint32 mark;  /*!< The current offset */
    nitf_Uint32 orig;  /*!< The original (base) offset */
} _nitf_Offsets32;

/*!
  \brief _nitf_Offsets64 - Buffer offset 64-bit

  The buffer offset stucture maintains the current and original values of
  a 64 bit offset. The current value advances through the processing and is
  ocasionally returned to the original or base value
*/

typedef struct
{
    nitf_Uint64 mark;  /*!< The current offset */
    nitf_Uint64 orig;  /*!< The original (base) offset */
} _nitf_Offsets64;

/*!
  \brief _nitf_DataBuffer32 - Data buffer with offset, 32-bit

  The data buffer with offset structure combines the common variable pairing
  of a data buffer and associated offset. The buffer base address plus the
  offset points to the current location in the buffer. For this type, the
  offset is a 32-bit offset stucture with base and current offset
*/

typedef struct
{
    nitf_Uint8* buffer;
    _nitf_Offsets32 offset;
} _nitf_DataBuffer32;

/*!
  \brief _nitf_DataBuffer64 - Data buffer with offset, 64-bit

  The data buffer with offset structure combines the common variable pairing
  of a data buffer and associated offset. The buffer base address plus the
  offset points to the current location in the buffer. For this type, the
  offset is a 64-bit offset stucture with base and current offset
*/

typedef struct
{
    nitf_Uint8* buffer;
    _nitf_Offsets64 offset;
} _nitf_DataBuffer64;

/*!
  \brief _nitf_ImageIOParameters - Parameters structure

  The _nitf_ImageIOParameters structure contains configuration parameters
  that can be set by the user.

*/

typedef struct
{
    double noCacheThreshold;/*!< Request/Block size threshold for no caching */
    nitf_Uint32 clearCache; /*!< Clear cache after I/O operation */
}
_nitf_ImageIOParameters;

/*!
  \brief _nitf_ImageIOBlockCacheControl - Block cache control

  The _nitf_ImageIOBlockCacheControl structure manages the block cache used by
  the cached reader.

  If there is no block in a block buffer, the corresponding block number
  will be set to NITF_IMAGE_IO_NO_BLOCK.

  The freeFlag is necesary because for some blockingmodes, block I/O
  objects share block buffers

  The block buffers are allocated by the system memory allocation facility

The current implementation supports a cache of one block

*/

typedef struct
{
    nitf_Uint32 number;         /*!< Current block number */
    NITF_BOOL freeFlag;         /*!< Free buffer if TRUE */
    nitf_Uint8 *block;          /*!< Current block buffer */
}
_nitf_ImageIOBlockCacheControl;

/*!
  \brief _nitf_ImageIO - Object private data structure

  The _nitf_ImageIO structure is the actual instance data for the
  nitf_ImageIO object.

  The number of rows and columns per block are as in the NITF header.
  Blocking schemes that have more than one band in a block do not count
  a row or column for each band. For example, if IMODE is "B" (band
  interleaved by block) and NBANDS is 2 and NPPBV is 128, then numRowsPerBlock
  is 128 not 256.

The imageBase and pixelBase offset will differ if there is block or
pad pixel mask data which (The masks are stored at the from of the image
data and is part of the image data area.

The block mask is the array of offsets to blocks. This is read from the
image segments's block mask table. If no table is present one is constructed
as a linear array for uncompressed images or by the decompression plugin
for compressed images. The table has one extra entry at the end of the
table that points past the last block. This entry facilitates calculating
the block size by subtracting the offset  of the current and next block.
The extra entry avoids making the last block a special case. This
calculation is used to read compressed blocks.
*/

typedef struct
{
    nitf_Uint32 numRows;          /*!< Number of rows */
    nitf_Uint32 numColumns;       /*!< Number of columns */
    nitf_Uint32 numBands;         /*!< Number of bands */
    _nitf_ImageIOPixelDef pixel;        /*!< Pixel definition */
    nitf_Uint32 nBlocksPerRow;  /*!< Number of blocks per row */
    nitf_Uint32 nBlocksPerColumn; /*!< Number of blocks per column */
    nitf_Uint32 numRowsPerBlock;    /*!< Number of rows per block */
    nitf_Uint32 numColumnsPerBlock; /*!< Number of columns per block */
    size_t blockSize;               /*!< Block size in bytes */
    nitf_Uint32 nBlocksTotal;     /*!< Total number of blocks */
    nitf_Uint32 numRowsActual;    /*!< Actual number of rows */
    nitf_Uint32 numColumnsActual; /*!< Actual number of columns */
    nitf_Uint32 compression;    /*!< Compression type code */
    double compressionRate;     /*!< Compression type code */
    nitf_Uint32 blockingMode;   /*!< Blocking mode code */
    int blockInfoFlag;          /*!< Blocking information set if TRUE */
    nitf_BlockingInfo blockInfo;/*!< "Official" blocking information */
    nitf_Uint64 imageBase;      /*!< File offset to image data section */
    nitf_Uint64 pixelBase;      /*!< File offset to actual pixel data */
    nitf_Uint64 dataLength;     /*!< Length of the data including masks */
    /*!< Configuration parameters */
    _nitf_ImageIOParameters parameters;
    /*!< Block control */
    _nitf_ImageIOBlockCacheControl blockControl;
    /*!< Compression handler function */
    nitf_CompressionInterface *compressor;
    /*!< Decompression handler function */
    nitf_DecompressionInterface *decompressor;
    /*!< Compression control object */
    nitf_CompressionControl *compressionControl;
    /*!< Decompression control object */
    nitf_DecompressionControl *decompressionControl;
    int cachedWriteFlag;        /*!< Using caching writes if TRUE */
    /*!< Block and pad mask header */
    _nitf_ImageIO_MaskHeader maskHeader;
    nitf_Uint64 *blockMask;     /*!< Block mask */
    nitf_Uint64 *padMask;       /*!< Pad pixel mask */
    _nitf_ImageIOVtbl vtbl;     /*!< Function vector table */
    int oneBand;                /*!< Read/write one band at a time if TRUE */
    /*!< Control structure for current write */
    struct _nitf_ImageIOWriteControl_s *writeControl;
    /*!< Control structure for current read */
    struct _nitf_ImageIOReadControl_s *readControl;
    _NITF_IMAGE_IO_PAD_SCAN_FUNC padScanner; /*! Scans for pad pixels in write */
}
_nitf_ImageIO;

/*!
  \brief _nitf_ImageIOControl - IO control structure

  The I/O control structure manages the information needed to process one
  read or write request. It is a container object for the _nitf_ImageIOBlock
  structures. Each block IO strcuture controls access to one band of one
  block column. The array is indexed by block columns and band as follows:

     cntl->bklIO[column][band]

  The userBase field is an array of pointers, one for each requested
band. The user is likely to want to put the bands in non-continuous buffers.

The padded field is a flags that indicates that pad pixels are
included. It is used by read operations.

The blockIO field is a two dimensional array of _nitf_ImageIOBlock
The ioCount field gives the total number of actual I/O operations.
This field be used by an implementation of the file I/O operations to
do deferred I/O. This would allow seek optimization. The count will include
pad pixel operations. The ioCountDown field is initialized to the same
value as ioCount an can be used by the I/O function to keep track of how
many I/Os have been done.

The downSampling, downSampleIn, and downSampleOut fields support
down-sampling. The first field is a flag indicating that the read includes
down-sampling. The other two are arrays of pointers to the input and output
buffers for the down-sample function. These arrays contain one pointer for
each band.

See ithe documentation of _nitf_ImageIOBlock for information on down-sampling
and "FR" and "DR" fields.
*/

typedef struct _nitf_ImageIOControl_s
{
    /*! Parent nitfImageIO object */
    _nitf_ImageIO *nitf;

    /*! Number of row in the sub-window */
    nitf_Uint32 numRows;

    /*! Start row in the main image */
    nitf_Uint32 row;

    /*! Row skip factor */
    nitf_Uint32 rowSkip;

    /*! Number of columns in the sub-window */    
    nitf_Uint32 numColumns;

    /*! Start column in the main image */
    nitf_Uint32 column;

    /*! Column skip factor */
    nitf_Uint32 columnSkip;

    /*! Array of bands to read/write */
    nitf_Uint32 *bandSubset;

    /*! Number of bands to read/write */
    nitf_Uint32 numBandSubset;

    /*! Base user buffer, one pointer per band */
    nitf_Uint8 **userBase;

    /*! Operation is read if TRUE */
    int reading;

    /*! Operation if down-sample if TRUE */
    int downSampling;

    /*! Array of input pointers for down-sample */
    NITF_DATA **downSampleIn;

    /*! Array of output pointers for down-sample */
    NITF_DATA **downSampleOut;

    /*! Number of _nitf_ImageIOBlock structures */
    nitf_Uint32 nBlockIO;

    /*! Array of _nitf_ImageIOBlock structures */
    struct _nitf_ImageIOBlock_s **blockIO;

    /*! Block number next row increment  */
    nitf_Uint32 numberInc;

    /*! Data's offset in block next row byte increment */
    nitf_Uint64 blockOffsetInc;

    /*! Read/write buffer next row byte increment */
    nitf_Uint32 bufferInc;

    /*! Unpacked data buffer next row byte increment */
    nitf_Uint32 unpackedInc;

    /*! User data buffer next row byte increment */
    nitf_Uint32 userInc;

    /*! Pad pixel buffer */
    nitf_Uint8 *padBuffer;

    /*! Pad pixel buffer size in bytes */
    nitf_Uint32 padBufferSize;

    /*! Operation involved pad pixels */
    int padded;

    /*! Total I/O count */
    size_t ioCount;

    /*! I/O done count down */
    size_t ioCountDown;

    /*! Save buffer for partial down-sample windows */
    nitf_Uint8 *columnSave;
}
_nitf_ImageIOControl;

/*!
  \brief _nitf_ImageIOBlock - Block I/O structure

  The block IO structure handles one segment of one band of the current row.
  After all row sements have been read/written the offsets are updated for
  the next row.

  The user data buffer is part of the buffer supplied by the user. In
  some blocking methods, the data can be read/written directly to/from the
  user buffer and the user and buffer pointers will be the same. This is
  not the case in the band interleaved by pixel blocking mode. Also the
user and write buffers must be different any time the data must be
formatted (i.e., byte swapped).

The unpacked buffer provides an additional level of buffering when the
user requets down-sampling. Data is read full-resolution into the read
read buffer and than unpacked into the unpacked buffer. After a complete
sample window of rows have been read and unpacked, the data is down-sampled
into the user buffer. Unformatting is done in-place in the unpacked buffer.
The unpacked buffer will contain one sample window more columns than the
full resolution pixel read count. This extra space is used to hold a
sample window that is split between block columns.

The actual current location in the user, unpacked and read/write buffers is
defined by the buffer pointers and offsets:

user + userOffset         - Current user buffer pointer
unpacked + unpackedOffset - Current unpacked buffer pointer
buffer + bufferOffset     - Current readWrite buffer pointer

The offset is updated, not the pointer.

At the end of the read the input buffer offset points to the begining of
the read data. This offset may be adjusted by the read function on a read
by read basis. In the simplier cases, is is initialize to point to the
user buffer and incremented by nitf_ImageIO_nextRow.

A blockMask pointer is maintained in the IO structure to handle block
sequential which has seperate masks for each band.

The actual file offset for any read/write is:

pixelBase + imageDataOffset + blockOffset.mark

The blockOffset.mark is updated each row, the imageDataOffset is updated at
the end of the current block;

The rowsUntil count is decremented after the block operations and so is set
to one less than the number of rows in the current block. For example, if
the number of blocks to be processed in the block is 1 the rowsUntil counter
is set to 0. After the first row of blocks, the counter is set to one less
than the number of rows in a block. This works because this is always right
for all rows of blocks except the first and last and in the last it is OK to
put in a to big value since there is a separate counter keeping track of the
rows processed.

The userEqBuffer flag indicates that the user buffer is the read/write
buffer.  This is useful for some read/write modes where the address of the
user buffer is not known at construct time. For example, sequential write
were the construction of the control object is done in a separate call from
the write row operations. In this case the user and buffer fields are
initialized zero and updated at each call. The flag tells the writer whether
the buffer field has to be modified. The flag would be set in the band
interleaved by row blocking mode but not in band interleaved by pixel.

The pad counts are used to indicate that pad pixels are required at the
end of the line or pad rows at the end of the image. This type of padding
occurs when the image size in the row or column dimension is not an
interger multiple of the corresponding block dimension. The pad column
count is the number of pad bytes that must be added to the end of the row.
the count is bytes not pixels. This field is only non-zero in blocks that
are in the last block column. The writeer uses this value as a flag
indicating the need to write pad pixels. The pad row count gives the number
of pad rows that need to be added to the end of the image. sice each block
I/O object represents a column of blocks for one band, all blocks have the
same row count. The current row argument gives the current row in the image
(not the row in the sub-image).  This field is required to support pad row
writes. The write loop write function (vtbl write function). Pad rows are
written when the current row is the last row in the image. This field is
not used by the reader

The pixel count is the number of pixels associated with the block operation
The format count is the number of pixels to be formatted before write. In
the "P" blocking mode, formatting is done at the time of the write and this
is only done once per block column. The write on the last band. The entire
write for all bands is done at this time and the formatting is also deferred
till then. Therefore the format count must be the pixelCountFR times the
number of bands, hence the separate variable.

Image down-sampling causes the user and internal read buffers and blocks to
have different resolutions, full resolution (internal buffers) and down-
sampled resolution (user buffers). Some variables and offsets are in full
resolution "units" and some in down-sampled "units". Variables and fields
ending in "FR" are full resolution and those ending in "DR" are down-
sample resolution. When not down-sampling or when writing, FR == DR.

The sample start fields give the offset in bytes to the first column of
the first complete sample window in a row of sample windows. Data on the
current row before this offset belongs to the partial window of the last
sample window of the corresponding block in the previous block column. This
value is used to line-up the read in the buffer so that data saved from the
previous block column can be used to complete the first sample window.

sampleStartColumn is zero based. So if "residual" is the number of sample
window columns for the last partial sample window in the previous block
column, then: residual + sampleStartColumn == columnSkip. The residual
columns and the first sampleStartColumn columns form the sample window that
is used to form the first pixel that goes in the user buffer for the current
block. If sampleStartColumn is 0, then there was a complete sample window at
the end of the previous block column and no partial neighborhood assembly
is required.

myResidual is the residual amount from the current block column read that
will be left for the next read. If blockA and blockB are in consecutive
block columns (blockA first) the blockA->residual == blockB->myResidual
*/

typedef struct _nitf_ImageIOBlock_s
{

    /*! Associated nitf_ImageIOControl object  */
    _nitf_ImageIOControl *cntl;

    /*! Band associated with this I/O */
    nitf_Uint32 band;

    /*! Do the read/write if TRUE */
    int doIO;

    /*! Block number */
    nitf_Uint32 number;

    /*! Rows until next block */
    nitf_Uint32 rowsUntil;

    /*! Block mask to use */
    nitf_Uint64 *blockMask;

    /*! Pad pixel mask to use */
    nitf_Uint64 *padMask;

    /*! Block's offset from image base */
    nitf_Uint64 imageDataOffset;

    /*! Current and base offset into the block */
    _nitf_Offsets64 blockOffset;

    /*! Read/Write data buffer plus offsets */
    _nitf_DataBuffer64 rwBuffer;

    /*! Unpacked data buffer plus offsets */
    _nitf_DataBuffer32 unpacked;

    /*! Free unpacked data buffer if TRUE */
    nitf_Uint32 unpackedNoFree;

    /*! User buffer plus offsets */
    _nitf_DataBuffer32 user;

    /*! Read/write buffer is user buffer if TRUE */
    nitf_Uint32 userEqBuffer;

    /*! Read count in bytes */
    size_t readCount;

    /*! Pixel count for this operation, FR */
    size_t pixelCountFR;

    /*! Pixel count for this operation, DR */
    size_t pixelCountDR;

    /*! Format count for this operation */
    size_t formatCount;

    /*! Byte count of pad pixel columns to write */
    nitf_Uint32 padColumnCount;

    /*! Row count of pad pixel rows to write */
    nitf_Uint32 padRowCount;

    /*! Start column of the first sample window */
    nitf_Uint32 sampleStartColumn;

    /*! Partial sample columns previous block */
    nitf_Uint32 residual;

    /*! Partial sample columns current block */
    nitf_Uint32 myResidual;

    /*! Current row in the image */
    nitf_Uint32 currentRow;

    /*! Block control for cached write */
    _nitf_ImageIOBlockCacheControl blockControl;
}
_nitf_ImageIOBlock;

/*!
  \brief _nitf_ImageIOWriteControl - Write control structure

  _nitf_ImageIOWriteControl is the object used to control the writing
  of image data for an ImageIO object. The object does not have a public
  constructor but rather is created by one of a set of setup function.
  Each of these functions configures the write control object for a
  particular writing method, such as sequential writes.

  The ImageIO structure maintains a pointer to the current write control.
  There can be only one active reader or writer for each ImageIO.

This is an internal object and is not used directly by the user.

*/

typedef struct _nitf_ImageIOWriteControl_s
{
    /*!< Writing method code */
    _nitf_ImageIO_writeMethod method;
    _nitf_ImageIOControl *cntl; /*!< Associated control structure */
    nitf_Uint32 nextRow;        /*!< Next row to write (sequential) */
}
_nitf_ImageIOWriteControl;

/*!
  \brief _nitf_ImageIOReadControl - Read control structure

  _nitf_ImageIOReadControl is the object used to control the reading
  of image data for an ImageIO object. The object does not have a public
  constructor but rather is created by one of a set of setup function.
  Each of these functions configures the read control object for a
  particular writing method, such as sequential reads.

  The ImageIO structure maintains a pointer to the current read control.
  There can be only one active reader or writer for each ImageIO.

This is an internal object and is not used directly by the user.

*/

typedef struct _nitf_ImageIOReadControl_s
{
    /*! Associated control structure */
    _nitf_ImageIOControl *cntl;
}
_nitf_ImageIOReadControl;

/*!
  \brief nitf_ImageIO_BPixelControl - The actual implementation beneath the
  opaque decompression control pointer
 */

typedef struct _nitf_ImageIO_BPixelControl
{

    /*! Saved open argument */
    nitf_IOInterface* io;

    /*! Saved open argument */
    nitf_Uint64 offset;

    /*! Saved open argument */
    nitf_BlockingInfo *blockInfo;
    
    /*! Saved open argument */
    nitf_Uint64 *blockMask;

    /*! Size of compressed block in bytes */
    size_t blockSizeCompressed;

    /*! Buffer for compressed block */
    nitf_Uint8 *buffer;
}
nitf_ImageIO_BPixelControl;

/*!
  \brief nitf_ImageIO_12PixelControl - The actual implementation beneath the
  opaque decompression control pointer
 */

typedef struct _nitf_ImageIO_12PixelControl
{

    /*! Saved open argument */
    nitf_IOInterface* io;

    /*! Saved open argument */
    nitf_Uint64 offset;

    /*! Saved open argument */
    nitf_BlockingInfo *blockInfo;

    /*! Saved open argument */
    nitf_Uint64 *blockMask;

    /*! Odd number of pixels in block flag 0 or 1, one means odd */
    nitf_Uint32 odd;

    /*! Number of pixels in block */
    size_t blockPixelCount;

    /*! Size of compressed block in bytes */
    size_t blockSizeCompressed;

    /*! Buffer for compressed block */
    nitf_Uint8 *buffer;
}
nitf_ImageIO_12PixelControl;

/*!
  \brief nitf_ImageIO_12PixelComControl - The actual implementation beneath the
  opaque compression control pointer
 */

typedef struct _nitf_ImageIO_12PixelComControl
{

    /*! Saved open argument */
    nitf_IOInterface* io;

    /*! Saved start argument */
    nitf_Uint64 offset;

    /*! Saved start argument */
    nitf_Uint64 dataLength;

    /*! Saved start argument */
    nitf_Uint64 *blockMask;

    /*! Saved start argument */
    nitf_Uint64 *padMask;

    /*! Odd number of pixels in block flag 0 or 1, one means odd */
    nitf_Uint32 odd;

    /*! Number of pixels in block */
    size_t blockPixelCount;

    /*! Size of uncompressed block in bytes */
    size_t blockSizeUncompressed;

    /*! Size of compressed block in bytes */
    size_t blockSizeCompressed;

    /*! Amount of data written so far */
    nitf_Uint64 written;

    /*! Buffer for compressed block */
    nitf_Uint8 *buffer;
}
nitf_ImageIO_12PixelComControl;

/*!
  \brief nitf_ImageIO_decodeModes - Decode compression and blocking mode
  strings

  nitf_ImageIO_decodeModes is the function used to decode compression,
  blocking mode and pixel type strings it sets the _nitf_ImageIO fields
  "compression","blockingMode", and the "vtbl" fields "setup", "reader",
  "writer", and "done"

  Note:

This is an internal function and is not intended to be called
directly by the user.

\return Returns FALSE on error

On error, the supplied error object is set. Possible errors include:

Invalid type code
*/

NITFPRIV(int) nitf_ImageIO_decodeModes(_nitf_ImageIO * nitf,
                                       nitf_ImageSubheader *subhdr,
                                       nitf_Error * errorBuffer);

/*!
 * Reverts the optimization modes that were added 10/2007 by tzellman.
 * This is called when:
 * - the user requests to read data, but does not want it 
 * in the RGB24 or IQ single-band format
 * - the user writes data
 * \param nitfI        the ImageIO structure
 * \param numBands    the number of bands (when reading), or 0 when writing.
 */
NITFPRIV(void) nitf_ImageIO_revertOptimizedModes(_nitf_ImageIO *nitfI,
                                                 int numBands);


/*!
  \brief nitf_ImageIO_setIO - Set the reader and writer functions

  nitf_ImageIO_setIO sets the pack and unpack functions in the vtbl field
  in the nitf argument. The functions are selected based on the options set
  in the nitf_ImageIO structure. The relevant options are compression,
  blocking mode and pixel type. This function must be called after the
  functions that initialize the correspondin fields.

  \b Note:

This is an internal function and is not intended to be called directly
by the user.

\return None

*/

/*!< nitf_ImageIO object */
NITFPRIV(void) nitf_ImageIO_setIO(_nitf_ImageIO * nitf);

/*!
  \brief nitf_ImageIO_setPixelDef - Set the pixel definition

  nitf_ImageIO_setPixelDef sets the pixel definition structure in the
  nitf argument. All of the fields of the _nitf_ImageIO "pixel" field
  are set as well as the "vtbl" "unformat" and "format" fields

  The pad pixel value (field "pad") is not set. It is initialized by
  nitf_ImageIO_initMaskHeader when the mask header (which contains the value)
  is read.

  Note: This is an internal function and is not intended to be called
  directly by the user.
  
  \return Returns NITF_FAILURE on error
  
  On error, the supplied error object is set. Possible errors include:
  
  Unsupported options

  \param nitf Image IO object
  \param pixelType the pixel type
  \param nBitsActual The actual number of bits per pixel
  \param justify The pixel justification
  \param error An error to populate on failure
  
*/

NITFPRIV(int) nitf_ImageIO_setPixelDef(_nitf_ImageIO * nitf,
                                       char *pixelType, 
                                       nitf_Uint32 nBits,
                                       nitf_Uint32 nBitsActual,
                                       char *justify,
                                       nitf_Error * error);

/*!
  \brief nitf_ImageIO_setUnpack - Set the pack and unpack functions

  nitf_ImageIO_setUnpack sets the pack and unpack functions in the vtbl
  field in the nitf argument. The functions are selected baseded on the
  options set in the nitf_ImageIO structure. The relevant options are
  compression, blocking mode and pixel. This function must be called after
  the functions that initialize the correspondin fields.

  \b Note:

This is an internal function and is not intended to be called directly
by the user.

\return None

*/

/*!< nitf_ImageIO object */
NITFPRIV(void) nitf_ImageIO_setUnpack(_nitf_ImageIO * nitf);

/*!
  \brief nitf_ImageIO_allocBlockArray -  Allocate the IO control structure's
  block array.

  nitf_ImageIO_allocBlockArray allocates the IO control structure's block
  array. The array is organized as a two dimensional array indexed by block
  column and band. The allocated _nitf_ImageIOBlock structures are
  initialized to zero.

  The array is accessed:

array[column][band]

\return A pointer to the array or NULL on error. On error, the error object
is set.

Possible errors:

Memory allocation error
*/

/*!< Number of block columns */
/*!< Number of bands */
NITFPRIV(_nitf_ImageIOBlock **) nitf_ImageIO_allocBlockArray(nitf_Uint32 numColumns, nitf_Uint32 numBands, nitf_Error * error       /*! Error object */
                                                            );

/*!
  \brief nitf_ImageIO_freeBlockArray - Free block array

  nitf_ImageIO_freeBlockArray frees the array allcoated by the function
  nitf_ImageIO_allocBlockArray. The array pointer is passed by reference
  and zeroed on return.

  \return None
*/

/*!< The array to free */
NITFPRIV(void) nitf_ImageIO_freeBlockArray(_nitf_ImageIOBlock *** blockIOs);

/*!
  \brief nitf_ImageIO_setup_SBR - Do read/write setup for the "S", "B", and "R"
   blocking modes

  nitf_ImageIO_setup_SBR does read/write setup for the "S" (band sequential),
  "B" (band interleaved by block) and "R" (band interleaved by row)
  blocking modes. It is called through the _nitf_ImageIO vtbl setup field
  (setup pointer) by nitf_ImageIO_read and nitf_ImageIO_write. All of the
  required information is in the "cntl" argument and its parent _nitf_ImageIO
  object. Fields in "cntl" are modified.

\b Note:

This is an internal function and is not intended to be called
directly by the user.

\return Returns FALSE on error

On error, FALSE is returned and the error object is set.

Possible errors include:

Memory allocation error
*/

/*!< nitf_ImageIO I/O control object */
/*!< Error object */
int nitf_ImageIO_setup_SBR(_nitf_ImageIOControl * cntl,
                           nitf_Error * error);

/*!

  \brief nitf_ImageIO_done_SBR - Do read/write compeltion for the "S", "B"
  and "R" blocking modes

  nitf_ImageIO_done_SBR does read/write completion for the "S" (band
  sequential), "B" (band interleaved by block) and "R" (band interleaved by
  row) blocking modes. It is called through the _nitf_ImageIO vtbl field
 (done pointer) by nitf_ImageIO_read and nitf_ImageIO_write. All of the
  required information is in the "cntl" argument and its parent
  _nitf_ImageIO object. Fields in "cntl" are modified.

\b Note: This is an internal function and is not intended to be called
directly by the user.

\return Returns FALSE on error (int)

On error, FALSE is returned and the error object is set.

Possible errors include:

I/O Errors
*/

/*!< nitf_ImageIO I/O control object */
/*!< Error object */
int nitf_ImageIO_done_SBR(_nitf_ImageIOControl * cntl, nitf_Error * error);

/*!
  \brief nitf_ImageIO_setup_P - Do read/write setup for the "P" blocking mode

  nitf_ImageIO_setup_P does read/write setup for the "P" (band interleaved by
  pixel) blocking mode. It is called through the _nitf_ImageIO vtbl field
  (setup pointer) by nitf_ImageIO_read and nitf_ImageIO_write. All of the
  required information is in the "cntl" argument and its parent
  _nitf_ImageIO object. Fields in "cntl" are modified.

  \b Note:

This is an internal function and is not intended to be called
directly by the user.

\return Returns FALSE on error

On error, FALSE is returned and the error object is set.

Possible errors include:

Memory allocation error
*/

/*!< nitf_ImageIO I/O control object */
/*!< Error object */
int nitf_ImageIO_setup_P(_nitf_ImageIOControl * cntl, nitf_Error * error);


/*!

  \brief nitf_ImageIO_done_P - Do read/write compeltion for the "P" blocking
  mode

  nitf_ImageIO_done_P does read/write completion for the "P" (band
  interleaved by pixel) blocking mode. It is called through the
  _nitf_ImageIO vtbl field (done pointer) by nitf_ImageIO_read and
  nitf_ImageIO_write. All of the required information is in the "cntl"
  argument and its parent _nitf_ImageIO object. Fields in "cntl" are modified.

\b Note: This is an internal function and is not intended to be called
directly by the user.

\return Returns FALSE on error (int)

On error, FALSE is returned and the error object is set.

Possible errors include:

I/O Errors
*/

/*!< nitf_ImageIO I/O control object */
/*!< Error object */
int nitf_ImageIO_done_P(_nitf_ImageIOControl * cntl, nitf_Error * error);

/*!
  \brief nitf_ImageIOControl_construct - Constructor for _nitf_ImageIOControl
   object

  nitf_ImageIOControl_construct is the constructor for the _nitf_ImageIOControl
  object. Although this is a "C" library, it is designed using object oriented
  techniques and is implemented to facilitate use by higher level languages
  such as C++. The returned object must be freed by
  nitf_ImageIOControl_destruct.

  The control structure manages the I/O operations required to read/write
one or more bands from a subwindow. Once set by this function, the bands
and subwindow cannot be changed (a new object must be created).

The subwindow should be validated by the caller

The user pointers argument should have a pointer to an sub-window sized
buffer for each requested band. For writing, there is a creation call
(i.e., nitf_ImageIO_writeSequential) followed by a write call. In this
case the user data pointers are not required for the creation call and
the users argument will be a NULL pointer

The band list in the nitf_SubWindow is an array of band numbers. Band numbers
are zero based and follow the physical ordering of the bands in the NITF
file. The band array list is copied so the caller version does not have to
be persistent.  If all bands are requested, the list can be a NULL pointer.

\b Note:

This is an internal function and is not intended to be called
directly by the user.

\return The new _nitf_ImageIOControl or NULL on error

On, error the error object is set.

Possible errors include:

Unimplemented
Memory allocation error
*/

/*!< nitf_ImageIO object */
/*!< IO handle for read */
NITFPRIV(_nitf_ImageIOControl *) nitf_ImageIOControl_construct(_nitf_ImageIO * nitf, nitf_IOInterface* io, nitf_Uint8 ** user,  /*!< User data buffer pointers */
        nitf_SubWindow * subWindow,      /*!< Sub-window to process */
        int reading,     /*!< Reading if TRUE, else writing */
        nitf_Error * error       /*!< Error object */
                                                              );

/*!
  \brief nitf_ImageIOControl_destruct - Destructor for the _nitf_ImageIOControl
   object

  nitf_ImageIOControl_destruct is the destructor for the _nitf_ImageIOControl
  object. Althouh this is a "C" library, it is designed using object
  oriented techniques and is implemented to facilitate use by higher level
  lanuages such as C++.

  This function deallocates the memory associated width the "cntl" argument
  and so this handle is not valid after this call.

The blocking specific "done" function should be called before the control
object is deleted

The argument is set to NULL on return

\b Note:

This is an internal function and is not intended to be called
directly by the user.

\return None

*/

/*!< Pointer to nitf_ImageIO I/O control object */
NITFPRIV(void) nitf_ImageIOControl_destruct(_nitf_ImageIOControl ** cntl);

/*!
  \brief nitf_ImageIOWriteControl_construct - Allocate and initialize a write
  control object

  nitf_ImageIOWriteControl_construct allocates and initializes a write control.

  The caller supplied I/O control object is the one created by the calling
  write setup function. The control object will be destroyed by the function
  nitf_ImageIOWriteControl_writeDone

  \return Returns FALSE on error

On error, the supplied error object is set. Possible errors include:

Memory allocation error
*/

/*!< Associated I/O control object */
/*!< Associated IO handle */
NITFPRIV(_nitf_ImageIOWriteControl *) nitf_ImageIOWriteControl_construct(_nitf_ImageIOControl * cntl, nitf_IOInterface* io, _nitf_ImageIO_writeMethod method,   /*!< Writing method that will be used */
        nitf_Error * error     /*!< Error object */
                                                                        );

/*!
  \brief nitf_ImageIOWriteControl_destruct - Destructor for the write control
  object

  The argument is set to NULL on return
  */

/*!< Pointer to write control object */
NITFPRIV(void) nitf_ImageIOWriteControl_destruct(_nitf_ImageIOWriteControl
        ** cntl);

/*!
  \brief nitf_ImageIOReadControl_construct - Consructor for the read control
  object

  nitf_ImageIOReadControl_construct allocates and initializes a read control.

  The caller supplied I/O control object is the one created by the calling
  read setup function.

  \return Returns FALSE on error

On error, the supplied error object is set. Possible errors include:

Memory allocation error
*/

/*!< Associated I/O control object */
/*!< Sub-window to read */
NITFPRIV(_nitf_ImageIOReadControl *) nitf_ImageIOReadControl_construct(_nitf_ImageIOControl * cntl, nitf_SubWindow * subWindow, nitf_Error * error      /*!< Error object */
                                                                      );

/*!
  \brief nitf_ImageIOReadControl_destruct - Destructor for the read control
  object

  The argument is set to NULL on return
  */

/*!< Pointer to read control object */
NITFPRIV(void) nitf_ImageIOReadControl_destruct(_nitf_ImageIOReadControl **
        cntl);

/*!
  \brief nitf_ImageIO_bigEndian - returns TRUE on big endian machines and
   FALSE on little endian machines.

  nitf_ImageIO_bigEndian returns TRUE on big endian machines and FALSE on
  little endian machines. The NITF specification states that pixel values
  are stored in network byte order (bi endian)

  \bNote:

  This is an internal function and is not intended to be called
directly by the user.

\return 1 or 0

*/

NITFPRIV(int) nitf_ImageIO_bigEndian(void);

/*!
  \brief nitf_ImageIO_checkSubWindow - Check a sub-window

  nitf_ImageIO_checkSubWindow is the internal function used to to check a
  sub-window. The sub-window is checked to verify that it is completely in the
  image and that the band subset includes valid bands.

  The tests are made against the actual image dimensions not the padded
  dimensions that can result from image blocking.

  If the sub-window specifies down-sampling (row or column skip != 1), then
the full resolution image is checked. The last sampling window irequested
can be partially out of range.

\b Note:

This is an internal function and is not intended to be called
directly by the user.

\return Returns FALSE on error (int)

On error, the supplied error object is set. Possible errors include:

Invalid dimension or band
*/

/*!< The nitf_ImageIO object used do the check */
/*!< The sub-window to check */
NITFPRIV(int) nitf_ImageIO_checkSubWindow(_nitf_ImageIO * nitf, nitf_SubWindow * subWindow, int *all,   /*!< Returns TRUE if the sub-window is the entire image */
        nitf_Error * error    /*!< Error object */
                                         );

/*!
  \brief nitf_ImageIO_checkOneRead - Check for single read case

  nitf_ImageIO_checkOneRead determines if the uer request can be satisfied
  in a single read operation. This will happen in three cases:

  If A:

    The image has only one block
    The full image is being read
    The blocking mode is "B"
No compression (for now)

Or B:

The image has only one block per column
The full image is being read
The blocking mode is "S"
No compression (for now)

Or C:
The image has only one band
The image has only one block per column
The full image is being read
The blocking mode is "B"
No compression (for now)

\return TRUE is returned if the request can be done in one read

*/

/*!< The NITF object internal data */
/*!< Entire image is being read if TRUE */
NITFPRIV(NITF_BOOL) nitf_ImageIO_checkOneRead(_nitf_ImageIO * nitfI,
        NITF_BOOL all);

/*!
  \brief nitf_ImageIO_mkMasks - Make the block and pad pixel masks

  nitf_ImageIO_mkMasks makes the block and pad pixel masks. It also sets
  up the pad pixel value. If reading, the masks are read from the file if
  they are present. Otherwise, a linear mask (no pad blocks) is created for
  the block mask and an empty pad mask (all blocks marked pad free). A pad
  pixel value if present, overrides one specified by the user

  The pixelBase offset in the parent nitf_ImageIO structure is adjusted based
  on the mask data (if any)

The "reading" argument indicates that the call is part of a read operation.
If TRUE the function will read any available mask information from the file.

\b Note:

This is an internal function and is not intended to be called
directly by the user.

\return FALSE is returned on error

On error, the error object object is set. Possible errors include:

Memory allocation error
I/O Error
*/

/*!< The ImageIO object */
/*!< IO handle for reads */
NITFPRIV(int) nitf_ImageIO_mkMasks(nitf_ImageIO * img, nitf_IOInterface* io, int reading,       /*!< Read operation if TRUE */
                                   nitf_Error * error   /*!< Used for error handling */
                                  );

/*!
  \brief nitf_ImageIO_initMaskHeader - Initialize the mask header structure
   in the _nitf_ImageIO structure.

  nitf_ImageIO_initMaskHeader initializes the mask header structure in the
  _nitf_ImageIO structure. It may read the information from the file or
  construct a default one.

  The "reading" argument indicates that the call is part of a read operation.
  If TRUE the function will read any available mask information from the file.

\b Note:

This is an internal function and is not intended to be called
directly by the user.

\return FALSE is returned on error

On error, the error object is set. Possible errors include:

I/O Error
*/

/*!< The ImageIO object */
/*!< IO handle for reads */
NITFPRIV(int) nitf_ImageIO_initMaskHeader(_nitf_ImageIO * nitf, nitf_IOInterface *io, nitf_Uint32 blockCount,   /*!< Total number of blocks in the file */
        int reading,  /*!< Read operation if TRUE */
        nitf_Error * error    /*!< Used for error handling */
                                         );

/*!
  \brief nitf_ImageIO_readMaskHeader - Read the mask header structure

  nitf_ImageIO_readMaskHeader reads the mask header from the file and uses
  the values to initialize the mask header strcuture in the ImageIO object.
  The values are byte swapped if needed. The blockMask file in the ImageIO
  object is modified.

  \return FALSE is returned on error and the error object is set
*/

/*!< The ImageIO object */
/*!< IO handle for reads */
NITFPRIV(int) nitf_ImageIO_readMaskHeader(_nitf_ImageIO * nitf, nitf_IOInterface* io, nitf_Error * error        /*!< Used for error handling */
                                         );

/*!
  \brief nitf_ImageIO_swapMaskHeader - Byte swap the mask header

  nitf_ImageIO_swapMaskHeader byte swaps the mask header. The mask
  header, which is a binary structure in the NITF file, needs to
  be byte swapped in little endian machines. Swapping is required
  before writing and afetr reading

  \return None
*/

/*<! the header to byte swap */
NITFPRIV(void) nitf_ImageIO_swapMaskHeader(_nitf_ImageIO_MaskHeader *
        header);

/*!
  \brief nitf_ImageIO_writeMasks - Write the masks

  nitf_ImageIO_writeMasks writes the mask header, block mask and pad mask.
  The header is written if the "CI" field indicates masking. The masks are
  written based on the mask header. Either mask or both may not be written
  even if a header is written

  \return FALSE is returned on error and the error object is set
*/

/*!< The ImageIO object */
/*!< IO handle for reads */
NITFPRIV(int) nitf_ImageIO_writeMasks(_nitf_ImageIO * nitf, nitf_IOInterface* io, nitf_Error * error    /*!< Used for error handling */
                                     );

/*!
  \brief nitf_ImageIO_oneRead      Do the read request as a single read

  nitf_ImageIO_oneRead is called when circumstances allow the total request
  to be done in one read. The cntl argument must be setup via
  nitf_ImageIOControl_construct prior to calling this function.

  Actually only one band is read and this function is called for
  each band in a multi-band request.

  The information needed to do the read is taken from the control
structure.

\b Note:

This is an internal function and is not intended to be called
directly by the user.

\return Returns FALSE on error

On error, the error object is set. Possible errors include:

I/O error
*/

/*!< The control structure */
/*!< I/O handle */
NITFPRIV(int) nitf_ImageIO_oneRead(_nitf_ImageIOControl * cntl, nitf_IOInterface* io, nitf_Error * error        /*!< Error object */
                                  );

/*!
  \brief nitf_ImageIO_readRequest - Do the read request

  nitf_ImageIO_readRequest does the actual reading and formatting of a user
  read request that requires multiple reads or decompression. The cntl
  argument must be setup via nitf_ImageIO_construct prior to calling this
  function.

  \b Note:

  This is an internal function and is not intended to be called
directly by the user.

On error, FALSE is returned and error is set.

Possible errors include:

I/O error
*/

/*!< The control structure */
/*!< I/O handle */
NITFPRIV(int) nitf_ImageIO_readRequest(_nitf_ImageIOControl * cntl, nitf_IOInterface* io, nitf_Error * error    /*!< Error object */
                                      );

/*!
  \brief nitf_ImageIO_readRequestDownSample - Do the read request with
  down-smapling

  nitf_ImageIO_readRequestDownSample does the actual reading and formatting
  of the data. This version differs from nitf_ImageIO_readRequest in that it
  supports down-sampling.

  \b Note:

  This is an internal function and is not intended to be called
directly by the user.

On error, FALSE is returned and error is set.

Possible errors include:

I/O error
*/

/*!< The control structure */
/*!< Associated sub-window object */
NITFPRIV(int) nitf_ImageIO_readRequestDownSample(_nitf_ImageIOControl * cntl, nitf_SubWindow * subWindow, nitf_IOInterface* io, /*!< I/O handle */
        nitf_Error * error     /*!< Error object */
                                                );

/*!
  \brief nitf_ImageIO_allocatePad - Allocate pad pixel buffer

  nitf_ImageIO_allocatePad allocates pad pixel buffer. This buffer is used
  to reading/writing pad pixels. A pointer to the buffer is stored in the
  I/O control structure field padBuffer and it size in bytes is padBufferSize;

  If the buffer is unformatted if the I/O control is reading and formatted if
  it is writing (using the vector table unformat or format function)

  \return FALSE is returned on error and the error object is set

Possible errors:

Memory allocation error
*/

/*!< Associated I/O control structure */
/*!< Error object */
NITFPRIV(int) nitf_ImageIO_allocatePad(_nitf_ImageIOControl * cntl,
                                       nitf_Error * error);

/*!
  \brief nitf_ImageIO_readPad - Read pad pixels

  nitf_ImageIO_readPad does a simulated read supplying pad data instead of
  reading. This function allocates the pad pixel buffer if has not been
  initialized.

  \b Note:

  This is an internal function and is not intended to be called
  directly by the user.

\return Returns FALSE on error. On error, error object is set.

Possible errors include:

Memory allocation error
*/

/*!< NITF block structure */
/*!< Error object */
NITFPRIV(int) nitf_ImageIO_readPad(_nitf_ImageIOBlock * blockIO,
                                   nitf_Error * error);

/*!
  \brief nitf_ImageIO_readFromFile - Read data from a file

  nitf_ImageIO_readFromFile reads data from a file at a specified offset.
  This function is used any time data must be read from a file.

  \b Note:

  This is an internal function and is not intended to be called
  directly by the user.

\return Returns FALSE on error

On error, the supplied error object is set. Possible errors include:

I/O error
*/

/*!< IO handle for read */
/*!< File offset for read */
NITFPRIV(int) nitf_ImageIO_readFromFile(nitf_IOInterface* io, nitf_Uint64 fileOffset, nitf_Uint8 * buffer,      /*!< Data buffer to read into */
                                        size_t count,      /*!< Number of bytes to read */
                                        nitf_Error * errorBuffer        /*!< Error object */
                                       );

/*!
  \brief nitf_ImageIO_writeToFile - Write data to a file

  nitf_ImageIO_writeToFile writes data to a file at a specified offset.
  This function is used any time data must be written to a file.

  \b Note:

  This is an internal function and is not intended to be called
  directly by the user.

\return Returns FALSE on error

On error, the supplied error object is set. Possible errors include:

I/O error
*/

/*!< IO handle for write */
/*!< File offset for write */
NITFPRIV(int) nitf_ImageIO_writeToFile(nitf_IOInterface* io,
                                       nitf_Uint64 fileOffset, const nitf_Uint8 * buffer, /*!< Data buffer to write from */
                                       size_t count,       /*!< Number of bytes to write */
                                       nitf_Error * errorBuffer /*!< Error object */
                                      );

/*!
  \brief nitf_ImageIO_writeToBlock - Write data to a block

  nitf_ImageIO_writeToBlock writes data to a block buffer at a specified
  offset. When the block is full it is written to disk. This function
  manages the allocation of the block buffers.

  This function is used for cached writes.

  This function checks for missing and pad present blocks if the compression
  mode is a masked type (i.e. NM) but masks are not generated for S mode.

  The reason for this is that unlike all other blocksing modes, in s mode,
  the complete pixel (all bands) for a give image location is not in the
  same block.

  All bands are written at once, rows top to bottom. For all modes except S
  this results in the blocks being written in block number order (i.e. block
  0 first followed by blocks 1, etc). Because the blocks are written in order,
  the absence of a block (one that is pad only) does not require shuffing
  blocks that have already been written. For S mode the discovery of an all
  pad block might require the moving of a previously written block from a
  higher numbered band.


  \b Note:

  This is an internal function and is not intended to be called
  directly by the user.

\return Returns FALSE on error

On error, the supplied error object is set. Possible errors include:

I/O error
*/

NITFPRIV(int) nitf_ImageIO_writeToBlock
(
    _nitf_ImageIOBlock * blockIO, /*!< Associated block control */
    nitf_IOInterface* io,         /*!< IO handle for write */
    size_t blockOffset,      /*!< Offset into block buffer for write */
    const nitf_Uint8 * buffer,    /*!< Data buffer to write from */
    size_t count,            /*!< Number of bytes to write */
    nitf_Error * error            /*!< Error object */
);

/*!
  \brief nitf_ImageIO_nextRow - Adjust a block IO structure for the next row

  nitf_ImageIO_nextRow adjusts a block IO structure for the next row. It
  also handles the next block case

  The noUserInc argument supports down-sampling where the internal buffer
  pointer is updated more frequently than the user buffer pointer

  \b Note:

This is an internal function and is not intended to be called
directly by the user.

\return None

*/

/*!< NITF block structure */
/*!< Do not increment user buffer pointer if TRUE */
NITFPRIV(void) nitf_ImageIO_nextRow(_nitf_ImageIOBlock * blockIO,
                                    NITF_BOOL noUserInc);

/*!
  \brief nitf_ImageIO_uncachedReader - Read pixel data from a file without
   block caching

  nitf_ImageIO_uncachedReader reads pixel data from a file without block
  caching. This is a "reader" function and as such handles setting the "padded"
  field in the associated blockIO structure.

  \b Note:

  This is an internal function and is not intended to be called
directly by the user.

\return Returns FALSE on error

On error, the error object is set. Possible errors include:

I/O errors
*/

/*!< NITF block structure */
/*!< I/O handle */
int nitf_ImageIO_uncachedReader(_nitf_ImageIOBlock * blockIO, nitf_IOInterface* io, nitf_Error * error    /*!< Error object */
                               );

/*!
  \brief nitf_ImageIO_cachedReader - Read pixel data from a file with
   block caching

  nitf_ImageIO_cachedReader reads pixel data from a file with block
  caching. This is a "reader" function and as such handles setting the
  "padded" field in the associated blockIO structure.

  \b Note:

  This is an internal function and is not intended to be called directly by
the user.

\return Returns FALSE on error

On error, the error object is set. Possible errors include:

I/O errors
*/

/*!< NITF block structure */
/*!< I/O handle */
int nitf_ImageIO_cachedReader(_nitf_ImageIOBlock * blockIO, nitf_IOInterface* io, nitf_Error * error      /*!< Error object */
                             );

/*!
  \brief nitf_ImageIO_uncachedWriter - Write pixel data to a file without
   block caching

  nitf_ImageIO_uncachedWriter writes pixel data to a file without block
  caching. This is a "writer" function for the vtbl "writer" field.

  \b Note:

  This is an internal function and is not intended to be called
  directly by the user.

\return Returns FALSE on error

On error, FALSE is returned the error object is set.

Possible errors include:

I/O errors
*/

/*!< NITF block structure */
/*!< I/O handle */
int nitf_ImageIO_uncachedWriter(_nitf_ImageIOBlock * blockIO, nitf_IOInterface* io, nitf_Error * error    /*!< Error object */
                               );

/*!
  \brief nitf_ImageIO_cachedWriter - Write pixel data to a file with
   block caching

  nitf_ImageIO_cachedWriter writes pixel data to a file with block
  caching. This is a "writer" function for the vtbl "writer" field

  \b Note:

  This is an internal function and is not intended to be called directly by
  the user.

\return Returns FALSE on error

On error, the error object is set. Possible errors include:

I/O errors
*/

/*!< NITF block structure */
/*!< I/O handle */
int nitf_ImageIO_cachedWriter(_nitf_ImageIOBlock * blockIO, nitf_IOInterface* io, nitf_Error * error      /*!< Error object */
                             );

/*!
  \brief nitf_ImageIO_setDefaultParameters - Set defaults for parameters

  This function initializes the  _nitf_ImageIOParameters in the nitf_ImageIO
  object

  \return None
*/

/*!< The object to setup */
void nitf_ImageIO_setDefaultParameters(_nitf_ImageIO * object);

/*!
  \brief nitf_ImageIO_unpack_P_* - Unpack functions for block mode P

  The nitf_ImageIO_nitf_ImageIO_unpack_P_* function do the unpacking
  operation for blocking mode "P" (band interleaved by pixel) reads
  involving byte swaps only

  There are five variants of this function that operate on various lengths
  of data:

  nitf_ImageIO_unpack_P_1   - Unpack 2 byte data\\n
nitf_ImageIO_unpack_P_2   - Unpack 4 byte data\\n
nitf_ImageIO_unpack_P_4   - Unpack 8 byte data\\n
nitf_ImageIO_unpack_P_8   - Unpack 8 byte, complex data data\\n
nitf_ImageIO_unpack_P_16  - Unpack 16 byte data\\n

There is no difference between anytypes when it comes to unpacking.

\b Note:

These are internal functions and are not intended to be called
directly by the user.

\return None
*/

/*!< NITF block structure */
/*!< Error object */
void nitf_ImageIO_unpack_P_1(_nitf_ImageIOBlock * blockIO,
                             nitf_Error * error);

/*!< NITF block structure */
/*!< Error object */
void nitf_ImageIO_unpack_P_2(_nitf_ImageIOBlock * blockIO,
                             nitf_Error * error);

/*!< NITF block structure */
/*!< Error object */
void nitf_ImageIO_unpack_P_4(_nitf_ImageIOBlock * blockIO,
                             nitf_Error * error);

/*!< NITF block structure */
/*!< Error object */
void nitf_ImageIO_unpack_P_8(_nitf_ImageIOBlock * blockIO,
                             nitf_Error * error);

/*!< NITF block structure */
/*!< Error object */
void nitf_ImageIO_unpack_P_16(_nitf_ImageIOBlock * blockIO,
                              nitf_Error * error);

/*!
  \brief nitf_ImageIO_pack_P_* - Pack functions for block mode P

  The nitf_ImageIO_nitf_ImageIO_pack_P_* function do the packing
  operation for blocking mode "P" (band interleaved by pixel) reads
  involving byte swaps only

  There are five variants of this function that operate on various lengths
  of data:

  nitf_ImageIO_pack_P_1   - Pack 2 byte data\\n
nitf_ImageIO_pack_P_2   - Pack 4 byte data\\n
nitf_ImageIO_pack_P_4   - Pack 8 byte data\\n
nitf_ImageIO_pack_P_8   - Pack 8 byte, complex data data\\n
nitf_ImageIO_pack_P_16  - Pack 16 byte data\\n

There is no difference between anytypes when it comes to packing.

\b Note:

These are internal functions and are not intended to be called
directly by the user.

\return None
*/

/*!< NITF block structure */
/*!< Error object */
void nitf_ImageIO_pack_P_1(_nitf_ImageIOBlock * blockIO, nitf_Error * error);

/*!< NITF block structure */
/*!< Error object */
void nitf_ImageIO_pack_P_2(_nitf_ImageIOBlock * blockIO, nitf_Error * error);

/*!< NITF block structure */
/*!< Error object */
void nitf_ImageIO_pack_P_4(_nitf_ImageIOBlock * blockIO, nitf_Error * error);

/*!< NITF block structure */
/*!< Error object */
void nitf_ImageIO_pack_P_8(_nitf_ImageIOBlock * blockIO, nitf_Error * error);

/*!< NITF block structure */
/*!< Error object */
void nitf_ImageIO_pack_P_16(_nitf_ImageIOBlock * blockIO,
                            nitf_Error * error);

/*!
  \brief nitf_ImageIO_unformatExtend - Do pixel unformats involving sign
   extensions

  The nitf_ImageIO_unformatExtend function does pixel unformats involving sign
  extension. They are implemented using signed shifts

  There are four variants of this function that operate on various lengths
  of signed integer data:

  nitf_ImageIO_unformatExtend_1 - Extend 1 byte data\\n
nitf_ImageIO_unformatExtend_2 - Extend 2 byte data\\n
nitf_ImageIO_unformatExtend_4 - Extend 4 byte data\\n
nitf_ImageIO_unformatExtend_8 - Extend 8 byte data\\n

These unformattings are only applied to pixel type (SI)

\b Note:

These are internal functions and are not intended to be called
directly by the user.

\return None

*/

/*!< Buffer holding the data to unformat */
/*!< Number of values to unformat */
void nitf_ImageIO_unformatExtend_1(nitf_Uint8 * buffer, size_t count, nitf_Uint32 shiftCount       /*!< Number of bits to shift */
                                  );

/*!< Buffer holding the data to unformat */
/*!< Number of values to unformat */
void nitf_ImageIO_unformatExtend_2(nitf_Uint8 * buffer, size_t count, nitf_Uint32 shiftCount       /*!< Number of bits to shift */
                                  );

/*!< Buffer holding the data to unformat */
/*!< Number of values to unformat */
void nitf_ImageIO_unformatExtend_4(nitf_Uint8 * buffer, size_t count, nitf_Uint32 shiftCount       /*!< Number of bits to shift */
                                  );

/*!< Buffer holding the data to unformat */
/*!< Number of values to unformat */
void nitf_ImageIO_unformatExtend_8(nitf_Uint8 * buffer, size_t count, nitf_Uint32 shiftCount       /*!< Number of bits to shift */
                                  );

/*!
  \brief nitf_ImageIO_unformatShift - Do pixel unformats involving signed
   shifts

  The nitf_ImageIO_unformatShift function does pixel unformats involving signed
  shifts

  There are four variants of this function that operate on various lengths
  of signed integer data:

  nitf_ImageIO_unformatShift_1 - Shift 1 byte data\\n
nitf_ImageIO_unformatShift_2 - Shift 2 byte data\\n
nitf_ImageIO_unformatShift_4 - Shift 4 byte data\\n
nitf_ImageIO_unformatShift_8 - Shift 8 byte data\\n

These unformattings are only applied to pixel type (SI)

\b Note:

These are internal functions and are not intended to be called
directly by the user.

\return None

*/

/*!< Buffer holding the data to unformat */
/*!< Number of values to unformat */
void nitf_ImageIO_unformatShift_1(nitf_Uint8 * buffer, size_t count, nitf_Uint32 shiftCount        /*!< Number of bits to shift */
                                 );

/*!< Buffer holding the data to unformat */
/*!< Number of values to unformat */
void nitf_ImageIO_unformatShift_2(nitf_Uint8 * buffer, size_t count, nitf_Uint32 shiftCount        /*!< Number of bits to shift */
                                 );

/*!< Buffer holding the data to unformat */
/*!< Number of values to unformat */
void nitf_ImageIO_unformatShift_4(nitf_Uint8 * buffer, size_t count, nitf_Uint32 shiftCount        /*!< Number of bits to shift */
                                 );

/*!< Buffer holding the data to unformat */
/*!< Number of values to unformat */
void nitf_ImageIO_unformatShift_8(nitf_Uint8 * buffer, size_t count, nitf_Uint32 shiftCount        /*!< Number of bits to shift */
                                 );

/*!
  \brief nitf_ImageIO_unformatUShift - Do pixel unformats involving signed
   shifts

  The nitf_ImageIO_unformatUShift function does pixel unformats involving
  unsigned shifts

  There are four variants of this function that operate on various lengths
  of unsigned integer data:

  nitf_ImageIO_unformatUShift_1 - Unsigned shift 1 byte data\\n
nitf_ImageIO_unformatUShift_2 - Unsigned shift 2 byte data\\n
nitf_ImageIO_unformatUShift_4 - Unsigned shift 4 byte data\\n
nitf_ImageIO_unformatUShift_8 - Unsigned shift 8 byte data\\n

These unformattings are only applied to pixel type (INT)

\b Note:

These are internal functions and are not intended to be called
directly by the user.

\return None

*/

/*!< Buffer holding the data to unformat */
/*!< Number of values to unformat */
void nitf_ImageIO_unformatUShift_1(nitf_Uint8 * buffer, size_t count, nitf_Uint32 shiftCount       /*!< Number of bits to shift */
                                  );

/*!< Buffer holding the data to unformat */
/*!< Number of values to unformat */
void nitf_ImageIO_unformatUShift_2(nitf_Uint8 * buffer, size_t count, nitf_Uint32 shiftCount       /*!< Number of bits to shift */
                                  );

/*!< Buffer holding the data to unformat */
/*!< Number of values to unformat */
void nitf_ImageIO_unformatUShift_4(nitf_Uint8 * buffer, size_t count, nitf_Uint32 shiftCount       /*!< Number of bits to shift */
                                  );

/*!< Buffer holding the data to unformat */
/*!< Number of values to unformat */
void nitf_ImageIO_unformatUShift_8(nitf_Uint8 * buffer, size_t count, nitf_Uint32 shiftCount       /*!< Number of bits to shift */
                                  );

/*!
  \brief nitf_ImageIO_swapOnly - Do pixel unformats and formats involving byte
   swaps only

  The nitf_ImageIO_swapOnly function does pixel unformats and unformats
  involving byte swaps only

  There are five variants of this function that operate on various lengths
  of data:

nitf_ImageIO_swapOnly_2   - Byte swap 2 byte data\\n
nitf_ImageIO_swapOnly_4   - Byte swap 4 byte data\\n
nitf_ImageIO_swapOnly_4c  - Byte swap 4 byte, complex data\\n
nitf_ImageIO_swapOnly_8   - Byte swap 8 byte data\\n
nitf_ImageIO_swapOnly_8c  - Byte swap 8 byte, complex data data
nitf_ImageIO_swapOnly_16c - Byte swap 16 byte, complex data data

There is no difference between integer and real types when it comes to
byte orderin, except for complex which is actually two consecutive reals.

The shiftCount argument is not used but is required for the calling
convention.

\b Note:

These are internal functions and are not intended to be called
directly by the user.

\return None

*/

/*!< Buffer holding the data to unformat */
/*!< Number of values to unformat */
void nitf_ImageIO_swapOnly_2(nitf_Uint8 * buffer, size_t count, nitf_Uint32 shiftCount     /*!< Number of bits to shift (unused) */
                            );

/*!< Buffer holding the data to unformat */
/*!< Number of values to unformat */
void nitf_ImageIO_swapOnly_4(nitf_Uint8 * buffer, size_t count, nitf_Uint32 shiftCount     /*!< Number of bits to shift (unused) */
                            );

/*!< Buffer holding the data to unformat */
/*!< Number of values to unformat */
void nitf_ImageIO_swapOnly_4c(nitf_Uint8 * buffer, size_t count, nitf_Uint32 shiftCount    /*!< Number of bits to shift (unused) */
                             );

/*!< Buffer holding the data to unformat */
/*!< Number of values to unformat */
void nitf_ImageIO_swapOnly_4c(nitf_Uint8 * buffer, size_t count, nitf_Uint32 shiftCount    /*!< Number of bits to shift (unused) */
                             );

/*!< Buffer holding the data to unformat */
/*!< Number of values to unformat */
void nitf_ImageIO_swapOnly_8(nitf_Uint8 * buffer, size_t count, nitf_Uint32 shiftCount     /*!< Number of bits to shift (unused) */
                            );

/*!< Buffer holding the data to unformat */
/*!< Number of values to unformat */
void nitf_ImageIO_swapOnly_8c(nitf_Uint8 * buffer, size_t count, nitf_Uint32 shiftCount    /*!< Number of bits to shift (unused) */
                             );

/*!< Buffer holding the data to unformat */
/*!< Number of values to unformat */
void nitf_ImageIO_swapOnly_16c(nitf_Uint8 * buffer, size_t count, nitf_Uint32 shiftCount   /*!< Number of bits to shift (unused) */
                              );

/*!
  \brief nitf_ImageIO_unformatSwapExtend - Do pixel unformats involving byte
   swap followed by sign extension

  The nitf_ImageIO_unformatSwapExtend function does pixel unformats involving
  byte swap followed by sign extension

  There are three variants of this function that operate on various lengths
  of signed integer data:

  nitf_ImageIO_unformatSwapExtend_2   - Byte swap 2 byte data\\n
nitf_ImageIO_unformatSwapExtend_4   - Byte swap 4 byte data\\n
nitf_ImageIO_unformatSwapExtend_8   - Byte swap 8 byte data\\n

These unformattings are only applied to pixel type (SI)

\b Note:

These are internal functions and are not intended to be called
directly by the user.

\return None

*/

/*!< Buffer holding the data to unformat */
/*!< Number of values to unformat */
void nitf_ImageIO_unformatSwapExtend_2(nitf_Uint8 * buffer, size_t count, nitf_Uint32 shiftCount   /*!< Number of bits to shift (unused) */
                                      );

/*!< Buffer holding the data to unformat */
/*!< Number of values to unformat */
void nitf_ImageIO_unformatSwapExtend_4(nitf_Uint8 * buffer, size_t count, nitf_Uint32 shiftCount   /*!< Number of bits to shift (unused) */
                                      );

/*!< Buffer holding the data to unformat */
/*!< Number of values to unformat */
void nitf_ImageIO_unformatSwapExtend_8(nitf_Uint8 * buffer, size_t count, nitf_Uint32 shiftCount   /*!< Number of bits to shift (unused) */
                                      );

/*!
  \brief nitf_ImageIO_unformatSwapShift - Do pixel unformats involving byte
   swap followed by signed shifts

  The nitf_ImageIO_unformatSwapExtend function does pixel unformats involving
  byte swap followed by signed shifts

  There are three variants of this function that operate on various lengths
  of signed integer data:

  nitf_ImageIO_unformatSwapShift_2   - Byte swap and shift 2 byte data\\n
nitf_ImageIO_unformatSwapShift_4   - Byte swap and shift 4 byte data\\n
nitf_ImageIO_unformatSwapShift_8   - Byte swap and shift 8 byte data\\n

These unformattings are only applied to pixel type (SI)

\b Note:

These are internal functions and are not intended to be called
directly by the user.

\return None

*/

/*!< Buffer holding the data to unformat */
/*!< Number of values to unformat */
void nitf_ImageIO_unformatSwapShift_2(nitf_Uint8 * buffer, size_t count, nitf_Uint32 shiftCount    /*!< Number of bits to shift */
                                     );

/*!< Buffer holding the data to unformat */
/*!< Number of values to unformat */
void nitf_ImageIO_unformatSwapShift_4(nitf_Uint8 * buffer, size_t count, nitf_Uint32 shiftCount    /*!< Number of bits to shift */
                                     );

/*!< Buffer holding the data to unformat */
/*!< Number of values to unformat */
void nitf_ImageIO_unformatSwapShift_8(nitf_Uint8 * buffer, size_t count, nitf_Uint32 shiftCount    /*!< Number of bits to shift */
                                     );

/*!
  \brief nitf_ImageIO_unformatSwapUShift - Do pixel unformats involving byte
   swap followed by unsigned shifts

  The nitf_ImageIO_unformatSwapExtend function does pixel unformats involving
  byte swap followed by unsigned shifts

  There are three variants of this function that operate on various lengths
  of unsigned integer data:

  nitf_ImageIO_unformatSwapUShift_2   - Byte swap and shift 2 byte data\\n
nitf_ImageIO_unformatSwapUShift_4   - Byte swap and shift 4 byte data\\n
nitf_ImageIO_unformatSwapUShift_8   - Byte swap and shift 8 byte data\\n

These unformattings are only applied to pixel type (INT)

\b Note:

These are internal functions and are not intended to be called
directly by the user.

\return None

*/

/*!< Buffer holding the data to unformat */
/*!< Number of values to unformat */
void nitf_ImageIO_unformatSwapUShift_2(nitf_Uint8 * buffer, size_t count, nitf_Uint32 shiftCount   /*!< Number of bits to shift */
                                      );

/*!< Buffer holding the data to unformat */
/*!< Number of values to unformat */
void nitf_ImageIO_unformatSwapUShift_4(nitf_Uint8 * buffer, size_t count, nitf_Uint32 shiftCount   /*!< Number of bits to shift */
                                      );

/*!< Buffer holding the data to unformat */
/*!< Number of values to unformat */
void nitf_ImageIO_unformatSwapUShift_8(nitf_Uint8 * buffer, size_t count, nitf_Uint32 shiftCount   /*!< Number of bits to shift */
                                      );

/*!
  \brief nitf_ImageIO_formatShift - Do pixel formatting involving left
   shifts

  The nitf_ImageIO_formatShift function does pixel formatting involving left
  shifts

  There are four variants of this function that operate on various lengths
  of signed and unsigned integer data:

  nitf_ImageIO_formatShift_1 - Shift 1 byte data\\n
nitf_ImageIO_formatShift_2 - Shift 2 byte data\\n
nitf_ImageIO_formatShift_4 - Shift 4 byte data\\n
nitf_ImageIO_formatShift_8 - Shift 8 byte data\\n

This formatting is only applied to pixel types INT and SI

\b Note:

These are internal functions and are not intended to be called directly by
the user.

\return None

*/

/*!< Buffer holding the data to format */
/*!< Number of values to format */
void nitf_ImageIO_formatShift_1(nitf_Uint8 * buffer, size_t count, nitf_Uint32 shiftCount  /*!< Number of bits to shift */
                               );

/*!< Buffer holding the data to format */
/*!< Number of values to format */
void nitf_ImageIO_formatShift_2(nitf_Uint8 * buffer, size_t count, nitf_Uint32 shiftCount  /*!< Number of bits to shift */
                               );

/*!< Buffer holding the data to format */
/*!< Number of values to format */
void nitf_ImageIO_formatShift_4(nitf_Uint8 * buffer, size_t count, nitf_Uint32 shiftCount  /*!< Number of bits to shift */
                               );

/*!< Buffer holding the data to format */
/*!< Number of values to format */
void nitf_ImageIO_formatShift_8(nitf_Uint8 * buffer, size_t count, nitf_Uint32 shiftCount  /*!< Number of bits to shift */
                               );

/*!
  \brief nitf_ImageIO_formatMask - Do pixel formatting involving masking

  The nitf_ImageIO_formatMask function does pixel formatting involving
  masking. The shift count argument is used to construct the mask

  There are four variants of this function that operate on various lengths
  of signed and unsigned integer data:

  nitf_ImageIO_formatMask_1 - Unsigned shift 1 byte data\\n
  nitf_ImageIO_formatMask_2 - Unsigned shift 2 byte data\\n
nitf_ImageIO_formatMask_4 - Unsigned shift 4 byte data\\n
nitf_ImageIO_formatMask_8 - Unsigned shift 8 byte data\\n

This formatting is applied to pixel types INT and SI

\b Note:

These are internal functions and are not intended to be called directly by
the user.

\return None

*/

/*!< Buffer holding the data to format */
/*!< Number of values to format */
void nitf_ImageIO_formatMask_1(nitf_Uint8 * buffer, size_t count, nitf_Uint32 shiftCount   /*!< Number of bits to shift */
                              );

/*!< Buffer holding the data to format */
/*!< Number of values to format */
void nitf_ImageIO_formatMask_2(nitf_Uint8 * buffer, size_t count, nitf_Uint32 shiftCount   /*!< Number of bits to shift */
                              );

/*!< Buffer holding the data to format */
/*!< Number of values to format */
void nitf_ImageIO_formatMask_4(nitf_Uint8 * buffer, size_t count, nitf_Uint32 shiftCount   /*!< Number of bits to shift */
                              );

/*!< Buffer holding the data to format */
/*!< Number of values to format */
void nitf_ImageIO_formatMask_8(nitf_Uint8 * buffer, size_t count, nitf_Uint32 shiftCount   /*!< Number of bits to shift */
                              );

/*!
  \brief nitf_ImageIO_formatShiftSwap - Do pixel formatments involving left
   shifts followed by byte swaps

  The nitf_ImageIO_formatSwapExtend function does pixel formatments involving
  left shifts followed by byte swaps

  There are three variants of this function that operate on various lengths
  of signed and unsigned integer data:

  nitf_ImageIO_formatShiftSwap_2   - Shift and byte swap 2 byte data\\n
nitf_ImageIO_formatShiftSwap_4   - Shift and byte swap 4 byte data\\n
nitf_ImageIO_formatShiftSwap_8   - Shift and byte swap 8 byte data\\n

Ths formatting is applied to pixel types INT and SI

\b Note:

These are internal functions and are not intended to be called directly by
the user.

\return None

*/

/*!< Buffer holding the data to format */
/*!< Number of values to format */
void nitf_ImageIO_formatShiftSwap_2(nitf_Uint8 * buffer, size_t count, nitf_Uint32 shiftCount      /*!< Number of bits to shift */
                                   );

/*!< Buffer holding the data to format */
/*!< Number of values to format */
void nitf_ImageIO_formatShiftSwap_4(nitf_Uint8 * buffer, size_t count, nitf_Uint32 shiftCount      /*!< Number of bits to shift */
                                   );

/*!< Buffer holding the data to format */
/*!< Number of values to format */
void nitf_ImageIO_formatShiftSwap_8(nitf_Uint8 * buffer, size_t count, nitf_Uint32 shiftCount      /*!< Number of bits to shift */
                                   );

/*!
  \brief nitf_ImageIO_formatMaskSwap - Do pixel formatments involving masking
  followed by byte swaps

  The nitf_ImageIO_formatSwapExtend function does pixel formatments involving
  masking followed by byte swaps. The shift count argument is used to construct
  the mask

  There are three variants of this function that operate on various lengths
  of signed and unsigned integer data:

nitf_ImageIO_formatMaskSwap_2   - Shift and Byte swap 2 byte data\\n
nitf_ImageIO_formatMaskSwap_4   - Shift and Byte swap 4 byte data\\n
nitf_ImageIO_formatMaskSwap_8   - Shift and Byte swap 8 byte data\\n

These formatments are only applied to pixel type INT and SI

\b Note:

These are internal functions and are not intended to be called directly by
the user.

\return None

*/

/*!< Buffer holding the data to format */
/*!< Number of values to format */
void nitf_ImageIO_formatMaskSwap_2(nitf_Uint8 * buffer, size_t count, nitf_Uint32 shiftCount       /*!< Number of bits to shift */
                                  );

/*!< Buffer holding the data to format */
/*!< Number of values to format */
void nitf_ImageIO_formatMaskSwap_4(nitf_Uint8 * buffer, size_t count, nitf_Uint32 shiftCount       /*!< Number of bits to shift */
                                  );

/*!< Buffer holding the data to format */
/*!< Number of values to format */
void nitf_ImageIO_formatMaskSwap_8(nitf_Uint8 * buffer, size_t count, nitf_Uint32 shiftCount       /*!< Number of bits to shift */
                                  );

/*!
  \brief nitf_ImageIO_print - Do a formatted print of
  a _nitf_ImageIO structure

  nitf_ImageIO_print does a formatted print of a _nitf_ImageIO structure.

  If the "file" argument is NULL stdout is used.

  \b Note:

  This is an internal function and is not intended to be called
directly by the user.

\return None

*/

/*!< nitf_ImageIO object */
/*!< FILE to use for print */
NITFPRIV(void) nitf_ImageIO_print(nitf_ImageIO * nitf, FILE * file);

/*!
  \brief nitf_ImageIOControl_print Do a formatted print of a
  _nitf_ImageIOControl structure

  nitf_ImageIOControl_print does a formatted print of a _nitf_ImageIOControl
  structure. If the "file" argument is NULL stdout is used. If the "full"
  argument is TRUE, the array of nitf_ImageIOBlock structures is printed.

  \b Note:

  This is an internal function and is not intended to be called
directly by the user.

\return None

*/

/*!< nitf_ImageIO control object */
/*!< FILE to use for print */
NITFPRIV(void) nitf_ImageIOControl_print(_nitf_ImageIOControl * cntl, FILE * file, int full     /*!< Full print if TRUE */
                                        );

/*!
  \brief nitf_ImageIOBlock_print - Do a formatted print of
  a _nitf_ImageIOBlock structure

  nitf_ImageIOBlock_print does a formatted print of a _nitf_ImageIOBlock
  structure. If the "file" argument is NULL stdout is used. The "longIndent"
  argument is used when the print is done as par of a _nitf_ImageIOControl
  structure.

  \b Note:
  This is an internal function and is not intended to be called
directly by the user.

\return None

*/

NITFPRIV(void) nitf_ImageIOBlock_print
(_nitf_ImageIOBlock * blockIO, FILE * file, int longIndent);

/*!
  \brief nitf_ImageIO_getMaskInfo - Get block/pad mask information

  nitf_ImageIO_getMaskInfo returns information from the image data mask
  table amd masks. This information is after the image subheader and before
  the pixel data in images with a mask type compression code (i.e. "NM")

  The masks are set in the ImageIO on demand so the user must first force
  them to be read. This can be done by reading pixel data or calling
  nitf_ImageIO_getBlockingInfo (or the corresponding function in the image
  reader

  The returned masks are the actual arays in the ImageIO and should not
  be modified, they will be freed when the ImageIO is destroyed

  All of the values are returned via uin32's but some are actually smaller.

  All values are in native byte ordering

  If this is not a masked image, FALSE is returned and no output values are
  set.

  \return TRUE if this is a masked image

  This is a diagnostic function not intended for normal use
*/

NITFAPI(NITF_BOOL) nitf_ImageIO_getMaskInfo
(
    nitf_ImageIO *nitf,            /*!< The ImageIO to access */
    nitf_Uint32 *imageDataOffset,  /*!< Offset to actual image data past masks */
    nitf_Uint32 *blockRecordLength, /*!< Block mask record length */
    nitf_Uint32 *padRecordLength,   /*!< Pad mask record length */
    nitf_Uint32 *padPixelValueLength, /*!< Pad pixel value length in bytes */
    nitf_Uint8 **padValue,          /*!< Pad value */
    nitf_Uint64 **blockMask,        /*!< Block mask array */
    nitf_Uint64 **padMask           /*!< Pad mask array */
);

/*!
  \brief nitf_ImageIO_bPixelFreeBlock - Free block function for B pixel
  type psuedo-decompression interface.

  \returns TRUE on success. On error, the error object is set
*/

/*!< Associated control structure */
/*!< Block to free */
NITFPRIV(NITF_BOOL) nitf_ImageIO_bPixelFreeBlock(nitf_DecompressionControl * control, nitf_Uint8 * block, nitf_Error * error    /*!< For error returns */
                                                );

/*!
  \brief nitf_ImageIO_bPixelOpen - Open function for B pixel type
  psuedo-decompression interface.

  The control structure stores its arguments for use by the other interface
  functions

  \returns The new control structure on success. On error, NULL is returned and
  the error object is set
*/

/*!< IO handle for reads */
/*!< Offset to start of blocks */
NITFPRIV(nitf_DecompressionControl *) nitf_ImageIO_bPixelOpen(nitf_IOInterface* io, nitf_Uint64 offset, nitf_Uint64 fileLength,     /*!< Total file length (not used) */
        nitf_BlockingInfo * blockInfo,    /*!< Associated blocking information */
        nitf_Uint64 * blockMask,  /*!< Associated block mask */
        nitf_Error * error        /*!< For error returns */
                                                             );

/*!
  \brief nitf_ImageIO_bPixelClose - Close function for B pixel type
  psuedo-decompression interface.

  \returns None
*/

/*!< Associated control structure */
NITFPRIV(void) nitf_ImageIO_bPixelClose(nitf_DecompressionControl **
                                        control);

/*!
  \brief nitf_ImageIO_bPixelReadBlock - Read block function for B pixel type
  psuedo-decompression interface.

  \returns The block buffer on success. On error, NULL is returned and
  the error object is set
*/

/*!< Associated control structure */
/*!< Block number to read */
NITFPRIV(nitf_Uint8 *) nitf_ImageIO_bPixelReadBlock(nitf_DecompressionControl * control, nitf_Uint32 blockNumber, nitf_Error * error    /*!< For error returns */
                                                   );

/*!
  \brief nitf_ImageIO_bPixelInterface - Decompression interface for B pixel
  type psuedo-decompression interface.
*/

static nitf_DecompressionInterface nitf_ImageIO_bPixelInterface =
    {
        nitf_ImageIO_bPixelOpen,
        nitf_ImageIO_bPixelReadBlock,
        nitf_ImageIO_bPixelFreeBlock,
        nitf_ImageIO_bPixelClose,
        NULL
    };

/*!
  \brief nitf_ImageIO_12PixelFreeBlock - Free block function for 12-bit pixel
  type psuedo-decompression interface.

  \returns TRUE on success. On error, the error object is set
*/

/*!< Associated control structure */
/*!< Block to free */
NITFPRIV(NITF_BOOL) nitf_ImageIO_12PixelFreeBlock(
  nitf_DecompressionControl * control, nitf_Uint8 * block,
  nitf_Error * error);    /*!< For error returns */

/*!
  \brief nitf_ImageIO_12PixelOpen - Open function for 12-bit pixel type
  psuedo-decompression interface.

  The control structure stores its arguments for use by the other interface
  functions

  \returns The new control structure on success. On error, NULL is returned and
  the error object is set
*/

NITFPRIV(nitf_DecompressionControl *) nitf_ImageIO_12PixelOpen(
   nitf_IOInterface* io, /*!< IO handle for reads */
   nitf_Uint64 offset,   /*!< Offset to start of blocks */
   nitf_Uint64 fileLength,     /*!< Total file length (not used) */
   nitf_BlockingInfo * blockInfo,    /*!< Associated blocking information */
   nitf_Uint64 * blockMask,  /*!< Associated block mask */
   nitf_Error * error);      /*!< For error returns */

/*!
  \brief nitf_ImageIO_12PixelClose - Close function for 12-bit pixel type
  psuedo-decompression interface.

  \returns None
*/

/*!< Associated control structure */
NITFPRIV(void) nitf_ImageIO_12PixelClose(nitf_DecompressionControl **control);

/*!
  \brief nitf_ImageIO_12PixelReadBlock - Read block function for 12-bit pixel
  type psuedo-decompression interface.

  \returns The block buffer on success. On error, NULL is returned and
  the error object is set
*/

NITFPRIV(nitf_Uint8 *) nitf_ImageIO_12PixelReadBlock(
  nitf_DecompressionControl * control, /*!< Associated control structure */
  nitf_Uint32 blockNumber, /*!< Block number to read */
  nitf_Error * error);    /*!< For error returns */

/*!
  \brief nitf_ImageIO_12PixelInterface - Decompression interface for 12-bit
  pixel type psuedo-decompression interface. (NBPP == ABPP)
*/

static nitf_DecompressionInterface nitf_ImageIO_12PixelInterface =
    {
        nitf_ImageIO_12PixelOpen,
        nitf_ImageIO_12PixelReadBlock,
        nitf_ImageIO_12PixelFreeBlock,
        nitf_ImageIO_12PixelClose,
        NULL
    };

/*!
    \brief nitf_ImageIO_12PixelComOpen - Open function for 12 bit pixel
    psuedo compression (NBPP == 12 and ABPP = 12 case)

    This function follows the NITF_COMPRESSION_INTERFACE_OPEN_FUNCTION calling
    sequence
*/

nitf_CompressionControl  *nitf_ImageIO_12PixelComOpen
  (nitf_ImageSubheader * subheader,nitf_Error * error);

/*!
    \brief nitf_ImageIO_12PixelComStart - Start function for 12 bit pixel
    psuedo compression (NBPP == 12 and ABPP = 12 case)

    This function follows the NITF_COMPRESSION_INTERFACE_START_FUNCTION calling
    sequence
*/

NITF_BOOL nitf_ImageIO_12PixelComStart
  (nitf_CompressionControl *object,nitf_Uint64 offset,nitf_Uint64 dataLength,
   nitf_Uint64 * blockMask,nitf_Uint64 * padMask, nitf_Error * error);

/*!
    \brief nitf_ImageIO_12PixelComWriteBlock - Write block function for 12 bit
    pixel type psuedo compression (NBPP == 12 and ABPP = 12 case)

    This function follows the NITF_COMPRESSION_INTERFACE_WRITE_BLOCK_FUNCTION
    calling sequence
*/

NITF_BOOL nitf_ImageIO_12PixelComWriteBlock (
    nitf_CompressionControl * object, nitf_IOInterface* io,nitf_Uint8 *data,
            NITF_BOOL pad,NITF_BOOL noData,nitf_Error *error);

/*!
    \brief nitf_ImageIO_12PixelComEnd - End function for 12 bit pixel type
    psuedo compression (NBPP == 12 and ABPP = 12 case)

    This function follows the NITF_COMPRESSION_INTERFACE_END_FUNCTION
    calling sequence
*/

  NITF_BOOL nitf_ImageIO_12PixelComEnd(
         nitf_CompressionControl * object,
                              nitf_IOInterface* io,nitf_Error *error);

/*!
    \brief nitf_ImageIO_12PixelComDestroy - Destroy function for 12 bit
    pixel type psuedo compression (NBPP == 12 and ABPP = 12 case)

    This function follows the NITF_COMPRESSION_INTERFACE_DESTROY_FUNCTION
    calling sequence
*/

void nitf_ImageIO_12PixelComDestroy(nitf_CompressionControl ** object);

/*!
  \brief nitf_ImageIO_12PixelComInterface - Compression interface for 12-bit
  pixel type psuedo-decompression interface. (NBPP == ABPP)
*/

static nitf_CompressionInterface nitf_ImageIO_12PixelComInterface =
    {
        nitf_ImageIO_12PixelComOpen,
        nitf_ImageIO_12PixelComStart,
        nitf_ImageIO_12PixelComWriteBlock,
        nitf_ImageIO_12PixelComEnd,
        nitf_ImageIO_12PixelComDestroy,
        NULL
    };


/*============================================================================*/
/*==================== Function definitions ==================================*/
/*============================================================================*/

/*==================== nitf_ImageIO_construct ================================*/

NITFPROT(nitf_ImageIO *) nitf_ImageIO_construct(nitf_ImageSubheader *
        subheader,
        nitf_Uint64 offset,
        nitf_Uint64 length,
        nitf_CompressionInterface *
        compressor,
        nitf_DecompressionInterface
        * decompressor,
        nitf_Error * error)
{
    _nitf_ImageIO *nitf;        /* The result */
    nitf_ImageSubheader *sub;   /* From the calling image segment */

    /*     Values from the segment */

    nitf_Uint32 numRows;          /* Number of rows in the image */
    nitf_Uint32 numColumns;       /* Number of columns in the image */
    nitf_Uint32 numBands, xBands; /* Number of bands */
    char *pixelType;            /* Pixel type */
    nitf_Uint32 nBits;          /* Number of bits per pixel */
    nitf_Uint32 nBitsActual;    /* Actual number of bits per pixel */
    char *justification;        /* Pixels justification */
    nitf_Uint32 nBlocksPerRow;    /* Number of blocks per row */
    nitf_Uint32 nBlocksPerColumn; /* Number of blocks per Column */
    nitf_Uint32 numRowsPerBlock;    /* Number of rows per block */
    nitf_Uint32 numColumnsPerBlock; /* Number of columns per block */

    /*      Load values from calling segment */

    sub = (nitf_ImageSubheader *) subheader;
    NITF_TRY_GET_UINT32(sub->numRows, &numRows, error);
    NITF_TRY_GET_UINT32(sub->numCols, &numColumns, error);
    NITF_TRY_GET_UINT32(sub->numImageBands, &numBands, error);
    NITF_TRY_GET_UINT32(sub->numMultispectralImageBands, &xBands, error);
    numBands += xBands;
    pixelType = sub->pixelValueType->raw;
    NITF_TRY_GET_UINT32(sub->numBitsPerPixel, &nBits, error);
    NITF_TRY_GET_UINT32(sub->actualBitsPerPixel, &nBitsActual, error);
    justification = sub->pixelJustification->raw;
    NITF_TRY_GET_UINT32(sub->numBlocksPerRow, &nBlocksPerRow, error);
    NITF_TRY_GET_UINT32(sub->numBlocksPerCol, &nBlocksPerColumn, error);
    NITF_TRY_GET_UINT32(sub->numPixelsPerVertBlock, &numRowsPerBlock, error);
    NITF_TRY_GET_UINT32(sub->numPixelsPerHorizBlock, &numColumnsPerBlock,
                        error);

    nitf = (_nitf_ImageIO *) NITF_MALLOC(sizeof(_nitf_ImageIO));
    if (nitf == NULL)
    {
        nitf_Error_initf(error, NITF_CTXT, NITF_ERR_MEMORY,
                         "Error allocating object: %s", NITF_STRERROR(NITF_ERRNO));
        return NULL;
    }
    /* Initialize all fields to zero */
    memset(nitf, 0, sizeof(_nitf_ImageIO));

    /*   Adjust block column and row counts for 2500C  */
    if ((nBlocksPerColumn == 1) && (numRowsPerBlock == 0))
        numRowsPerBlock = numRows;
    if ((nBlocksPerRow == 1) && (numColumnsPerBlock == 0))
        numColumnsPerBlock = numColumns;

    nitf->numRows = numRows;
    nitf->numColumns = numColumns;
    nitf->numBands = numBands;
    nitf->nBlocksPerRow = nBlocksPerRow;
    nitf->nBlocksPerColumn = nBlocksPerColumn;
    nitf->numRowsPerBlock = numRowsPerBlock;
    nitf->numColumnsPerBlock = numColumnsPerBlock;
    nitf->numRowsActual = numRowsPerBlock * nBlocksPerColumn;
    nitf->numColumnsActual = numColumnsPerBlock * nBlocksPerRow;
    nitf->compressor = compressor;
    nitf->decompressor = decompressor;
    nitf->compressionControl = NULL;
    nitf->decompressionControl = NULL;
    nitf->blockControl.number = NITF_IMAGE_IO_NO_BLOCK;
    nitf->blockControl.freeFlag = 1;
    nitf->blockControl.block = NULL;
    nitf->cachedWriteFlag = 0;

    nitf_ImageIO_setDefaultParameters(nitf);

    nitf->imageBase = offset;
    nitf->pixelBase = offset;
    nitf->dataLength = length;

    nitf->blockMask = NULL;     /* Set by first read/write */

    if (!nitf_ImageIO_setPixelDef(nitf, pixelType, nBits, nBitsActual,
                                  justification, error))
        return NULL;

    /* Set function pointers */
    if (!nitf_ImageIO_decodeModes(nitf, subheader, error))
        return NULL;

    /*
     *      Check for pixel type B (binary), if there is no decompressor, set
     *  The psuedo decompressor for B typ3 pixels
     */

    if ((nitf->pixel.type == NITF_IMAGE_IO_PIXEL_TYPE_B)
            && (nitf->compression & NITF_IMAGE_IO_NO_COMPRESSION))
        nitf->decompressor = &nitf_ImageIO_bPixelInterface;

    /*
     *      Check for pixel NBPP == 12 and ABPP == 12, (pixel type 12) if
     * there is no decompressor, set the psuedo decompressor for the 12
     * bit pixel type. Also set the compressor in case this is a write
     */

    if ((nitf->pixel.type == NITF_IMAGE_IO_PIXEL_TYPE_12)
            && (nitf->compression & NITF_IMAGE_IO_NO_COMPRESSION))
    {
        nitf->decompressor = &nitf_ImageIO_12PixelInterface;
        nitf->compressor = &nitf_ImageIO_12PixelComInterface;
    }

    if (nitf->blockingMode == NITF_IMAGE_IO_BLOCKING_MODE_S)
    {
        nitf->nBlocksTotal =
            nitf->nBlocksPerRow * nitf->nBlocksPerColumn * nitf->numBands;
        nitf->blockSize = (size_t)numRowsPerBlock *
                (size_t)numColumnsPerBlock * (nitf->pixel.bytes);
    }
    else
    {
        nitf->nBlocksTotal = nitf->nBlocksPerRow * nitf->nBlocksPerColumn;
        nitf->blockSize = (size_t)numRowsPerBlock * (size_t)numColumnsPerBlock *
                        (nitf->pixel.bytes) * (nitf->numBands);
    }

    nitf_ImageIO_setUnpack(nitf);
    nitf_ImageIO_setIO(nitf);

    /* Call if compressor open function if the compressor is not NULL */
    if(nitf->compressor != NULL)
    {
        nitf->compressionControl = (*(nitf->compressor->open))(sub,error);
        if(nitf->compressionControl == NULL)
        {
            nitf_ImageIO_destruct((nitf_ImageIO **) &nitf);
            return(NULL);
        }
    }

    return (nitf_ImageIO *) nitf;

CATCH_ERROR:
    return NULL;
}


NITFPROT(nitf_ImageIO *) nitf_ImageIO_clone(nitf_ImageIO * image,
                                            nitf_Error * error)
{
    _nitf_ImageIO *clone;       /* The result */
    
    clone = (_nitf_ImageIO *) NITF_MALLOC(sizeof(_nitf_ImageIO));
    if (clone == NULL)
    {
        nitf_Error_initf(error, NITF_CTXT, NITF_ERR_MEMORY,
                         "Error allocating object: %s", NITF_STRERROR(NITF_ERRNO));
        return NULL;
    }

    /* Make copy */
    *clone = *((_nitf_ImageIO *) image);

    /* Clear some fields */

    clone->blockInfoFlag = 0;

    memset(&(clone->blockControl), 0,
           sizeof(_nitf_ImageIOBlockCacheControl));

    clone->decompressionControl = NULL;

    memset(&(clone->maskHeader), 0, sizeof(_nitf_ImageIO_MaskHeader));
    clone->blockMask = NULL;
    clone->padMask = NULL;

    return (nitf_ImageIO *) clone;
}

NITFPROT(void) nitf_ImageIO_destruct(nitf_ImageIO ** nitf)
{
    _nitf_ImageIO *nitfp;       /* Pointer to internal type */
    nitf_Error error;           /* For decompressor free block call */

    if (*nitf == NULL)
        return;

    nitfp = *((_nitf_ImageIO **) nitf);

    if (nitfp->blockMask != NULL)
        NITF_FREE(nitfp->blockMask);

    if (nitfp->padMask != NULL)
        NITF_FREE(nitfp->padMask);

    if (nitfp->blockControl.block != NULL)
    {
        /* No plugin */
        if (nitfp->decompressor == NULL)
            NITF_FREE(nitfp->blockControl.block);
        else
            (*(nitfp->decompressor->freeBlock)) (nitfp->
                                                 decompressionControl,
                                                 nitfp->blockControl.block,
                                                 &error);
    }

    if (nitfp->decompressionControl != NULL)
        (*(nitfp->decompressor->destroyControl))(&(nitfp->decompressionControl));

    if (nitfp->compressionControl != NULL)
        (*(nitfp->compressor->destroyControl))(&(nitfp->compressionControl));

    NITF_FREE(nitfp);
    *nitf = NULL;
    return;
}


/*========================= nitf_ImageIO_read ================================*/

NITFPROT(NITF_BOOL) nitf_ImageIO_read(nitf_ImageIO * nitf,
                                      nitf_IOInterface* io,
                                      nitf_SubWindow * subWindow,
                                      nitf_Uint8 ** user,
                                      int *padded, nitf_Error * error)
{
    _nitf_ImageIO *nitfI;       /* Internal version of nitf */
    int all;                    /* Full image read flag */
    nitf_BlockingInfo *blockInfo; /* For get blocking info call */
    NITF_BOOL oneRead;          /* Complete request in one read flag */
    int oneBand;                /* One band flag */
    _nitf_ImageIOControl *cntl; /* IO control structure */
    /* Read control structure */
    _nitf_ImageIOReadControl *readCntl;
    nitf_SubWindow tmpSub;      /* Temp sub-window structure for one band loop */
    nitf_Uint32 band;           /* Current band */
    int ret;                    /* Return value */

    ret = 1;                    /* To avoid warning */
    nitfI = (_nitf_ImageIO *) nitf;

    if ((nitfI->writeControl != NULL) || (nitfI->readControl != NULL))
    {
        nitf_Error_initf(error, NITF_CTXT, NITF_ERR_MEMORY,
                         "I/O operation in progress");
        return NITF_FAILURE;
    }

    /* *possibly* revert the optimized modes */
    nitf_ImageIO_revertOptimizedModes(nitfI, subWindow->numBands);

    /*  Create I/O control */

    /*
     *      Check the request, set-up blocking first since the sub-window
     *  check requires the block size
     */

    blockInfo = nitf_ImageIO_getBlockingInfo(nitf, io, error);
    if (blockInfo == NULL)
        return NITF_FAILURE;

    /* Not needed */
    nitf_BlockingInfo_destruct(&blockInfo);

    if (!nitf_ImageIO_checkSubWindow(nitfI, subWindow, &all, error))
        return 0;

    /*
     *   Look for single read cases (down-sampling never does a single read ori
     * one band reads if the method is multi-band)
     */

    oneBand = nitfI->oneBand;
    if ((subWindow->downsampler != NULL) &&
            ((subWindow->downsampler->rowSkip != 1)
             || (subWindow->downsampler->colSkip != 1)))
    {
        oneRead = 0;
        if (subWindow->downsampler->multiBand)
            oneBand = 0;
    }
    else
        oneRead = nitf_ImageIO_checkOneRead(nitfI, all);

    /*      Set-up and do the read (one band at a time or all bands at once) */

    if (oneBand || oneRead)
    {
        tmpSub = *subWindow;
        for (band = 0; band < subWindow->numBands; band++)
        {
            tmpSub.bandList = subWindow->bandList + band;
            tmpSub.numBands = 1;
            cntl = nitf_ImageIOControl_construct(nitfI,
                                                 io,
                                                 user + band,
                                                 &tmpSub, 1 /* Reading */ ,
                                                 error);
            if (cntl == NULL)
                return 0;

            readCntl =
                nitf_ImageIOReadControl_construct(cntl, subWindow, error);
            if (readCntl == NULL)
            {
                nitf_ImageIOControl_destruct(&cntl);
                return 0;
            }
            nitfI->readControl = readCntl;
            if (oneRead)
                ret = nitf_ImageIO_oneRead(cntl, io, error);
            else
            {
                if (cntl->downSampling)
                    ret =
                        nitf_ImageIO_readRequestDownSample(cntl, subWindow,
                                                           io, error);
                else
                    ret = nitf_ImageIO_readRequest(cntl, io, error);
            }

            nitf_ImageIOControl_destruct(&cntl);
            nitf_ImageIOReadControl_destruct(&(nitfI->readControl));
        }
    }
    else
    {
        cntl = nitf_ImageIOControl_construct(nitfI, io,
                                             user,
                                             subWindow, 1 /* Reading */ ,
                                             error);
        if (cntl == NULL)
            return 0;

        readCntl =
            nitf_ImageIOReadControl_construct(cntl, subWindow, error);
        if (readCntl == NULL)
        {
            nitf_ImageIOControl_destruct(&cntl);
            return 0;
        }
        nitfI->readControl = readCntl;

        if (cntl->downSampling)
            ret =
                nitf_ImageIO_readRequestDownSample(cntl, subWindow, io,
                                                   error);
        else
            ret = nitf_ImageIO_readRequest(cntl, io, error);

        *padded = cntl->padded;
        nitf_ImageIOControl_destruct(&cntl);
        nitf_ImageIOReadControl_destruct(&(nitfI->readControl));
    }

    return ret;
}


NITFPROT(NITF_BOOL) nitf_ImageIO_writeDone(nitf_ImageIO * object,
                                           nitf_IOInterface* io,
                                           nitf_Error * error)
{
    int ret;                    /* Return from flush */
    /* Internal representations */
    _nitf_ImageIO *nitfI;
    _nitf_ImageIOWriteControl *cntl;

    nitfI = (_nitf_ImageIO *) object;
    cntl = nitfI->writeControl;
    if (cntl == NULL)
    {
        nitf_Error_initf(error, NITF_CTXT, NITF_ERR_COMPRESSION,
                "Write operation in not progress");
        return NITF_FAILURE;
    }

    /* Call the compression end function */

    if(nitfI->compressor != NULL)
    {
        if(!(*(nitfI->compressor->end))(nitfI->compressionControl,io,error))
        {
            return NITF_FAILURE;
        }
    }
    
    /*      Flush the object */
    
    ret = nitf_ImageIO_flush(object, io, error);
    
    /*      Destroy the I/O control */
    
    nitf_ImageIOControl_destruct(&(cntl->cntl));
    nitf_ImageIOWriteControl_destruct(&
                                      (((_nitf_ImageIO *) object)->
                                       writeControl));
    
    return ret;
}


NITFPROT(NITF_BOOL) nitf_ImageIO_flush(nitf_ImageIO * object,
                                       nitf_IOInterface* io,
                                       nitf_Error * error)
{
    /* Internal view of object */
    _nitf_ImageIOWriteControl *wrtCntl;
    nitf_Off currentOffset;            /* File offset when called (restored) */

    wrtCntl = ((_nitf_ImageIO *) object)->writeControl;

    if (wrtCntl == NULL)
    {
        nitf_Error_initf(error, NITF_CTXT, NITF_ERR_MEMORY,
                         "Write operation in not progress");
        return NITF_FAILURE;
    }

    currentOffset = nitf_IOInterface_tell(io, error);

    if (!nitf_ImageIO_writeMasks(wrtCntl->cntl->nitf, io, error))
        return NITF_FAILURE;

    if (!NITF_IO_SUCCESS(nitf_IOInterface_seek(io, currentOffset, NITF_SEEK_SET, error)))
        return NITF_FAILURE;

    return NITF_SUCCESS;
}


/*========================= nitf_ImageIO_writeSequential =====================*/

NITFPROT(NITF_BOOL) nitf_ImageIO_writeSequential(nitf_ImageIO * nitf,
                                                 nitf_IOInterface* io,
                                                 nitf_Error * error)
{
    _nitf_ImageIO *nitfI;       /* Internal version of nitf */
    nitf_SubWindow *subWindow;  /* Needed for the constructor */
    _nitf_ImageIOControl *cntl; /* I/O control structure */
    /* The write control structure */
    _nitf_ImageIOWriteControl *writeCntl;

    nitfI = (_nitf_ImageIO *) nitf;

    /* *possibly* revert the optimized modes */
    nitf_ImageIO_revertOptimizedModes(nitfI, 0);

    /*      Check for I/O in progress */

    if ((nitfI->writeControl != NULL) || (nitfI->readControl != NULL))
    {
        nitf_Error_initf(error, NITF_CTXT, NITF_ERR_MEMORY,
                         "I/O operation in progress");
        return NITF_FAILURE;
    }

    /*      Create I/O control */

    subWindow = nitf_SubWindow_construct(error);
    subWindow->startRow = 0;
    subWindow->numRows = nitfI->numRows;
    subWindow->startCol = 0;
    subWindow->numCols = nitfI->numColumns;
    subWindow->bandList = NULL;
    subWindow->numBands = nitfI->numBands;

    cntl = nitf_ImageIOControl_construct(nitfI, io, NULL,
                                         subWindow, 0 /* Writing */ ,
                                         error);
    nitf_SubWindow_destruct(&subWindow);

    if (cntl == NULL)
        return NITF_FAILURE;

    /*      Get the result object */

    writeCntl = nitf_ImageIOWriteControl_construct(cntl, io,
                SEQUENTIAL_ALL_BANDS,
                error);
    if (writeCntl == NULL)
    {
        nitf_ImageIOControl_destruct(&cntl);
        return NITF_FAILURE;
    }

    nitfI->writeControl = writeCntl;
    return NITF_SUCCESS;
}


NITFPROT(NITF_BOOL) nitf_ImageIO_writeRows(nitf_ImageIO * object,
                                           nitf_IOInterface* io,
                                           nitf_Uint32 numRows,
                                           nitf_Uint8 ** data,
                                           nitf_Error * error)
{
    _nitf_ImageIO *nitf;        /* Parent _nitf_ImageIO object */
    /* Internal representation */
    _nitf_ImageIOWriteControl *cntl;
    /* Associated IO control object */
    _nitf_ImageIOControl *ioCntl;
    nitf_Uint32 idxIO;          /* Current block IO index (linear array) */
    nitf_Uint32 nBlockCols;       /* Number of block columns */
    nitf_Uint32 numBands;         /* Number of bands */
    nitf_Uint32 col;            /* Block column index */
    nitf_Uint32 row;            /* Current row in sub-window */
    nitf_Uint32 band;           /* Current band in sub-window */
    _nitf_ImageIOBlock *blockIO; /* The current  block IO structure */

    cntl = ((_nitf_ImageIO *) object)->writeControl;
    if (cntl == NULL)
    {
        nitf_Error_initf(error, NITF_CTXT, NITF_ERR_MEMORY,
                         "Write operation in not progress");
        return NITF_FAILURE;
    }

    ioCntl = cntl->cntl;
    nitf = ioCntl->nitf;
    numBands = ioCntl->numBandSubset;
    nBlockCols = ioCntl->nBlockIO / numBands;

    /* Check for row out of bounds */

    if (cntl->nextRow + numRows > nitf->numRows)
    {
        nitf_Error_initf(error, NITF_CTXT, NITF_ERR_MEMORY,
                         "Row write request of %ld rows at row %ld exceeds row limit %ld",
                         numRows, cntl->nextRow, nitf->numRows);
        return NITF_FAILURE;
    }

    /*     Setup user pointers in blockIO structures */

    blockIO = &(ioCntl->blockIO[0][0]);
    for (idxIO = 0; idxIO < ioCntl->nBlockIO; idxIO++)
    {
        /* Current user data pointer from "data" argument */
        nitf_Uint8 *user;

        user = data[ioCntl->bandSubset[blockIO->band]];

        /* Setup user and read/write buffers to start and reset offsets */

        blockIO->user.buffer = user;
        if (blockIO->userEqBuffer)
            blockIO->rwBuffer.buffer = user;

        blockIO->user.offset.mark = blockIO->user.offset.orig;
        blockIO->rwBuffer.offset.mark = blockIO->rwBuffer.offset.orig;
        blockIO += 1;
    }

    /*      Main write loop */
    blockIO = &(ioCntl->blockIO[0][0]);
    blockIO->currentRow = cntl->nextRow;
    for (col = 0; col < nBlockCols; col++)
    {
        for (row = 0; row < numRows; row++)
        {
            for (band = 0; band < numBands; band++)
            {
                blockIO = &(ioCntl->blockIO[col][band]);

                if (nitf->vtbl.pack != NULL)
                    (*(nitf->vtbl.pack)) (blockIO, error);
                else
                    memcpy(blockIO->rwBuffer.buffer,blockIO->user.buffer
                          + blockIO->user.offset.mark,blockIO->readCount);

                if (blockIO->doIO)
                {
                    if (nitf->vtbl.format != NULL)
                        (*(nitf->vtbl.format)) (blockIO->rwBuffer.buffer +
                                                blockIO->rwBuffer.offset.mark,
                                                blockIO->formatCount,
                                                nitf->pixel.shift);

                    if (!(*(nitf->vtbl.writer)) (blockIO, io, error))
                        return NITF_FAILURE;
                }
                blockIO->currentRow += 1;

                /*
                 * You have to check for last row and not call 
                 * nitf_ImageIO_nextRow because if the last row is the last 
                 * line in the last block, you can
                 * wind up accessing past the end of the block mask while 
                 * setting-up for
                 * the non-existant next block.
                 */
                if (row != nitf->numRows - 1)
                    nitf_ImageIO_nextRow(blockIO, 0);

                if (blockIO->rowsUntil == 0)
                    blockIO->rowsUntil = nitf->numRowsPerBlock - 1;
                else
                    blockIO->rowsUntil -= 1;
            }
        }
    }

    cntl->nextRow += numRows;
    return NITF_SUCCESS;
}


NITFPROT(NITF_BOOL) nitf_ImageIO_setPadPixel(nitf_ImageIO * object,
                                             nitf_Uint8 * value,
                                             nitf_Uint32 length,
                                             nitf_Error* error)
{
    
    _nitf_ImageIO* nio = (_nitf_ImageIO*)object;

    if (length > NITF_IMAGE_IO_PAD_MAX_LENGTH)
        length = NITF_IMAGE_IO_PAD_MAX_LENGTH;

    memmove(nio->pixel.pad, value, length);
    
    /*  The pad value is stored big endian */

    switch (length)
    {
    case 2:
    {
        nitf_Uint16* int16 = 
            (nitf_Uint16*)&(nio->pixel.pad);
        *int16 = NITF_HTONS(*int16);
        break;

    }
    case 4:
    {
        nitf_Uint32* int32 = 
            (nitf_Uint32*)&(nio->pixel.pad);
        *int32 = NITF_HTONL(*int32);
        break;
    }
    case 8:
    {
        nitf_Uint64* int64 =
            (nitf_Uint64*)&(nio->pixel.pad);
        
        if (nio->pixel.type == NITF_IMAGE_IO_PIXEL_TYPE_C)
        {
            *int64 = NITF_HTONLC(*int64);
            
        }
        else
        {
            *int64 = NITF_HTONLL(*int64);
        }
        break;
    }

        /* The 16 byte complex pixel is not actually possible */
    default:
        nitf_Error_initf(error, NITF_CTXT, NITF_ERR_INVALID_PARAMETER, 
                         "Invalid format size [%s]", length);
        return NITF_FAILURE;
    }

    return NITF_SUCCESS;
}


/*=================== nitf_ImageIO_pixelSize =================================*/

NITFPROT(nitf_Uint32) nitf_ImageIO_pixelSize(nitf_ImageIO * nitf)
{
    return (((_nitf_ImageIO *) nitf)->pixel.bytes);
}


/*=================== nitf_ImageIO_pixelSize =================================*/

NITFPROT(NITF_BOOL) nitf_ImageIO_setFileOffset(nitf_ImageIO * nitf,
        nitf_Uint64 offset,
        nitf_Error * error)
{
    _nitf_ImageIO *nitfI;       /* Internal version of the handle */

    nitfI = (_nitf_ImageIO *) nitf;

    /* Write in progress */
    if (nitfI->writeControl != NULL)
    {
        nitf_Error_initf(error, NITF_CTXT, NITF_ERR_MEMORY,
                         "Can not change file offset during I/O operation");
        return NITF_FAILURE;
    }

    /*
     * The two relevant fields are imageBase and pixelBase. pixelBase the
     * imageBase offset plus another offset so subtract the current imageBase
     * value from pixelBase and then add the new offset.
     */

    nitfI->pixelBase -= nitfI->imageBase;
    nitfI->imageBase = offset;
    nitfI->pixelBase += offset;
    return NITF_SUCCESS;
}


NITFPROT(nitf_BlockingInfo *) nitf_BlockingInfo_construct(nitf_Error *
        error)
{
    /*  Allocate the info object */
    nitf_BlockingInfo *info =
        (nitf_BlockingInfo *) NITF_MALLOC(sizeof(nitf_BlockingInfo));

    /*  Return now if we have a problem above */
    if (!info)
    {
        nitf_Error_init(error, NITF_STRERROR(NITF_ERRNO),
                        NITF_CTXT, NITF_ERR_MEMORY);
        return NULL;
    }

    return info;
}


/*=================== nitf_BlockingInfo_destruct ===========================*/
NITFPROT(void) nitf_BlockingInfo_destruct(nitf_BlockingInfo ** info)
{
    NITF_FREE(*info);
    *info = NULL;
    return;
}


/*=================== nitf_ImageIO_getBlockingInfo ===========================*/

NITFPROT(nitf_BlockingInfo *)
nitf_ImageIO_getBlockingInfo(nitf_ImageIO *
                             image,
                             nitf_IOInterface* io,
                             nitf_Error *error)
{
    _nitf_ImageIO *img;         /* Internal representation of object */
    nitf_BlockingInfo *result;  /* The requested information */

    img = (_nitf_ImageIO *) image;

    /*      Create the block mask if it has not been done already */

    if (img->blockMask == NULL)
    {
        if (!nitf_ImageIO_mkMasks(img, io, 1, error))
            return NULL;
    }

    /*      Allocate the result */

    result = nitf_BlockingInfo_construct(error);
    if (result == NULL)
    {
        return NULL;
    }

    if (img->blockInfoFlag)
    {
        *result = img->blockInfo; /* Make a copy */
        return result;
    }

    img->blockInfo.numBlocksPerRow = img->nBlocksPerRow;
    img->blockInfo.numBlocksPerCol = img->nBlocksPerColumn;
    img->blockInfo.numRowsPerBlock = img->numRowsPerBlock;
    img->blockInfo.numColsPerBlock = img->numColumnsPerBlock;
    img->blockInfo.length = img->blockSize;

    /*
     * Create the decompression control object if image is compressed.
     * This is done here because the decompressor may update the blocking
     * information.
     */
    
    if ((img->decompressor != NULL) && (img->decompressionControl == NULL))
    {
        img->decompressionControl =
            (*(img->decompressor->open)) (io, img->pixelBase,
                                          img->dataLength -
                                          img->maskHeader.imageDataOffset,
                                          &(img->blockInfo), img->blockMask,
                                          error);
        if (img->decompressionControl == NULL)
        {
            nitf_BlockingInfo_destruct(&result);
            return NULL;
        }
    }

    img->blockInfoFlag = 1;       /* Only do this once */
    *result = img->blockInfo;     /* Make a copy */
    return result;
}

NITFPROT(int) nitf_ImageIO_setWriteCaching(nitf_ImageIO * nitf, int enable)
{
    _nitf_ImageIO *initf;   /* Internal representation of object */
    int saved;              /* Saved result */
    
    initf = (_nitf_ImageIO *) nitf;
    saved = initf->cachedWriteFlag;
    if (enable)
    {
        initf->vtbl.writer = nitf_ImageIO_cachedWriter;
        initf->cachedWriteFlag = 1;
    }
    else
    {
        initf->vtbl.writer = nitf_ImageIO_uncachedWriter;
        initf->cachedWriteFlag = 0;
    }

    return saved;
}
NITFPROT(void) nitf_ImageIO_setReadCaching(nitf_ImageIO * nitf)
{
    _nitf_ImageIO *initf;   /* Internal representation of object */

    initf = (_nitf_ImageIO *) nitf;
    initf->vtbl.reader = nitf_ImageIO_cachedReader;

    return;
}

/*=================== nitf_BlockingInfo_print ================================*/

NITFPROT(void) nitf_BlockingInfo_print(nitf_BlockingInfo * info,
                                       FILE * file)
{
#ifdef NITF_DEBUG
    if (file == NULL)
        file = stdout;

    fprintf(file, "nitf_ImageIOBlockingInfo structure:\n");
    fprintf(file, "  Number of blocks per row: %ld\n",
            info->numBlocksPerRow);
    fprintf(file, "  Number of blocks per column: %ld\n",
            info->numBlocksPerCol);
    fprintf(file, "  Number of rows per block: %ld\n",
            info->numRowsPerBlock);
    fprintf(file, "  Number of columns per block: %ld\n",
            info->numColsPerBlock);
    fprintf(file, "  Block length in bytes: %ld\n", info->length);
#endif
    return;
}


/*============================================================================*/
/*======================== Internal Functions ================================*/
/*============================================================================*/

NITFPRIV(void) nitf_ImageIO_revertOptimizedModes(_nitf_ImageIO *nitfI, int numBands)
{
    if (nitfI->blockingMode == NITF_IMAGE_IO_BLOCKING_MODE_RGB24 &&
        (numBands == 3 || numBands == 0))
    {
        numBands = 3;
        /* revert to normal 'P' mode */
        nitfI->blockingMode = NITF_IMAGE_IO_BLOCKING_MODE_P;
        nitfI->vtbl.setup = nitf_ImageIO_setup_P;
        nitfI->vtbl.done = nitf_ImageIO_setup_P;
        nitfI->numBands = numBands;
        nitfI->pixel.bytes /= numBands;
        /* reset the unpack functions */
        nitf_ImageIO_setUnpack(nitfI);
    }
    else if (nitfI->blockingMode == NITF_IMAGE_IO_BLOCKING_MODE_IQ &&
        (numBands == 2 || numBands == 0))
    {
        numBands = 2;
        /* revert to normal 'P' mode */
        nitfI->blockingMode = NITF_IMAGE_IO_BLOCKING_MODE_P;
        nitfI->vtbl.setup = nitf_ImageIO_setup_P;
        nitfI->vtbl.done = nitf_ImageIO_setup_P;
        nitfI->numBands = numBands;
        nitfI->pixel.bytes /= numBands;
        /* reset the unpack functions */
        nitf_ImageIO_setUnpack(nitfI);
        if (nitfI->vtbl.unformat)
        {
            switch (nitfI->pixel.bytes) 
            {
                case 8:
                    nitfI->vtbl.unformat = nitf_ImageIO_swapOnly_8;
                    break;
                case 4:
                    nitfI->vtbl.unformat = nitf_ImageIO_swapOnly_4;
                    break;
                case 2:
                    nitfI->vtbl.unformat = nitf_ImageIO_swapOnly_2;
                    break;
                case 1:
                    nitfI->vtbl.unformat = NULL;
                    break;
                default:
                    /* No optimized mode */
                    return;
            }
        }
    }
}

/*======================== nitf_ImageIO_decodeModes ==========================*/

NITFPRIV(int) nitf_ImageIO_decodeModes(_nitf_ImageIO * nitf,
                                       nitf_ImageSubheader *subhdr,
                                       nitf_Error * error)
{
    /*      Decode compression */

    char *blockingMode = NULL;
    char *compression = NULL;

    blockingMode = subhdr->imageMode->raw;
    compression = subhdr->imageCompression->raw;

    if (strncmp(compression, "NC", 2) == 0)
    {
        nitf->compression = NITF_IMAGE_IO_COMPRESSION_NC;
    }
    else if (strncmp(compression, "NM", 2) == 0)
    {
        nitf->compression = NITF_IMAGE_IO_COMPRESSION_NM;
    }
    else if (strncmp(compression, "C1", 2) == 0)
    {
        nitf->compression = NITF_IMAGE_IO_COMPRESSION_C1;
    }
    else if (strncmp(compression, "C3", 2) == 0)
    {
        nitf->compression = NITF_IMAGE_IO_COMPRESSION_C3;
    }
    else if (strncmp(compression, "C4", 2) == 0)
    {
        nitf->compression = NITF_IMAGE_IO_COMPRESSION_C4;
    }
    else if (strncmp(compression, "C5", 2) == 0)
    {
        nitf->compression = NITF_IMAGE_IO_COMPRESSION_C5;
    }
    else if (strncmp(compression, "C6", 2) == 0)
    {
        nitf->compression = NITF_IMAGE_IO_COMPRESSION_C6;
    }
    else if (strncmp(compression, "C8", 2) == 0)
    {
        nitf->compression = NITF_IMAGE_IO_COMPRESSION_C8;
    }
    else if (strncmp(compression, "I1", 2) == 0)
    {
        nitf->compression = NITF_IMAGE_IO_COMPRESSION_I1;
    }
    else if (strncmp(compression, "M1", 2) == 0)
    {
        nitf->compression = NITF_IMAGE_IO_COMPRESSION_M1;
    }
    else if (strncmp(compression, "M3", 2) == 0)
    {
        nitf->compression = NITF_IMAGE_IO_COMPRESSION_M3;
    }
    else if (strncmp(compression, "M4", 2) == 0)
    {
        nitf->compression = NITF_IMAGE_IO_COMPRESSION_M4;
    }
    else if (strncmp(compression, "M5", 2) == 0)
    {
        nitf->compression = NITF_IMAGE_IO_COMPRESSION_M5;
    }
    else if (strncmp(compression, "M8", 2) == 0)
    {
        nitf->compression = NITF_IMAGE_IO_COMPRESSION_M8;
    }
    else
    {
        nitf_Error_initf(error, NITF_CTXT, NITF_ERR_READING_FROM_FILE,
                         "Invalid compression type %s", compression);
        return NITF_FAILURE;
    }

    if (blockingMode[0] == 'B')
    {
        nitf->blockingMode = NITF_IMAGE_IO_BLOCKING_MODE_B;
        nitf->oneBand = 0;
        nitf->vtbl.setup = nitf_ImageIO_setup_SBR;
        nitf->vtbl.reader = NULL;
        nitf->vtbl.writer = NULL;
        nitf->vtbl.done = nitf_ImageIO_done_SBR;
    }
    else if (blockingMode[0] == 'P')
    {
        if (nitf->numBands == 3
            && strncmp("RGB", subhdr->imageRepresentation->raw, 3) == 0
            && nitf->pixel.bytes == 1
            && (nitf->compression
                & (NITF_IMAGE_IO_COMPRESSION_NC
                    | NITF_IMAGE_IO_COMPRESSION_NM)))
        {
            nitf->blockingMode = NITF_IMAGE_IO_BLOCKING_MODE_RGB24;
            /* now, we fake the nbands and bytes */
            nitf->pixel.bytes *= nitf->numBands;
            nitf->numBands = 1;
            nitf->vtbl.setup = nitf_ImageIO_setup_SBR;
            nitf->vtbl.done = nitf_ImageIO_setup_SBR;
        }
        else if (nitf->numBands == 2
            && ((subhdr->bandInfo[0]->subcategory->raw[0] == 'I'
                && subhdr->bandInfo[1]->subcategory->raw[0] == 'Q'))
            && (nitf->compression
                & (NITF_IMAGE_IO_COMPRESSION_NC
                    | NITF_IMAGE_IO_COMPRESSION_NM)))
        {
            nitf->blockingMode = NITF_IMAGE_IO_BLOCKING_MODE_IQ;
            /* now, we fake the nbands and bytes */
            nitf->pixel.bytes *= nitf->numBands;
            nitf->numBands = 1;
            nitf->vtbl.setup = nitf_ImageIO_setup_SBR;
            nitf->vtbl.done = nitf_ImageIO_setup_SBR;
            /* changing the unformat will only matters on LE systems
             * this is required to do proper byte-swapping
             */
            if (nitf->vtbl.unformat)
            {
                switch (nitf->pixel.bytes)
                {
                    case 16:
                        nitf->vtbl.unformat = nitf_ImageIO_swapOnly_16c;
                        break;
                    case 8:
                        nitf->vtbl.unformat = nitf_ImageIO_swapOnly_8c;
                        break;
                    case 4:
                        nitf->vtbl.unformat = nitf_ImageIO_swapOnly_4c;
                        break;
                    case 2:
                        nitf->vtbl.unformat = NULL;
                        break;
                    default:
                        nitf_Error_initf(error, NITF_CTXT, NITF_ERR_READING_FROM_FILE,
                                         "Invalid number of bytes in complex data %d", nitf->pixel.bytes);
                         return 0;
                }
            }
        }
        else
        {
            nitf->blockingMode = NITF_IMAGE_IO_BLOCKING_MODE_P;
            nitf->vtbl.setup = nitf_ImageIO_setup_P;
            nitf->vtbl.done = nitf_ImageIO_setup_P;
        }

        nitf->oneBand = 0;
        nitf->vtbl.reader = NULL;
        nitf->vtbl.writer = NULL;
    }
    else if (blockingMode[0] == 'R')
    {
        nitf->blockingMode = NITF_IMAGE_IO_BLOCKING_MODE_R;
        nitf->oneBand = 0;
        nitf->vtbl.setup = nitf_ImageIO_setup_SBR;
        nitf->vtbl.reader = NULL;
        nitf->vtbl.writer = NULL;
        nitf->vtbl.done = nitf_ImageIO_done_SBR;
    }
    else if (blockingMode[0] == 'S')
    {
        nitf->blockingMode = NITF_IMAGE_IO_BLOCKING_MODE_S;
        nitf->oneBand = 1;
        nitf->vtbl.setup = nitf_ImageIO_setup_SBR;
        nitf->vtbl.reader = NULL;
        nitf->vtbl.writer = NULL;
        nitf->vtbl.done = nitf_ImageIO_done_SBR;
    }
    else
    {
        nitf_Error_initf(error, NITF_CTXT, NITF_ERR_READING_FROM_FILE,
                         "Invalid blocking mode %s", blockingMode);
        return NITF_FAILURE;
    }

    /* Check for unimplemented blocking mode */
    if (nitf->vtbl.setup == NULL)
    {
        nitf_Error_initf(error, NITF_CTXT, NITF_ERR_READING_FROM_FILE,
                         "Unimplemented blocking mode %s", blockingMode);
        return NITF_FAILURE;
    }

    return NITF_SUCCESS;
}

NITFPRIV(void) nitf_ImageIO_setIO(_nitf_ImageIO * nitf)
{

    /* For now, used uncached reading for uncompressed images */

    if ((nitf->pixel.type != NITF_IMAGE_IO_PIXEL_TYPE_B)
         && (nitf->pixel.type != NITF_IMAGE_IO_PIXEL_TYPE_12)
                      && (nitf->compression & NITF_IMAGE_IO_NO_COMPRESSION))
    {
        nitf->vtbl.reader = nitf_ImageIO_uncachedReader;
        nitf->vtbl.writer = nitf_ImageIO_uncachedWriter;
    }
    else
    {
        nitf->vtbl.reader = nitf_ImageIO_cachedReader;
        nitf->vtbl.writer = nitf_ImageIO_cachedWriter;
    }

    return;
}



/* Table used for selecting unformat format functions */

/* Pixel characteristics flags */

#define SWAP    (0x00000001)
#define NOSWAP  (0x00000002)
#define JUST    (0x00000004)
#define NOJUST  (0x00000008)
#define SIGN    (0x00000010)
#define NOSIGN  (0x00000020)

#define BYTE_BASE (0x100)
#define BYTES_1   (BYTE_BASE << 1)
#define BYTES_2   (BYTE_BASE << 2)
#define BYTES_4   (BYTE_BASE << 4)
#define BYTES_8   (BYTE_BASE << 8)
#define BYTES_16  (BYTE_BASE << 16)

typedef struct
{
    /* Pixel types mask */
    nitf_Uint32 types;

    /* Pixel size in byte flas */
    nitf_Uint32 bytes;

    /* Characteristics flag. swap */
    nitf_Uint32 swap;

    /* Characteristics flag. sign */
    nitf_Uint32 sign;

    /* Characteristics flag, just */
    nitf_Uint32 just;

    /* The function to use for unformat */
    _NITF_IMAGE_IO_UNFORMAT_FUNC unfmt;

    /* The function to use for format */
    _NITF_IMAGE_IO_FORMAT_FUNC fmt;
}
unformatTable; /* Also used for format */

#define NENTRIES 34

static unformatTable table[NENTRIES] =
{
    /* Bi-endian full bits integers (NBPP=ABPP)
       and right justified INT */
    {
        NITF_IMAGE_IO_PIXEL_TYPE_B, BYTES_1, NOSWAP, NOSIGN, NOJUST,
        NULL, NULL
    },
    {     /* 12-bit */
        NITF_IMAGE_IO_PIXEL_TYPE_12,
        BYTES_2, NOSWAP, NOSIGN, NOJUST,
        NULL, NULL
    },
    {
        NITF_IMAGE_IO_PIXEL_TYPE_INT | NITF_IMAGE_IO_PIXEL_TYPE_SI,
        BYTES_1 | BYTES_2 | BYTES_4 | BYTES_8, NOSWAP, NOSIGN, NOJUST,
        NULL, NULL
    },
    /* Bi-endian reals */
    {
        NITF_IMAGE_IO_PIXEL_TYPE_R,
        BYTES_4 | BYTES_8,
        NOSWAP, NOSIGN, NOJUST,
        NULL, NULL
    },
    /* Bi-endian complex */
    {
        NITF_IMAGE_IO_PIXEL_TYPE_C,
        BYTES_8 | BYTES_16 | BYTES_4,
        NOSWAP, NOSIGN, NOJUST,
        NULL, NULL
    },
    /* Little-endian full bits integers (NBPP=ABPP),
       right justified INT and floats */
    {
        NITF_IMAGE_IO_PIXEL_TYPE_B, BYTES_1, SWAP, NOSIGN, NOJUST,
        NULL, NULL
    },
    {     /* 12-bit */
        NITF_IMAGE_IO_PIXEL_TYPE_12,
        BYTES_2, SWAP, NOSIGN, NOJUST,
        NULL, NULL
    },
    {
        NITF_IMAGE_IO_PIXEL_TYPE_INT | NITF_IMAGE_IO_PIXEL_TYPE_SI,
        BYTES_2, SWAP, NOSIGN, NOJUST,
        nitf_ImageIO_swapOnly_2, nitf_ImageIO_swapOnly_2
    },
    
    {
        NITF_IMAGE_IO_PIXEL_TYPE_INT | NITF_IMAGE_IO_PIXEL_TYPE_SI |
        NITF_IMAGE_IO_PIXEL_TYPE_R,
        BYTES_4, SWAP, NOSIGN, NOJUST,
        nitf_ImageIO_swapOnly_4, nitf_ImageIO_swapOnly_4
    },
    
    {
        NITF_IMAGE_IO_PIXEL_TYPE_INT | NITF_IMAGE_IO_PIXEL_TYPE_R,
        BYTES_8, SWAP, NOSIGN, NOJUST,
        nitf_ImageIO_swapOnly_8, nitf_ImageIO_swapOnly_8
    },
    
    /* Little-endian complex */
    {
        NITF_IMAGE_IO_PIXEL_TYPE_C,
        BYTES_4, SWAP, NOSIGN, NOJUST,
        nitf_ImageIO_swapOnly_4, nitf_ImageIO_swapOnly_4
    },
    {
        NITF_IMAGE_IO_PIXEL_TYPE_C,
        BYTES_8, SWAP, NOSIGN, NOJUST,
        nitf_ImageIO_swapOnly_8c, nitf_ImageIO_swapOnly_8c
    },
    
    {
        NITF_IMAGE_IO_PIXEL_TYPE_C,
        BYTES_16, SWAP, NOSIGN, NOJUST,
        nitf_ImageIO_swapOnly_16c, nitf_ImageIO_swapOnly_16c
    },

    /* Bi-endian INT, left justified */
    {
        NITF_IMAGE_IO_PIXEL_TYPE_INT, BYTES_1, NOSWAP, NOSIGN, JUST,
        nitf_ImageIO_unformatUShift_1, nitf_ImageIO_formatShift_1
    },
    {
        NITF_IMAGE_IO_PIXEL_TYPE_INT, BYTES_2, NOSWAP, NOSIGN, JUST,
        nitf_ImageIO_unformatUShift_2, nitf_ImageIO_formatShift_2
    },
    {
        NITF_IMAGE_IO_PIXEL_TYPE_INT, BYTES_4, NOSWAP, NOSIGN, JUST,
        nitf_ImageIO_unformatUShift_4, nitf_ImageIO_formatShift_4
    },
    {
        NITF_IMAGE_IO_PIXEL_TYPE_INT, BYTES_8, NOSWAP, NOSIGN, JUST,
        nitf_ImageIO_unformatUShift_8, nitf_ImageIO_formatShift_8
    },
    /* Little-endian INT, left justified */
    {
        NITF_IMAGE_IO_PIXEL_TYPE_INT, BYTES_2, SWAP, NOSIGN, JUST,
        nitf_ImageIO_unformatSwapUShift_2, nitf_ImageIO_formatShiftSwap_2
    },
    {
        NITF_IMAGE_IO_PIXEL_TYPE_INT, BYTES_4, SWAP, NOSIGN, JUST,
        nitf_ImageIO_unformatSwapUShift_4, nitf_ImageIO_formatShiftSwap_4
    },
    {
        NITF_IMAGE_IO_PIXEL_TYPE_INT, BYTES_8, SWAP, NOSIGN, JUST,
        nitf_ImageIO_unformatSwapUShift_8, nitf_ImageIO_formatShiftSwap_8
    },
    /* Bi-endian SI, left justified */
    {
        NITF_IMAGE_IO_PIXEL_TYPE_SI, BYTES_1, NOSWAP, SIGN, JUST,
        nitf_ImageIO_unformatShift_1, nitf_ImageIO_formatShift_1
    },
    {
        NITF_IMAGE_IO_PIXEL_TYPE_SI, BYTES_2, NOSWAP, SIGN, JUST,
        nitf_ImageIO_unformatShift_2, nitf_ImageIO_formatShift_2
    },
    {
        NITF_IMAGE_IO_PIXEL_TYPE_SI, BYTES_4, NOSWAP, SIGN, JUST,
        nitf_ImageIO_unformatShift_4, nitf_ImageIO_formatShift_4
    },
    {
        NITF_IMAGE_IO_PIXEL_TYPE_SI, BYTES_8, NOSWAP, SIGN, JUST,
        nitf_ImageIO_unformatShift_8, nitf_ImageIO_formatShift_8
    },
    /* Little-endian INT, left justified */
    {
        NITF_IMAGE_IO_PIXEL_TYPE_SI, BYTES_2, SWAP, SIGN, JUST,
        nitf_ImageIO_unformatSwapShift_2, nitf_ImageIO_formatShiftSwap_2
    },
    {
        NITF_IMAGE_IO_PIXEL_TYPE_SI, BYTES_4, SWAP, SIGN, JUST,
        nitf_ImageIO_unformatSwapShift_4, nitf_ImageIO_formatShiftSwap_4
    },
    {
        NITF_IMAGE_IO_PIXEL_TYPE_SI, BYTES_8, SWAP, SIGN, JUST,
        nitf_ImageIO_unformatSwapShift_8, nitf_ImageIO_formatShiftSwap_8
    },
    /* Bi-endian SI, right justified */
    {
        NITF_IMAGE_IO_PIXEL_TYPE_SI, BYTES_1, NOSWAP, SIGN, NOJUST,
        nitf_ImageIO_unformatExtend_1, nitf_ImageIO_formatMask_1
    },
    {
        NITF_IMAGE_IO_PIXEL_TYPE_SI, BYTES_2, NOSWAP, SIGN, NOJUST,
        nitf_ImageIO_unformatExtend_2, nitf_ImageIO_formatMask_2
    },
    {
        NITF_IMAGE_IO_PIXEL_TYPE_SI, BYTES_4, NOSWAP, SIGN, NOJUST,
        nitf_ImageIO_unformatExtend_4, nitf_ImageIO_formatMask_4
    },
    {
        NITF_IMAGE_IO_PIXEL_TYPE_SI, BYTES_8, NOSWAP, SIGN, NOJUST,
        nitf_ImageIO_unformatExtend_8, nitf_ImageIO_formatMask_8
    },
    /* Little-endian INT, right justified */
    {
        NITF_IMAGE_IO_PIXEL_TYPE_SI, BYTES_2, SWAP, SIGN, NOJUST,
        nitf_ImageIO_unformatSwapExtend_2, nitf_ImageIO_formatMaskSwap_2
    },
    {
        NITF_IMAGE_IO_PIXEL_TYPE_SI, BYTES_4, SWAP, SIGN, NOJUST,
        nitf_ImageIO_unformatSwapExtend_4, nitf_ImageIO_formatMaskSwap_4
    },
    {
        NITF_IMAGE_IO_PIXEL_TYPE_SI, BYTES_8, SWAP, SIGN, NOJUST,
        nitf_ImageIO_unformatSwapExtend_8, nitf_ImageIO_formatMaskSwap_8
    }
};

NITFPRIV(int) nitf_ImageIO_setPixelDef(_nitf_ImageIO * nitf,
                                       char *pixelType, 
                                       nitf_Uint32 nBits,
                                       nitf_Uint32 nBitsActual,
                                       char *justify, 
                                       nitf_Error * error)
{
    char pixelTypeBuf[4];  /* Buffer for justified pixel type string */
    char *pixelTypePtr;    /* Pointer into justified pixel type string */
    char *pixelTypeBufPtr; /* Pointer into justified pixel type buffer */
    nitf_Uint32 swap;      /* Pixel characteristics flag, swap */
    nitf_Uint32 sign;      /* Pixel characteristics flag, sign */
    nitf_Uint32 just;      /* Pixel characteristics flag, just */
    int cmplx;             /* Complex pixel type if TRUE */
    int found;             /* Found a valid unformat/format function */
    int i;

    /* Common field setup */
    sign = 0;
    just = 0;

    nitf->pixel.bytes = NITF_NBPP_TO_BYTES(nBits);
    nitf->pixel.shift = nBits - nBitsActual;

    if (nitf->pixel.bytes == 1)
        nitf->pixel.swap = 0;
    else
        nitf->pixel.swap = !nitf_ImageIO_bigEndian();

    if (nitf->pixel.swap)
        swap = SWAP;
    else
        swap = NOSWAP;

    if ((justify[0] == 'L') && (nitf->pixel.shift != 0))
        just = JUST;
    else
        just = NOJUST;

    /* Sign extension */

    sign = NOSIGN;

    /* Decode type string */

    /* Eliminate spaces from string */

    pixelTypePtr = pixelType;
    pixelTypeBufPtr = pixelTypeBuf;
    for (i = 0; i < 3; i++)
    {
        if (*pixelTypePtr == 0)
            break;

        if (*pixelTypePtr != ' ')
            *(pixelTypeBufPtr++) = *pixelTypePtr;

        pixelTypePtr += 1;
    }
    *pixelTypeBufPtr = 0;

    cmplx = 0;
    if ((nBits == 12) && (nBitsActual == 12))
    {
        nitf->pixel.type = NITF_IMAGE_IO_PIXEL_TYPE_12;
        nitf->pixel.bytes = 2;
        nitf->pixel.shift = 0;
    }
    else if (strncmp(pixelType, "INT", 3) == 0)
    {
        nitf->pixel.type = NITF_IMAGE_IO_PIXEL_TYPE_INT;
    }
    else if (pixelType[0] == 'B')
    {
        nitf->pixel.type = NITF_IMAGE_IO_PIXEL_TYPE_B;
        nitf->pixel.bytes = 1;
        nitf->pixel.shift = 0;
    }
    else if (pixelType[0] == 'S' && pixelType[1] == 'I')
    {
        nitf->pixel.type = NITF_IMAGE_IO_PIXEL_TYPE_SI;
        if (nitf->pixel.shift != 0)
            sign = SIGN;
        else
            sign = NOSIGN;
    }
    else if (pixelType[0] == 'R')
    {
        nitf->pixel.type = NITF_IMAGE_IO_PIXEL_TYPE_R;
    }
    else if (pixelType[0] == 'C')
    {
        nitf->pixel.type = NITF_IMAGE_IO_PIXEL_TYPE_C;
        cmplx = 1;
    }
    else
    {
        nitf_Error_initf(error, NITF_CTXT, NITF_ERR_READING_FROM_FILE,
                         "Invalid pixel type X %s", pixelType);
        return NITF_FAILURE;
    }

    /*      Pixel unformat and format functions */

    found = 0;
    for (i = 0; i < NENTRIES; i++)
    {
        if ((nitf->pixel.type & table[i].types) &&
            ((BYTE_BASE << nitf->pixel.bytes) & table[i].bytes) &&
            (swap & table[i].swap) &&
            (sign & table[i].sign) && (just & table[i].just))
        {
            nitf->vtbl.unformat = table[i].unfmt;
            nitf->vtbl.format = table[i].fmt;
            found = 1;
            break;
        }
    }
    
    if (!found)
    {
        nitf_Error_initf(error, NITF_CTXT, NITF_ERR_READING_FROM_FILE,
                         "Invalid pixel options: Type %s bits %ld justification %1s",
                         pixelType, nBits, justify);
        return NITF_FAILURE;
    }
    
    return NITF_SUCCESS;
}


NITFPRIV(void) nitf_ImageIO_setUnpack(_nitf_ImageIO * nitf)
{

    /*
     *      Unpacking is required in three cases:
     *
     *            Band interleved by pixel
     *            Binary pixels ("B" pixel type)
     *            Both of the above
     */

    /*      None of the cases are implemented */
    nitf->vtbl.unpack = nitf->vtbl.pack = NULL;

    if (nitf->blockingMode == NITF_IMAGE_IO_BLOCKING_MODE_P)
    {
        switch (nitf->pixel.bytes)
        {
            case 1:
                nitf->vtbl.unpack = nitf_ImageIO_unpack_P_1;
                nitf->vtbl.pack = nitf_ImageIO_pack_P_1;
                break;
            case 2:
                nitf->vtbl.unpack = nitf_ImageIO_unpack_P_2;
                nitf->vtbl.pack = nitf_ImageIO_pack_P_2;
                break;
            case 4:
                nitf->vtbl.unpack = nitf_ImageIO_unpack_P_4;
                nitf->vtbl.pack = nitf_ImageIO_pack_P_4;
                break;
            case 8:
                nitf->vtbl.unpack = nitf_ImageIO_unpack_P_8;
                nitf->vtbl.pack = nitf_ImageIO_pack_P_8;
                break;
            default:               /* Should never happen */
            case 16:
                nitf->vtbl.unpack = nitf_ImageIO_unpack_P_16;
                nitf->vtbl.pack = nitf_ImageIO_pack_P_16;
                break;
        }
    }
    return;
}

NITFPRIV(_nitf_ImageIOBlock **) nitf_ImageIO_allocBlockArray
(nitf_Uint32 numColumns, nitf_Uint32 numBands, nitf_Error * error)
{
    _nitf_ImageIOBlock **blockIOs;        /*!< The result */
    _nitf_ImageIOBlock *blockIOPtr;       /*!< The linear array of structures */
    nitf_Uint32 i;

    blockIOs =
        (_nitf_ImageIOBlock **) NITF_MALLOC(sizeof(_nitf_ImageIOBlock *) *
                                            numColumns);
    if (blockIOs == NULL)
    {
        nitf_Error_initf(error, NITF_CTXT, NITF_ERR_MEMORY,
                         "Error allocating block I/O structure: %s",
                         NITF_STRERROR(NITF_ERRNO));
        return NITF_FAILURE;
    }

    blockIOPtr =
        (_nitf_ImageIOBlock *) NITF_MALLOC(sizeof(_nitf_ImageIOBlock) *
                                           numColumns * numBands);
    if (blockIOPtr == NULL)
    {
        nitf_Error_initf(error, NITF_CTXT, NITF_ERR_MEMORY,
                         "Error allocating block I/O structure: %s",
                         NITF_STRERROR(NITF_ERRNO));
        NITF_FREE(blockIOs);
        return NITF_FAILURE;
    }

    memset(blockIOPtr, 0, sizeof(_nitf_ImageIOBlock) * numColumns * numBands);

    for (i = 0; i < numColumns; i++)
    {
        blockIOs[i] = blockIOPtr;
        blockIOPtr += numBands;
    }

    return blockIOs;
}


NITFPRIV(void) nitf_ImageIO_freeBlockArray(_nitf_ImageIOBlock *** blockIOs)
{
    /* Dereferenced argument */
    _nitf_ImageIOBlock **blockIOsDeref;

    if (blockIOs == NULL)
        return;

    blockIOsDeref = *blockIOs;

    if (blockIOsDeref[0] != NULL)
        NITF_FREE(blockIOsDeref[0]);

    NITF_FREE(blockIOsDeref);

    *blockIOs = NULL;
    return;
}


int nitf_ImageIO_setup_SBR(_nitf_ImageIOControl * cntl, nitf_Error * error)
{
    _nitf_ImageIO *nitf;        /* Parent _nitf_ImageIO object */
    nitf_Uint32 startBlockRow;  /* Starting blockRow */
    nitf_Uint32 endBlockRow;    /* Ending blockRow */
    nitf_Uint32 startBlockCol;  /* Starting blockCol */
    nitf_Uint32 endBlockCol;    /* Ending blockCol */
    nitf_Uint32 nBlockRows;     /* Number of blockRows */
    nitf_Uint32 nBlockCols;     /* Number of blockCols */
    nitf_Uint32 startBlock;     /* Block number of start block */
    nitf_Uint32 startRowInBlock0; /* Start row in the first block (pixels) */
    nitf_Uint32 startColumnInBlock0;/* Start column in the first block (pixels) */
    nitf_Uint32 residual;       /* Partial sample columns previous block */
    nitf_Uint32 myResidual;     /* Partial sample columns current block */
    nitf_Uint32 bytes;          /* Bytes per pixel */
    _nitf_ImageIOBlock **blockIOs;/* The block I/O control structures */
    _nitf_ImageIOBlock *blockIO;  /* The current block I/O control structure */
    nitf_Uint32 columnCountFR;  /* Remaining column transfer count (bytes) */
    nitf_Uint32 numColsFR;      /* Total number of columns requested FR */
    nitf_Uint32 startRowThisBlock;      /* Start row for current block (pixels) */
    /* Start column for current block (pixels) */
    nitf_Uint32 startColumnThisBlock;
    nitf_Uint32 blockNumber;    /* Current block number */
    nitf_Uint32 userOff;        /* Offsets into user buffers */
    nitf_Uint32 bandCnt;        /* Number of bands in the read */
    nitf_Uint32 bandIdx;        /* Current band index */
    nitf_Uint32 blockIdx;       /* Current block index */
    nitf_Uint32 band;           /* Current band */
    nitf_Uint32 modeBlockBaseOffset; /* Block pixel offset based on mode */
    nitf_Uint32 modeMaskOffset; /* Mask offset based on mode */
    nitf_Uint8 *writeBuffer;    /* Write buffer */
    nitf_Uint8 *readBuffer;     /* Read buffer */
    nitf_Uint8 blockColIdx;     /* Current block column index */
    nitf_Uint8 *cacheBuffer;    /* Current cach buffer */
    NITF_BOOL freeCacheBuffer;  /* Sets block control free flag */
    /* Resets freeCacheBuffer flag */
    NITF_BOOL freeCacheBufferReset;

    nitf = cntl->nitf;

    /*
      BlockRows and BlockCols variables manipulate block row and column indexes.
      They have units of blocks and are used to calculate the start block number
      and other block mask indexes and increments.
    */

    /*
     *    Calculate block row and column values
     *
     * The sub image row and column sizes are at down-sampled resolution but most
     * of the calculation require full resolution. Full resolution count are
     * obtained by multiplying by the pixel skips
     *
     * The tests applied to endBlockRow and endBlockColumn are looking for the case
     * where the last down-sample neighborhood in the subwindow extends paritally
     * beyond the edge of the image (right or bottom edge). This case is the
     * exception to the rule that the request subwindow must be entirely in the
     * image. In this case the end block calculation gives a block row or column
     * out of range.
     *
     */

    startBlockRow = cntl->row / nitf->numRowsPerBlock;
    endBlockRow = (cntl->numRows * (cntl->rowSkip) + cntl->row - 1) /
                nitf->numRowsPerBlock;
    if (endBlockRow >= nitf->nBlocksPerRow)
        endBlockRow -= 1;

    startBlockCol = cntl->column / nitf->numColumnsPerBlock;
    endBlockCol = (cntl->numColumns * (cntl->columnSkip) + cntl->column - 1) /
                nitf->numColumnsPerBlock;
    if (endBlockCol >= nitf->nBlocksPerRow)
        endBlockCol -= 1;

    nBlockRows = endBlockRow - startBlockRow + 1;
    nBlockCols = endBlockCol - startBlockCol + 1;
    startBlock = startBlockRow * nitf->nBlocksPerRow + startBlockCol;

    startRowInBlock0 = cntl->row - startBlockRow * nitf->numRowsPerBlock;
    startColumnInBlock0 = cntl->column - startBlockCol * nitf->numColumnsPerBlock;

    /*
       Initialize increment values. For writing, the user buffer is not
       used as the write buffer, so the buffer is not incremented
     */

    bytes = nitf->pixel.bytes;
    cntl->numberInc = nitf->nBlocksPerRow;
    cntl->userInc = cntl->numColumns * bytes;
    if (cntl->reading)
    {
        if (cntl->downSampling)
            cntl->bufferInc =
                (nitf->numColumnsPerBlock + cntl->columnSkip) * bytes;
        else
            /* FR == DR */
            cntl->bufferInc = cntl->userInc;
    }
    else
        cntl->bufferInc = 0;

    /* Create the block I/O structures */

    bandCnt = cntl->numBandSubset;

    blockIOs = nitf_ImageIO_allocBlockArray(nBlockCols, bandCnt, error);
    if (blockIOs == NULL)
        return NITF_FAILURE;

    /* Set-up BlockIO's for each band */

    cntl->nBlockIO = nBlockCols * bandCnt;
    cntl->blockIO = blockIOs;
    numColsFR = cntl->numColumns * (cntl->columnSkip);
    columnCountFR = numColsFR;
    startRowThisBlock = startRowInBlock0;
    startColumnThisBlock = startColumnInBlock0;
    residual = 0;               /* Residual from block -1 is 0 */
    myResidual = 0;             /* Avoids an uninitialized variable warning */
    blockNumber = startBlock;

    /*
     * Allocate write buffer if writing or allocate read buffer if reading
     * with down-sample
     */

    writeBuffer = NULL;         /* Avoids uninitialized variable warning */
    readBuffer = NULL;          /* Avoids uninitialized variable warning */
    if (cntl->reading)
    {
        if (cntl->downSampling)
        {
            /*
             * The buffer should contain one sample window
             * height rows of the width of a block plus one 
             * sample window width. The extra width is used to construct
             * neighborhoods that are split across blocks. 
             * There has to be one buffer per band since the band loop
             * is inside the row loop and a down-sample height
             * number of rows must be accumulated before 
             * you can reuse the buffer.
             */
            readBuffer = (nitf_Uint8 *) NITF_MALLOC((cntl->rowSkip) *
                                                    (nitf->numColumnsPerBlock +
                                                     cntl->columnSkip) *
                                                    bytes * bandCnt);
            if (readBuffer == NULL)
            {
                nitf_Error_initf(error, NITF_CTXT, NITF_ERR_MEMORY,
                                 "Error allocating read buffer: %s",
                                 NITF_STRERROR(NITF_ERRNO));
                return NITF_FAILURE;
            }
        }
    }
    else
    {
        writeBuffer =
            (nitf_Uint8 *) NITF_MALLOC(nitf->numColumnsPerBlock * bytes);
        if (writeBuffer == NULL)
        {
            nitf_Error_initf(error, NITF_CTXT, NITF_ERR_MEMORY,
                             "Error allocating write buffer: %s",
                             NITF_STRERROR(NITF_ERRNO));
            if (readBuffer != NULL)
                NITF_FREE(readBuffer);
            return NITF_FAILURE;
        }
    }

    blockIO = &(blockIOs[0][0]);    /* Eliminates spurious warning */
    userOff = 0;
    blockColIdx = 0;
    for (blockIdx = 0; blockIdx < nBlockCols; blockIdx++)
    {
        /*
         * The write cach set-up requires that a buffer be allocated
         * for each block I/O for mode S but all block I/O from the
         * same column share the same buffer. The block control structure
         * freeFlag tells the destructor when to free the buffers so as
         * not to double free a buffer. This is the reason for the flag
         * variables
         */

        freeCacheBuffer = 1;       /* Always allocate first band */
        freeCacheBufferReset = 1;  /* Do allocate after first band */
        if ((nitf->blockingMode != NITF_IMAGE_IO_BLOCKING_MODE_S)
            && nitf->cachedWriteFlag)
        {
            cacheBuffer =
                (nitf_Uint8 *) NITF_MALLOC(nitf->blockSize);
            if (cacheBuffer == NULL)
            {
                nitf_Error_initf(error, NITF_CTXT, 
                                 NITF_ERR_MEMORY,
                                 "Error allocating block buffer: %s",
                                 NITF_STRERROR(NITF_ERRNO));
                return NITF_FAILURE;
            }
            freeCacheBufferReset = 1; /* Do not allocate after first band */
        }
        else
            cacheBuffer = NULL; /* This is meaningless */

        for (bandIdx = 0; bandIdx < bandCnt; bandIdx++)
        {
            band = cntl->bandSubset[bandIdx];
            if ((nitf->blockingMode == NITF_IMAGE_IO_BLOCKING_MODE_S)
                    && freeCacheBuffer  && nitf->cachedWriteFlag)
            {
                cacheBuffer =
                    (nitf_Uint8 *) NITF_MALLOC(nitf->blockSize);
                if (cacheBuffer == NULL)
                {
                    nitf_Error_initf(error, NITF_CTXT, NITF_ERR_MEMORY,
                                     "Error allocating block buffer: %s", 
                                     NITF_STRERROR(NITF_ERRNO));
                    return NITF_FAILURE;
                }
            }

            blockIO = &(blockIOs[blockIdx][bandIdx]);
            if (nitf->blockingMode == NITF_IMAGE_IO_BLOCKING_MODE_S)
            {
                cntl->blockOffsetInc = nitf->numColumnsPerBlock * bytes;
                modeBlockBaseOffset = 0;
                modeMaskOffset =
                    band * nitf->nBlocksPerRow * nitf->nBlocksPerColumn;
                blockIO->blockOffset.orig =
                    modeBlockBaseOffset + startColumnThisBlock * bytes;

                blockIO->blockOffset.mark =
                    blockIO->blockOffset.orig +
                    ((nitf_Uint64) startRowThisBlock) *
                    ((nitf_Uint64) nitf->numColumnsPerBlock) *
                    ((nitf_Uint64)  bytes);
            }
            else if (nitf->blockingMode == NITF_IMAGE_IO_BLOCKING_MODE_B)
            {
                cntl->blockOffsetInc = nitf->numColumnsPerBlock * bytes;
                modeBlockBaseOffset =
                    band * nitf->numColumnsPerBlock * nitf->numRowsPerBlock *
                    bytes;
                modeMaskOffset = 0;
                blockIO->blockOffset.orig =
                    modeBlockBaseOffset + startColumnThisBlock * bytes;

                blockIO->blockOffset.mark =
                    blockIO->blockOffset.orig +
                    ((nitf_Uint64) startRowThisBlock) *
                    ((nitf_Uint64) nitf->numColumnsPerBlock) *
                    ((nitf_Uint64)  bytes);
            }
            else /* IMODE == "R" */
            {
                cntl->blockOffsetInc =
                    nitf->numColumnsPerBlock * (nitf->numBands) * bytes;
                modeBlockBaseOffset = band * nitf->numColumnsPerBlock * bytes;
                modeMaskOffset = 0;
                blockIO->blockOffset.orig =
                    modeBlockBaseOffset + startColumnThisBlock * bytes;

                blockIO->blockOffset.mark =
                    blockIO->blockOffset.orig +
                    ((nitf_Uint64) startRowThisBlock) *
                    ((nitf_Uint64) nitf->numColumnsPerBlock) *
                    ((nitf_Uint64) nitf->numBands) *
                    ((nitf_Uint64)  bytes);
            }

            blockIO->cntl = cntl;
            blockIO->band = band;
            blockIO->doIO = 1;
            blockIO->number = blockNumber;
            /*
             * See description of _nitf_ImageIOControl for an
             * explaination of the - 1 
             */
            blockIO->rowsUntil = nitf->numRowsPerBlock - startRowInBlock0 - 1;
            blockIO->blockMask = nitf->blockMask + modeMaskOffset;
            blockIO->padMask = nitf->padMask + modeMaskOffset;
            blockIO->imageDataOffset = blockIO->blockMask[blockNumber];
            blockIO->pixelCountFR =
                (nitf->numColumnsPerBlock - startColumnThisBlock);
            /* Last block may not span full block */
            if (blockIO->pixelCountFR > columnCountFR)
                blockIO->pixelCountFR = columnCountFR;
            blockIO->readCount = blockIO->pixelCountFR * bytes;
            blockIO->padColumnCount = 0;
            blockIO->padRowCount = 0;
            blockIO->currentRow = cntl->row;

            /* Calculate the block column's residual and myResidual */

            if (residual == 0)
                blockIO->sampleStartColumn = 0;
            else
                blockIO->sampleStartColumn = cntl->columnSkip - residual;

            blockIO->pixelCountDR =
                (blockIO->pixelCountFR -
                 blockIO->sampleStartColumn) / (cntl->columnSkip);
            /*
             * If there is a pixel split between this and the 
             * previous block, add one to the pixel count size 
             * the split pixel is precessed this block. A start
             * column not zero indicates this
             */
            if (blockIO->sampleStartColumn > 0)
                blockIO->pixelCountDR += 1;
            
            /*
             * Only update residual once per block column and only 
             * if down-sampling.  The last column block is a special
             * case because the normal myResidual calculation is based
             * on read count which is not the right value in this
             * case. myResidual only needs to be updated for the first
             * band in a block column since the value will be the same
             * for each band but this ad
             */

            blockIO->residual = residual;
            if (bandIdx == 0)
            {
                if (blockColIdx != (nBlockCols - 1))
                {
                    myResidual =
                        (blockIO->pixelCountFR -
                         blockIO->sampleStartColumn) % cntl->columnSkip;
                }
                else
                {
                    if (cntl->column + numColsFR > nitf->numColumns)
                        myResidual =
                            cntl->column + numColsFR - nitf->numColumns;
                    else
                        myResidual = 0;
                }
            }
            
            /*
             * If there is a partial sample window at the end of the 
             * line that extends
             * beyond the physical block size, the DR count must be 
             * incremented to account for this last partial neighborhood.
             */
            if (cntl->downSampling && (blockColIdx == (nBlockCols - 1)) &&
                (cntl->column + numColsFR > nitf->numColumnsActual))
                blockIO->pixelCountDR += 1;
            
            if ((bandIdx >= (bandCnt - 1)) && cntl->downSampling)
            {
                blockColIdx += 1;
                residual = myResidual;
            }
            blockIO->myResidual = myResidual;
            
            blockIO->formatCount = blockIO->pixelCountDR * (cntl->columnSkip);
            
            /*
             * The user and buffer fields will be the same for reading
             * but a separate buffer is required for writing. The data is 
             * copied into the buffer so that the format function does not 
             * modifiy the user supplied data. This is not required for
             * reading. The write buffer only needs enough save for one write.
             */

            if (cntl->userBase != NULL)
                blockIO->user.buffer = cntl->userBase[bandIdx];
            else
                blockIO->user.buffer = NULL;
            blockIO->user.offset.mark = userOff;
            blockIO->user.offset.orig = userOff;

            if (cntl->reading)
            {
                if (cntl->downSampling)
                {
                    /* There is a different buffer for each band */
                    blockIO->rwBuffer.buffer = readBuffer +
                        (cntl->rowSkip) * (nitf->numColumnsPerBlock +
                                           cntl->columnSkip) * bytes * bandIdx;
                    blockIO->rwBuffer.offset.mark = blockIO->residual * bytes;
                    blockIO->rwBuffer.offset.orig = blockIO->residual * bytes;
                    blockIO->userEqBuffer = 0;
                }
                else
                {   
                    /* Read directly into user buffer */
                    blockIO->rwBuffer.buffer = blockIO->user.buffer;
                    /* FR == DR */
                    blockIO->rwBuffer.offset.mark = userOff;
                    blockIO->rwBuffer.offset.orig = userOff;
                    blockIO->userEqBuffer = 1;
                }
            }
            else
            {                   
                /* Allocate a buffer shared by all block I/O's */
                blockIO->rwBuffer.buffer = writeBuffer;
                blockIO->rwBuffer.offset.mark = 0;
                blockIO->rwBuffer.offset.orig = 0;
                blockIO->userEqBuffer = 0;
            }

            /*
             * Initialize the unpacked buffer, for SBR modes this 
             * is the read/write buffer
             */
            cntl->unpackedInc = cntl->bufferInc;
            blockIO->unpacked.buffer = blockIO->rwBuffer.buffer;
            blockIO->unpacked.offset.mark = blockIO->rwBuffer.offset.mark;
            blockIO->unpacked.offset.orig = blockIO->rwBuffer.offset.orig;
            blockIO->unpackedNoFree = 1;

            /*
             * Initialize block control (used for cached writes)
             */
            blockIO->blockControl.number = NITF_IMAGE_IO_NO_BLOCK;
            blockIO->blockControl.freeFlag = freeCacheBuffer;
            freeCacheBuffer = freeCacheBufferReset;
            blockIO->blockControl.block = cacheBuffer;
        }

        userOff += blockIO->pixelCountDR * bytes;
        
        startColumnThisBlock = 0;

        /*
         * Start at the begining after
         * first block column 
         */
        columnCountFR -= blockIO->pixelCountFR;
        blockNumber += 1;
    }

    /* Set pad buffer size */

    cntl->padBuffer = NULL; /* Created the first time it is used */
    if (cntl->reading)
    {
        if (cntl->numColumns < nitf->numColumnsPerBlock)
            cntl->padBufferSize =
                cntl->numColumns * nitf->pixel.bytes * cntl->columnSkip;
        else
            cntl->padBufferSize = nitf->numColumnsPerBlock * nitf->pixel.bytes;
    }
    else
        cntl->padBufferSize = nitf->numColumnsPerBlock * nitf->pixel.bytes;

    /*
     * Set-up last block column for column pad if writing
     *
     * Writing of pad pixels is required if the actual number of columns is
     * greater than the NITF header number of columns field and the sub-image
     * being written includes the last pixel in the line.
     *
     * If all of this is TRUE, then set the padColumnCount field in the 
     * blocks that are at the end of the rows. There will be one per band.
     *
     * The actual row length will be bigger if the block size is not an integral
     * multiple of the row length.
     *
     */

    if (!(cntl->reading)
        && (nitf->numColumnsActual > nitf->numColumns)
        && (cntl->column + cntl->numColumns >= nitf->numColumns))
    {
        /* The relevent blockIO's are at the end of the row 
         * (the last block column) 
         */
        for (bandIdx = 0; bandIdx < bandCnt; bandIdx++)
        {
            blockIO = &(cntl->blockIO[nBlockCols - 1][bandIdx]);
            blockIO->padColumnCount =
                (nitf->numColumnsActual - nitf->numColumns) * bytes;
            blockIO += nBlockCols;
        }
    }

    /*
     * Set-up block columns for row pad if writing
     *
     * Writing of pad pixels is also required if the actual number of rows is
     * greater than the NITF header number of rows field and the sub-image
     * being written includes the last row in the image.
     *
     * If all of this is TRUE, then set the padRowCount field 
     * in all of the blocks.
     * Since the block structures represent columns of blocks, 
     * all of the columns
     * will include the end of the image if any do.
     */
    if (!(cntl->reading)
        && (nitf->numRowsActual > nitf->numRows)
        && (cntl->row + cntl->numRows >= nitf->numRows))
    {
        blockIO = &(cntl->blockIO[0][0]);
        for (blockIdx = 0; blockIdx < cntl->nBlockIO; blockIdx++)
        {
            if (nitf->blockingMode == NITF_IMAGE_IO_BLOCKING_MODE_R)
                blockIO->padRowCount =
                    (nitf->numRowsActual - nitf->numRows) * (nitf->numBands);
            else
                blockIO->padRowCount = nitf->numRowsActual - nitf->numRows;
            
            blockIO += 1;
        }
    }
    
    cntl->ioCount = (size_t)nBlockCols * nitf->numBands * cntl->numRows;
    return NITF_SUCCESS;
}

int nitf_ImageIO_done_SBR(_nitf_ImageIOControl * cntl, nitf_Error * error)
{

    return NITF_SUCCESS;
}


int nitf_ImageIO_setup_P(_nitf_ImageIOControl * cntl, nitf_Error * error)
{
    _nitf_ImageIO *nitf = NULL; /* Parent _nitf_ImageIO object */
    nitf_Uint32 startBlockRow;  /* Starting blockRow */
    nitf_Uint32 endBlockRow;    /* Ending blockRow */
    nitf_Uint32 startBlockCol;  /* Starting blockCol */
    nitf_Uint32 endBlockCol;    /* Ending blockCol */
    nitf_Uint32 nBlockRows;     /* Number of blockRows */
    nitf_Uint32 nBlockCols;     /* Number of blockCols */
    nitf_Uint32 startBlock;     /* Block number of start block */
    nitf_Uint32 startRowInBlock0; /* Start row in the first block (pixels) */
    nitf_Uint32 startColumnInBlock0; /* Start column in the first block (pixels) */
    nitf_Uint32 residual;       /* Partial sample columns previous block */
    nitf_Uint32 myResidual;     /* Partial sample columns current block */
    nitf_Uint32 bytes;          /* Bytes per pixel */
    _nitf_ImageIOBlock **blockIOs = NULL;/* The block I/O control structures */
    _nitf_ImageIOBlock *blockIO = NULL; /* The current block I/O control structure */
    nitf_Uint32 columnCountFR;  /* Remaining column transfer count (bytes) */
    nitf_Uint32 numColsFR;      /* Total number of columns requested FR */
    nitf_Uint32 startRowThisBlock;      /* Start row for current block (pixels) */
    /* Start column for current block (pixels) */
    nitf_Uint32 startColumnThisBlock;
    nitf_Uint32 blockNumber;    /* Current block number */
    nitf_Uint32 userOff;        /* Offsets into user buffers */
    nitf_Uint32 bandCnt;        /* Number of bands in the read */
    nitf_Uint32 bandIdx;        /* Current band index */
    nitf_Uint32 blockIdx;       /* Current block index */
    nitf_Uint32 band;           /* Current band */
    nitf_Uint8 *ioBuffer = NULL; /* I/O buffer */
    nitf_Uint8 *unpackedBuffer = NULL; /* Unpacked data buffer */
    nitf_Uint8 blockColIdx;     /* Current block column index */
    nitf_Uint8 *cacheBuffer = NULL; /* Current cach buffer */
    NITF_BOOL freeCacheBuffer;  /* Sets block control free flag */
    /* Resets freeCacheBuffer flag */
    NITF_BOOL freeCacheBufferReset;

    nitf = cntl->nitf;

    /*
     * BlockRows and BlockCols variables manipulate block row 
     * and column indexes.
     * They have units of blocks and are used to calculate the 
     * start block number
     * and other block mask indexes and increments.
     */

    /*
     * Calculate block row and column values  (see nitf_ImageIO_setup_SBR
     * for more information
     */
    startBlockRow = cntl->row / nitf->numRowsPerBlock;
    endBlockRow = (cntl->numRows * (cntl->rowSkip) + cntl->row - 1) /
        nitf->numRowsPerBlock;
    if (endBlockRow >= nitf->nBlocksPerRow)
        endBlockRow -= 1;
    
    startBlockCol = cntl->column / nitf->numColumnsPerBlock;
    endBlockCol = (cntl->numColumns * (cntl->columnSkip) + cntl->column - 1) /
        nitf->numColumnsPerBlock;
    if (endBlockCol >= nitf->nBlocksPerRow)
        endBlockCol -= 1;
    
    nBlockRows = endBlockRow - startBlockRow + 1;
    nBlockCols = endBlockCol - startBlockCol + 1;
    startBlock = startBlockRow * nitf->nBlocksPerRow + startBlockCol;
    
    startRowInBlock0 = cntl->row - startBlockRow * nitf->numRowsPerBlock;
    startColumnInBlock0 = cntl->column - startBlockCol * nitf->numColumnsPerBlock;
    
    /*
     * Initialize increment values. For writing, the user buffer is not
     * used as the write buffer, so the buffer is not incremented
     */
    bytes = nitf->pixel.bytes;
    cntl->numberInc = nitf->nBlocksPerRow;
    cntl->blockOffsetInc = (nitf->numColumnsPerBlock) * (nitf->numBands) * bytes;
    cntl->userInc = cntl->numColumns * bytes;
    cntl->bufferInc = 0;
    if (cntl->reading)
    {
        if (cntl->downSampling)
            cntl->unpackedInc =
                (nitf->numColumnsPerBlock + cntl->columnSkip) * bytes;
        else
            /* FR == DR */
            cntl->unpackedInc = cntl->userInc;
    }
    else
        cntl->unpackedInc = 0;
    
    /* Create the block I/O structures */
    
    bandCnt = cntl->numBandSubset;
    
    blockIOs = nitf_ImageIO_allocBlockArray(nBlockCols, bandCnt, error);
    if (blockIOs == NULL)
        return NITF_FAILURE;
    
    /* Set-up BlockIO's for each band */
    cntl->nBlockIO = nBlockCols * bandCnt;
    cntl->blockIO = blockIOs;
    columnCountFR = cntl->numColumns * (cntl->columnSkip);
    numColsFR = cntl->numColumns * (cntl->columnSkip);
    startRowThisBlock = startRowInBlock0;
    startColumnThisBlock = startColumnInBlock0;
    residual = 0;               /* Residual from block -1 is 0 */
    myResidual = 0;             /* Avoids an uninitialized variable warning */
    blockNumber = startBlock;
    
    /* Allocate I/O and unpacked buffer */
    if (cntl->downSampling)
    {
        unpackedBuffer = (nitf_Uint8 *) NITF_MALLOC((nitf->numColumnsPerBlock +
                                                     cntl->columnSkip) *
                                                    (nitf->numBands) *
                                                    (cntl->rowSkip) *
                                                    bytes);
        if (unpackedBuffer == NULL)
        {
            nitf_Error_initf(error, NITF_CTXT, NITF_ERR_MEMORY,
                             "Error allocating unpacked data buffer: %s",
                             NITF_STRERROR(NITF_ERRNO));
            return NITF_FAILURE;
        }
    }
    else
        unpackedBuffer = NULL;
    
    ioBuffer =
        (nitf_Uint8 *) NITF_MALLOC(nitf->numColumnsPerBlock * (nitf->numBands) *
                                   bytes);
    if (ioBuffer == NULL)
    {
        nitf_Error_initf(error, NITF_CTXT, NITF_ERR_MEMORY,
                         "Error allocating I/O buffer: %s",
                         NITF_STRERROR(NITF_ERRNO));
        if (unpackedBuffer != NULL)
            NITF_FREE(unpackedBuffer);
        return NITF_FAILURE;
    }

    /*    Initialize blocks */
    blockIO = &(blockIOs[0][0]);    /* Eliminates spurious warning */
    userOff = 0;
    blockColIdx = 0;
    for (blockIdx = 0; blockIdx < nBlockCols; blockIdx++)
    {
        freeCacheBuffer = 1;       /* Always allocate first band */
        freeCacheBufferReset = 0;  /* Do not allocate after first band */
        if (nitf->cachedWriteFlag)
        {
            cacheBuffer =
                (nitf_Uint8 *) NITF_MALLOC(nitf->blockSize);
            if (cacheBuffer == NULL)
            {
                nitf_Error_initf(error, NITF_CTXT, NITF_ERR_MEMORY,
                                 "Error allocating block buffer: %s", 
                                 NITF_STRERROR(NITF_ERRNO));
                return NITF_FAILURE;
            }
        }

        for (bandIdx = 0; bandIdx < bandCnt; bandIdx++)
        {
            band = cntl->bandSubset[bandIdx];
            
            blockIO = &(blockIOs[blockIdx][bandIdx]);
            blockIO->cntl = cntl;
            blockIO->band = band;

            /*
             * When reading, only the first blockIO in
             * each column does the actual read,
             * the rest just unpack
             *
             * When writing, only the last blockIO in each 
             * column does the actual write,
             * the rest just pack
             */
            if (cntl->reading)
                blockIO->doIO = (bandIdx == 0) ? 1 : 0;
            else
                blockIO->doIO = (bandIdx == (nitf->numBands - 1)) ? 1 : 0;
            blockIO->number = blockNumber;
            /* 
             * See description of _nitf_ImageIOControl for an 
             * explaination of the - 1 
             */
            blockIO->rowsUntil = nitf->numRowsPerBlock - startRowInBlock0 - 1;
            blockIO->blockMask = nitf->blockMask;
            blockIO->padMask = nitf->padMask;
            blockIO->imageDataOffset = blockIO->blockMask[blockNumber];
            blockIO->blockOffset.orig =
                startColumnThisBlock * (nitf->numBands) * bytes;
            
            blockIO->blockOffset.mark =
                blockIO->blockOffset.orig +
                ((nitf_Uint64) startRowThisBlock) *
                ((nitf_Uint64) nitf->numColumnsPerBlock) *
                ((nitf_Uint64) nitf->numBands) *
                ((nitf_Uint64)  bytes);
            
            blockIO->pixelCountFR =
                (nitf->numColumnsPerBlock - startColumnThisBlock);

            /* Last block may not span full block */
            if (blockIO->pixelCountFR > columnCountFR)
                blockIO->pixelCountFR = columnCountFR;

            blockIO->readCount =
                blockIO->pixelCountFR * (nitf->numBands) * bytes;
            blockIO->padColumnCount = 0;
            blockIO->padRowCount = 0;
            blockIO->currentRow = cntl->row;
            
            /* Calculate the block column's residual and myResidual */
            if (residual == 0)
                blockIO->sampleStartColumn = 0;
            else
                blockIO->sampleStartColumn = cntl->columnSkip - residual;
            
            blockIO->pixelCountDR =
                (blockIO->pixelCountFR -
                 blockIO->sampleStartColumn) / (cntl->columnSkip);
            /*
             * If there is a pixel split between this and the 
             * previous block, add one
             * to the pixel count size the split pixel is 
             * precessed this block. A start
             * column not zero indicates this
             */

            if (blockIO->sampleStartColumn > 0)
                blockIO->pixelCountDR += 1;
            
            /*
             * Only update residual once per block column and 
             * only if down-sampling. The last column block is a 
             * special case because the normal myResidual
             * calculation is based on read count which is not the 
             * right value in this case. myResidual only needs to be 
             * updated for the first band in a block
             * column since the value will be the same for 
             * each band but this ad
             */
            
            blockIO->residual = residual;
            if (cntl->downSampling)
            {
                if (bandIdx == 0)
                {
                    if (blockColIdx != (nBlockCols - 1))
                    {
                        myResidual =
                            (blockIO->pixelCountFR -
                             blockIO->sampleStartColumn) % cntl->columnSkip;
                    }
                    else
                    {
                        if (cntl->column + numColsFR > nitf->numColumns)
                            myResidual =
                                cntl->column + numColsFR - nitf->numColumns;
                        else
                            myResidual = 0;
                    }
                }
                /*
                 * If there is a partial sample window at the end 
                 * of the line that extends
                 * beyond the physical block size, the DR count 
                 * must be incremented to
                 * account for this last partial neighborhood.
                 */
                if ((blockColIdx == (nBlockCols - 1)) &&
                    (cntl->column + numColsFR > nitf->numColumnsActual))
                    blockIO->pixelCountDR += 1;
                
                if (bandIdx >= (bandCnt - 1))
                {
                    blockColIdx += 1;
                    residual = myResidual;
                }
            }
            blockIO->myResidual = myResidual;
            
            if (cntl->downSampling)
                blockIO->formatCount =
                    blockIO->pixelCountDR * (cntl->columnSkip);
            else
                blockIO->formatCount = blockIO->pixelCountFR * nitf->numBands;
            
            /*
             * The user and buffer fields will be separate buffers since 
             * the amount read/written is nitf->numBands times more than
             * is required for any one band due to the interleaving
             *
             * The bufferOffset field gives the spacing between bands 
             * in the read case and is set to 0 in the write case.
             *
             * For reading, the first read for a given row segment reads 
             * all of the bands for that segment. The initial offset of 0
             * for the first band is the correct
             * buffer offset for the read. The pixel size times band offset
             * then correctly line-up the start buffer for the unpack 
             * operations.
             *
             * For writing, the buffer offset should be zero since the 
             * last block IO does the write. The offsets need for the bands
             * are calculated directly by the pack function.
             */
            if (cntl->userBase != NULL)
                blockIO->user.buffer = cntl->userBase[bandIdx];
            else
                blockIO->user.buffer = NULL;

            blockIO->user.offset.mark = userOff;
            blockIO->user.offset.orig = userOff;
            blockIO->rwBuffer.buffer = ioBuffer;
            if (cntl->reading)
            {
                blockIO->rwBuffer.offset.mark = bytes * band;
                blockIO->rwBuffer.offset.orig = bytes * band;
            }
            else
            {
                blockIO->rwBuffer.offset.mark = 0;
                blockIO->rwBuffer.offset.orig = 0;
            }
            blockIO->userEqBuffer = 0;

            /*
             * Initialize the unpacked buffer, for P modes this is the 
             * user buffer buffer unless there is down-sampling
             */

            /*
             * Initialize the unpacked buffer, for P modes this 
             * is a separate buffer
             */
            if (cntl->downSampling)
            {
                /* There is a different buffer for each band */
                blockIO->unpacked.buffer = unpackedBuffer +
                    (cntl->rowSkip) * (nitf->numColumnsPerBlock +
                                       cntl->columnSkip) * bytes * band;
                blockIO->unpacked.offset.mark = blockIO->residual * bytes;
                blockIO->unpacked.offset.orig = blockIO->residual * bytes;
                blockIO->unpackedNoFree = 0;
            }
            else
            {
                /* Read directly into user buffer */
                blockIO->unpacked.buffer = blockIO->user.buffer;
                /* FR == DR */
                blockIO->unpacked.offset.mark = blockIO->user.offset.mark;
                blockIO->unpacked.offset.orig = blockIO->user.offset.mark;
                blockIO->unpackedNoFree = 1;
            }

            /*
             * Initialize block control (used for cached writes)
             */
            blockIO->blockControl.number = NITF_IMAGE_IO_NO_BLOCK;
            blockIO->blockControl.freeFlag = freeCacheBuffer;
            freeCacheBuffer = freeCacheBufferReset;
            blockIO->blockControl.block = cacheBuffer;
        }

        userOff += blockIO->pixelCountDR * bytes;
        startColumnThisBlock = 0;
        /* 
         * Start at the begining after
         * first block column 
         */
        columnCountFR -= blockIO->pixelCountFR;
        blockNumber += 1;
    }

    /* Set pad buffer size */

    cntl->padBuffer = NULL; /* Created the first time it is used */
    if (cntl->reading)
    {
        if (cntl->numColumns < nitf->numColumnsPerBlock)
            cntl->padBufferSize = cntl->numColumns * (nitf->numBands) *
                (nitf->pixel.bytes * cntl->columnSkip);
        else
            cntl->padBufferSize = nitf->numColumnsPerBlock *
                (nitf->numBands) * (nitf->pixel.bytes);
    }
    else
        cntl->padBufferSize =
            nitf->numColumnsPerBlock * (nitf->numBands) * nitf->pixel.bytes;
    
    /*
     * Set-up last block column for column pad if writing
     * 
     * Writing of pad pixels is required if the actual number of columns is
     * greater than the NITF header number of columns field and the sub-image
     * being written includes the last pixel in the line.
     *
     * If all of this is TRUE, then set the padColumnCount 
     * field in the blocks that
     * are at the end of the rows. There will be one per band.
     *
     * The actual row length will be bigger if the block size is not an integral
     * multiple of the row length.
     *
     */

    if (!(cntl->reading)
            && (nitf->numColumnsActual > nitf->numColumns)
            && (cntl->column + cntl->numColumns >= nitf->numColumns))
    {
        /*
         * The relevent blockIO's are at the end of the row 
         * (the last block column) 
         */

        for (bandIdx = 0; bandIdx < bandCnt; bandIdx++)
        {
            blockIO = &(cntl->blockIO[nBlockCols - 1][bandIdx]);
            blockIO->padColumnCount =
                (nitf->numColumnsActual -
                 nitf->numColumns) * (nitf->numBands) * bytes;
        }
    }
    
    /*
     * Set-up block columns for row pad if writing
     * 
     * Writing of pad pixels is also required if the actual number of rows is
     * greater than the NITF header number of rows field and the sub-image
     * being written includes the last row in the image.
     *
     * If all of this is TRUE, then set the padRowCount field in 
     * all of the blocks.
     * Since the block structures represent columns of blocks, 
     * all of the columns
     * will include the end of the image if any do.
     */

    if (!(cntl->reading)
            && (nitf->numRowsActual > nitf->numRows)
            && (cntl->row + cntl->numRows >= nitf->numRows))
    {
        blockIO = &(cntl->blockIO[0][0]);
        for (blockIdx = 0; blockIdx < cntl->nBlockIO; blockIdx++)
        {
            blockIO->padRowCount = nitf->numRowsActual - nitf->numRows;
            blockIO += 1;
        }
    }

    cntl->ioCount = (size_t)nBlockCols * nitf->numBands * cntl->numRows;
    return NITF_SUCCESS;
}

int nitf_ImageIO_done_P(_nitf_ImageIOControl * cntl, nitf_Error * error)
{

    return NITF_SUCCESS;
}


NITFPRIV(_nitf_ImageIOControl *)
nitf_ImageIOControl_construct(_nitf_ImageIO * nitf, 
                              nitf_IOInterface* io,
                              nitf_Uint8 ** user,
                              nitf_SubWindow * subWindow, int reading,
                              nitf_Error * error)
{
    _nitf_ImageIOControl *cntl; /* The result */

    cntl =
        (_nitf_ImageIOControl *) NITF_MALLOC(sizeof(_nitf_ImageIOControl));
    if (cntl == NULL)
    {
        nitf_Error_initf(error, NITF_CTXT, NITF_ERR_MEMORY,
                         "Memory allocation error: %s", 
                         NITF_STRERROR(NITF_ERRNO));
        return NULL;
    }
    /* Initialize all fields to 0 */
    memset(cntl, 0, sizeof(_nitf_ImageIOControl));
    
    /* Set fields from arguments */
    cntl->nitf = nitf;
    cntl->numRows = subWindow->numRows;
    cntl->row = subWindow->startRow;
    cntl->numColumns = subWindow->numCols;
    cntl->column = subWindow->startCol;
    if (subWindow->downsampler != NULL)
    {
        cntl->rowSkip = subWindow->downsampler->rowSkip;
        cntl->columnSkip = subWindow->downsampler->colSkip;
    }
    else
    {
        cntl->rowSkip = 1;
        cntl->columnSkip = 1;
    }
    cntl->downSampling = (cntl->rowSkip != 1) || (cntl->columnSkip != 1);
    if (cntl->downSampling)
    {
        cntl->downSampleIn =
            (NITF_DATA **) NITF_MALLOC(subWindow->numBands *
                                       sizeof(nitf_Uint8 *));
        if (cntl->downSampleIn == NULL)
        {
            nitf_Error_initf(error, NITF_CTXT, NITF_ERR_MEMORY,
                             "Memory allocation error: %s",
                             NITF_STRERROR(NITF_ERRNO));
            return NULL;
        }
        cntl->downSampleOut =
            (NITF_DATA **) NITF_MALLOC(subWindow->numBands *
                                       sizeof(nitf_Uint8 *));
        if (cntl->downSampleOut == NULL)
        {
            nitf_Error_initf(error, NITF_CTXT, NITF_ERR_MEMORY,
                             "Memory allocation error: %s",
                             NITF_STRERROR(NITF_ERRNO));
            return NULL;
        }
    }
    else
    {
        cntl->downSampleIn = NULL;
        cntl->downSampleOut = NULL;
    }
    
    cntl->bandSubset =
        (nitf_Uint32 *) NITF_MALLOC(subWindow->numBands *
                                    sizeof(nitf_Uint32));
    if (cntl->bandSubset == NULL)
    {
        nitf_Error_initf(error, NITF_CTXT, NITF_ERR_MEMORY,
                         "Memory allocation error: %s", 
                         NITF_STRERROR(NITF_ERRNO));
        return NULL;
    }
    
    if (subWindow->bandList != NULL)
        memmove(cntl->bandSubset, subWindow->bandList,
                subWindow->numBands * sizeof(nitf_Uint32));
    else if (subWindow->numBands == nitf->numBands)
    {
        nitf_Uint32 i;
        for (i = 0; i < nitf->numBands; i++)
            cntl->bandSubset[i] = i;
    }
    else
    {
        nitf_Error_initf(error, NITF_CTXT, NITF_ERR_MEMORY,
                         "NULL band list with numBands not all bands");
        return NULL;
    }
    cntl->numBandSubset = subWindow->numBands;
    cntl->userBase = user;
    cntl->reading = reading;
    
    /*
     * If writing, create masks and check for masked compression type
     * A masked compression type requires block caching
     */
    
    if (!reading)
    {
        if ((nitf->blockMask == NULL)
            && (!nitf_ImageIO_mkMasks(nitf, io, 0, error)))
        {
            nitf_ImageIOControl_destruct(&cntl);
            return NULL;
        }
        
        if ((nitf->compression &
             (NITF_IMAGE_IO_COMPRESSION_NM
              | NITF_IMAGE_IO_COMPRESSION_M1
              | NITF_IMAGE_IO_COMPRESSION_M3
              | NITF_IMAGE_IO_COMPRESSION_M4
              | NITF_IMAGE_IO_COMPRESSION_M5
              | NITF_IMAGE_IO_COMPRESSION_M8)))
            nitf_ImageIO_setWriteCaching((nitf_ImageIO *) nitf, 1);
    }
    
    /*
     * Allocate column save buffer if down-sampling. The buffer is one
     * sample window width by the full resolution request number of rows times
     * the number of bands. Storage for each band is required because the band
     * loop is inside the row loop
     */
    
    if (cntl->downSampling)
    {
        /* Full resolution */
        cntl->columnSave =
            (nitf_Uint8 *) NITF_MALLOC((cntl->numRows) * (cntl->rowSkip) *
                                       (cntl->columnSkip) *
                                       (cntl->numBandSubset) *
                                       (nitf->pixel.bytes));
        if (cntl->columnSave == NULL)
        {
            nitf_Error_initf(error, NITF_CTXT, NITF_ERR_MEMORY,
                             "Memory allocation error: %s",
                             NITF_STRERROR(NITF_ERRNO));
            return NULL;
        }
    }
    
    /* Call mode specific setup function */
    if (!(*(nitf->vtbl.setup)) (cntl, error))
    {
        nitf_ImageIOControl_destruct(&cntl);
        return NULL;
    }
    
    if(nitf->compressor != NULL)
    {
        if(!(*(nitf->compressor->start))(nitf->compressionControl,
                        nitf->pixelBase,
                        nitf->dataLength -
                        nitf->maskHeader.imageDataOffset,
                        nitf->blockMask,nitf->padMask,
                        error) )
        {
            nitf_ImageIO_destruct((void **) &nitf);
            return NITF_FAILURE;
        }
    }

    cntl->padded = 0;
    return cntl;
}

NITFPRIV(void) nitf_ImageIOControl_destruct(_nitf_ImageIOControl ** cntl)
{
    _nitf_ImageIOBlock *blocks;     /* Block I/Os as a linrar array */
    nitf_Uint32 i;

    /* Actual object */
    _nitf_ImageIOControl *cntlActual;
    
    if (*cntl == NULL)
        return;
    
    cntlActual = *cntl;
    
    /* Free fields */
    
    if (cntlActual->blockIO != NULL)
    {
        /* Free buffer */
        if (!(cntlActual->blockIO[0][0].userEqBuffer))
            if (cntlActual->blockIO[0][0].rwBuffer.buffer != NULL)
                NITF_FREE(cntlActual->blockIO[0][0].rwBuffer.buffer);
        
        /* Free buffer */
        if (!(cntlActual->blockIO[0][0].unpackedNoFree))
            if (cntlActual->blockIO[0][0].unpacked.buffer != NULL)
                NITF_FREE(cntlActual->blockIO[0][0].unpacked.buffer);
        
        /*
         * Free block buffers if allocated
         * This works because of how
         * They are allocated 
         */
        blocks = &(cntlActual->blockIO[0][0]);
        for (i = 0;i < cntlActual->nBlockIO;i++)
            if ((blocks[i].blockControl.block != NULL)
                && (blocks[i].blockControl.freeFlag))
                NITF_FREE(blocks[i].blockControl.block);
        
        nitf_ImageIO_freeBlockArray(&(cntlActual->blockIO));
    }
    
    if (cntlActual->downSampleIn != NULL)
        NITF_FREE(cntlActual->downSampleIn);

    if (cntlActual->downSampleOut != NULL)
        NITF_FREE(cntlActual->downSampleOut);
    
    if (cntlActual->bandSubset != NULL)
        NITF_FREE(cntlActual->bandSubset);
    
    if (cntlActual->padBuffer != NULL)
        NITF_FREE(cntlActual->padBuffer);
    
    if (cntlActual->columnSave != NULL)
        NITF_FREE(cntlActual->columnSave);
    
    NITF_FREE(cntlActual);
    *cntl = NULL;
    return;
}

NITFPRIV(_nitf_ImageIOWriteControl *)
nitf_ImageIOWriteControl_construct(_nitf_ImageIOControl * cntl,
                                   nitf_IOInterface* io,
                                   _nitf_ImageIO_writeMethod method,
                                   nitf_Error * error)
{
    /* The return value */
    _nitf_ImageIOWriteControl *result;
    
    result = (_nitf_ImageIOWriteControl *)
        NITF_MALLOC(sizeof(_nitf_ImageIOWriteControl));
    if (result == NULL)
    {
        nitf_Error_initf(error, NITF_CTXT, NITF_ERR_MEMORY,
                         "Error allocating object: %s", 
                         NITF_STRERROR(NITF_ERRNO));
        return NULL;
    }
    
    /* Initialize fields */
    result->cntl = cntl;
    result->method = method;
    result->nextRow = 0;
    return result;
}

NITFPRIV(void) nitf_ImageIOWriteControl_destruct(_nitf_ImageIOWriteControl
                                                 ** cntl)
{
    NITF_FREE(*cntl);
    *cntl = NULL;
    return;
}


NITFPRIV(_nitf_ImageIOReadControl *) nitf_ImageIOReadControl_construct
(_nitf_ImageIOControl * cntl, nitf_SubWindow * subWindow,
 nitf_Error * error)
{
    /* The return value */
    _nitf_ImageIOReadControl *result;
    
    result = (_nitf_ImageIOReadControl *)
        NITF_MALLOC(sizeof(_nitf_ImageIOReadControl));
    if (result == NULL)
    {
        nitf_Error_initf(error, NITF_CTXT, NITF_ERR_MEMORY,
                         "Error allocating object: %s", NITF_STRERROR(NITF_ERRNO));
        return NULL;
    }
    
    /* Initialize fields */
    result->cntl = cntl;
    
    return result;
}

NITFPRIV(void) nitf_ImageIOReadControl_destruct(_nitf_ImageIOReadControl **
        cntl)
{
    NITF_FREE(*cntl);
    *cntl = NULL;
    return;
}


NITFPRIV(int) nitf_ImageIO_bigEndian(void)
{
    nitf_Uint8 p8[2] =          /* For big-endian test */
        {
            1, 2
        };
    nitf_Uint16 *p16;           /* For big-endian test */
    
    p16 = (nitf_Uint16 *) p8;
    /* 0x102 => big-endian */
    return ((*p16 == 0x102) ? 1 : 0);
}

NITFPRIV(int) nitf_ImageIO_checkSubWindow(_nitf_ImageIO * nitf,
                                          nitf_SubWindow * subWindow,
                                          int *all, nitf_Error * error)
{
    nitf_Uint32 rowSkip;        /* Row skip factor */
    nitf_Uint32 colSkip;        /* Column skip factor */
    nitf_Uint32 bandIdx;        /* Current band index */
    nitf_Uint32 numRowsFR;      /* Number of rows at full resolution */
    nitf_Uint32 numColsFR;      /* Number of columns at full resolution */
    
    if (subWindow->downsampler != NULL)
    {
        rowSkip = subWindow->downsampler->rowSkip;
        colSkip = subWindow->downsampler->colSkip;
    }
    else
    {
        rowSkip = 1;
        colSkip = 1;
    }
    
    /* Look for down-sampling */
    
    if ((rowSkip > nitf->blockInfo.numRowsPerBlock)
        || (colSkip > nitf->blockInfo.numColsPerBlock))
    {
        nitf_Error_initf(error, NITF_CTXT, NITF_ERR_READING_FROM_FILE,
                         "Invalid pixel skips %ld %ld (limits are %ld %ld)",
                         rowSkip, colSkip,
                         nitf->blockInfo.numRowsPerBlock,
                         nitf->blockInfo.numColsPerBlock);
        return NITF_FAILURE;
    }
    
    numRowsFR = (subWindow->numRows) * rowSkip;
    numColsFR = (subWindow->numCols) * colSkip;

    /* Check sub-window size */

    if (numRowsFR > (nitf->numRows + rowSkip - 1))
    {
        nitf_Error_initf(error, NITF_CTXT, NITF_ERR_READING_FROM_FILE,
                         "Invalid number of rows %ld (Full resolution) (limit is %ld)",
                         numRowsFR, nitf->numRows);
        return NITF_FAILURE;
    }

    if (numColsFR > (nitf->numColumns + colSkip - 1))
    {
        nitf_Error_initf(error, NITF_CTXT, NITF_ERR_READING_FROM_FILE,
                         "Invalid number of columns %ld (Full resolution) (limit is %ld)",
                         numColsFR, nitf->numColumns);
        return NITF_FAILURE;
    }

    /* Check sub-window position */
    
    if (subWindow->startRow + numRowsFR > (nitf->numRows + rowSkip - 1))
    {
        nitf_Error_initf(error, NITF_CTXT, NITF_ERR_READING_FROM_FILE,
                         "Invalid start row %ld for sub-window height %ld rows,"
                         "full resolution (limit is %ld)",
                         subWindow->startRow, numRowsFR, nitf->numRows - 1);
        return NITF_FAILURE;
    }

    if (subWindow->startCol + numColsFR > (nitf->numColumns + colSkip - 1))
    {
        nitf_Error_initf(error, NITF_CTXT, NITF_ERR_READING_FROM_FILE,
                         "Invalid start column %ld for sub-window width %ld columns,"
                         "full resolution (limit is %ld)",
                         subWindow->startCol, numColsFR,
                         nitf->numColumns - 1);
        return NITF_FAILURE;
    }

    /* Check requested bands */

    if (subWindow->numBands > nitf->numBands)
    {
        nitf_Error_initf(error, NITF_CTXT, NITF_ERR_READING_FROM_FILE,
                         "Too many bands %ld (limit is %ld)\n",
                         subWindow->numBands, nitf->numBands);
        return NITF_FAILURE;
    }

    if (subWindow->numBands <= 0)
    {
        nitf_Error_initf(error, NITF_CTXT, NITF_ERR_READING_FROM_FILE,
                         "Invalid bands count %ld\n", subWindow->numBands,
                         nitf->numBands);
        return NITF_FAILURE;
    }
    
    for (bandIdx = 0; bandIdx < subWindow->numBands; bandIdx++)
    {
        if (subWindow->bandList[bandIdx] >= nitf->numBands)
        {
            nitf_Error_initf(error, NITF_CTXT, NITF_ERR_READING_FROM_FILE,
                             "Invalid band %ld (limit is %ld)\n",
                             subWindow->bandList[bandIdx],
                             nitf->numBands - 1);
            return NITF_FAILURE;
        }
    }

    /* Check for full image request */
    
    *all = 0;
    if ((numRowsFR == nitf->numRowsActual)
        && (numColsFR == nitf->numColumnsActual))
        *all = 1;
    
    return NITF_SUCCESS;
}


NITFPRIV(NITF_BOOL) nitf_ImageIO_checkOneRead(_nitf_ImageIO * nitfI,
                                              NITF_BOOL all)
{
    NITF_BOOL oneReadA;  /* Complete request in one read flag, partial logic */
    NITF_BOOL oneReadB;  /* Complete request in one read flag, partial logic */
    NITF_BOOL oneReadC;  /* Complete request in one read flag, partial logic */
    NITF_BOOL oneReadRGB;
    NITF_BOOL oneReadIQ;
    NITF_BOOL oneRead;   /* The result */
    
    /*
     * Look for the special case where pixel type is "B" (binary). This can
     * look like a one read case, but is actually handled as a 
     * type of compression
     */
    
    if (nitfI->pixel.type == NITF_IMAGE_IO_PIXEL_TYPE_B)
        return 0;
    
    /*
     *  Look for the "single read" cases.
     *
     *  If A:
     *       The image has only one block
     *       The full image is being read
     *       The blocking mode is "B"
     *       No compression (for now)
     */
    
    oneReadA = ((nitfI->nBlocksPerRow * nitfI->nBlocksPerColumn) == 1)
        && all
        && (nitfI->blockingMode & NITF_IMAGE_IO_BLOCKING_MODE_S)
        && (nitfI->compression
            & (NITF_IMAGE_IO_COMPRESSION_NC |
               NITF_IMAGE_IO_COMPRESSION_NM));
    /*
     *  Or B:
     *       The image has only one block per column
     *       The full image is being read
     *       The blocking mode is "S"
     *       No compression (for now)
     */
    oneReadB = (nitfI->nBlocksPerColumn == 1)
        && all
        && (nitfI->blockingMode & NITF_IMAGE_IO_BLOCKING_MODE_S)
        && (nitfI->compression
            & (NITF_IMAGE_IO_COMPRESSION_NC |
               NITF_IMAGE_IO_COMPRESSION_NM));
    /*
     *  Or C:
     *       The image has only one band
     *       The image has only one block per column
     *       The full image is being read
     *       The blocking mode is "B"
     *       No compression (for now)
     */
    oneReadC = (nitfI->numBands == 1)
        && (nitfI->nBlocksPerColumn == 1)
        && all
        && (nitfI->blockingMode & NITF_IMAGE_IO_BLOCKING_MODE_B)
        && (nitfI->compression
            & (NITF_IMAGE_IO_COMPRESSION_NC |
               NITF_IMAGE_IO_COMPRESSION_NM));
    
    oneReadRGB = (nitfI->blockingMode == NITF_IMAGE_IO_BLOCKING_MODE_RGB24)
        && (nitfI->nBlocksPerColumn == 1) && all;
    
    oneReadIQ = (nitfI->blockingMode == NITF_IMAGE_IO_BLOCKING_MODE_IQ)
        && (nitfI->nBlocksPerColumn == 1) && all;
    
    
    /* Actually, its one read per band */
    oneRead = oneReadA || oneReadB || oneReadC || oneReadRGB || oneReadIQ;

    return oneRead;
}


NITFPRIV(int) nitf_ImageIO_mkMasks(nitf_ImageIO * img,
                                   nitf_IOInterface* io, int reading,
                                   nitf_Error * error)
{
    _nitf_ImageIO *nitf;        /* Parent _nitf_ImageIO structure */
    nitf_Uint32 nBlocksTotal;   /* Total number of blocks */
    nitf_Uint32 maskSizeMemory; /* Block mask size in bytes in memory */
    nitf_Uint32 maskSizeFile;   /* Block mask size in bytes in the file */
    nitf_Uint64 maskOffset;     /* Current mask offset */
    nitf_Uint64 *maskp;         /* Current mask entry */
    size_t bytesPerBlock;       /* Total bytes in one block */
    nitf_Uint32 headerOffset;   /* File offset of masks due to mask header */
    nitf_Uint32 padOffset;      /* File offset of pad mask due to block mask */
    nitf_Uint32 i;
    
    nitf = (_nitf_ImageIO *) img;
    
    nBlocksTotal = nitf->nBlocksTotal;
    bytesPerBlock = nitf->blockSize; /* Adjust for B and 12 bit pixels */

    if(nitf->pixel.type == NITF_IMAGE_IO_PIXEL_TYPE_B)
    {
        bytesPerBlock = (nitf->blockSize + 7) / 8;
    }

    if(nitf->pixel.type == NITF_IMAGE_IO_PIXEL_TYPE_12)
    {
        bytesPerBlock = 3*((nitf->blockSize/2)/nitf->pixel.bytes);
        if((nitf->blockSize/nitf->pixel.bytes) & 1) /* Odd number of pixels */
            bytesPerBlock += 2;
    }

    /* Initialize header */

    if (!nitf_ImageIO_initMaskHeader
            (nitf, io, nBlocksTotal, reading, error))
        return NITF_FAILURE;

    /* Adjust image pixel offset */

    nitf->pixelBase += nitf->maskHeader.imageDataOffset;

    /* Allocate Block mask */

    maskSizeFile = nBlocksTotal * sizeof(nitf_Uint32);
    maskSizeMemory = (nBlocksTotal + 1) * sizeof(nitf_Uint64);
    nitf->blockMask = (nitf_Uint64 *) NITF_MALLOC(maskSizeMemory);

    if (nitf->blockMask == NULL)
    {
        nitf_Error_initf(error, NITF_CTXT, NITF_ERR_MEMORY,
                         "Memory allocation error: %s", NITF_STRERROR(NITF_ERRNO));
        return NITF_FAILURE;
    }
    
    if ((nitf->maskHeader.blockRecordLength == 0) || !reading)
    {                           /* No mask */
        headerOffset = 0;
        maskOffset = 0;
        
        maskp = nitf->blockMask;
        for (i = 0; i < nBlocksTotal + 1; i++)
        {
            *(maskp++) = maskOffset;
            maskOffset += bytesPerBlock;
        }
        padOffset = 0;
    }
    else
    {
        /*
         * Because the in memory offset is 64-bit and the file is 32-bit
         * you must allocate a 32-bit array for read and copy after
         * byte-swapping
         */
        nitf_Uint32 *fileMask;   /* Buffer to hold file mask */
        nitf_Uint32 i;
        
        fileMask = (nitf_Uint32 *) NITF_MALLOC(maskSizeFile);
        if (fileMask == NULL)
        {
            nitf_Error_initf(error, NITF_CTXT, NITF_ERR_MEMORY,
                             "Memory allocation error: %s", 
                             NITF_STRERROR(NITF_ERRNO));
            return NITF_FAILURE;
        }
        
        headerOffset = NITF_IMAGE_IO_MASK_HEADER_LEN
            + nitf->maskHeader.padPixelValueLength;
        
        if (!nitf_ImageIO_readFromFile(io,
                                       nitf->imageBase + headerOffset,
                                       (nitf_Uint8 *) fileMask, 
                                       maskSizeFile, error))
        {
            NITF_FREE(fileMask);
            return NITF_FAILURE;
        }
        
        /* The offsets are stored as big-endian binary */
        if (!nitf_ImageIO_bigEndian())
            nitf_ImageIO_swapOnly_4((nitf_Uint8 *) fileMask, nBlocksTotal, 0);
        
        for (i = 0;i < nBlocksTotal;i++)
            nitf->blockMask[i] = fileMask[i];
        
        nitf->blockMask[nBlocksTotal] =
            nitf->blockMask[nBlocksTotal - 1] + bytesPerBlock;
        
        padOffset = maskSizeFile;
        NITF_FREE(fileMask);
    }

    /* Allocate pad pixel mask */
    if (nitf->padMask != NULL)  /* Should not happen */
        return NITF_SUCCESS;

    nitf->padMask = (nitf_Uint64 *) NITF_MALLOC(maskSizeMemory);
    if (nitf->padMask == NULL)
    {
        nitf_Error_initf(error, NITF_CTXT, NITF_ERR_MEMORY,
                         "Memory allocation error: %s", 
                         NITF_STRERROR(NITF_ERRNO));
        return NITF_FAILURE;
    }

    /* 
     * Create masks with all blocks marked as unpadded if either there
     * is not pad mask on read or this is a write operstion
     */

    if ((nitf->maskHeader.padRecordLength == 0) || !reading)
    {                           /* No mask */
        for (i = 0;i < nBlocksTotal;i++)
            nitf->padMask[i] = NITF_IMAGE_IO_NO_BLOCK;
    }
    else
    {                           /* Read mask in file */
        /*
         * Because the in memory offset is 64-bit and the file is 32-bit
         * you must allocate a 32-bit array for read and copy after
         * byte-swapping
         */
        nitf_Uint32 *fileMask;   /* Buffer to hold file mask */
        nitf_Uint32 i;

        fileMask = (nitf_Uint32 *) NITF_MALLOC(maskSizeFile);
        if (fileMask == NULL)
        {
            nitf_Error_initf(error, NITF_CTXT, NITF_ERR_MEMORY,
                             "Memory allocation error: %s",
                             NITF_STRERROR(NITF_ERRNO));
            return NITF_FAILURE;
        }

        if (!nitf_ImageIO_readFromFile(io,
                                       nitf->imageBase + headerOffset +
                                       padOffset,
                                       (nitf_Uint8 *) fileMask,
                                       maskSizeFile, error))
        {
            NITF_FREE(fileMask);
            return NITF_FAILURE;
        }

        /* The offsets are stored as big-endian binary */
        if (nitf_ImageIO_bigEndian())
            nitf_ImageIO_swapOnly_4((nitf_Uint8 *) fileMask, nBlocksTotal, 0);
        
        for (i = 0; i < nBlocksTotal;i++)
            nitf->padMask[i] = fileMask[i];
        
        NITF_FREE(fileMask);
    }

    return NITF_SUCCESS;
}

/*========================= Pad Pixel Scan Functions ========================*/

NITF_IMAGE_IO_PAD_SCANNER(_nitf_Image_IO_pad_scan_1, nitf_Uint8)
NITF_IMAGE_IO_PAD_SCANNER(_nitf_Image_IO_pad_scan_2, nitf_Uint16)
NITF_IMAGE_IO_PAD_SCANNER(_nitf_Image_IO_pad_scan_4, nitf_Uint32)
NITF_IMAGE_IO_PAD_SCANNER(_nitf_Image_IO_pad_scan_8, nitf_Uint64)

NITFPRIV(int) nitf_ImageIO_initMaskHeader(_nitf_ImageIO * nitf,
                                          nitf_IOInterface* io,
                                          nitf_Uint32 blockCount,
                                          int reading, 
                                          nitf_Error * error)
{
    /* The structure to initialize */
    _nitf_ImageIO_MaskHeader *maskHeader;

    /* Holds mask header (except pad value) */    
    maskHeader = &(nitf->maskHeader);
    
    if (maskHeader->ready)
        return NITF_SUCCESS;

    if ((nitf->compression &    /* Look for no mask */
            (NITF_IMAGE_IO_COMPRESSION_NM
             | NITF_IMAGE_IO_COMPRESSION_M1
             | NITF_IMAGE_IO_COMPRESSION_M3
             | NITF_IMAGE_IO_COMPRESSION_M4
             | NITF_IMAGE_IO_COMPRESSION_M5
             | NITF_IMAGE_IO_COMPRESSION_M8)) == 0 /* No masks */ )
    {
        maskHeader->ready = 1;
        return NITF_SUCCESS;
    }

    /* Create header if not reading */

    if (!reading)
    {
        /* Check for masked type with S blocks (not supported) */
        if (nitf->blockingMode == NITF_IMAGE_IO_BLOCKING_MODE_S)
        {
            nitf_Error_init(error,
                            "Masked image with S mode blocking is not supported",
                            NITF_CTXT, NITF_ERR_INVALID_PARAMETER);
            return NITF_FAILURE;
        }
        maskHeader->blockRecordLength = 4;
        
        /*
         * Check for margin padding and set pad pixel length 
         * accordingly (Margin padding is required when image 
         * dimensions are not integral multiples of
         * block dimensions).
         */
        maskHeader->padRecordLength = 4;
        maskHeader->padPixelValueLength = nitf->pixel.bytes;

        /*
         * The image data offset is the base header length plus the pad pixel
         * value (if present) plus the lengths of the masks (if present). 
         * Since this is only called for a write with a masked compression
         * type (i.e., "NM") the base header is always written.
         */

        maskHeader->imageDataOffset =
            NITF_IMAGE_IO_MASK_HEADER_LEN +
            + maskHeader->padPixelValueLength;
        
        if (maskHeader->blockRecordLength != 0)
            maskHeader->imageDataOffset +=
                blockCount * sizeof(nitf_Uint32);
        
        if (maskHeader->padRecordLength != 0)
            maskHeader->imageDataOffset +=
                blockCount * sizeof(nitf_Uint32);
        
        /* Set the pad scanner function based on the pad pixel length */
        switch (maskHeader->padPixelValueLength)
        {
        case 1:
            nitf->padScanner = _nitf_Image_IO_pad_scan_1;
            break;
        case 2:
            nitf->padScanner = _nitf_Image_IO_pad_scan_2;
            break;
        case 4:
            nitf->padScanner = _nitf_Image_IO_pad_scan_4;
            break;
        case 8:
            nitf->padScanner = _nitf_Image_IO_pad_scan_8;
            break;
        default:
            nitf->padScanner = NULL;
            break;
        }
        
        maskHeader->ready = 1;
        return NITF_SUCCESS;
    }
    
    /* Read header from file */
    if (!nitf_ImageIO_readMaskHeader(nitf, io, error))
        return NITF_FAILURE;

    maskHeader->ready = 1;
    return NITF_SUCCESS;
}


NITFPRIV(int) nitf_ImageIO_readMaskHeader(_nitf_ImageIO * nitf,
                                          nitf_IOInterface* io,
                                          nitf_Error * error)
{
    /* The structure to initialize */
    _nitf_ImageIO_MaskHeader *maskHeader;
    nitf_Uint8 buffer[NITF_IMAGE_IO_MASK_HEADER_LEN];
    nitf_Uint8 *bp;             /* Points into buffer */

    maskHeader = &(nitf->maskHeader);

    /*      Read header and load it into the mask header structure */

    if (!nitf_ImageIO_readFromFile(io, nitf->imageBase,
                                   buffer, NITF_IMAGE_IO_MASK_HEADER_LEN,
                                   error))
        return NITF_FAILURE;

    bp = buffer;
    memcpy(&(maskHeader->imageDataOffset), bp, sizeof(nitf_Uint32));
    bp += 4;
    memcpy(&(maskHeader->blockRecordLength), bp, sizeof(nitf_Uint16));
    bp += 2;
    memcpy(&(maskHeader->padRecordLength), bp, sizeof(nitf_Uint16));
    bp += 2;
    memcpy(&(maskHeader->padPixelValueLength), bp, sizeof(nitf_Uint16));

    /*      Byte swap structure  if needed */

    if (!nitf_ImageIO_bigEndian())
        nitf_ImageIO_swapMaskHeader(maskHeader);

    /*
       The pad pixel value length is in bits and the structure field is in
       bytes. Unless the field is 0 substitute the number of bytes per pixel in
       the structure. NOTE: maskHeader->padPixelValueLength == 0 mean zero bits
       in the pad pixel which means no pad value.
     */

    if (maskHeader->padPixelValueLength != 0)
        maskHeader->padPixelValueLength = nitf->pixel.bytes;

    /*      Read pad pixel value */

    if (maskHeader->padPixelValueLength != 0)
    {
        if (!nitf_ImageIO_readFromFile(io,
                                       nitf->imageBase +
                                       NITF_IMAGE_IO_MASK_HEADER_LEN,
                                       nitf->pixel.pad, nitf->pixel.bytes,
                                       error))
        {
            return NITF_FAILURE;
        }
    }

    return NITF_SUCCESS;
}


NITFPRIV(int) nitf_ImageIO_writeMasks(_nitf_ImageIO * nitf,
                                      nitf_IOInterface* io,
                                      nitf_Error * error)
{
    /* Copy of the structure to write */
    _nitf_ImageIO_MaskHeader maskHeader;
    nitf_Uint8 buffer[NITF_IMAGE_IO_MASK_HEADER_LEN];
    nitf_Uint16 padCodeLength;  /* Pad code length for header */
    nitf_Uint64 maskOffset;     /* Offset of block or pad mask */
    nitf_Uint32 maskSizeFile;   /* Block mask size in bytes in the file */

    /* Do not write anything if the IC is not a mask type */

    if ((nitf->compression &
            (NITF_IMAGE_IO_COMPRESSION_NM
             | NITF_IMAGE_IO_COMPRESSION_M1
             | NITF_IMAGE_IO_COMPRESSION_M3
             | NITF_IMAGE_IO_COMPRESSION_M4
             | NITF_IMAGE_IO_COMPRESSION_M5
             | NITF_IMAGE_IO_COMPRESSION_M8)) == 0 /* No masks */ )
        return NITF_SUCCESS;

    /* A copy of the structure is made so it can be byte swapped if needed */

    maskHeader = nitf->maskHeader;
    if (!nitf_ImageIO_bigEndian())
        nitf_ImageIO_swapMaskHeader(&maskHeader);

    /* Format and write the header buffer */

    buffer[0] = ((nitf_Uint8 *) & maskHeader.imageDataOffset)[0];
    buffer[1] = ((nitf_Uint8 *) & maskHeader.imageDataOffset)[1];
    buffer[2] = ((nitf_Uint8 *) & maskHeader.imageDataOffset)[2];
    buffer[3] = ((nitf_Uint8 *) & maskHeader.imageDataOffset)[3];

    buffer[4] = ((nitf_Uint8 *) & maskHeader.blockRecordLength)[0];
    buffer[5] = ((nitf_Uint8 *) & maskHeader.blockRecordLength)[1];
    buffer[6] = ((nitf_Uint8 *) & maskHeader.padRecordLength)[0];
    buffer[7] = ((nitf_Uint8 *) & maskHeader.padRecordLength)[1];

    /*
       Write pad output pixel code length actually NBPP, M3 is a special case
       fix later XXX
     */

    if (nitf->maskHeader.padPixelValueLength != 0)
        padCodeLength = 8 * (nitf->pixel.bytes);
    else
        padCodeLength = 0;

    if (nitf_ImageIO_bigEndian())
    {
        ((nitf_Uint8 *) buffer)[8] = padCodeLength >> 8;
        ((nitf_Uint8 *) buffer)[9] = padCodeLength & 0xff;
    }
    else
    {
        ((nitf_Uint8 *) buffer)[9] = padCodeLength >> 8;
        ((nitf_Uint8 *) buffer)[8] = padCodeLength & 0xff;
    }

    if (!nitf_ImageIO_writeToFile(io, nitf->imageBase,
                                  buffer, NITF_IMAGE_IO_MASK_HEADER_LEN,
                                  error))
        return NITF_FAILURE;

    /* Write pad pixel value */

    maskOffset = nitf->imageBase + NITF_IMAGE_IO_MASK_HEADER_LEN;
    if (maskHeader.padPixelValueLength != 0)
    {
        if (!nitf_ImageIO_writeToFile(io, maskOffset,
                                      nitf->pixel.pad,
                                      (size_t)nitf->maskHeader.padPixelValueLength,
                                      error))
        {
            return NITF_FAILURE;
        }
        maskOffset += nitf->maskHeader.padPixelValueLength;
    }

    /*
       Write the block mask. The offsets are stored as big-endian binary so
       swap them on little endian machines. swap in place and swap back after write
     */

    if (nitf->maskHeader.blockRecordLength != 0)
    {
        /*
              Because the in memory offset is 64-bit and the file is 32-bit
              you must allocate a 32-bit array and byte-swap after copying
        */
        nitf_Uint32 *fileMask;   /* Buffer to hold file mask */
        nitf_Uint32 i;

        maskSizeFile = nitf->nBlocksTotal * sizeof(nitf_Uint32);
        fileMask = (nitf_Uint32 *) NITF_MALLOC(maskSizeFile);
        if (fileMask == NULL)
        {
            nitf_Error_initf(error, NITF_CTXT, NITF_ERR_MEMORY,
                             "Memory allocation error: %s", NITF_STRERROR(NITF_ERRNO));
            return NITF_FAILURE;
        }

        for (i = 0;i < nitf->nBlocksTotal;i++)
            fileMask[i] = nitf->blockMask[i];

        for (i = 0;i < nitf->nBlocksTotal;i++)   /* Overflow check */
            if (fileMask[i] != nitf->blockMask[i])
            {
                NITF_FREE(fileMask);
                nitf_Error_initf(error, NITF_CTXT,
                                 NITF_ERR_INVALID_PARAMETER, "Mask index overflow");
                return NITF_FAILURE;
            }

        if (!nitf_ImageIO_bigEndian())
            nitf_ImageIO_swapOnly_4((nitf_Uint8 *) fileMask,
                                    nitf->nBlocksTotal, 0);

        if (!nitf_ImageIO_writeToFile(io, maskOffset,
                                      (nitf_Uint8 *) fileMask,
                                      (size_t)nitf->nBlocksTotal *
                                      sizeof(nitf_Uint32), error))
        {
            NITF_FREE(fileMask);
            return NITF_FAILURE;
        }

        maskOffset += nitf->nBlocksTotal * sizeof(nitf_Uint32);
        NITF_FREE(fileMask);
    }
    /*
       Write the pad mask.
     */

    if (nitf->maskHeader.padRecordLength != 0)
    {
        /*
          Because the in memory offset is 64-bit and the file is 32-bit
          you must allocate a 32-bit array and byte-swap after copying
        */
        nitf_Uint32 *fileMask;   /* Buffer to hold file mask */
        nitf_Uint32 i;

        maskSizeFile = nitf->nBlocksTotal * sizeof(nitf_Uint32);
        fileMask = (nitf_Uint32 *) NITF_MALLOC(maskSizeFile);
        if (fileMask == NULL)
        {
            nitf_Error_initf(error, NITF_CTXT, NITF_ERR_MEMORY,
                             "Memory allocation error: %s", NITF_STRERROR(NITF_ERRNO));
            return NITF_FAILURE;
        }

        for (i = 0;i < nitf->nBlocksTotal;i++)
            fileMask[i] = nitf->padMask[i];

        for (i = 0;i < nitf->nBlocksTotal;i++)   /* Overflow check */
            if (fileMask[i] != nitf->padMask[i])
            {
                NITF_FREE(fileMask);
                nitf_Error_initf(error, NITF_CTXT,
                                 NITF_ERR_INVALID_PARAMETER, "Mask index overflow");
                return NITF_FAILURE;
            }

        if (!nitf_ImageIO_bigEndian())
            nitf_ImageIO_swapOnly_4((nitf_Uint8 *) fileMask,
                                    nitf->nBlocksTotal, 0);

        if (!nitf_ImageIO_writeToFile(io, maskOffset,
                                      (nitf_Uint8 *) fileMask,
                                      (size_t)nitf->nBlocksTotal *
                                      sizeof(nitf_Uint32), error))
        {
            NITF_FREE(fileMask);
            return NITF_FAILURE;
        }

        NITF_FREE(fileMask);
        maskOffset += nitf->nBlocksTotal * sizeof(nitf_Uint32);
    }
    return NITF_SUCCESS;
}


NITFPRIV(void) nitf_ImageIO_swapMaskHeader(_nitf_ImageIO_MaskHeader *
        header)
{
    nitf_Uint8 *hp;             /* Points into header */
    nitf_Uint8 tmp;             /* Temp value for byte swaps */

    hp = (nitf_Uint8 *) & (header->imageDataOffset);
    tmp = hp[0];
    hp[0] = hp[3];
    hp[3] = tmp;
    tmp = hp[1];
    hp[1] = hp[2];
    hp[2] = tmp;

    hp = (nitf_Uint8 *) & (header->blockRecordLength);
    tmp = hp[0];
    hp[0] = hp[1];
    hp[1] = tmp;

    hp = (nitf_Uint8 *) & (header->padRecordLength);
    tmp = hp[0];
    hp[0] = hp[1];
    hp[1] = tmp;

    hp = (nitf_Uint8 *) & (header->padPixelValueLength);
    tmp = hp[0];
    hp[0] = hp[1];
    hp[1] = tmp;

    return;
}


NITFPRIV(int) nitf_ImageIO_oneRead(_nitf_ImageIOControl * cntl,
                                   nitf_IOInterface* io,
                                   nitf_Error * error)
{
    _nitf_ImageIO *nitf; /* Parent _nitf_ImageIO object */
    _nitf_ImageIOBlock *blockIO; /* The current  block IO structure */
    size_t pixelCount; /* Total pixel count (size of one band in pixels) */
    size_t count;      /* Total read count (size of one band in bytes) */

    nitf = cntl->nitf;
    blockIO = &(cntl->blockIO[0][0]);
    pixelCount = (size_t)nitf->numRowsActual * (size_t)nitf->numColumnsActual;
    count = pixelCount * nitf->pixel.bytes;

    if (!nitf_ImageIO_readFromFile(io,
                                   blockIO->cntl->nitf->pixelBase +
                                   blockIO->blockOffset.orig,
                                   blockIO->user.buffer + 
                                   blockIO->user.offset.mark,
                                   count,error))
        return NITF_FAILURE;

    if (nitf->vtbl.unformat != NULL)
        (*(nitf->vtbl.unformat)) (blockIO->user.buffer
                       + blockIO->user.offset.mark,
                                  pixelCount, nitf->pixel.shift);

    return NITF_SUCCESS;
}


/* This function is used when FR == DR (no down-Sampling) */

NITFPRIV(int) nitf_ImageIO_readRequest(_nitf_ImageIOControl * cntl,
                                       nitf_IOInterface* io,
                                       nitf_Error * error)
{
    _nitf_ImageIO *nitf;       /* Parent _nitf_ImageIO object */
    nitf_Uint32 nBlockCols;    /* Number of block columns */
    nitf_Uint32 numRows;       /* Number of rows in the requested sub-window */
    nitf_Uint32 numBands;      /* Number of bands */
    nitf_Uint32 col;           /* Block column index */
    nitf_Uint32 row;           /* Current row in sub-window */
    nitf_Uint32 band;          /* Current band in sub-window */
    _nitf_ImageIOBlock *blockIO; /* The current  block IO structure */

    nitf = cntl->nitf;
    numRows = cntl->numRows;
    numBands = cntl->numBandSubset;
    nBlockCols = cntl->nBlockIO / numBands;

    for (col = 0; col < nBlockCols; col++)
    {
        for (row = 0; row < numRows; row++)
        {
            for (band = 0; band < numBands; band++)
            {
                blockIO = &(cntl->blockIO[col][band]);
                if (blockIO->doIO)
                    if (!(*(nitf->vtbl.reader)) (blockIO, io, error))
                        return NITF_FAILURE;
                
                if (nitf->vtbl.unpack != NULL)
                    (*(nitf->vtbl.unpack)) (blockIO, error);
                
                if (nitf->vtbl.unformat != NULL)
                    (*(nitf->vtbl.unformat)) (blockIO->user.buffer +
                                              blockIO->user.offset.mark,
                                              blockIO->pixelCountDR,
                                              nitf->pixel.shift);
                /*
                 * You have to check for last row and not call 
                 * nitf_ImageIO_nextRow because if the last row is the
                 * last line in the last block, you can
                 * wind up accessing past the end of the block mask 
                 * while setting-up for
                 * the non-existant next block.
                 */
                if (row != numRows - 1)
                    nitf_ImageIO_nextRow(blockIO, 0);

                if (blockIO->rowsUntil == 0)
                    /* 
                     * See documentation of nitf_ImageIOBlock  for an 
                     * explaination of the - 1 
                     */
                    blockIO->rowsUntil = nitf->numRowsPerBlock - 1;
                else
                    blockIO->rowsUntil -= 1;
            }
        }
    }

    return 1;
}

/* This function is used when FR != DR (down-Sampling) */
NITFPRIV(int) nitf_ImageIO_readRequestDownSample(_nitf_ImageIOControl *
                                                 cntl,
                                                 nitf_SubWindow *
                                                 subWindow,
                                                 nitf_IOInterface* io,
                                                 nitf_Error * error)
{
    _nitf_ImageIO *nitf; /* Parent _nitf_ImageIO object */
    nitf_Uint32 nBlockCols;   /* Number of block columns */
    nitf_Uint32 numRowsFull; /* Number of rows to read in full res sub-window */
    nitf_Uint32 numBands;     /* Number of bands */
    nitf_Uint32 col;          /* Block column index */
    nitf_Uint32 row;          /* Current row in sub-window */
    nitf_Uint32 band;         /* Current band in sub-window */
    _nitf_ImageIOBlock *blockIO; /* The current  block IO structure */
    nitf_Uint8 *columnSave;   /* Column save buffer ptr,current block column */
    NITF_BOOL noUserInc;      /* Do not increment user buffer pointer if TRUE */
    nitf_Uint32 bytes;        /* Pixel size in bytes */
    nitf_Uint32 rowSkipCount; /* Number of rows since the last row skip */
    nitf_Uint32 numColsDR;    /* Number of columns, down-sample resolution */

    nitf = cntl->nitf;

    /*
     * Calculate the number of rows to read (full resolution). Check for the
     * full resolution size to go outside of the image (This can happen if
     * down-sampling and the final row of neighbords is partially out of the
     * image
     */
    
    numRowsFull = (cntl->numRows) * (cntl->rowSkip);
    if (cntl->row + numRowsFull > nitf->numRows)
        numRowsFull = nitf->numRows - cntl->row;
    
    numBands = cntl->numBandSubset;
    nBlockCols = cntl->nBlockIO / numBands;
    bytes = nitf->pixel.bytes;
    noUserInc = 1; /* Only increment after calling down-sample plugin */
    rowSkipCount = 0;

    blockIO = NULL; /* Avoid spurious uninitialized warning */
    numColsDR = 0;  /* Avoid spurious uninitialized warning */
    for (col = 0; col < nBlockCols; col++)
    {
        columnSave = cntl->columnSave;
        rowSkipCount = 0;
        for (row = 0; row < numRowsFull; row++)
        {
            for (band = 0; band < numBands; band++)
            {
                blockIO = &(cntl->blockIO[col][band]);
                numColsDR = blockIO->pixelCountDR;
                
                if (blockIO->doIO)
                    if (!(*(nitf->vtbl.reader)) (blockIO, io, error))
                        return NITF_FAILURE;
                
                if (nitf->vtbl.unpack != NULL)
                    (*(nitf->vtbl.unpack)) (blockIO, error);
                
                /*
                 * Copy first neighborhood data from previous block 
                 * column and save
                 * residual data from this column
                 */
                
                if (blockIO->residual != 0)
                    memmove(blockIO->unpacked.buffer
                            + blockIO->unpacked.offset.mark -
                            (blockIO->residual) * bytes, columnSave,
                            (blockIO->residual) * bytes);
                
                if (blockIO->myResidual != 0)
                {
                    /* Normal case (not last block) */
                    if (col != (nBlockCols - 1))
                    {
                        memmove(columnSave,
                                blockIO->unpacked.buffer
                                + blockIO->unpacked.offset.mark +
                                (blockIO->pixelCountFR -
                                 blockIO->myResidual) * bytes,
                                (blockIO->myResidual) * bytes);
                    }
                    else        /* Off the side of the image, supply pad */
                    {
                        if (cntl->padBuffer == NULL)
                            if (!nitf_ImageIO_allocatePad(cntl, error))
                                return NITF_FAILURE;
                        
                        memmove(blockIO->unpacked.buffer
                                + blockIO->unpacked.offset.mark
                                + blockIO->pixelCountFR * bytes,
                                cntl->padBuffer,
                                (blockIO->myResidual) * bytes);
                    }
                }
                
                /* Update columnSave, array is organized like [row][band] */

                columnSave += (cntl->columnSkip) * bytes;
                
                if (nitf->vtbl.unformat != NULL)
                    (*(nitf->vtbl.unformat)) (blockIO->unpacked.buffer +
                                              (rowSkipCount) *
                                              (nitf->numColumnsPerBlock +
                                               cntl->columnSkip) * bytes,
                                              blockIO->formatCount,
                                              nitf->pixel.shift);
                
                /*
                 * Setup down-sample plugin buffer arguments 
                 * if there is a complete sample
                 * window of rows.
                 */
                
                if (rowSkipCount == (cntl->rowSkip - 1))
                {
                    blockIO->rwBuffer.offset.mark =
                        blockIO->rwBuffer.offset.orig - cntl->bufferInc;
                    blockIO->unpacked.offset.mark =
                        blockIO->unpacked.offset.orig - cntl->unpackedInc;
                    cntl->downSampleIn[band] =
                        (NITF_DATA *) (blockIO->unpacked.buffer);
                    cntl->downSampleOut[band] =
                        (NITF_DATA *) (blockIO->user.buffer
                                       + blockIO->user.offset.mark);
                    noUserInc = 0; /* Increment user buffer this row */
                }
                
                /*
                 * You have to check for last row and not 
                 * call nitf_ImageIO_nextRow
                 * because if the last row is the last line in 
                 * the last block, you can
                 * wind up accessing past the end of the block mask
                 * while setting-up for
                 * the non-existant next block.
                 */
                if (row != numRowsFull - 1)
                    nitf_ImageIO_nextRow(blockIO, noUserInc);
                noUserInc = 1;
                
                if (blockIO->rowsUntil == 0)
                    /* 
                     * See documentation of nitf_ImageIOBlock  
                     * for an explaination of the - 1 
                     */
                    blockIO->rowsUntil = nitf->numRowsPerBlock - 1;
                else
                    blockIO->rowsUntil -= 1;
            }
            
            if (rowSkipCount == (cntl->rowSkip - 1))
            {
                /* For down-sampler call */
                nitf_Uint32 colsInLastWindow;
                
                /* Partial neighborhoods happen in last block */
                if (col == (nBlockCols - 1))
                    colsInLastWindow =
                        cntl->columnSkip - blockIO->myResidual;
                else
                    colsInLastWindow = cntl->columnSkip;
                
                /* Always one row of windows */
                if (!nitf_DownSampler_apply
                    (subWindow->downsampler, cntl->downSampleIn,
                     cntl->downSampleOut, cntl->numBandSubset, 1, numColsDR,
                     nitf->numColumnsPerBlock + cntl->columnSkip,
                     subWindow->numCols, nitf->pixel.type,
                     nitf->pixel.bytes, cntl->rowSkip, colsInLastWindow,
                     error))
                    return NITF_FAILURE;
            }
            
            rowSkipCount += 1;
            if (rowSkipCount >= cntl->rowSkip)
                rowSkipCount = 0;
        }

        /*
         * Check for a partial sampling neighborhood, if the row skip count is
         * not exactly zero. This count is set to zero after a window of rows is
         * passed to the down-sample plugin. If it is not zero then the
         * read buffer will contain the skip count number of valid rows. 
         * Fill the missing rows with
         * pad and call the down-sample plugin.
         */

        if (rowSkipCount != 0)
        {
            nitf_Uint32 padRows; /* Number of pad rows */
            /* For down-sampler call */
            nitf_Uint32 colsInLastWindow;
            nitf_Uint32 i;
            
            if (cntl->padBuffer == NULL)
                if (!nitf_ImageIO_allocatePad(cntl, error))
                    return NITF_FAILURE;
            /* Eliminates suprious uninitialized variable warning */
            colsInLastWindow = 0;
            padRows = cntl->rowSkip - rowSkipCount;
            /* Back-up blockIO pointer */
            blockIO = blockIO - numBands + 1;
            for (band = 0; band < numBands; band++)
            {
                for (i = 0; i < padRows; i++)
                {
                    memmove(blockIO->unpacked.buffer +
                            (cntl->unpackedInc) * (rowSkipCount + i),
                            cntl->padBuffer, cntl->unpackedInc);
                }
                /* Partial neighborhoods happen in last block */
                if (col == (nBlockCols - 1))
                    colsInLastWindow =
                        cntl->columnSkip - blockIO->myResidual;
                else
                    colsInLastWindow = cntl->columnSkip;
                
                /* 
                 * The iputput pointer needs to be updated. 
                 * This pointer is the user
                 * buffer plus offset. The offset is updated in the 
                 * nextRow function, but
                 * only at the end of a complete sample neighborhood of rows
                 */
                cntl->downSampleIn[band] =
                    (NITF_DATA *) (blockIO->unpacked.buffer);
                cntl->downSampleOut[band] =
                    (NITF_DATA *) (blockIO->user.buffer
                                   + blockIO->user.offset.mark);
                blockIO += 1;
            }
            
            /* Always one row of windows */
            if (!nitf_DownSampler_apply
                (subWindow->downsampler, cntl->downSampleIn,
                 cntl->downSampleOut, cntl->numBandSubset, 1, numColsDR,
                 nitf->numColumnsPerBlock + cntl->columnSkip,
                 subWindow->numCols, nitf->pixel.type, nitf->pixel.bytes,
                 rowSkipCount, colsInLastWindow, error))
                return NITF_FAILURE;
        }
    }

    return NITF_SUCCESS;
}

NITFPRIV(int) nitf_ImageIO_allocatePad(_nitf_ImageIOControl * cntl,
                                       nitf_Error * error)
{
    _nitf_ImageIO *nitf;        /* NITF block structure */
    nitf_Uint8 *padBufp1;       /* Pad buffer pointer 1 */
    nitf_Uint8 *padBufp2;       /* Pad buffer pointer 2 */
    nitf_Uint32 i;
    
    nitf = cntl->nitf;
    
    cntl->padBuffer = (nitf_Uint8 *) NITF_MALLOC(cntl->padBufferSize);
    if (cntl->padBuffer == NULL)
    {
        nitf_Error_initf(error, NITF_CTXT, NITF_ERR_MEMORY,
                         "Memory allocation error: %s", 
                         NITF_STRERROR(NITF_ERRNO));
        return NITF_FAILURE;
    }
    memmove(cntl->padBuffer, nitf->pixel.pad, nitf->pixel.bytes);
    
    /*      Duplicate pad pixel by propagating */
    
    padBufp1 = cntl->padBuffer;
    padBufp2 = cntl->padBuffer + nitf->pixel.bytes;
    for (i = 0; i < cntl->padBufferSize - nitf->pixel.bytes; i++)
        *(padBufp2++) = *(padBufp1++);
    
    return NITF_SUCCESS;
}


NITFPRIV(int) nitf_ImageIO_readPad(_nitf_ImageIOBlock * blockIO,
                                   nitf_Error * error)
{
    _nitf_ImageIOControl *cntl; /* Control structure */

    cntl = blockIO->cntl;
    if (cntl->padBuffer == NULL)
        if (!nitf_ImageIO_allocatePad(cntl, error))
            return NITF_FAILURE;

    memmove(blockIO->rwBuffer.buffer + blockIO->rwBuffer.offset.mark,
                  cntl->padBuffer,blockIO->readCount);
    return NITF_SUCCESS;
}


NITFPRIV(int) nitf_ImageIO_readFromFile(nitf_IOInterface* io,
                                        nitf_Uint64 fileOffset,
                                        nitf_Uint8 * buffer,
                                        size_t count,
                                        nitf_Error * error)
{
    size_t bytes;               /* Amount of current read */
    char *bufp;                 /* pointer into the buffer */
    /* Seek to the offset */
    bytes = count;
    bufp = (char *) buffer;
    
    
    if (!NITF_IO_SUCCESS(nitf_IOInterface_seek(io,
                                               (nitf_Off) fileOffset,
                                               NITF_SEEK_SET, error)))
    {
        return NITF_FAILURE;
    }
    if (!nitf_IOInterface_read(io, bufp, bytes, error))
    {
        return NITF_FAILURE;
    }
    return NITF_SUCCESS;
}

NITFPRIV(int) nitf_ImageIO_writeToFile(nitf_IOInterface* io,
                                       nitf_Uint64 fileOffset,
                                       const nitf_Uint8 * buffer,
                                       size_t count,
                                       nitf_Error * error)
{
    
    /* Seek to the offset */
    if (!NITF_IO_SUCCESS
        (nitf_IOInterface_seek
         (io, (nitf_Off) fileOffset, NITF_SEEK_SET, error)))
        return NITF_FAILURE;
    
    /* Write the data */
    if (! nitf_IOInterface_write(io, (char *) buffer, count, error) )
        return NITF_FAILURE;
    
    return NITF_SUCCESS;
}
NITFPRIV(int) nitf_ImageIO_writeToBlock(_nitf_ImageIOBlock * blockIO,
                                        nitf_IOInterface* io,
                                        size_t blockOffset,
                                        const nitf_Uint8 * buffer,
                                        size_t count,
                                        nitf_Error * error)
{
    _nitf_ImageIO *nitf;        /* Associated image I/O object */
    /* Associated block control */
    _nitf_ImageIOBlockCacheControl *blockCntl;
    nitf_Uint64 fileOffset;     /* Offset in filw for write */
    /* Pad scanning function */
    _NITF_IMAGE_IO_PAD_SCAN_FUNC scanner;
    
    nitf = ((_nitf_ImageIOControl *) (blockIO->cntl))->nitf;
    blockCntl = &(blockIO->blockControl);
    scanner = nitf->padScanner;
    
    
    if (blockCntl->block == NULL)
    {
        blockCntl->block =
            (nitf_Uint8 *) NITF_MALLOC(nitf->blockSize);
        if (blockCntl->block == NULL)
        {
            nitf_Error_initf(error, NITF_CTXT, NITF_ERR_MEMORY,
                             "Error allocating block buffer: %s", 
                             NITF_STRERROR(NITF_ERRNO));
            return NITF_FAILURE;
        }
    }
    
    /* Overflow check*/
    
    if ((blockOffset + count) > nitf->blockSize)
    {
        nitf_Error_initf(error, NITF_CTXT, NITF_ERR_MEMORY,
                         "Block buffer overflow");
        return NITF_FAILURE;
    }
    
    /* Copy data */
    
    memcpy(blockCntl->block + blockOffset, buffer, count);
    
    /*    Write block if full */
    
    if ((blockOffset + count) >= nitf->blockSize)
    {
        NITF_BOOL padPresent;     /* Pad values in block */
        NITF_BOOL dataPresent;    /* Data values in block */
        
        /*    Scan for pad if the scanner function is not NULL */
        
        if (scanner != NULL)
        {
            (*scanner)(blockIO, &padPresent, &dataPresent);
            if (!dataPresent)                  /* Pad only do not write */
            {
                /*
                 * Copy down all of the offsets since with the missing block
                 * this gives the correct offset for the following blocks. Copy
                 * one less offset than the remaining blocks. The block count is
                 * gotten by multiplying blocks per row by blocks 
                 * per column except
                 * for (band sequential) which has seperate 
                 * masks for each band and
                 * is not currently supported as explained in this functions
                 * documentation.
                 *
                 * The pad mask is set to the no block value correct 
                 * for a missing block
                 */
                /*
                 * This commented out code will generate the correct 
                 * blocking mask
                 * for S mode but the writing of the may be at the wrong offset
                 * It is left here for a future update to include S mode
                 */
#if 0
                if (nitf->blockingMode == NITF_IMAGE_IO_BLOCKING_MODE_S)
                {
                    
                    nitf_Uint32 nBlocksPerBand;  /* Number of blocks per band */
                    nitf_Uint32 nBlocksTotal;    /* Number of blocks total */
                    nitf_Uint32 numBands;      /* Number of bands */
                    nitf_Uint32 fullNumber;  /* Block number if full mask */
                    nitf_Uint32 srcIdx;      /* Index of copy source */
                    nitf_Uint32 dstIdx;      /* Index of copy destination */
                    
                    nBlocksPerBand = (nitf->nBlocksPerRow) * (nitf->nBlocksPerColumn);
                    nBlocksTotal = nitf->nBlocksTotal;
                    numBands = nitf->numBands;
                    fullNumber = blockIO->number + nBlocksPerBand * (blockIO->band);
                    srcIdx = nBlocksTotal - 2;
                    for (dstIdx = nBlocksTotal - 1;dstIdx > fullNumber;)
                    {
                        if (nitf->blockMask[dstIdx] == NITF_IMAGE_IO_NO_BLOCK)
                        {
                            dstIdx -= 1;
                            continue;
                        }
                        if (nitf->blockMask[srcIdx] == NITF_IMAGE_IO_NO_BLOCK)
                        {
                            srcIdx -= 1;
                            continue;
                        }
                        
                        nitf->blockMask[dstIdx--] = nitf->blockMask[srcIdx--];
                    }
                }
                else
                    memmove(&(blockIO->blockMask[blockIO->number+1]),
                            &(blockIO->blockMask[blockIO->number]),
                            (nitf->nBlocksPerRow * nitf->nBlocksPerColumn -
                             blockIO->number) * sizeof(nitf_Uint64));
#endif
                memmove(&(blockIO->blockMask[blockIO->number+1]),
                        &(blockIO->blockMask[blockIO->number]),
                        (nitf->nBlocksPerRow * nitf->nBlocksPerColumn -
                         blockIO->number) * sizeof(nitf_Uint64));
                
                blockIO->blockMask[blockIO->number] = NITF_IMAGE_IO_NO_BLOCK;
                blockIO->padMask[blockIO->number] = NITF_IMAGE_IO_NO_BLOCK;
                return NITF_SUCCESS;  /* Don't write */
            }
            
            if (padPresent)     /* Real block with pad */
                blockIO->padMask[blockIO->number] = 
                    blockIO->blockMask[blockIO->number];
        }
        
        /*
         * Reset the image data offset since it may be different now 
         * due to skipped blocks
         */
        blockIO->imageDataOffset = blockIO->blockMask[blockIO->number];
        
        fileOffset = nitf->pixelBase + blockIO->imageDataOffset;
        

        /*  Check for compression */

        if(nitf->compressor != NULL)
        {
            if(!(*(nitf->compressor->writeBlock))(nitf->compressionControl,
                            io,blockCntl->block,padPresent,!dataPresent,error))
            return(NITF_FAILURE);
        }
        else
        {
            /* Seek to the offset */
            if (!NITF_IO_SUCCESS
                    (nitf_IOInterface_seek
                            (io, (nitf_Off) fileOffset, NITF_SEEK_SET, error)))
            return NITF_FAILURE;

            /* Write the data */

            if (!nitf_IOInterface_write(io,
                            (char *) (blockCntl->block),
                            nitf->blockSize, error) )
            return NITF_FAILURE;
        }
    }
    
    return NITF_SUCCESS;
}

NITFPRIV(void) nitf_ImageIO_nextRow(_nitf_ImageIOBlock * blockIO,
                                    NITF_BOOL noUserInc)
{
    _nitf_ImageIOControl *cntl; /* Associated I/O control structure */
    _nitf_ImageIO *nitf;        /* Parent _nitf_ImageIO object */
    
    cntl = blockIO->cntl;
    nitf = cntl->nitf;
    
    if (blockIO->rowsUntil == 0)  /* Next block */
    {
        blockIO->number += cntl->numberInc;
        blockIO->imageDataOffset = blockIO->blockMask[blockIO->number];
        blockIO->blockOffset.mark = blockIO->blockOffset.orig;
    }
    else
        blockIO->blockOffset.mark += cntl->blockOffsetInc;
    
    blockIO->rwBuffer.offset.mark += cntl->bufferInc;
    blockIO->unpacked.offset.mark += cntl->unpackedInc;
    if (!noUserInc)
        blockIO->user.offset.mark += cntl->userInc;
    
    return;
}


int nitf_ImageIO_uncachedReader(_nitf_ImageIOBlock * blockIO,
                                nitf_IOInterface* io, 
                                nitf_Error * error)
{
    
    /* Check for pad pixel read */
    if (blockIO->imageDataOffset == NITF_IMAGE_IO_NO_OFFSET)
    {
        if (!nitf_ImageIO_readPad(blockIO, error))
            return NITF_FAILURE;
        
        blockIO->cntl->padded = 1;
        return NITF_SUCCESS;
    }
    else
    {
        if (!nitf_ImageIO_readFromFile(io,
                                       blockIO->cntl->nitf->pixelBase +
                                       blockIO->imageDataOffset +
                                       blockIO->blockOffset.mark,
                                       blockIO->rwBuffer.buffer +
                                       blockIO->rwBuffer.offset.mark,
                                       blockIO->readCount, error))
        {
            return NITF_FAILURE;
        }
        
        if (blockIO->padMask[blockIO->number] != NITF_IMAGE_IO_NO_OFFSET)
            blockIO->cntl->padded = 1;
        
        return NITF_SUCCESS;
    }
}


int nitf_ImageIO_cachedReader(_nitf_ImageIOBlock * blockIO,
                              nitf_IOInterface* io, 
                              nitf_Error * error)
{
    _nitf_ImageIO *nitf;        /* Associated ImageIO object */
    _nitf_ImageIOControl *cntl; /* Associated control object */
    
    cntl = blockIO->cntl;
    nitf = cntl->nitf;
    
    /* Check for pad pixel read */
    
    if (blockIO->imageDataOffset == NITF_IMAGE_IO_NO_OFFSET)
    {
        if (!nitf_ImageIO_readPad(blockIO, error))
            return NITF_FAILURE;
        
        cntl->padded = 1;
        return NITF_SUCCESS;
    }
    else
    {
        if (nitf->blockControl.number != blockIO->number)
        {
            if ((nitf->pixel.type != NITF_IMAGE_IO_PIXEL_TYPE_B)
                  && (nitf->pixel.type != NITF_IMAGE_IO_PIXEL_TYPE_12)
                     && (nitf->compression & NITF_IMAGE_IO_NO_COMPRESSION))
            {
                /* Allocate block buffer if required */
                if (nitf->blockControl.block == NULL)
                {
                    nitf->blockControl.block =
                        (nitf_Uint8 *) NITF_MALLOC(nitf->blockSize);
                    if (nitf->blockControl.block == NULL)
                    {
                        nitf_Error_initf(error, NITF_CTXT, NITF_ERR_MEMORY,
                                         "Error allocating block buffer: %s",
                                         NITF_STRERROR(NITF_ERRNO));
                        return NITF_FAILURE;
                    }
                }
                /* Read the block */

                if (!nitf_ImageIO_readFromFile(io,
                                               blockIO->cntl->nitf->
                                               pixelBase +
                                               blockIO->imageDataOffset,
                                               nitf->blockControl.block,
                                               nitf->blockSize, error))
                    return NITF_FAILURE;
            }
            else
            {
                /* Decompression interface structure */
                nitf_DecompressionInterface *interface;
                
                /* No plugin */
                if (nitf->decompressor == NULL)
                {
                    nitf_Error_initf(error, NITF_CTXT,
                                     NITF_ERR_DECOMPRESSION,
                                     "No decompression plugin for compressed type");
                    return NITF_FAILURE;
                }
                
                interface = nitf->decompressor;
                if (nitf->blockControl.block != NULL)
                    (*(interface->freeBlock)) (nitf->decompressionControl,
                                               nitf->blockControl.block,
                                               error);
                nitf->blockControl.block =
                    (*(interface->readBlock)) (nitf->decompressionControl,
                                               blockIO->number, error);
                if (nitf->blockControl.block == NULL)
                    return NITF_FAILURE;
            }
            nitf->blockControl.number = blockIO->number;
        }
        
        /* Get data from block */
        
        memcpy(blockIO->rwBuffer.buffer + blockIO->rwBuffer.offset.mark,
               nitf->blockControl.block + blockIO->blockOffset.mark,
               blockIO->readCount);
        
        if (blockIO->padMask[blockIO->number] != NITF_IMAGE_IO_NO_OFFSET)
            blockIO->cntl->padded = 1;
        
        return NITF_SUCCESS;
    }
}


int nitf_ImageIO_uncachedWriter(_nitf_ImageIOBlock * blockIO,
                                nitf_IOInterface* io, 
                                nitf_Error * error)
{
    _nitf_ImageIOControl *cntl; /* Associated image I/O control object */
    _nitf_ImageIO *nitf;        /* Associated image I/O object */
    
    cntl = blockIO->cntl;
    nitf = cntl->nitf;
    
    if (!nitf_ImageIO_writeToFile(io,
                                  nitf->pixelBase + blockIO->imageDataOffset
                                  + blockIO->blockOffset.mark,
                                  blockIO->rwBuffer.buffer + 
                                  blockIO->rwBuffer.offset.mark,
                                  blockIO->readCount, error))
        return NITF_FAILURE;
    
    /*      Write column and row padding if required. This is fill not pad */
    
    if (blockIO->padColumnCount != 0)
    {
        if (cntl->padBuffer == NULL)
            if (!nitf_ImageIO_allocatePad(cntl, error))
                return NITF_FAILURE;
        
        if (!nitf_ImageIO_writeToFile(io,
                                      nitf->pixelBase +
                                      blockIO->imageDataOffset +
                                      blockIO->blockOffset.mark + 
                                      blockIO->readCount,
                                      cntl->padBuffer,
                                      (size_t)blockIO->padColumnCount,
                                      error))
            return NITF_FAILURE;
        /* Fill not pad blockIO->padMask[blockIO->number] = blockIO->blockMask[blockIO->number]; */
    }
    
    if ((blockIO->padRowCount != 0)
        && (blockIO->currentRow >= (nitf->numRows - 1)))
    {
        size_t writeCount;      /* Amount to write each row */
        nitf_Uint64 offset;     /* File offset for each write */
        size_t increment;       /* Byte increment between writes */
        nitf_Uint32 rowIdx;     /* Rwo counter */
        
        if (cntl->padBuffer == NULL)
            if (!nitf_ImageIO_allocatePad(cntl, error))
                return NITF_FAILURE;
        
        writeCount = blockIO->readCount + blockIO->padColumnCount;
        increment = writeCount;
        offset = nitf->pixelBase + blockIO->imageDataOffset
                 + blockIO->blockOffset.mark + increment;

        for (rowIdx = 0; rowIdx < blockIO->padRowCount; rowIdx++)
        {
            if (!nitf_ImageIO_writeToFile(io,
                                          offset, cntl->padBuffer,
                                          writeCount, error))
                return NITF_FAILURE;
            offset += increment;
        }
    }
    
    return NITF_SUCCESS;
}

int nitf_ImageIO_cachedWriter(_nitf_ImageIOBlock * blockIO,
                              nitf_IOInterface* io, 
                              nitf_Error * error)
{
    _nitf_ImageIOControl *cntl; /* Associated image I/O control object */
    _nitf_ImageIO *nitf;        /* Associated image I/O object */
    nitf_Uint32 i;
    
    cntl = blockIO->cntl;
    nitf = cntl->nitf;
    
    if (!nitf_ImageIO_writeToBlock(blockIO, io, blockIO->blockOffset.mark,
                                   blockIO->rwBuffer.buffer + 
                                   blockIO->rwBuffer.offset.mark,
                                   blockIO->readCount, error))
        return NITF_FAILURE;

    /*      Write column and row padding if required */

    if (blockIO->padColumnCount != 0)
    {
        if (cntl->padBuffer == NULL)
            if (!nitf_ImageIO_allocatePad(cntl, error))
                return NITF_FAILURE;

        if (!nitf_ImageIO_writeToBlock(blockIO, io,
                                       blockIO->blockOffset.mark + 
                                       blockIO->readCount,
                                       cntl->padBuffer,
                                       blockIO->padColumnCount, error))
            return NITF_FAILURE;
    }
    
    if ((blockIO->padRowCount != 0)
        && (blockIO->currentRow >= (nitf->numRows - 1)))
    {
        size_t writeCount; /* Amount to write each row */
        size_t offset;     /* Current offset into block buffer */
        
        if (cntl->padBuffer == NULL)
            if (!nitf_ImageIO_allocatePad(cntl, error))
                return NITF_FAILURE;
        
        writeCount = blockIO->readCount + blockIO->padColumnCount;
        offset = writeCount;
        
        /*
         * Because the pad buffer is only one line long, this 
         * has to be a loop of
         * smaller writes
         */
        for (i = 0;i < blockIO->padRowCount;i++)
        {
            if (!nitf_ImageIO_writeToBlock(blockIO, io,
                                           blockIO->blockOffset.mark + offset,
                                           cntl->padBuffer, writeCount, error))
                return NITF_FAILURE;
            offset += writeCount;
        }
    }

    return NITF_SUCCESS;
}

void nitf_ImageIO_setDefaultParameters(_nitf_ImageIO * object)
{
    
    object->parameters.noCacheThreshold = 0.5;
    object->parameters.clearCache = 0;;
    
    return;
}


void nitf_ImageIO_unformatExtend_1(nitf_Uint8 * buffer,
                                   size_t count,
                                   nitf_Uint32 shiftCount)
{
    nitf_Int8 shift;            /* Shift count */
    nitf_Int8 *bp8;             /* Buffer pointer, 8 bit */
    nitf_Int16 tmp8;            /* Temp value, 8 bit */
    size_t i;
    
    shift = (nitf_Int8) shiftCount;
    bp8 = (nitf_Int8 *) buffer;
    for (i = 0; i < count; i++)
    {
        tmp8 = *bp8 << shift;
        *(bp8++) = tmp8 >> shift;
    }
    
    return;
}


void nitf_ImageIO_unformatExtend_2(nitf_Uint8 * buffer,
                                   size_t count,
                                   nitf_Uint32 shiftCount)
{
    nitf_Int16 shift;           /* Shift count */
    nitf_Int16 *bp16;           /* Buffer pointer, 16 bit */
    nitf_Int16 tmp16;           /* Temp value, 16 bit */
    size_t i;
    
    shift = (nitf_Int16) shiftCount;
    bp16 = (nitf_Int16 *) buffer;
    for (i = 0; i < count; i++)
    {
        tmp16 = *bp16 << shift;
        *(bp16++) = tmp16 >> shift;
    }
    
    return;
}


void nitf_ImageIO_unformatExtend_4(nitf_Uint8 * buffer,
                                   size_t count,
                                   nitf_Uint32 shiftCount)
{
    nitf_Int32 shift;           /* Shift count */
    nitf_Int32 *bp32;           /* Buffer pointer, 32 bit */
    nitf_Int32 tmp32;           /* Temp value, 32 bit */
    size_t i;
    
    shift = (nitf_Int32) shiftCount;
    bp32 = (nitf_Int32 *) buffer;
    for (i = 0; i < count; i++)
    {
        tmp32 = *bp32 << shift;
        *(bp32++) = tmp32 >> shift;
    }
    
    return;
}


/* Uses 64 bit types */

void nitf_ImageIO_unformatExtend_8(nitf_Uint8 * buffer,
                                   size_t count,
                                   nitf_Uint32 shiftCount)
{
    nitf_Int64 shift;           /* Shift count */
    nitf_Int64 *bp64;           /* Buffer pointer, 64 bit */
    nitf_Int64 tmp64;           /* Temp value, 64 bit */
    size_t i;
    
    shift = (nitf_Int64) shiftCount;
    bp64 = (nitf_Int64 *) buffer;
    for (i = 0; i < count; i++)
    {
        tmp64 = *bp64 << shift;
        *(bp64++) = tmp64 >> shift;
    }
    
    return;
}


/*========================= nitf_ImageIO_unformatShift_* =====================*/

void nitf_ImageIO_unformatShift_1(nitf_Uint8 * buffer,
                                  size_t count,
                                  nitf_Uint32 shiftCount)
{
    nitf_Int8 shift;            /* Shift count */
    nitf_Int8 *bp8;             /* Buffer pointer, 8 bit */
    size_t i;
    
    shift = (nitf_Int8) shiftCount;
    bp8 = (nitf_Int8 *) buffer;
    for (i = 0; i < count; i++)
        *(bp8++) >>= shift;
    
    return;
}


void nitf_ImageIO_unformatShift_2(nitf_Uint8 * buffer,
                                  size_t count,
                                  nitf_Uint32 shiftCount)
{
    nitf_Int16 shift;           /* Shift count */
    nitf_Int16 *bp16;           /* Buffer pointer, 16 bit */
    size_t i;
    
    shift = (nitf_Int16) shiftCount;
    bp16 = (nitf_Int16 *) buffer;
    for (i = 0; i < count; i++)
        *(bp16++) >>= shift;
    
    return;
}


void nitf_ImageIO_unformatShift_4(nitf_Uint8 * buffer,
                                  size_t count,
                                  nitf_Uint32 shiftCount)
{
    nitf_Int32 shift;           /* Shift count */
    nitf_Int32 *bp32;           /* Buffer pointer, 32 bit */
    size_t i;
    
    shift = (nitf_Int32) shiftCount;
    bp32 = (nitf_Int32 *) buffer;
    for (i = 0; i < count; i++)
        *(bp32++) >>= shift;
    
    return;
}


/* Uses 64 bit types */

void nitf_ImageIO_unformatShift_8(nitf_Uint8 * buffer,
                                  size_t count,
                                  nitf_Uint32 shiftCount)
{
    nitf_Int64 shift;           /* Shift count */
    nitf_Int64 *bp64;           /* Buffer pointer, 64 bit */
    size_t i;
    
    shift = (nitf_Int64) shiftCount;
    bp64 = (nitf_Int64 *) buffer;
    for (i = 0; i < count; i++)
        *(bp64++) >>= shift;
    
    return;
}

void nitf_ImageIO_unformatUShift_1(nitf_Uint8 * buffer,
                                   size_t count,
                                   nitf_Uint32 shiftCount)
{
    nitf_Uint8 shift;           /* Shift count */
    nitf_Uint8 *bp8;            /* Buffer pointer, 8 bit */
    size_t i;
    
    shift = (nitf_Uint8) shiftCount;
    bp8 = (nitf_Uint8 *) buffer;
    for (i = 0; i < count; i++)
        *(bp8++) >>= shift;
    
    return;
}


void nitf_ImageIO_unformatUShift_2(nitf_Uint8 * buffer,
                                   size_t count,
                                   nitf_Uint32 shiftCount)
{
    nitf_Uint16 shift;          /* Shift count */
    nitf_Uint16 *bp16;          /* Buffer pointer, 16 bit */
    size_t i;
    
    shift = (nitf_Uint16) shiftCount;
    bp16 = (nitf_Uint16 *) buffer;
    for (i = 0; i < count; i++)
        *(bp16++) >>= shift;
    
    return;
}


void nitf_ImageIO_unformatUShift_4(nitf_Uint8 * buffer,
                                   size_t count,
                                   nitf_Uint32 shiftCount)
{
    nitf_Uint32 shift;          /* Shift count */
    nitf_Uint32 *bp32;          /* Buffer pointer, 32 bit */
    size_t i;
    
    shift = (nitf_Uint32) shiftCount;
    bp32 = (nitf_Uint32 *) buffer;
    for (i = 0; i < count; i++)
        *(bp32++) >>= shift;
    
    return;
}


/* Uses 64 bit types */
void nitf_ImageIO_unformatUShift_8(nitf_Uint8 * buffer,
                                   size_t count,
                                   nitf_Uint32 shiftCount)
{
    nitf_Uint64 shift;          /* Shift count */
    nitf_Uint64 *bp64;          /* Buffer pointer, 64 bit */
    size_t i;
    
    shift = (nitf_Uint64) shiftCount;
    bp64 = (nitf_Uint64 *) buffer;
    for (i = 0; i < count; i++)
        *(bp64++) >>= shift;
    
    return;
}

void nitf_ImageIO_swapOnly_2(nitf_Uint8 * buffer,
        size_t count, nitf_Uint32 shiftCount)
{
    nitf_Uint8 *bp8;            /* Buffer pointer, 8 bit */
    nitf_Uint16 *bp16;          /* Buffer pointer, 16 bit */
    nitf_Uint8 tmp8;            /* Temp value, 8 bit */
    size_t i;
    
    bp16 = (nitf_Uint16 *) buffer;
    for (i = 0; i < count; i++)
    {
        bp8 = (nitf_Uint8 *) bp16++;
        tmp8 = bp8[0];
        bp8[0] = bp8[1];
        bp8[1] = tmp8;
    }
    
    return;
}


void nitf_ImageIO_swapOnly_4(nitf_Uint8 * buffer,
        size_t count, nitf_Uint32 shiftCount)
{
    nitf_Uint8 *bp8;            /* Buffer pointer, 8 bit */
    nitf_Uint32 *bp32;          /* Buffer pointer, 32 bit */
    nitf_Uint8 tmp8;            /* Temp value, 8 bit */
    size_t i;
    
    bp32 = (nitf_Uint32 *) buffer;
    for (i = 0; i < count; i++)
    {
        bp8 = (nitf_Uint8 *) (bp32++);
        
        tmp8 = bp8[0];
        bp8[0] = bp8[3];
        bp8[3] = tmp8;
        tmp8 = bp8[1];
        bp8[1] = bp8[2];
        bp8[2] = tmp8;
    }
    
    return;
}

void nitf_ImageIO_swapOnly_4c(nitf_Uint8 * buffer,
        size_t count, nitf_Uint32 shiftCount)
{
    nitf_Uint8 *bp8;            /* Buffer pointer, 8 bit */
    nitf_Uint32 *bp32;          /* Buffer pointer, 32 bit */
    nitf_Uint8 tmp8;            /* Temp value, 8 bit */
    size_t i;

    bp32 = (nitf_Uint32 *) buffer;
    for (i = 0; i < count; i++)
    {
        bp8 = (nitf_Uint8 *) (bp32++);

        tmp8 = bp8[0];
        bp8[0] = bp8[1];
        bp8[1] = tmp8;
        tmp8 = bp8[2];
        bp8[2] = bp8[3];
        bp8[3] = tmp8;
    }

    return;
}


void nitf_ImageIO_swapOnly_8(nitf_Uint8 * buffer,
        size_t count, nitf_Uint32 shiftCount)
{
    nitf_Uint8 *bp8;            /* Buffer pointer, 8 bit */
    nitf_Uint64 *bp64;          /* Buffer pointer, 64 bit */
    nitf_Uint8 tmp8;            /* Temp value, 8 bit */
    size_t i;
    
    bp64 = (nitf_Uint64 *) buffer;
    for (i = 0; i < count; i++)
    {
        bp8 = (nitf_Uint8 *) (bp64++);
        
        tmp8 = bp8[0];
        bp8[0] = bp8[7];
        bp8[7] = tmp8;
        tmp8 = bp8[1];
        bp8[1] = bp8[6];
        bp8[6] = tmp8;
        tmp8 = bp8[2];
        bp8[2] = bp8[5];
        bp8[5] = tmp8;
        tmp8 = bp8[3];
        bp8[3] = bp8[4];
        bp8[4] = tmp8;
    }
    
    return;
}


void nitf_ImageIO_swapOnly_8c(nitf_Uint8 * buffer,
        size_t count, nitf_Uint32 shiftCount)
{
    nitf_Uint8 *bp8;            /* Buffer pointer, 8 bit */
    nitf_Uint64 *bp64;          /* Buffer pointer, 64 bit */
    nitf_Uint8 tmp8;            /* Temp value, 8 bit */
    size_t i;
    
    bp64 = (nitf_Uint64 *) buffer;
    for (i = 0; i < count; i++)
    {
        bp8 = (nitf_Uint8 *) (bp64++);
        
        tmp8 = bp8[4];
        bp8[4] = bp8[7];
        bp8[7] = tmp8;
        tmp8 = bp8[5];
        bp8[5] = bp8[6];
        bp8[6] = tmp8;
        tmp8 = bp8[0];
        bp8[0] = bp8[3];
        bp8[3] = tmp8;
        tmp8 = bp8[1];
        bp8[1] = bp8[2];
        bp8[2] = tmp8;
    }
    
    return;
}


void nitf_ImageIO_swapOnly_16c(nitf_Uint8 * buffer,
        size_t count, nitf_Uint32 shiftCount)
{
    nitf_Uint8 *bp8;            /* Buffer pointer, 8 bit */
    nitf_Uint64 *bp64;          /* Buffer pointer, 64 bit */
    nitf_Uint8 tmp8;            /* Temp value, 8 bit */
    size_t i;
    
    bp64 = (nitf_Uint64 *) buffer;
    for (i = 0; i < count; i++)
    {
        bp8 = (nitf_Uint8 *) (bp64++);
        
        tmp8 = bp8[0];
        bp8[0] = bp8[7];
        bp8[7] = tmp8;
        tmp8 = bp8[1];
        bp8[1] = bp8[6];
        bp8[6] = tmp8;
        tmp8 = bp8[2];
        bp8[2] = bp8[5];
        bp8[5] = tmp8;
        tmp8 = bp8[3];
        bp8[3] = bp8[4];
        bp8[4] = tmp8;
        
        bp8 = (nitf_Uint8 *) (bp64++);
        
        tmp8 = bp8[0];
        bp8[0] = bp8[7];
        bp8[7] = tmp8;
        tmp8 = bp8[1];
        bp8[1] = bp8[6];
        bp8[6] = tmp8;
        tmp8 = bp8[2];
        bp8[2] = bp8[5];
        bp8[5] = tmp8;
        tmp8 = bp8[3];
        bp8[3] = bp8[4];
        bp8[4] = tmp8;
    }
    
    return;
}


void nitf_ImageIO_unformatSwapExtend_2(nitf_Uint8 * buffer,
                                       size_t count,
                                       nitf_Uint32 shiftCount)
{
    nitf_Int16 shift;           /* Shift count */
    nitf_Uint8 *bp8;            /* Buffer pointer, 8 bit */
    nitf_Int16 *bp16;           /* Buffer pointer, 16 bit */
    nitf_Uint8 tmp8;            /* Temp value, 8 bit */
    nitf_Uint16 tmp16;          /* Temp value, 16 bit */
    size_t i;
    
    shift = (nitf_Int16) shiftCount;
    bp16 = (nitf_Int16 *) buffer;
    for (i = 0; i < count; i++)
    {
        bp8 = (nitf_Uint8 *) bp16;
        tmp8 = bp8[0];
        bp8[0] = bp8[1];
        bp8[1] = tmp8;
        
        tmp16 = *bp16 << shift;
        *(bp16++) = tmp16 >> shift;
    }
    
    return;
}


void nitf_ImageIO_unformatSwapExtend_4(nitf_Uint8 * buffer,
                                       size_t count,
                                       nitf_Uint32 shiftCount)
{
    nitf_Int32 shift;           /* Shift count */
    nitf_Uint8 *bp8;            /* Buffer pointer, 8 bit */
    nitf_Int32 *bp32;           /* Buffer pointer, 32 bit */
    nitf_Uint8 tmp8;            /* Temp value, 8 bit */
    nitf_Uint8 tmp32;           /* Temp value, 32 bit */
    size_t i;
    
    shift = (nitf_Int32) shiftCount;
    bp32 = (nitf_Int32 *) buffer;
    for (i = 0; i < count; i++)
    {
        bp8 = (nitf_Uint8 *) (bp32++);
        
        tmp8 = bp8[0];
        bp8[0] = bp8[3];
        bp8[3] = tmp8;
        tmp8 = bp8[1];
        bp8[1] = bp8[2];
        bp8[2] = tmp8;
        
        tmp32 = *bp32 << shift;
        *(bp32++) = tmp32 >> shift;
    }
    
    return;
}


/* Uses 64 bit types */
void nitf_ImageIO_unformatSwapExtend_8(nitf_Uint8 * buffer,
                                       size_t count,
                                       nitf_Uint32 shiftCount)
{
    nitf_Int64 shift;           /* Shift count */
    nitf_Uint8 *bp8;            /* Buffer pointer, 8 bit */
    nitf_Int64 *bp64;           /* Buffer pointer, 64 bit */
    nitf_Uint8 tmp8;            /* Temp value, 8 bit */
    nitf_Uint64 tmp64;          /* Temp value, 64 bit */
    size_t i;
    
    shift = (nitf_Int64) shiftCount;
    bp64 = (nitf_Int64 *) buffer;
    for (i = 0; i < count; i++)
    {
        bp8 = (nitf_Uint8 *) bp64;
        
        tmp8 = bp8[0];
        bp8[0] = bp8[7];
        bp8[7] = tmp8;
        tmp8 = bp8[1];
        bp8[1] = bp8[6];
        bp8[6] = tmp8;
        tmp8 = bp8[2];
        bp8[2] = bp8[5];
        bp8[5] = tmp8;
        tmp8 = bp8[3];
        bp8[3] = bp8[4];
        bp8[4] = tmp8;
        
        tmp64 = *bp64 << shift;
        *(bp64++) = tmp64 >> shift;
    }
    
    return;
}


void nitf_ImageIO_unformatSwapShift_2(nitf_Uint8 * buffer,
                                      size_t count,
                                      nitf_Uint32 shiftCount)
{
    nitf_Int16 shift;           /* Shift count */
    nitf_Uint8 *bp8;            /* Buffer pointer, 8 bit */
    nitf_Int16 *bp16;           /* Buffer pointer, 16 bit */
    nitf_Uint8 tmp8;            /* Temp value, 8 bit */
    size_t i;
    
    shift = (nitf_Int16) shiftCount;
    bp16 = (nitf_Int16 *) buffer;
    for (i = 0; i < count; i++)
    {
        bp8 = (nitf_Uint8 *) bp16;
        tmp8 = bp8[0];
        bp8[0] = bp8[1];
        bp8[1] = tmp8;
        
        *(bp16++) >>= shift;
    }
    
    return;
}


void nitf_ImageIO_unformatSwapShift_4(nitf_Uint8 * buffer,
                                      size_t count,
                                      nitf_Uint32 shiftCount)
{
    nitf_Int32 shift;           /* Shift count */
    nitf_Uint8 *bp8;            /* Buffer pointer, 8 bit */
    nitf_Int32 *bp32;           /* Buffer pointer, 32 bit */
    nitf_Uint8 tmp8;            /* Temp value, 8 bit */
    size_t i;
    
    shift = (nitf_Int32) shiftCount;
    bp32 = (nitf_Int32 *) buffer;
    for (i = 0; i < count; i++)
    {
        bp8 = (nitf_Uint8 *) bp32;
        
        tmp8 = bp8[0];
        bp8[0] = bp8[3];
        bp8[3] = tmp8;
        tmp8 = bp8[1];
        bp8[1] = bp8[2];
        bp8[2] = tmp8;

        *(bp32++) >>= shift;
    }
    
    return;
}


/* Uses 64 bit types */
void nitf_ImageIO_unformatSwapShift_8(nitf_Uint8 * buffer,
                                      size_t count,
                                      nitf_Uint32 shiftCount)
{
    nitf_Int64 shift;           /* Shift count */
    nitf_Uint8 *bp8;            /* Buffer pointer, 8 bit */
    nitf_Int64 *bp64;           /* Buffer pointer, 64 bit */
    nitf_Uint8 tmp8;            /* Temp value, 8 bit */
    size_t i;
    
    shift = (nitf_Int64) shiftCount;
    bp64 = (nitf_Int64 *) buffer;
    for (i = 0; i < count; i++)
    {
        bp8 = (nitf_Uint8 *) bp64;
        
        tmp8 = bp8[0];
        bp8[0] = bp8[7];
        bp8[7] = tmp8;
        tmp8 = bp8[1];
        bp8[1] = bp8[6];
        bp8[6] = tmp8;
        tmp8 = bp8[2];
        bp8[2] = bp8[5];
        bp8[5] = tmp8;
        tmp8 = bp8[3];
        bp8[3] = bp8[4];
        bp8[4] = tmp8;
        
        *(bp64++) >>= shift;
    }
    
    return;
}


void nitf_ImageIO_unformatSwapUShift_2(nitf_Uint8 * buffer,
                                       size_t count,
                                       nitf_Uint32 shiftCount)
{
    nitf_Uint16 shift;          /* Shift count */
    nitf_Uint8 *bp8;            /* Buffer pointer, 8 bit */
    nitf_Uint16 *bp16;          /* Buffer pointer, 16 bit */
    nitf_Uint8 tmp8;            /* Temp value, 8 bit */
    size_t i;
    
    shift = (nitf_Uint16) shiftCount;
    bp16 = (nitf_Uint16 *) buffer;
    for (i = 0; i < count; i++)
    {
        bp8 = (nitf_Uint8 *) bp16;
        tmp8 = bp8[0];
        bp8[0] = bp8[1];
        bp8[1] = tmp8;
        
        *(bp16++) >>= shift;
    }
    
    return;
}

void nitf_ImageIO_unformatSwapUShift_4(nitf_Uint8 * buffer,
                                       size_t count,
                                       nitf_Uint32 shiftCount)
{
    nitf_Uint32 shift;          /* Shift count */
    nitf_Uint8 *bp8;            /* Buffer pointer, 8 bit */
    nitf_Uint32 *bp32;          /* Buffer pointer, 32 bit */
    nitf_Uint8 tmp8;            /* Temp value, 8 bit */
    size_t i;
    
    shift = (nitf_Uint32) shiftCount;
    bp32 = (nitf_Uint32 *) buffer;
    for (i = 0; i < count; i++)
    {
        bp8 = (nitf_Uint8 *) bp32;
        
        tmp8 = bp8[0];
        bp8[0] = bp8[3];
        bp8[3] = tmp8;
        tmp8 = bp8[1];
        bp8[1] = bp8[2];
        bp8[2] = tmp8;
        
        *(bp32++) >>= shift;
    }
    
    return;
}


/* Uses 64 bit types */
void nitf_ImageIO_unformatSwapUShift_8(nitf_Uint8 * buffer,
                                       size_t count,
                                       nitf_Uint32 shiftCount)
{
    nitf_Uint64 shift;          /* Shift count */
    nitf_Uint8 *bp8;            /* Buffer pointer, 8 bit */
    nitf_Uint64 *bp64;          /* Buffer pointer, 64 bit */
    nitf_Uint8 tmp8;            /* Temp value, 8 bit */
    size_t i;
    
    shift = (nitf_Uint64) shiftCount;
    bp64 = (nitf_Uint64 *) buffer;
    for (i = 0; i < count; i++)
    {
        bp8 = (nitf_Uint8 *) bp64;
        
        tmp8 = bp8[0];
        bp8[0] = bp8[7];
        bp8[7] = tmp8;
        tmp8 = bp8[1];
        bp8[1] = bp8[6];
        bp8[6] = tmp8;
        tmp8 = bp8[2];
        bp8[2] = bp8[5];
        bp8[5] = tmp8;
        tmp8 = bp8[3];
        bp8[3] = bp8[4];
        bp8[4] = tmp8;
        
        *(bp64++) >>= shift;
    }
    
    return;
}


void nitf_ImageIO_unpack_P_1(_nitf_ImageIOBlock * blockIO,
                             nitf_Error * error)
{
    nitf_Uint8 *src;            /* Source buffer */
    nitf_Uint8 *dst;            /* Destination buffer */
    size_t count;               /* Number of pixels to transfer */
    nitf_Uint32 skip;           /* Source buffer skip count */
    size_t i;
    
    src = (nitf_Uint8 *) (blockIO->rwBuffer.buffer
                          + blockIO->rwBuffer.offset.mark);
    dst = (nitf_Uint8 *) (blockIO->unpacked.buffer
                          + blockIO->unpacked.offset.mark);
    count = blockIO->pixelCountFR;
    skip = blockIO->cntl->nitf->numBands;

    for (i = 0; i < count; i++)
    {
        *(dst++) = *src;
        src += skip;
    }
    return;
}


void nitf_ImageIO_unpack_P_2(_nitf_ImageIOBlock * blockIO,
                             nitf_Error * error)
{
    nitf_Uint16 *src;           /* Source buffer */
    nitf_Uint16 *dst;           /* Destination buffer */
    size_t count;               /* Number of pixels to transfer */
    nitf_Uint32 skip;           /* Source buffer skip count */
    size_t i;
    
    src = (nitf_Uint16 *) (blockIO->rwBuffer.buffer
                           + blockIO->rwBuffer.offset.mark);
    dst = (nitf_Uint16 *) (blockIO->unpacked.buffer
                           + blockIO->unpacked.offset.mark);
    count = blockIO->pixelCountFR;
    skip = blockIO->cntl->nitf->numBands;
    
    for (i = 0; i < count; i++)
    {
        *(dst++) = *src;
        src += skip;
    }
    return;
}


void nitf_ImageIO_unpack_P_4(_nitf_ImageIOBlock * blockIO,
                             nitf_Error * error)
{
    nitf_Uint32 *src;           /* Source buffer */
    nitf_Uint32 *dst;           /* Destination buffer */
    size_t count;               /* Number of pixels to transfer */
    nitf_Uint32 skip;           /* Source buffer skip count */
    size_t i;
    
    src = (nitf_Uint32 *) (blockIO->rwBuffer.buffer
                           + blockIO->rwBuffer.offset.mark);
    dst = (nitf_Uint32 *) (blockIO->unpacked.buffer
                           + blockIO->unpacked.offset.mark);
    count = blockIO->pixelCountFR;
    skip = blockIO->cntl->nitf->numBands;
    
    for (i = 0; i < count; i++)
    {
        *(dst++) = *src;
        src += skip;
    }
    return;
}


void nitf_ImageIO_unpack_P_8(_nitf_ImageIOBlock * blockIO,
                             nitf_Error * error)
{
    nitf_Uint64 *src;           /* Source buffer */
    nitf_Uint64 *dst;           /* Destination buffer */
    size_t count;               /* Number of pixels to transfer */
    nitf_Uint32 skip;           /* Source buffer skip count */
    size_t i;
    
    src = (nitf_Uint64 *) (blockIO->rwBuffer.buffer
                           + blockIO->rwBuffer.offset.mark);
    dst = (nitf_Uint64 *) (blockIO->unpacked.buffer
                           + blockIO->unpacked.offset.mark);
    count = blockIO->pixelCountFR;
    skip = blockIO->cntl->nitf->numBands;
    
    for (i = 0; i < count; i++)
    {
        *(dst++) = *src;
        src += skip;
    }
    return;
}


void nitf_ImageIO_unpack_P_16(_nitf_ImageIOBlock * blockIO,
                              nitf_Error * error)
{
    nitf_Uint64 *src1;          /* Source buffer 1 */
    nitf_Uint64 *dst1;          /* Destination buffer 1 */
    nitf_Uint64 *src2;          /* Source buffer 2 */
    nitf_Uint64 *dst2;          /* Destination buffer 2 */
    size_t count;               /* Number of pixels to transfer */
    nitf_Uint32 skip;           /* Source buffer skip count */
    size_t i;
    
    src1 = (nitf_Uint64 *) (blockIO->rwBuffer.buffer
                            + blockIO->rwBuffer.offset.mark);
    dst1 = (nitf_Uint64 *) (blockIO->unpacked.buffer
                            + blockIO->unpacked.offset.mark);
    src2 = src1 + 1;
    dst2 = dst1 + 1;
    count = blockIO->pixelCountFR;
    skip = blockIO->cntl->nitf->numBands * 2;
    
    for (i = 0; i < count; i++)
    {
        *(dst1++) = *src1;
        *(dst2++) = *src2;
        src1 += skip;
        src2 += skip;
    }
    return;
}

void nitf_ImageIO_pack_P_1(_nitf_ImageIOBlock * blockIO, nitf_Error * error)
{
    nitf_Uint8 *src;            /* Source buffer */
    nitf_Uint8 *dst;            /* Destination buffer */
    size_t count;               /* Number of pixels to transfer */
    nitf_Uint32 skip;           /* Source buffer skip count */
    size_t i;
    
    src = (nitf_Uint8 *) (blockIO->user.buffer + blockIO->user.offset.mark);
    dst = (nitf_Uint8 *) (blockIO->rwBuffer.buffer);
    dst += blockIO->band;
    count = blockIO->pixelCountFR;
    skip = blockIO->cntl->nitf->numBands;
    
    for (i = 0; i < count; i++)
    {
        *dst = *(src++);
        dst += skip;
    }
    return;
}


void nitf_ImageIO_pack_P_2(_nitf_ImageIOBlock * blockIO, nitf_Error * error)
{
    nitf_Uint16 *src;           /* Source buffer */
    nitf_Uint16 *dst;           /* Destination buffer */
    size_t count;               /* Number of pixels to transfer */
    nitf_Uint32 skip;           /* Source buffer skip count */
    size_t i;
    
    src = (nitf_Uint16 *) (blockIO->user.buffer + blockIO->user.offset.mark);
    dst = (nitf_Uint16 *) (blockIO->rwBuffer.buffer);
    dst += blockIO->band;
    count = blockIO->pixelCountFR;
    skip = blockIO->cntl->nitf->numBands;
    
    for (i = 0; i < count; i++)
    {
        *dst = *(src++);
        dst += skip;
    }
    return;
}


void nitf_ImageIO_pack_P_4(_nitf_ImageIOBlock * blockIO, nitf_Error * error)
{
    nitf_Uint32 *src;           /* Source buffer */
    nitf_Uint32 *dst;           /* Destination buffer */
    size_t count;               /* Number of pixels to transfer */
    nitf_Uint32 skip;           /* Source buffer skip count */
    size_t i;
    
    src = (nitf_Uint32 *) (blockIO->user.buffer + blockIO->user.offset.mark);
    dst = (nitf_Uint32 *) (blockIO->rwBuffer.buffer);
    dst += blockIO->band;
    count = blockIO->pixelCountFR;
    skip = blockIO->cntl->nitf->numBands;
    
    for (i = 0; i < count; i++)
    {
        *dst = *(src++);
        dst += skip;
    }
    return;
}


void nitf_ImageIO_pack_P_8(_nitf_ImageIOBlock * blockIO, nitf_Error * error)
{
    nitf_Uint64 *src;           /* Source buffer */
    nitf_Uint64 *dst;           /* Destination buffer */
    size_t count;               /* Number of pixels to transfer */
    nitf_Uint32 skip;           /* Source buffer skip count */
    size_t i;
    
    src = (nitf_Uint64 *) (blockIO->user.buffer + blockIO->user.offset.mark);
    dst = (nitf_Uint64 *) (blockIO->rwBuffer.buffer);
    dst += blockIO->band;
    count = blockIO->pixelCountFR;
    skip = blockIO->cntl->nitf->numBands;
    
    for (i = 0; i < count; i++)
    {
        *dst = *(src++);
        dst += skip;
    }
    return;
}


void nitf_ImageIO_pack_P_16(_nitf_ImageIOBlock * blockIO, nitf_Error * error)
{
    nitf_Uint64 *src1;          /* Source buffer 1 */
    nitf_Uint64 *dst1;          /* Destination buffer 1 */
    nitf_Uint64 *src2;          /* Source buffer 2 */
    nitf_Uint64 *dst2;          /* Destination buffer 2 */
    size_t count;               /* Number of pixels to transfer */
    nitf_Uint32 skip;           /* Source buffer skip count */
    size_t i;
    
    src1 = (nitf_Uint64 *) (blockIO->user.buffer + blockIO->user.offset.mark);
    dst1 = (nitf_Uint64 *) (blockIO->rwBuffer.buffer);
    dst1 += blockIO->band;
    src2 = src1 + 1;
    dst2 = dst1 + 1;
    count = blockIO->pixelCountFR;
    skip = blockIO->cntl->nitf->numBands * 2;
    
    for (i = 0; i < count; i++)
    {
        *dst1 = *(src1++);
        *dst2 = *(src2++);
        dst1 += skip;
        dst2 += skip;
    }
    return;
}

void nitf_ImageIO_formatShift_1(nitf_Uint8 * buffer,
        size_t count, nitf_Uint32 shiftCount)
{
    nitf_Int8 shift;            /* Shift count */
    nitf_Int8 *bp8;             /* Buffer pointer, 8 bit */
    size_t i;
    
    shift = (nitf_Int8) shiftCount;
    bp8 = (nitf_Int8 *) buffer;
    for (i = 0; i < count; i++)
        *(bp8++) <<= shift;
    
    return;
}


void nitf_ImageIO_formatShift_2(nitf_Uint8 * buffer,
        size_t count, nitf_Uint32 shiftCount)
{
    nitf_Int16 shift;           /* Shift count */
    nitf_Int16 *bp16;           /* Buffer pointer, 16 bit */
    size_t i;
    
    shift = (nitf_Int16) shiftCount;
    bp16 = (nitf_Int16 *) buffer;
    for (i = 0; i < count; i++)
        *(bp16++) <<= shift;
    
    return;
}


void nitf_ImageIO_formatShift_4(nitf_Uint8 * buffer,
        size_t count, nitf_Uint32 shiftCount)
{
    nitf_Int32 shift;           /* Shift count */
    nitf_Int32 *bp32;           /* Buffer pointer, 32 bit */
    size_t i;
    
    shift = (nitf_Int32) shiftCount;
    bp32 = (nitf_Int32 *) buffer;
    for (i = 0; i < count; i++)
        *(bp32++) <<= shift;
    
    return;
}


/* Uses 64 bit types */

void nitf_ImageIO_formatShift_8(nitf_Uint8 * buffer,
        size_t count, nitf_Uint32 shiftCount)
{
    nitf_Int64 shift;           /* Shift count */
    nitf_Int64 *bp64;           /* Buffer pointer, 64 bit */
    size_t i;
    
    shift = (nitf_Int64) shiftCount;
    bp64 = (nitf_Int64 *) buffer;
    for (i = 0; i < count; i++)
        *(bp64++) <<= shift;
    
    return;
}

void nitf_ImageIO_formatMask_1(nitf_Uint8 * buffer,
        size_t count, nitf_Uint32 shiftCount)
{
    nitf_Uint8 mask;            /* The mask */
    nitf_Uint8 *bp8;            /* Buffer pointer, 8 bit */
    size_t i;
    
    mask = ((nitf_Uint8) - 1) << (8 - shiftCount);
    bp8 = (nitf_Uint8 *) buffer;
    for (i = 0; i < count; i++)
        *(bp8++) &= mask;
    
    return;
}

void nitf_ImageIO_formatMask_2(nitf_Uint8 * buffer,
        size_t count, nitf_Uint32 shiftCount)
{
    nitf_Uint8 mask;            /* The mask */
    nitf_Uint16 *bp16;          /* Buffer pointer, 16 bit */
    size_t i;
    
    mask = ((nitf_Uint16) - 1) << (16 - shiftCount);
    bp16 = (nitf_Uint16 *) buffer;
    for (i = 0; i < count; i++)
        *(bp16++) &= mask;
    
    return;
}


void nitf_ImageIO_formatMask_4(nitf_Uint8 * buffer,
        size_t count, nitf_Uint32 shiftCount)
{
    nitf_Uint8 mask;            /* The mask */
    nitf_Uint32 *bp32;          /* Buffer pointer, 32 bit */
    size_t i;
    
    mask = ((nitf_Uint32) - 1) << (32 - shiftCount);
    bp32 = (nitf_Uint32 *) buffer;
    for (i = 0; i < count; i++)
        *(bp32++) &= mask;
    
    return;
}


/* Uses 64 bit types */

void nitf_ImageIO_formatMask_8(nitf_Uint8 * buffer,
        size_t count, nitf_Uint32 shiftCount)
{
    nitf_Uint8 mask;            /* The mask */
    nitf_Uint64 *bp64;          /* Buffer pointer, 64 bit */
    size_t i;
    
    mask = ((nitf_Uint64) - 1) << (64 - shiftCount);
    bp64 = (nitf_Uint64 *) buffer;
    for (i = 0; i < count; i++)
        *(bp64++) &= mask;
    
    return;
}


void nitf_ImageIO_formatShiftSwap_2(nitf_Uint8 * buffer,
                                    size_t count,
                                    nitf_Uint32 shiftCount)
{
    nitf_Int16 shift;           /* Shift count */
    nitf_Uint8 *bp8;            /* Buffer pointer, 8 bit */
    nitf_Int16 *bp16;           /* Buffer pointer, 16 bit */
    nitf_Uint8 tmp8;            /* Temp value, 8 bit */
    size_t i;
    
    shift = (nitf_Int16) shiftCount;
    bp16 = (nitf_Int16 *) buffer;
    for (i = 0; i < count; i++)
    {
        bp8 = (nitf_Uint8 *) bp16;
        tmp8 = bp8[0];
        bp8[0] = bp8[1];
        bp8[1] = tmp8;
        
        *(bp16++) >>= shift;
    }
    
    return;
}


void nitf_ImageIO_formatShiftSwap_4(nitf_Uint8 * buffer,
                                    size_t count,
                                    nitf_Uint32 shiftCount)
{
    nitf_Int32 shift;           /* Shift count */
    nitf_Uint8 *bp8;            /* Buffer pointer, 8 bit */
    nitf_Int32 *bp32;           /* Buffer pointer, 32 bit */
    nitf_Uint8 tmp8;            /* Temp value, 8 bit */
    size_t i;
    
    shift = (nitf_Int32) shiftCount;
    bp32 = (nitf_Int32 *) buffer;
    for (i = 0; i < count; i++)
    {
        bp8 = (nitf_Uint8 *) bp32;
        
        tmp8 = bp8[0];
        bp8[0] = bp8[3];
        bp8[3] = tmp8;
        tmp8 = bp8[1];
        bp8[1] = bp8[2];
        bp8[2] = tmp8;
        
        *(bp32++) >>= shift;
    }
    
    return;
}


/* Uses 64 bit types */
void nitf_ImageIO_formatShiftSwap_8(nitf_Uint8 * buffer,
                                    size_t count,
                                    nitf_Uint32 shiftCount)
{
    nitf_Int64 shift;           /* Shift count */
    nitf_Uint8 *bp8;            /* Buffer pointer, 8 bit */
    nitf_Int64 *bp64;           /* Buffer pointer, 64 bit */
    nitf_Uint8 tmp8;            /* Temp value, 8 bit */
    size_t i;
    
    shift = (nitf_Int64) shiftCount;
    bp64 = (nitf_Int64 *) buffer;
    for (i = 0; i < count; i++)
    {
        bp8 = (nitf_Uint8 *) bp64;
        
        tmp8 = bp8[0];
        bp8[0] = bp8[7];
        bp8[7] = tmp8;
        tmp8 = bp8[1];
        bp8[1] = bp8[6];
        bp8[6] = tmp8;
        tmp8 = bp8[2];
        bp8[2] = bp8[5];
        bp8[5] = tmp8;
        tmp8 = bp8[3];
        bp8[3] = bp8[4];
        bp8[4] = tmp8;

        *(bp64++) >>= shift;
    }
    
    return;
}


void nitf_ImageIO_formatMaskSwap_2(nitf_Uint8 * buffer,
                                   size_t count,
                                   nitf_Uint32 shiftCount)
{
    nitf_Uint8 mask;            /* The mask */
    nitf_Uint8 *bp8;            /* Buffer pointer, 8 bit */
    nitf_Uint16 *bp16;          /* Buffer pointer, 16 bit */
    nitf_Uint8 tmp8;            /* Temp value, 8 bit */
    size_t i;
    
    mask = ((nitf_Uint16) - 1) << (16 - shiftCount);
    bp16 = (nitf_Uint16 *) buffer;
    for (i = 0; i < count; i++)
    {
        *(bp16++) &= mask;
        
        bp8 = (nitf_Uint8 *) bp16;
        tmp8 = bp8[0];
        bp8[0] = bp8[1];
        bp8[1] = tmp8;
    }
    
    return;
}


void nitf_ImageIO_formatMaskSwap_4(nitf_Uint8 * buffer,
                                   size_t count,
                                   nitf_Uint32 shiftCount)
{
    nitf_Uint8 mask;            /* The mask */
    nitf_Uint8 *bp8;            /* Buffer pointer, 8 bit */
    nitf_Uint32 *bp32;          /* Buffer pointer, 32 bit */
    nitf_Uint8 tmp8;            /* Temp value, 8 bit */
    size_t i;
    
    mask = ((nitf_Uint32) - 1) << (32 - shiftCount);
    bp32 = (nitf_Uint32 *) buffer;
    for (i = 0; i < count; i++)
    {
        bp8 = (nitf_Uint8 *) bp32;
        
        *(bp32++) &= mask;
        
        tmp8 = bp8[0];
        bp8[0] = bp8[3];
        bp8[3] = tmp8;
        tmp8 = bp8[1];
        bp8[1] = bp8[2];
        bp8[2] = tmp8;
    }
    
    return;
}


/* Uses 64 bit types */
void nitf_ImageIO_formatMaskSwap_8(nitf_Uint8 * buffer,
                                   size_t count,
                                   nitf_Uint32 shiftCount)
{
    nitf_Uint8 mask;            /* The mask */
    nitf_Uint8 *bp8;            /* Buffer pointer, 8 bit */
    nitf_Uint64 *bp64;          /* Buffer pointer, 64 bit */
    nitf_Uint8 tmp8;            /* Temp value, 8 bit */
    size_t i;
    
    mask = ((nitf_Uint64) - 1) << (64 - shiftCount);
    bp64 = (nitf_Uint64 *) buffer;
    for (i = 0; i < count; i++)
    {
        bp8 = (nitf_Uint8 *) bp64;
        
        *(bp64++) &= mask;
        
        tmp8 = bp8[0];
        bp8[0] = bp8[7];
        bp8[7] = tmp8;
        tmp8 = bp8[1];
        bp8[1] = bp8[6];
        bp8[6] = tmp8;
        tmp8 = bp8[2];
        bp8[2] = bp8[5];
        bp8[5] = tmp8;
        tmp8 = bp8[3];
        bp8[3] = bp8[4];
        bp8[4] = tmp8;
    }
    
    return;
}


/*============================================================================*/
/*======================== B pixel type psuedo decompressor ==================*/
/*============================================================================*/

NITFPRIV(NITF_BOOL) nitf_ImageIO_bPixelFreeBlock(nitf_DecompressionControl
        * control,
        nitf_Uint8 * block,
        nitf_Error * error)
{
    NITF_FREE(block);
    return NITF_SUCCESS;
}

NITFPRIV(nitf_DecompressionControl *) 
nitf_ImageIO_bPixelOpen(nitf_IOInterface* io,
                        nitf_Uint64 offset,
                        nitf_Uint64 fileLength,
                        nitf_BlockingInfo* blockInfo,
                        nitf_Uint64 *blockMask,
                        nitf_Error *error)
{
    nitf_ImageIO_BPixelControl *icntl;
    icntl =
        (nitf_ImageIO_BPixelControl *)
        NITF_MALLOC(sizeof(nitf_ImageIO_BPixelControl));
    if (icntl == NULL)
    {
        nitf_Error_init(error, "Error creating control object",
                        NITF_CTXT, NITF_ERR_DECOMPRESSION);
        return NULL;
    }

    icntl->io = io;
    icntl->offset = offset;
    icntl->blockInfo = blockInfo;
    icntl->blockMask = blockMask;
    icntl->blockSizeCompressed = (blockInfo->length + 7) / 8;
    icntl->buffer = (nitf_Uint8 *) NITF_MALLOC(icntl->blockSizeCompressed);
    if (icntl->buffer == NULL)
    {
        nitf_Error_init(error, "Error creating control object",
                        NITF_CTXT, NITF_ERR_DECOMPRESSION);
        NITF_FREE(icntl);
        return NULL;
    }

    return (nitf_DecompressionControl *) icntl;
}


NITFPRIV(nitf_Uint8 *)
nitf_ImageIO_bPixelReadBlock(nitf_DecompressionControl * control,
                             nitf_Uint32 blockNumber, nitf_Error * error)
{
    /* Actual control type */
    nitf_ImageIO_BPixelControl *icntl;
    size_t uncompressedLen;     /* Length of uncompressed block */
    nitf_Uint8 *block;          /* Uncompressed result */
    nitf_Uint8 *blockPtr;       /* Pointer in uncompressed result */
    nitf_Uint8 *compPtr;        /* Pointer in compressed input */
    nitf_Uint8 current;         /* Current byte of compressed data */
    size_t i;
    
    icntl = (nitf_ImageIO_BPixelControl *) control;
    uncompressedLen = icntl->blockInfo->length;
    
    /* Read the data */
    
    if (!NITF_IO_SUCCESS(nitf_IOInterface_seek(icntl->io,
                                               (nitf_Off) (icntl->offset +
                                                           icntl->
                                                           blockMask
                                                           [blockNumber]),
                                               NITF_SEEK_SET, error)))
        return NULL;
    
    if (!nitf_IOInterface_read(icntl->io,
                               (char *) (icntl->buffer),
                               icntl->blockSizeCompressed, error))
        return NULL;
    
    /* Allocate block */
    
    block = (nitf_Uint8 *) NITF_MALLOC(uncompressedLen);
    if (block == NULL)
    {
        nitf_Error_init(error, "Error creating block buffer",
                        NITF_CTXT, NITF_ERR_DECOMPRESSION);
        return NULL;
    }

    /* Decompress the result */

    blockPtr = block;
    compPtr = icntl->buffer;
    current = 0;                /* Avoids uninitialized variable warning */
    for (i = 0; i < uncompressedLen; i++)
    {
        if (i % 8 == 0)
            current = *(compPtr++);
        *(blockPtr++) = (current & 0x80) ? 1 : 0;
        current <<= 1;
    }

    return block;
}


NITFPRIV(void) 
nitf_ImageIO_bPixelClose(nitf_DecompressionControl **control)
{
    nitf_ImageIO_BPixelControl *icntl;
    icntl = (nitf_ImageIO_BPixelControl *) * control;
    
    if (icntl->buffer != NULL)
        NITF_FREE((void *) (icntl->buffer));
    NITF_FREE((void *) (icntl));
    *control = NULL;
    return;
}


/*============================================================================*/
/*======================== 12-bit pixel type psuedo decompressor =============*/
/*============================================================================*/

NITFPRIV(NITF_BOOL) nitf_ImageIO_12PixelFreeBlock(nitf_DecompressionControl
        * control,
        nitf_Uint8 * block,
        nitf_Error * error)
{
    NITF_FREE(block);
    return NITF_SUCCESS;
}

NITFPRIV(nitf_DecompressionControl *)
nitf_ImageIO_12PixelOpen(nitf_IOInterface* io,
                        nitf_Uint64 offset,
                        nitf_Uint64 fileLength,
                        nitf_BlockingInfo* blockInfo,
                        nitf_Uint64 *blockMask,
                        nitf_Error *error)
{
    nitf_ImageIO_12PixelControl *icntl;

    icntl =
        (nitf_ImageIO_12PixelControl *)
        NITF_MALLOC(sizeof(nitf_ImageIO_12PixelControl));
    if (icntl == NULL)
    {
        nitf_Error_init(error, "Error creating control object",
                        NITF_CTXT, NITF_ERR_DECOMPRESSION);
        return NULL;
    }

    icntl->io = io;
    icntl->offset = offset;
    icntl->blockInfo = blockInfo;
    icntl->blockMask = blockMask;
    icntl->blockPixelCount = blockInfo->length/2;
    if(icntl->blockPixelCount & 1)   /* Odd number of pixels in block */
      icntl->odd = 1;
    else                        /* Even number of pixels in block */
      icntl->odd = 0;
/*
    The 12-bit pixel arrangement puts 2 pixels (24 bits) in three bytes.
    If the pixel count is odd the last pixel is packed into 2 bytes. So
    The calculation of the size of the compress block has to consider whether
    the pixel count is odd or even
*/

    icntl->blockSizeCompressed = 3*(icntl->blockPixelCount/2) + 2*(icntl->odd);

    icntl->buffer = (nitf_Uint8 *) NITF_MALLOC(icntl->blockSizeCompressed);
    if (icntl->buffer == NULL)
    {
        nitf_Error_init(error, "Error creating control object",
                        NITF_CTXT, NITF_ERR_DECOMPRESSION);
        NITF_FREE(icntl);
        return NULL;
    }

    return (nitf_DecompressionControl *) icntl;
}

NITFPRIV(nitf_Uint8 *)
nitf_ImageIO_12PixelReadBlock(nitf_DecompressionControl * control,
                             nitf_Uint32 blockNumber, nitf_Error * error)
{
    /* Actual control type */
    nitf_ImageIO_12PixelControl *icntl;
    size_t uncompressedLen;        /* Length of uncompressed block */
    nitf_Uint8 *block;             /* Uncompressed result */
    nitf_Uint16 *blockPtr;         /* Pointer in uncompressed result */
    nitf_Uint8 *compPtr;           /* Pointer in compressed input */
    nitf_Uint16 a;                 /* Components of compressed pixel */
    nitf_Uint16 b;
    nitf_Uint16 c;
    size_t i;

    icntl = (nitf_ImageIO_12PixelControl *) control;
    uncompressedLen = icntl->blockInfo->length;

    /* Read the data */

    if (!NITF_IO_SUCCESS(nitf_IOInterface_seek(icntl->io,
                 (nitf_Off) (icntl->offset + icntl->blockMask[blockNumber]),
                                               NITF_SEEK_SET, error)))
        return NULL;

    if (!nitf_IOInterface_read(icntl->io,
                               (char *) (icntl->buffer),
                               icntl->blockSizeCompressed, error))
        return NULL;

    /* Allocate block */

    block = (nitf_Uint8 *) NITF_MALLOC(uncompressedLen);
    if (block == NULL)
    {
        nitf_Error_init(error, "Error creating block buffer",
                        NITF_CTXT, NITF_ERR_DECOMPRESSION);
        return NULL;
    }

    /* Decompress the result */

    blockPtr = (nitf_Uint16 *) block;
    compPtr = icntl->buffer;
    for (i = 0; i < icntl->blockPixelCount/2; i++)
    {
      a = *(compPtr++);
      b = *(compPtr++);
      c = *(compPtr++);

      *(blockPtr++) = (a << 4) + (b >> 4);
      *(blockPtr++) = ((b << 8) & 0xf00) + c;
    }

    if(icntl->odd)   /* Look for odd count and handle last pixel */
    {
      a = *(compPtr++);
      b = *(compPtr++);

      *(blockPtr++) = (a << 4) + (b >>4);
    }

    return block;
}


NITFPRIV(void)
nitf_ImageIO_12PixelClose(nitf_DecompressionControl **control)
{
    nitf_ImageIO_12PixelControl *icntl;
    icntl = (nitf_ImageIO_12PixelControl *) * control;

    if (icntl->buffer != NULL)
        NITF_FREE((void *) (icntl->buffer));
    NITF_FREE((void *) (icntl));
    *control = NULL;
    return;
}

/*============================================================================*/
/*======================== 12-bit pixel type psuedo compressor =============*/
/*============================================================================*/

nitf_CompressionControl  *nitf_ImageIO_12PixelComOpen
( nitf_ImageSubheader * subheader,nitf_Error * error)
{
  nitf_ImageIO_12PixelComControl *icntl;   /* The result */
  nitf_Uint32 numRowsPerBlock;      /* Number of rows per block */
  nitf_Uint32 numColumnsPerBlock;   /* Number of columns per block */
  nitf_Uint32 numBands, xBands;     /* Number of bands */

  icntl =
      (nitf_ImageIO_12PixelComControl *)
        NITF_MALLOC(sizeof(nitf_ImageIO_12PixelComControl));
  if (icntl == NULL)
  {
    nitf_Error_init(error, "Error creating control object",
                                          NITF_CTXT, NITF_ERR_COMPRESSION);
    return(NULL);
  }

/* Get values from the subheader, need block dimensions */

  NITF_TRY_GET_UINT32(subheader->numImageBands, &numBands, error);
  NITF_TRY_GET_UINT32(subheader->numMultispectralImageBands, &xBands, error);
  numBands += xBands;
  NITF_TRY_GET_UINT32(subheader->numPixelsPerVertBlock,&numRowsPerBlock, error);
  NITF_TRY_GET_UINT32(subheader->numPixelsPerHorizBlock,&numColumnsPerBlock,
                        error);

/* Does not work for S mode which is not supported */
  icntl->blockPixelCount = (size_t)numRowsPerBlock*numColumnsPerBlock*numBands;
  icntl->odd = icntl->blockPixelCount & 1;
  icntl->blockSizeCompressed = 3*(icntl->blockPixelCount/2) + 2*(icntl->odd);
  icntl->blockSizeUncompressed = icntl->blockPixelCount*2;
  icntl->buffer = NITF_MALLOC(icntl->blockSizeCompressed);
  if(icntl->buffer == NULL)
  {
    nitf_Error_init(error, "Error creating control object",
                                          NITF_CTXT, NITF_ERR_COMPRESSION);
    NITF_FREE(icntl);
    return(NULL);
  }


  return((nitf_CompressionControl *) icntl);

CATCH_ERROR:
    NITF_FREE(icntl);
    return NULL;
}

NITF_BOOL nitf_ImageIO_12PixelComStart
( nitf_CompressionControl *object,
  nitf_Uint64 offset,
  nitf_Uint64 dataLength,
  nitf_Uint64 * blockMask,
  nitf_Uint64 * padMask,
  nitf_Error * error)
{
  nitf_ImageIO_12PixelComControl *icntl;  /* The internal data structure */

  icntl = (nitf_ImageIO_12PixelComControl *) object;
  icntl->io = NULL;
  icntl->offset = offset;
  icntl->dataLength = dataLength;
  icntl->offset = offset;
  icntl->blockMask = blockMask;
  icntl->padMask = padMask;
  icntl->buffer = NULL;
  icntl->written = 0;

/* Allocate compressed block buffer */

  icntl->buffer = (nitf_Uint8 *) NITF_MALLOC(icntl->blockSizeCompressed);
  if(icntl->buffer == NULL)
    return(NITF_FAILURE);

  return(NITF_SUCCESS);
}

NITF_BOOL nitf_ImageIO_12PixelComWriteBlock
( nitf_CompressionControl * object,
  nitf_IOInterface* io,
  nitf_Uint8 *data,
  NITF_BOOL pad,
  NITF_BOOL noData,
  nitf_Error *error)
{
  nitf_ImageIO_12PixelComControl *icntl;  /* The internal data structure */
  size_t pairs;                /* Number of pixel pairs */
  nitf_Uint16 *dp;             /* Pointer into input buffer */
  nitf_Uint8 *bp;              /* Pointer into output buffer */
  nitf_Uint16 i1;              /* First pixel in input pair */
  nitf_Uint16 i2;              /* Second pixel in input pair */
  nitf_Off fileOffset;         /* File offset for write */
  size_t i;

  icntl = (nitf_ImageIO_12PixelComControl *) object;

  icntl->io = io;

/* Compress block into buffer */

  pairs = icntl->blockPixelCount/2;
  bp = icntl->buffer;
  dp = (nitf_Uint16 *) data;
  for(i=0;i<pairs;i++)
  {
    i1 = *(dp++);
    i2 = *(dp++);

    *(bp++) = (i1 >> 4) & 0xff;
    *(bp++) = ((i1 & 0x0f) << 4) + ((i2 >> 8) & 0x0f);
    *(bp++) = i2 & 0xff;
  }

  if(icntl->odd)  /* Handle last pixel in odd block length case */
  {
    i1 = *dp;
    *(bp++) = (i1 >> 4) & 0xff;
    *(bp++) = (i1 & 0x0f) << 4;
  }

/* Do the write */

  fileOffset = icntl->offset + icntl->written;
  if(!NITF_IO_SUCCESS(
            nitf_IOInterface_seek(io,fileOffset,NITF_SEEK_SET,error)))
    return NITF_FAILURE;


   if(!nitf_IOInterface_write(io,(char *) (icntl->buffer),
                                    icntl->blockSizeCompressed, error))
     return NITF_FAILURE;

  icntl->written += icntl->blockSizeCompressed;

  return(NITF_SUCCESS);
}

NITF_BOOL nitf_ImageIO_12PixelComEnd
( nitf_CompressionControl * object,nitf_IOInterface* io, nitf_Error *error)
{

  return(NITF_SUCCESS);
}

void nitf_ImageIO_12PixelComDestroy(nitf_CompressionControl ** object)
{
  nitf_ImageIO_12PixelComControl *icntl;  /* The internal data structure */

  icntl = (nitf_ImageIO_12PixelComControl *) object;

   if(object != NULL)
   {
     icntl = *((nitf_ImageIO_12PixelComControl **) object);
     if(icntl != NULL)
     {
       if(icntl->buffer != NULL)
         NITF_FREE(icntl->buffer);
       NITF_FREE(icntl);
     }
     *object = NULL;
   }
  return;
}

/*============================================================================*/
/*======================== Diagnostic Functions ===============================*/
/*============================================================================*/

/*========================= nitf_ImageIO_print ===============================*/

NITFPRIV(void) nitf_ImageIO_print(nitf_ImageIO * nitf, FILE * file)
{
#ifdef NITF_DEBUG
    /* Correct type for pointer */
    _nitf_ImageIO *nitfp = (_nitf_ImageIO *) nitf;
    int i;
    
    if (file == NULL)
        file = stdout;
    
    fprintf(file, "nitf_ImageIO structure:\n");
    fprintf(file, "  Number of rows: %ld\n", nitfp->numRows);
    fprintf(file, "  Number of columns: %ld\n", nitfp->numColumns);
    fprintf(file, "  Number of bands: %ld\n", nitfp->numBands);
    
    fprintf(file, "  Pixel definition:\n");
    fprintf(file, "    Pixel type code: %08lx\n", nitfp->pixel.type);
    fprintf(file, "    Pixel type size in bytes: %08lx\n",
            nitfp->pixel.bytes);
    fprintf(file, "    Pad value: ");
    for (i = 0; i < NITF_IMAGE_IO_PAD_MAX_LENGTH; i++)
        fprintf(file, "%02x ", nitfp->pixel.pad[i]);
    fprintf(file, "\n");
    fprintf(file, "    Byte swap if TRUE: %d\n", nitfp->pixel.swap);
    fprintf(file, "    Shift count for left justified data: %ld\n",
            nitfp->pixel.shift);
    fprintf(file, "  Number of blocks per row: %ld\n", nitfp->nBlocksPerRow);
    fprintf(file, "  Number of blocks per column: %ld\n",
            nitfp->nBlocksPerColumn);
    fprintf(file, "  Number of rows per block: %ld\n", nitfp->numRowsPerBlock);
    fprintf(file, "  Number of columns per block: %ld\n",
            nitfp->numColumnsPerBlock);
    fprintf(file, "  Block size in bytes: %ld\n", nitfp->blockSize);
    fprintf(file, "  Total number of blocks: %ld\n", nitfp->nBlocksTotal);
    fprintf(file, "  Actual number of rows: %ld\n", nitfp->numRowsActual);
    fprintf(file, "  Actual number of columns: %ld\n",
            nitfp->numColumnsActual);
    fprintf(file, "  Compression type code: %08lx\n", nitfp->compression);
    fprintf(file, "  Blocking mode code: %08lx\n", nitfp->blockingMode);
    fprintf(file, "  File offset to image data section: %llx\n",
            nitfp->imageBase);
    fprintf(file, "  File offset to pixel data: %llx\n", nitfp->pixelBase);
    fprintf(file, "  Image data length including masks: %llx\n",
            nitfp->dataLength);
    fprintf(file, "  Parameters:\n");
    fprintf(file,
            "     Request/Block size threshold for no caching: %lf\n",
            nitfp->parameters.noCacheThreshold);
    fprintf(file, "     Clear cache after I/O operation: %ld\n",
            nitfp->parameters.clearCache);
    fprintf(file, "  Block and pad mask header:\n");
    fprintf(file, "    Ready flag %d\n", nitfp->maskHeader.ready);
    fprintf(file, "    Offset to actual image data past masks: %lx\n",
            nitfp->maskHeader.imageDataOffset);
    fprintf(file, "    Block mask record length: %x\n",
            nitfp->maskHeader.blockRecordLength);
    fprintf(file, "    Pad pixel mask record length: %x\n",
            nitfp->maskHeader.padRecordLength);
    fprintf(file, "    Pad pixel value length in bytes: %x\n",
            nitfp->maskHeader.padPixelValueLength);
    fprintf(file, "  Read/write one band at a time if TRUE: %d\n",
            nitfp->oneBand);
    
    fflush(file);
#endif
    return;
}


NITFPRIV(void) nitf_ImageIOControl_print(_nitf_ImageIOControl * cntl,
                                         FILE * file, int full)
{
#ifdef NITF_DEBUG
    nitf_Uint32 i;
    
    if (file == NULL)
        file = stdout;
    
    fprintf(file, "nitf_ImageIO control structure:\n");
    fprintf(file, "  Number of row in the sub-window: %ld\n", cntl->numRows);
    fprintf(file, "  Start row in the main image: %ld\n", cntl->row);
    fprintf(file, "  Row skip factor: %ld\n", cntl->rowSkip);
    fprintf(file, "  Number of columns in the sub-window: %ld\n",
            cntl->numColumns);
    fprintf(file, "  Start column in the main image: %ld\n", cntl->column);
    fprintf(file, "  Column skip factor: %ld\n", cntl->columnSkip);
    fprintf(file, "  Down-smapling flag: %ld\n", cntl->downSampling);
    fprintf(file, "  Number of bands to read/write: %ld\n",
            cntl->numBandSubset);
    fprintf(file, "  Array of bands to read/write:\n");
    for (i = 0; i < cntl->numBandSubset; i++)
        fprintf(file, " %ld", cntl->bandSubset[i]);
    fprintf(file, "\n");
    fprintf(file, "  Operation is read if TRUE: %d\n", cntl->reading);
    fprintf(file, "  Number of _nitf_ImageIOBlock structures: %ld\n",
            cntl->nBlockIO);
    if (full)
    {
        for (i = 0; i < cntl->nBlockIO; i++)
            nitf_ImageIOBlock_print(&(cntl->blockIO[0][0]) + i, file, 1);
    }
    fprintf(file, "  Block number next row increment: %ld\n",
            cntl->numberInc);
    fprintf(file,
            "  Data's offset in block next row byte increment: %ld\n",
            cntl->blockOffsetInc);
    fprintf(file, "  Read/write buffer next row byte increment: %ld\n",
            cntl->bufferInc);
    fprintf(file, "  User data buffer next row byte increment: %ld\n",
            cntl->userInc);
    fprintf(file, "  Pad pixel buffer size in bytes: %ld\n",
            cntl->padBufferSize);
    fprintf(file, "  Pad pixels read fla: %d\n", cntl->padded);
    fprintf(file, "  Total I/O count: %ld I/O count down : %ld\n",
            cntl->ioCount, cntl->ioCountDown);
    
    fflush(file);
#endif
    return;
}

NITFPRIV(void) nitf_ImageIOBlock_print(_nitf_ImageIOBlock * blockIO,
                                       FILE * file, int longIndent)
{
#ifdef NITF_BLOCK_DEBUG
    char *sp;
    
    if (file == NULL)
        file = stdout;
    
    if (longIndent)
        sp = "    ";
    else
        sp = "  ";
    
    if (!longIndent)
        fprintf(file, "Block IO control structure:\n");
    
    fprintf(file, "%sAssociated nitf_ImageIOControl object:\n", sp);
    fprintf(file, "%sBand associated width this I/O: %ld\n", sp,
            blockIO->band);
    fprintf(file, "%sDo the read/write if TRUE: %d\n", sp, blockIO->doIO);
    fprintf(file, "%sBlock number: %ld\n", sp, blockIO->number);
    fprintf(file, "  Rows until next block: %ld\n", blockIO->rowsUntil);
    fprintf(file, "%sBlock's offset from image base: %lx\n",
            sp, blockIO->imageDataOffset);
    fprintf(file, "%sData's base byte offset in block: %ld\n",
            sp, blockIO->blockOffset.orig);
    fprintf(file, "%sData's current byte offset in block: %ld\n",
            sp, blockIO->blockOffset.mark);
    fprintf(file, "%sRead count in bytes: %ld\n", sp, blockIO->readCount);
    /*fprintf(file, "%sOffset into read/write buffer: %ld\n", sp,
            blockIO->bufferOffset);
    fprintf(file, "%sOriginal offset into read/write buffer: %ld\n", sp,
            blockIO->bufferOffsetOrg);
    fprintf(file, "%sOffset into user buffer: %ld\n", sp,
            blockIO->userOffset);
    fprintf(file, "%sOriginal offset into user buffer: %ld\n", sp,
            blockIO->userOffsetOrg);*/
    fprintf(file, "%sRead/write buffer is user buffer if TRUE: %ld\n", sp,
            blockIO->userEqBuffer);
    fprintf(file,
            "%sPixel count in this operation, full resolution: %ld\n", sp,
            blockIO->pixelCountFR);
    fprintf(file,
            "%sPixel count in this operation, down-sample resolution: %ld\n",
            sp, blockIO->pixelCountDR);
    fprintf(file, "%sFormat count in this operation: %ld\n", sp,
            blockIO->formatCount);
    fprintf(file, "%sPad pixel column write byte count: %ld\n", sp,
            blockIO->padColumnCount);
    fprintf(file, "%sPad pixel row write count: %ld\n", sp,
            blockIO->padRowCount);
    fprintf(file, "%sStart column of the first new sample window: %ld\n",
            sp, blockIO->sampleStartColumn);
    fprintf(file, "%sPartial sample columns previous block: %ld\n", sp,
            blockIO->residual);
    fprintf(file, "%sPartial sample columns current block: %ld\n", sp,
            blockIO->myResidual);
    fprintf(file, "%sCurrent row in the image: %ld\n", sp,
            blockIO->currentRow);
    
    fflush(file);
#endif
    return;
}

NITFAPI(NITF_BOOL) nitf_ImageIO_getMaskInfo(nitf_ImageIO *nitf,
                                            nitf_Uint32 *imageDataOffset, 
                                            nitf_Uint32 *blockRecordLength,
                                            nitf_Uint32 *padRecordLength, 
                                            nitf_Uint32 *padPixelValueLength,
                                            nitf_Uint8 **padValue, 
                                            nitf_Uint64 **blockMask, 
                                            nitf_Uint64 **padMask)
{
    _nitf_ImageIO *initf = (_nitf_ImageIO *) nitf;
    
    if ((initf->compression &    /* Look for no mask */
         (NITF_IMAGE_IO_COMPRESSION_NM
          | NITF_IMAGE_IO_COMPRESSION_M1
          | NITF_IMAGE_IO_COMPRESSION_M3
          | NITF_IMAGE_IO_COMPRESSION_M4
          | NITF_IMAGE_IO_COMPRESSION_M5
          | NITF_IMAGE_IO_COMPRESSION_M8)) == 0 /* No masks */ )
        return NITF_FAILURE;
    
    *imageDataOffset = initf->maskHeader.imageDataOffset;
    *blockRecordLength = initf->maskHeader.blockRecordLength;
    *padRecordLength = initf->maskHeader.padRecordLength;
    *padPixelValueLength = initf->maskHeader.padPixelValueLength;
    *padValue = initf->pixel.pad;
    *blockMask = initf->blockMask;
    *padMask = initf->padMask;

    return NITF_SUCCESS;
}

