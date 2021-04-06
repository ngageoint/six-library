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

#include <import/nitf.h>
#include <jpeglib.h>
#include <jerror.h>
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#if defined(WIN32) || defined(_WIN32)
    #include <Winsock2.h>
#else
    #include <netinet/in.h>
#endif
#define _BLOCK_SIZE(BLOCK) (BLOCK->rows * BLOCK->cols * BLOCK->bands)

/* borrowed from ImageIO.c */
#ifndef NITF_IMAGE_IO_NO_BLOCK
#   define NITF_IMAGE_IO_NO_BLOCK             ((uint32_t) 0xffffffff)
#endif

#define INPUT_BUF_SIZE  4096

/*
      Zero Block enable

  This plugin provides the option not returning an error when decopression
  falis for a block. If enbabled, this option causes the block reader to
  return a block of zeros. A sub-option causes a diagnostic print on stderr

  Both of these options are controlled by defines because the current
  decompression interface does not allow dynamic specification of this
  option or the returning of warnings

*/
/* Enable zero blocks if defined */
/*#define ZERO_BLOCK 1*/
/* Enable zero block warning if defined */
/*#define ZERO_BLOCK_WARN 1*/

/*    Macros for debug prints.

 The "A#" versions takes printf arguments beyond the format
*/

#if NITF_DEBUG_PLUGIN_REG
#define DPRINT(fmt) fprintf(stdout,fmt)
#define DPRINTA1(fmt,a1) fprintf(stdout,fmt,a1)
#define DPRINTA2(fmt,a1,a2) fprintf(stdout,fmt,a1,a2)
#define DPRINTA3(fmt,a1,a2,a3) fprintf(stdout,fmt,a1,a2,a3)
#else
#define DPRINT(fmt)
#define DPRINTA1(fmt,a1)
#define DPRINTA2(fmt,a1,a2)
#define DPRINTA3(fmt,a1,a2,a3)
#endif

NITF_CXX_GUARD

static int Q1[] =
    {
        /* C3 Quality 1 (9.9000) */
        8,   72,  72,  72,  72,  72,  72,  72,
        72,  72,  78,  74,  76,  74,  78,  89,
        81,  84,  84,  81,  89,  106, 93,  94,
        99,  94,  93,  106, 129, 111, 108, 116,
        116, 108, 111, 129, 135, 128, 136, 145,
        136, 128, 135, 155, 160, 177, 177, 160,
        155, 193, 213, 228, 213, 193, 255, 255,
        255, 255, 255, 255, 255, 255, 255, 255
    };

static int Q2[] =
    {
        /* C3 Quality 2 (5.0000)  */
        8,   36,  36,  36,  36,  36,  36,  36,
        36,  36,  39,  37,  38,  37,  39,  45,
        41,  42,  42,  41,  45,  53,  47,  47,
        50,  47,  47,  53,  65,  56,  54,  59,
        59,  54,  56,  65,  68,  64,  69,  73,
        69,  64,  68,  78,  81,  89,  89,  81,
        78,  98,  108, 115, 108, 98,  130, 144,
        144, 130, 178, 190, 178, 243, 243, 255
    };

static int Q3[] =
    {
        /* C3 Quality 3 (1.4000)  */
        8,   10,  10,  10,  10,  10,  10,  10,
        10,  10,  11,  10,  11,  10,  11,  13,
        11,  12,  12,  11,  13,  15,  13,  13,
        14,  13,  13,  15,  18,  16,  15,  16,
        16,  15,  16,  18,  19,  18,  19,  21,
        19,  18,  19,  22,  23,  25,  25,  23,
        22,  27,  30,  32,  30,  27,  36,  40,
        40,  36,  50,  53,  50,  68,  68,  91
    };

static int Q4[] =
    {
        /* C3 Quality 4 (1.0000)  */
        8,   7,   7,   7,   7,   7,   7,   7,
        7,   7,   8,   7,   8,   7,   8,   9,
        8,   8,   8,   8,   9,   11,  9,   9,
        10,  9,   9,   11,  13,  11,  11,  12,
        12,  11,  11,  13,  14,  13,  14,  15,
        14,  13,  14,  16,  16,  18,  18,  16,
        16,  20,  22,  23,  22,  20,  26,  29,
        29,  26,  36,  38,  36,  49,  49,  65
    };

static int Q5[] =
    {
        /* C3 Quality 5 (0.5519)  */
        4,   4,   4,   4,   4,   4,   4,   4,
        4,   4,   4,   4,   4,   4,   4,   5,
        5,   5,   5,   5,   5,   6,   5,   5,
        6,   5,   5,   6,   7,   6,   6,   6,
        6,   6,   6,   7,   8,   7,   8,   8,
        8,   7,   8,   9,   9,   10,  10,  9,
        9,   11,  12,  13,  12,  11,  14,  16,
        16,  14,  20,  21,  20,  27,  27,  36
    };

#define JPEGMarkerItem_name_LEN 4
typedef struct _JPEGMarkerItem
{
    char name[JPEGMarkerItem_name_LEN];
    nitf_Off off;
    uint32_t block;

}
JPEGMarkerItem;

NITFPRIV(JPEGMarkerItem*) JPEGMarkerItem_construct(nitf_Error* error)
{
    JPEGMarkerItem* item = (JPEGMarkerItem*)NITF_MALLOC(sizeof(JPEGMarkerItem));
    if (! item )
    {
        nitf_Error_init(error, NITF_STRERROR( NITF_ERRNO ),
                NITF_CTXT, NITF_ERR_DECOMPRESSION);
        return NULL;
    }
    memset(item->name, 0, 4);
    item->off = 0;
    return item;
}

/* not used - the list destructor will NITF_FREE these */
NITFPRIV(void) JPEGMarkerItem_destruct(JPEGMarkerItem** item)
{
    if (*item)
    {
        NITF_FREE( *item );
        *item = NULL;
    }
}

/* Added for diagnostics */

NITFPRIV(void) JPEGMarkerItem_print(
        JPEGMarkerItem* item, FILE *file)
{
    if (file == NULL)
    {
        file = stdout;
    }

    fprintf(file, "JPEG Marker: %s %lld\n", item->name, (long long)item->off);
    return;
}

NITFPRIV(void) JPEGMarkerItem_printList(
        nitf_List* markerList, FILE *file)
{
    nitf_ListIterator x; /* Iterator for walking list */
    nitf_ListIterator end; /* End of list iterator */
    JPEGMarkerItem* item; /* Current marker */

    end = nitf_List_end(markerList);
    for (x = nitf_List_begin(markerList);
            nitf_ListIterator_notEqualTo(&x, &end);
            nitf_ListIterator_increment(&x))
    {
        item = (JPEGMarkerItem*)nitf_ListIterator_get(&x);
        JPEGMarkerItem_print(item, file);
    }
    return;
}

/*!
 *  This is called by the ImageIO controller whenever it is necessary to
 *  delete a block.  It is implemented as a counter function to
 *  implReadBlock() which reads and allocates a block of the imagery.
 *
 *  \param control Opaque handle to the general control object
 *  \param block   This is the block to free
 *  \param error In the event that this function returns zero, the error
 *  will be populated
 *  \return One on success, zero on failure
 */
NITFPRIV(int) implFreeBlock(nitf_DecompressionControl* control,
                            uint8_t* block,
                            nitf_Error* error);

NITFPRIV(nitf_DecompressionControl*) implOpen(nitf_ImageSubheader* subheader,
                                              nrt_HashTable* options,
                                              nitf_Error* error);

/*!
 *  This function is the creator of the decompression control interface
 *  object.  It produces an opaque handle which refers to the object
 *  necessary to perform any decompression task.  The underlying
 *  implementation is private.
 *
 *  \param io The io handle.  This will be stored for future reference
 *  \param offset The offset of this image segment in the file
 *  \param fileLength  The length of the image segment
 *  \param blockInfo A structure containing the blocking information
 *  \param blockMask The structure containing the block mask (if
 *  any exists).  This parameter is currently ignored.
 *  \param error An error which will be populated on failure
 *  \return NULL on failure, an opaque pointer on success
 */
NITFPRIV(NITF_BOOL) implStart(nitf_DecompressionControl* control,
                              nitf_IOInterface*  io,
                              uint64_t offset,
                              uint64_t fileLength,
                              nitf_BlockingInfo* blockInfo,
                              uint64_t* blockMask,
                              nitf_Error* error);

/*!
 *  This function is the symmetric counter-function to the implOpen()
 *  method.  It destroys the control object, and returns an address pointing
 *  to NULL.
 *
 *  \param control An address pointing to an object to be destroyed.
 */
NITFPRIV(void) implClose(nitf_DecompressionControl** control);



/*!
 *  Reading a new block out of memory.  Ill give you whichever block
 *  you want based on the offset that I found when scanning for JPEG
 *  begin markers!
 *
 *  \param control  The control object
 *  \param blockNumber  The block number to retrieve from file
 *  \param separateBands Should the bands be separated?  Hint: if
 *  it is single band data, I optimize it for you by ignoring this
 *  entirely, so any time you get B or S, you can set this
 *  \param error  An error to populate on failure
 *  \return NULL on failure, or a pointer to a block on success.
 *
 */

NITFPRIV(uint8_t*) implReadBlock(nitf_DecompressionControl* control,
                                    uint32_t blockNumber,
                                    uint64_t* blockSize,
                                    nitf_Error* error);


/*!
 *  This static array of strings describes the contract of our
 *  plugin.  Put explicitly, we agree to handle decompression of type
 *  C3 (and later C5), representing JPEG imagery (except JPEG2000).
 *  We terminate with a NULL.
 */
static const char *ident[] =
    {
        NITF_PLUGIN_DECOMPRESSION_KEY,
        "C3",
        "M3",
        /*"C5",*/
        NULL,
    };

/*! Simple little typedef to make life easier for me  */
typedef uint8_t* DATA_BUFFER;

/*!
 *  \struct ImplControl
 *  \brief The actual implementation beneath the opaque control pointer
 *
 *  The ImplControl object is treated publicly as an opaque handle.
 *  In fact, it contains persist information which is necessary for
 *  the decompression control.
 *
 *  \ar io The io handle (provided when we opened the interface)
 *  \ar markerList A list of SOI markers which we need to read blocks out
 *  of order
 *  \ar quantTable  Quantization table (currently not used)
 *  \ar length  The length of the block in bytes
 *
 *  The length value is needed to generate the zero block for decompresion
 *  error recovery. If enabled, blocks that can not be decompressed are
 *  returned as zeros. This is controled by the ZERO_BLOCK define
 */
typedef struct _JPEGImplControl
{
    nitf_IOInterface* ioInterface;
    nitf_List*        markerList;
    int*              quantTable;
    uint32_t       length;       /* Total length of the block in bytes */
}
JPEGImplControl;


/* NITFPRIV(NITF_BOOL) JPEGImplControl_assignQuantTable(JPEGImplControl* control, */
/*            float comrat, */
/*            nitf_Error* error) */
/* { */
/*     if (comrat < ) */

/* } */

/*!
 *  \struct JPEGBlock
 *  \brief One more object to simplify the components one level further
 *
 *  The JPEGBlock structure has characteristics that make up one block
 *  of once JPEG compressed imagery, in uncompressed form.  This is
 *  the structure which contains the uncompressed imagery once it has been
 *  read.
 *
 *  The buffer of uncompressed data is given by reference back to the caller.
 *
 *  \ar uncompressed The uncompressed data
 *  \ar rows The number of rows
 *  \ar cols The number of cols
 *  \ar bands The number of bands (components in JPEG terms)
 *  \ar current A placeholder for the current end of the buffer
 */
typedef struct _JPEGBlock
{
    DATA_BUFFER uncompressed;
    int         rows;
    int         cols;
    int         bands;
    int         current;
}
JPEGBlock;

/*!
 *  Destruct ourselves a JPEGBlock
 *  \param block An address of a pointer to delete (we NULL-set the pointer)
 *
 */
void JPEGBlock_destruct(JPEGBlock** block)
{
    if (block == NULL)
        return;
    if (*block == NULL)
        return;

    if ((*block)->uncompressed != NULL)
        NITF_FREE((*block)->uncompressed);

    NITF_FREE(*block);
    *block = NULL;
}

/*!
 *  Construct a new JPEGBlock object.  'Give me lots of info, and
 *  I will build it'-type contract.  We don't populate the uncompressed
 *  section until later
 *  \param rows The number of rows in the block
 *  \param cols The number of cols in the block
 *
 */
JPEGBlock* JPEGBlock_construct(int rows, int cols, int bands, nitf_Error* error)
{
    JPEGBlock* block = (JPEGBlock*) NITF_MALLOC(sizeof(JPEGBlock));
    if (!block)
    {
        nitf_Error_init(error, "Failure to construct JPEG block", NITF_CTXT,
                        NITF_ERR_MEMORY);
        return NULL;
    }
    block->rows = rows;
    block->cols = cols;
    block->bands = bands;
    block->current = 0;

    block->uncompressed = (DATA_BUFFER) NITF_MALLOC(_BLOCK_SIZE(block));
    if (!block->uncompressed)
    {
        /* need to destroy */
        JPEGBlock_destruct(&block);
    }
    return block;
}

/*!
 *  Here, we append uncompressed scanlines as we read them.  This
 *  of course, does not solve the band interleaved by block issue
 *  \param block The block to append to
 *  \param buffer The sample buffer
 *  \param rowStride The stride of the row (keeping in mind there
 *  may be multiple bands.
 */
void JPEGBlock_append(JPEGBlock* block, JSAMPLE* buffer, int rowStride)
{
    assert(rowStride + block->current <= _BLOCK_SIZE(block));
    /*hexDump(buffer, rowStride);*/

    DPRINTA3("Copying %d components from ranges [%d - %d]\n", rowStride,
            block->current, block->current + rowStride);
    memcpy(block->uncompressed + block->current, buffer, rowStride);
    block->current += rowStride;
}


void JPEGBlock_reorder(JPEGBlock* block)
{
    int i;
    int n;
    int j;
    off_t current = 0;
    DATA_BUFFER* bands = (DATA_BUFFER*) NITF_MALLOC(sizeof(DATA_BUFFER)
            * block->bands);
    if (bands == NULL)
    {
        return;
    }

    for (i = 0; i < block->bands; i++)
    {
        bands[i] = (DATA_BUFFER) NITF_MALLOC(((size_t)block->rows) * block->cols);
    }

    for (n = 0; n < block->rows * block->cols; n++)
    {
        j = n * block->bands;

        for (i = 0; i < block->bands; i++)
        {
            DATA_BUFFER thisBand = bands[i];
            if (thisBand != NULL)
            {
                thisBand[n] = block->uncompressed[j + i];
            }
        }
    }

    for (i = 0; i < block->bands; i++)
    {
        if (bands[i] != NULL)
        {
            memcpy(block->uncompressed + current, bands[i], n);
        }
        current += n;
    }

    for (i = 0; i < block->bands; i++)
    {
        NITF_FREE(bands[i]);
    }
    NITF_FREE(bands);
}

static nitf_DecompressionInterface interfaceTable =
{
    implOpen,
    implStart,
    implReadBlock,
    implFreeBlock,
    implClose,
    NULL
};

NITFPRIV(int) implFreeBlock(nitf_DecompressionControl* control,
                            uint8_t* block,
                            nitf_Error* error)
{
    (void)control;
    (void)error;

    if (block)
        NITF_FREE(block);
    return 1;
}

typedef enum _JPEGMarker
{
    JPEG_NOT_MARKER,
    JPEG_MARKER_ERROR,
    JPEG_MARKER_FF_OCCURS_NATURALLY,
    JPEG_MARKER_SOI,
    JPEG_MARKER_DECL_QUANT_TABLE,
    JPEG_MARKER_DECL_HUFF_TABLE,
    JPEG_MARKER_SOF,
    JPEG_MARKER_SOS,
    JPEG_MARKER_COMMENT,
    JPEG_MARKER_EOI,
    JPEG_MARKER_APP6,
    JPEG_MARKER_APP7,
    JPEG_MARKER_DONT_CARE,
} JPEGMarker;

NITFPRIV(NITF_BOOL) readByte(nitf_IOInterface* io,
        unsigned char* b,
        nitf_Error* error)
{
    return nitf_IOInterface_read(io, (NITF_DATA *) b, 1, error);
}

NITFPRIV(NITF_BOOL) readShort(nitf_IOInterface* io,
        uint16_t* native,
        nitf_Error* error)
{
    uint16_t raw;
    if (!nitf_IOInterface_read(io, (char*)&raw, 2, error))
    {
        return NITF_FAILURE;
    }
    *native = ntohs(raw);
    return NITF_SUCCESS;
}

NITFPRIV(int) readMarker(nitf_IOInterface* io, nitf_Error* error)
{
    int markerEnum = JPEG_MARKER_ERROR;
    unsigned char native = 0x0000;
    if (readByte(io, &native, error) )
    {
        switch (native)
        {
        case 0x00:
            markerEnum = JPEG_MARKER_FF_OCCURS_NATURALLY;
            break;

        case 0xD8:
            markerEnum = JPEG_MARKER_SOI;
            break;

        case 0xD9:
            markerEnum = JPEG_MARKER_EOI;
            break;

        case 0xDB:
            markerEnum = JPEG_MARKER_DECL_QUANT_TABLE;
            break;

        case 0xC4:
            markerEnum = JPEG_MARKER_DECL_HUFF_TABLE;
            break;

        case 0xC0:
            markerEnum = JPEG_MARKER_SOF;
            break;

        case 0xDA:
            markerEnum = JPEG_MARKER_SOS;
            break;

        case 0xFE:
            markerEnum = JPEG_MARKER_COMMENT;
            break;

        case 0xE6:
            markerEnum = JPEG_MARKER_APP6;
            break;

        case 0xE7:
            markerEnum = JPEG_MARKER_APP7;
            break;
            /*
            case 0xE9:
            markerEnum = JPEG_MARKER_APP7;
            break;
            */
        default:
            markerEnum = JPEG_MARKER_DONT_CARE;
        }
    }
    /*  Then we already have an error!  */
    return markerEnum;
}


NITFPRIV(NITF_BOOL) readSOI(nitf_IOInterface* io,
        uint64_t* bytesRead,
        nitf_Error* error)
{
    unsigned char needFF;
    int tokenType;

    if (! readByte(io, &needFF, error) ) return NITF_FAILURE;
    (*bytesRead)++;

    if ( needFF != 0xFF )
    {
        nitf_Error_init(error,
                "Missing mandatory APP6 marker indicator (FF)\n",
                NITF_CTXT,
                NITF_ERR_DECOMPRESSION);

        return NITF_FAILURE;
    }
    tokenType = readMarker(io, error);
    (*bytesRead)++;

    if (tokenType == JPEG_MARKER_ERROR)
    return NITF_FAILURE;

    DPRINT("====================================\n");
    DPRINT("Successful SOI read!\n");
    DPRINT("====================================\n");

    return NITF_SUCCESS;
}

/*
    In order to get here, we must have read:
    SOI, APP6, DQT

NITFPRIV(NITF_BOOL) readSOF(nitf_IOInterface* io,
                            uint64_t* bytesRead,
                            nitf_Error* error)
{

    char precision;
    char nf;
    uint16_t x;
    uint16_t y;
    uint16_t numBytesInHdr;
    if (! readShort(io, &numBytesInHdr, error) )
        return NITF_FAILURE;

    (*bytesRead) += numBytesInHdr;
    DPRINTA1("SOF: Header length: [%d]\n", numBytesInHdr);
    numBytesInHdr -= 2;



    nitf_IOInterface_read(io, &precision, 1, error);

    DPRINTA1("SOF: Precision (usually 8): %d\n", (int)precision);

    if (! readShort(io, &x, error ) )
        return NITF_FAILURE;

    if (! readShort(io, &y, error ) )
        return NITF_FAILURE;

    DPRINTA2("SOF: Frame is %d x %d\n", y, x);

    nitf_IOInterface_read(io, &nf, 1, error);
    DPRINTA1("SOF: Number of components in image: [%d]\n", (int)nf);

    {
        int i;
        char cid;
        char hvSampleF;
        char qTableNum;
        for (i = 0; i < (int)nf; i++)
        {
            nitf_IOInterface_read(io, &cid, 1, error);
            DPRINTA2("SOF: Nf[%d]->cid [%x]: \n", i, (int)cid);
            nitf_IOInterface_read(io, &hvSampleF, 1, error);
            DPRINTA2("SOF: Nf[%d]->hvSampleF: [%x]\n", i, (int)hvSampleF);
            nitf_IOInterface_read(io, &qTableNum, 1, error);
            DPRINTA2("SOF: Nf[%d]->qTableNum: [%x]\n", i, (int)qTableNum);
        }
    }

    DPRINT("Successful SOF read!\n");
    return NITF_SUCCESS;
}

*/

/*
    In order to get here, we must have read:
    SOI, APP6, DQT, SOF0, DHT
*/
NITFPRIV(NITF_BOOL) readSOS(nitf_IOInterface* io,
                            uint64_t* bytesRead,
                            nitf_Error* error)
{

    /*  Need to read bytes in header  */
    uint16_t numBytesInHdr;
    /*  If this isnt happening, throw up  */
    if (! readShort(io, &numBytesInHdr, error) )
        return NITF_FAILURE;
    /*  Print  now     */
    DPRINTA1("SOS: Header length: [%d]\n", numBytesInHdr);
    /*  Markup now     */
    (*bytesRead) += numBytesInHdr;
    /*  Normalize now  */
    numBytesInHdr -= 2;
    /*  Skip for now   */
    if (!  NITF_IO_SUCCESS(nitf_IOInterface_seek(io,
                                                 numBytesInHdr,
                                                 NITF_SEEK_CUR,
                                                 error)))
        return NITF_FAILURE;
    /*  Be happy now   */
    DPRINT("Successful SOS read!\n");
    /*  Return success */
    return NITF_SUCCESS;
}
/*
    In order to get here, we must have read:
    SOI, APP6, DQT, SOF0
*/


NITFPRIV(NITF_BOOL) readHuffTable(nitf_IOInterface* io,
                                  uint64_t* bytesRead,
                                  nitf_Error* error)
{
    /*  Need to read a header length */
    uint16_t numBytesInHdr;
    /*  Read it or die  */
    if (! readShort(io, &numBytesInHdr, error) )
        return NITF_FAILURE;
    /*  Print now  */
    DPRINTA1("Huff Table: Header length: [%d]\n", numBytesInHdr);
    /*  Markup now */
    (*bytesRead) += numBytesInHdr;
    /*  Adjust for what we have read already  */
    numBytesInHdr -= 2;

    if (! NITF_IO_SUCCESS(nitf_IOInterface_seek(io,
                                                numBytesInHdr,
                                                NITF_SEEK_CUR,
                                                error)))
        return NITF_FAILURE;

    /*  Rejoice!  */
    DPRINT("Successful Huff Table read!\n");
    /*  Return!  */
    return NITF_SUCCESS;
}

NITFPRIV(NITF_BOOL) readQuantTable(nitf_IOInterface* io,
                                   uint64_t* bytesRead,
                                   nitf_Error* error)
{
    /*  Declare something to read into  */
    uint16_t numBytesInHdr;
    /*  Now start reading... */
    if (! readShort(io, &numBytesInHdr, error) )
        return NITF_FAILURE;

    /*  Print now   */
    DPRINTA1("Quant Table: Header length: [%d]\n", numBytesInHdr);
    /*  Markup now  */
    (*bytesRead) += numBytesInHdr;

    /*  Adjust now  */
    numBytesInHdr -= 2;

    if (! NITF_IO_SUCCESS(nitf_IOInterface_seek(io,
                                                numBytesInHdr,
                                                NITF_SEEK_CUR,
                                                error)))
        return NITF_FAILURE;

    /*  Celebrate  */
    DPRINT("Successful Quant Table read!\n");
    /*  Go Home    */
    return NITF_SUCCESS;
}

/*
   This method is currently used to push a marker and its
   respective offset onto our list.
*/

NITFPRIV(NITF_BOOL) pushMarker(nitf_List* markerList,
                               const char* name,
                               off_t where,
                               nitf_Error* error)
{
    JPEGMarkerItem* item = JPEGMarkerItem_construct(error);
    nrt_strcpy_s(item->name, JPEGMarkerItem_name_LEN, name);
    item->off = where;
    if (!item) return NITF_FAILURE;
    if (!nitf_List_pushBack(markerList, item, error) )
        return NITF_FAILURE;
    return  NITF_SUCCESS;
}

/*
  This gets called (for now) when we open the interface.
  We then begin to apply that information later.

  This is basically a book-keeping function (and not a
  terribly efficient one at that...)

*/
NITFPRIV(NITF_BOOL) scanOffsets(nitf_IOInterface* io,
                                nitf_List* markerList,
                                uint64_t fileLength,
                                nitf_Error* error)
{

    uint64_t bytesRead = 0;

    /*  Book keeping block  */
    const nitf_Off origin_ = nitf_IOInterface_tell(io, error);
    assert(NITF_IO_SUCCESS(origin_));
    uint64_t origin = origin_;
    /*  End book keeping block  */
    DPRINTA1("File length: %ld\n",  fileLength);
    while (bytesRead < fileLength)
    {

        unsigned char b;
        if (! readByte(io, &b, error) )
        {

            DPRINTA1("Read byte failed on byte %ld!\n", bytesRead);
            goto CATCH_ERROR;
        }
        ++bytesRead;


        if (b == 0xFF )
        {
            int tokenType;
            tokenType = readMarker(io, error);
            ++bytesRead;

            if (tokenType == JPEG_MARKER_ERROR)
            {
                DPRINT("tokenType is JPEG_MARKER_ERROR\n");
                goto CATCH_ERROR;
            }

            else if (tokenType == JPEG_MARKER_FF_OCCURS_NATURALLY)
            {
                /*printf("Found naturally occuring ff sequence!\n");*/
            }
            else
            {
                off_t where = (off_t)nitf_IOInterface_tell(io, error);

                uint64_t totalBytes = (fileLength - bytesRead) +
                    (where - origin);
                assert( fileLength == totalBytes);
                switch (tokenType)
                {
                case JPEG_MARKER_EOI:
                    if (! pushMarker(markerList, "EOI", where, error) )
                    {
                        DPRINT("Failure EOI (pushMarker)\n");
                        goto CATCH_ERROR;
                    }

                    DPRINT("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
                    DPRINT("Successful EOI!\n");
                    DPRINT("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
                    break;
                    /*return NITF_SUCCESS;
                     */
                case JPEG_NOT_MARKER:
                    break;

                    /*  If it is the start of image, I want to read an APP6  */
                case JPEG_MARKER_SOI:
                    if (! pushMarker(markerList, "SOI", where, error) )
                    {
                        DPRINT("Failure SOI (pushMarker)\n");
                        goto CATCH_ERROR;
                    }

                    if (!readSOI(io, &bytesRead, error))
                    {
                        DPRINT("Failure SOF (readSOI)\n");
                        goto CATCH_ERROR;
                    }

                    break;

                case JPEG_MARKER_SOS:
                    if (! pushMarker(markerList, "SOS", where, error) )
                    {
                        DPRINT("Failure SOS (pushMarker)\n");
                        goto CATCH_ERROR;
                    }

                    if (!readSOS(io, &bytesRead, error))
                    {
                        DPRINT("Failure SOS (readSOS)\n");
                        goto CATCH_ERROR;
                    }
                    break;


                case JPEG_MARKER_DECL_QUANT_TABLE:
                    if (! pushMarker(markerList, "DQT", where, error) )
                    {
                        DPRINT("Failure DQT (pushMarker)\n");
                        goto CATCH_ERROR;
                    }

                    if (!readQuantTable(io, &bytesRead, error))
                    {
                        DPRINT("Failure DQT (readQuantTable)\n");
                        goto CATCH_ERROR;
                    }
                    break;

                case JPEG_MARKER_DECL_HUFF_TABLE:
                    if (! pushMarker(markerList, "DHT", where, error) )
                    {
                        DPRINT("Failure DHT (pushMarker)\n");
                        goto CATCH_ERROR;
                    }

                    if (!readHuffTable(io, &bytesRead, error))
                    {
                        DPRINT("Failure DHT (readHuffTable)\n");
                        goto CATCH_ERROR;
                    }

                    break;

                case JPEG_MARKER_SOF:
                    if (! pushMarker(markerList, "SOF", where, error) )
                    {
                        DPRINT("Failure SOF (pushMarker)\n");
                        goto CATCH_ERROR;
                    }

                    /* if (!readSOF(io, error)) */
                    /*       goto CATCH_ERROR; */
                    break;
                }
            }
        }
    }

    /*  Well this is what I wanted to happen, although I didnt have
    the guts to ask for it in the while loop, since I have seen
    some pretty unfortunate JPEGs in NITF files
    */
    if (bytesRead != fileLength)
    {
        DPRINT("Warning: couldnt equalize the number of bytes desired and those read\n");
    }
    return NITF_SUCCESS;

CATCH_ERROR:
    nitf_Error_print(error, stdout, "While scanning offsets!");
    return NITF_FAILURE;
}

NITFPRIV(nitf_DecompressionControl*) implOpen(nitf_ImageSubheader* subheader,
                                              nrt_HashTable* options,
                                              nitf_Error* error)
{
    (void)subheader;
    (void)options;
    (void)error;

    JPEGImplControl* implControl; /* This is our local storage  */
    implControl = (JPEGImplControl*)NITF_MALLOC(sizeof(JPEGImplControl));
    if (implControl == NULL)
    {
        nitf_Error_init(error,
                "Error creating control object",
                NITF_CTXT,
                NITF_ERR_DECOMPRESSION);
        return NULL;
    }
    implControl->ioInterface = NULL;
    implControl->markerList = NULL;
    implControl->quantTable = NULL;
    implControl->length = 0;
    return (nitf_DecompressionControl*)implControl;
}

/*!
 *  Open our interface up.  This thing saves a reference to our
 *  io, and sets us up to read.  We call fdopen() to get a FILE*
 *  back from our IOInterface.
 *
 *  \todo  This function needs to know Bits per pixel so that it
 *  can load the correct JPEG library
 */
NITFPRIV(NITF_BOOL) implStart(nitf_DecompressionControl* control,
                              nitf_IOInterface* io,
                              uint64_t offset,
                              uint64_t fileLength,
                              nitf_BlockingInfo* blockInfo,
                              uint64_t* blockMask,
                              nitf_Error* error)
{
    JPEGImplControl* implControl = (JPEGImplControl*) control;

    DPRINT("=============================================================\n");
    DPRINT("JPEG decompression\n");
    DPRINT("Blocking Info:\n");
    DPRINTA1("[%d] blockInfo->numBlocksPerRow\n", blockInfo->numBlocksPerRow);
    DPRINTA1("[%d] blockInfo->numBlocksPerCol\n", blockInfo->numBlocksPerCol);
    DPRINTA1("[%d] blockInfo->numRowsPerBlock\n", blockInfo->numRowsPerBlock);
    DPRINTA1("[%d] blockInfo->numColsPerBlock\n", blockInfo->numColsPerBlock);
    DPRINTA1("[%d] blockInfo->length\n", blockInfo->length);

    /*  For right now, we are storing this silly markerList.  I think
     this will prove to be a useless mechanism  */
    implControl->markerList = nitf_List_construct(error);
    if (!implControl->markerList)
    {
        /*  If this fails, the error is already set  */
        return NITF_FAILURE;
    }

    /*  Seek to our start point, just in case... */
    if (!NITF_IO_SUCCESS(nitf_IOInterface_seek(io,
                                               offset,
                                               NITF_SEEK_SET,
                                               error)))
    {
        nitf_Error_initf(error,
                NITF_CTXT,
                NITF_ERR_DECOMPRESSION,
                "Error seeking to offset for JPEG block [%ld]",
                offset);
        return NITF_FAILURE;
    }

    /*  Find all marker offsets!!!!  */
    if (!scanOffsets(io, implControl->markerList, fileLength, error))
    {
        return NITF_FAILURE;
    }

    {
        uint32_t nextBlock = 0;
        nitf_ListIterator x = nitf_List_begin((implControl->markerList));
        nitf_ListIterator e = nitf_List_end((implControl->markerList));
        while (nitf_ListIterator_notEqualTo(&x, &e))

        {
            JPEGMarkerItem *item = nitf_ListIterator_get(&x);
            DPRINTA2("[%s:%d]", item->name, item->off );

            if (strcmp(item->name, "SOI") == 0)
            {
                while(blockMask[nextBlock++] == NITF_IMAGE_IO_NO_BLOCK);
                item->block = nextBlock - 1;
            }
            else
            {
                item->block = NITF_IMAGE_IO_NO_BLOCK;
            }

            nitf_ListIterator_increment(&x);
        }
        DPRINT("\n");
    }

    /*  Seek to our start point, just in case... */
    if (!NITF_IO_SUCCESS(nitf_IOInterface_seek(io,
                                               offset,
                                               NITF_SEEK_SET,
                                               error)))
    {
        nitf_Error_init(error,
                "Error seeking to necessary offset for JPEG block",
                NITF_CTXT,
                NITF_ERR_DECOMPRESSION);
        return NITF_FAILURE;
    }

    implControl->ioInterface = io;
    implControl->length = (uint32_t)blockInfo->length;
    return NITF_SUCCESS;
}

typedef struct _JPEGIOManager
{
    struct jpeg_source_mgr pub;         /* public fields */
    uint8_t buffer[INPUT_BUF_SIZE];  /* start of buffer */
    boolean start_of_file;              /* have we gotten any data yet? */
    nitf_IOInterface* ioInterface;      /* source IO */
    nitf_Off ioStart;                   /* the io interface start offset (tell) */
    nitf_Off ioEnd;                     /* the io interface end offset (size) */
    uint32_t blockLength;            /* the length of the block */
    uint32_t bytesRead;              /* the number of bytes read so far */
    nitf_Error *error;
} JPEGIOManager;


NITFPRIV(void) JPEGInitSource (j_decompress_ptr cinfo)
{
    JPEGIOManager* src = (JPEGIOManager*)cinfo->src;
    /* We reset the empty-input-file flag for each image,
     * but we don't clear the input buffer.
     * This is correct behavior for reading a series of images from one source.
     */
    src->start_of_file = TRUE;
}

NITFPRIV(void) JPEGTerminateSource(j_decompress_ptr cinfo)
{
    /* delete the source manager and buffer */
    if (cinfo)
    {
        JPEGIOManager* src = (JPEGIOManager*)cinfo->src;
        /*if (src && src->buffer)
        {
            NITF_FREE(src->buffer);
            src->buffer = NULL;
        }*/
        if (src)
        {
            NITF_FREE(src);
        }
        cinfo->src = NULL;
    }
}

NITFPRIV(boolean) JPEGFillInputBuffer (j_decompress_ptr cinfo)
{
    JPEGIOManager* src = (JPEGIOManager*)cinfo->src;
    uint32_t toRead;
    nitf_Off ioOff;

    toRead = src->blockLength - src->bytesRead;
    if (toRead > INPUT_BUF_SIZE)
    {
        toRead = INPUT_BUF_SIZE;
    }

    /* check for EOF bounds */
    ioOff = src->ioStart + src->bytesRead;
    if (ioOff + toRead > src->ioEnd)
    {
        nitf_Off ioDiff = src->ioEnd - ioOff;
        if (ioDiff < 0)
            toRead = 0;
        else
            toRead = (uint32_t)ioDiff;
    }

    if (toRead == 0)
    {
        if (src->start_of_file)
        {
            /* Treat empty input file as fatal error */
            ERREXIT(cinfo, JERR_INPUT_EMPTY);
        }
        WARNMS(cinfo, JWRN_JPEG_EOF);
        /* Insert a fake EOI marker */
        src->buffer[0] = 0xFF;
        src->buffer[1] = JPEG_EOI;
        toRead = 2;
    }
    else
    {
        if (!nitf_IOInterface_read(src->ioInterface, src->buffer,
                                   toRead, src->error))
        {
            return FALSE;
        }
        src->bytesRead += toRead;
    }

    src->pub.next_input_byte = src->buffer;
    src->pub.bytes_in_buffer = toRead;
    src->start_of_file = FALSE;
    return TRUE;
}

NITFPRIV(void) JPEGSkipInputData(j_decompress_ptr cinfo, long num_bytes)
{
    JPEGIOManager* src = (JPEGIOManager*)cinfo->src;

    if (num_bytes > 0)
    {
        while (num_bytes > (long) src->pub.bytes_in_buffer)
        {
            num_bytes -= (long) src->pub.bytes_in_buffer;
            (void) src->pub.fill_input_buffer(cinfo);
            /* note we assume that fill_input_buffer will never return FALSE,
             * so suspension need not be handled.
             */
        }
        src->pub.next_input_byte += (size_t) num_bytes;
        src->pub.bytes_in_buffer -= (size_t) num_bytes;
    }
}

NITFPRIV(NITF_BOOL) JPEGCreateIOSource(j_decompress_ptr cinfo,
                                       JPEGImplControl* implControl,
                                       nitf_Error* error)
{
    JPEGIOManager* src = NULL;
    if (!cinfo->src)
    {
        src = (JPEGIOManager*)NITF_MALLOC(sizeof(JPEGIOManager));
        if (src == NULL)
        {
            nitf_Error_init(error,
                            "Error creating IO manager object",
                            NITF_CTXT,
                            NITF_ERR_DECOMPRESSION);
            return NITF_FAILURE;
        }
        cinfo->src = (struct jpeg_source_mgr*)src;

        /*src->buffer = (uint8_t*)NITF_MALLOC(INPUT_BUF_SIZE);
        if (src->buffer == NULL)
        {
            nitf_Error_init(error,
                            "Error creating IO buffer",
                            NITF_CTXT,
                            NITF_ERR_DECOMPRESSION);
            return NITF_FAILURE;
        }*/
    }
    else
    {
        src = (JPEGIOManager*)cinfo->src;
    }

    src->pub.init_source = JPEGInitSource;
    src->pub.fill_input_buffer = JPEGFillInputBuffer;
    src->pub.skip_input_data = JPEGSkipInputData;
    src->pub.resync_to_restart = jpeg_resync_to_restart; /* use default method */
    src->pub.term_source = JPEGTerminateSource;
    src->pub.bytes_in_buffer = 0; /* forces fill_input_buffer on first read */
    src->pub.next_input_byte = NULL; /* until buffer loaded */
    src->ioInterface = implControl->ioInterface;
    src->blockLength = implControl->length;
    src->bytesRead = 0;
    src->ioStart = nitf_IOInterface_tell(src->ioInterface, error);
    src->ioEnd = nitf_IOInterface_getSize(src->ioInterface, error);
    src->error = error;

    return NITF_SUCCESS;
}


/*!
 *  Returns a starting offset for the block number indicated.
 *  The information is in the control structure, which was
 *  generated during implOpen.
 */
NITFPRIV(NITF_BOOL) findBlockSOI(JPEGImplControl* control,
                                 uint32_t blockNumber,
                                 off_t* soi,
                                 nitf_Error* error)
{
    nitf_ListIterator x = nitf_List_begin(control->markerList);
    nitf_ListIterator end = nitf_List_end(control->markerList);
    uint32_t j = blockNumber - 1;

    for (x = nitf_List_begin(control->markerList);
            nitf_ListIterator_notEqualTo(&x, &end);
            nitf_ListIterator_increment(&x))
    {
        JPEGMarkerItem* item = (JPEGMarkerItem*)nitf_ListIterator_get(&x);
        j = item->block;
        if ((strcmp(item->name, "SOI") == 0) && (j == blockNumber))
        {
            *soi = (off_t)(item->off - 2);
            break;
        }
        /*++j;
        if (j == blockNumber)
        {
            *soi = item->off - 2;
            break;
        } */
    }
    if (j != blockNumber)
    {
        nitf_Error_initf(error,
                         NITF_CTXT,
                         NITF_ERR_DECOMPRESSION,
                         "Invalid block (no offset found) [%d]", blockNumber);
        return NITF_FAILURE;
    }
    return NITF_SUCCESS;
}

static uint64_t block_size(const JPEGBlock* block)
{
    const int retval = _BLOCK_SIZE(block);
    return retval;
}

NITFPRIV(uint8_t*) implReadBlock(nitf_DecompressionControl* control,
                                    uint32_t blockNumber,
                                    uint64_t* blockSize,
                                    nitf_Error* error)
{
    /*
     *  For now, do as I say (to test that you dont break anything in
     *  the trivial case
     */
    NITF_BOOL separateBands = 0;

    /*  Get out the read object from the opaque handle  */
    JPEGImplControl* implControl = (JPEGImplControl*)control;
    off_t soi = 0;

    struct jpeg_error_mgr jerr;
    struct jpeg_decompress_struct cinfo;

    JSAMPARRAY buffer;
    int row_stride;
    int ret;
    JPEGBlock* block;
    NITF_DATA *uncompressed;

    if (!findBlockSOI(control, blockNumber, &soi, error))
#ifdef ZERO_BLOCK
    {
        uint8_t *zeros; /* Buffer of zeros */

        zeros = NITF_MALLOC(implControl->length);
        if (zeros == NULL)
        {
            nitf_Error_init(error, "Malloc failure for zero block",
                    NITF_CTXT, NITF_ERR_MEMORY);
            return(NULL);
        }
#ifdef ZERO_BLOCK_WARN
        fprintf(stderr,
                "JPEG Decompression error: No SOI for block %d, returning zeros\n",
                blockNumber);
#endif
        memset(zeros, 0, implControl->length);
        return(zeros);
    }
#else
    {
        nitf_Error_initf(error, NITF_CTXT, NITF_ERR_DECOMPRESSION,
                "Missing SOI for block %d", blockNumber);
        return(NULL);
    }
#endif

    DPRINTA2("Found SOI [%d] for block # %d\n", (int)soi, (int)blockNumber);
    /*  We need to locate our block offset!! */
    if (nitf_IOInterface_seek(implControl->ioInterface, soi,
                    NITF_SEEK_SET, error) == (nitf_Off)-1)
    {
        nitf_Error_init(error, NITF_STRERROR( NITF_ERRNO ),
                NITF_CTXT,
                NITF_ERR_DECOMPRESSION);
    }

    /*  Set up the error handler  */
    cinfo.err = jpeg_std_error(&jerr);
    DPRINT("Creating decompression struct!\n");
    /*  Tell our info struct to be decompression  */
    jpeg_create_decompress(&cinfo);

    /*  Bind up our source location */
    /*jpeg_stdio_src(&cinfo, jstream);*/
    JPEGCreateIOSource(&cinfo, implControl, error);

    /*  Read the header  */
    DPRINT("Reading header... ");
    ret = jpeg_read_header(&cinfo, 0);
    DPRINTA2("success! [%dx%d]\n", cinfo.image_width, cinfo.image_height);
    if (ret != JPEG_HEADER_OK)
#ifdef ZERO_BLOCK
    {
        uint8_t *zeros; /* Buffer of zeros */

        zeros = NITF_MALLOC(implControl->length);
        if (zeros == NULL)
        {
            nitf_Error_init(error, "Malloc failure for zero block",
                    NITF_CTXT, NITF_ERR_MEMORY);
            return(NULL);
        }
#ifdef ZERO_BLOCK_WARN
        fprintf(stderr,
                "JPEG Decompression error: "
                "Invalid JPEG Header for block %d, returning zeros\n",
                blockNumber);
#endif
        memset(zeros, 0, implControl->length);
        return(zeros);
    }
#else
    {
        nitf_Error_initf(error,
                NITF_CTXT,
                NITF_ERR_READING_FROM_FILE,
                "Invalid JPEG Header for block [%d]",
                blockNumber);
        return NULL;
    }
#endif

    jpeg_start_decompress(&cinfo);
    DPRINT("Started decompress... \n");
    block =
    JPEGBlock_construct(cinfo.output_height,
            cinfo.output_width,
            cinfo.output_components,
            error);

    if (!block)
#ifdef ZERO_BLOCK
    {
        uint8_t *zeros; /* Buffer of zeros */

        zeros = NITF_MALLOC(implControl->length);
        if (zeros == NULL)
        {
            nitf_Error_init(error, "Malloc failure for zero block",
                    NITF_CTXT, NITF_ERR_MEMORY);
            return(NULL);
        }
#ifdef ZERO_BLOCK_WARN
        fprintf(stderr,
                "JPEG Decompression error: "
                "Block object construct failed for block %d, returning zeros",
                blockNumber);
#endif
        memset(zeros, 0, implControl->length);
        return(zeros);
    }
#else
    {
        nitf_Error_initf(error,
                NITF_CTXT,
                NITF_ERR_DECOMPRESSION,
                "Block object construct failed for block [%d]",
                blockNumber);
        return NULL;
    }
#endif

    row_stride = cinfo.output_width *
    cinfo.output_components;
    buffer = (cinfo.mem->alloc_sarray)
    ((j_common_ptr) & cinfo,
            JPOOL_IMAGE, row_stride, 1);

    while (cinfo.output_scanline <
            cinfo.output_height)
    {
        jpeg_read_scanlines(&cinfo, buffer, 1);
        /* Clobbering each time  */
        JPEGBlock_append(block, buffer[0], row_stride);
    }

    jpeg_finish_decompress(&cinfo);
    jpeg_destroy_decompress(&cinfo);
    DPRINT("JPEG decompression complete\n");
    DPRINT("=============================================================\n");

    /*  We separate bands if it is multi-band AND Band-interleaved */
    /*  by block                                                   */
    if (separateBands)
    {
        JPEGBlock_reorder(block);
    }

    uncompressed = (uint8_t*)(block->uncompressed);
    block->uncompressed = NULL;
    *blockSize = block_size(block);
    JPEGBlock_destruct(&block);

    return uncompressed;
}


NITFPRIV(void) implClose(nitf_DecompressionControl** control)
{
    JPEGImplControl* implControl;
    DPRINT("Destroying compression object in JPEG plugin\n");
    implControl = (JPEGImplControl*) * control;

    /* delete marker list */
    if (implControl && implControl->markerList)
    {
        nitf_List_destruct(&implControl->markerList);
    }
    /* delete quant table */
    if (implControl && implControl->quantTable)
    {
        free(implControl->quantTable);
    }
    if (implControl)
    {
        NITF_FREE(implControl);
    }
    *control = NULL;
}

NITFAPI(const char**) LibjpegDecompress_init(nitf_Error *error)
{
    (void)error;

    /*  Return the identifier structure  */
    return ident;
}

NITFAPI(void) C3_cleanup(void)
{
}
NITFAPI(void*) C3_construct(char *compressionType,
                            nitf_Error* error)
{
    if (strcmp(compressionType, "C3") != 0)
    {
        nitf_Error_init(error,
                        "Unsupported compression type",
                        NITF_CTXT,
                        NITF_ERR_DECOMPRESSION);

        return NULL;
    }
    return((void *) &interfaceTable);
}

NITFAPI(void) M3_cleanup(void)
{
}

NITFAPI(void*) M3_construct(char *compressionType,
                            nitf_Error* error)
{
    if (strcmp(compressionType, "M3") != 0)
    {
        nitf_Error_init(error,
                        "Unsupported compression type",
                        NITF_CTXT,
                        NITF_ERR_DECOMPRESSION);

        return NULL;
    }
    return((void *) &interfaceTable);
}


/*!
 *  \struct JPEGQuantTable
 *  \brief  In the event that we use a NITF default QT
 *
 *  This object is provided so that we may apply a QT from
 *  the NITF specification.  The predefined ones are listed
 *  above.
 *
 *  \todo This cannot be applied until the decompression interface
 *  changes to allow in the compression ratio
 */
typedef struct _JPEGQuantTable
{
    int* qtablePointer;
}
JPEGQuantTable;


/*!
 *  \fn nitf_JPEGQuantTable_construct
 *
 *  This method constructs a new quantization table.  In the
 *  event of an error, the error object is populated and a NULL value
 *  is returned.
 *
 *  \param error The object to populate if something bad happens
 *  \return A new JPEGQuantTable or NULL on failure.
 */

NITFPRIV(JPEGQuantTable*) JPEGQuantTable_construct(float compressionRatio,
        nitf_Error* error)
{
    JPEGQuantTable* qt = (JPEGQuantTable*)NITF_MALLOC(sizeof(JPEGQuantTable));
    if (! qt )
    {
        nitf_Error_init(error, NITF_STRERROR( NITF_ERRNO ),
                        NITF_CTXT, NITF_ERR_DECOMPRESSION);
        return NULL;
    }

    if (compressionRatio > 0.0 && compressionRatio <= 0.1)
    {
        qt->qtablePointer = Q1;
    }
    else if (compressionRatio > 0.1 && compressionRatio <= 0.2)
    {
        qt->qtablePointer = Q2;
    }
    else if (compressionRatio > 0.3 && compressionRatio <= 0.3)
    {
        qt->qtablePointer = Q3;
    }
    else if (compressionRatio > 0.3 && compressionRatio <= 0.4)
    {
        qt->qtablePointer = Q4;
    }
    else if (compressionRatio > 0.4 && compressionRatio <= 0.5)
    {
        qt->qtablePointer = Q5;
    }
    else
    {
        qt->qtablePointer = NULL;
    }
    return qt;
}

/*!
 *  \fn JPEGQuantTable_destruct
 *
 *  Destroy a JPEGQuantTable object, if non-null.  Then
 *  set the object to NULL.
 *
 *  \param qt  The quantization table to destroy
 *
 */
NITFPRIV(void) JPEGQuantTable_destruct(JPEGQuantTable** qt)
{
    if ( *qt )
    {
        NITF_FREE( *qt );
        *qt = NULL;
    }
}


/*!
 *  This is a debugging utility.  It just dumps the
 *  contents of a buffer as hex.  It was originally created
 *  to find a problem with pointer indirection
 *  \param buffer A buffer to dump out
 *  \param size The size of the buffer to dump out
 *

NITFPRIV(void) hexDump(char *buffer, int size)
{
    int i;
    for (i = 0; i < size; i++)
    {
        printf("%x ", buffer[i]);
    }
    printf("\n");
}

*/
