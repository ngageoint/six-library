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
 */

typedef mxArray* (*MEX_NTF_IMREAD)(nitf_Reader* reader,
                                   int idx,
                                   long startRow,
                                   long startCol,
                                   long endRow,
                                   long endCol);


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
                                  long numCols)
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
        mxCreateNumericMatrix(numRows, numCols, mxSINGLE_CLASS, mxREAL);

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
                            long numCols)
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
        mxCreateNumericMatrix(numRows, numCols, mxUINT8_CLASS, mxREAL);

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
 * TODO: Implement me using normal mode, assign Pr and Pi using each 
 * deinterleaved band
 */
mxArray* readComplexPixelArray(nitf_Reader* reader,
                               int idx,
                               long startRow, 
                               long startCol,
                               long endRow, 
                               long endCol)
{
    return NULL;
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



                              
MEX_NTF_IMREAD findImageReader(nitf_ImageSegment* segment)
{
    /* Figure out if we have a float or a byte for now */
    int pixelDepth = 
        NITF_NBPP_TO_BYTES(atol(segment->subheader->NITF_NBPP->raw));
    
    const char* pixelValue = segment->subheader->NITF_PVTYPE->raw;

    if (pixelDepth == 1)
    {
        return &read8BitPixelArray;
    }
    /* Deal with real mode */
    else if (memcmp(pixelValue, "R", 1) == 0 && pixelDepth == 4)
    {
        return &read32BitFloatPixelArray;
    }
    else
    {
    	mexErrMsgTxt("Sorry, this MEX wrapper doesnt currently handle the pixel type");
        return NULL;
    }
    
}

/**
 *  The first thing to do here is read all of the nitf fields
 *  and then the TREs
 */
void mexFunction(int nlhs, mxArray *plhs[],
		 int nrhs, const mxArray *prhs[])
{

    /* Our nitf Reader */
    nitf_Reader* reader;
    
    /* The meta-data record */
    nitf_Record* record;

    /* Handle to the file */
    nitf_IOHandle io;

    /* Local error */
    nitf_Error error;

    /* Iterator for entry at idx */
    nitf_ListIterator it;
    
    /* Segment to be used */
    nitf_ImageSegment* segment;

    /* Pointer to input file name */
    char *inputFile = NULL;

    /* index */
    int idx = 0;

    /* pointer to a function of value */
    MEX_NTF_IMREAD imread = NULL;

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
    imread = findImageReader(segment);

    if (imread == NULL)
        return;
    

    
    mxImageArray = (*imread)(reader, idx, 0, 0, 
                             atol(segment->subheader->NITF_NROWS->raw),
                             atol(segment->subheader->NITF_NCOLS->raw));
    plhs[0] = mxImageArray;
}


