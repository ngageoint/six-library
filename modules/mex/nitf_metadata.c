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
#define MEX_NTF_ERR(E) \
    mexErrMsgTxt(( (E)->level == NITF_ERR_UNK) ? ("Unknown Error") : ( (E)->message))

/*
 *  Add a field from a nitf_Field* object into a mex object.
 *  The field in Matlab will have the name given by name.
 */
void addField(const char* name, mxArray* mexObj, nitf_Field* field)
{
    mxArray* mxStr = mxCreateString(field->raw);
    int fNum = mxAddField(mexObj, name);
    mxSetFieldByNumber(mexObj, 0, fNum, mxStr);
}

/*
 *  Add a C-string into a mex object.
 *  The field in Matlab will have the name given by name.
 */
void addString(const char* name, mxArray* mexObj, const char* str)
{
    mxArray* mxStr = mxCreateString(str);
    int fNum = mxAddField(mexObj, name);
    mxSetFieldByNumber(mexObj, 0, fNum, mxStr);
}

/*
 *  Create the NITF security record for any security section.
 *  This will use the generic names of the fields, since its
 *  allows us to reuse this function.
 */
static mxArray* createSecurityMx(nitf_FileSecurity* security)
{
    mxArray *mxSecurity = mxCreateStructMatrix(1, 1, 0, (const char**) NULL);

    addField("CLSY", mxSecurity, security->NITF_CLSY);
    addField("CODE", mxSecurity, security->NITF_CODE);
    addField("CTLH", mxSecurity, security->NITF_CTLH);
    addField("REL", mxSecurity, security->NITF_REL);
    addField("DCTP", mxSecurity, security->NITF_DCTP);
    addField("DCDT", mxSecurity, security->NITF_DCDT);
    addField("DCXM", mxSecurity, security->NITF_DCXM);
    addField("DG", mxSecurity, security->NITF_DG);
    addField("DGDT", mxSecurity, security->NITF_DGDT);
    addField("CLTX", mxSecurity, security->NITF_CLTX);
    addField("CATP", mxSecurity, security->NITF_CATP);
    addField("CAUT", mxSecurity, security->NITF_CAUT);
    addField("CRSN", mxSecurity, security->NITF_CRSN);
    addField("RDT", mxSecurity, security->NITF_RDT);
    addField("CTLN", mxSecurity, security->NITF_CTLN);

    return mxSecurity;
}

/*
 *  Turn a TRE into an object.  This will walk each value
 *  in the TRE.  NITRO has a particular method for naming these
 *  and rather than try and be clever with the mex, I currently
 *  just search and replace the names when they are in NITRO's
 *  array notation:
 *
 *  TRE_THING[0] => TRE_THING_0
 *
 */
static mxArray* createTREMx(nitf_TRE* tre)
{
    nitf_Error error;
    nitf_TREEnumerator* it = NULL;

    mxArray *mxTRE = mxCreateStructMatrix(1, 1, 0, (const char**) NULL);

    addString("CETAG", mxTRE, tre->tag);

    it = nitf_TRE_begin(tre, &error);

    while (it && it->hasNext(&it))
    {
        /* If this isn't set, it should have been an error */
        nitf_Pair* fieldPair = it->next(it, &error);
        if (fieldPair)
        {
            nitf_Field* field = NULL;
            /* Make a mutable copy of the key */
            char* key = (char*) malloc(strlen(fieldPair->key) + 1);
            strcpy(key, fieldPair->key);

            /* Replace offending fields */
            nitf_Utils_replace(key, '[', '_');
            nitf_Utils_replace(key, ']', ' ');
            nitf_Utils_trimString(key);

            /* Now set the key to the field */
            field = (nitf_Field*) fieldPair->data;
            addField(key, mxTRE, field);
            free(key);
        }
        else
            nitf_Error_print(&error, stdout, "Field retrieval error");

    }
    addString("__treID__", mxTRE, nitf_TRE_getID(tre));

    return mxTRE;
}

/*
 *  Create the Extensions object as a cell-array.  If there are no extensions,
 *  no big deal, they will be read in as an empty array anyway.
 */
static mxArray* createExtMx(nitf_Extensions* ext)
{
    int fNum = 0;

    /* Get the size of this cell array */
    int size = nitf_List_size(ext->ref);

    mxArray *mxExt = mxCreateCellMatrix(size, 1);

    nitf_ExtensionsIterator it;
    nitf_ExtensionsIterator end;

    /* Loop through and count */
    it = nitf_Extensions_begin(ext);
    end = nitf_Extensions_end(ext);

    while (nitf_ExtensionsIterator_notEqualTo(&it, &end))
    {
        nitf_TRE* tre = (nitf_TRE*) nitf_ExtensionsIterator_get(&it);

        //fNum = mxAddField(mxExt, tre->name);
        mxSetCell(mxExt, fNum++, createTREMx(tre));
        nitf_ExtensionsIterator_increment(&it);
    }

    return mxExt;
}

static mxArray* createImageCommentsMx(nitf_ImageSubheader* subheader)
{
    nitf_Error error;
    int fNum = 0;
    int size = 0;
    nitf_ListIterator it = nitf_List_begin(subheader->imageComments);
    nitf_ListIterator end = nitf_List_end(subheader->imageComments);
    mxArray* mxList = NULL;

    /* Faster to look this up then iterate the list twice */

    if (!nitf_Field_get(subheader->NITF_NICOM, &size, NITF_CONV_UINT,
                        NITF_INT32_SZ, &error))
        MEX_NTF_ERR(&error);

    mxList = mxCreateCellMatrix(size, 1);

    while (nitf_ListIterator_notEqualTo(&it, &end))
    {
        nitf_Field* field = (nitf_Field*) nitf_ListIterator_get(&it);
        mxArray* mxStr = mxCreateString(field->raw);
        mxSetCell(mxList, fNum++, mxStr);
        nitf_ListIterator_increment(&it);
    }
    return mxList;
}

/*
 *  Create the header struct to convert the nitf_FileHeader
 *  This converts all fields over, and assumes that they
 *  are string-able.
 */
static mxArray* createHeaderMx(nitf_FileHeader* header)
{
    int fNum = 0;

    mxArray *mxHeader = mxCreateStructMatrix(1, 1, 0, (const char**) NULL);

    addField("FHDR", mxHeader, header->NITF_FHDR);
    addField("FVER", mxHeader, header->NITF_FVER);
    addField("CLEVEL", mxHeader, header->NITF_CLEVEL);
    addField("STYPE", mxHeader, header->NITF_STYPE);
    addField("OSTAID", mxHeader, header->NITF_OSTAID);
    addField("FDT", mxHeader, header->NITF_FDT);
    addField("FTITLE", mxHeader, header->NITF_FTITLE);
    addField("FSCLAS", mxHeader, header->NITF_FSCLAS);

    addField("FSCOP", mxHeader, header->NITF_FSCOP);
    addField("FSCPYS", mxHeader, header->NITF_FSCPYS);
    addField("ENCRYP", mxHeader, header->NITF_ENCRYP);

    /* Skip background color for now */

    addField("ONAME", mxHeader, header->NITF_ONAME);
    addField("OPHONE", mxHeader, header->NITF_OPHONE);

    addField("FL", mxHeader, header->NITF_FL);
    addField("HL", mxHeader, header->NITF_HL);
    addField("NUMI", mxHeader, header->NITF_NUMI);
    addField("NUMS", mxHeader, header->NITF_NUMS);
    addField("NUMX", mxHeader, header->NITF_NUMX);
    addField("NUMT", mxHeader, header->NITF_NUMT);
    addField("NUMDES", mxHeader, header->NITF_NUMDES);
    addField("NUMRES", mxHeader, header->NITF_NUMRES);

    addField("UDHDL", mxHeader, header->NITF_UDHDL);
    addField("UDHOFL", mxHeader, header->NITF_UDHOFL);
    addField("XHDL", mxHeader, header->NITF_XHDL);
    addField("XHDLOFL", mxHeader, header->NITF_XHDLOFL);

    fNum = mxAddField(mxHeader, "FS");
    mxSetFieldByNumber(mxHeader, 0, fNum,
                       createSecurityMx(header->securityGroup));

    fNum = mxAddField(mxHeader, "UDHD");
    mxSetFieldByNumber(mxHeader, 0, fNum,
                       createExtMx(header->userDefinedSection));

    fNum = mxAddField(mxHeader, "XHD");
    mxSetFieldByNumber(mxHeader, 0, fNum, createExtMx(header->extendedSection));

    return mxHeader;
}

static mxArray* createBandInfoMx(nitf_BandInfo* bandInfo)
{
    mxArray *mxBandInfo = mxCreateStructMatrix(1, 1, 0, (const char**) NULL);

    addField("IREPBAND", mxBandInfo, bandInfo->NITF_IREPBAND);
    addField("ISUBCAT", mxBandInfo, bandInfo->NITF_ISUBCAT);
    addField("IFC", mxBandInfo, bandInfo->NITF_IFC);
    addField("IMFLT", mxBandInfo, bandInfo->NITF_IMFLT);
    addField("NLUTS", mxBandInfo, bandInfo->NITF_NLUTS);
    addField("NELUT", mxBandInfo, bandInfo->NITF_NELUT);
    return mxBandInfo;
}

static mxArray* createBandInfoArrayMx(nitf_ImageSubheader* subheader)
{
    int fNum = 0;
    int size;
    nitf_Error error;
    mxArray* mxList;

    /* First figure out how many band info's we are going to allocate */
    size = nitf_ImageSubheader_getBandCount(subheader, &error);
    if (size == NITF_INVALID_BAND_COUNT)
        MEX_NTF_ERR(&error);

    mxList = mxCreateCellMatrix(size, 1);

    /* Go through each band info and make a struct */

    for (fNum = 0; fNum < size; fNum++)
    {
        nitf_BandInfo* bandInfo = nitf_ImageSubheader_getBandInfo(subheader,
                                                                  fNum, &error);
        if (bandInfo == NULL)
            MEX_NTF_ERR(&error);

        mxSetCell(mxList, fNum++, createBandInfoMx(bandInfo));
    }

    return mxList;
}

static mxArray* createImageSubheaderMx(nitf_ImageSubheader* subheader)
{
    int fNum = 0;
    mxArray *mxSubheader = mxCreateStructMatrix(1, 1, 0, (const char**) NULL);

    addField("IM", mxSubheader, subheader->NITF_IM);
    addField("IID1", mxSubheader, subheader->NITF_IID1);
    addField("IDATIM", mxSubheader, subheader->NITF_IDATIM);
    addField("TGTID", mxSubheader, subheader->NITF_TGTID);
    addField("IID2", mxSubheader, subheader->NITF_IID2);
    addField("ISCLAS", mxSubheader, subheader->NITF_ISCLAS);

    fNum = mxAddField(mxSubheader, "IS");
    mxSetFieldByNumber(mxSubheader, 0, fNum,
                       createSecurityMx(subheader->securityGroup));

    addField("ENCRYP", mxSubheader, subheader->NITF_ENCRYP);
    addField("ISORCE", mxSubheader, subheader->NITF_ISORCE);

    addField("NROWS", mxSubheader, subheader->NITF_NROWS);
    addField("NCOLS", mxSubheader, subheader->NITF_NCOLS);

    addField("PVTYPE", mxSubheader, subheader->NITF_PVTYPE);
    addField("IREP", mxSubheader, subheader->NITF_IREP);
    addField("ICAT", mxSubheader, subheader->NITF_ICAT);

    addField("ABPP", mxSubheader, subheader->NITF_ABPP);

    addField("PJUST", mxSubheader, subheader->NITF_PJUST);
    addField("ICORDS", mxSubheader, subheader->NITF_ICORDS);
    addField("IGEOLO", mxSubheader, subheader->NITF_IGEOLO);

    addField("NICOM", mxSubheader, subheader->NITF_NICOM);

    fNum = mxAddField(mxSubheader, "ICOM");
    mxSetFieldByNumber(mxSubheader, 0, fNum, createImageCommentsMx(subheader));

    addField("IC", mxSubheader, subheader->NITF_IC);
    addField("COMRAT", mxSubheader, subheader->NITF_COMRAT);

    addField("NBANDS", mxSubheader, subheader->NITF_NBANDS);
    addField("XBANDS", mxSubheader, subheader->NITF_XBANDS);

    fNum = mxAddField(mxSubheader, "bands");
    mxSetFieldByNumber(mxSubheader, 0, fNum, createBandInfoArrayMx(subheader));

    addField("ISYNC", mxSubheader, subheader->NITF_ISYNC);
    addField("IMODE", mxSubheader, subheader->NITF_IMODE);

    addField("NBPR", mxSubheader, subheader->NITF_NBPR);
    addField("NBPC", mxSubheader, subheader->NITF_NBPC);
    addField("NPPBH", mxSubheader, subheader->NITF_NPPBH);
    addField("NPPBV", mxSubheader, subheader->NITF_NPPBV);
    addField("NBPP", mxSubheader, subheader->NITF_NBPP);

    addField("IDLVL", mxSubheader, subheader->NITF_IDLVL);
    addField("IALVL", mxSubheader, subheader->NITF_IALVL);
    addField("ILOC", mxSubheader, subheader->NITF_ILOC);
    addField("IMAG", mxSubheader, subheader->NITF_IMAG);

    addField("UDIDL", mxSubheader, subheader->NITF_UDIDL);
    addField("UDOFL", mxSubheader, subheader->NITF_UDOFL);
    addField("IXSHDL", mxSubheader, subheader->NITF_IXSHDL);
    addField("IXSOFL", mxSubheader, subheader->NITF_IXSOFL);

    fNum = mxAddField(mxSubheader, "UDID");
    mxSetFieldByNumber(mxSubheader, 0, fNum,
                       createExtMx(subheader->userDefinedSection));

    fNum = mxAddField(mxSubheader, "XSHD");
    mxSetFieldByNumber(mxSubheader, 0, fNum,
                       createExtMx(subheader->extendedSection));

    return mxSubheader;
}

static mxArray* createGraphicSubheaderMx(nitf_GraphicSubheader* subheader)
{
    int fNum = 0;
    mxArray *mxSubheader = mxCreateStructMatrix(1, 1, 0, (const char**) NULL);

    addField("SID", mxSubheader, subheader->NITF_SID);
    addField("SNAME", mxSubheader, subheader->NITF_SNAME);
    addField("SSCLAS", mxSubheader, subheader->NITF_SSCLAS);

    fNum = mxAddField(mxSubheader, "SS");
    mxSetFieldByNumber(mxSubheader, 0, fNum,
                       createSecurityMx(subheader->securityGroup));

    addField("SFMT", mxSubheader, subheader->NITF_SFMT);
    addField("SSTRUCT", mxSubheader, subheader->NITF_SSTRUCT);
    addField("SDLVL", mxSubheader, subheader->NITF_SDLVL);
    addField("SALVL", mxSubheader, subheader->NITF_SALVL);
    addField("SLOC", mxSubheader, subheader->NITF_SLOC);
    addField("SBND1", mxSubheader, subheader->NITF_SBND1);
    addField("SCOLOR", mxSubheader, subheader->NITF_SCOLOR);
    addField("SBND2", mxSubheader, subheader->NITF_SBND2);
    addField("SRES2", mxSubheader, subheader->NITF_SRES2);
    addField("SXSHDL", mxSubheader, subheader->NITF_SXSHDL);
    addField("SXSOFL", mxSubheader, subheader->NITF_SXSOFL);

    fNum = mxAddField(mxSubheader, "SXSHD");
    mxSetFieldByNumber(mxSubheader, 0, fNum,
                       createExtMx(subheader->extendedSection));

    return mxSubheader;
}

static mxArray* createTextSubheaderMx(nitf_TextSubheader* subheader)
{
    int fNum = 0;
    mxArray *mxSubheader = mxCreateStructMatrix(1, 1, 0, (const char**) NULL);

    addField("TE", mxSubheader, subheader->NITF_TE);
    addField("TEXTID", mxSubheader, subheader->NITF_TEXTID);
    addField("TXTALVL", mxSubheader, subheader->NITF_TXTALVL);
    addField("TXTDT", mxSubheader, subheader->NITF_TXTDT);
    addField("TXTITL", mxSubheader, subheader->NITF_TXTITL);
    addField("TSCLAS", mxSubheader, subheader->NITF_TSCLAS);

    fNum = mxAddField(mxSubheader, "TS");
    mxSetFieldByNumber(mxSubheader, 0, fNum,
                       createSecurityMx(subheader->securityGroup));

    addField("ENCRYP", mxSubheader, subheader->NITF_ENCRYP);
    addField("TXTFMT", mxSubheader, subheader->NITF_TXTFMT);
    addField("TXSHDL", mxSubheader, subheader->NITF_TXSHDL);
    addField("TXSOFL", mxSubheader, subheader->NITF_TXSOFL);

    fNum = mxAddField(mxSubheader, "TXSHD");
    mxSetFieldByNumber(mxSubheader, 0, fNum,
                       createExtMx(subheader->extendedSection));

    return mxSubheader;
}

static mxArray* createDESubheaderMx(nitf_DESubheader* subheader)
{
    int fNum = 0;
    mxArray *mxSubheader = mxCreateStructMatrix(1, 1, 0, (const char**) NULL);

    addField("DE", mxSubheader, subheader->filePartType);
    addField("DESTAG", mxSubheader, subheader->NITF_DESTAG);
    addField("DESVER", mxSubheader, subheader->NITF_DESVER);
    addField("DESCLAS", mxSubheader, subheader->NITF_DESCLAS);

    fNum = mxAddField(mxSubheader, "DES");
    mxSetFieldByNumber(mxSubheader, 0, fNum,
                       createSecurityMx(subheader->securityGroup));

    addField("DESOFLW", mxSubheader, subheader->NITF_DESOFLW);
    addField("DESITEM", mxSubheader, subheader->NITF_DESITEM);
    addField("DESSHL", mxSubheader, subheader->NITF_DESSHL);

    fNum = mxAddField(mxSubheader, "DESDATA");
    mxSetFieldByNumber(mxSubheader, 0, fNum,
                       createExtMx(subheader->userDefinedSection));

    return mxSubheader;
}

/*
 *  Create the Matlab struct to represent the Record.
 *  This will create separate structs for each of the
 *  sub-items
 *
 */
static mxArray* createRecordMx(nitf_Record* record)
{
    nitf_Error error;
    int fNum = 0;
    int size = 0;
    nitf_ListIterator it;
    nitf_ListIterator end;

    mxArray *mxList = NULL;
    mxArray *mxRecord = mxCreateStructMatrix(1, 1, 0, (const char**) NULL);

    mxArray* mxHeader = NULL;

    fNum = mxAddField(mxRecord, "header");
    mxHeader = createHeaderMx(record->header);
    mxSetFieldByNumber(mxRecord, 0, fNum, mxHeader);

    /* images */
    {
        size = nitf_Record_getNumImages(record, &error);
        mxList = mxCreateCellMatrix(size, 1);

        it = nitf_List_begin(record->images);
        end = nitf_List_end(record->images);

        fNum = 0;
        while (nitf_ListIterator_notEqualTo(&it, &end))
        {
            nitf_ImageSegment* imageSegment =
                    (nitf_ImageSegment*) nitf_ListIterator_get(&it);
            mxSetCell(mxList, fNum++,
                      createImageSubheaderMx(imageSegment->subheader));
            nitf_ListIterator_increment(&it);
        }

        /* Now add our cell matrix to the record */
        fNum = mxAddField(mxRecord, "images");
        mxSetFieldByNumber(mxRecord, 0, fNum, mxList);
    }

    /* graphics */
    {
        size = nitf_Record_getNumGraphics(record, &error);
        mxList = mxCreateCellMatrix(size, 1);

        it = nitf_List_begin(record->graphics);
        end = nitf_List_end(record->graphics);

        fNum = 0;
        while (nitf_ListIterator_notEqualTo(&it, &end))
        {
            nitf_GraphicSegment* segment =
                    (nitf_GraphicSegment*) nitf_ListIterator_get(&it);
            mxSetCell(mxList, fNum++,
                      createGraphicSubheaderMx(segment->subheader));
            nitf_ListIterator_increment(&it);
        }

        /* Now add our cell matrix to the record */
        fNum = mxAddField(mxRecord, "graphics");
        mxSetFieldByNumber(mxRecord, 0, fNum, mxList);
    }

    /* texts */
    {
        size = nitf_Record_getNumTexts(record, &error);
        mxList = mxCreateCellMatrix(size, 1);

        it = nitf_List_begin(record->texts);
        end = nitf_List_end(record->texts);

        fNum = 0;
        while (nitf_ListIterator_notEqualTo(&it, &end))
        {
            nitf_TextSegment* segment =
                    (nitf_TextSegment*) nitf_ListIterator_get(&it);
            mxSetCell(mxList, fNum++, createTextSubheaderMx(segment->subheader));
            nitf_ListIterator_increment(&it);
        }

        /* Now add our cell matrix to the record */
        fNum = mxAddField(mxRecord, "texts");
        mxSetFieldByNumber(mxRecord, 0, fNum, mxList);
    }

    /* DES */
    {
        size = nitf_Record_getNumDataExtensions(record, &error);
        mxList = mxCreateCellMatrix(size, 1);

        it = nitf_List_begin(record->dataExtensions);
        end = nitf_List_end(record->dataExtensions);

        fNum = 0;
        while (nitf_ListIterator_notEqualTo(&it, &end))
        {
            nitf_DESegment* segment =
                    (nitf_DESegment*) nitf_ListIterator_get(&it);
            mxSetCell(mxList, fNum++, createDESubheaderMx(segment->subheader));
            nitf_ListIterator_increment(&it);
        }

        /* Now add our cell matrix to the record */
        fNum = mxAddField(mxRecord, "dataExtensions");
        mxSetFieldByNumber(mxRecord, 0, fNum, mxList);
    }

    return mxRecord;
}

/*
 *  Allocate a C-string from a Matlab array
 */
static char* newString(const mxArray* mx)
{
    size_t len;
    char* str = NULL;
    if (!mxIsChar(mx))
        mexErrMsgTxt("Require string arg");

    if (mxGetM(mx) != 1)
        mexErrMsgTxt("Input must be a row vector");

    len = (mxGetM(mx) * mxGetN(mx)) + 1;
    str = (char*) malloc(len + 1);
    str[len] = 0;
    if (mxGetString(mx, str, len) != 0)
    {
        free(str);
        mexErrMsgTxt("Not enough space!");
    }
    return str;
}

/**
 *  The first thing to do here is read all of the nitf fields
 *  and then the TREs
 */
void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{

    /* Our nitf Reader */
    nitf_Reader* reader;

    /* The meta-data record */
    nitf_Record* record;

    /* Handle to the file */
    nitf_IOHandle io;

    /* Local error */
    nitf_Error error;

    /* Pointer to input file name */
    char *inputFile = NULL;

    /* Returned array */
    mxArray* mxRecord = NULL;

    if (nrhs != 1 && nrhs != 2)
    {
        mexErrMsgTxt("<file-name> (nitf-plugin-path)");
        return;
    }

    if (nrhs == 2)
    {
        char* pluginPath = newString(prhs[1]);
        /* Alternative to NITF_PLUGIN_PATH */
        if (!nitf_PluginRegistry_loadDir(pluginPath, &error))
        {
            free(pluginPath);
            MEX_NTF_ERR(&error);
        }
        free(pluginPath);
    }

    /* Output will be a single struct */
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
    io = nitf_IOHandle_create(inputFile, NITF_ACCESS_READONLY,
                              NITF_OPEN_EXISTING, &error);

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
    mxRecord = createRecordMx(record);
    plhs[0] = mxRecord;
}

