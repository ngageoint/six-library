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

#include <assert.h>

#include "nitf/Record.h"

/*
 * Destroy a list items that we have in our record.
 * The first argument is the list pointer.
 * The second argument is the type of object in the list
 * For example, here is how to destroy the images segments:
 *
 * _NITF_TRASH_LIST(r->images, nitf_ImageSegment)
 *
 * Lp = r->images
 * Ty = nitf_ImageSegment
 * so...
 * while (!nitf_List_isEmpty(r->images))
 * {
 *     nitf_ImageSegment* type =
 *           (nitf_ImageSegment*)nitf_List_popFront(r->images);
 *
 *     nitf_ImageSegment_destruct(& type );
 * }
 *
 * Note that we dont want a semicolon at the end
 */
#define _NITF_TRASH_LIST(Lp_, Ty)                \
    while (!nitf_List_isEmpty(Lp_))              \
    {                                            \
        Ty* type = (Ty*)nitf_List_popFront(Lp_); \
        Ty##_destruct(&type);                    \
    }

/* Local functions and constants for overflow segments */

#define TRE_OVERFLOW_STR "TRE_OVERFLOW"
#define TRE_OVERFLOW_VERSION 1

NITFAPI(uint32_t)
nitf_Record_getNumReservedExtensions(const nitf_Record* record,
                                     nitf_Error* error)
{
    const nitf_FileHeader* fhdr = record->header;
    uint32_t num;

    /*  This can only really happen if they have junk in NUMI */
    if (!nitf_Field_get(fhdr->NITF_NUMRES,
                        &num,
                        NITF_CONV_UINT,
                        sizeof(uint32_t),
                        error))
        return (uint32_t)-1;

    return num;
}

NITFAPI(uint32_t)
nitf_Record_getNumDataExtensions(const nitf_Record* record, nitf_Error* error)
{
    const nitf_FileHeader* fhdr = record->header;
    uint32_t num;

    /*  This can only really happen if they have junk in the field */
    if (!nitf_Field_get(fhdr->NITF_NUMDES,
                        &num,
                        NITF_CONV_UINT,
                        sizeof(uint32_t),
                        error))
        return (uint32_t)-1;

    return num;
}

NITFAPI(uint32_t)
nitf_Record_getNumGraphics(const nitf_Record* record, nitf_Error* error)
{
    const nitf_FileHeader* fhdr = record->header;
    uint32_t num;

    /*  This can only really happen if they have junk in the field */
    if (!nitf_Field_get(fhdr->NITF_NUMS,
                        &num,
                        NITF_CONV_UINT,
                        sizeof(uint32_t),
                        error))
        return (uint32_t)-1;

    return num;
}

NITFAPI(uint32_t)
nitf_Record_getNumTexts(const nitf_Record* record, nitf_Error* error)
{
    const nitf_FileHeader* fhdr = record->header;
    uint32_t num;

    /*  This can only really happen if they have junk in the field */
    if (!nitf_Field_get(fhdr->NITF_NUMT,
                        &num,
                        NITF_CONV_UINT,
                        sizeof(uint32_t),
                        error))
        return (uint32_t)-1;

    return num;
}

NITFAPI(uint32_t)
nitf_Record_getNumLabels(const nitf_Record* record, nitf_Error* error)
{
    const nitf_FileHeader* fhdr = record->header;
    uint32_t num;

    /*  This can only really happen if they have junk in the field */
    if (!nitf_Field_get(fhdr->NITF_NUMX,
                        &num,
                        NITF_CONV_UINT,
                        sizeof(uint32_t),
                        error))
        return (uint32_t)-1;

    return num;
}

NITFAPI(uint32_t)
nitf_Record_getNumImages(const nitf_Record* record, nitf_Error* error)
{
    const nitf_FileHeader* fhdr = record->header;
    uint32_t num;

    /*  This can only really happen if they have junk in the field */
    if (!nitf_Field_get(fhdr->NITF_NUMI,
                        &num,
                        NITF_CONV_UINT,
                        sizeof(uint32_t),
                        error))
        return (uint32_t)-1;
    return num;
}

/*
 * addOverflowSegment adds a (DE) overflow section
 *
 * The security section in the new sections's header is initialized from the
 * supplied file security object which should come from the assoicated segment
 *
 * All fields are set except DESSHL and DESDATA
 *
 * The index one based of the new segment is returned, or zero on error
 *
 * \param record Record to modify
 * \param segmentIndex Index of associated segment
 * \param segmentType security information from associated segment
 * \param seecurityClass the classification value
 * \param fileSecurity The file security record
 * \param overflow A new DES segment
 * \param error The error that occured if index is zero
 */

NITFPRIV(uint32_t)
addOverflowSegment(nitf_Record* record,
                   uint32_t segmentIndex,
                   char* segmentType,
                   nitf_Field* securityClass,
                   nitf_FileSecurity* fileSecurity,
                   nitf_DESegment** overflow,
                   nitf_Error* error)
{
    uint32_t overflowIndex;

    /* Create the segment */

    overflowIndex = nitf_List_size(record->dataExtensions) + 1;
    *overflow = nitf_Record_newDataExtensionSegment(record, error);
    if (*overflow == NULL)
    {
        nitf_Error_init(error,
                        "Could not add overflow segment index",
                        NITF_CTXT,
                        NITF_ERR_INVALID_OBJECT);
        return 0;
    }

    /* Set fields */
    nitf_FileSecurity_destruct(&((*overflow)->subheader->securityGroup));
    (*overflow)->subheader->securityGroup =
            nitf_FileSecurity_clone(fileSecurity, error);

    nitf_Field_destruct(&((*overflow)->subheader->securityClass));
    (*overflow)->subheader->securityClass =
            nitf_Field_clone(securityClass, error);
    if ((*overflow)->subheader->securityClass == NULL)
        return 0;

    if (!nitf_Field_setString((*overflow)->subheader->NITF_DESTAG,
                              TRE_OVERFLOW_STR,
                              error))
        return 0;

    if (!nitf_Field_setUint32((*overflow)->subheader->NITF_DESVER,
                              TRE_OVERFLOW_VERSION,
                              error))
        return 0;

    if (!nitf_Field_setString((*overflow)->subheader->NITF_DESOFLW,
                              segmentType,
                              error))
        return 0;

    if (!nitf_Field_setUint32((*overflow)->subheader->NITF_DESITEM,
                              segmentIndex,
                              error))
        return 0;

    return overflowIndex;
}

/*
 * moveTREs - Move TREs from one section to another
 *
 * If the skip length is greater than zero, then initial TREs are skpped
 * until the first TRE that makes the total skipped larger than the skip
 * length. This is for transferring from the main segment to the DE segment
 * (unmerge), the zero case is for DE to main segment (merge)
 *
 * The return is TRUE on success and FALSE on failure
 *
 * \param destination Source extension
 * \param skipLength Amount of TRE data to skip
 * \param error For errors
 */
NITFPRIV(NITF_BOOL)
moveTREs(nitf_Extensions* source,
         nitf_Extensions* destination,
         uint32_t skipLength,
         nitf_Error* error)
{
    nitf_ExtensionsIterator srcIter; /* Source extension iterator */
    nitf_ExtensionsIterator srcEnd; /* Source extension iterator end */
    nitf_TRE* tre; /* Current TRE */

    srcIter = nitf_Extensions_begin(source);
    srcEnd = nitf_Extensions_end(source);

    if (skipLength != 0)
    {
        int32_t skipLeft; /* Amount left to skip */
        uint32_t treLength; /* Length of current TRE */

        skipLeft = skipLength;
        while (nitf_ExtensionsIterator_notEqualTo(&srcIter, &srcEnd))
        {
            tre = nitf_ExtensionsIterator_get(&srcIter);
            treLength = (uint32_t)tre->handler->getCurrentSize(tre, error);
            treLength += NITF_ETAG_SZ + NITF_EL_SZ;
            skipLeft -= treLength;
            if (skipLeft < 1)
                break;
            nitf_ExtensionsIterator_increment(&srcIter);
        }
        if (skipLeft > 1) /* No transfer required */
            srcIter = nitf_Extensions_end(source);
    }

    srcEnd = nitf_Extensions_end(source);

    while (nitf_ExtensionsIterator_notEqualTo(&srcIter, &srcEnd))
    {
        tre = nitf_Extensions_remove(source, &srcIter, error);
        nitf_Extensions_appendTRE(destination, tre, error);
    }

    return NITF_SUCCESS;
}

/*
 * fixOverflowIndexes reviews and corrects the indexes of DE overflow
 * segments when any segment that might overflow (i.e., image segment) is
 * removed. Removing a segment could effect the indexes in existing overflowed
 * segments.
 *
 * If the segment being deleted does not have an overflow segment. If it has
 * one, it shold be deleted before this function is called
 *
 * This function also removes the associated DE segent if it still exists
 *
 * Note: Adding a segment does not effect any existing overflows because new
 * segments are added at the end of the corresponding segment list
 *
 * \param record Record
 * \param type  The type of DES
 * \param segmentIndex Segment index (zero based)
 * \param error Error
 *
 */
NITFPRIV(NITF_BOOL)
fixOverflowIndexes(nitf_Record* record,
                   char* type,
                   uint32_t segmentIndex,
                   nitf_Error* error)
{
    nitf_ListIterator deIter; /* For DE iterating */
    nitf_ListIterator deEnd; /* End point */
    nitf_DESubheader* subheader; /* Current DE segment subheader */
    char oflw[NITF_DESOFLW_SZ + 1]; /* Parent segment type (DESOFLW) */
    uint32_t item; /* Segment index (DESITEM) */

    /*
     * Scan extensions for ones that have the same type
     * and a index greater than
     * the index of the segment being deleted. If this is
     * the case decrement the
     * overflow's index
     */

    deIter = nitf_List_begin(record->dataExtensions);
    deEnd = nitf_List_begin(record->dataExtensions);
    while (nitf_ListIterator_notEqualTo(&deIter, &deEnd))
    {
        subheader = (nitf_DESubheader*)((nitf_DESegment*)nitf_ListIterator_get(
                                                &deIter))
                            ->subheader;

        if (!nitf_Field_get(subheader->NITF_DESOFLW,
                            (NITF_DATA*)oflw,
                            NITF_CONV_STRING,
                            NITF_DESOFLW_SZ + 1,
                            error))
        {
            nitf_Error_init(error,
                            "Could not retrieve DESOVFLW field",
                            NITF_CTXT,
                            NITF_ERR_INVALID_OBJECT);
            return NITF_FAILURE;
        }

        if (!nitf_Field_get(subheader->NITF_DESITEM,
                            (NITF_DATA*)&item,
                            NITF_CONV_UINT,
                            sizeof(item),
                            error))
        {
            nitf_Error_init(error,
                            "Could not retrieve DESITEM field",
                            NITF_CTXT,
                            NITF_ERR_INVALID_OBJECT);
            return NITF_FAILURE;
        }

        if ((strcmp(oflw, type) == 0) && (item > (segmentIndex + 1)))
            if (!nitf_Field_setUint32(subheader->NITF_DESITEM, item - 1, error))
                return 0;

        nitf_ListIterator_increment(&deIter);
    }

    return NITF_SUCCESS;
}

/*
 * fixSegmentIndexes reviews and corrects the indexes of all overflowing
 * segments (i.e., image segment) when any DE segment is removed. Removing a
 * DE segment could effect the indexes of any existing overflowed segments.
 * This function is called when any DE segment is remove, not just overflow
 * segments. The file header is also checked.
 *
 * Note: Adding a DE segment does not effect any existing overflows because new
 * segments are added at the end of the corresponding segment list
 *
 * \param record Record to fix
 * \param index DE segment of removed segment zero based
 * \param error for Error returns
 *
 */

NITFPRIV(NITF_BOOL)
fixSegmentIndexes(nitf_Record* record,
                  uint32_t segmentIndex,
                  nitf_Error* error)
{
    nitf_FileHeader* header; /* File header */
    nitf_ListIterator segIter; /* Current segment list */
    nitf_ListIterator segEnd; /* Current segment list end */
    uint32_t deIndex; /* Desegment index */

    /* File header */

    header = record->header;
    if (!nitf_Field_get(header->NITF_UDHOFL,
                        (NITF_DATA*)&deIndex,
                        NITF_CONV_UINT,
                        sizeof(deIndex),
                        error))
        return NITF_FAILURE;

    if (deIndex > (segmentIndex + 1))
        if (!nitf_Field_setUint32(header->NITF_UDHOFL, deIndex - 1, error))
            return NITF_FAILURE;

    if (!nitf_Field_get(header->NITF_XHDLOFL,
                        (NITF_DATA*)&deIndex,
                        NITF_CONV_UINT,
                        sizeof(deIndex),
                        error))
        return NITF_FAILURE;

    if (deIndex > (segmentIndex + 1))
        if (!nitf_Field_setUint32(header->NITF_XHDLOFL, deIndex - 1, error))
            return NITF_FAILURE;

    /* Image segments */

    segIter = nitf_List_begin(record->images);
    segEnd = nitf_List_end(record->images);
    while (nitf_ListIterator_notEqualTo(&segIter, &segEnd))
    {
        nitf_ImageSubheader* subheader; /* Current subheader */

        subheader =
                (nitf_ImageSubheader*)((nitf_ImageSegment*)
                                               nitf_ListIterator_get(&segIter))
                        ->subheader;

        if (!nitf_Field_get(subheader->NITF_UDOFL,
                            (NITF_DATA*)&deIndex,
                            NITF_CONV_UINT,
                            sizeof(deIndex),
                            error))
            return NITF_FAILURE;

        if (deIndex > (segmentIndex + 1))
            if (!nitf_Field_setUint32(subheader->NITF_UDOFL,
                                      deIndex - 1,
                                      error))
                return NITF_FAILURE;

        if (!nitf_Field_get(subheader->NITF_IXSOFL,
                            (NITF_DATA*)&deIndex,
                            NITF_CONV_UINT,
                            sizeof(deIndex),
                            error))
            return NITF_FAILURE;

        if (deIndex > (segmentIndex + 1))
            if (!nitf_Field_setUint32(subheader->NITF_IXSOFL,
                                      deIndex - 1,
                                      error))
                return NITF_FAILURE;

        nitf_ListIterator_increment(&segIter);
    }

    /* Graphics segments */

    segIter = nitf_List_begin(record->images);
    segEnd = nitf_List_end(record->images);
    while (nitf_ListIterator_notEqualTo(&segIter, &segEnd))
    {
        nitf_GraphicSubheader* subheader; /* Current subheader */

        subheader = (nitf_GraphicSubheader*)((nitf_GraphicSegment*)
                                                     nitf_ListIterator_get(
                                                             &segIter))
                            ->subheader;

        if (!nitf_Field_get(subheader->NITF_SXSOFL,
                            (NITF_DATA*)&deIndex,
                            NITF_CONV_UINT,
                            sizeof(deIndex),
                            error))
            return NITF_FAILURE;

        if (deIndex > (segmentIndex + 1))
            if (!nitf_Field_setUint32(subheader->NITF_SXSOFL,
                                      deIndex - 1,
                                      error))
                return NITF_FAILURE;

        nitf_ListIterator_increment(&segIter);
    }

    /* Label segments */

    segIter = nitf_List_begin(record->labels);
    segEnd = nitf_List_end(record->labels);
    while (nitf_ListIterator_notEqualTo(&segIter, &segEnd))
    {
        nitf_LabelSubheader* subheader; /* Current subheader */

        subheader =
                (nitf_LabelSubheader*)((nitf_LabelSegment*)
                                               nitf_ListIterator_get(&segIter))
                        ->subheader;

        if (!nitf_Field_get(subheader->NITF_LXSOFL,
                            (NITF_DATA*)&deIndex,
                            NITF_CONV_UINT,
                            sizeof(deIndex),
                            error))
            return NITF_FAILURE;

        if (deIndex > (segmentIndex + 1))
            if (!nitf_Field_setUint32(subheader->NITF_LXSOFL,
                                      deIndex - 1,
                                      error))
                return NITF_FAILURE;

        nitf_ListIterator_increment(&segIter);
    }

    /* Text segments */

    segIter = nitf_List_begin(record->images);
    segEnd = nitf_List_end(record->images);
    while (nitf_ListIterator_notEqualTo(&segIter, &segEnd))
    {
        nitf_TextSubheader* subheader; /* Current subheader */

        subheader =
                (nitf_TextSubheader*)((nitf_GraphicSegment*)
                                              nitf_ListIterator_get(&segIter))
                        ->subheader;

        if (!nitf_Field_get(subheader->NITF_TXSOFL,
                            (NITF_DATA*)&deIndex,
                            NITF_CONV_UINT,
                            sizeof(deIndex),
                            error))
            return NITF_FAILURE;

        if (deIndex > (segmentIndex + 1))
            if (!nitf_Field_setUint32(subheader->NITF_TXSOFL,
                                      deIndex - 1,
                                      error))
                return NITF_FAILURE;
        nitf_ListIterator_increment(&segIter);
    }

    return NITF_SUCCESS;
}

/*
 *  This macro assumes existance of some kind of NITF header/subheader named
 *  'header,' an error named 'error', and a catch clause called 'CATCH_ERROR'
 *  within the caller
 */
#define _NITF_SET_FIELD(fld, val)                                              \
    if (!nitf_Field_setRawData(header->fld, (NITF_DATA*)val, fld##_SZ, error)) \
    goto CATCH_ERROR

NITFPRIV(nitf_FileHeader*)
createDefaultFileHeader(nitf_Version version, nitf_Error* error)
{
    nitf_FileHeader* header = nitf_FileHeader_construct(error);

    /* Got nothing */
    if (!header)
        return NULL;

    _NITF_SET_FIELD(NITF_FHDR, "NITF");

    /* NOW, set any version-specific data */
    if (version == NITF_VER_20)
    {
        _NITF_SET_FIELD(NITF_FVER, "02.00");
        /* Resize for NITF 2.0 */
        if (!nitf_FileSecurity_resizeForVersion(header->securityGroup,
                                                version,
                                                error))
            goto CATCH_ERROR;
    }

    /* The default is 2.1 */
    else
    {
        _NITF_SET_FIELD(NITF_FVER, "02.10");
    }

    _NITF_SET_FIELD(NITF_STYPE, "BF01");

    /* Set 'U' by default.  This makes life easier for lots of folks */
    _NITF_SET_FIELD(NITF_FSCLAS, "U");

    /* I can almost guarantee its not encrypted */
    _NITF_SET_FIELD(NITF_ENCRYP, "0");

    return header;

CATCH_ERROR:
    nitf_FileHeader_destruct(&header);
    return NULL;
}

NITFPRIV(nitf_ImageSegment*)
createDefaultImageSegment(nitf_Version version,
                          uint32_t displayLevel,
                          nitf_Error* error)
{
    /* Create the new segment */
    nitf_ImageSegment* segment = nitf_ImageSegment_construct(error);
    nitf_ImageSubheader* header = segment->subheader;

    /* Update the version of the FileSecurity, if necessary */

    if (version == NITF_VER_20)
    {
        /* Resize */
        nitf_FileSecurity_resizeForVersion(header->securityGroup,
                                           version,
                                           error);
    }

    _NITF_SET_FIELD(NITF_IM, "IM");
    _NITF_SET_FIELD(NITF_ENCRYP, "0");
    /* This *could* be 'L', but its unlikely */
    _NITF_SET_FIELD(NITF_PJUST, "R");
    /* This is often set to 1.0 */
    _NITF_SET_FIELD(NITF_IMAG, "1.0 ");
    /* IC should default to NC -- we dont know we have a mask yet */
    _NITF_SET_FIELD(NITF_IC, "NC");

    _NITF_SET_FIELD(NITF_ISCLAS, "U");

    if (!nitf_Field_setUint32(header->NITF_IDLVL, displayLevel, error))
        goto CATCH_ERROR;

    return segment;

CATCH_ERROR:
    return NITF_FAILURE;
}

NITFAPI(nitf_Record*)
nitf_Record_construct(nitf_Version version, nitf_Error* error)
{
    nitf_Record* record = (nitf_Record*)NITF_MALLOC(sizeof(nitf_Record));
    if (!record)
    {
        nitf_Error_init(error,
                        NITF_STRERROR(NITF_ERRNO),
                        NITF_CTXT,
                        NITF_ERR_MEMORY);
        return NULL;
    }

    /* Right now, we are only doing these  */
    record->header = NULL;
    record->images = NULL;

    /* Right now, we aren't doing these things  */
    record->graphics = NULL;
    record->labels = NULL;
    record->texts = NULL;
    record->dataExtensions = NULL;
    record->reservedExtensions = NULL;

    /*
     * This block does the children creations
     * In the event of a constructor error, the
     * error object is already populated, and
     * since we have all of our objects NULL-inited
     * we have the same behavior for each failure.
     * It lives at the end and is called TRAGIC
     */
    record->header = createDefaultFileHeader(version, error);
    if (!record->header)
        goto CATCH_TRAGIC;

    record->images = nitf_List_construct(error);
    if (!record->images)
        goto CATCH_TRAGIC;

    record->graphics = nitf_List_construct(error);
    if (!record->graphics)
        goto CATCH_TRAGIC;

    record->labels = nitf_List_construct(error);
    if (!record->labels)
        goto CATCH_TRAGIC;

    record->texts = nitf_List_construct(error);
    if (!record->texts)
        goto CATCH_TRAGIC;

    record->dataExtensions = nitf_List_construct(error);
    if (!record->dataExtensions)
        goto CATCH_TRAGIC;

    record->reservedExtensions = nitf_List_construct(error);
    if (!record->reservedExtensions)
        goto CATCH_TRAGIC;

    /* If all went well, we are very happy.  Now return  */
    return record;

CATCH_TRAGIC:
    /*
     * If something went wrong, somebody threw to us
     * So tragic!  So young to die!
     */
    nitf_Record_destruct(&record);
    return NULL;
}

NITFAPI(nitf_Record*)
nitf_Record_clone(const nitf_Record* source, nitf_Error* error)
{
    nitf_Record* record = NULL;

    if (!source)
    {
        nitf_Error_initf(error,
                         NITF_CTXT,
                         NITF_ERR_INVALID_OBJECT,
                         "Trying to clone NULL pointer");
        return NULL;
    }

    record = (nitf_Record*)NITF_MALLOC(sizeof(nitf_Record));
    if (!record)
    {
        nitf_Error_init(error,
                        NITF_STRERROR(NITF_ERRNO),
                        NITF_CTXT,
                        NITF_ERR_MEMORY);
        return NULL;
    }

    /* Null-set in case we auto-destruct  */
    record->header = NULL;
    record->images = NULL;
    record->graphics = NULL;
    record->labels = NULL;
    record->texts = NULL;
    record->dataExtensions = NULL;
    record->reservedExtensions = NULL;

    /* Right now, we are only doing the header and image setup  */
    record->header = nitf_FileHeader_clone(source->header, error);
    if (!record->header)
    {
        /* Destruct gracefully if we had a problem  */
        nitf_Record_destruct(&record);
        return NULL;
    }
    record->images =
            nitf_List_clone(source->images,
                            (NITF_DATA_ITEM_CLONE)nitf_ImageSegment_clone,
                            error);

    if (!record->images)
    {
        /* Destruct gracefully if we had a problem  */
        nitf_Record_destruct(&record);
        return NULL;
    }

    record->graphics =
            nitf_List_clone(source->graphics,
                            (NITF_DATA_ITEM_CLONE)nitf_GraphicSegment_clone,
                            error);
    if (!record->graphics)
    {
        /* Destruct gracefully if we had a problem  */
        nitf_Record_destruct(&record);
        return NULL;
    }

    record->labels =
            nitf_List_clone(source->labels,
                            (NITF_DATA_ITEM_CLONE)nitf_LabelSegment_clone,
                            error);
    if (!record->labels)
    {
        /* Destruct gracefully if we had a problem  */
        nitf_Record_destruct(&record);
        return NULL;
    }

    record->texts =
            nitf_List_clone(source->texts,
                            (NITF_DATA_ITEM_CLONE)nitf_TextSegment_clone,
                            error);
    if (!record->texts)
    {
        /* Destruct gracefully if we had a problem  */
        nitf_Record_destruct(&record);
        return NULL;
    }

    record->dataExtensions =
            nitf_List_clone(source->dataExtensions,
                            (NITF_DATA_ITEM_CLONE)nitf_DESegment_clone,
                            error);
    if (!record->dataExtensions)
    {
        /* Destruct gracefully if we had a problem  */
        nitf_Record_destruct(&record);
        return NULL;
    }

    record->reservedExtensions =
            nitf_List_clone(source->reservedExtensions,
                            (NITF_DATA_ITEM_CLONE)nitf_RESegment_clone,
                            error);
    if (!record->reservedExtensions)
    {
        /* Destruct gracefully if we had a problem  */
        nitf_Record_destruct(&record);
        return NULL;
    }

    return record;
}

NITFAPI(void) nitf_Record_destruct(nitf_Record** record)
{
    if (*record)
    {
        if ((*record)->header)
        {
            nitf_FileHeader_destruct(&(*record)->header);
        }

        /*
         * When destroying these, we always use the same macro
         * The macro is walking the list and deleting the items
         * by calling the segment destructors by name
         */
        if ((*record)->images)
        {
            /* Walk the list and destroy it  */
            _NITF_TRASH_LIST((*record)->images, nitf_ImageSegment)
            nitf_List_destruct(&(*record)->images);
        }

        if ((*record)->graphics)
        {
            /* Walk the list and destroy it  */
            _NITF_TRASH_LIST((*record)->graphics, nitf_GraphicSegment)
            nitf_List_destruct(&(*record)->graphics);
        }

        if ((*record)->labels)
        {
            /* Walk the list and destroy it  */
            _NITF_TRASH_LIST((*record)->labels, nitf_LabelSegment)
            nitf_List_destruct(&(*record)->labels);
        }

        if ((*record)->texts)
        {
            /* Walk the list and destroy it  */
            _NITF_TRASH_LIST((*record)->texts, nitf_TextSegment)
            nitf_List_destruct(&(*record)->texts);
        }

        if ((*record)->dataExtensions)
        {
            /* Walk the list and destroy it  */
            _NITF_TRASH_LIST((*record)->dataExtensions, nitf_DESegment)
            nitf_List_destruct(&(*record)->dataExtensions);
        }

        if ((*record)->reservedExtensions)
        {
            /* Walk the list and destroy it  */
            _NITF_TRASH_LIST((*record)->reservedExtensions, nitf_RESegment)
            nitf_List_destruct(&(*record)->reservedExtensions);
        }

        NITF_FREE(*record);
        *record = NULL;
    }
}

NITFAPI(nitf_Version) nitf_Record_getVersion(const nitf_Record* record)
{
    char version[6];
    nitf_Version fver = NITF_VER_UNKNOWN;

    /* Make sure the pieces of what we want exist */
    if (record && record->header && record->header->NITF_FVER)
    {
        memcpy(version, record->header->NITF_FVER->raw, 5);
        version[5] = 0;

        /* NSIF1.0 == NITF2.1 (from our point of view anyway) */
        if (strncmp(record->header->NITF_FHDR->raw, "NSIF", 4) == 0 ||
            strncmp(version, "02.10", 5) == 0)
            fver = NITF_VER_21;
        else if (strncmp(version, "02.00", 5) == 0)
            fver = NITF_VER_20;
    }
    return fver;
}

static nitf_ComponentInfo** nitf_malloc_ComponentInfo(uint32_t num, nitf_Error* error)
{
    assert(error != NULL);
    const size_t num_ = ((size_t)num) + 1;
    nitf_ComponentInfo** infoArray = (nitf_ComponentInfo**)NITF_MALLOC(sizeof(nitf_ComponentInfo*) * num_);
    if (!infoArray)
    {
        nitf_Error_init(error,
            NITF_STRERROR(NITF_ERRNO),
            NITF_CTXT,
            NITF_ERR_MEMORY);
    }
    return infoArray;
}


NITFAPI(nitf_ImageSegment*)
nitf_Record_newImageSegment(nitf_Record* record, nitf_Error* error)
{
    nitf_ImageSegment* segment = NULL;
    nitf_ComponentInfo* info = NULL;
    nitf_ComponentInfo** infoArray = NULL;
    uint32_t num;
    uint32_t i;
    nitf_Version version;

    /* Get current num of images */
    NITF_TRY_GET_UINT32(record->header->NITF_NUMI, &num, error);

    /* Verify the number is ok */
    if (((int32_t)num) < 0)
        num = 0;
    else if (num >= 999)
    {
        nitf_Error_initf(error,
                         NITF_CTXT,
                         NITF_ERR_INVALID_OBJECT,
                         "Cannot add another image segment, already have %d",
                         num);
        goto CATCH_ERROR;
    }

    /* Create the new info */
    info = nitf_ComponentInfo_construct(NITF_LISH_SZ, NITF_LI_SZ, error);
    if (!info)
        goto CATCH_ERROR;

    version = nitf_Record_getVersion(record);
    segment = createDefaultImageSegment(version, num + 1, error);
    if (!segment)
        goto CATCH_ERROR;

    /* Add to the list */
    if (!nitf_List_pushBack(record->images, (NITF_DATA*)segment, error))
        goto CATCH_ERROR;

    /* Make new array, one bigger */
    infoArray = nitf_malloc_ComponentInfo(num, error);
    if (!infoArray)
    {
        goto CATCH_ERROR;
    }

    /* Iterate over current infos */
    for (i = 0; i < num; ++i)
        infoArray[i] = record->header->imageInfo[i];

    /* Add the new one */
    infoArray[i] = info;
    num++;

    if (!nitf_Field_setUint32(record->header->NITF_NUMI, num, error))
        goto CATCH_ERROR;

    /* Delete old one, if there, and set to new one */
    if (record->header->imageInfo)
        NITF_FREE(record->header->imageInfo);

    record->header->imageInfo = infoArray;

    /* Return the segment */
    return segment;

CATCH_ERROR:
    if (info)
        nitf_ComponentInfo_destruct(&info);

    if (infoArray)
        NITF_FREE(infoArray);

    if (segment)
        nitf_ImageSegment_destruct(&segment);

    return NULL;
}

NITFAPI(nitf_GraphicSegment*)
nitf_Record_newGraphicSegment(nitf_Record* record, nitf_Error* error)
{
    nitf_GraphicSegment* segment = NULL;
    nitf_GraphicSubheader* header = NULL;
    nitf_ComponentInfo* info = NULL;
    nitf_ComponentInfo** infoArray = NULL;
    uint32_t num;
    uint32_t i;
    nitf_Version version;

    /* Get current num of graphics */
    NITF_TRY_GET_UINT32(record->header->NITF_NUMS, &num, error);

    /* Verify the number is ok */
    if (((int32_t)num) < 0)
        num = 0;
    else if (num >= 999)
    {
        nitf_Error_initf(error,
                         NITF_CTXT,
                         NITF_ERR_INVALID_OBJECT,
                         "Cannot add another graphic segment, already have %d",
                         num);
        goto CATCH_ERROR;
    }

    /* Create the new info */
    info = nitf_ComponentInfo_construct(NITF_LSSH_SZ, NITF_LS_SZ, error);
    if (!info)
        goto CATCH_ERROR;

    /* Create the new segment */
    segment = nitf_GraphicSegment_construct(error);
    if (!segment)
        goto CATCH_ERROR;

    header = segment->subheader;

    _NITF_SET_FIELD(NITF_SY, "SY");
    _NITF_SET_FIELD(NITF_ENCRYP, "0");
    _NITF_SET_FIELD(NITF_SSCLAS, "U");

    if (!nitf_Field_setUint32(header->NITF_SDLVL, num + 1, error))
        goto CATCH_ERROR;

    /* Update the version of the FileSecurity, if necessary */
    version = nitf_Record_getVersion(record);
    if (version == NITF_VER_20)
    {
        /* Resize */
        if (!nitf_FileSecurity_resizeForVersion(header->securityGroup,
                                                version,
                                                error))
            goto CATCH_ERROR;
    }

    /* Add to the list */
    if (!nitf_List_pushBack(record->graphics, (NITF_DATA*)segment, error))
        goto CATCH_ERROR;

    /* Make new array, one bigger */
    infoArray = nitf_malloc_ComponentInfo(num, error);
    if (!infoArray)
    {
        goto CATCH_ERROR;
    }

    /* Iterate over current infos */
    for (i = 0; i < num; ++i)
        infoArray[i] = record->header->graphicInfo[i];

    /* Add the new one */
    infoArray[i] = info;
    num++;

    /* Convert the data to chars */
    if (!nitf_Field_setUint32(record->header->NITF_NUMS, num, error))
        goto CATCH_ERROR;

    /* Delete old one, if there, and set to new one */
    if (record->header->graphicInfo)
        NITF_FREE(record->header->graphicInfo);

    record->header->graphicInfo = infoArray;

    /* Return the segment */
    return segment;

CATCH_ERROR:

    if (info)
        nitf_ComponentInfo_destruct(&info);

    if (infoArray)
        NITF_FREE(infoArray);

    if (segment)
        nitf_GraphicSegment_destruct(&segment);

    return NULL;
}

NITFAPI(nitf_TextSegment*)
nitf_Record_newTextSegment(nitf_Record* record, nitf_Error* error)
{
    nitf_TextSegment* segment = NULL;
    nitf_TextSubheader* header = NULL;
    nitf_ComponentInfo* info = NULL;
    nitf_ComponentInfo** infoArray = NULL;
    uint32_t num;
    uint32_t i;
    nitf_Version version;

    /* Get current num of texts */
    NITF_TRY_GET_UINT32(record->header->numTexts, &num, error);

    /* Verify the number is ok */
    if (((int32_t)num) < 0)
        num = 0;
    else if (num >= 999)
    {
        nitf_Error_initf(error,
                         NITF_CTXT,
                         NITF_ERR_INVALID_OBJECT,
                         "Cannot add another text segment, already have %d",
                         num);
        goto CATCH_ERROR;
    }

    /* Create the new info */
    info = nitf_ComponentInfo_construct(NITF_LTSH_SZ, NITF_LT_SZ, error);
    if (!info)
    {
        goto CATCH_ERROR;
    }

    /* Create the new segment */
    segment = nitf_TextSegment_construct(error);
    if (!segment)
        goto CATCH_ERROR;

    header = segment->subheader;

    _NITF_SET_FIELD(NITF_TE, "TE");
    _NITF_SET_FIELD(NITF_ENCRYP, "0");
    _NITF_SET_FIELD(NITF_SSCLAS, "U");

    /* Update the version of the FileSecurity, if necessary */
    version = nitf_Record_getVersion(record);
    if (version == NITF_VER_20)
    {
        /* Resize */
        nitf_FileSecurity_resizeForVersion(header->securityGroup,
                                           version,
                                           error);
    }

    /* Add to the list */
    if (!nitf_List_pushBack(record->texts, (NITF_DATA*)segment, error))
    {
        goto CATCH_ERROR;
    }

    /* Make new array, one bigger */
    infoArray = nitf_malloc_ComponentInfo(num, error);
    if (!infoArray)
    {
        goto CATCH_ERROR;
    }

    /* Iterate over current infos */
    for (i = 0; i < num; ++i)
    {
        infoArray[i] = record->header->textInfo[i];
    }
    /* Add the new one */
    infoArray[i] = info;
    num++;

    if (!nitf_Field_setUint32(record->header->NITF_NUMT, num, error))
        goto CATCH_ERROR;

    /* Delete old one, if there, and set to new one */
    if (record->header->textInfo)
        NITF_FREE(record->header->textInfo);

    record->header->textInfo = infoArray;

    /* Return the segment */
    return segment;

CATCH_ERROR:

    if (info)
        nitf_ComponentInfo_destruct(&info);

    if (infoArray)
        NITF_FREE(infoArray);

    if (segment)
        nitf_TextSegment_destruct(&segment);

    return NULL;
}

NITFAPI(nitf_DESegment*)
nitf_Record_newDataExtensionSegment(nitf_Record* record, nitf_Error* error)
{
    nitf_DESegment* segment = NULL;
    nitf_DESubheader* header = NULL;
    nitf_ComponentInfo* info = NULL;
    nitf_ComponentInfo** infoArray = NULL;
    uint32_t num;
    uint32_t i;
    nitf_Version version;

    /* Get current num of DEs */
    NITF_TRY_GET_UINT32(record->header->NITF_NUMDES, &num, error);

    /* Verify the number is ok */
    if (((int32_t)num) < 0)
        num = 0;

    else if (num >= 999)
    {
        nitf_Error_initf(error,
                         NITF_CTXT,
                         NITF_ERR_INVALID_OBJECT,
                         "Cannot add another DE segment, already have %d",
                         num);
        goto CATCH_ERROR;
    }

    /* Create the new info */
    info = nitf_ComponentInfo_construct(NITF_LDSH_SZ, NITF_LD_SZ, error);
    if (!info)
        goto CATCH_ERROR;

    /* Create the new segment */
    segment = nitf_DESegment_construct(error);
    if (!segment)
        goto CATCH_ERROR;

    header = segment->subheader;

    _NITF_SET_FIELD(NITF_DE, "DE");
    _NITF_SET_FIELD(NITF_DESCLAS, "U");

    /* Update the version of the FileSecurity, if necessary */
    version = nitf_Record_getVersion(record);
    if (version == NITF_VER_20)
    {
        /* Resize */
        if (!nitf_FileSecurity_resizeForVersion(header->securityGroup,
                                                version,
                                                error))
            goto CATCH_ERROR;
    }

    /* Add to the list */
    if (!nitf_List_pushBack(record->dataExtensions, (NITF_DATA*)segment, error))
        goto CATCH_ERROR;

    /* Make new array, one bigger */
    infoArray = nitf_malloc_ComponentInfo(num, error);
    if (!infoArray)
    {
        goto CATCH_ERROR;
    }

    /* Iterate over current infos */
    for (i = 0; i < num; ++i)
    {
        infoArray[i] = record->header->dataExtensionInfo[i];
    }
    /* Add the new one */
    infoArray[i] = info;
    num++;

    /* Convert the data to chars */
    if (!nitf_Field_setUint32(record->header->NITF_NUMDES, num, error))
        goto CATCH_ERROR;

    /* Delete old one, if there, and set to new one */
    if (record->header->dataExtensionInfo)
    {
        NITF_FREE(record->header->dataExtensionInfo);
    }
    record->header->dataExtensionInfo = infoArray;

    /* Return the segment */
    return segment;

CATCH_ERROR:

    if (info)
        nitf_ComponentInfo_destruct(&info);

    if (infoArray)
        NITF_FREE(infoArray);

    if (segment)
        nitf_DESegment_destruct(&segment);

    return NULL;
}

NITFAPI(NITF_BOOL)
nitf_Record_removeImageSegment(nitf_Record* record,
                               uint32_t segmentNumber,
                               nitf_Error* error)
{
    uint32_t num;
    nitf_ComponentInfo** infoArray = NULL;
    nitf_ImageSegment* segment = NULL;
    uint32_t i;
    nitf_ListIterator iter = nitf_List_at(record->images, segmentNumber);

    if (iter.current == NULL)
    {
        nitf_Error_initf(error,
                         NITF_CTXT,
                         NITF_ERR_INVALID_OBJECT,
                         "Invalid image segment number");
        goto CATCH_ERROR;
    }

    /* Remove from the list */
    segment = (nitf_ImageSegment*)nitf_List_remove(record->images, &iter);
    /* Destroy it */
    nitf_ImageSegment_destruct(&segment);

    /* Get current num*/
    NITF_TRY_GET_UINT32(record->header->numImages, &num, error);

    /* If we actually need to resize */
    if (num > 1)
    {
        /* Make new array, one smaller */
        infoArray = (nitf_ComponentInfo**)NITF_MALLOC(
                sizeof(nitf_ComponentInfo*) * (num - 1));
        if (!infoArray)
        {
            nitf_Error_init(error, NITF_STRERROR(NITF_ERRNO), NITF_CTXT, NITF_ERR_MEMORY);
            goto CATCH_ERROR;
        }

        /* Iterate over current infos */
        for (i = 0; i < segmentNumber; ++i)
        {
            infoArray[i] = record->header->imageInfo[i];
        }
        for (i = segmentNumber + 1; i < num; ++i)
        {
            infoArray[i - 1] = record->header->imageInfo[i];
        }
    }

    /* Update the num field in the header */
    num--;

    if (!nitf_Field_setUint32(record->header->NITF_NUMI, num, error))
        goto CATCH_ERROR;

    /* Delete old one, if there, and set to new one */
    if (record->header->imageInfo)
        NITF_FREE(record->header->imageInfo);

    record->header->imageInfo = infoArray;

    /* Update the indexes in any overflow segments */
    if (!fixOverflowIndexes(record, "UDID", segmentNumber, error))
        return NITF_FAILURE;

    if (!fixOverflowIndexes(record, "IXSHD", segmentNumber, error))
        return NITF_FAILURE;

    return NITF_SUCCESS;

CATCH_ERROR:
    if (infoArray)
        NITF_FREE(infoArray);

    return NITF_FAILURE;
}

NITFAPI(NITF_BOOL)
nitf_Record_removeGraphicSegment(nitf_Record* record,
                                 uint32_t segmentNumber,
                                 nitf_Error* error)
{
    uint32_t num;
    nitf_ComponentInfo** infoArray = NULL;
    nitf_GraphicSegment* segment = NULL;
    uint32_t i;
    nitf_ListIterator iter = nitf_List_at(record->graphics, segmentNumber);

    if (iter.current == NULL)
    {
        nitf_Error_initf(error,
                         NITF_CTXT,
                         NITF_ERR_INVALID_OBJECT,
                         "Invalid graphic segment number");
        goto CATCH_ERROR;
    }

    /* Remove from the list */
    segment = (nitf_GraphicSegment*)nitf_List_remove(record->graphics, &iter);
    /* Destroy it */
    nitf_GraphicSegment_destruct(&segment);

    /* Get current num */
    NITF_TRY_GET_UINT32(record->header->numGraphics, &num, error);

    /* If we actually need to resize */
    if (num > 1)
    {
        /* Make new array, one smaller */
        infoArray = (nitf_ComponentInfo**)NITF_MALLOC(
                sizeof(nitf_ComponentInfo*) * (num - 1));
        if (!infoArray)
        {
            nitf_Error_init(error, NITF_STRERROR(NITF_ERRNO), NITF_CTXT, NITF_ERR_MEMORY);
            goto CATCH_ERROR;
        }

        /* Iterate over current infos */
        for (i = 0; i < segmentNumber; ++i)
        {
            infoArray[i] = record->header->graphicInfo[i];
        }
        for (i = segmentNumber + 1; i < num; ++i)
        {
            infoArray[i - 1] = record->header->graphicInfo[i];
        }
    }

    /* Update the num field in the header */
    num--;

    if (!nitf_Field_setUint32(record->header->NITF_NUMS, num, error))
        goto CATCH_ERROR;

    /* Delete old one, if there, and set to new one */
    if (record->header->graphicInfo)
        NITF_FREE(record->header->graphicInfo);

    record->header->graphicInfo = infoArray;

    /* Update the indexes in any overflow segments */
    if (!fixOverflowIndexes(record, "SXSHD", segmentNumber, error))
        return NITF_FAILURE;

    return NITF_SUCCESS;

CATCH_ERROR:
    if (infoArray)
        NITF_FREE(infoArray);

    return NITF_FAILURE;
}

NITFAPI(NITF_BOOL)
nitf_Record_removeLabelSegment(nitf_Record* record,
                               uint32_t segmentNumber,
                               nitf_Error* error)
{
    uint32_t num;
    nitf_ComponentInfo** infoArray = NULL;
    nitf_LabelSegment* segment = NULL;
    uint32_t i;

    nitf_ListIterator iter = nitf_List_at(record->labels, segmentNumber);

    if (iter.current == NULL)
    {
        nitf_Error_initf(error,
                         NITF_CTXT,
                         NITF_ERR_INVALID_OBJECT,
                         "Invalid label segment number");
        goto CATCH_ERROR;
    }

    /* Remove from the list */
    segment = (nitf_LabelSegment*)nitf_List_remove(record->labels, &iter);

    /* Destroy it */
    nitf_LabelSegment_destruct(&segment);

    /* Get current num */
    NITF_TRY_GET_UINT32(record->header->numLabels, &num, error);

    /* If we actually need to resize */
    if (num > 1)
    {
        /* Make new array, one smaller */
        infoArray = (nitf_ComponentInfo**)NITF_MALLOC(
                sizeof(nitf_ComponentInfo*) * (num - 1));
        if (!infoArray)
        {
            nitf_Error_init(error, NITF_STRERROR(NITF_ERRNO), NITF_CTXT, NITF_ERR_MEMORY);
            goto CATCH_ERROR;
        }
        /* Iterate over current infos */
        for (i = 0; i < segmentNumber; ++i)
        {
            infoArray[i] = record->header->labelInfo[i];
        }
        for (i = segmentNumber + 1; i < num; ++i)
        {
            infoArray[i - 1] = record->header->labelInfo[i];
        }
    }

    /* Update the num field in the header */
    num--;

    if (!nitf_Field_setUint32(record->header->NITF_NUMX, num, error))
        goto CATCH_ERROR;

    /* Delete old one, if there, and set to new one */
    if (record->header->labelInfo)
        NITF_FREE(record->header->labelInfo);

    record->header->labelInfo = infoArray;

    /* Update the indexes in any overflow segments */
    if (!fixOverflowIndexes(record, "LXSHD", segmentNumber, error))
        return NITF_FAILURE;

    return NITF_SUCCESS;

CATCH_ERROR:
    if (infoArray)
        NITF_FREE(infoArray);

    return NITF_FAILURE;
}

NITFAPI(NITF_BOOL)
nitf_Record_removeTextSegment(nitf_Record* record,
                              uint32_t segmentNumber,
                              nitf_Error* error)
{
    uint32_t num;
    nitf_ComponentInfo** infoArray = NULL;
    nitf_TextSegment* segment = NULL;
    uint32_t i;
    nitf_ListIterator iter = nitf_List_at(record->texts, segmentNumber);

    if (iter.current == NULL)
    {
        nitf_Error_initf(error,
                         NITF_CTXT,
                         NITF_ERR_INVALID_OBJECT,
                         "Invalid text segment number");
        goto CATCH_ERROR;
    }

    /* Remove from the list */
    segment = (nitf_TextSegment*)nitf_List_remove(record->texts, &iter);
    /* Destroy it */
    nitf_TextSegment_destruct(&segment);

    /* Get current num */
    NITF_TRY_GET_UINT32(record->header->NITF_NUMT, &num, error);

    /* If we actually need to resize */
    if (num > 1)
    {
        /* Make new array, one smaller */
        infoArray = (nitf_ComponentInfo**)NITF_MALLOC(
                sizeof(nitf_ComponentInfo*) * (num - 1));
        if (!infoArray)
        {
            nitf_Error_init(error, NITF_STRERROR(NITF_ERRNO), NITF_CTXT, NITF_ERR_MEMORY);
            goto CATCH_ERROR;
        }
        /* Iterate over current infos */
        for (i = 0; i < segmentNumber; ++i)
            infoArray[i] = record->header->textInfo[i];
        for (i = segmentNumber + 1; i < num; ++i)
            infoArray[i - 1] = record->header->textInfo[i];
    }

    /* Update the num field in the header */
    num--;

    if (!nitf_Field_setUint32(record->header->NITF_NUMT, num, error))
        goto CATCH_ERROR;
    /* Delete old one, if there, and set to new one */
    if (record->header->textInfo)
        NITF_FREE(record->header->textInfo);

    record->header->textInfo = infoArray;

    /* Update the indexes in any overflow segments */
    if (!fixOverflowIndexes(record, "TXSHD", segmentNumber, error))
        return NITF_FAILURE;

    return NITF_SUCCESS;

CATCH_ERROR:

    if (infoArray)
        NITF_FREE(infoArray);

    return NITF_FAILURE;
}

NITFAPI(NITF_BOOL)
nitf_Record_removeDataExtensionSegment(nitf_Record* record,
                                       uint32_t segmentNumber,
                                       nitf_Error* error)
{
    uint32_t num;
    nitf_ComponentInfo** infoArray = NULL;
    nitf_DESegment* segment = NULL;
    uint32_t i;
    nitf_ListIterator iter =
            nitf_List_at(record->dataExtensions, segmentNumber);

    if (iter.current == NULL)
    {
        nitf_Error_initf(error,
                         NITF_CTXT,
                         NITF_ERR_INVALID_OBJECT,
                         "Invalid dataExtension segment number");
        goto CATCH_ERROR;
    }

    /* Remove from the list */
    segment = (nitf_DESegment*)nitf_List_remove(record->dataExtensions, &iter);

    /* Destroy it */
    nitf_DESegment_destruct(&segment);

    /* Get current num */
    NITF_TRY_GET_UINT32(record->header->NITF_NUMDES, &num, error);

    /* If we actually need to resize */
    if (num > 1)
    {
        /* Make new array, one smaller */
        infoArray = (nitf_ComponentInfo**)NITF_MALLOC(
                sizeof(nitf_ComponentInfo*) * (num - 1));
        if (!infoArray)
        {
            nitf_Error_init(error, NITF_STRERROR(NITF_ERRNO), NITF_CTXT, NITF_ERR_MEMORY);
            goto CATCH_ERROR;
        }
        /* Iterate over current infos */
        for (i = 0; i < segmentNumber; ++i)
        {
            infoArray[i] = record->header->dataExtensionInfo[i];
        }
        for (i = segmentNumber + 1; i < num; ++i)
        {
            infoArray[i - 1] = record->header->dataExtensionInfo[i];
        }
    }

    /* Destruct removed nitf_ComponentInfo */
    nitf_ComponentInfo_destruct(
            &record->header->dataExtensionInfo[segmentNumber]);

    /* Update the num field in the header */
    num--;

    if (!nitf_Field_setUint32(record->header->NITF_NUMDES, num, error))
        goto CATCH_ERROR;

    /* Delete old one, if there, and set to new one */
    if (record->header->dataExtensionInfo)
    {
        NITF_FREE(record->header->dataExtensionInfo);
    }
    record->header->dataExtensionInfo = infoArray;

    /* Fix indexes in other segments with overflows */
    if (!fixSegmentIndexes(record, segmentNumber, error))
        return NITF_FAILURE;

    return NITF_SUCCESS;

CATCH_ERROR:
    return NITF_FAILURE;
}

NITFAPI(NITF_BOOL)
nitf_Record_removeReservedExtensionSegment(nitf_Record* record,
                                           uint32_t segmentNumber,
                                           nitf_Error* error)
{
    uint32_t num;
    nitf_ComponentInfo** infoArray = NULL;
    nitf_RESegment* segment = NULL;
    uint32_t i;

    nitf_ListIterator iter =
            nitf_List_at(record->reservedExtensions, segmentNumber);

    if (iter.current == NULL)
    {
        nitf_Error_initf(error,
                         NITF_CTXT,
                         NITF_ERR_INVALID_OBJECT,
                         "Invalid reservedExtension segment number");
        goto CATCH_ERROR;
    }

    /* Remove from the list */
    segment = (nitf_RESegment*)nitf_List_remove(record->reservedExtensions,
                                                &iter);

    /* Destroy it */
    nitf_RESegment_destruct(&segment);

    /* Get current num */
    NITF_TRY_GET_UINT32(record->header->NITF_NUMRES, &num, error);

    /* If we actually need to resize */
    if (num > 1)
    {
        /* Make new array, one smaller */
        infoArray = (nitf_ComponentInfo**)NITF_MALLOC(
                sizeof(nitf_ComponentInfo*) * (num - 1));
        if (!infoArray)
        {
            nitf_Error_init(error, NITF_STRERROR(NITF_ERRNO), NITF_CTXT, NITF_ERR_MEMORY);
            goto CATCH_ERROR;
        }
        /* Iterate over current infos */
        for (i = 0; i < segmentNumber; ++i)
        {
            infoArray[i] = record->header->reservedExtensionInfo[i];
        }
        for (i = segmentNumber + 1; i < num; ++i)
        {
            infoArray[i - 1] = record->header->reservedExtensionInfo[i];
        }
    }

    /* Update the num field in the header */
    num--;

    if (!nitf_Field_setUint32(record->header->NITF_NUMRES, num, error))
        goto CATCH_ERROR;

    /* Delete old one, if there, and set to new one */
    if (record->header->reservedExtensionInfo)
    {
        NITF_FREE(record->header->reservedExtensionInfo);
    }
    record->header->reservedExtensionInfo = infoArray;

    return NITF_SUCCESS;

CATCH_ERROR:

    if (infoArray)
    {
        NITF_FREE(infoArray);
    }

    return NITF_FAILURE;
}

NITFAPI(NITF_BOOL)
nitf_Record_moveImageSegment(nitf_Record* record,
                             uint32_t oldIndex,
                             uint32_t newIndex,
                             nitf_Error* error)
{
    uint32_t num;
    nitf_ComponentInfo* tempInfo = NULL;

    NITF_TRY_GET_UINT32(record->header->numImages, &num, error);

    if (oldIndex >= num || newIndex >= num)
    {
        nitf_Error_initf(error,
                         NITF_CTXT,
                         NITF_ERR_INVALID_OBJECT,
                         "Invalid index provided");
        goto CATCH_ERROR;
    }

    /* Just return OK if no change */
    if (oldIndex == newIndex)
        return NITF_SUCCESS;

    /* Do the list move */
    if (nitf_List_move(record->images, oldIndex, newIndex, error))
        goto CATCH_ERROR;

    /* Now, need to move the component info - just move pointers */
    tempInfo = record->header->imageInfo[oldIndex];
    record->header->imageInfo[oldIndex] = record->header->imageInfo[newIndex];
    record->header->imageInfo[newIndex] = tempInfo;

    return NITF_SUCCESS;

CATCH_ERROR:
    return NITF_FAILURE;
}

NITFAPI(NITF_BOOL)
nitf_Record_moveGraphicSegment(nitf_Record* record,
                               uint32_t oldIndex,
                               uint32_t newIndex,
                               nitf_Error* error)
{
    uint32_t num;
    nitf_ComponentInfo* tempInfo = NULL;

    NITF_TRY_GET_UINT32(record->header->numGraphics, &num, error);

    if (oldIndex >= num || newIndex >= num)
    {
        nitf_Error_initf(error,
                         NITF_CTXT,
                         NITF_ERR_INVALID_OBJECT,
                         "Invalid index provided");
        goto CATCH_ERROR;
    }

    /* Just return OK if no change */
    if (oldIndex == newIndex)
        return NITF_SUCCESS;

    /* Do the list move */
    if (nitf_List_move(record->graphics, oldIndex, newIndex, error))
        goto CATCH_ERROR;

    /* Now, need to move the component info - just move pointers */
    tempInfo = record->header->graphicInfo[oldIndex];

    record->header->graphicInfo[oldIndex] =
            record->header->graphicInfo[newIndex];

    record->header->graphicInfo[newIndex] = tempInfo;

    return NITF_SUCCESS;

CATCH_ERROR:
    return NITF_FAILURE;
}

NITFAPI(NITF_BOOL)
nitf_Record_moveLabelSegment(nitf_Record* record,
                             uint32_t oldIndex,
                             uint32_t newIndex,
                             nitf_Error* error)
{
    uint32_t num;
    nitf_ComponentInfo* tempInfo = NULL;

    NITF_TRY_GET_UINT32(record->header->numLabels, &num, error);

    if (oldIndex >= num || newIndex >= num)
    {
        nitf_Error_initf(error,
                         NITF_CTXT,
                         NITF_ERR_INVALID_OBJECT,
                         "Invalid index provided");
        goto CATCH_ERROR;
    }

    /* Just return OK if no change */
    if (oldIndex == newIndex)
        return NITF_SUCCESS;

    /* Do the list move */
    if (nitf_List_move(record->labels, oldIndex, newIndex, error))
        goto CATCH_ERROR;

    /* Now, need to move the component info - just move pointers */
    tempInfo = record->header->labelInfo[oldIndex];
    record->header->labelInfo[oldIndex] = record->header->labelInfo[newIndex];
    record->header->labelInfo[newIndex] = tempInfo;

    return NITF_SUCCESS;

CATCH_ERROR:
    return NITF_FAILURE;
}

NITFAPI(NITF_BOOL)
nitf_Record_moveTextSegment(nitf_Record* record,
                            uint32_t oldIndex,
                            uint32_t newIndex,
                            nitf_Error* error)
{
    uint32_t num;
    nitf_ComponentInfo* tempInfo = NULL;

    NITF_TRY_GET_UINT32(record->header->numTexts, &num, error);

    if (oldIndex >= num || newIndex >= num)
    {
        nitf_Error_initf(error,
                         NITF_CTXT,
                         NITF_ERR_INVALID_OBJECT,
                         "Invalid index provided");
        goto CATCH_ERROR;
    }

    /* Just return OK if no change */
    if (oldIndex == newIndex)
        return NITF_SUCCESS;

    /* Do the list move */
    if (nitf_List_move(record->texts, oldIndex, newIndex, error))
        goto CATCH_ERROR;

    /* Now, need to move the component info - just move pointers */
    tempInfo = record->header->textInfo[oldIndex];
    record->header->textInfo[oldIndex] = record->header->textInfo[newIndex];
    record->header->textInfo[newIndex] = tempInfo;

    return NITF_SUCCESS;

CATCH_ERROR:
    return NITF_FAILURE;
}

NITFAPI(NITF_BOOL)
nitf_Record_moveDataExtensionSegment(nitf_Record* record,
                                     uint32_t oldIndex,
                                     uint32_t newIndex,
                                     nitf_Error* error)
{
    uint32_t num;
    nitf_ComponentInfo* tempInfo = NULL;

    NITF_TRY_GET_UINT32(record->header->numDataExtensions, &num, error);

    if (oldIndex >= num || newIndex >= num)
    {
        nitf_Error_initf(error,
                         NITF_CTXT,
                         NITF_ERR_INVALID_OBJECT,
                         "Invalid index provided");
        goto CATCH_ERROR;
    }

    /* Just return OK if no change */
    if (oldIndex == newIndex)
        return NITF_SUCCESS;

    /* Do the list move */
    if (nitf_List_move(record->dataExtensions, oldIndex, newIndex, error))
        goto CATCH_ERROR;

    /* Now, need to move the component info - just move pointers */
    tempInfo = record->header->dataExtensionInfo[oldIndex];
    record->header->dataExtensionInfo[oldIndex] =
            record->header->dataExtensionInfo[newIndex];
    record->header->dataExtensionInfo[newIndex] = tempInfo;

    return NITF_SUCCESS;

CATCH_ERROR:
    return NITF_FAILURE;
}

NITFAPI(NITF_BOOL)
nitf_Record_moveReservedExtensionSegment(nitf_Record* record,
                                         uint32_t oldIndex,
                                         uint32_t newIndex,
                                         nitf_Error* error)
{
    uint32_t num;
    nitf_ComponentInfo* tempInfo = NULL;

    NITF_TRY_GET_UINT32(record->header->numReservedExtensions, &num, error);

    if (oldIndex >= num || newIndex >= num)
    {
        nitf_Error_initf(error,
                         NITF_CTXT,
                         NITF_ERR_INVALID_OBJECT,
                         "Invalid index provided");
        goto CATCH_ERROR;
    }

    /* Just return OK if no change */
    if (oldIndex == newIndex)
        return NITF_SUCCESS;

    /* Do the list move */
    if (nitf_List_move(record->reservedExtensions, oldIndex, newIndex, error))
        goto CATCH_ERROR;

    /* Now, need to move the component info - just move pointers */
    tempInfo = record->header->reservedExtensionInfo[oldIndex];

    record->header->reservedExtensionInfo[oldIndex] =
            record->header->reservedExtensionInfo[newIndex];

    record->header->reservedExtensionInfo[newIndex] = tempInfo;

    return NITF_SUCCESS;

CATCH_ERROR:
    return NITF_FAILURE;
}

/*
 * Ugly macro that is most of the merge logic for a single extension section
 *
 * This macro is very localized and assumes several local variables exist and
 * are initialized.
 *
 * section      - Extended section (i.e., userDefinedSection)
 * securityCls - security class field (i.e., imageSecurityClass)
 * securityGrp - Security group object (i.e., securityGroup)
 * idx         - Index field (DE index in original segment) (i.e.,UDOFL)
 * segmentType     - Type string (i.e.,UDID)
 */
NITFPRIV(NITF_BOOL) unmergeSegment(nitf_Version version, nitf_Record* record,
    nitf_Extensions* section, nitf_Field* securityCls, nitf_FileSecurity* securityGrp, nitf_Field* idx, char* segmentType,
    uint32_t maxLength, uint32_t segIndex, nitf_Error* error)
{
    assert(record != NULL);
    assert(section != NULL);

    /* Overflow segment */
    nitf_DESegment* overflow = NULL;

    /* Length of TREs in current section */
    const uint32_t length = nitf_Extensions_computeLength(section,version,error);
    if (length > maxLength) 
    { 
        /* Overflow index of current extension */
        uint32_t overflowIndex;
        if (!nitf_Field_get(idx, &overflowIndex, 
                           NITF_CONV_INT,NITF_INT32_SZ, error)) 
        { 
            nitf_Error_init(error, "Could not retrieve overflow segment index", 
                            NITF_CTXT, NITF_ERR_INVALID_OBJECT); 
            return NITF_FAILURE; 
        } 
        if (overflowIndex == 0) 
        { 
            overflowIndex = addOverflowSegment(record, segIndex, segmentType,
                                               securityCls, securityGrp, &overflow, error); 
            if (overflowIndex == 0) 
            { 
                nitf_Error_init(error, "Could not add overflow segment",
                                NITF_CTXT, NITF_ERR_INVALID_OBJECT);
                return NITF_FAILURE; 
            }

            if (!nitf_Field_setUint32(idx, overflowIndex, error))
            {
                nitf_Error_init(error, "Could not set overflow segment index",
                                NITF_CTXT, NITF_ERR_INVALID_OBJECT);
                return NITF_FAILURE;
            }
        }
        else /* already tested for 0 above, wrap-around from -1 (below) isn't possible */
        {
            assert(overflowIndex > 0);
            nitf_ListIterator iter = nitf_List_at(record->dataExtensions, overflowIndex - 1);
            const nitf_ListIterator end = nitf_List_end(record->dataExtensions);
            if (nitf_ListIterator_notEqualTo(&iter, &end))
            {
                overflow = (nitf_DESegment*)nitf_ListIterator_get(&iter);
            }
        }

        if (overflow == NULL)
        {
            nitf_Error_init(error, "Invalid dataExtension segment number",
                             NITF_CTXT, NITF_ERR_INVALID_OBJECT);
            return NITF_FAILURE;
        }

        nitf_DESubheader* subheader = overflow->subheader;
        if (subheader == NULL)
        {
            nitf_Error_init(error, "Invalid dataExtension segment number (overflow->subheader)",
                NITF_CTXT, NITF_ERR_INVALID_OBJECT);
            return NITF_FAILURE;
        }
        nitf_Extensions* userDefinedSection = subheader->userDefinedSection;
        if (userDefinedSection == NULL)
        {
            nitf_Error_init(error, "Invalid dataExtension segment number (subheader->userDefinedSection)",
                NITF_CTXT, NITF_ERR_INVALID_OBJECT);
            return NITF_FAILURE;
        }
        if(!moveTREs(section, userDefinedSection, maxLength, error))
        { 
            nitf_Error_init(error, "Could not transfer TREs to overflow segment", 
                            NITF_CTXT, NITF_ERR_INVALID_OBJECT); 
            return NITF_FAILURE; 
        } 
    }
    return NITF_SUCCESS;
}

NITFAPI(NITF_BOOL)
nitf_Record_unmergeTREs(nitf_Record* record, nitf_Error* error)
{
    /* check for NULL data */
    if (record == NULL)
    {
        nitf_Error_init(error, "NULL data",
            NITF_CTXT, NITF_ERR_INVALID_PARAMETER);
        return NITF_FAILURE;
    }

    /* NITF version */
    const nitf_Version version = nitf_Record_getVersion(record);

    /* Max length for this type of section */
    uint32_t maxLength = 99999;

    /* Current segment index */
    uint32_t segIndex = 1; /* ??? I moved this up so this would be initialized!! */

    /* File header */
    const nitf_FileHeader*  header = record->header;
    if (header != NULL)
    {
        unmergeSegment(version, record, header->userDefinedSection,
            header->classification, header->securityGroup,
            header->NITF_UDHOFL, "UDHD",
            maxLength, segIndex, error);

        unmergeSegment(version, record, header->extendedSection,
            header->classification, header->securityGroup,
            header->NITF_XHDLOFL, "XHD",
            maxLength, segIndex, error);
    }

    /* Image segments */
    nitf_ListIterator segIter = nitf_List_begin(record->images);
    nitf_ListIterator segEnd = nitf_List_end(record->images);
    while (nitf_ListIterator_notEqualTo(&segIter, &segEnd))
    {
        maxLength = 99999;
        
        /* Current subheader */
        nitf_ImageSubheader* subheader =
                (nitf_ImageSubheader*)((nitf_ImageSegment*)
                                               nitf_ListIterator_get(&segIter))
                        ->subheader;
        if (subheader != NULL)
        {
            /* User defined section */
            unmergeSegment(version, record, subheader->userDefinedSection,
                subheader->imageSecurityClass, subheader->securityGroup,
                subheader->NITF_UDOFL, "UDID",
                maxLength, segIndex, error);

            /* Extension section */
            unmergeSegment(version, record, subheader->extendedSection,
                subheader->imageSecurityClass, subheader->securityGroup,
                subheader->NITF_IXSOFL, "IXSHD",
                maxLength, segIndex, error);
        }

        segIndex += 1;
        nitf_ListIterator_increment(&segIter);
    }

    /* Graphics segments */
    segIter = nitf_List_begin(record->graphics);
    segEnd = nitf_List_end(record->graphics);
    segIndex = 1;

    while (nitf_ListIterator_notEqualTo(&segIter, &segEnd))
    {
        /* Hello, really?  Somebody needs to fix hardcode DP */
        maxLength = 9741;

        /* Current subheader */
        nitf_GraphicSubheader* subheader = (nitf_GraphicSubheader*)((nitf_GraphicSegment*)
                                                     nitf_ListIterator_get(
                                                             &segIter))
                            ->subheader;
        if (subheader != NULL)
        {
            /* Extension section */
            unmergeSegment(version, record, subheader->extendedSection,
                subheader->securityClass, subheader->securityGroup,
                subheader->NITF_SXSOFL, "SXSHD",
                maxLength, segIndex, error);
        }

        segIndex += 1;
        nitf_ListIterator_increment(&segIter);
    }

    /* Label segments */
    segIter = nitf_List_begin(record->labels);
    segEnd = nitf_List_end(record->labels);
    segIndex = 1;

    while (nitf_ListIterator_notEqualTo(&segIter, &segEnd))
    {
        /*  Fix hardcode! */
        maxLength = 9747;

        /* Current subheader */
        nitf_LabelSubheader* subheader =
                (nitf_LabelSubheader*)((nitf_LabelSegment*)
                                               nitf_ListIterator_get(&segIter))
                        ->subheader;
        if (subheader != NULL)
        {
            /* Extension section */
            unmergeSegment(version, record, subheader->extendedSection,
                subheader->securityClass, subheader->securityGroup,
                subheader->NITF_LXSOFL, "LXSHD",
                maxLength, segIndex, error);
        }

        segIndex += 1;
        nitf_ListIterator_increment(&segIter);
    }

    /* Text segments */
    segIter = nitf_List_begin(record->texts);
    segEnd = nitf_List_end(record->texts);
    segIndex = 1;

    while (nitf_ListIterator_notEqualTo(&segIter, &segEnd))
    {
        /* Fix hardcode! */
        maxLength = 9717;

        /* Current subheader */
        nitf_TextSubheader* subheader =
                (nitf_TextSubheader*)((nitf_TextSegment*)nitf_ListIterator_get(
                                              &segIter))
                        ->subheader;
        if (subheader != NULL)
        {
            /* Extension section */
            unmergeSegment(version, record, subheader->extendedSection,
                subheader->securityClass, subheader->securityGroup,
                subheader->NITF_TXSOFL, "TXSHD",
                maxLength, segIndex, error);
        }

        segIndex += 1;
        nitf_ListIterator_increment(&segIter);
    }

    return NITF_SUCCESS;
}

NITFAPI(NITF_BOOL) nitf_Record_mergeTREs(nitf_Record* record, nitf_Error* error)
{
    /* DE segment list */
    nitf_ListIterator deIter;

    /* DE segment list end */
    nitf_ListIterator deEnd;

    /* Current DE segment index (one based) */
    uint32_t deIndex;

    /* Number of DE segments removed */
    int32_t deRemoved;

    deIter = nitf_List_begin(record->dataExtensions);
    deEnd = nitf_List_end(record->dataExtensions);
    deIndex = 1;
    deRemoved = 0;

    while (nitf_ListIterator_notEqualTo(&deIter, &deEnd))
    {
        /* Current subheader */
        nitf_DESubheader* subheader;
        char desid[NITF_DESTAG_SZ + 1];

        /* Owner of overflow */
        nitf_Extensions* destination;

        /* Extended header length field */
        nitf_Field* extLength;

        /* Overflow length  field */
        nitf_Field* overflowIndex;

        subheader = (nitf_DESubheader*)((nitf_DESegment*)nitf_ListIterator_get(
                                                &deIter))
                            ->subheader;

        if (!nitf_Field_get(subheader->NITF_DESTAG,
                            (NITF_DATA*)desid,
                            NITF_CONV_STRING,
                            NITF_DESTAG_SZ + 1,
                            error))
        {
            nitf_Error_init(error, "Could not retrieve DE segment id",
                            NITF_CTXT,
                            NITF_ERR_INVALID_OBJECT);
            return NITF_FAILURE;
        }
        nitf_Field_trimString(desid);

        /* This is an overflow */
        if (strcmp(desid, TRE_OVERFLOW_STR) == 0)
        {
            uint32_t segIndex;
            char type[NITF_DESOFLW_SZ + 1];
            NITF_BOOL eflag;

            /* Get type and index */
            eflag = !nitf_Field_get(subheader->NITF_DESOFLW,
                                    (NITF_DATA*)type,
                                    NITF_CONV_STRING,
                                    NITF_DESOFLW_SZ + 1,
                                    error);
            eflag |= !nitf_Field_get(subheader->NITF_DESITEM,
                                     &segIndex,
                                     NITF_CONV_INT,
                                     NITF_INT32_SZ,
                                     error);
            if (eflag)
            {
                nitf_Error_init(error, "Could not retrieve DE segment header overflow value",
                        NITF_CTXT,
                        NITF_ERR_INVALID_OBJECT);
                return NITF_FAILURE;
            }
            nitf_Field_trimString(type);

            /* File header user defined */
            if (strcmp(type, "UDHD") == 0)
            {
                extLength = record->header->NITF_UDHDL;
                overflowIndex = record->header->NITF_UDHOFL;
                destination = record->header->userDefinedSection;
            }
            /* File header extended */
            else if (strcmp(type, "XHD") == 0)
            {
                extLength = record->header->NITF_XHDL;
                overflowIndex = record->header->NITF_XHDLOFL;
                destination = record->header->extendedSection;
            }

            /* Image segment */
            else if ((strcmp(type, "UDID") == 0) ||
                     (strcmp(type, "IXSHD") == 0))
            {
                nitf_ImageSegment* imSeg;

                imSeg = nitf_List_get(record->images, segIndex - 1, error);

                /* Image segment user defined */
                if (strcmp(type, "UDID") == 0)
                {
                    extLength = imSeg->subheader->NITF_UDIDL;
                    overflowIndex = imSeg->subheader->NITF_UDOFL;
                    destination = imSeg->subheader->userDefinedSection;
                }
                else
                {
                    extLength = imSeg->subheader->NITF_IXSHDL;
                    overflowIndex = imSeg->subheader->NITF_IXSOFL;
                    destination = imSeg->subheader->extendedSection;
                }
            }

            /* Graphics segment */
            else if (strcmp(type, "SXSHD") == 0)
            {
                nitf_GraphicSegment* grSeg;

                grSeg = nitf_List_get(record->graphics, segIndex - 1, error);
                extLength = grSeg->subheader->NITF_SXSHDL;
                overflowIndex = grSeg->subheader->NITF_SXSOFL;
                destination = grSeg->subheader->extendedSection;
            }

            /* Labels segment */
            else if (strcmp(type, "LXSHD") == 0)
            {
                nitf_LabelSegment* lbSeg;

                lbSeg = nitf_List_get(record->labels, segIndex - 1, error);
                extLength = lbSeg->subheader->NITF_LXSHDL;
                overflowIndex = lbSeg->subheader->NITF_LXSOFL;
                destination = lbSeg->subheader->extendedSection;
            }

            /* Text segment */
            else if (strcmp(type, "TXSHD") == 0)
            {
                nitf_TextSegment* txSeg;

                txSeg = nitf_List_get(record->texts, segIndex - 1, error);
                extLength = txSeg->subheader->NITF_TXSHDL;
                overflowIndex = txSeg->subheader->NITF_TXSOFL;
                destination = txSeg->subheader->extendedSection;
            }
            else
            {
                nitf_Error_init(error,
                                "Invalid overflow segment type or index",
                                NITF_CTXT,
                                NITF_ERR_INVALID_OBJECT);
                return NITF_FAILURE;
            }

            if (!nitf_Field_setUint32(extLength, 0, error))
                return NITF_FAILURE;
            if (!nitf_Field_setUint32(overflowIndex, 0, error))
                return NITF_FAILURE;
            if (!moveTREs(subheader->userDefinedSection, destination, 0, error))
                return NITF_FAILURE;

            /* Remove the DE segment how does this effect indexes */
            if (!nitf_Record_removeDataExtensionSegment(record,
                                                        deIndex - 1,
                                                        error))
                return NITF_FAILURE;

            deIter = nitf_List_begin(record->dataExtensions);
            deIndex = 1;
            deRemoved += 1;
            continue;
        }

        deIndex += 1;
        nitf_ListIterator_increment(&deIter);
    }

    return NITF_SUCCESS;
}
