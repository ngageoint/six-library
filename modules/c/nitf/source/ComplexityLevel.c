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
 * License along with this program;
 * If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "nitf/ComplexityLevel.h"

typedef NITF_CLEVEL (*CLEVEL_CHECK_PTR)(nitf_Record*, nitf_Error*);

/* Still need to handle CGM aggregate size check! */
/* Still need to handle Text Size/seg Text Format Codes */



NITFPRIV(NITF_CLEVEL) checkILOC(nitf_ImageSubheader* subhdr, nitf_Error* error)
{
    char iloc[NITF_ILOC_SZ + 1];
    char num[6];
    int rowCoord, colCoord;
    int lastRow, lastCol;
    int nrows, ncols;

    if (!nitf_Field_get(subhdr->NITF_ILOC, iloc,
                        NITF_CONV_STRING, 
                        NITF_ILOC_SZ + 1, error))
        return NITF_CLEVEL_CHECK_FAILED;

    num[5] = '\0';
    memcpy(num, iloc, 5);
    rowCoord = atoi(num);
    memcpy(num, &iloc[5], 5);
    colCoord = atoi(num);

    if (!nitf_Field_get(subhdr->NITF_NROWS, &nrows, 
                        NITF_CONV_INT, sizeof(int), error))
        return NITF_CLEVEL_CHECK_FAILED;

    if (!nitf_Field_get(subhdr->NITF_NCOLS, &ncols, 
                        NITF_CONV_INT, sizeof(int), error))
        return NITF_CLEVEL_CHECK_FAILED;

    /* The Common Coordinate System Extent ranges are referring to the
     * (inclusive) last row/col */
    lastRow = rowCoord + nrows - 1;
    lastCol = colCoord + ncols - 1;

    if (lastRow <= 2047 && lastCol <= 2047)
        return NITF_CLEVEL_03;

    if (lastRow <= 8191 && lastCol <= 8191)
        return NITF_CLEVEL_05;

    if (lastRow <= 65535 && lastCol <= 65535)
        return NITF_CLEVEL_06;

    if (lastRow <= 99999999 && lastCol <= 99999999)
        return NITF_CLEVEL_07;

    return NITF_CLEVEL_09;
}


NITFPRIV(NITF_CLEVEL) checkCCSExtent(nitf_Record* record, 
                                     nitf_Error* error)
{
    int i = 0;
    int clevel = NITF_CLEVEL_03;
    nitf_ListIterator it = nitf_List_begin(record->images);
    nitf_ListIterator end = nitf_List_end(record->images);
    
    while (nitf_ListIterator_notEqualTo(&it, &end))
    {
        nitf_ImageSegment* imageSegment = 
            (nitf_ImageSegment*)nitf_ListIterator_get(&it);

        int result = checkILOC(imageSegment->subheader, error);
        if (result == NITF_CLEVEL_CHECK_FAILED )
            return result;

        if ( result > clevel )
        {
            clevel = result;
        }
        ++i;
        nitf_ListIterator_increment(&it);
        
    }
    return clevel;

}

NITFPRIV(NITF_CLEVEL) checkFileSize(nitf_Record* record, 
                                    nitf_Error* error)
{
    nitf_Int64 fl;
    if (!nitf_Field_get(record->header->NITF_FL, &fl, NITF_CONV_INT, 8, error))
        return NITF_CLEVEL_CHECK_FAILED;

    if (fl <= 52428799)
        return NITF_CLEVEL_03;

    if (fl <= 1073741823)
        return NITF_CLEVEL_05;

    if (fl <= 2147483647)
        return NITF_CLEVEL_06;

    if (fl <= NITF_INT64(10737418239))
        return NITF_CLEVEL_07;

    return NITF_CLEVEL_09;

}

NITFPRIV(NITF_CLEVEL) checkImage(nitf_ImageSubheader* subhdr, 
                                 nitf_Error* error)
{
    int nrows, ncols;
    if (!nitf_Field_get(subhdr->NITF_NROWS, &nrows, NITF_CONV_INT, 
                        sizeof(int), error))
        return NITF_CLEVEL_CHECK_FAILED;


    if (!nitf_Field_get(subhdr->NITF_NCOLS, &ncols, NITF_CONV_INT, 
                        sizeof(int), error))
        return NITF_CLEVEL_CHECK_FAILED;

    if (nrows <= 2048 && ncols <= 2048)
        return NITF_CLEVEL_03;
    
    if (nrows <= 8192 && ncols <= 8192)
        return NITF_CLEVEL_05;
    
    if (nrows <= 65536 && ncols <= 65536)
        return NITF_CLEVEL_06;
    
    if (nrows <= 99999999 && ncols <= 99999999)
        return NITF_CLEVEL_07;

    return NITF_CLEVEL_09;

}

NITFPRIV(NITF_CLEVEL) checkImageSize(nitf_Record* record, 
                                     nitf_Error* error)
{
    int clevel = NITF_CLEVEL_03;
    
    nitf_ListIterator it = nitf_List_begin(record->images);
    nitf_ListIterator end = nitf_List_end(record->images);
    
    int i = 0;

    while (nitf_ListIterator_notEqualTo(&it, &end) )
    {
        nitf_ImageSegment* imageSegment = 
            (nitf_ImageSegment*)nitf_ListIterator_get(&it);
        int result = checkImage(imageSegment->subheader, error);
        if ( result == NITF_CLEVEL_CHECK_FAILED ) return result;
        
        if ( result > clevel )
        {
            clevel = result;

        }
        ++i;
        nitf_ListIterator_increment(&it);
    }
    return clevel;
}

NITFPRIV(NITF_CLEVEL) checkImageBlock(nitf_ImageSubheader* subhdr, 
                                      nitf_Error* error)
{
    int nppbh, nppbv;
    if (!nitf_Field_get(subhdr->NITF_NPPBH, &nppbh, NITF_CONV_INT, 
                        sizeof(int), error))
        return NITF_CLEVEL_CHECK_FAILED;


    if (!nitf_Field_get(subhdr->NITF_NPPBV, &nppbv, NITF_CONV_INT, 
                        sizeof(int), error))
        return NITF_CLEVEL_CHECK_FAILED;

    if (nppbh <= 0 || nppbv <= 0)
        return NITF_CLEVEL_09;
    
    if (nppbh <= 2048 && nppbv <= 2048)
        return NITF_CLEVEL_03;

    if (nppbh <= 8192 && nppbv <= 8192)
        return NITF_CLEVEL_05;

    return NITF_CLEVEL_06;

}

NITFPRIV(NITF_CLEVEL) checkBlockSize(nitf_Record* record, nitf_Error* error)
{
    int clevel = NITF_CLEVEL_03;
    
    nitf_ListIterator it = nitf_List_begin(record->images);
    nitf_ListIterator end = nitf_List_end(record->images);
    
    int i = 0;

    while (nitf_ListIterator_notEqualTo(&it, &end) )
    {
        nitf_ImageSegment* imageSegment = 
            (nitf_ImageSegment*)nitf_ListIterator_get(&it);
        int result = checkImageBlock(imageSegment->subheader, error);
        if ( result == NITF_CLEVEL_CHECK_FAILED ) return result;
        
        if ( result > clevel )
        {
            clevel = result;

        }
        ++i;
        nitf_ListIterator_increment(&it);
    }
    return clevel;
    


}

NITFPRIV(NITF_CLEVEL) checkNumImages(nitf_Record* record, nitf_Error* error)
{
    nitf_Uint32 numi = nitf_Record_getNumImages(record, error);
    return (numi > 20) ? NITF_CLEVEL_05 : NITF_CLEVEL_03;
}

NITFPRIV(NITF_CLEVEL) checkNumDES(nitf_Record* record, nitf_Error* error)
{

    int clevel = NITF_CLEVEL_03;
    nitf_Uint32 numdes = nitf_Record_getNumDataExtensions(record, error);


    if (numdes > 10)
    {

        if (numdes <= 50)
            clevel = NITF_CLEVEL_06;

        if (numdes <= 100)
            clevel = NITF_CLEVEL_07;
       
        else
            clevel = NITF_CLEVEL_09;

    }
    return clevel;

}

NITFPRIV(NITF_CLEVEL) checkRGBImage(nitf_ImageSubheader* subhdr,
                                    nitf_Error* error)
{
    int clevel = NITF_CLEVEL_03;
    int nbands, nbpp;
    char imode = subhdr->imageMode->raw[0];
    if (!nitf_Field_get(subhdr->NITF_NBANDS, &nbands, NITF_CONV_INT,
                        sizeof(int), error))
    {
        return NITF_CLEVEL_CHECK_FAILED;
    }

    if (!nitf_Field_get(subhdr->NITF_NBPP, &nbpp, NITF_CONV_INT,
                        sizeof(int), error))
    {
        return NITF_CLEVEL_CHECK_FAILED;
    }

    if ((memcmp(subhdr->NITF_IC->raw, "C8", 2) == 0 ||
         memcmp(subhdr->NITF_IC->raw, "M8", 2) == 0) &&
        nbpp > 32)
    {
        clevel = NITF_CLEVEL_09;
    }

    if ((memcmp(subhdr->NITF_IC->raw, "C3", 2) == 0 ||
         memcmp(subhdr->NITF_IC->raw, "M3", 2) == 0) &&
        (nbpp > 8 || imode != 'P'))
    {
        clevel = NITF_CLEVEL_09;
    }
    
    if (nbands != 3)
    {
        clevel = NITF_CLEVEL_09;
    }
    else if (imode != 'B' &&
             imode != 'P' &&
             imode != 'S' &&
             imode != 'R')
    {
        clevel = NITF_CLEVEL_09;
    }
    else
    {
        if (nbpp == 16 || nbpp == 32)
        {
            if (clevel < NITF_CLEVEL_06)
            {
                clevel = NITF_CLEVEL_06;
            }
        }
        else if (nbpp != 8)
        {
            clevel = NITF_CLEVEL_09;
        }
    }

    return clevel;
}

NITFPRIV(NITF_CLEVEL) checkRGBLUTImage(nitf_ImageSubheader* subhdr, 
                                       nitf_Error* error)
{
    int clevel = NITF_CLEVEL_03;
    int nbands, nbpp;
    char imode = subhdr->imageMode->raw[0];

    if (!nitf_Field_get(subhdr->NITF_NBANDS, &nbands, NITF_CONV_INT,
                        sizeof(int), error))
        return NITF_CLEVEL_CHECK_FAILED;

    if (!nitf_Field_get(subhdr->NITF_NBPP, &nbpp, NITF_CONV_INT,
                        sizeof(int), error))
        return NITF_CLEVEL_CHECK_FAILED;

    if (memcmp(subhdr->NITF_IC->raw, "NC", 2) != 0 &&
        memcmp(subhdr->NITF_IC->raw, "NM", 2) != 0)
        clevel = NITF_CLEVEL_09;

    if (nbands != 1)
    {
        clevel = NITF_CLEVEL_09;
    }
    else if (nbpp != 1 && nbpp != 8)
    {
        clevel = NITF_CLEVEL_09;
    }
    else if (imode != 'B')
    {
        clevel = NITF_CLEVEL_09;
    }
    return clevel;

}

NITFPRIV(NITF_CLEVEL) checkMonoImage(nitf_ImageSubheader* subhdr, 
                                     nitf_Error* error)
{
    int clevel = NITF_CLEVEL_03;
    int nbands, nbpp;
    char imode = subhdr->imageMode->raw[0];

    if (!nitf_Field_get(subhdr->NITF_NBANDS, &nbands, NITF_CONV_INT,
                        sizeof(int), error))
        return NITF_CLEVEL_CHECK_FAILED;

    if (!nitf_Field_get(subhdr->NITF_NBPP, &nbpp, NITF_CONV_INT,
                        sizeof(int), error))
        return NITF_CLEVEL_CHECK_FAILED;

    if ( (memcmp(subhdr->NITF_IC->raw, "C3", 2) == 0) ||
         (memcmp(subhdr->NITF_IC->raw, "M3", 2) == 0) )
    {
        if (nbpp != 8 && nbpp != 12)
            clevel = NITF_CLEVEL_09;
    }

    if (nbands != 1)
    {
        clevel = NITF_CLEVEL_09;
    }
    else if (nbpp != 1 &&
             nbpp != 8 &&
             nbpp != 12 &&
             nbpp != 16 &&
             nbpp != 32 &&
             nbpp != 64)
    {
        clevel = NITF_CLEVEL_09;
    }
    else if (imode != 'B')
    {
        clevel = NITF_CLEVEL_09;
    }
    return clevel;
}

/* This is an optional feature */
/* NITFPRIV(NITF_CLEVEL) checkNoDisplay(nitf_ImageSubheader* subhdr, */
/*                                      nitf_Error* error) */
/* { */
/*     /\* Is it elevation data? *\/ */
    
/*     /\* Is it location grid? *\/ */

/*     /\* Is it matrix data? *\/ */

    

/* } */

NITFPRIV(NITF_CLEVEL) checkMultiImage(nitf_ImageSubheader* subhdr, 
                                      nitf_Error* error)
{
    int clevel = NITF_CLEVEL_03;
    int nbands, nbpp;
    char imode = subhdr->imageMode->raw[0];

    if (!nitf_Field_get(subhdr->NITF_NBANDS, &nbands, NITF_CONV_INT,
                        sizeof(int), error))
        return NITF_CLEVEL_CHECK_FAILED;

    if (!nitf_Field_get(subhdr->NITF_NBPP, &nbpp, NITF_CONV_INT,
                        sizeof(int), error))
        return NITF_CLEVEL_CHECK_FAILED;

    /*  Note that the rest of this check is covered below */
    if (memcmp(subhdr->NITF_IC->raw, "C8", 2) == 0 ||
        memcmp(subhdr->NITF_IC->raw, "M8", 2) == 0)
    {
        if (imode == 'B' || nbpp > 32)
            clevel = NITF_CLEVEL_09;
        
    }
    /*  Normal JPEG */
    else if (memcmp(subhdr->NITF_IC->raw, "C3", 2) == 0 ||
             memcmp(subhdr->NITF_IC->raw, "M3", 2) == 0)
    {
        if (nbpp != 8 && nbpp != 12)
            return NITF_CLEVEL_09;
    }

    if (nbands < 2)
    {
        clevel = NITF_CLEVEL_09;
    }
    else if (nbands < 10)
    {
        if (clevel < NITF_CLEVEL_03)
            clevel = NITF_CLEVEL_03;
    }
    else
    {
        if (nbands <= 255)
        {
            if (clevel < NITF_CLEVEL_06)
                clevel = NITF_CLEVEL_06;
        }
        else if (nbands <= 999)
        {
            if (clevel < NITF_CLEVEL_07)
                clevel = NITF_CLEVEL_07;
        }
        else clevel = NITF_CLEVEL_09;
                
        
    }

    if (nbpp != 8 &&
        nbpp != 16 &&
        nbpp != 32 &&
        nbpp != 64)
    {

        clevel = NITF_CLEVEL_09;
    }
    else if (imode != 'B')
    {
        clevel = NITF_CLEVEL_09;
    }
    return clevel;
}



NITFPRIV(NITF_CLEVEL) checkSpecificImageAttributes(nitf_Record* record,
                                         nitf_Error* error)
{
    NITF_CLEVEL clevel = NITF_CLEVEL_03;
    
  
    nitf_ListIterator it = nitf_List_begin(record->images);
    nitf_ListIterator end = nitf_List_end(record->images);
    
    int i = 0;

    while (nitf_ListIterator_notEqualTo(&it, &end) )
    {

        NITF_CLEVEL result = NITF_CLEVEL_UNKNOWN;
        char irep[ NITF_IREP_SZ + 1];

        nitf_ImageSegment* imageSegment = 
            (nitf_ImageSegment*)nitf_ListIterator_get(&it);

        if (!nitf_Field_get(imageSegment->subheader->NITF_IREP,
                            irep, NITF_CONV_STRING,
                            NITF_IREP_SZ + 1, error))
            return NITF_CLEVEL_CHECK_FAILED;
        nitf_Utils_trimString(irep);

        if (strcmp( irep, "MONO") == 0)
        {

            result = checkMonoImage(imageSegment->subheader, error);
        }
        else if (strcmp( irep, "RGB") == 0)
        {
            result = checkRGBImage(imageSegment->subheader, error);
        }
        else if (strcmp( irep, "RGB/LUT" ) == 0)
        {
            result = checkRGBLUTImage(imageSegment->subheader, error);
        }
        else if (strcmp( irep, "MULTI" ) == 0)
        {
            result = checkMultiImage(imageSegment->subheader, error);
        }
        else
        {
            /* What happens for these other reps ? */
            return result;

        }

        if ( result == NITF_CLEVEL_CHECK_FAILED ) return result;
        
        if ( result > clevel )
        {
            clevel = result;

        }
        ++i;
        nitf_ListIterator_increment(&it);
    }
    return clevel;
}

typedef struct _ComplexityLevelCheck
{
    CLEVEL_CHECK_PTR check;
} ComplexityLevelCheck;

static ComplexityLevelCheck checks[] =
{
    { checkCCSExtent },
    { checkFileSize },
    { checkImageSize },
    { checkBlockSize },
    { checkNumImages },
    { checkNumDES },
    { checkSpecificImageAttributes },
    { NULL }
};

NITFAPI(NITF_CLEVEL) nitf_ComplexityLevel_measure(nitf_Record* record,
                                                  nitf_Error* error)
{
    /*char *p = checks[0].checkName;*/
    CLEVEL_CHECK_PTR checkToRun = checks[0].check;
    NITF_CLEVEL clevel = NITF_CLEVEL_UNKNOWN;
    int i = 0;
    while ( checkToRun != NULL )
    {
        NITF_CLEVEL checkComplexity = (*checkToRun)(record, error);
        if (checkComplexity == NITF_CLEVEL_CHECK_FAILED)
        {
            return NITF_CLEVEL_CHECK_FAILED;
        }
        if (checkComplexity > clevel)
        {
            clevel = checkComplexity;
        }
        checkToRun = checks[++i].check;
    }
    return clevel;
}


NITFAPI(NITF_CLEVEL) nitf_ComplexityLevel_get(nitf_Record* record)
{

    char* c2 = record->header->NITF_CLEVEL->raw;

    if (memcmp(c2, "03", 2) == 0)
        return NITF_CLEVEL_03;
    if (memcmp(c2, "05", 2) == 0)
        return NITF_CLEVEL_05;
    if (memcmp(c2, "06", 2) == 0)
        return NITF_CLEVEL_06;
    if (memcmp(c2, "07", 2) == 0)
        return NITF_CLEVEL_07;
    if (memcmp(c2, "09", 2) == 0)
        return NITF_CLEVEL_09;
    else return NITF_CLEVEL_UNKNOWN;
            

}


NITFAPI(NITF_BOOL) nitf_ComplexityLevel_toString(NITF_CLEVEL clevel,
                                                 char* c2)
{
    
    NITF_BOOL success = NITF_FAILURE;
    switch (clevel)
    {
    case NITF_CLEVEL_CHECK_FAILED:
        break;
        
    case NITF_CLEVEL_03:
        memcpy(c2, "03", 2);
        success = NITF_SUCCESS;
        break;

    case NITF_CLEVEL_05:
        memcpy(c2, "05", 2);
        success = NITF_SUCCESS;
        break;
        
    case NITF_CLEVEL_06:
        memcpy(c2, "06", 2);
        success = NITF_SUCCESS;
        break;


    case NITF_CLEVEL_07:
        memcpy(c2, "07", 2);
        success = NITF_SUCCESS;
        break;


    case NITF_CLEVEL_09:
        success = NITF_SUCCESS;
        /* Dont break, we want 09 */

    case NITF_CLEVEL_UNKNOWN:
        memcpy(c2, "09", 2);
        break;

    }
    return success;
}
