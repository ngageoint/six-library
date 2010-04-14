/* =========================================================================
 * This file is part of NITRO
 * =========================================================================
 *
 * (C) Copyright 2004 - 2008, General Dynamics - Advanced Information Systems
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
#include <jasper/jasper.h>

/*!
 *  \file JasPerDecompress.c - Decompression plugin for jpeg2000
 *  using the jasper library,
 *
 *  \brief The jasper decompression plugin follows the decompression
 *  plugin
 *
 *  specification given in nitf/PluginRegistry.h and nitf/ImageIO.h
 *
 *  Compression type supported:
 *
 *   C8 - Jpeg 2000
 *
 *  This is the simplest version.It decompresses the entire image into a
 *  memory buffer the first time the image data is accessed. It does not
 *  present the compression tiling structure as the block organization and
 *  reports the image as one big block.
 */

/*  Prototypes  */
NITFPRIV(nitf_DecompressionControl*) implOpen(nitf_IOInterface*  io,
                                              nitf_Uint64        offset,
                                              nitf_Uint64        fileLength,
                                              nitf_BlockingInfo* blockInfo,
                                              nitf_Uint64*       blockMask,
                                              nitf_Error*        error);

NITFPRIV(nitf_Uint8*) implReadBlock(nitf_DecompressionControl *control,
                                    nitf_Uint32 blockNumber,
                                    nitf_Error* error);
NITFPRIV(int) implFreeBlock(nitf_DecompressionControl* control,
                            nitf_Uint8* block,
                            nitf_Error* error);

NITFPRIV(void) implClose(nitf_DecompressionControl** control);


NITF_CXX_GUARD
/*
 *  This is the identifier for the plugin.  The first
 *  value must be either "COMPRESSION," "DECOMPRESSION," or "TRE"
 *  to signify which area the plugin fills
 */
static char *ident[] =
    {
        "DECOMPRESSION",  /*  This is a decompression handler  */
        "C8",             /*  JPEG2000 is C8 in the NITF spec  */
        NULL              /*  Need to NULL terminate this      */
    };



/*  Function prototypes  */
static nitf_DecompressionInterface interfaceTable =
    {
        implOpen,
        implReadBlock,
        implFreeBlock,
        implClose,
        NULL
    };

typedef struct _ImplControl
{
    nitf_BlockingInfo blockInfo; /* Kept for convience */
    nitf_Uint8 *data;            /* The block is kept here after decoding */
    nitf_Uint64 offset;          /* File offset to data */
    nitf_Uint64 fileLength;      /* Length of compressed data in file */
}
ImplControl;

NITF_CXX_ENDGUARD


/*!
 *  This function is required by the plugin manager.
 *  It is called at plugin manager load time.  In our
 *  case, we call the initialization function for jasper
 *  \param error An error to populate on failure.
 *  \return The identifier structure
 *
 */
NITFAPI(char**) JasPerDecompress_init(nitf_Error *error)
{

    /*  Initialize jasper  */
    if (jas_init())
    {
        /*  Or initialize an error!  */
        nitf_Error_init(error,
                        "Error initializing jasper library",
                        NITF_CTXT,
                        NITF_ERR_DECOMPRESSION);
        return NULL;
    }

    /*  Return the identifier structure  */
    return ident;
}

/*!
 *  This function is required by the plugin manager, and is
 *  called at unload time.
 *
 */
NITFAPI(void) C8_cleanup(void)
{

    /*  Cleanup the jasper library  */
    jas_cleanup();
}




/*!
 *  Interface free block function
 *
 *  Since the block is maintained by the contol structure until it is freed,
 *  this function does nothing
 */

NITFPRIV(int) implFreeBlock(nitf_DecompressionControl* control,
                            nitf_Uint8* block,
                            nitf_Error* error)
{
    return 1;
}


NITFAPI(void*) C8_construct(char *compressionType,
                            nitf_Error* error)
{
    if (strcmp(compressionType, "C8") != 0)
    {
        nitf_Error_init(error,
                        "Unsupported compression type",
                        NITF_CTXT,
                        NITF_ERR_DECOMPRESSION);

        return NULL;
    }

    return((void *) &interfaceTable);
}

/*
 *  Interface read block function
 *
 *  This function just returns the block pointer in the parent control
 */
NITFPRIV(nitf_Uint8*) implReadBlock(nitf_DecompressionControl *control,
                                    nitf_Uint32 blockNumber,
                                    nitf_Error* error)
{
    nitf_Uint32 blockRow, blockCol, i, blockRowLength, colBlockOffset, colBlockSkip;
    nitf_Uint64 imageOffset, bufOffset;
    
    ImplControl* implControl = (ImplControl*)control;
    nitf_Uint8* buf = NULL;
    
    /* the length, in bytes of one row */
    blockRowLength = implControl->blockInfo.length / implControl->blockInfo.numRowsPerBlock;
    
    buf = NITF_MALLOC(implControl->blockInfo.length);
    bufOffset = 0;
    
    blockRow = blockNumber / implControl->blockInfo.numBlocksPerRow;
    blockCol = blockNumber % implControl->blockInfo.numBlocksPerRow;
    
    /* skip past row blocks */
    imageOffset = blockRow * implControl->blockInfo.numBlocksPerRow * implControl->blockInfo.length;
    colBlockOffset = blockCol * blockRowLength;
    colBlockSkip = (implControl->blockInfo.numBlocksPerRow - blockCol) * blockRowLength;
    
    /* loop over the rows in the block */
    for (i = 0; i < implControl->blockInfo.numRowsPerBlock; ++i)
    {
        imageOffset += colBlockOffset;
        memcpy(buf + bufOffset, implControl->data + imageOffset, blockRowLength);
        bufOffset += blockRowLength;
        imageOffset += colBlockSkip;
    }
    
    return buf;
}

NITFPRIV(void*) implMemAlloc(size_t size, nitf_Error* error)
{
    void * p = NITF_MALLOC(size);
    memset(p, 0, size);
    if (!p)
    {
        nitf_Error_init(error,
                        NITF_STRERROR( NITF_ERRNO ),
                        NITF_CTXT,
                        NITF_ERR_MEMORY);
    }
    return p;
}

NITFPRIV(void) implMemFree(void* p)
{
    if (p) NITF_FREE(p);
}

/* Place this above readJPEG2000 */
#define PRIV_READ_BUF(_SZ) \
nitf_Uint##_SZ* data##_SZ = (nitf_Uint##_SZ*)outputp; \
for (i = 0; i < numRows; ++i) { \
    for (j = 0; j < numCols; ++j) { \
            *data##_SZ++ = \
                (nitf_Uint##_SZ)jas_image_readcmptsample(image, component, j, i); \
    } \
}


#define PRIV_READ_MATRIX(_SZ) { \
nitf_Uint##_SZ* data##_SZ = (nitf_Uint##_SZ*)outputp; \
jas_matrix_t* matrix##_SZ = jas_matrix_create(numRows, numCols); \
jas_image_readcmpt (image, component, 0, 0, numCols, numRows, matrix##_SZ); \
for (i = 0; i < numRows; ++i){ \
    for (j = 0; j < numCols; ++j){ \
        *data##_SZ++ = (nitf_Uint##_SZ)jas_matrix_get(matrix##_SZ, i, j); \
    } \
} \
jas_matrix_destroy (matrix##_SZ); }


NITFPRIV(int) readJPEG2000(nitf_Uint8 *input,
                           nitf_Uint32 inputLen,
                           nitf_Uint8 **output,
                           nitf_Uint64 *outputLen,
                           nitf_Uint64 reserveSize,
                           nitf_Error *error)
{
    jas_stream_t *inStr;   /* Jasper input stream */
    int fmtid;             /* Format ID */
    char *fmtname;         /* Format name */
    jas_image_t *image;    /* The input image */
    /* Number of image components */
    nitf_Uint32 numComponents;
    nitf_Uint32 i, j;
    nitf_Uint32 numRows;   /* Number of rows */
    nitf_Uint32 numCols;   /* Number of columns */
    nitf_Uint32 numBits;   /* Number of bits/pixel */
    nitf_Uint32 numBytes;  /* Number of bytes/pixel */
    /* Length of a component in bytes */
    nitf_Uint64 componentLength;
    nitf_Uint8 *outputp;   /* Pointer into the output */
    jas_stream_t *outStr;  /* Output stream */
    nitf_Uint32 component; /* Current component (index) */
    jas_image_fmtinfo_t *fmtinfo;

    /*      Open jasper input stream */

    inStr = jas_stream_memopen(input, inputLen);
    if (inStr == NULL)
    {
        nitf_Error_init(error,
                        "Error creating Jasper input stream",
                        NITF_CTXT,
                        NITF_ERR_DECOMPRESSION);

        return(0);
    }

    /*      Get format identifer and name */

    if ((fmtid = jas_image_getfmt(inStr)) < 0)
    {
        nitf_Error_init(error,
                        "Error in Jasper reading format ID",
                        NITF_CTXT,
                        NITF_ERR_DECOMPRESSION);

        return 0;
    }

    fmtname = jas_image_fmttostr(fmtid);
    fmtinfo = jas_image_lookupfmtbyid(fmtid);
    if (fmtname == NULL)
    {
        nitf_Error_init(error,
                        "Error in Jasper, invalid format ID",
                        NITF_CTXT,
                        NITF_ERR_DECOMPRESSION);
        return 0;
    }


    /*      Decode the stream */
    image = jas_image_decode(inStr, fmtid, NULL);
    if (image == NULL)
    {
        nitf_Error_initf(error,
                           NITF_CTXT,
                           NITF_ERR_DECOMPRESSION,
                           "Error in Jasper decoding image with format %s",
                           fmtname);
        return 0;
    }

    numComponents = jas_image_numcmpts(image);
    numRows       = jas_image_cmptheight(image, 0);
    numCols       = jas_image_cmptwidth(image, 0);
    numBits       = jas_image_cmptprec(image, 0);
    numBytes      = ((numBits - 1) / 8) + 1;

    /*      Make the output buffer */
    componentLength = 
        (nitf_Uint64)numRows * (nitf_Uint64)numCols * (nitf_Uint64)numBytes;
    *outputLen = componentLength * numComponents;
    *output = implMemAlloc(reserveSize, error);
    if (*output == NULL) return 0;

    /*      Copy the result to the output buffer */
    outputp = *output;
    /*printf("component size: [%d]\n", componentLength);*/
    for (component = 0; component < numComponents; component++)
    {
        /*jas_stream_memobj_t* memobj = NULL;*/

        numRows       = jas_image_cmptheight(image, component);
        numCols       = jas_image_cmptwidth(image, component);
        assert(jas_image_cmptprec(image, component) == numBits);

        /*printf("Real Rows [%d]\n", numRows);
        printf("Real Cols [%d]\n", numCols);*/

        /*outStr = image->cmpts_[component]->stream_;
        memobj = (jas_stream_memobj_t*) outStr->obj_;*/
        /*jas_stream_flush(outStr);*/

        /*printf("Total bufsize for component #%d: [%d]\n", component,
               memobj->bufsize_);*/

        /* When we skip, we leave zeros from (c)alloc */
        /*memcpy(outputp, outStr->ptr_, outStr->cnt_);*/
        /*memcpy(outputp, memobj->buf_, memobj->bufsize_);*/

        if (numBytes == 1)
        {
            /*PRIV_READ_BUF(8);*/
            PRIV_READ_MATRIX(8);
        }
        else if (numBytes == 2)
        {
            /*PRIV_READ_BUF(16);*/
            PRIV_READ_MATRIX(16);
        }
        else if (numBytes == 4)
        {
            /*PRIV_READ_BUF(32);*/
            PRIV_READ_MATRIX(32);
        }
        else
        {
            nitf_Error_init(error,
                            "Unknown pixel size",
                            NITF_CTXT,
                            NITF_ERR_DECOMPRESSION);
            return 0;
        }

        outputp += componentLength;
    }

    /*      Cleanup memory */
    jas_stream_close(inStr);
    jas_image_destroy(image);
    return 1;
}



NITFPRIV(int) decode(ImplControl* implControl,
                     nitf_IOInterface* io,
                     nitf_Error* error)
{
    nitf_Uint8 *input;        /* Input (compressed) image data */
    nitf_Uint8 *output;       /* Output (decompressed) image data */
    nitf_Uint64 outputLen;    /* Output (decompressed) image data */
    nitf_Uint64 expected = 
        (nitf_Uint64)(implControl->blockInfo.length) *
        (nitf_Uint64)(implControl->blockInfo.numBlocksPerRow) *
        (nitf_Uint64)(implControl->blockInfo.numBlocksPerCol);

    int check;

    /*      Seek the handle to the start of data */
    nitf_Off found = nitf_IOInterface_seek(io,
                                           implControl->offset,
                                           NITF_SEEK_SET, error);

    if (!NITF_IO_SUCCESS(found)) return 0;

    /*  Allocate read buffer */
    input = implMemAlloc(implControl->fileLength, error);
    if (input == NULL) return 0;

    /*  Read compressed data */

    check = nitf_IOInterface_read(io, (char*)input, 
                                  implControl->fileLength, error);

    if (!NITF_IO_SUCCESS(check)) return 0;

    /*  Decompress image */
    check = readJPEG2000(input, 
                         (nitf_Uint32) (implControl->fileLength),
                         &output,
                         &outputLen,
                         expected,
                         error);

    /*  Free input buffer  */
    implMemFree(input);
    input = NULL;
    if (!check) return 0;
    if (outputLen > expected)
    {
        nitf_Error_initf(error,
                           NITF_CTXT,
                           NITF_ERR_MEMORY,
                           "Bad decompressed block size %lld. (expected %lld)",
                           outputLen,
                           expected);
        return 0;
    }
    implControl->data = output;
    return 1;
}



NITFPRIV(void) implClose(nitf_DecompressionControl** control)
{
    ImplControl *implControl;   /* The object */
    implControl = (ImplControl *) * control;
    if (implControl->data != NULL)
    {
        implMemFree(implControl->data);
        implControl->data = NULL;
    }

    implMemFree((void *)(*control));
    *control = NULL;
}
NITFPRIV(nitf_DecompressionControl*) implOpen(nitf_IOInterface* io,
                                              nitf_Uint64        offset,
                                              nitf_Uint64        fileLength,
                                              nitf_BlockingInfo* blockInfo,
                                              nitf_Uint64*       blockMask,
                                              nitf_Error*        error)
{
    ImplControl *implControl;    /* The answer */

    /*      Allocate the result */
    implControl = (ImplControl *) NITF_MALLOC(sizeof(ImplControl));
    if (implControl == NULL)
    {
        nitf_Error_init(error,
                        "Error creating control object",
                        NITF_CTXT,
                        NITF_ERR_DECOMPRESSION);

        return NULL;
    }
    implControl->offset     = offset;
    implControl->data       = NULL;
    implControl->fileLength = fileLength;
    implControl->blockInfo  = *blockInfo;

    /*     Decode data */
    if ( ! decode(implControl, io, error) )
        return NULL;

    /*  Set it at the end  */
    *blockInfo = implControl->blockInfo;
    return((nitf_DecompressionControl*) implControl);

}
