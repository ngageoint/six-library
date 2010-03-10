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
#define MEX_NTF_ERR(E) \
mexErrMsgTxt(( (E)->level == NITF_ERR_UNK) ? ("Unknown Error") : ( (E)->message))

/* Size of extra array (This is really ugly but is confined to this file) */

#define EXTRA_SIZE sizeof(long)*2

/*
 *  Allocate a C-string from a Matlab array
 */
static char* newString(const mxArray* mx)
{
    if (!mxIsChar(mx))
	mexErrMsgTxt("Require string arg");

    if (mxGetM(mx) != 1)
	mexErrMsgTxt("Input must be a row vector");

     size_t len = (mxGetM(mx) * mxGetN(mx)) + 1;
     char* str = (char*)malloc(len + 1);
     str[len] = 0;
     if (mxGetString(mx, str, len) != 0)
     {
         free(str);
	 mexErrMsgTxt("Not enough space!");
     }
     return str;
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
 *
 *  The extra pointer allows the caller to pas case specific information
 *  to the reader fucntion
 */

typedef mxArray* (*MEX_NTF_IMREAD)(nitf_Reader* reader,
                                   int idx,
                                   long startRow,
                                   long startCol,
                                   long endRow,
                                   long endCol,
                                   nitf_Uint8 *extra);


nitf_SubWindow* createSubWindow(long startRow,
                                long startCol,
                                long numRows,
                                long numCols,
                                int numBands)
{
    /* Window size */
    nitf_SubWindow* subWindow;

    /* Band ordering list */
    nitf_Uint32* bandList = NULL;

    /* Starting bandID is always 1 for now */
    nitf_Uint32 bandID = 0;

    /* Error handler */
    nitf_Error error;

    /* Create the sub-window */
    subWindow = nitf_SubWindow_construct(&error);

    if (!subWindow)
        MEX_NTF_ERR(&error);

    /* 
     *  This tells us what order to give you bands in.  Normally
     *  you just want it in the order of the banding.  For example,
     *  in a non-accelerated band-interleaved by pixel cases, you might
     *  have a band of magnitude and a band of phase.  If you order the
     *  bandList backwards, the phase buffer comes first in the output
     */
    bandList = (nitf_Uint32 *) malloc(sizeof(nitf_Uint32 *) * numBands);

    for (bandID = 0; bandID < numBands; ++bandID)
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
                                  nitf_Uint8 *extra)
{
    
    int padded = 0;

    mxArray* mxImageArray = NULL;
    
    /* Window size */
    nitf_SubWindow* subWindow = NULL;

    /* Image Reader */
    nitf_ImageReader* imageReader = NULL;

    /* Buffer array ptr */
    nitf_Uint8** buffers = NULL;

    nitf_Uint8* buffer = NULL;

    /* Error handler */
    nitf_Error error;

    int numBands = 1;

    /* Create the sub-window - will fail internally if error */
    subWindow = createSubWindow(startRow, startCol,
                                numRows, numCols, numBands);

    /* Create the image buffer - will fail internally if error */
    buffers = (nitf_Uint8**)malloc( sizeof(nitf_Uint32*) * numBands);

    mxImageArray = 
        mxCreateNumericMatrix(numCols, numRows, mxSINGLE_CLASS, mxREAL);

    /* Now attach the buffer up to the mxImageArray */
    buffer = (nitf_Uint8*)mxGetData(mxImageArray);

    /* Only works with one band */
    buffers[0] = buffer;

    /* Image reader (could fail) */
    imageReader = nitf_Reader_newImageReader(reader,
                                             idx,
                                             &error);
    if (!imageReader)
        goto CATCH_ERROR;
   
    /* Now read the data */
    if (!nitf_ImageReader_read(imageReader, subWindow, 
                               buffers, &padded, &error))
        goto CATCH_ERROR;
    return mxImageArray;

CATCH_ERROR:

    if (subWindow) free(subWindow);
    MEX_NTF_ERR(&error);
    return NULL;
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
                            nitf_Uint8 *extra)
{
    
    int padded = 0;

    mxArray* mxImageArray = NULL;
    
    /* Window size */
    nitf_SubWindow* subWindow = NULL;

    /* Image Reader */
    nitf_ImageReader* imageReader = NULL;

    /* Buffer array ptr */
    nitf_Uint8** buffers = NULL;

    nitf_Uint8* buffer = NULL;

    /* Error handler */
    nitf_Error error;

    int numBands = 1;

    /* Create the sub-window - will fail internally if error */
    subWindow = createSubWindow(startRow, startCol,
                                numRows, numCols, numBands);

    /* Create the image buffer - will fail internally if error */
    buffers = (nitf_Uint8**)malloc( sizeof(nitf_Uint8*) * numBands);

    mxImageArray = 
        mxCreateNumericMatrix(numCols, numRows, mxUINT8_CLASS, mxREAL);

    /* Now attach the buffer up to the mxImageArray */
    buffer = (nitf_Uint8*)mxGetData(mxImageArray);

    /* Only works with one band */
    buffers[0] = buffer;

    /* Image reader (could fail) */
    imageReader = nitf_Reader_newImageReader(reader,
                                             idx,
                                             &error);
    if (!imageReader)
        goto CATCH_ERROR;
   
    /* Now read the data */
    if (!nitf_ImageReader_read(imageReader, subWindow, 
                               buffers, &padded, &error))
        goto CATCH_ERROR;
    return mxImageArray;

CATCH_ERROR:

    if (subWindow) free(subWindow);
    MEX_NTF_ERR(&error);
    return NULL;
}

/*
 *  Two band Complex pixel, the I/Q or M/P case
 * 
 *  This case is mostly straightforwad.  We just read in the pixel
 *  array and create a matrix value. Need to assign Pr and Pi to
 *  the correct band (I/Q or M/P can be in any order)
 *
 *  This function is not called directly but from the actual I/O or
 *  M/P float reader. The extra arguments are the namd indexes of the
 *  I/Q or M/P (either 0,1 or 1,0). Also the M/P case has to convert
 *  The data to I/Q (real/imaginary)
 *
 *  Things in the matlab world are one-based but these functions
 *  assume that the arguments here are 0-based
 */
mxArray* read2BandComplexFloatPixelArray(nitf_Reader* reader,
                               int idx,
                               long startRow, 
                               long startCol,
                               long numRows, 
                               long numCols,
                               long IBand,       /* or M */
                               long Qband)       /* or P */
{
    
    int padded = 0;

    mxArray* mxImageArray = NULL;
    
    /* Window size */
    nitf_SubWindow* subWindow = NULL;

    /* Image Reader */
    nitf_ImageReader* imageReader = NULL;

    /* Buffer array ptr */
    nitf_Uint8* buffers[2];

    /* Error handler */
    nitf_Error error;

    int numBands = 2;

    /* Create the sub-window - will fail internally if error */
    subWindow = createSubWindow(startRow, startCol,
                                numRows, numCols, numBands);

    mxImageArray = 
        mxCreateNumericMatrix(numCols, numRows, mxSINGLE_CLASS, mxCOMPLEX);

    /* Now attach the buffer up to the mxImageArray */
    buffers[0] = (nitf_Uint8*)mxGetData(mxImageArray);
    buffers[1] = (nitf_Uint8*)mxGetImagData(mxImageArray);

    /* Image reader (could fail) */
    imageReader = nitf_Reader_newImageReader(reader,
                                             idx,
                                             &error);
    if (!imageReader)
        goto CATCH_ERROR;
   
    /* Now read the data */
    if (!nitf_ImageReader_read(imageReader, subWindow, 
                               buffers, &padded, &error))
        goto CATCH_ERROR;
    return mxImageArray;

CATCH_ERROR:

    if (subWindow) free(subWindow);
    MEX_NTF_ERR(&error);
    return NULL;
}

mxArray* readIQComplexFloatPixelArray(nitf_Reader* reader,
                               int idx,
                               long startRow, 
                               long startCol,
                               long numRows, 
                               long numCols,
                               nitf_Uint8 *extra)
{

  return(read2BandComplexFloatPixelArray(reader,
                               idx,
                               startRow, 
                               startCol,
                               numRows, 
                               numCols,
                               ((long *) extra)[0],       /* I or M */
                               ((long *) extra)[1]));      /* Q or P */
} 

/* TODO: Implement me (using normal mode?) mode */
mxArray* read24BitPixelArray(nitf_Reader* reader,
                             int idx,
                             long startRow, 
                             long startCol,
                             long endRow, 
                             long endCol)
{
    
    return NULL;
}


/*
   On error conditions, this function calls mexErrMsgTxt which pops back
   to MATLAB without returning, so from the callers point of view, it
   can't fail
*/

                              
MEX_NTF_IMREAD findImageReader(nitf_ImageSegment* segment,nitf_Uint8 *extra)
{
    /* Figure out if we have a float or a byte for now */
    int pixelDepth = 
        NITF_NBPP_TO_BYTES(atol(segment->subheader->NITF_NBPP->raw));
    
    const char* pixelValue = segment->subheader->NITF_PVTYPE->raw;

    int numBands = atol(segment->subheader->NITF_NBANDS->raw);

    if(numBands == 1) /* One band, support 8-bit int and 32 bit float for now */
    {
        if (pixelDepth == 1)
        {
            return &read8BitPixelArray;
        }
    /* Deal with real mode */
        else if (memcmp(pixelValue, "R", 1) == 0 && pixelDepth == 4)
        {
            return &read32BitFloatPixelArray;
        }
   	mexErrMsgTxt(
           "Sorry, this MEX wrapper doesnt currently handle the pixel type");
    }
    else if(numBands == 2)
    {
    /* Complex IQ or MP
       Look for IQ, 2 bands, with ICAT = {"SAR","SARIQ"} and ISUBCAT = {I,Q}
    */
        if(
             (memcmp(segment->subheader->NITF_ICAT->raw,"SAR",3)
           || memcmp(segment->subheader->NITF_ICAT->raw,"SARIQ",5)))
        {  /* This is SAR, 2 bands */
             char b0 = segment->subheader->bandInfo[0]->NITF_ISUBCAT->raw[0];
             char b1 = segment->subheader->bandInfo[1]->NITF_ISUBCAT->raw[0];

             if((b0 == 'I') && (b1 == 'Q'))
             {
                 extra[0] = 0;     /* I band */
                 extra[1] = 1;     /* Q band */
             }
             else if((b1 == 'I') && (b0 == 'Q'))
             {
                 extra[1] = 0;     /* I band */
                 extra[0] = 1;     /* Q band */
             }
             else
             {
   	         mexErrMsgTxt(
                    "Sorry, this MEX wrapper doesnt currently handle the pixel type");
                 return(NULL);
             }

             if(pixelDepth == 4) /* Real*4 valued IQ complex */
             {
                 return(&readIQComplexFloatPixelArray);
             }
   	    mexErrMsgTxt(
             "Sorry, this MEX wrapper doesnt currently handle the pixel type");
            return(NULL);
        }
    }
    mexErrMsgTxt(
              "Sorry, this MEX wrapper doesnt currently handle the pixel type");
    return NULL;
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
    int idx = 0;

    /* pointer to a function of value */
    MEX_NTF_IMREAD imread = NULL;

    /* Extra information from findImageReader */
    nitf_Uint8 extra[EXTRA_SIZE];

    mxArray* mxImageArray;

    if (nrhs < 2) 
    {
	mexErrMsgTxt("<file-name> <idx> (nitf-plugin-path)");
	return;
    }

    if (nrhs == 3)
    {
        char* pluginPath = newString(prhs[2]);
        /* Alternative to NITF_PLUGIN_PATH */
        if (! nitf_PluginRegistry_loadDir(pluginPath, &error) )
        {
            free(pluginPath);
            MEX_NTF_ERR(&error);
            return;
        }
        free(pluginPath);
    }

    /* TODO: Support colormaps here too */

    else if (nlhs > 1) 
    {
	mexErrMsgTxt("function requires only one output (struct)");
	return;
    }

    /* Set input file (copy) */
    inputFile = newString(prhs[0]);

    /* Check that we have a NITF */
    if (nitf_Reader_getNITFVersion(inputFile) == NITF_VER_UNKNOWN)
    {
        mexErrMsgTxt(inputFile);
        free(inputFile);
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

    /* Still rockin' */

    it = nitf_List_at(record->images, idx);
    segment = (nitf_ImageSegment*)nitf_ListIterator_get(&it);

    /* TODO: Check most of the fields to make sure we are okay */
    imread = findImageReader(segment,extra);

    if (imread == NULL)
        return;
    
    
    mxImageArray = (*imread)(reader, idx, 0, 0, 
                             atol(segment->subheader->NITF_NROWS->raw),
                             atol(segment->subheader->NITF_NCOLS->raw),
                             extra);
    plhs[0] = mxImageArray;

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

    return;
}


