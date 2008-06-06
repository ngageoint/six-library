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

/*  Local functions and constants for overflow segments */

#define TRE_OVERFLOW_STR "TRE_OVERFLOW"
#define TRE_OVERFLOW_VERSION 1
/*
  addOverflowSegment adds a (DE) overflow section

  The security section in the new sections's header is initialized from the
  supplied file security object which should come from the assoicated segment

  All fields are set except DESSHL and DESDATA

  The index one based of the new segment is returned, or zero on error
*/

NITFPRIV(nitf_Uint32) addOverflowSegment
(
  nitf_Record *record,        /* Record to modify */
  nitf_Uint32 segmentIdx,     /* Index of associated segment */
  char *segmentType,          /* Type of associated segment (i.e., UDID) */
                              /* Security information from associated segment */
  nitf_Field *securityClass,
  nitf_FileSecurity *fileSecurity,
  nitf_DESegment  **overflow, /* Returns new Overflow DESegment */
  nitf_Error *error           /* For error returns */
)
{
  nitf_Uint32 ovfIdx;        /* Index of overflow segment */

/*  Create the segment */

    ovfIdx = nitf_List_size(record->dataExtensions) + 1;
    *overflow = nitf_Record_newDataExtensionSegment(record,error);
    if(*overflow == NULL)
    { 
      nitf_Error_init(error,
            "Could not add overflow segment index",
                              NITF_CTXT, NITF_ERR_INVALID_OBJECT);
      return(0);
    }

/*  Set fields */

  nitf_FileSecurity_destruct(&((*overflow)->subheader->securityGroup));
  (*overflow)->subheader->securityGroup =
                          nitf_FileSecurity_clone(fileSecurity,error);

  nitf_Field_destruct(&((*overflow)->subheader->securityClass));
  (*overflow)->subheader->securityClass =
                          nitf_Field_clone(securityClass,error);
  if((*overflow)->subheader->securityClass == NULL)
    return(0);


  if(!nitf_Field_setString((*overflow)->subheader->NITF_DESTAG,
                                                     TRE_OVERFLOW_STR,error))
    return(0);

  if(!nitf_Field_setUint32((*overflow)->subheader->NITF_DESVER,
                                                 TRE_OVERFLOW_VERSION,error))
    return(0);

  if(!nitf_Field_setString((*overflow)->subheader->NITF_DESOFLW,
                                                     segmentType,error))
    return(0);

  if(!nitf_Field_setUint32((*overflow)->subheader->NITF_DESITEM,
                                                     segmentIdx,error))
    return(0);

  return(ovfIdx);
}

/* 
  moveTREs - Move TREs from one section to another 

  If the skip length is greater than zero, then initial TREs are skpped
  until the first TRE that makes the total skipped larger than the skip
  length. This is for transferring from the main segment to the DE segment
  (unmerge), the zero case is for DE to main segment (merge)

  The return is TRUE on success and FALSE on failure
*/

NITFPRIV(NITF_BOOL) moveTREs
(
  nitf_Extensions *source,       /* Source extension */
  nitf_Extensions *destination,  /* Destination extension */
  nitf_Uint32 skipLength,        /* Amount of TRE data to skip */
  nitf_Error *error              /* For errors */
)
{ 
  nitf_ExtensionsIterator srcIter;  /* Source extension iterator */
  nitf_ExtensionsIterator srcEnd;   /* Source extension iterator end */
  nitf_ExtensionsIterator dstIter;  /* Destination extension iterator */
  nitf_ExtensionsIterator dstEnd;   /* Destination extension iterator end */
  nitf_TRE *tre;                    /* Current TRE */

  srcIter = nitf_Extensions_begin(source);
  srcEnd = nitf_Extensions_end(source);

  if(skipLength != 0)
  {
    nitf_Int32 skipLeft;              /* Amount left to skip */
    nitf_Uint32 treLength;            /* Length of current TRE */

    skipLeft = skipLength;
    while(nitf_ExtensionsIterator_notEqualTo(&srcIter, &srcEnd))
    {
      tre = nitf_ExtensionsIterator_get(&srcIter);
      treLength = tre->length;
      if(treLength <= 0)
        treLength = (nitf_Uint32)tre->handler->getCurrentSize(tre, error);
      skipLeft -= treLength;
      if(skipLeft < 1)
        break;
      nitf_ExtensionsIterator_increment(&srcIter);
    } 
    if(skipLeft > 1)   /* No transfer required */
      srcIter = nitf_Extensions_end(source);
  }

  srcEnd = nitf_Extensions_end(source);

  while(nitf_ExtensionsIterator_notEqualTo(&srcIter, &srcEnd))
  {
    tre = nitf_Extensions_remove(source,&srcIter,error);
    nitf_Extensions_appendTRE(destination,tre,error);
  }

  return(NITF_SUCCESS);
}

/*
  fixOverflowIndexes reviews and corrects the indexes of DE overflow
  segments when any segment that might overflow (i.e., image segment) is
  removed. Removing a segment could effect the indexes in existing overflowed
  segments.

  If the segment being deleted does not have an overflow segment. If it has
  one, it shold be deleted before this function is called

  This function also removes the associated DE segent if it still exists

  Note: Adding a segment does not effect any existing overflows because new
  segments are added at the end of the corresponding segment list
*/

NITFPRIV(NITF_BOOL) fixOverflowIndexes
(
  nitf_Record *record,
  char *type,
  nitf_Uint32 index,   /* Segment index (zero based) */
  nitf_Error *error
)
{
  nitf_ListIterator deIter;        /* For DE iterating */
  nitf_ListIterator deEnd;         /* End point */
  nitf_DESubheader *subheader;     /* Current DE segment subheader */
  char oflw[NITF_DESOFLW_SZ+1];    /* Parent segment type (DESOFLW) */
  nitf_Uint32 item;                /* Segment index (DESITEM) */

/*
   Scan extensions for ones that have the same type and a index greater than
   the index of the segment being deleted. If this is the case decrement the
   overflow's index
*/

  deIter = nitf_List_begin(record->dataExtensions);
  deEnd = nitf_List_begin(record->dataExtensions);
  while(nitf_ListIterator_notEqualTo(&deIter, &deEnd))
  {
    subheader = (nitf_DESubheader *)
        ((nitf_DESegment *) nitf_ListIterator_get(&deIter))->subheader;

    if(!nitf_Field_get(subheader->NITF_DESOFLW,(NITF_DATA *) oflw,
                                  NITF_CONV_STRING,NITF_DESOFLW_SZ+1,error))
    {
      nitf_Error_init(error,
          "Could not retrieve DESOVFLW field",
                              NITF_CTXT,NITF_ERR_INVALID_OBJECT);
      return(NITF_FAILURE);
    }

    if(!nitf_Field_get(subheader->NITF_DESITEM,(NITF_DATA *) &item,
                                  NITF_CONV_UINT,sizeof(item), error))
    {
      nitf_Error_init(error,
          "Could not retrieve DESITEM field",
                              NITF_CTXT,NITF_ERR_INVALID_OBJECT);
      return(NITF_FAILURE);
    }

    if((strcmp(oflw,type) == 0) && (item > (index+1)))
      if(!nitf_Field_setUint32(subheader->NITF_DESITEM,item-1,error))
        return(0);

    nitf_ListIterator_increment(&deIter);
  }

  return(NITF_SUCCESS);
}

/*
  fixSegmentIndexes reviews and corrects the indexes of all overflowing
  segments (i.e., image segment) when any DE segment is removed. Removing a
  DE segment could effect the indexes of any existing overflowed segments.
  This function is called when any DE segment is remove, not just overflow
  segments. The file header is also checked.

  Note: Adding a DE segment does not effect any existing overflows because new
  segments are added at the end of the corresponding segment list
*/
  
NITFPRIV(NITF_BOOL) fixSegmentIndexes
(
  nitf_Record *record,    /* Record to fix */
  nitf_Uint32 index,      /* DE segment index of removed segment zero based */
  nitf_Error *error       /* For error returns */
)
{
  nitf_FileHeader *header;          /* File header */
  nitf_ListIterator segIter;        /* Current segment list */
  nitf_ListIterator segEnd;         /* Current segment list end */
  nitf_Uint32 deIdx;                /* Desegment index */

/*  File header */

  header = record->header;
  if(!nitf_Field_get(header->NITF_UDHOFL,(NITF_DATA *) &deIdx,
                                  NITF_CONV_UINT,sizeof(deIdx), error))
      return(0);

    if(deIdx > (index+1))
      if(!nitf_Field_setUint32(header->NITF_UDHOFL,deIdx-1,error))
        return(0);

  if(!nitf_Field_get(header->NITF_XHDLOFL,(NITF_DATA *) &deIdx,
                                  NITF_CONV_UINT,sizeof(deIdx), error))
      return(0);

    if(deIdx > (index+1))
      if(!nitf_Field_setUint32(header->NITF_XHDLOFL,deIdx-1,error))
        return(0);

/* Image segments */

  segIter = nitf_List_begin(record->images);
  segEnd = nitf_List_end(record->images);
  while(nitf_ListIterator_notEqualTo(&segIter, &segEnd))
  {
    nitf_ImageSubheader *subheader;    /* Current subheader */

    subheader = (nitf_ImageSubheader *)
        ((nitf_ImageSegment *) nitf_ListIterator_get(&segIter))->subheader;

    if(!nitf_Field_get(subheader->NITF_UDOFL,(NITF_DATA *) &deIdx,
                                  NITF_CONV_UINT,sizeof(deIdx), error))
      return(0);

    if(deIdx > (index+1))
      if(!nitf_Field_setUint32(subheader->NITF_UDOFL,deIdx-1,error))
        return(0);

    if(!nitf_Field_get(subheader->NITF_IXSOFL,(NITF_DATA *) &deIdx,
                                  NITF_CONV_UINT,sizeof(deIdx), error))
      return(0);

    if(deIdx > (index+1))
      if(!nitf_Field_setUint32(subheader->NITF_IXSOFL,deIdx-1,error))
        return(0);

    nitf_ListIterator_increment(&segIter);
  }

/* Graphics segments */

  segIter = nitf_List_begin(record->images);
  segEnd = nitf_List_end(record->images);
  while(nitf_ListIterator_notEqualTo(&segIter, &segEnd))
  {
    nitf_GraphicSubheader *subheader;    /* Current subheader */

    subheader = (nitf_GraphicSubheader *)
        ((nitf_GraphicSegment *) nitf_ListIterator_get(&segIter))->subheader;

    if(!nitf_Field_get(subheader->NITF_SXSOFL,(NITF_DATA *) &deIdx,
                                  NITF_CONV_UINT,sizeof(deIdx), error))
      return(0);

    if(deIdx > (index+1))
      if(!nitf_Field_setUint32(subheader->NITF_SXSOFL,deIdx-1,error))
        return(0);

    nitf_ListIterator_increment(&segIter);
  }

/* Label segments */

  segIter = nitf_List_begin(record->labels);
  segEnd = nitf_List_end(record->labels);
  while(nitf_ListIterator_notEqualTo(&segIter, &segEnd))
  {
    nitf_LabelSubheader *subheader;    /* Current subheader */

    subheader = (nitf_LabelSubheader *)
        ((nitf_LabelSegment *) nitf_ListIterator_get(&segIter))->subheader;

    if(!nitf_Field_get(subheader->NITF_LXSOFL,(NITF_DATA *) &deIdx,
                                  NITF_CONV_UINT,sizeof(deIdx), error))
      return(0);

    if(deIdx > (index+1))
      if(!nitf_Field_setUint32(subheader->NITF_LXSOFL,deIdx-1,error))
        return(0);

    nitf_ListIterator_increment(&segIter);
  }

/* Text segments */

  segIter = nitf_List_begin(record->images);
  segEnd = nitf_List_end(record->images);
  while(nitf_ListIterator_notEqualTo(&segIter, &segEnd))
  {
    nitf_TextSubheader *subheader;    /* Current subheader */

    subheader = (nitf_TextSubheader *)
        ((nitf_GraphicSegment *) nitf_ListIterator_get(&segIter))->subheader;

    if(!nitf_Field_get(subheader->NITF_TXSOFL,(NITF_DATA *) &deIdx,
                                  NITF_CONV_UINT,sizeof(deIdx), error))
      return(0);

    if(deIdx > (index+1))
      if(!nitf_Field_setUint32(subheader->NITF_TXSOFL,deIdx-1,error))
        return(0);
    nitf_ListIterator_increment(&segIter);
  }

  return(NITF_SUCCESS);
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

/*  Update the indexes in any overflow segments */

    if(!fixOverflowIndexes(record,"UDID",segmentNumber,error))
      return(NITF_FAILURE);

    if(!fixOverflowIndexes(record,"IXSHD",segmentNumber,error))
      return(NITF_FAILURE);

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

/*  Update the indexes in any overflow segments */

    if(!fixOverflowIndexes(record,"SXSHD",segmentNumber,error))
      return(NITF_FAILURE);

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

/*  Update the indexes in any overflow segments */

    if(!fixOverflowIndexes(record,"LXSHD",segmentNumber,error))
      return(NITF_FAILURE);

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

/*  Update the indexes in any overflow segments */

    if(!fixOverflowIndexes(record,"TXSHD",segmentNumber,error))
      return(NITF_FAILURE);

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

  /* Fix indexes in other segments with overflows */

    if(!fixSegmentIndexes(record,segmentNumber,error))
      return(NITF_FAILURE);

    return(NITF_SUCCESS);

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

/*
   Ugly macro that is most of the merge logic for a single extension section

   This macro is very localized and assumes several local variables exist and
   are initialized.

   section      - Extended section (i.e., userDefinedSection)
   securityCls - security class field (i.e., imageSecurityClass)
   securityGrp - Security group object (i.e., securityGroup)
   idx         - Index field (DE index in original segment) (i.e.,UDOFL)
   typeStr     - Type string (i.e.,UDID)
*/

#define UNMERGE_SEGMENT(section,securityCls,securityGrp,idx,typeStr) \
    length = nitf_Extensions_computeLength(section,version,error); \
    if(length > maxLength)   /* Overflow */ \
    { \
      if(!nitf_Field_get(idx,&ovfIdx,NITF_CONV_INT,NITF_INT32_SZ, error)) \
      { \
        nitf_Error_init(error,\
            "Could not retrieve overflow segment index", \
                              NITF_CTXT, NITF_ERR_INVALID_OBJECT); \
        return(NITF_FAILURE); \
      } \
      if(ovfIdx == 0) \
      { \
        ovfIdx = addOverflowSegment(record,segIdx,#typeStr, \
             securityCls,securityGrp,&overflow,error); \
        if(ovfIdx == 0) \
        { \
          nitf_Error_init(error, \
            "Could not add overflow segment", \
                              NITF_CTXT, NITF_ERR_INVALID_OBJECT); \
          return(NITF_FAILURE); \
        } \
      } \
      if(!moveTREs(section, \
               overflow->subheader->userDefinedSection,maxLength,error)) \
      { \
        nitf_Error_init(error, \
            "Could not transfer TREs to overflow segment", \
                              NITF_CTXT,NITF_ERR_INVALID_OBJECT); \
        return(NITF_FAILURE); \
      } \
      if(!nitf_Field_setUint32(idx,ovfIdx,error)) \
      { \
        nitf_Error_init(error,\
            "Could not set overflow segment index", \
                              NITF_CTXT, NITF_ERR_INVALID_OBJECT); \
        return(NITF_FAILURE); \
      }\
    } 

NITFAPI(NITF_BOOL) nitf_Record_unmergeTREs
(
    nitf_Record * record,
    nitf_Error * error
)
{
    nitf_Version version; /* NITF version */
    nitf_FileHeader *header; /* File header */
    nitf_ListIterator segIter; /* Current segment list */
    nitf_ListIterator segEnd; /* Current segment list end */
    nitf_Uint32 segIdx; /* Current segment index */
    nitf_Uint32 length; /* Length Of TREs in current section */
    nitf_Uint32 maxLength; /* Max length for this type of section */
    nitf_Uint32 ovfIdx; /* Overflow index of current extension */
    nitf_DESegment *overflow; /* Overflow segment */

    version = nitf_Record_getVersion(record,error);

    /* File header */

    header = record->header;
    UNMERGE_SEGMENT(header->userDefinedSection,
            header->classification,header->securityGroup,header->NITF_UDHOFL,UDHD);

    UNMERGE_SEGMENT(header->extendedSection,
            header->classification,header->securityGroup,header->NITF_XHDLOFL,XHD);

    /* Image segments */

    segIter = nitf_List_begin(record->images);
    segEnd = nitf_List_end(record->images);
    segIdx = 1;
    while(nitf_ListIterator_notEqualTo(&segIter, &segEnd))
    {
        nitf_ImageSubheader *subheader; /* Current subheader */

        maxLength = 99999;

        subheader = (nitf_ImageSubheader *)
                ((nitf_ImageSegment *) nitf_ListIterator_get(&segIter))->subheader;

        /*  User defined section */

        UNMERGE_SEGMENT(subheader->userDefinedSection,subheader->imageSecurityClass,
                subheader->securityGroup,subheader->NITF_UDOFL,UDID);

        /*  Extension section */

        UNMERGE_SEGMENT(subheader->extendedSection,subheader->imageSecurityClass,
                subheader->securityGroup,subheader->NITF_IXSOFL,IXSHD);

        segIdx += 1;
        nitf_ListIterator_increment(&segIter);
    }

    /* Graphics segments */

    segIter = nitf_List_begin(record->graphics);
    segEnd = nitf_List_end(record->graphics);
    segIdx = 1;
    while(nitf_ListIterator_notEqualTo(&segIter, &segEnd))
    {
        nitf_GraphicSubheader *subheader; /* Current subheader */

        maxLength = 9741;

        subheader = (nitf_GraphicSubheader *)
                ((nitf_GraphicSegment *) nitf_ListIterator_get(&segIter))->subheader;

        /*  Extension section */

        UNMERGE_SEGMENT(subheader->extendedSection,subheader->securityClass,
                subheader->securityGroup,subheader->NITF_SXSOFL,SXSHD);

        segIdx += 1;
        nitf_ListIterator_increment(&segIter);
    }

    /* Label segments */

    segIter = nitf_List_begin(record->labels);
    segEnd = nitf_List_end(record->labels);
    segIdx = 1;
    while(nitf_ListIterator_notEqualTo(&segIter, &segEnd))
    {
        nitf_LabelSubheader *subheader; /* Current subheader */

        maxLength = 9747;

        subheader = (nitf_LabelSubheader *)
                ((nitf_LabelSegment *) nitf_ListIterator_get(&segIter))->subheader;

        /*  Extension section */

        UNMERGE_SEGMENT(subheader->extendedSection,subheader->securityClass,
                subheader->securityGroup,subheader->NITF_LXSOFL,LXSHD);

        segIdx += 1;
    }

    /* Text segments */

    segIter = nitf_List_begin(record->texts);
    segEnd = nitf_List_end(record->texts);
    segIdx = 1;
    while(nitf_ListIterator_notEqualTo(&segIter, &segEnd))
    {
        nitf_TextSubheader *subheader; /* Current subheader */

        maxLength = 9717;

        subheader = (nitf_TextSubheader *)
                ((nitf_TextSegment *) nitf_ListIterator_get(&segIter))->subheader;

        /*  Extension section */

        UNMERGE_SEGMENT(subheader->extendedSection,subheader->securityClass,
                subheader->securityGroup,subheader->NITF_TXSOFL,TXSHD);

        segIdx += 1;
        nitf_ListIterator_increment(&segIter);
    }

    return(NITF_SUCCESS);
}

NITFAPI(NITF_BOOL) nitf_Record_mergeTREs
(
    nitf_Record * record,
    nitf_Error * error
)
{
    nitf_ListIterator deIter; /* DE segment list */
    nitf_ListIterator deEnd; /* DE segment list end */
    nitf_Uint32 deIdx; /* Current DE segment index (one based) */
    nitf_Int32 deRemoved; /* Number of DE segments removed */

    deIter = nitf_List_begin(record->dataExtensions);
    deEnd = nitf_List_end(record->dataExtensions);
    deIdx = 1;
    deRemoved = 0;
    while(nitf_ListIterator_notEqualTo(&deIter, &deEnd))
    {
        nitf_DESubheader *subheader; /* Current subheader */
        char desid[NITF_DESTAG_SZ+1];
        nitf_Extensions *destination; /* Owner of overflow */
        nitf_Field *extLength; /* Extended header length field */
        nitf_Field *overflowIdx; /* Overflow length  field */

        subheader = (nitf_DESubheader *)
                ((nitf_DESegment *) nitf_ListIterator_get(&deIter))->subheader;

        if(!nitf_Field_get(subheader->NITF_DESTAG,(NITF_DATA *) desid,
                        NITF_CONV_STRING,NITF_DESTAG_SZ+1, error))
        {
            nitf_Error_init(error,
                    "Could not retrieve DE segment id",
                    NITF_CTXT, NITF_ERR_INVALID_OBJECT);
            return(NITF_FAILURE);
        }
        nitf_Field_trimString(desid);

        if(strcmp(desid,TRE_OVERFLOW_STR) == 0) /* This is an overflow */
        {
            nitf_Uint32 segIdx;
            char type[NITF_DESOFLW_SZ+1];
            NITF_BOOL eflag;

            /* Get type and index */

            eflag = !nitf_Field_get(subheader->NITF_DESOFLW,(NITF_DATA *) type,
                    NITF_CONV_STRING,NITF_DESOFLW_SZ+1,error);
            eflag |= !nitf_Field_get(subheader->NITF_DESITEM,&segIdx,
                    NITF_CONV_INT,NITF_INT32_SZ,error);
            if(eflag)
            {
                nitf_Error_init(error,
                        "Could not retrieve DE segment header overflow value",
                        NITF_CTXT, NITF_ERR_INVALID_OBJECT);
                return(NITF_FAILURE);
            }
            nitf_Field_trimString(type);

            if(strcmp(type,"UDHD") == 0) /* File header user defined */
            {
                extLength = record->header->NITF_UDHDL;
                overflowIdx = record->header->NITF_UDHOFL;
                destination = record->header->userDefinedSection;
            }
            else if(strcmp(type,"XHD") == 0) /* File header extended */
            {
                extLength = record->header->NITF_XHDL;
                overflowIdx = record->header->NITF_XHDLOFL;
                destination = record->header->extendedSection;
            }
            else if((strcmp(type,"UDID") == 0) /* Image segment */
                    || (strcmp(type,"IXSHD") == 0))
            {
                nitf_ImageSegment *imSeg;

                imSeg = nitf_List_get(record->images,segIdx-1,error);
                if(strcmp(type,"UDID") == 0) /* Image segment user defined */
                {
                    extLength = imSeg->subheader->NITF_UDIDL;
                    overflowIdx = imSeg->subheader->NITF_UDOFL;
                    destination = imSeg->subheader->userDefinedSection;
                }
                else
                {
                    extLength = imSeg->subheader->NITF_IXSHDL;
                    overflowIdx = imSeg->subheader->NITF_IXSOFL;
                    destination = imSeg->subheader->extendedSection;
                }
            }
            else if(strcmp(type,"SXSHD") == 0) /* Graphics segment */
            {
                nitf_GraphicSegment *grSeg;

                grSeg = nitf_List_get(record->graphics,segIdx-1,error);
                extLength = grSeg->subheader->NITF_SXSHDL;
                overflowIdx = grSeg->subheader->NITF_SXSOFL;
                destination = grSeg->subheader->extendedSection;
            }
            else if(strcmp(type,"LXSHD") == 0) /* Labels segment */
            {
                nitf_LabelSegment *lbSeg;

                lbSeg = nitf_List_get(record->labels,segIdx-1,error);
                extLength = lbSeg->subheader->NITF_LXSHDL;
                overflowIdx = lbSeg->subheader->NITF_LXSOFL;
                destination = lbSeg->subheader->extendedSection;
            }
            else if(strcmp(type,"TXSHD") == 0) /* Text segment */
            {
                nitf_TextSegment *txSeg;

                txSeg = nitf_List_get(record->texts,segIdx-1,error);
                extLength = txSeg->subheader->NITF_TXSHDL;
                overflowIdx = txSeg->subheader->NITF_TXSOFL;
                destination = txSeg->subheader->extendedSection;
            }
            else
            {
                nitf_Error_init(error,
                        "Invalid overflow segment type or index",
                        NITF_CTXT, NITF_ERR_INVALID_OBJECT);
                return(NITF_FAILURE);
            }

            if(!nitf_Field_setUint32(extLength,0,error))
                return(NITF_FAILURE);
            if(!nitf_Field_setUint32(overflowIdx,0,error))
                return(NITF_FAILURE);
            if(!moveTREs(subheader->userDefinedSection,destination,0,error))
                return(NITF_FAILURE);

            /* Remove the DE segment how does this effect indexes */

            if(!nitf_Record_removeDataExtensionSegment(record,deIdx-1,error))
                return(NITF_FAILURE);
            deIter = nitf_List_begin(record->dataExtensions);
            deIdx = 1;
            deRemoved += 1;
            continue;
        }

        deIdx += 1;
        nitf_ListIterator_increment(&deIter);
    }

    if(deRemoved> 0)
    {
        nitf_Int32 deCount;

        if(!nitf_Field_get(record->header->NITF_NUMDES,&deCount,
                        NITF_CONV_INT,NITF_INT32_SZ,error))
            return(NITF_FAILURE);
        deCount -= deRemoved;
        if(deCount < 0)
            deCount = 0;
        if(!nitf_Field_setUint32(record->header->NITF_NUMDES,deCount,error))
            return(NITF_FAILURE);
    }
    return(NITF_SUCCESS);
}
