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
 * License along with this program; If not, 
 * see <http://www.gnu.org/licenses/>.
 *
 */
#include <import/nitf.h>
#include "mex.h"

/*
 *  Error handling method
 *
 */
#define MEX_NTF_ERR(E)                                                  \
    mexErrMsgTxt(( (E)->level == NITF_ERR_UNK) ? ("Unknown Error") : ( (E)->message))


/*
 *  Allocate a C-string from a Matlab array (returns NULL on error)
 */
static char* newString(const mxArray* mx)
{
    size_t len;
    char* str;

    if (!mxIsChar(mx))
	return(NULL);

    if (mxGetM(mx) != 1)
	return(NULL);

    len = (mxGetM(mx) * mxGetN(mx)) + 1;
    str = (char*)malloc(len + 1);
    str[len] = 0;
    if (mxGetString(mx, str, len) != 0)
    {
        free(str);
	return(NULL);
    }
    return str;
}

/*
 *   Transpose a matrix
 *
 * The return is the transposed result, or NULL on error
 *
 *  This either transposes the input in-place or allocates a new matrix and
 *  frees the input
 */

mxArray *transpose(mxArray *org)
{
    return NULL;


}


/*
 *  Currently we will not handle merging images, but interface
 *  is set up to support in the future.  If they wanted to
 *  merge pixel arrays, we could allow a magic -1 for segment
 *  ID
 *
 *  All of these methods will attempt to use optimized mode if they
 *  have the opportunity.
 *
 *  Everything is assumed to be validated prior to this call, and
 *  sizes/bands, etc, should be valid
 */

typedef mxArray* (*MEX_NTF_IMREAD)(nitf_Reader* reader,
                                   int idx,
                                   long startRow,
                                   long startCol,
                                   long numRows,
                                   long numCols,
                                   nitf_Error *error);


nitf_SubWindow* createSubWindow(long startRow,
                                long startCol,
                                long numRows,
                                long numCols,
                                int numBands)
{
    /* Window size */
    nitf_SubWindow* subWindow;

    /* Band ordering list */
    uint32_t* bandList = NULL;

    /* Starting bandID is always 1 for now */
    uint32_t bandID = 0;

    /* Error handler */
    nitf_Error error;

    /* Create the sub-window */
    subWindow = nitf_SubWindow_construct(&error);

    if (!subWindow)
        return(NULL);

    /* 
     *  This tells us what order to give you bands in.  Normally
     *  you just want it in the order of the banding.  For example,
     *  in a non-accelerated band-interleaved by pixel cases, you might
     *  have a band of magnitude and a band of phase.  If you order the
     *  bandList backwards, the phase buffer comes first in the output
     */
    bandList = (uint32_t *) malloc(sizeof(uint32_t *) * numBands);
    if(bandList == NULL)
    {
        nitf_SubWindow_destruct(&subWindow);
        return(NULL);
    }

    for (bandID = 0; bandID < (uint32_t)numBands; ++bandID)
        bandList[bandID] = bandID;

    subWindow->numBands = numBands;
    subWindow->startRow = startRow;
    subWindow->startCol = startCol;
    subWindow->numRows = numRows;
    subWindow->numCols = numCols;
    subWindow->bandList = bandList;

    return subWindow;
}

mxArray* read32BitFloatPixelArray(nitf_Reader* reader,
                                  int idx,
                                  long startRow, 
                                  long startCol,
                                  long numRows, 
                                  long numCols,
				                  nitf_Error *error)
{
    int padded = 0;

    mxArray* mxImageArray = NULL;
    
    /* Window size */
    nitf_SubWindow* subWindow = NULL;

    /* Image Reader */
    nitf_ImageReader* imageReader = NULL;

    /* Buffer array ptr */
    uint8_t* buffers[1];

    uint8_t* buffer = NULL;

    int numBands = 1;

    /* Do this first to avoid abort on fail causing memory leak */

    mxImageArray = 
        mxCreateNumericMatrix(numCols, numRows, mxSINGLE_CLASS, mxREAL);

    /* Create the sub-window - will fail internally if error */
    subWindow = createSubWindow(startRow, startCol,
                                numRows, numCols, numBands);

    /* Now attach the buffer up to the mxImageArray */
    buffer = (uint8_t*)mxGetData(mxImageArray);

    /* Only works with one band */
    buffers[0] = buffer;

    /* Image reader (could fail) */
    imageReader = nitf_Reader_newImageReader(reader, idx, NULL, error);
    if (imageReader == NULL)
        goto CATCH_ERROR;
   
    /* Now read the data */
    if(!nitf_ImageReader_read(imageReader, subWindow, buffers, &padded, error))
        goto CATCH_ERROR;

    if(subWindow != NULL)
        nitf_SubWindow_destruct(&subWindow);
    if(imageReader != NULL)
        nitf_ImageReader_destruct(&imageReader);

    return mxImageArray;

CATCH_ERROR:

    if(subWindow != NULL)
        nitf_SubWindow_destruct(&subWindow);
    if(imageReader != NULL)
        nitf_ImageReader_destruct(&imageReader);
    if(mxImageArray != NULL);
    mxFree(mxImageArray);
    return NULL;
}

/*
 *  This case is totally straightforwad.  We just read in the
 *  pixel array and create a matrix value.
 *
 *  Things in the matlab world are one-based but these functions
 *  assume that the arguments here are 0-based
 */
mxArray* readSingleBandPixelArray(nitf_Reader* reader,
                                  int idx,
                                  int matlabClassType,
                                  long startRow, 
                                  long startCol,
                                  long numRows, 
                                  long numCols,
			                      nitf_Error *error)
{
    
    int padded = 0;

    mxArray* mxImageArray = NULL;
    
    /* Window size */
    nitf_SubWindow* subWindow = NULL;

    /* Image Reader */
    nitf_ImageReader* imageReader = NULL;

    /* Buffer array ptr */
    uint8_t** buffers = NULL;

    uint8_t* buffer = NULL;

    int numBands = 1;

    /* Create the sub-window - will fail internally if error */
    subWindow = createSubWindow(startRow, startCol,
                                numRows, numCols, numBands);

    /* Create the image buffer - will fail internally if error */
    buffers = (uint8_t**)malloc( sizeof(uint8_t*) * numBands);

    mxImageArray = 
        mxCreateNumericMatrix(numCols, numRows, matlabClassType, mxREAL);

    /* Now attach the buffer up to the mxImageArray */
    buffer = (uint8_t*)mxGetData(mxImageArray);

    /* Only works with one band */
    buffers[0] = buffer;

    /* Image reader (could fail) */
    imageReader = nitf_Reader_newImageReader(reader, idx, NULL, error);
    if (!imageReader)
        goto CATCH_ERROR;
   
    /* Now read the data */
    if (!nitf_ImageReader_read(imageReader, subWindow, 
                               buffers, &padded, error))
        goto CATCH_ERROR;

    if(subWindow != NULL)
        nitf_SubWindow_destruct(&subWindow);
    if(imageReader != NULL)
        nitf_ImageReader_destruct(&imageReader);

    return mxImageArray;

CATCH_ERROR:

    if(subWindow != NULL)
        nitf_SubWindow_destruct(&subWindow);
    if(imageReader != NULL)
        nitf_ImageReader_destruct(&imageReader);
    if(mxImageArray != NULL);
    mxFree(mxImageArray);
    return NULL;
}

/*
 *  This case is totally straightforwad.  We just read in the
 *  pixel array and create a matrix value.
 *
 *  Things in the matlab world are one-based but these functions
 *  assume that the arguments here are 0-based
 */
mxArray* read16BitPixelArray(nitf_Reader* reader,
                            int idx,
                            long startRow, 
                            long startCol,
                            long numRows, 
                            long numCols,
			                nitf_Error *error)
{
    
    return readSingleBandPixelArray(reader, idx, mxUINT16_CLASS, startRow, startCol, numRows, numCols, error);
}

/*
 *  This case is totally straightforwad.  We just read in the
 *  pixel array and create a matrix value.
 *
 *  Things in the matlab world are one-based but these functions
 *  assume that the arguments here are 0-based
 */
mxArray* read8BitPixelArray(nitf_Reader* reader,
                            int idx,
                            long startRow, 
                            long startCol,
                            long numRows, 
                            long numCols,
			                nitf_Error *error)
{
    
    return readSingleBandPixelArray(reader, idx, mxUINT8_CLASS, startRow, startCol, numRows, numCols, error);
}

/*
 *  Two band Complex pixel, the I/Q or M/P case
 * 
 *  This case is mostly straightforwad.  We just read in the pixel
 *  array and create a matrix value. Need to assign Pr and Pi to
 *  the correct band I/Q or M/P can be in any order, but for this version,
 *  we will assume that they will be ordered, I then Q.
 *
 *
 *  Things in the matlab world are one-based but these functions
 *  assume that the arguments here are 0-based
 */
mxArray* read2BandComplexPixelArray(nitf_Reader* reader,
                                    int idx,
                                    int matlabClassType,
                                    long startRow, 
                                    long startCol,
                                    long numRows, 
                                    long numCols,
                                    nitf_Error *error)
{
    
    int padded = 0;

    mxArray* mxImageArray = NULL;
    
    /* Window size */
    nitf_SubWindow* subWindow = NULL;

    /* Image Reader */
    nitf_ImageReader* imageReader = NULL;

    /* Buffer array ptr */
    uint8_t* buffers[2];

    int numBands = 2;

/*
 *  Do this here since if it failes the whole function aborts and there is
 *   no chance to clean-up
 */
    mxImageArray = 
        mxCreateNumericMatrix(numCols, numRows, matlabClassType, mxCOMPLEX);

    /* Create the sub-window - will fail internally if error */
    subWindow = createSubWindow(startRow, startCol,
                                numRows, numCols, numBands);


    /* Now attach the buffer up to the mxImageArray */
    buffers[0] = (uint8_t*)mxGetData(mxImageArray);
    buffers[1] = (uint8_t*)mxGetImagData(mxImageArray);

    /* Image reader (could fail) */
    imageReader = nitf_Reader_newImageReader(reader, idx, NULL, error);
    if (!imageReader)
        goto CATCH_ERROR;
   
    /* Now read the data */
    if (!nitf_ImageReader_read(imageReader, subWindow, 
                               buffers, &padded, error))
        goto CATCH_ERROR;

    if(subWindow != NULL)
        nitf_SubWindow_destruct(&subWindow);
    if(imageReader != NULL)
        nitf_ImageReader_destruct(&imageReader);

    return mxImageArray;

CATCH_ERROR:

    if(subWindow != NULL)
        nitf_SubWindow_destruct(&subWindow);
    if(imageReader != NULL)
        nitf_ImageReader_destruct(&imageReader);
    if(mxImageArray != NULL);
    mxFree(mxImageArray);
    return NULL;
}

mxArray* readIQComplexFloatPixelArray(nitf_Reader* reader,
                                      int idx,
                                      long startRow, 
                                      long startCol,
                                      long numRows, 
                                      long numCols,
                                      nitf_Error *error)
{

    return(read2BandComplexPixelArray(reader,
                                      idx,
                                      mxSINGLE_CLASS,
                                      startRow, 
                                      startCol,
                                      numRows, 
                                      numCols,
                                      error));
} 

mxArray* readIQSignedIntPixelArray(nitf_Reader* reader,
                                   int idx,
                                   long startRow, 
                                   long startCol,
                                   long numRows, 
                                   long numCols,
                                   nitf_Error *error)
{

    return(read2BandComplexPixelArray(reader,
                                      idx,
                                      mxINT16_CLASS,
                                      startRow, 
                                      startCol,
                                      numRows, 
                                      numCols,
                                      error));
} 


mxArray* read24BitPixelArray(nitf_Reader* reader,
                             int idx,
                             long startRow, 
                             long startCol,
                             long numRows, 
                             long numCols,
                             nitf_Error *error)
{
    int padded = 0;   

    mxArray* mxImageArray = NULL;
    /* Window size */
    nitf_SubWindow* subWindow = NULL;

    /* Image Reader */
    nitf_ImageReader* imageReader = NULL;

    /* Buffer array ptr */
    uint8_t* buffers[3];

    const mwSize dims[] = { numCols, numRows, 3 };
    const int numBands = 3;
    const size_t frame = numRows * numCols;
    /*
     *  Do this here since if it fails the whole function aborts and there is
     *   no chance to clean-up
     */
    mxImageArray = 
        mxCreateNumericArray(3, dims, mxUINT8_CLASS, mxREAL);

    /* Create the sub-window - will fail internally if error */
    subWindow = createSubWindow(startRow, startCol,
                                numRows, numCols, numBands);

    buffers[0] = ((uint8_t*)mxGetData(mxImageArray));
    buffers[1] = ((uint8_t*)mxGetData(mxImageArray)) + frame;
    buffers[2] = ((uint8_t*)mxGetData(mxImageArray)) + 2 * frame;
    /* Image reader (could fail) */
    imageReader = nitf_Reader_newImageReader(reader, idx, NULL, error);
    if (!imageReader)
        goto CATCH_ERROR;
   
    /* Now read the data */
    if (!nitf_ImageReader_read(imageReader, subWindow, 
                               buffers, &padded, error))
        goto CATCH_ERROR;

    if(subWindow != NULL)
        nitf_SubWindow_destruct(&subWindow);
    if(imageReader != NULL)
        nitf_ImageReader_destruct(&imageReader);

    return mxImageArray;

CATCH_ERROR:

    if(subWindow != NULL)
        nitf_SubWindow_destruct(&subWindow);
    if(imageReader != NULL)
        nitf_ImageReader_destruct(&imageReader);
    if(mxImageArray != NULL);
    mxFree(mxImageArray);
    return NULL;
}



/*
  On error conditions, this function returns NULL and a const error string
  via the errStr argument. It cannot call  mexErrMsgTxt which pops back
  to MATLAB without returning which would result in memory leaks
*/                              
MEX_NTF_IMREAD findImageReader(nitf_ImageSegment* segment,
                               const  char **errStr)
{
    /* Figure out if we have a float or a byte for now */
    int pixelDepth = 
        NITF_NBPP_TO_BYTES(NITF_ATO32(segment->subheader->NITF_NBPP->raw));
    
    const char* pixelValue = segment->subheader->NITF_PVTYPE->raw;

    int numBands = NITF_ATO32(segment->subheader->NITF_NBANDS->raw);

    if(numBands == 1) /* One band, support 8-bit int and 32 bit float for now */
    {
        if (pixelDepth == 1)
        {
            return &read8BitPixelArray;
        }
        else if (pixelDepth == 2)
        {
            return &read16BitPixelArray;
        }
        /* Deal with real mode */
        else if (memcmp(pixelValue, "R", 1) == 0 && pixelDepth == 4)
        {
            return &read32BitFloatPixelArray;
        }
   	    *errStr =
            "Sorry, this MEX wrapper doesnt currently handle the pixel type";
        return(NULL);
    }
    else if(numBands == 2)
    {
        /* Complex IQ or MP
           Look for IQ, 2 bands, with ICAT = {"SAR","SARIQ"} and ISUBCAT = {I,Q}
        */
        if(memcmp(segment->subheader->NITF_ICAT->raw, "SAR", 3) == 0
             || memcmp(segment->subheader->NITF_ICAT->raw, "SARIQ", 5) == 0)
        { 
            /* This is SAR, 2 bands */
            
            char b0 = segment->subheader->bandInfo[0]->NITF_ISUBCAT->raw[0];
            char b1 = segment->subheader->bandInfo[1]->NITF_ISUBCAT->raw[0];

            if((b0 == 'I') && (b1 == 'Q'))
            {
                /*
                extra[0] = 0;
                extra[1] = 1;
                */
            }
            /*
            else if((b1 == 'I') && (b0 == 'Q'))
            {
                extra[1] = 0;
                extra[0] = 1;
            }
            */
            else
            {
                *errStr =
                    "Sorry, this MEX wrapper doesnt currently handle the pixel type";
                return(NULL);
            }

            if (pixelDepth == 4) /* Real*4 valued IQ complex */
            {
                return &readIQComplexFloatPixelArray;
            }
            else if (pixelDepth == 2)
            {
                if (memcmp(pixelValue, "SI", 2) == 0)
                {
                    return &readIQSignedIntPixelArray;
                }
            }
   	        *errStr =
                "Sorry, this MEX wrapper doesnt currently handle the pixel type";
            return(NULL);
        }
    }
    else if (numBands == 3 && pixelDepth == 1)
    {
        if (memcmp(segment->subheader->NITF_IREP->raw, "RGB", 3) == 0)
        {
            return &read24BitPixelArray;
        }

    }
    *errStr = "Sorry, this MEX wrapper doesnt currently handle the pixel type";
    return NULL;
}

/*
 * Parse arguments
 *
 */

/* Returns true on success */

int parseArguments(int nrhs, const mxArray *prhs[],
                   char **inputFile,
                   int *idx,
                   long *startRow, 
                   long *startCol,
                   long *numRows, 
                   long *numCols,
                   const char **errStr)
{
    int argCount;     /* Count of remaining arguments */
    int argIdx;       /* Current argumetn index */
    char *key;        /* Current keyword */
    char *plugin;     /* Plugin path */
    nitf_Error error; /* For NITF library calls */

/*

Initialize window values, use impossible values (0 for number of
rows/cols) to flag that window values are not supplied
*/

    *startRow = 0; 
    *startCol = 0;
    *numRows = 0; 
    *numCols = 0;

    *idx = 0;  /* idx is zero based */

    if(nrhs == 0)
    {
        *errStr = "At least one argument is required";
        return(0);
    }

    *inputFile = newString(prhs[0]);
    if(*inputFile == NULL)
    {
        *errStr = "Invalid keyword";
        return(0);
    }


    argCount = nrhs-1;   /* Total remaining arguments */
    argIdx = 1;
    while(argCount != 0)
    {
        key = mxArrayToString(prhs[argIdx]);
        if(key == NULL)
        {
            *errStr = "Invalid keyword";
            free(*inputFile);
            *inputFile = NULL;
            return(0);
        }
        if(strcmp(key,"Segment") == 0)
        {
            argCount -= 1;
            argIdx += 1;
            if(argCount == 0)
            {
                mxFree(key);
                *errStr = "Segment keyword requires an index value";
                free(*inputFile);
                *inputFile = NULL;
                return(0);
            }
            if(!mxIsNumeric(prhs[argIdx]))
            {
                mxFree(key);
                *errStr = "Segment keyword requires an index value";
                free(*inputFile);
                *inputFile = NULL;
                return(0);
            }
            *idx = (int) (mxGetScalar(prhs[argIdx]) + 0.5); /* Paranoid ???*/
            *idx -= 1;     /* Zero base */
            argCount -= 1;
            argIdx += 1;
        }
        if(strcmp(key,"Plugins") == 0)
        {
            argCount -= 1;
            argIdx += 1;
            if(argCount == 0)
            {
                mxFree(key);
                *errStr = "Segment keyword requires a string value";
                free(*inputFile);
                *inputFile = NULL;
                return(0);
            }

            if((plugin = newString(prhs[argIdx])) == NULL)
            {
                mxFree(key);
                *errStr = "Plugin keyword requires a string value";
                free(*inputFile);
                *inputFile = NULL;
                return(0);
            }
            if(!nitf_PluginRegistry_loadDir(plugin,&error) )
            {
                *errStr = "Could not load NITF plugins";
                free(plugin);
                mxFree(key);
                free(*inputFile);
                *inputFile = NULL;
                return(0);
            }
            free(plugin);

            argCount -= 1;
            argIdx += 1;
        }
        if(strcmp(key,"Window") == 0)
        {
            const mwSize *dims; /* Window spec dimensions */
            double *ptr;        /* Pointer to dimensions */

            argCount -= 1;
            argIdx += 1;
            if(argCount == 0)
            {
                *errStr = "Window keyword requires a row vector (4 values)";
                mxFree(key);
                free(*inputFile);
                *inputFile = NULL;
                return(0);
            }
            if(!mxIsNumeric(prhs[argIdx]))
            {
                *errStr = "Window keyword requires a row vector (4 values)";
                mxFree(key);
                free(*inputFile);
                *inputFile = NULL;
                return(0);
            }

            dims = mxGetDimensions(prhs[argIdx]);
            if((dims == NULL) || (dims[0] != 1) || (dims[1] != 4))
            {
                *errStr = "Window keyword requires a row vector (4 values)";
                mxFree(key);
                free(*inputFile);
                *inputFile = NULL;
                return(0);
            }

            ptr = mxGetData(prhs[argIdx]);

            *startRow = (long) (ptr[0] + 0.5) - 1; /* Paranoid ???*/
            *startCol = (long) (ptr[1] + 0.5) - 1;
            *numRows  = (long) (ptr[2] + 0.5);
            *numCols = (long) (ptr[3] + 0.5);

            argCount -= 1;
            argIdx += 1;
        }
        mxFree(key);
    }
    return(1);
}

/**
 *  The first thing to do here is read all of the nitf fields
 *  and then the TREs
 */
void mexFunction(int nlhs, mxArray *plhs[],
		 int nrhs, const mxArray *prhs[])
{

    /* Our nitf Reader */
    nitf_Reader* reader = NULL;
    
    /* The meta-data record */
    nitf_Record* record = NULL;

    /* Handle to the file */
    nitf_IOHandle io = NITF_INVALID_HANDLE_VALUE;

    /* Local error */
    nitf_Error error;

    /* Iterator for entry at idx */
    nitf_ListIterator it;
    
    /* Segment to be used */
    nitf_ImageSegment* segment = NULL;

    /* Pointer to input file name */
    char *inputFile = NULL;

    /* index */
    int idx;
    int imageCount;   /* Segment count (one based) */

    /*  Sub-window components */

    long startRow; 
    long startCol;
    long numRows; 
    long numCols;
    long numRowsTotal;
    long numColsTotal;
    /* Error string */

    const char *errStr;

    /* pointer to a function of value */
    MEX_NTF_IMREAD imread = NULL;

    mxArray* mxImageArray;

    if(!parseArguments(nrhs,prhs,&inputFile,&idx,
                       &startRow, &startCol, &numRows, &numCols, &errStr))
    {
	    mexErrMsgTxt(errStr);
	    return;
    }
    if(nlhs != 1)
    {
        free(inputFile);
	    mexErrMsgTxt("function requires exactly one output (struct)");
	    return;
    }

    /* Check that we have a NITF */
    if (nitf_Reader_getNITFVersion(inputFile) == NITF_VER_UNKNOWN)
    {
        free(inputFile);
        mexErrMsgTxt("Input file is not NITF");
        return;
    }
    
    /* Try to open this file */
    io = nitf_IOHandle_create(inputFile, 
                              NITF_ACCESS_READONLY, 
                              NITF_OPEN_EXISTING, 
                              &error);
    /* Get rid of this as soon as possible */
    free(inputFile);

    /* If the handle was bad, go home */
    if (NITF_INVALID_HANDLE(io))
        MEX_NTF_ERR(&error);

    /* Create the reader */
    reader = nitf_Reader_construct(&error);
    if (!reader)
    {
        /* Close the handle and close up shop */
        nitf_IOHandle_close(io);
        MEX_NTF_ERR(&error);
    }
    /* Read the meta-data */
    record = nitf_Reader_read(reader, io, &error);

    /* If the record was bad, start shutting down */
    if (!record)
    {
        nitf_Reader_destruct(&reader);
        nitf_IOHandle_close(io);
        MEX_NTF_ERR(&error);
    }

/*  Get number of image segments from file header */

    if(!nitf_Field_get(record->header->NITF_NUMI,(NITF_DATA *) &imageCount,
                       NITF_CONV_INT, sizeof(imageCount), &error))
    {
        nitf_Reader_destruct(&reader);
        nitf_Record_destruct(&record);
        nitf_IOHandle_close(io);
        MEX_NTF_ERR(&error);
    }

    /* Check the image index argument which is one based, the segment count
       is zero based */

    if((idx < 0) || (idx > imageCount-1)) /*LJW*/
    {
        nitf_Reader_destruct(&reader);
        nitf_Record_destruct(&record);
        nitf_IOHandle_close(io);
        mexErrMsgTxt("Invalid image segment index");
    }

    /* Still rockin' */
    it = nitf_List_at(record->images, idx);
    segment = (nitf_ImageSegment*)nitf_ListIterator_get(&it);

    /* TODO: Check most of the fields to make sure we are okay */
    imread = findImageReader(segment, &errStr);

    if (imread == NULL)
    {
        nitf_Reader_destruct(&reader);
        nitf_Record_destruct(&record);
        nitf_IOHandle_close(io);
        mexErrMsgTxt(errStr);
    }

    numRowsTotal = atol(segment->subheader->NITF_NROWS->raw);
    numColsTotal = atol(segment->subheader->NITF_NCOLS->raw);
    if(numRows == 0)      /* No window specified, read full iamge */    
    {
        startRow = 0;
        startCol = 0;
        numRows = numRowsTotal;
        numCols = numColsTotal;
    }
    /* Now validate the rectangle, and if its too big, resize to be nice */
    if (numRows + startRow > numRowsTotal)
    {
        numRows = numRowsTotal - startRow;
    }
    if (numCols + startCol > numColsTotal)
    {
        numCols = numColsTotal - startCol;
    }

    mxImageArray = (*imread)(reader, idx,startRow, startCol,
                             numRows, numCols, &error);

    /*
        Clean-up

        The image segment is part of the record so do not destruct it separately
    */

    if(reader != NULL)
        nitf_Reader_destruct(&reader);

    if(record != NULL)
        nitf_Record_destruct(&record);

    if(!NITF_INVALID_HANDLE(io))
        nitf_IOHandle_close(io);

    if(mxImageArray == NULL)
        MEX_NTF_ERR(&error);

    plhs[0] = mxImageArray;

    return;
}


