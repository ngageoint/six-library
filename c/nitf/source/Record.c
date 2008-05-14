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

#include "nitf/Record.h"

/*
 *  Destroy a list items that we have in our record.
 *  The first argument is the list pointer.
 *  The second argument is the type of object in the list
 *  For example, here is how to destroy the images segments:
 *
 *  _NITF_TRASH_LIST(r->images, nitf_ImageSegment)
 *
 *  Lp = r->images
 *  Ty = nitf_ImageSegment
 *  so...
 *  while (!nitf_List_isEmpty(r->images))
 *  {
 *      nitf_ImageSegment* type =
 *            (nitf_ImageSegment*)nitf_List_popFront(r->images);
 *
 *      nitf_ImageSegment_destruct(& type );
 *  }
 *
 *  Note that we dont want a semicolon at the end
 */
#define _NITF_TRASH_LIST(Lp_, Ty) \
    while (!nitf_List_isEmpty(Lp_)) \
    { \
        Ty* type = (Ty*)nitf_List_popFront(Lp_); \
        Ty##_destruct(& type ); \
    }

NITFAPI(nitf_Record *) nitf_Record_construct
(nitf_Version version, nitf_Error * error)
{
    nitf_Record *record = (nitf_Record *) NITF_MALLOC(sizeof(nitf_Record));
    if (!record)
    {
        nitf_Error_init(error, NITF_STRERROR(NITF_ERRNO),
                        NITF_CTXT, NITF_ERR_MEMORY);
        return NULL;
    }

    /*  Right now, we are only doing these  */
    record->header = NULL;
    record->images = NULL;

    /*  Right now, we aren't doing these things  */
    record->graphics = NULL;
    record->labels = NULL;
    record->texts = NULL;
    record->dataExtensions = NULL;
    record->reservedExtensions = NULL;

    /*  This block does the children creations        */
    /*  In the event of a constructor error, the      */
    /*  error object is already populated, and        */
    /*  since we have all of our objects NULL-inited  */
    /*  we have the same behavior for each failure.   */
    /*  It lives at the end and is called TRAGIC      */

    record->header = nitf_FileHeader_construct(error);
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

    /**************************************/
    /* NOW, set any version-specific data */
    if (version == NITF_VER_20)
    {
        nitf_Field_setRawData(record->header->fileVersion,
                              (NITF_DATA *) "02.00", NITF_FVER_SZ, error);

        /* resize for NITF 2.0 */
        nitf_FileSecurity_resizeForVersion(record->header->securityGroup,
                                           version, error);
    }
    /* the default is 2.1 */
    else
    {
        nitf_Field_setRawData(record->header->fileVersion,
                              (NITF_DATA *) "02.10", NITF_FVER_SZ, error);
    }

    /*  If all went well, we are very happy.  Now return  */
    return record;

CATCH_TRAGIC:
    /*  If something went wrong, somebody threw to us  */
    /*  So tragic!  So young to die!                   */
    nitf_Record_destruct(&record);
    return NULL;
}


NITFAPI(nitf_Record *) nitf_Record_clone(nitf_Record * source,
        nitf_Error * error)
{
    nitf_Record *record = NULL;
    NITF_BOOL success;

    if (!source)
    {
        nitf_Error_initf(error,
                         NITF_CTXT,
                         NITF_ERR_INVALID_OBJECT,
                         "Trying to clone NULL pointer");
        return NULL;
    }

    record = (nitf_Record *) NITF_MALLOC(sizeof(nitf_Record));
    if (!record)
    {
        nitf_Error_init(error, NITF_STRERROR(NITF_ERRNO),
                        NITF_CTXT, NITF_ERR_MEMORY);
        return NULL;
    }

    /*  Null-set in case we auto-destruct  */
    record->header = NULL;
    record->images = NULL;
    record->graphics = NULL;
    record->labels = NULL;
    record->texts = NULL;
    record->dataExtensions = NULL;
    record->reservedExtensions = NULL;

    /*  Right now, we are only doing the header and image setup  */
    record->header = nitf_FileHeader_clone(source->header, error);
    if (!record->header)
    {
        /*  Destruct gracefully if we had a problem  */
        nitf_Record_destruct(&record);
        return NULL;
    }
    record->images =
        nitf_List_clone(source->images,
                        (NITF_DATA_ITEM_CLONE) nitf_ImageSegment_clone,
                        error);

    if (!record->images)
    {
        /*  Destruct gracefully if we had a problem  */
        nitf_Record_destruct(&record);
        return NULL;
    }

    record->graphics =
        nitf_List_clone(source->graphics,
                        (NITF_DATA_ITEM_CLONE) nitf_GraphicSegment_clone,
                        error);
    if (!record->graphics)
    {
        /*  Destruct gracefully if we had a problem  */
        nitf_Record_destruct(&record);
        return NULL;
    }

    record->labels =
        nitf_List_clone(source->labels,
                        (NITF_DATA_ITEM_CLONE) nitf_LabelSegment_clone,
                        error);
    if (!record->labels)
    {
        /*  Destruct gracefully if we had a problem  */
        nitf_Record_destruct(&record);
        return NULL;
    }

    record->texts =
        nitf_List_clone(source->texts,
                        (NITF_DATA_ITEM_CLONE) nitf_TextSegment_clone,
                        error);
    if (!record->texts)
    {
        /*  Destruct gracefully if we had a problem  */
        nitf_Record_destruct(&record);
        return NULL;
    }

    record->dataExtensions =
        nitf_List_clone(source->dataExtensions,
                        (NITF_DATA_ITEM_CLONE) nitf_DESegment_clone,
                        error);
    if (!record->dataExtensions)
    {
        /*  Destruct gracefully if we had a problem  */
        nitf_Record_destruct(&record);
        return NULL;
    }

    record->reservedExtensions =
        nitf_List_clone(source->reservedExtensions,
                        (NITF_DATA_ITEM_CLONE) nitf_RESegment_clone,
                        error);
    if (!record->reservedExtensions)
    {
        /*  Destruct gracefully if we had a problem  */
        nitf_Record_destruct(&record);
        return NULL;
    }

    return record;

CATCH_ERROR:
    /*  Destruct gracefully if we had a problem  */
    nitf_Record_destruct(&record);
    return NULL;
}


NITFAPI(void) nitf_Record_destruct(nitf_Record ** record)
{
    if (*record)
    {
        if ((*record)->header)
        {
            nitf_FileHeader_destruct(&(*record)->header);
        }

        /*  When destroying these, we always use the same macro  */
        /*  The macro is walking the list and deleting the items */
        /*  by calling the segment destructors by name           */
        if ((*record)->images)
        {
            /*  Walk the list and destroy it  */
            _NITF_TRASH_LIST((*record)->images,
                             nitf_ImageSegment)
            nitf_List_destruct(&(*record)->images);
        }

        if ((*record)->graphics)
        {
            /*  Walk the list and destroy it  */
            _NITF_TRASH_LIST((*record)->graphics,
                             nitf_GraphicSegment)
            nitf_List_destruct(&(*record)->graphics);
        }

        if ((*record)->labels)
        {
            /*  Walk the list and destroy it  */
            _NITF_TRASH_LIST((*record)->labels,
                             nitf_LabelSegment)
            nitf_List_destruct(&(*record)->labels);
        }

        if ((*record)->texts)
        {
            /*  Walk the list and destroy it  */
            _NITF_TRASH_LIST((*record)->texts,
                             nitf_TextSegment)
            nitf_List_destruct(&(*record)->texts);
        }

        if ((*record)->dataExtensions)
        {
            /*  Walk the list and destroy it  */
            _NITF_TRASH_LIST((*record)->dataExtensions,
                             nitf_DESegment)
            nitf_List_destruct(&(*record)->dataExtensions);
        }

        if ((*record)->reservedExtensions)
        {
            /*  Walk the list and destroy it  */
            _NITF_TRASH_LIST((*record)->reservedExtensions,
                             nitf_RESegment)
            nitf_List_destruct(&(*record)->reservedExtensions);
        }

        NITF_FREE(*record);
        *record = NULL;
    }
}


NITFAPI(nitf_Version) nitf_Record_getVersion(nitf_Record * record,
        nitf_Error * error)
{
    char version[6];
    nitf_Version fver = NITF_VER_UNKNOWN;

    /* make sure the pieces of what we want exist */
    if (record && record->header && record->header->NITF_FVER)
    {
        memcpy(version, record->header->NITF_FVER->raw, 5);
        version[5] = 0;

        /* NSIF1.0 == NITF2.1 (from our point of view anyway) */
        if (strncmp(record->header->NITF_FHDR->raw, "NSIF", 4) == 0
                || strncmp(version, "02.10", 5) == 0)
            fver = NITF_VER_21;
        else if (strncmp(version, "02.00", 5) == 0)
            fver = NITF_VER_20;
    }
    return fver;
}


NITFAPI(nitf_ImageSegment *) nitf_Record_newImageSegment(nitf_Record *
        record,
        nitf_Error *
        error)
{
    nitf_ImageSegment *segment = NULL;
    nitf_ComponentInfo *info = NULL;
    nitf_ComponentInfo **infoArray = NULL;
    nitf_Uint32 num;
    int i, stringLen;
    char buf[256];               /* temp for holding string */
    nitf_Version ver;           /* holds the version */

    /* create the new info */
    info = nitf_ComponentInfo_construct(NITF_LISH_SZ, NITF_LI_SZ, error);
    if (!info)
    {
        goto CATCH_ERROR;
    }

    /* create the new segment */
    segment = nitf_ImageSegment_construct(error);
    if (!segment)
    {
        goto CATCH_ERROR;
    }

    /* update the version of the FileSecurity, if necessary */
    ver = nitf_Record_getVersion(record, error);
    if (ver == NITF_VER_20)
    {
        /* resize */
        nitf_FileSecurity_resizeForVersion(segment->subheader->
                                           securityGroup, ver, error);
    }

    /* add to the list */
    if (!nitf_List_pushBack(record->images, (NITF_DATA *) segment, error))
    {
        goto CATCH_ERROR;
    }

    /* get current num of images */
    NITF_TRY_GET_UINT32(record->header->numImages, &num, error);

    /* verify the number is ok */
    if (num < 0)
        num = 0;
    else if (num >= 999)
    {
        nitf_Error_initf(error, NITF_CTXT, NITF_ERR_INVALID_OBJECT,
                         "Cannot add another image segment, already have %d", num);
        goto CATCH_ERROR;
    }

    /* make new array, one bigger */
    infoArray =
        (nitf_ComponentInfo **) NITF_MALLOC(sizeof(nitf_ComponentInfo *) *
                                            (num + 1));
    if (!infoArray)
    {
        nitf_Error_init(error, NITF_STRERROR(NITF_ERRNO),
                        NITF_CTXT, NITF_ERR_MEMORY);
        goto CATCH_ERROR;
    }

    /* iterate over current infos */
    for (i = 0; i < num; ++i)
    {
        infoArray[i] = record->header->imageInfo[i];
    }
    /* add the new one */
    infoArray[i] = info;
    num++;

    /* convert the data to chars */
    stringLen = sprintf(buf, "%d", num);
    nitf_Field_setRawData(record->header->numImages, buf, stringLen,
                          error);

    /* delete old one, if there, and set to new one */
    if (record->header->imageInfo)
    {
        NITF_FREE(record->header->imageInfo);
    }
    record->header->imageInfo = infoArray;

    /* return the segment */
    return segment;

CATCH_ERROR:
    if (info)
        nitf_ComponentInfo_destruct(&info);
    if (segment)
        nitf_ImageSegment_destruct(&segment);
    return NULL;
}


NITFAPI(nitf_GraphicSegment *) nitf_Record_newGraphicSegment(nitf_Record *
        record,
        nitf_Error *
        error)
{
    nitf_GraphicSegment *segment = NULL;
    nitf_ComponentInfo *info = NULL;
    nitf_ComponentInfo **infoArray = NULL;
    nitf_Uint32 num;
    int i, stringLen;
    char buf[256];               /* temp for holding string */
    nitf_Version ver;           /* holds the version */

    /* create the new info */
    info = nitf_ComponentInfo_construct(NITF_LSSH_SZ, NITF_LS_SZ, error);
    if (!info)
    {
        goto CATCH_ERROR;
    }

    /* create the new segment */
    segment = nitf_GraphicSegment_construct(error);
    if (!segment)
    {
        goto CATCH_ERROR;
    }

    /* update the version of the FileSecurity, if necessary */
    ver = nitf_Record_getVersion(record, error);
    if (ver == NITF_VER_20)
    {
        /* resize */
        nitf_FileSecurity_resizeForVersion(segment->subheader->
                                           securityGroup, ver, error);
    }

    /* add to the list */
    if (!nitf_List_pushBack
            (record->graphics, (NITF_DATA *) segment, error))
    {
        goto CATCH_ERROR;
    }

    /* get current num of graphics */
    NITF_TRY_GET_UINT32(record->header->numGraphics, &num, error);

    /* verify the number is ok */
    if (num < 0)
        num = 0;
    else if (num >= 999)
    {
        nitf_Error_initf(error, NITF_CTXT, NITF_ERR_INVALID_OBJECT,
                         "Cannot add another graphic segment, already have %d", num);
        goto CATCH_ERROR;
    }

    /* make new array, one bigger */
    infoArray =
        (nitf_ComponentInfo **) NITF_MALLOC(sizeof(nitf_ComponentInfo *) *
                                            (num + 1));
    if (!infoArray)
    {
        nitf_Error_init(error, NITF_STRERROR(NITF_ERRNO),
                        NITF_CTXT, NITF_ERR_MEMORY);
        goto CATCH_ERROR;
    }

    /* iterate over current infos */
    for (i = 0; i < num; ++i)
    {
        infoArray[i] = record->header->graphicInfo[i];
    }
    /* add the new one */
    infoArray[i] = info;
    num++;

    /* convert the data to chars */
    stringLen = sprintf(buf, "%d", num);
    nitf_Field_setRawData(record->header->numGraphics, buf, stringLen,
                          error);

    /* delete old one, if there, and set to new one */
    if (record->header->graphicInfo)
    {
        NITF_FREE(record->header->graphicInfo);
    }
    record->header->graphicInfo = infoArray;

    /* return the segment */
    return segment;

CATCH_ERROR:
    if (info)
        nitf_ComponentInfo_destruct(&info);
    if (segment)
        nitf_GraphicSegment_destruct(&segment);
    return NULL;
}


NITFAPI(nitf_TextSegment *) nitf_Record_newTextSegment(nitf_Record *
        record,
        nitf_Error * error)
{
    nitf_TextSegment *segment = NULL;
    nitf_ComponentInfo *info = NULL;
    nitf_ComponentInfo **infoArray = NULL;
    nitf_Uint32 num;
    int i, stringLen;
    char buf[256];               /* temp for holding string */
    nitf_Version ver;           /* holds the version */

    /* create the new info */
    info = nitf_ComponentInfo_construct(NITF_LTSH_SZ, NITF_LT_SZ, error);
    if (!info)
    {
        goto CATCH_ERROR;
    }

    /* create the new segment */
    segment = nitf_TextSegment_construct(error);
    if (!segment)
    {
        goto CATCH_ERROR;
    }

    /* update the version of the FileSecurity, if necessary */
    ver = nitf_Record_getVersion(record, error);
    if (ver == NITF_VER_20)
    {
        /* resize */
        nitf_FileSecurity_resizeForVersion(segment->subheader->
                                           securityGroup, ver, error);
    }

    /* add to the list */
    if (!nitf_List_pushBack(record->texts, (NITF_DATA *) segment, error))
    {
        goto CATCH_ERROR;
    }

    /* get current num of texts */
    NITF_TRY_GET_UINT32(record->header->numTexts, &num, error);

    /* verify the number is ok */
    if (num < 0)
        num = 0;
    else if (num >= 999)
    {
        nitf_Error_initf(error, NITF_CTXT, NITF_ERR_INVALID_OBJECT,
                         "Cannot add another text segment, already have %d", num);
        goto CATCH_ERROR;
    }

    /* make new array, one bigger */
    infoArray =
        (nitf_ComponentInfo **) NITF_MALLOC(sizeof(nitf_ComponentInfo *) *
                                            (num + 1));
    if (!infoArray)
    {
        nitf_Error_init(error, NITF_STRERROR(NITF_ERRNO),
                        NITF_CTXT, NITF_ERR_MEMORY);
        goto CATCH_ERROR;
    }

    /* iterate over current infos */
    for (i = 0; i < num; ++i)
    {
        infoArray[i] = record->header->textInfo[i];
    }
    /* add the new one */
    infoArray[i] = info;
    num++;

    /* convert the data to chars */
    stringLen = sprintf(buf, "%d", num);
    nitf_Field_setRawData(record->header->numTexts, buf, stringLen, error);

    /* delete old one, if there, and set to new one */
    if (record->header->textInfo)
    {
        NITF_FREE(record->header->textInfo);
    }
    record->header->textInfo = infoArray;

    /* return the segment */
    return segment;

CATCH_ERROR:
    if (info)
        nitf_ComponentInfo_destruct(&info);
    if (segment)
        nitf_TextSegment_destruct(&segment);
    return NULL;
}


NITFAPI(nitf_DESegment *) nitf_Record_newDataExtensionSegment(
    nitf_Record * record,
    nitf_Error * error)
{
    nitf_DESegment *segment = NULL;
    nitf_ComponentInfo *info = NULL;
    nitf_ComponentInfo **infoArray = NULL;
    nitf_Uint32 num;
    int i, stringLen;
    char buf[256];               /* temp for holding string */
    nitf_Version ver;           /* holds the version */

    /* create the new info */
    info = nitf_ComponentInfo_construct(NITF_LDSH_SZ, NITF_LD_SZ, error);
    if (!info)
    {
        goto CATCH_ERROR;
    }

    /* create the new segment */
    segment = nitf_DESegment_construct(error);
    if (!segment)
    {
        goto CATCH_ERROR;
    }

    /* update the version of the FileSecurity, if necessary */
    ver = nitf_Record_getVersion(record, error);
    if (ver == NITF_VER_20)
    {
        /* resize */
        nitf_FileSecurity_resizeForVersion(segment->subheader->
                                           securityGroup, ver, error);
    }

    /* add to the list */
    if (!nitf_List_pushBack(record->dataExtensions, (NITF_DATA *) segment, error))
    {
        goto CATCH_ERROR;
    }

    /* get current num of DEs */
    NITF_TRY_GET_UINT32(record->header->numDataExtensions, &num, error);

    /* verify the number is ok */
    if (num < 0)
        num = 0;
    else if (num >= 999)
    {
        nitf_Error_initf(error, NITF_CTXT, NITF_ERR_INVALID_OBJECT,
                         "Cannot add another DE segment, already have %d", num);
        goto CATCH_ERROR;
    }

    /* make new array, one bigger */
    infoArray =
        (nitf_ComponentInfo **) NITF_MALLOC(sizeof(nitf_ComponentInfo *) *
                                            (num + 1));
    if (!infoArray)
    {
        nitf_Error_init(error, NITF_STRERROR(NITF_ERRNO),
                        NITF_CTXT, NITF_ERR_MEMORY);
        goto CATCH_ERROR;
    }

    /* iterate over current infos */
    for (i = 0; i < num; ++i)
    {
        infoArray[i] = record->header->dataExtensionInfo[i];
    }
    /* add the new one */
    infoArray[i] = info;
    num++;

    /* convert the data to chars */
    stringLen = sprintf(buf, "%d", num);
    nitf_Field_setRawData(record->header->numDataExtensions, buf, stringLen, error);

    /* delete old one, if there, and set to new one */
    if (record->header->dataExtensionInfo)
    {
        NITF_FREE(record->header->dataExtensionInfo);
    }
    record->header->dataExtensionInfo = infoArray;

    /* return the segment */
    return segment;

CATCH_ERROR:
    if (info)
        nitf_ComponentInfo_destruct(&info);
    if (segment)
        nitf_DESegment_destruct(&segment);
    return NULL;
}


NITFAPI(NITF_BOOL) nitf_Record_removeImageSegment
(
    nitf_Record * record,
    nitf_Uint32 segmentNumber,
    nitf_Error * error
)
{
    nitf_Uint32 num; /* holds the number */
    nitf_ComponentInfo **infoArray = NULL;
    nitf_ImageSegment* segment = NULL;
    int i, stringLen;
    char buf[26];               /* temp for holding string */
    nitf_ListIterator iter = nitf_List_at(record->images, segmentNumber);

    if (iter.current == NULL)
    {
        nitf_Error_initf(error,
                         NITF_CTXT,
                         NITF_ERR_INVALID_OBJECT,
                         "Invalid image segment number");
        goto CATCH_ERROR;
    }

    /* remove from the list */
    segment = (nitf_ImageSegment*)nitf_List_remove(record->images, &iter);
    /* destroy it */
    nitf_ImageSegment_destruct(&segment);

    /* get current num*/
    NITF_TRY_GET_UINT32(record->header->numImages, &num, error);

    /* if we actually need to resize */
    if (num > 1)
    {
        /* make new array, one smaller */
        infoArray =
            (nitf_ComponentInfo **) NITF_MALLOC(sizeof(nitf_ComponentInfo *) *
                                                (num - 1));

        /* iterate over current infos */
        for (i = 0; i < segmentNumber; ++i)
        {
            infoArray[i] = record->header->imageInfo[i];
        }
        for (i = segmentNumber + 1; i < num; ++i)
        {
            infoArray[i - 1] = record->header->imageInfo[i];
        }
    }

    /* update the num field in the header */
    num--;

    /* convert the data to chars */
    stringLen = sprintf(buf, "%d", num);
    nitf_Field_setRawData(record->header->numImages, buf, stringLen, error);

    /* delete old one, if there, and set to new one */
    if (record->header->imageInfo)
    {
        NITF_FREE(record->header->imageInfo);
    }
    record->header->imageInfo = infoArray;

    return NITF_SUCCESS;

CATCH_ERROR:
    return NITF_FAILURE;
}


NITFAPI(NITF_BOOL) nitf_Record_removeGraphicSegment
(
    nitf_Record * record,
    nitf_Uint32 segmentNumber,
    nitf_Error * error
)
{
    nitf_Uint32 num; /* holds the number */
    nitf_ComponentInfo **infoArray = NULL;
    nitf_GraphicSegment* segment = NULL;
    int i, stringLen;
    char buf[26];               /* temp for holding string */
    nitf_ListIterator iter = nitf_List_at(record->graphics, segmentNumber);

    if (iter.current == NULL)
    {
        nitf_Error_initf(error,
                         NITF_CTXT,
                         NITF_ERR_INVALID_OBJECT,
                         "Invalid graphic segment number");
        goto CATCH_ERROR;
    }

    /* remove from the list */
    segment = (nitf_GraphicSegment*)nitf_List_remove(record->graphics, &iter);
    /* destroy it */
    nitf_GraphicSegment_destruct(&segment);

    /* get current num*/
    NITF_TRY_GET_UINT32(record->header->numGraphics, &num, error);

    /* if we actually need to resize */
    if (num > 1)
    {
        /* make new array, one smaller */
        infoArray =
            (nitf_ComponentInfo **) NITF_MALLOC(sizeof(nitf_ComponentInfo *) *
                                                (num - 1));

        /* iterate over current infos */
        for (i = 0; i < segmentNumber; ++i)
        {
            infoArray[i] = record->header->graphicInfo[i];
        }
        for (i = segmentNumber + 1; i < num; ++i)
        {
            infoArray[i - 1] = record->header->graphicInfo[i];
        }
    }

    /* update the num field in the header */
    num--;

    /* convert the data to chars */
    stringLen = sprintf(buf, "%d", num);
    nitf_Field_setRawData(record->header->numGraphics, buf, stringLen, error);

    /* delete old one, if there, and set to new one */
    if (record->header->graphicInfo)
    {
        NITF_FREE(record->header->graphicInfo);
    }
    record->header->graphicInfo = infoArray;

    return NITF_SUCCESS;

CATCH_ERROR:
    return NITF_FAILURE;
}


NITFAPI(NITF_BOOL) nitf_Record_removeLabelSegment
(
    nitf_Record * record,
    nitf_Uint32 segmentNumber,
    nitf_Error * error
)
{
    nitf_Uint32 num; /* holds the number */
    nitf_ComponentInfo **infoArray = NULL;
    nitf_LabelSegment* segment = NULL;
    int i, stringLen;
    char buf[26];               /* temp for holding string */
    nitf_ListIterator iter = nitf_List_at(record->labels, segmentNumber);

    if (iter.current == NULL)
    {
        nitf_Error_initf(error,
                         NITF_CTXT,
                         NITF_ERR_INVALID_OBJECT,
                         "Invalid label segment number");
        goto CATCH_ERROR;
    }

    /* remove from the list */
    segment = (nitf_LabelSegment*)nitf_List_remove(record->labels, &iter);
    /* destroy it */
    nitf_LabelSegment_destruct(&segment);

    /* get current num*/
    NITF_TRY_GET_UINT32(record->header->numLabels, &num, error);

    /* if we actually need to resize */
    if (num > 1)
    {
        /* make new array, one smaller */
        infoArray =
            (nitf_ComponentInfo **) NITF_MALLOC(sizeof(nitf_ComponentInfo *) *
                                                (num - 1));

        /* iterate over current infos */
        for (i = 0; i < segmentNumber; ++i)
        {
            infoArray[i] = record->header->labelInfo[i];
        }
        for (i = segmentNumber + 1; i < num; ++i)
        {
            infoArray[i - 1] = record->header->labelInfo[i];
        }
    }

    /* update the num field in the header */
    num--;

    /* convert the data to chars */
    stringLen = sprintf(buf, "%d", num);
    nitf_Field_setRawData(record->header->numLabels, buf, stringLen, error);

    /* delete old one, if there, and set to new one */
    if (record->header->labelInfo)
    {
        NITF_FREE(record->header->labelInfo);
    }
    record->header->labelInfo = infoArray;

    return NITF_SUCCESS;

CATCH_ERROR:
    return NITF_FAILURE;
}


NITFAPI(NITF_BOOL) nitf_Record_removeTextSegment
(
    nitf_Record * record,
    nitf_Uint32 segmentNumber,
    nitf_Error * error
)
{
    nitf_Uint32 num; /* holds the number */
    nitf_ComponentInfo **infoArray = NULL;
    nitf_TextSegment* segment = NULL;
    int i, stringLen;
    char buf[26];               /* temp for holding string */
    nitf_ListIterator iter = nitf_List_at(record->texts, segmentNumber);

    if (iter.current == NULL)
    {
        nitf_Error_initf(error,
                         NITF_CTXT,
                         NITF_ERR_INVALID_OBJECT,
                         "Invalid text segment number");
        goto CATCH_ERROR;
    }

    /* remove from the list */
    segment = (nitf_TextSegment*)nitf_List_remove(record->texts, &iter);
    /* destroy it */
    nitf_TextSegment_destruct(&segment);

    /* get current num*/
    NITF_TRY_GET_UINT32(record->header->numTexts, &num, error);

    /* if we actually need to resize */
    if (num > 1)
    {
        /* make new array, one smaller */
        infoArray =
            (nitf_ComponentInfo **) NITF_MALLOC(sizeof(nitf_ComponentInfo *) *
                                                (num - 1));

        /* iterate over current infos */
        for (i = 0; i < segmentNumber; ++i)
        {
            infoArray[i] = record->header->textInfo[i];
        }
        for (i = segmentNumber + 1; i < num; ++i)
        {
            infoArray[i - 1] = record->header->textInfo[i];
        }
    }

    /* update the num field in the header */
    num--;

    /* convert the data to chars */
    stringLen = sprintf(buf, "%d", num);
    nitf_Field_setRawData(record->header->numTexts, buf, stringLen, error);

    /* delete old one, if there, and set to new one */
    if (record->header->textInfo)
    {
        NITF_FREE(record->header->textInfo);
    }
    record->header->textInfo = infoArray;

    return NITF_SUCCESS;

CATCH_ERROR:
    return NITF_FAILURE;
}


NITFAPI(NITF_BOOL) nitf_Record_removeDataExtensionSegment
(
    nitf_Record * record,
    nitf_Uint32 segmentNumber,
    nitf_Error * error
)
{
    nitf_Uint32 num; /* holds the number */
    nitf_ComponentInfo **infoArray = NULL;
    nitf_DESegment* segment = NULL;
    int i, stringLen;
    char buf[26];               /* temp for holding string */
    nitf_ListIterator iter = nitf_List_at(record->dataExtensions, segmentNumber);

    if (iter.current == NULL)
    {
        nitf_Error_initf(error,
                         NITF_CTXT,
                         NITF_ERR_INVALID_OBJECT,
                         "Invalid dataExtension segment number");
        goto CATCH_ERROR;
    }

    /* remove from the list */
    segment = (nitf_DESegment*)nitf_List_remove(record->dataExtensions, &iter);
    /* destroy it */
    nitf_DESegment_destruct(&segment);

    /* get current num*/
    NITF_TRY_GET_UINT32(record->header->numDataExtensions, &num, error);

    /* if we actually need to resize */
    if (num > 1)
    {
        /* make new array, one smaller */
        infoArray =
            (nitf_ComponentInfo **) NITF_MALLOC(sizeof(nitf_ComponentInfo *) *
                                                (num - 1));

        /* iterate over current infos */
        for (i = 0; i < segmentNumber; ++i)
        {
            infoArray[i] = record->header->dataExtensionInfo[i];
        }
        for (i = segmentNumber + 1; i < num; ++i)
        {
            infoArray[i - 1] = record->header->dataExtensionInfo[i];
        }
    }

    /* update the num field in the header */
    num--;

    /* convert the data to chars */
    stringLen = sprintf(buf, "%d", num);
    nitf_Field_setRawData(record->header->numDataExtensions, buf, stringLen, error);

    /* delete old one, if there, and set to new one */
    if (record->header->dataExtensionInfo)
    {
        NITF_FREE(record->header->dataExtensionInfo);
    }
    record->header->dataExtensionInfo = infoArray;

    return NITF_SUCCESS;

CATCH_ERROR:
    return NITF_FAILURE;
}


NITFAPI(NITF_BOOL) nitf_Record_removeReservedExtensionSegment
(
    nitf_Record * record,
    nitf_Uint32 segmentNumber,
    nitf_Error * error
)
{
    nitf_Uint32 num; /* holds the number */
    nitf_ComponentInfo **infoArray = NULL;
    nitf_RESegment* segment = NULL;
    int i, stringLen;
    char buf[26];               /* temp for holding string */
    nitf_ListIterator iter = nitf_List_at(record->reservedExtensions, segmentNumber);

    if (iter.current == NULL)
    {
        nitf_Error_initf(error,
                         NITF_CTXT,
                         NITF_ERR_INVALID_OBJECT,
                         "Invalid reservedExtension segment number");
        goto CATCH_ERROR;
    }

    /* remove from the list */
    segment = (nitf_RESegment*)nitf_List_remove(record->reservedExtensions, &iter);
    /* destroy it */
    nitf_RESegment_destruct(&segment);

    /* get current num*/
    NITF_TRY_GET_UINT32(record->header->numReservedExtensions, &num, error);

    /* if we actually need to resize */
    if (num > 1)
    {
        /* make new array, one smaller */
        infoArray =
            (nitf_ComponentInfo **) NITF_MALLOC(sizeof(nitf_ComponentInfo *) *
                                                (num - 1));

        /* iterate over current infos */
        for (i = 0; i < segmentNumber; ++i)
        {
            infoArray[i] = record->header->reservedExtensionInfo[i];
        }
        for (i = segmentNumber + 1; i < num; ++i)
        {
            infoArray[i - 1] = record->header->reservedExtensionInfo[i];
        }
    }

    /* update the num field in the header */
    num--;

    /* convert the data to chars */
    stringLen = sprintf(buf, "%d", num);
    nitf_Field_setRawData(record->header->numReservedExtensions, buf, stringLen, error);

    /* delete old one, if there, and set to new one */
    if (record->header->reservedExtensionInfo)
    {
        NITF_FREE(record->header->reservedExtensionInfo);
    }
    record->header->reservedExtensionInfo = infoArray;

    return NITF_SUCCESS;

CATCH_ERROR:
    return NITF_FAILURE;
}

NITFAPI(NITF_BOOL) nitf_Record_moveImageSegment
(
    nitf_Record * record,
    nitf_Uint32 oldIndex,
    nitf_Uint32 newIndex,
    nitf_Error * error
)
{
    nitf_Uint32 num;
    nitf_ComponentInfo *tempInfo = NULL;
    
    NITF_TRY_GET_UINT32(record->header->numImages, &num, error);

    if (oldIndex < 0 || newIndex < 0 || oldIndex >= num || newIndex >= num)
    {
        nitf_Error_initf(error,
                         NITF_CTXT,
                         NITF_ERR_INVALID_OBJECT,
                         "Invalid index provided");
        goto CATCH_ERROR;
    }
    
    /* just return OK if no change */
    if (oldIndex == newIndex)
        return NITF_SUCCESS;

    /* do the list move */
    if (nitf_List_move(record->images, oldIndex, newIndex, error))
        goto CATCH_ERROR;
    
    /* now, need to move the component info - just move pointers */
    tempInfo = record->header->imageInfo[oldIndex];
    record->header->imageInfo[oldIndex] = record->header->imageInfo[newIndex];
    record->header->imageInfo[newIndex] = tempInfo;
    
    return NITF_SUCCESS;
    
  CATCH_ERROR:
      return NITF_FAILURE;
}


NITFAPI(NITF_BOOL) nitf_Record_moveGraphicSegment
(
    nitf_Record * record,
    nitf_Uint32 oldIndex,
    nitf_Uint32 newIndex,
    nitf_Error * error
)
{
    nitf_Uint32 num;
    nitf_ComponentInfo *tempInfo = NULL;
    
    NITF_TRY_GET_UINT32(record->header->numGraphics, &num, error);

    if (oldIndex < 0 || newIndex < 0 || oldIndex >= num || newIndex >= num)
    {
        nitf_Error_initf(error,
                         NITF_CTXT,
                         NITF_ERR_INVALID_OBJECT,
                         "Invalid index provided");
        goto CATCH_ERROR;
    }
    
    /* just return OK if no change */
    if (oldIndex == newIndex)
        return NITF_SUCCESS;

    /* do the list move */
    if (nitf_List_move(record->graphics, oldIndex, newIndex, error))
        goto CATCH_ERROR;
    
    /* now, need to move the component info - just move pointers */
    tempInfo = record->header->graphicInfo[oldIndex];
    record->header->graphicInfo[oldIndex] = record->header->graphicInfo[newIndex];
    record->header->graphicInfo[newIndex] = tempInfo;
    
    return NITF_SUCCESS;
    
  CATCH_ERROR:
    return NITF_FAILURE;
}


NITFAPI(NITF_BOOL) nitf_Record_moveLabelSegment
(
    nitf_Record * record,
    nitf_Uint32 oldIndex,
    nitf_Uint32 newIndex,
    nitf_Error * error
)
{
    nitf_Uint32 num;
    nitf_ComponentInfo *tempInfo = NULL;
    
    NITF_TRY_GET_UINT32(record->header->numLabels, &num, error);

    if (oldIndex < 0 || newIndex < 0 || oldIndex >= num || newIndex >= num)
    {
        nitf_Error_initf(error,
                         NITF_CTXT,
                         NITF_ERR_INVALID_OBJECT,
                         "Invalid index provided");
        goto CATCH_ERROR;
    }
    
    /* just return OK if no change */
    if (oldIndex == newIndex)
        return NITF_SUCCESS;

    /* do the list move */
    if (nitf_List_move(record->labels, oldIndex, newIndex, error))
        goto CATCH_ERROR;
    
    /* now, need to move the component info - just move pointers */
    tempInfo = record->header->labelInfo[oldIndex];
    record->header->labelInfo[oldIndex] = record->header->labelInfo[newIndex];
    record->header->labelInfo[newIndex] = tempInfo;
    
    return NITF_SUCCESS;
    
  CATCH_ERROR:
    return NITF_FAILURE;
}


NITFAPI(NITF_BOOL) nitf_Record_moveTextSegment
(
    nitf_Record * record,
    nitf_Uint32 oldIndex,
    nitf_Uint32 newIndex,
    nitf_Error * error
)
{
    nitf_Uint32 num;
    nitf_ComponentInfo *tempInfo = NULL;
    
    NITF_TRY_GET_UINT32(record->header->numTexts, &num, error);

    if (oldIndex < 0 || newIndex < 0 || oldIndex >= num || newIndex >= num)
    {
        nitf_Error_initf(error,
                         NITF_CTXT,
                         NITF_ERR_INVALID_OBJECT,
                         "Invalid index provided");
        goto CATCH_ERROR;
    }
    
    /* just return OK if no change */
    if (oldIndex == newIndex)
        return NITF_SUCCESS;

    /* do the list move */
    if (nitf_List_move(record->texts, oldIndex, newIndex, error))
        goto CATCH_ERROR;
    
    /* now, need to move the component info - just move pointers */
    tempInfo = record->header->textInfo[oldIndex];
    record->header->textInfo[oldIndex] = record->header->textInfo[newIndex];
    record->header->textInfo[newIndex] = tempInfo;
    
    return NITF_SUCCESS;
    
  CATCH_ERROR:
    return NITF_FAILURE;
}


NITFAPI(NITF_BOOL) nitf_Record_moveDataExtensionSegment
(
    nitf_Record * record,
    nitf_Uint32 oldIndex,
    nitf_Uint32 newIndex,
    nitf_Error * error
)
{
    nitf_Uint32 num;
    nitf_ComponentInfo *tempInfo = NULL;
    
    NITF_TRY_GET_UINT32(record->header->numDataExtensions, &num, error);

    if (oldIndex < 0 || newIndex < 0 || oldIndex >= num || newIndex >= num)
    {
        nitf_Error_initf(error,
                         NITF_CTXT,
                         NITF_ERR_INVALID_OBJECT,
                         "Invalid index provided");
        goto CATCH_ERROR;
    }
    
    /* just return OK if no change */
    if (oldIndex == newIndex)
        return NITF_SUCCESS;

    /* do the list move */
    if (nitf_List_move(record->dataExtensions, oldIndex, newIndex, error))
        goto CATCH_ERROR;
    
    /* now, need to move the component info - just move pointers */
    tempInfo = record->header->dataExtensionInfo[oldIndex];
    record->header->dataExtensionInfo[oldIndex] = record->header->dataExtensionInfo[newIndex];
    record->header->dataExtensionInfo[newIndex] = tempInfo;
    
    return NITF_SUCCESS;
    
  CATCH_ERROR:
    return NITF_FAILURE;
}


NITFAPI(NITF_BOOL) nitf_Record_moveReservedExtensionSegment
(
    nitf_Record * record,
    nitf_Uint32 oldIndex,
    nitf_Uint32 newIndex,
    nitf_Error * error
)
{
    nitf_Uint32 num;
    nitf_ComponentInfo *tempInfo = NULL;
    
    NITF_TRY_GET_UINT32(record->header->numReservedExtensions, &num, error);

    if (oldIndex < 0 || newIndex < 0 || oldIndex >= num || newIndex >= num)
    {
        nitf_Error_initf(error,
                         NITF_CTXT,
                         NITF_ERR_INVALID_OBJECT,
                         "Invalid index provided");
        goto CATCH_ERROR;
    }
    
    /* just return OK if no change */
    if (oldIndex == newIndex)
        return NITF_SUCCESS;

    /* do the list move */
    if (nitf_List_move(record->reservedExtensions, oldIndex, newIndex, error))
        goto CATCH_ERROR;
    
    /* now, need to move the component info - just move pointers */
    tempInfo = record->header->reservedExtensionInfo[oldIndex];
    record->header->reservedExtensionInfo[oldIndex] = record->header->reservedExtensionInfo[newIndex];
    record->header->reservedExtensionInfo[newIndex] = tempInfo;
    
    return NITF_SUCCESS;

  CATCH_ERROR:
    return NITF_FAILURE;
}