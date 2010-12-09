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

int gAlerts = 1;

typedef enum _CLEVEL
{
    CLEVEL_CHECK_FAILED = 0,
    CLEVEL_03,
    CLEVEL_05,
    CLEVEL_06,
    CLEVEL_07,
    CLEVEL_09,
    CLEVEL_UNKNOWN
} CLEVEL;

#define TRY_IT(X) if (! X ) return CLEVEL_CHECK_FAILED
typedef CLEVEL (*CLEVEL_CHECK_PTR)(nitf_Record*, nitf_Error*);


void showMessage(const char* format, ...)
{
    if (gAlerts)
    {
        va_list ap;
        va_start(ap, format);

        vfprintf(stdout, format, ap);
        va_end(ap);
    }

}

CLEVEL checkILOC(nitf_ImageSubheader* subhdr, nitf_Error* error)
{
    char iloc[NITF_ILOC_SZ + 1];
    char num[6];
    int rowCoord, colCoord;
    int rowExtent, colExtent;
    int nrows, ncols;
    TRY_IT( nitf_Field_get(subhdr->imageLocation,
                           iloc, NITF_CONV_STRING, NITF_ILOC_SZ + 1, error) );
    num[5] = 0;
    memcpy(num, iloc, 5);
    rowCoord = atoi(num);
    memcpy(num, &iloc[5], 5);
    colCoord = atoi(num);

    subhdr->numRows;
    subhdr->numCols;

    TRY_IT( nitf_Field_get(subhdr->numRows,
                           &nrows, NITF_CONV_INT, sizeof(int), error) );
    TRY_IT( nitf_Field_get(subhdr->numCols,
                           &ncols, NITF_CONV_INT, sizeof(int), error) );

    rowExtent = rowCoord + nrows;
    colExtent = colCoord + ncols;

    if (rowExtent <= 2047 && colExtent <= 2047)
        return CLEVEL_03;

    if (rowExtent <= 8191 && colExtent <= 8191)
        return CLEVEL_05;

    if (rowExtent <= 65535 && colExtent <= 65535)
        return CLEVEL_06;

    if (rowExtent <= 99999999 && colExtent <= 99999999)
        return CLEVEL_07;

    else return CLEVEL_09;

}

CLEVEL checkCCSExtent(nitf_Record* record, nitf_Error* error)
{
    int clevel = CLEVEL_03;

    nitf_ListIterator it = nitf_List_begin(record->images);
    nitf_ListIterator end = nitf_List_end(record->images);
    int i = 0;
    while ( nitf_ListIterator_notEqualTo(&it, &end) )
    {
        nitf_ImageSegment* imageSegment = (nitf_ImageSegment*)nitf_ListIterator_get(&it);
        int result = checkILOC(imageSegment->subheader, error);
        if ( result == CLEVEL_CHECK_FAILED ) return result;
        if ( result > clevel )
        {
            showMessage("Image [%d]'s CCS elevated CLEVEL\n", i);
            clevel = result;
        }
        ++i;
        nitf_ListIterator_increment(&it);
    }

    return clevel;
}


CLEVEL checkFileSize(nitf_Record* record, nitf_Error* error)
{
    nitf_Int64 fl;
    TRY_IT( nitf_Field_get(record->header->fileLength,
                           &fl, NITF_CONV_INT, 8, error ));
    if (fl <= 52428799)
    {
        return CLEVEL_03;
    }
    if (fl <= 1073741823)
    {
        return CLEVEL_05;
    }
    if (fl <= 2147483647)
    {
        return CLEVEL_06;
    }
    if (fl <= NITF_INT64(10737418239))
    {
        return CLEVEL_07;
    }
    return CLEVEL_09;
}



CLEVEL checkImage(nitf_ImageSubheader* subhdr, nitf_Error* error)
{
    int nrows, ncols;

    TRY_IT( nitf_Field_get(subhdr->numRows,
                           &nrows, NITF_CONV_INT, sizeof(int), error) );
    TRY_IT( nitf_Field_get(subhdr->numCols,
                           &ncols, NITF_CONV_INT, sizeof(int), error) );


    if (nrows <= 2047 && ncols <= 2047)
        return CLEVEL_03;

    if (nrows <= 8191 && ncols <= 8191)
        return CLEVEL_05;

    if (nrows <= 65535 && ncols <= 65535)
        return CLEVEL_06;

    if (nrows <= 99999999 && ncols <= 99999999)
        return CLEVEL_07;

    else return CLEVEL_09;

}




CLEVEL checkImageSize(nitf_Record* record, nitf_Error* error)
{
    int clevel = CLEVEL_03;

    nitf_ListIterator it = nitf_List_begin(record->images);
    nitf_ListIterator end = nitf_List_end(record->images);
    int i = 0;
    while ( nitf_ListIterator_notEqualTo(&it, &end) )
    {
        nitf_ImageSegment* imageSegment =
            (nitf_ImageSegment*)nitf_ListIterator_get(&it);
        int result = checkImage(imageSegment->subheader, error);
        if ( result == CLEVEL_CHECK_FAILED ) return result;
        if ( result > clevel )
        {
            showMessage("Image [%d]'s size elevated CLEVEL\n", i);
            clevel = result;
        }
        ++i;
        nitf_ListIterator_increment(&it);
    }

    return clevel;
}

CLEVEL checkImageBlock(nitf_ImageSubheader* subhdr, nitf_Error* error)
{

    int nppbh, nppbv;
    TRY_IT( nitf_Field_get(subhdr->numPixelsPerHorizBlock,
                           &nppbh, NITF_CONV_INT, sizeof(int), error) );
    TRY_IT( nitf_Field_get(subhdr->numPixelsPerVertBlock,
                           &nppbv, NITF_CONV_INT, sizeof(int), error) );

    if (nppbh <= 0 || nppbv <= 0)
        return CLEVEL_09;

    if (nppbh <= 2048 && nppbv <= 2048)
        return CLEVEL_03;

    if (nppbh <= 8192 && nppbv <= 8192)
        return CLEVEL_05;


    return CLEVEL_06;
}



CLEVEL checkBlockSize(nitf_Record* record, nitf_Error* error)
{
    int clevel = CLEVEL_03;

    nitf_ListIterator it = nitf_List_begin(record->images);
    nitf_ListIterator end = nitf_List_end(record->images);
    int i = 0;
    while ( nitf_ListIterator_notEqualTo(&it, &end) )
    {
        nitf_ImageSegment* imageSegment = (nitf_ImageSegment*)nitf_ListIterator_get(&it);
        int result = checkImageBlock(imageSegment->subheader, error);
        if ( result == CLEVEL_CHECK_FAILED ) return result;
        if ( result > clevel )
        {
            showMessage("Image [%d]'s size elevated CLEVEL\n", i);
            clevel = result;
        }
        ++i;
        nitf_ListIterator_increment(&it);
    }

    return clevel;
}



CLEVEL checkDES(nitf_Record* record, nitf_Error* error)
{
    int clevel = CLEVEL_03;

    int numdes;
    TRY_IT( nitf_Field_get(record->header->numDataExtensions,
                           &numdes, NITF_CONV_INT, sizeof(int), error ) );

    if (numdes > 10)
    {
        if (numdes <= 50)
        {
            clevel = CLEVEL_06;
        }
        if (numdes <= 100)
        {
            clevel = CLEVEL_07;
        }
        else clevel = CLEVEL_09;
    }
    return clevel;

}
CLEVEL checkRGBImage(nitf_ImageSubheader* subhdr, nitf_Error* error)
{
    int clevel = CLEVEL_03;
    int nbands, nbpp;
    char imode = subhdr->imageMode->raw[0];
    TRY_IT( nitf_Field_get(subhdr->numImageBands,
                           &nbands, NITF_CONV_INT, sizeof(int), error ) );
    TRY_IT( nitf_Field_get(subhdr->numBitsPerPixel,
                           &nbpp, NITF_CONV_INT, sizeof(int), error ) );

    if (nbands != 3)
    {
        clevel = CLEVEL_09;
    }

    else if (imode != 'B' &&
             imode != 'P' &&
             imode != 'R' &&
             imode != 'S')
    {
        clevel = CLEVEL_09;
    }
    else
    {
        if (nbpp == 8)
        {
            /*clevel = CLEVEL_03;*/
        }
        else if (nbpp == 16 || nbpp == 32)
        {
            clevel = CLEVEL_06;
        }
        else clevel = CLEVEL_09;
    }
    return clevel;

}
CLEVEL checkRGBLUTImage(nitf_ImageSubheader* subhdr, nitf_Error* error)
{
    int clevel = CLEVEL_03;
    int nbands, nbpp;
    char imode = subhdr->imageMode->raw[0];
    TRY_IT( nitf_Field_get(subhdr->numImageBands,
                           &nbands, NITF_CONV_INT, sizeof(int), error ) );
    TRY_IT( nitf_Field_get(subhdr->numBitsPerPixel,
                           &nbpp, NITF_CONV_INT, sizeof(int), error ) );


    if (nbands != 1)
    {
        clevel = CLEVEL_09;
    }

    else if (nbpp != 1 &&
             nbpp != 8)
    {
        clevel = CLEVEL_09;
    }
    else if (imode != 'B')
    {
        clevel = CLEVEL_09;
    }

    return clevel;

}
CLEVEL checkMonoImage(nitf_ImageSubheader* subhdr, nitf_Error* error)
{
    int clevel = CLEVEL_03;
    int nbands, nbpp;
    char imode = subhdr->imageMode->raw[0];
    TRY_IT( nitf_Field_get(subhdr->numImageBands,
                           &nbands, NITF_CONV_INT, sizeof(int), error ) );
    TRY_IT( nitf_Field_get(subhdr->numBitsPerPixel,
                           &nbpp, NITF_CONV_INT, sizeof(int), error ) );

    if (nbands != 1)
    {
        clevel = CLEVEL_09;
    }

    else if (nbpp != 1 &&
             nbpp != 8 &&
             nbpp != 12 &&
             nbpp != 16 &&
             nbpp != 32 &&
             nbpp != 64)
    {
        clevel = CLEVEL_09;
    }
    else if (imode != 'B')
    {
        clevel = CLEVEL_09;
    }

    return clevel;


}

CLEVEL checkSpecificImageAttributes(nitf_Record* record, nitf_Error* error)
{
    CLEVEL clevel = CLEVEL_03;
    nitf_ListIterator it = nitf_List_begin(record->images);
    nitf_ListIterator end = nitf_List_end(record->images);
    int i = 0;
    while ( nitf_ListIterator_notEqualTo(&it, &end) )
    {
        CLEVEL result = CLEVEL_CHECK_FAILED;
        char irep[ NITF_IREP_SZ + 1 ];
        nitf_ImageSegment* imageSegment =
            (nitf_ImageSegment*)nitf_ListIterator_get(&it);
        TRY_IT( nitf_Field_get(imageSegment->subheader->imageRepresentation,
                               irep, NITF_CONV_STRING,
                               NITF_IREP_SZ + 1, error ) );
        if (strcmp( irep, "MONO    ") == 0)
        {
            result = checkMonoImage(imageSegment->subheader, error);
        }
        else if (strcmp( irep, "RGB     ") == 0)
        {
            result = checkRGBImage(imageSegment->subheader, error);
        }
        else if (strcmp( irep, "RGB/LUT ") == 0)
        {
            result = checkRGBLUTImage(imageSegment->subheader, error);
        }
        else
        {
            nitf_Error_init(error, "Unknown IREP Clevel check failed",
                            NITF_CTXT, NITF_ERR_INVALID_OBJECT);
            return result;
        }

        if ( result == CLEVEL_CHECK_FAILED ) return result;
        if (result > clevel)
        {
            showMessage("Image: [%d], elevated CLEVEL\n", i);
            clevel = result;
        }
        ++i;
        nitf_ListIterator_increment(&it);

    }
    return clevel;

}


typedef struct _ComplexityLevelCheck
{
    char* checkName;
    CLEVEL_CHECK_PTR check;
}
ComplexityLevelCheck;

static ComplexityLevelCheck checks[] =
    {
        { "CCS Extent",       checkCCSExtent
        },
        { "File Size",        checkFileSize                },
        { "Image Size",       checkImageSize               },
        { "Block Size",       checkBlockSize               },
        { "DES",              checkDES                     },
        { "Image Attributes", checkSpecificImageAttributes },
        { NULL, NULL }
    };


NITF_BOOL measureComplexityLevel(nitf_Record* record,
                                 CLEVEL* clevel,
                                 nitf_Error* error)
{
    int i = 0;
    int checkComplexity;
    char *p = checks[0].checkName;
    CLEVEL_CHECK_PTR checkToRun = checks[0].check;
    *clevel = CLEVEL_03;

    while ( p != NULL )
    {
        showMessage("Performing check [%s]\n", p);

        checkComplexity = (*checkToRun)(record, error);
        if (checkComplexity == CLEVEL_CHECK_FAILED)
        {
            *clevel = checkComplexity;
            return NITF_FAILURE;
        }
        else if (checkComplexity > *clevel)
        {
            showMessage("Check [%s] elevated complexity level to [%d]\n",
                        p, checkComplexity);
            *clevel = checkComplexity;
        }
        ++i;
        p = checks[i].checkName;
        checkToRun = checks[i].check;
    }

}

char* clevelAsString(CLEVEL clevel)
{

    switch (clevel)
    {
        case CLEVEL_03:
            return "03";
        case CLEVEL_05:
            return "05";
        case CLEVEL_06:
            return "06";
        case CLEVEL_07:
            return "07";
        case CLEVEL_09:
            return "09";
        case CLEVEL_UNKNOWN:
            return "Unknown";
    }
    return "Error";
}

int main(int argc, char** argv)
{
    CLEVEL clevel;
    nitf_Reader* reader;
    nitf_Record* record;
    nitf_IOHandle io;
    nitf_Error error;
    /*  Check argv and make sure we are happy  */
    if ( argc != 2 )
    {
        printf("Usage: %s <nitf-file>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    io = nitf_IOHandle_create(argv[1], NITF_ACCESS_READONLY,
                              NITF_OPEN_EXISTING, &error);

    if ( NITF_INVALID_HANDLE( io ) )
    {
        nitf_Error_print(&error, stdout, "Exiting...");
        exit( EXIT_FAILURE );
    }

    reader = nitf_Reader_construct(&error);
    if (!reader)
    {
        nitf_Error_print(&error, stdout, "Exiting ...");
        nitf_IOHandle_close(io);
        exit( EXIT_FAILURE );
    }

    /* read the record */
    record =  nitf_Reader_read(reader, io, &error  );
    if (!record)
    {
        goto DESTROY_BLOCK;
    }

    if (! measureComplexityLevel(record, &clevel, &error) )
    {
        nitf_Error_print(&error, stdout, "During complexity level check ...");
        exit( EXIT_FAILURE );
    }
    printf("Complexity level of file is [%s]\n", clevelAsString(clevel));
DESTROY_BLOCK:
    nitf_IOHandle_close(io);
    nitf_Record_destruct(&record);
    nitf_Reader_destruct(&reader);

    return 0;
}
