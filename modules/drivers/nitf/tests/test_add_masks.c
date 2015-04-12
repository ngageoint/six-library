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

/*
    Test program to add block and pad masks to the image segments of a NITF file

  This program reads a input NITF file and writes a new file with block and
  pad masks added to each image segment. Only image segments are written

  The data for the image segments is read into memory so the file can't be
  super big.

  The calling sequence is:

    test_add_mask inputFile outputFile

  The program can also remove a mask:

    test_add_mask [-r] [-p padValue] inputFile outputFile

    The -r switch causes the program to remove the mask

    The -p padValue switch allows the user to specify the pad pixel
    value. The pad value is a hex string (with leading 0x) which gives
    the pad value in big endian format. i.e., for 2 byte pixels:

        -p 0x1122

    The value is usd for all image segments should be long enough for the
    longest pixel value in any image segment.

    The -p and -r options are mutually exclusive
*/

#include <import/nitf.h>

/* This structure controls needed information about each iamge segment */

#define MAX_PAD 40

typedef struct _imgInfo
{
    nitf_Uint32 index;               /* Segment index */
    nitf_ImageSegment *seg;          /* Image segment object */
    nitf_ImageSubheader *subhdr;     /* Image subheader object */
    nitf_Uint32 nBands;              /* Number of bands */
    nitf_Uint32 bytes;               /* Bytes/pixel */
    size_t imgSize;                  /* Size of each band in bytes */
    nitf_Uint8 padValue[MAX_PAD];    /* Pad value */
    nitf_Uint8 **buffers;            /* Buffers containing the data */
    nitf_ImageWriter *imgWriter;     /* Image writer for segment */
    nitf_ImageSource *imgSource;     /* Image source for segment */
}
imgInfo;

/*  Local function declarations (definitions at the end) */

NITF_BOOL readImageSegment(imgInfo *img, nitf_Reader *reader, nitf_Error *error);

NITF_BOOL makeImageSource(imgInfo *img, nitf_Writer *writer, nitf_Error *error);

NITF_BOOL decodePadValue(imgInfo *info,
                         char *string, nitf_Uint8 *value, nitf_Error *error);

int main(int argc, char *argv[])
{
    char *inFile;                 /* Input filename */
    char *outFile;                /* Output filename */
    char *compCode;               /* Compression code NC or NM */
    char *padValueString = NULL;  /* Pad value string */
    nitf_Reader *reader;          /* Reader object */
    nitf_Record *record;          /* Record used for input and output */
    imgInfo *imgs;                /* Image segment information */
    nitf_FileHeader *fileHdr;     /* File header */
    nitf_Uint32 numImages;        /* Total number of image segments */
    nitf_ListIterator imgIter;    /* Image segment list iterator */
    nitf_IOHandle in;             /* Input I/O handle */
    nitf_IOHandle out;            /* Output I/O handle */
    nitf_Writer *writer;          /* Writer object for output */
    static nitf_Error errorObj;   /* Error object for messages */
    nitf_Error *error;            /* Pointer to the error object */
    int i;

    error = &errorObj;

    compCode = "NM";
    if (argc == 3)
    {
        inFile = argv[1];
        outFile = argv[2];
    }
    else if (argc == 4)
    {
        if (strcmp(argv[1], "-r") != 0)
        {
            fprintf(stderr,
                    "Usage %s [-r] [-p padValue] inputFile outputFile\n", argv[0]);
            exit(EXIT_FAILURE);
        }
        inFile = argv[2];
        outFile = argv[3];
        compCode = "NC";
    }
    else if (argc == 5)
    {
        if (strcmp(argv[1], "-p") != 0)
        {
            fprintf(stderr,
                    "Usage %s [-r] [-p padValue] inputFile outputFile\n", argv[0]);
            exit(EXIT_FAILURE);
        }
        inFile = argv[3];
        outFile = argv[4];
        padValueString = argv[2];
    }
    else
    {
        fprintf(stderr, "Usage %s [-r] inputFile outputFile\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    /*    Get the input record */

    in = nitf_IOHandle_create(inFile,
                              NITF_ACCESS_READONLY, NITF_OPEN_EXISTING, error);
    if (NITF_INVALID_HANDLE(in))
    {
        nitf_Error_print(error, stderr, "Error opening input ");
        exit(EXIT_FAILURE);
    }

    reader = nitf_Reader_construct(error);
    if (!reader)
    {
        nitf_Error_print(error, stderr, "Error creating reader ");
        exit(EXIT_FAILURE);
    }

    record = nitf_Reader_read(reader, in, error);
    if (!record)
    {
        nitf_Error_print(error, stderr, "Error reading input ");
        exit(EXIT_FAILURE);
    }
    fileHdr = record->header;

    nitf_Field_get(fileHdr->numImages,
                   &numImages, NITF_CONV_UINT, NITF_INT32_SZ, error);

    /*  Get information about all image segments and set-up for write */

    imgs = (imgInfo *) NITF_MALLOC(numImages * sizeof(imgInfo));
    if (imgs == NULL)
    {
        nitf_Error_print(error, stderr, "Error allocating image info ");
        exit(EXIT_FAILURE);
    }

    for (i = 0;i < numImages;i++)
    {
        imgs[i].index = i;
        imgIter = nitf_List_at(record->images, i);
        imgs[i].seg = (nitf_ImageSegment *) nitf_ListIterator_get(&imgIter);
        if (!imgs[i].seg)
        {
            fprintf(stderr, "No Image segment in file\n");
            exit(EXIT_FAILURE);
        }
        imgs[i].subhdr = imgs[i].seg->subheader;

        if (padValueString != NULL)
        {
            if (!decodePadValue(&(imgs[i]), padValueString, imgs[i].padValue, error))
            {
                nitf_Error_print(error, stderr, "Error allocating image info ");
                exit(EXIT_FAILURE);
            }
        }
        else
            memset(imgs[i].padValue, 0, MAX_PAD);
    }

    /*   Read all of the image data into buffers */

    for (i = 0;i < numImages;i++)
        if (!readImageSegment(&(imgs[i]), reader, error))
        {
            nitf_Error_print(error, stderr, "Error reading image segment ");
            exit(EXIT_FAILURE);
        }

    /*  Set compression type to NM or NC (has to happen afetr read) */

    for (i = 0;i < numImages;i++)
    {

        if (!nitf_ImageSubheader_setCompression(imgs[i].subhdr, compCode, "", error))
        {
            nitf_Error_print(error, stderr, "No DE segment in file ");
            exit(EXIT_FAILURE);
        }
    }

    /*   Create output */

    out = nitf_IOHandle_create(outFile, NITF_ACCESS_WRITEONLY, NITF_CREATE, error);
    if (NITF_INVALID_HANDLE(out))
    {
        nitf_Error_print(error, stderr, "Could not create output file ");
        exit(EXIT_FAILURE);
    }

    /*   Setup write and write */

    writer = nitf_Writer_construct(error);
    if (writer == NULL)
    {
        nitf_Error_print(error, stderr, "Write setup failed ");
        exit(EXIT_FAILURE);
    }

    if (!nitf_Writer_prepare(writer, record, out, error))
    {
        nitf_Error_print(error, stderr, "Write setup failed ");
        exit(EXIT_FAILURE);
    }

    for (i = 0;i < numImages;i++)
        if (!makeImageSource(&(imgs[i]), writer, error))
        {
            nitf_Error_print(error, stderr, "Write setup failed ");
            exit(EXIT_FAILURE);
        }

    /*  Set pad pixel if padValue string is not NULL */

    if (padValueString != NULL)
        for (i = 0;i < numImages;i++)
        {
        }

    if (!nitf_Writer_write(writer, error))
    {
        nitf_Error_print(error, stderr, "Write error ");
        exit(EXIT_FAILURE);
    }


    /*    Clean-up */

    nitf_Record_destruct(&record);
    nitf_IOHandle_close(out);
    return 0;

}

/*
  Read Image segment - Read the image segment image data

  All of the information accept the handle is in the imgInfo

  True returned on success
*/

NITF_BOOL readImageSegment(imgInfo *img, nitf_Reader *reader, nitf_Error *error)
{
    nitf_Uint32 nBits;         /* Bits/pixel */
    nitf_Uint32 nBands;        /* Number of bands */
    nitf_Uint32 xBands;        /* Number of extended bands */
    nitf_Uint32 nRows;         /* Number of rows */
    nitf_Uint32 nColumns;      /* Number of columns */
    size_t subimageSize;       /* Image band size in bytes */
    nitf_SubWindow *subimage;  /* Sub-image object specifying full image */
    nitf_DownSampler *pixelSkip; /* Downsample for sub-window */
    nitf_Uint32 *bandList;     /* List of bands for read */
    nitf_Uint32 band;          /* Current band */
    nitf_Uint8 **buffers;      /* Read buffer one/band */
    /* Image reader */
    nitf_ImageReader *deserializer;
    int padded;                /* Argument for read */
    int i;


    /*  Get dimension and band info */

    nitf_Field_get(img->subhdr->numBitsPerPixel,
                   &nBits, NITF_CONV_UINT, NITF_INT32_SZ, error);
    nitf_Field_get(img->subhdr->numImageBands,
                   &nBands, NITF_CONV_UINT, NITF_INT32_SZ, error);
    nitf_Field_get(img->subhdr->numMultispectralImageBands,
                   &xBands, NITF_CONV_UINT, NITF_INT32_SZ, error);
    nBands += xBands;
    nitf_Field_get(img->subhdr->numRows,
                   &nRows, NITF_CONV_UINT, NITF_INT32_SZ, error);
    nitf_Field_get(img->subhdr->numCols,
                   &nColumns, NITF_CONV_UINT, NITF_INT32_SZ, error);
    img->bytes = NITF_NBPP_TO_BYTES(nBits);
    subimageSize = nRows * nColumns * img->bytes;
    img->imgSize = subimageSize;


    /*  Setup sub-window */

    subimage = nitf_SubWindow_construct(error);
    if (subimage == NULL)
        return(NITF_FAILURE);

    bandList = (nitf_Uint32 *) NITF_MALLOC(sizeof(nitf_Uint32 *) * nBands);
    if (bandList == NULL)
        return(NITF_FAILURE);

    subimage->startCol = 0;
    subimage->startRow = 0;
    subimage->numRows = nRows;
    subimage->numCols = nColumns;
    for (band = 0;band < nBands;band++)
        bandList[band] = band;
    subimage->bandList = bandList;
    subimage->numBands = nBands;

    pixelSkip = nitf_PixelSkip_construct(1, 1, error);
    if (pixelSkip == NULL)
        return(NITF_FAILURE);

    if (!nitf_SubWindow_setDownSampler(subimage, pixelSkip, error))
        return(NITF_FAILURE);

    /*  Set-up buffers (one/band) */

    buffers = (nitf_Uint8 **) NITF_MALLOC(nBands * sizeof(nitf_Uint8*));
    if (buffers == NULL)
        return(NITF_FAILURE);

    for (i = 0;i < nBands;i++)
    {
        buffers[i] = (nitf_Uint8 *) malloc(subimageSize);
        if (buffers[i] == NULL)
            return(NITF_FAILURE);
    }

    deserializer = nitf_Reader_newImageReader(reader, img->index, error);
    if (deserializer == NULL)
        return(NITF_FAILURE);

    if (!nitf_ImageReader_read(deserializer, subimage, buffers, &padded, error))
        return(NITF_FAILURE);

    img->nBands = nBands;
    img->buffers = buffers;
    return(NITF_SUCCESS);
}

NITF_BOOL makeImageSource(imgInfo *img, nitf_Writer *writer, nitf_Error *error)
{
    nitf_ImageSource *source;     /* Image source for image writer */
    nitf_BandSource *bandSource;  /* Current band source */
    int i;

    img->imgWriter = nitf_Writer_newImageWriter(writer, img->index, error);
    source = nitf_ImageSource_construct(error);
    if (source == NULL)
        return(NITF_FAILURE);

    for (i = 0;i < img->nBands;i++)
    {
        bandSource = nitf_MemorySource_construct((char *) (img->buffers[i]),
                     img->imgSize, (nitf_Off) 0, 0, 0, error);
        if (!bandSource)
            return(NITF_FAILURE);

        if (!nitf_ImageSource_addBand(source, bandSource, error))
            return(NITF_FAILURE);
    }

    img->imgSource = source;

    if (!nitf_ImageWriter_attachSource(img->imgWriter, source, error))
        return(NITF_FAILURE);

    nitf_ImageIO_setPadPixel(img->imgWriter->imageBlocker,
                             img->padValue, img->bytes);

    return(NITF_SUCCESS);
}

NITF_BOOL decodePadValue(imgInfo *info,
                         char *string, nitf_Uint8 *value, nitf_Error *error)
{
    char *str;           /* Pointer into the string */
    size_t len;          /* Current length of the value string */
    nitf_Uint32 nValues; /* Number of byte values in pad value */
    nitf_Uint32 i;

    str = string;
    len = strlen(str);
    nValues = (len - 2) / 2;

    if (
        (len <= 2)                 /* Must be more that 2 characters */
        || ((len & 1) != 0)          /* String length must be even */
        || (nValues > (MAX_PAD - 1))   /* Specifcation is too long */
    )
    {
        nitf_Error_initf(error, NITF_CTXT, NITF_ERR_INVALID_PARAMETER,
                         "Error decoding pad value: %s", string);
        return (NITF_FAILURE);
    }

    if ((str[0] != '0') || ((str[1] != 'x') && (str[1] != 'X')))
    {
        nitf_Error_initf(error, NITF_CTXT, NITF_ERR_INVALID_PARAMETER,
                         "Error decoding pad value: %s", string);
        return (NITF_FAILURE);
    }
    str += 2;
    len -= 2;

    for (i = 0;i < nValues;i++)
    {
        int currentByte;     /* Current byte in value */
        char val[3];         /* Current byte as characters */

        val[0] = tolower(*str);
        str += 1;
        val[1] = tolower(*str);
        str += 1;
        val[2] = 0;
        if (sscanf(val, "%x", &currentByte) != 1)
        {
            nitf_Error_initf(error, NITF_CTXT, NITF_ERR_INVALID_PARAMETER,
                             "Error decoding pad value: %s", string);
            return (NITF_FAILURE);
        }
        *(value++) = currentByte;
    }

    return(NITF_SUCCESS);
}

