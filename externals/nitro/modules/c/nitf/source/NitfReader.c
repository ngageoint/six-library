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
 * License along with this program; if not,
 * see <http://www.gnu.org/licenses/>.
 *
 */

#include <assert.h>
#include <limits.h>

#include "nitf/Reader.h"

/****************************
 *** NOTE ABOUT THE MACROS ***
 *****************************
All of these macros make the assumption that a label
called CATCH_ERROR exists.  This allows macros to
simulate throwing as seen in C++

NITF_TRY_GET_UINT32|64() -- these are public macros found in nitf/Value.h
*/

/*  This macro basically allows exceptions.  It performs  */
/*  error checking for the user, and thus simplifies the  */
/*  code.  It should be used any time you want to read a  */
/*  field, but be careful with it.  You must note the     */
/*  assumptions.                                          */
/*  ASSUMPTIONS:                                          */
/*  1)  A nitf_Reader object named reader exists          */
/*  2)  CATCH_ERROR exists (see above)                    */

#define TRY_READ_FIELD(own_, fld_)                        \
    if (!readField(reader, own_->fld_, fld_##_SZ, error)) \
        goto CATCH_ERROR;

/*  This macro makes it easier to read the user-defined    */
/*  header data section.  The readExtras() method supplies */
/*  the underlying driving call, but it can be generalized */
/*  for this case, and for the extended header components  */
#define TRY_READ_UDHD(reader_)                                  \
    if (!readExtras(reader_,                                    \
                    reader->record->header->userDefinedSection, \
                    reader->record->header->NITF_UDHDL,         \
                    reader->record->header->NITF_UDHOFL,        \
                    error))                                     \
        goto CATCH_ERROR;

/*  This macro makes it easier to read the extended header */
/*  section.  As above, the readExtras() method supplies   */
/*  the underlying driving call, but it can be generalized */
/*  for this case, and for the user header components too  */
#define TRY_READ_XHD(reader_)                                \
    if (!readExtras(reader_,                                 \
                    reader->record->header->extendedSection, \
                    reader->record->header->NITF_XHDL,       \
                    reader->record->header->NITF_XHDLOFL,    \
                    error))                                  \
        goto CATCH_ERROR;

#define TRY_READ_UDID(reader_, imageIndex_, subhdr_) \
    if (!readExtras(reader_,                         \
                    subhdr_->userDefinedSection,     \
                    subhdr_->NITF_UDIDL,             \
                    subhdr_->NITF_UDOFL,             \
                    error))                          \
        goto CATCH_ERROR;

#define TRY_READ_IXSHD(reader_, imageIndex_, subhdr_) \
    if (!readExtras(reader_,                          \
                    subhdr_->extendedSection,         \
                    subhdr_->NITF_IXSHDL,             \
                    subhdr_->NITF_IXSOFL,             \
                    error))                           \
        goto CATCH_ERROR;

#define TRY_READ_SXSHD(reader_, graphicIndex_, subhdr_) \
    if (!readExtras(reader_,                            \
                    subhdr_->extendedSection,           \
                    subhdr_->NITF_SXSHDL,               \
                    subhdr_->NITF_SXSOFL,               \
                    error))                             \
        goto CATCH_ERROR;

#define TRY_READ_LXSHD(reader_, labelIndex_, subhdr_) \
    if (!readExtras(reader_,                          \
                    subhdr_->extendedSection,         \
                    subhdr_->NITF_LXSHDL,             \
                    subhdr_->NITF_LXSOFL,             \
                    error))                           \
        goto CATCH_ERROR;

#define TRY_READ_TXSHD(reader_, textIndex_, subhdr_) \
    if (!readExtras(reader_,                         \
                    subhdr_->extendedSection,        \
                    subhdr_->NITF_TXSHDL,            \
                    subhdr_->NITF_TXSOFL,            \
                    error))                          \
        goto CATCH_ERROR;

/*  These are internal macros which basically provide a         */
/*  mock version of exceptions in C.  They are simple           */
/*  so just read them.  Because they are internal, use          */
/*  them at your own risk.                                      */
/*                                                              */
/*  ASSUMPTIONS:                                                */
/*  1)  An instance of a NITF_BOOL named success exists,        */
/*      and can be overwritten by the macros.                   */
/*  2)  A label is in scope called CATCH_ERROR                  */

#define TRY_READ_VALUE(reader_, field_, length_)     \
    if (!readValue(reader_, field_, length_, error)) \
        goto CATCH_ERROR;

#define TRY_READ_MEMBER_VALUE(reader_, OWNER, ID)       \
    if (!readValue(reader_, OWNER->ID, ID##_SZ, error)) \
        goto CATCH_ERROR;

#define TRY_READ_COMPONENT(                                                  \
        reader_, infoPtrPtr_, numValue_, subHdrSz_, dataSz_)                 \
    if (!readComponentInfo(                                                  \
                reader_, infoPtrPtr_, numValue_, subHdrSz_, dataSz_, error)) \
        goto CATCH_ERROR;

/*  This is the size of each num* (numi, numx, nums, numdes, numres)  */
#define NITF_IVAL_SZ 3

NITFPRIV(nitf_BandInfo**)
readBandInfo(nitf_Reader* reader, unsigned int nbands, nitf_Error* error);

NITFPRIV(NITF_BOOL)
readCorners(nitf_Reader* reader,
            nitf_ImageSubheader* subhdr,
            nitf_Version fver,
            nitf_Error* error);

NITFPRIV(NITF_BOOL)
readTRE(nitf_Reader* reader, nitf_Extensions* ext, nitf_Error* error);

NITFPRIV(NITF_BOOL)
readField(nitf_Reader* reader, char* fld, int length, nitf_Error* error);

NITFPRIV(NITF_BOOL)
readValue(nitf_Reader* reader,
          nitf_Field* field,
          int length,
          nitf_Error* error);
NITFPRIV(NITF_BOOL)
handleTRE(nitf_Reader* reader,
          uint32_t length,
          nitf_TRE* tre,
          nitf_Error* error);

/*  This method reads the extra sections from the header, if    */
/*  there _are_ any.  If not, no big deal.                      */
NITFPRIV(NITF_BOOL)
readExtras(nitf_Reader* reader,
           nitf_Extensions* ext,
           nitf_Field* totalLengthValue,
           nitf_Field* overflowOffsetValue,
           nitf_Error* error);

/*  Reading the component info sections is not a big deal, but it does      */
/*  suit us that we are interested in reuse.  This method may be            */
/*  used for all component info sections, however, we must be handed the    */
/*  offset size, which will require a front-end macro.  They are listed:    */
/*  NITF_READ_IMAGE_INFO()                                                  */
/*  NITF_READ_GRAPHICS_INFO()                                               */
/*  NITF_READ_TEXT_INFO()                                                   */
/*  NITF_READ_LABEL_INFO()                                                  */
/*  NITF_READ_DATA_EXT_INFO()                                               */
/*  NITF_READ_RESERVED_EXT_INFO()                                           */
/*                                                                          */
/*  \param reader The reader object                                         */
/*  \param ioHandle The IO handle                                           */
/*  \param infoPtr A pointer to the componentInfo object to be populated    */
/*  \param num A value representing the number of components that existed   */
/*  \param subHdrSz Size of the field describing subheader length           */
/*  \param dataSz Size of the field describing the data length              */
/*  \param error An error object to be populated on failure                 */
/*  \return NITF_SUCCESS on success and 0 otherwise                         */
NITFPRIV(NITF_BOOL)
readComponentInfo(nitf_Reader* reader,
                  nitf_ComponentInfo*** infoPtrPtr,
                  nitf_Field* numValue,
                  const int subHdrSz,
                  const int dataSz,
                  nitf_Error* error)
{
    int i;
    int numComponents; /* Total number of components */

    /* Read the number of components */
    TRY_READ_VALUE(reader, numValue, NITF_IVAL_SZ);

    /* Make sure it is a positive integer */
    NITF_TRY_GET_UINT32(numValue, &numComponents, error);

    /*  Save ourselves some time if there is nothing in this section  */
    /*  NOTE: this should be the case for the label segment if NITF 2.1 */
    if (!numComponents)
    {
        *infoPtrPtr = NULL;
        return NITF_SUCCESS;
    }

    /*  Malloc enough space for N image info nodes  */
    *infoPtrPtr = (nitf_ComponentInfo**)NITF_MALLOC(
            sizeof(nitf_ComponentInfo*) * numComponents);

    if (!*infoPtrPtr)
    {
        nitf_Error_init(error,
                        NITF_STRERROR(NITF_ERRNO),
                        NITF_CTXT,
                        NITF_ERR_MEMORY);
        return NITF_FAILURE;
    }

    /*  Read the image info  */
    for (i = 0; i < numComponents; i++)
    {
        (*infoPtrPtr)[i] =
                nitf_ComponentInfo_construct(subHdrSz, dataSz, error);
        TRY_READ_VALUE(reader, (*infoPtrPtr)[i]->lengthSubheader, subHdrSz);

        TRY_READ_VALUE(reader, (*infoPtrPtr)[i]->lengthData, dataSz);
    }
    return NITF_SUCCESS;

CATCH_ERROR:
    *infoPtrPtr = NULL;
    return NITF_FAILURE;
}

/*  This is a simple read method.  Note that this is NOT a finalized  */
/*  method.                                         */
NITFPRIV(NITF_BOOL)
readField(nitf_Reader* reader, char* fld, int length, nitf_Error* error)
{
    assert(fld != NULL);

    /* Make sure the field is nulled out  */
    memset(fld, 0, length);

    /* Read from the IO handle */
    if (!nitf_IOInterface_read(reader->input, fld, length, error))
    {
        nitf_Error_init(error,
                        "Unable to read from IO object",
                        NITF_CTXT,
                        NITF_ERR_READING_FROM_FILE);
        goto CATCH_ERROR;
    }

    return NITF_SUCCESS;

CATCH_ERROR:
    return NITF_FAILURE;
}

NITFPRIV(NITF_BOOL)
readValue(nitf_Reader* reader, nitf_Field* field, int length, nitf_Error* error)
{
    char* buf = (char*)NITF_MALLOC(length);
    if (!buf)
    {
        nitf_Error_init(error,
                        NITF_STRERROR(NITF_ERRNO),
                        NITF_CTXT,
                        NITF_ERR_MEMORY);
        goto CATCH_ERROR;
    }

    if (!readField(reader, buf, length, error))
        goto CATCH_ERROR;

    /* first, check to see if we need to swap bytes */
    if (field->type == NITF_BINARY)
    {
        if (length == NITF_INT16_SZ)
        {
            int16_t int16 = (int16_t)NITF_NTOHS(*((int16_t*)buf));
            if (!nitf_Field_setRawData(
                        field, (NITF_DATA*)&int16, length, error))
                goto CATCH_ERROR;
        }
        else if (length == NITF_INT32_SZ)
        {
            int32_t int32 = (int32_t)NITF_NTOHL(*((int32_t*)buf));
            if (!nitf_Field_setRawData(
                        field, (NITF_DATA*)&int32, length, error))
                goto CATCH_ERROR;
        }
        else
        {
            /* TODO what to do??? 8 bit is ok, but what about 64? */
            if (!nitf_Field_setRawData(field, (NITF_DATA*)buf, length, error))
                goto CATCH_ERROR;
        }
    }
    else
    {
        if (!nitf_Field_setRawData(field, (NITF_DATA*)buf, length, error))
            goto CATCH_ERROR;
    }

    NITF_FREE(buf);
    return NITF_SUCCESS;

CATCH_ERROR:
    if (buf)
        NITF_FREE(buf);
    return NITF_FAILURE;
}

/* This function reads the whole security section from a v2.0 (sub)header */
NITFPRIV(NITF_BOOL)
read20FileSecurity(nitf_Reader* reader,
                   nitf_FileSecurity* securityGroup,
                   nitf_Error* error)
{
    /* NOTE: The sizes are different from those of 2.1 */
    nitf_FileSecurity_resizeForVersion(securityGroup, NITF_VER_20, error);

    TRY_READ_VALUE(reader, securityGroup->NITF_CODE, NITF_CODE_20_SZ);
    TRY_READ_VALUE(reader, securityGroup->NITF_CTLH, NITF_CTLH_20_SZ);
    TRY_READ_VALUE(reader, securityGroup->NITF_REL, NITF_REL_20_SZ);
    TRY_READ_VALUE(reader, securityGroup->NITF_CAUT, NITF_CAUT_20_SZ);
    TRY_READ_VALUE(reader, securityGroup->NITF_CTLN, NITF_CTLN_20_SZ);
    TRY_READ_VALUE(reader, securityGroup->NITF_DGDT, NITF_DGDT_20_SZ);
    /* !!! fix century on date? */

    if (!strncmp(securityGroup->NITF_DGDT->raw, "999998", 6))
    {
        TRY_READ_VALUE(reader, securityGroup->NITF_CLTX, NITF_CLTX_20_SZ);
        /* !!! fix century on date? */
    }

    return NITF_SUCCESS;

CATCH_ERROR:
    return NITF_FAILURE;
}

/*  This function reads the whole file security section     */
/*  using the NITF_READ_FIELD() macro.  The macro takes  */
/*  advantage of the naming conventions to read the         */
/*  data with very little effort in calling.  If you        */
/*  dont like this kind of abstraction, you are welcome     */
/*  to be more rigorous and expand out the macro.           */
NITFPRIV(NITF_BOOL)
read21FileSecurity(nitf_Reader* reader,
                   nitf_FileSecurity* securityGroup,
                   nitf_Error* error)
{
    TRY_READ_MEMBER_VALUE(reader, securityGroup, NITF_CLSY);
    TRY_READ_MEMBER_VALUE(reader, securityGroup, NITF_CODE);
    TRY_READ_MEMBER_VALUE(reader, securityGroup, NITF_CTLH);
    TRY_READ_MEMBER_VALUE(reader, securityGroup, NITF_REL);
    TRY_READ_MEMBER_VALUE(reader, securityGroup, NITF_DCTP);
    TRY_READ_MEMBER_VALUE(reader, securityGroup, NITF_DCDT);
    TRY_READ_MEMBER_VALUE(reader, securityGroup, NITF_DCXM);
    TRY_READ_MEMBER_VALUE(reader, securityGroup, NITF_DG);
    TRY_READ_MEMBER_VALUE(reader, securityGroup, NITF_DGDT);
    TRY_READ_MEMBER_VALUE(reader, securityGroup, NITF_CLTX);
    TRY_READ_MEMBER_VALUE(reader, securityGroup, NITF_CATP);
    TRY_READ_MEMBER_VALUE(reader, securityGroup, NITF_CAUT);
    TRY_READ_MEMBER_VALUE(reader, securityGroup, NITF_CRSN);
    TRY_READ_MEMBER_VALUE(reader, securityGroup, NITF_RDT);
    TRY_READ_MEMBER_VALUE(reader, securityGroup, NITF_CTLN);

    return NITF_SUCCESS;

CATCH_ERROR:
    return NITF_FAILURE;
}

/* This function reads a security section */
NITFPRIV(NITF_BOOL)
readFileSecurity(nitf_Reader* reader,
                 nitf_Version fver,
                 nitf_FileSecurity* securityGroup,
                 nitf_Error* error)
{
    if (IS_NITF20(fver))
    {
        if (!read20FileSecurity(reader, securityGroup, error))
            goto CATCH_ERROR;
    }
    else if (IS_NITF21(fver))
    {
        if (!read21FileSecurity(reader, securityGroup, error))
            goto CATCH_ERROR;
    }
    else
    {
        /* Invalid NITF Version (We had better never get here) */
        nitf_Error_init(error,
                        "Invalid NITF Version",
                        NITF_CTXT,
                        NITF_ERR_INVALID_FILE);
        goto CATCH_ERROR;
    }

    return NITF_SUCCESS;

CATCH_ERROR:
    return NITF_FAILURE;
}

NITFPRIV(void) resetIOInterface(nitf_Reader* reader)
{
    if (reader->input && reader->ownInput)
        nitf_IOInterface_destruct(&(reader->input));
    reader->input = NULL;
    reader->ownInput = 0;
}

NITFAPI(nitf_Reader*) nitf_Reader_construct(nitf_Error* error)
{
    /*  Create the reader */
    nitf_Reader* reader = (nitf_Reader*)NITF_MALLOC(sizeof(nitf_Reader));

    /*  If we have problems, populate the error object  */
    if (!reader)
    {
        nitf_Error_init(error,
                        NITF_STRERROR(NITF_ERRNO),
                        NITF_CTXT,
                        NITF_ERR_MEMORY);
        return NULL;
    }

    reader->warningList = nitf_List_construct(error);
    if (!reader->warningList)
    {
        nitf_Reader_destruct(&reader);
        return NULL;
    }

    reader->record = NULL;
    reader->input = NULL;
    reader->ownInput = 0;
    resetIOInterface(reader);

    /*  Return our results  */
    return reader;
}

NITFAPI(void) nitf_Reader_destruct(nitf_Reader** reader)
{
    /*  If the reader has already been destructed, or was never  */
    /*  Inited, dont dump core                                   */
    if (*reader)
    {
        nitf_ListIterator iter;
        nitf_ListIterator end;
        if ((*reader)->warningList)
        {
            iter = nitf_List_begin((*reader)->warningList);
            end = nitf_List_end((*reader)->warningList);

            while (nitf_ListIterator_notEqualTo(&iter, &end))
            {
                nitf_FieldWarning* warning =
                        (nitf_FieldWarning*)nitf_ListIterator_get(&iter);
                nitf_FieldWarning_destruct(&warning);
                nitf_List_remove((*reader)->warningList, &iter);
            }
            nitf_List_destruct(&(*reader)->warningList);
        }
        /*
           if ((*reader)->imageReaderPool)
           {
           iter = nitf_List_begin( (*reader)->imageReaderPool );
           end  = nitf_List_end( (*reader)->imageReaderPool );

           while (nitf_ListIterator_notEqualTo(&iter, &end))
           {
           nitf_ImageReader* imageReader =
           (nitf_ImageReader*)nitf_ListIterator_get(&iter);

           nitf_ImageReader_destruct(& imageReader );
           nitf_ListIterator_increment(&iter);
           }

           nitf_List_destruct( & (*reader)->imageReaderPool );

           }
         */

        /* this will delete the input if we own it */
        resetIOInterface(*reader);

        (*reader)->warningList = NULL;
        (*reader)->record = NULL;

        NITF_FREE(*reader);
        *reader = NULL;
    }
}

NITFPRIV(NITF_BOOL)
readImageSubheader(nitf_Reader* reader,
                   unsigned int imageIndex,
                   nitf_Version fver,
                   nitf_Error* error)
{
    unsigned int i;
    nitf_ImageSubheader* subhdr;
    nitf_FileHeader* hdr = reader->record->header;
    /* List iterator pointing to the image segment */
    nitf_ListIterator listIter = nitf_List_begin(reader->record->images);

    uint32_t numComments; /* Number of comment fields */
    uint32_t nbands; /* An integer representing the \nbands field */
    uint32_t xbands; /* An integer representing the xbands field */
    nitf_Off subheaderStart; /* Start position of image subheader */
    nitf_Off subheaderEnd; /* End position of image subheader */
    nitf_Off expectedSubheaderLength = 0; /* What the file header says the
                                         subheader length ought to be. */

    for (i = 0; i < imageIndex; i++)
        nitf_ListIterator_increment(&listIter);

    /* image sub-header object */
    subhdr = ((nitf_ImageSegment*)nitf_ListIterator_get(&listIter))->subheader;

    subheaderStart = nitf_IOInterface_tell(reader->input, error);

    /* If this isn't IM, is there something we can do? */
    TRY_READ_MEMBER_VALUE(reader, subhdr, NITF_IM);
    TRY_READ_MEMBER_VALUE(reader, subhdr, NITF_IID1);
    TRY_READ_MEMBER_VALUE(reader, subhdr, NITF_IDATIM);
    TRY_READ_MEMBER_VALUE(reader, subhdr, NITF_TGTID);
    TRY_READ_MEMBER_VALUE(reader, subhdr, NITF_IID2);
    TRY_READ_MEMBER_VALUE(reader, subhdr, NITF_ISCLAS);

    /* Read the security group */
    if (!readFileSecurity(reader, fver, subhdr->securityGroup, error))
        goto CATCH_ERROR;

    TRY_READ_MEMBER_VALUE(reader, subhdr, NITF_ENCRYP);
    TRY_READ_MEMBER_VALUE(reader, subhdr, NITF_ISORCE);
    TRY_READ_MEMBER_VALUE(reader, subhdr, NITF_NROWS);
    TRY_READ_MEMBER_VALUE(reader, subhdr, NITF_NCOLS);
    TRY_READ_MEMBER_VALUE(reader, subhdr, NITF_PVTYPE);
    TRY_READ_MEMBER_VALUE(reader, subhdr, NITF_IREP);
    TRY_READ_MEMBER_VALUE(reader, subhdr, NITF_ICAT);
    TRY_READ_MEMBER_VALUE(reader, subhdr, NITF_ABPP);
    TRY_READ_MEMBER_VALUE(reader, subhdr, NITF_PJUST);

    /* Read the ICORDS and IGEOLO segments  */
    if (!readCorners(reader, subhdr, fver, error))
        goto CATCH_ERROR;

    /*  Figure out how many comments we have  */
    TRY_READ_MEMBER_VALUE(reader, subhdr, NITF_NICOM);
    NITF_TRY_GET_UINT32(subhdr->numImageComments, &numComments, error);
    for (i = 0; i < numComments; i++)
    {
        nitf_Field* commentField =
                nitf_Field_construct(NITF_ICOM_SZ, NITF_BCS_A, error);
        if (!commentField)
            goto CATCH_ERROR;
        TRY_READ_VALUE(reader, commentField, NITF_ICOM_SZ);
        nitf_List_pushBack(subhdr->imageComments, commentField, error);
    }
    TRY_READ_MEMBER_VALUE(reader, subhdr, NITF_IC);

    if (strncmp(subhdr->NITF_IC->raw, "NC", 2) != 0 &&
        strncmp(subhdr->NITF_IC->raw, "NM", 2) != 0)
    {
        TRY_READ_MEMBER_VALUE(reader, subhdr, NITF_COMRAT);
    }
    /*  Figure out how many bands  */
    TRY_READ_MEMBER_VALUE(reader, subhdr, NITF_NBANDS);
    xbands = 0;

    /*  In NITF versions before 2.1, the number of bands was a single
       digit 1-9, because the designers were not thinking of
       multi/hyperspectral images.  So the multispectral folk put a 0
       in numbands and put the real number of bands in a BANDSA SDE.
       So, version 2.1 legalized this hack, but put in a larger field
       (conditionally) so the actual number of bands could be put in
       the image subheader, where it belongs.  Sheesh!
     */
    /* !!! note we do not handle the ver2.0 case where true nbands only
       in BANDSA */
    NITF_TRY_GET_UINT32(subhdr->numImageBands, &nbands, error);
    if ((!nbands) && IS_NITF21(fver))
    {
        TRY_READ_MEMBER_VALUE(reader, subhdr, NITF_XBANDS);
        NITF_TRY_GET_UINT32(subhdr->numMultispectralImageBands, &xbands, error);
    }
    nbands = nbands + xbands;
    subhdr->bandInfo = readBandInfo(reader, (unsigned int)nbands, error);

    if (!subhdr->bandInfo)
    {
        /* Error, BandInfo is null */
        goto CATCH_ERROR;
    }

    /*  Afer we have read the band info, things are cool again for a bit  */
    TRY_READ_MEMBER_VALUE(reader, subhdr, NITF_ISYNC);
    TRY_READ_MEMBER_VALUE(reader, subhdr, NITF_IMODE);
    TRY_READ_MEMBER_VALUE(reader, subhdr, NITF_NBPR);
    TRY_READ_MEMBER_VALUE(reader, subhdr, NITF_NBPC);
    TRY_READ_MEMBER_VALUE(reader, subhdr, NITF_NPPBH);
    TRY_READ_MEMBER_VALUE(reader, subhdr, NITF_NPPBV);
    TRY_READ_MEMBER_VALUE(reader, subhdr, NITF_NBPP);
    TRY_READ_MEMBER_VALUE(reader, subhdr, NITF_IDLVL);
    TRY_READ_MEMBER_VALUE(reader, subhdr, NITF_IALVL);
    TRY_READ_MEMBER_VALUE(reader, subhdr, NITF_ILOC);
    TRY_READ_MEMBER_VALUE(reader, subhdr, NITF_IMAG);

    /* Read the user defined image data */
    TRY_READ_UDID(reader, imageIndex, subhdr);
    /* Read the extended header info section */
    TRY_READ_IXSHD(reader, imageIndex, subhdr);

    subheaderEnd = nitf_IOInterface_tell(reader->input, error);
    NITF_TRY_GET_UINT32(hdr->NITF_LISH(imageIndex),
                        &expectedSubheaderLength,
                        error);

    if (subheaderEnd - subheaderStart != expectedSubheaderLength)
    {
        nitf_Error_initf(error,
                         NITF_CTXT,
                         NITF_ERR_READING_FROM_FILE,
                         "Image subheader %u expected to have length %ju, "
                         "but read %ju bytes",
                         imageIndex,
                         (size_t)expectedSubheaderLength,
                         (size_t)(subheaderEnd - subheaderStart));
        goto CATCH_ERROR;
    }

    return NITF_SUCCESS;

CATCH_ERROR:

    return NITF_FAILURE;
}

NITFPRIV(NITF_BOOL)
readGraphicSubheader(nitf_Reader* reader,
                     int graphicIndex,
                     nitf_Version fver,
                     nitf_Error* error)
{
    int i;
    nitf_GraphicSubheader* subhdr;

    /* List iterator pointing to the graphics segment */
    nitf_ListIterator listIter = nitf_List_begin(reader->record->graphics);
    for (i = 0; i < graphicIndex; i++)
        nitf_ListIterator_increment(&listIter);

    /* graphics sub-header object */
    subhdr =
            ((nitf_GraphicSegment*)nitf_ListIterator_get(&listIter))->subheader;

    TRY_READ_MEMBER_VALUE(reader, subhdr, NITF_SY);
    TRY_READ_MEMBER_VALUE(reader, subhdr, NITF_SID);
    TRY_READ_MEMBER_VALUE(reader, subhdr, NITF_SNAME);
    TRY_READ_MEMBER_VALUE(reader, subhdr, NITF_SSCLAS);

    /* Read the security section */
    if (!readFileSecurity(reader, fver, subhdr->securityGroup, error))
        goto CATCH_ERROR;

    TRY_READ_MEMBER_VALUE(reader, subhdr, NITF_ENCRYP);
    TRY_READ_MEMBER_VALUE(reader, subhdr, NITF_SFMT);
    TRY_READ_MEMBER_VALUE(reader, subhdr, NITF_SSTRUCT);
    TRY_READ_MEMBER_VALUE(reader, subhdr, NITF_SDLVL);
    TRY_READ_MEMBER_VALUE(reader, subhdr, NITF_SALVL);
    TRY_READ_MEMBER_VALUE(reader, subhdr, NITF_SLOC);
    TRY_READ_MEMBER_VALUE(reader, subhdr, NITF_SBND1);
    TRY_READ_MEMBER_VALUE(reader, subhdr, NITF_SCOLOR);
    TRY_READ_MEMBER_VALUE(reader, subhdr, NITF_SBND2);
    TRY_READ_MEMBER_VALUE(reader, subhdr, NITF_SRES2);

    /* Read the extended header info section */
    TRY_READ_SXSHD(reader, graphicIndex, subhdr);

    return NITF_SUCCESS;

CATCH_ERROR:

    return NITF_FAILURE;
}

NITFPRIV(NITF_BOOL)
readLabelSubheader(nitf_Reader* reader,
                   int labelIndex,
                   nitf_Version fver,
                   nitf_Error* error)
{
    int i;
    nitf_LabelSubheader* subhdr;

    /* List iterator pointing to the label segment */
    nitf_ListIterator listIter = nitf_List_begin(reader->record->labels);
    for (i = 0; i < labelIndex; i++)
        nitf_ListIterator_increment(&listIter);

    /* label sub-header object */
    subhdr = ((nitf_LabelSegment*)nitf_ListIterator_get(&listIter))->subheader;

    /* uint32_t lxshdl, lxsofl; */
    TRY_READ_MEMBER_VALUE(reader, subhdr, NITF_LA);
    TRY_READ_MEMBER_VALUE(reader, subhdr, NITF_LID);
    TRY_READ_MEMBER_VALUE(reader, subhdr, NITF_LSCLAS);

    /* Read the security group */
    if (!readFileSecurity(reader, fver, subhdr->securityGroup, error))
        goto CATCH_ERROR;

    TRY_READ_MEMBER_VALUE(reader, subhdr, NITF_ENCRYP);
    TRY_READ_MEMBER_VALUE(reader, subhdr, NITF_LFS);
    TRY_READ_MEMBER_VALUE(reader, subhdr, NITF_LCW);
    TRY_READ_MEMBER_VALUE(reader, subhdr, NITF_LCH);
    TRY_READ_MEMBER_VALUE(reader, subhdr, NITF_LDLVL);
    TRY_READ_MEMBER_VALUE(reader, subhdr, NITF_LALVL);
    TRY_READ_MEMBER_VALUE(reader, subhdr, NITF_LLOCR);
    TRY_READ_MEMBER_VALUE(reader, subhdr, NITF_LLOCC);
    TRY_READ_MEMBER_VALUE(reader, subhdr, NITF_LTC);
    TRY_READ_MEMBER_VALUE(reader, subhdr, NITF_LBC);

    /* Read the extended header info section */
    TRY_READ_LXSHD(reader, labelIndex, subhdr);

    return NITF_SUCCESS;

CATCH_ERROR:

    return NITF_FAILURE;
}

NITFPRIV(NITF_BOOL)
readTextSubheader(nitf_Reader* reader,
                  int textIndex,
                  nitf_Version fver,
                  nitf_Error* error)
{
    int i;
    nitf_TextSubheader* subhdr;

    /* List iterator pointing to the text segment */
    nitf_ListIterator listIter = nitf_List_begin(reader->record->texts);
    for (i = 0; i < textIndex; i++)
        nitf_ListIterator_increment(&listIter);

    /* text sub-header object */
    subhdr = ((nitf_TextSegment*)nitf_ListIterator_get(&listIter))->subheader;

    TRY_READ_MEMBER_VALUE(reader, subhdr, NITF_TE);
    TRY_READ_MEMBER_VALUE(reader, subhdr, NITF_TEXTID);
    TRY_READ_MEMBER_VALUE(reader, subhdr, NITF_TXTALVL);
    TRY_READ_MEMBER_VALUE(reader, subhdr, NITF_TXTDT);
    TRY_READ_MEMBER_VALUE(reader, subhdr, NITF_TXTITL);
    TRY_READ_MEMBER_VALUE(reader, subhdr, NITF_TSCLAS);

    /* Read the security group */
    if (!readFileSecurity(reader, fver, subhdr->securityGroup, error))
        goto CATCH_ERROR;

    TRY_READ_MEMBER_VALUE(reader, subhdr, NITF_ENCRYP);
    TRY_READ_MEMBER_VALUE(reader, subhdr, NITF_TXTFMT);

    /* Read the extended header info section */
    TRY_READ_TXSHD(reader, textIndex, subhdr);

    return NITF_SUCCESS;

CATCH_ERROR:

    return NITF_FAILURE;
}

NITFPRIV(NITF_BOOL)
readDESubheader(nitf_Reader* reader,
                int desIndex,
                nitf_Version fver,
                nitf_Error* error)
{
    int i;
    nitf_DESegment* segment;
    nitf_DESubheader* subhdr;
    nitf_FileHeader* hdr;
    /* Length of the sub-header */
    uint32_t subLen;
    /* Position where this DE Subheader begins */
    nitf_Off subheaderStart;
    /* End position of DE Subheader */
    nitf_Off subheaderEnd;
    /* What the header says the length ought to be */
    uint32_t expectedSubheaderLength = 0;
    char desID[NITF_DESTAG_SZ + 1]; /* DES ID string */

    nitf_ListIterator listIter =
            nitf_List_begin(reader->record->dataExtensions);

    for (i = 0; i < desIndex; i++)
        nitf_ListIterator_increment(&listIter);

    /* get the correct objects */
    segment = (nitf_DESegment*)nitf_ListIterator_get(&listIter);
    subhdr = segment->subheader;
    hdr = reader->record->header;
    subheaderStart = nitf_IOInterface_tell(reader->input, error);

    TRY_READ_MEMBER_VALUE(reader, subhdr, NITF_DE);
    TRY_READ_MEMBER_VALUE(reader, subhdr, NITF_DESTAG);
    TRY_READ_MEMBER_VALUE(reader, subhdr, NITF_DESVER);
    TRY_READ_MEMBER_VALUE(reader, subhdr, NITF_DESCLAS);

    /* Read the security group */
    if (!readFileSecurity(reader, fver, subhdr->securityGroup, error))
        goto CATCH_ERROR;

    /* get the DESID and trim it */
    nitf_Field_get(subhdr->NITF_DESTAG,
                   desID,
                   NITF_CONV_STRING,
                   NITF_DESTAG_SZ + 1,
                   error);
    nitf_Field_trimString(desID);

    /* read the two conditional fields if TRE_OVERFLOW */
    if ((IS_NITF20(fver) &&
         ((strcmp(desID, "Registered Extensions") == 0) ||
          (strcmp(desID, "Controlled Extensions") == 0))) ||
        (IS_NITF21(fver) && strcmp(desID, "TRE_OVERFLOW") == 0))
    {
        TRY_READ_MEMBER_VALUE(reader, subhdr, NITF_DESOFLW);
        TRY_READ_MEMBER_VALUE(reader, subhdr, NITF_DESITEM);
    }

    /* get the subheaderfields length */
    TRY_READ_MEMBER_VALUE(reader, subhdr, NITF_DESSHL);

    /* Verify that it is a UINT */
    NITF_TRY_GET_UINT32(subhdr->NITF_DESSHL, &subLen, error);
    if (subLen > 0)
    {
        /*  We know our constraints, so build up the tre object  */
        subhdr->subheaderFields = nitf_TRE_createSkeleton(desID, error);

        if (!subhdr->subheaderFields)
            goto CATCH_ERROR;

        if (!handleTRE(reader, subLen, subhdr->subheaderFields, error))
            goto CATCH_ERROR;
    }

    NITF_TRY_GET_UINT64(
            reader->record->header->dataExtensionInfo[desIndex]->lengthData,
            &subhdr->dataLength,
            error);

    /* set the offset and end of the segment */
    const nitf_Off offset = nitf_IOInterface_tell(reader->input, error);
    if (!NITF_IO_SUCCESS(offset))
        goto CATCH_ERROR;
    segment->offset = offset;
    segment->end = segment->offset + subhdr->dataLength;
    subheaderEnd = segment->offset;

    /* see if we need to read the data now as part of a TRE */
    if ((strcmp(desID, "TRE_OVERFLOW") == 0) ||
        (strcmp(desID, "Controlled Extensions") == 0))
    {
        uint64_t currentOffset = segment->offset;

        /* loop until we are done */
        while (currentOffset < segment->end)
        {
            /* read a TRE */
            if (!readTRE(reader, subhdr->userDefinedSection, error))
                goto CATCH_ERROR;

            /* update the offset */
            const nitf_Off currentOffset_ = nitf_IOInterface_tell(reader->input, error);
            if (!NITF_IO_SUCCESS(currentOffset_))
                goto CATCH_ERROR;
            currentOffset = currentOffset_;
        }
    }
    else
    {
        /* seek past the data for now */
        if (!NITF_IO_SUCCESS(nitf_IOInterface_seek(
                    reader->input, segment->end, NITF_SEEK_SET, error)))
        {
            goto CATCH_ERROR;
        }
    }

    NITF_TRY_GET_UINT32(hdr->NITF_LDSH(desIndex),
                        &expectedSubheaderLength,
                        error);

    if (subheaderEnd - subheaderStart != expectedSubheaderLength)
    {
        nitf_Error_initf(error,
                         NITF_CTXT,
                         NITF_ERR_READING_FROM_FILE,
                         "DE subheader %d expected to have length %u, "
                         "but read %ju bytes",
                         desIndex,
                         expectedSubheaderLength,
                         (size_t)(subheaderEnd - subheaderStart));
        goto CATCH_ERROR;
    }
    return NITF_SUCCESS;

CATCH_ERROR:
    return NITF_FAILURE;
}

NITFPRIV(NITF_BOOL)
readRESubheader(nitf_Reader* reader,
                int resIndex,
                nitf_Version fver,
                nitf_Error* error)
{
    int i;
    nitf_RESegment* segment;
    nitf_RESubheader* subhdr;
    uint32_t subLen;
    /* List iterator pointing to the reserved extension segment */
    nitf_ListIterator listIter =
            nitf_List_begin(reader->record->reservedExtensions);

    for (i = 0; i < resIndex; i++)
        nitf_ListIterator_increment(&listIter);

    /* RE  objects */
    segment = (nitf_RESegment*)nitf_ListIterator_get(&listIter);
    subhdr = segment->subheader;

    TRY_READ_MEMBER_VALUE(reader, subhdr, NITF_RE);
    TRY_READ_MEMBER_VALUE(reader, subhdr, NITF_RESTAG);
    TRY_READ_MEMBER_VALUE(reader, subhdr, NITF_RESVER);
    TRY_READ_MEMBER_VALUE(reader, subhdr, NITF_RESCLAS);

    /* Read the security group */
    if (!readFileSecurity(reader, fver, subhdr->securityGroup, error))
        goto CATCH_ERROR;

    /* get the subheader fields length */
    TRY_READ_MEMBER_VALUE(reader, subhdr, NITF_RESSHL);
    NITF_TRY_GET_UINT32(subhdr->subheaderFieldsLength, &subLen, error);
    if (subLen > 0)
    {
        if (subLen >= UINT32_MAX)
        {
            nitf_Error_init(error, "uint32_t+1 overflow", NITF_CTXT, NITF_ERR_MEMORY);
            goto CATCH_ERROR;
        }
        subhdr->subheaderFields = NITF_MALLOC(subLen + 1);
        if (!subhdr->subheaderFields)
            goto CATCH_ERROR;

        /* for now, we just read it into a buffer... change in the future */
        if (!readField(reader, subhdr->subheaderFields, subLen, error))
            goto CATCH_ERROR;
    }

    /* now, we will just zoom past the data, if any */
    NITF_TRY_GET_UINT64(
            reader->record->header->reservedExtensionInfo[resIndex]->lengthData,
            &subhdr->dataLength,
            error);

    /* set the offset and end of the segment */
    const nitf_Off offset = nitf_IOInterface_tell(reader->input, error);
    if (!NITF_IO_SUCCESS(offset))
        goto CATCH_ERROR;
    segment->offset = offset;
    segment->end = segment->offset + subhdr->dataLength;

    return NITF_SUCCESS;

CATCH_ERROR:

    return NITF_FAILURE;
}

/*  This function reads the whole file header section    */
/*  using the READ_HDR_FIELD() macro.  The macro takes   */
/*  advantage of the naming conventions to read the      */
/*  data with very little effort in calling.  If you     */
/*  dont like this kind of abstraction, you are welcome  */
/*  to be more rigorous and expand out the macro.        */
/*                                                       */
/*  It also then reads the sections beneath, one by one. */
/*  HELP!!!!! Someone beef up these comments, and get the .h file up to snuff */
NITFPRIV(NITF_BOOL) readHeader(nitf_Reader* reader, nitf_Error* error)
{
    /* Pointer to the fileHeade, simplifies futher references to this object */
    nitf_FileHeader* fileHeader = reader->record->header;

    /* generic uint32 */
    uint32_t num32;

    /* Number of bytes consumed when reading header */
    uint32_t actualHeaderLength;

    /* What the header says its lenght ought to be */
    uint32_t expectedHeaderLength = 0;

    nitf_Version fver;
    /*uint32_t udhdl, udhofl, xhdl, xhdlofl; */

    char fileLenBuf[NITF_FL_SZ + 1]; /* File length buffer */
    char streamingBuf[NITF_FL_SZ];

    /* FHDR */
    TRY_READ_MEMBER_VALUE(reader, fileHeader, NITF_FHDR);
    if ((strncmp(fileHeader->NITF_FHDR->raw, "NITF", 4) != 0) &&
        (strncmp(fileHeader->NITF_FHDR->raw, "NSIF", 4) != 0))
    {
        nitf_Error_init(error,
                        "File does not appear to be NITF",
                        NITF_CTXT,
                        NITF_ERR_INVALID_FILE);
        goto CATCH_ERROR;
    }

    /* FVER */
    TRY_READ_MEMBER_VALUE(reader, fileHeader, NITF_FVER);

    fver = nitf_Record_getVersion(reader->record);
    if (!IS_NITF20(fver) && !IS_NITF21(fver))
    {
        nitf_Error_init(error,
                        "Unknown NITF version",
                        NITF_CTXT,
                        NITF_ERR_PARSING_FILE);
        goto CATCH_ERROR;
    }

    TRY_READ_MEMBER_VALUE(reader, fileHeader, NITF_CLEVEL);
    TRY_READ_MEMBER_VALUE(reader, fileHeader, NITF_STYPE);
    TRY_READ_MEMBER_VALUE(reader, fileHeader, NITF_OSTAID);
    TRY_READ_MEMBER_VALUE(reader, fileHeader, NITF_FDT);
    TRY_READ_MEMBER_VALUE(reader, fileHeader, NITF_FTITLE);
    TRY_READ_MEMBER_VALUE(reader, fileHeader, NITF_FSCLAS);

    if (!readFileSecurity(reader, fver, fileHeader->securityGroup, error))
        goto CATCH_ERROR;

    TRY_READ_MEMBER_VALUE(reader, fileHeader, NITF_FSCOP);
    TRY_READ_MEMBER_VALUE(reader, fileHeader, NITF_FSCPYS);
    TRY_READ_MEMBER_VALUE(reader, fileHeader, NITF_ENCRYP);

    /* In 2.0 there wasn't a FBKGC field, so we resize ONAME and read */
    if (IS_NITF20(fver))
    {
        if (!nitf_Field_resetLength(fileHeader->NITF_ONAME,
                                    NITF_FBKGC_SZ + NITF_ONAME_SZ,
                                    0,
                                    error))
            goto CATCH_ERROR;

        if (!readValue(reader,
                       fileHeader->NITF_ONAME,
                       NITF_FBKGC_SZ + NITF_ONAME_SZ,
                       error))
            goto CATCH_ERROR;
    }
    else
    {
        TRY_READ_MEMBER_VALUE(reader, fileHeader, NITF_FBKGC);
        TRY_READ_MEMBER_VALUE(reader, fileHeader, NITF_ONAME);
    }
    TRY_READ_MEMBER_VALUE(reader, fileHeader, NITF_OPHONE);

    /* FL */
    TRY_READ_MEMBER_VALUE(reader, fileHeader, NITF_FL);
    /* Check for streaming header (Length is all 9's) */
    memset(streamingBuf, '9', NITF_FL_SZ);

    nitf_Field_get(fileHeader->NITF_FL,
                   fileLenBuf,
                   NITF_CONV_STRING,
                   NITF_FL_SZ + 1,
                   error);
    if (strncmp(fileLenBuf, streamingBuf, NITF_FL_SZ) == 0)
    {
        nitf_Error_init(error,
                        "Streaming headers are not supported",
                        NITF_CTXT,
                        NITF_ERR_PARSING_FILE);
        goto CATCH_ERROR;
    }

    /* HL */
    TRY_READ_MEMBER_VALUE(reader, fileHeader, NITF_HL);
    NITF_TRY_GET_UINT32(fileHeader->NITF_HL, &num32, error);

    /* Read the image info section */
    TRY_READ_COMPONENT(reader,
                       &fileHeader->imageInfo,
                       fileHeader->NITF_NUMI,
                       NITF_LISH_SZ,
                       NITF_LI_SZ);

    /* Read the graphic info section */
    TRY_READ_COMPONENT(reader,
                       &fileHeader->graphicInfo,
                       fileHeader->NITF_NUMS,
                       NITF_LSSH_SZ,
                       NITF_LS_SZ);

    /* Read the label info section */
    TRY_READ_COMPONENT(reader,
                       &fileHeader->labelInfo,
                       fileHeader->NITF_NUMX,
                       NITF_LLSH_SZ,
                       NITF_LL_SZ);

    /* Read the text info section */
    TRY_READ_COMPONENT(reader,
                       &fileHeader->textInfo,
                       fileHeader->NITF_NUMT,
                       NITF_LTSH_SZ,
                       NITF_LT_SZ);

    /* Read the data extension info section */
    TRY_READ_COMPONENT(reader,
                       &fileHeader->dataExtensionInfo,
                       fileHeader->NITF_NUMDES,
                       NITF_LDSH_SZ,
                       NITF_LD_SZ);

    /* Read the reserved extension info section */
    TRY_READ_COMPONENT(reader,
                       &fileHeader->reservedExtensionInfo,
                       fileHeader->NITF_NUMRES,
                       NITF_LRESH_SZ,
                       NITF_LRE_SZ);

    /* Read the user header info section */
    TRY_READ_UDHD(reader);

    /* Read the extended header info section */
    TRY_READ_XHD(reader);

    actualHeaderLength =
            (uint32_t)nitf_IOInterface_tell(reader->input, error);
    NITF_TRY_GET_UINT32(fileHeader->NITF_HL, &expectedHeaderLength, error);
    if (actualHeaderLength != expectedHeaderLength)
    {
        nitf_Error_initf(error,
                         NITF_CTXT,
                         NITF_ERR_READING_FROM_FILE,
                         "NITF header expected to have length %u, "
                         "but read %u bytes",
                         expectedHeaderLength,
                         actualHeaderLength);
        goto CATCH_ERROR;
    }

    return NITF_SUCCESS;

CATCH_ERROR:
    return NITF_FAILURE;
}

/*  This function is quite possibly, not even right, except  */
/*  I dont have any data that proves that it doesnt work.    */
/*  Someone who knows better should fix this, because Im not */
/*  even convinced that it does the right thing.             */
/*  Check to see if headerFieldSize and offsetFieldSize are always the same */
NITFPRIV(NITF_BOOL)
readExtras(nitf_Reader* reader,
           nitf_Extensions* ext,
           nitf_Field* totalLengthValue,
           nitf_Field* overflowOffsetValue,
           nitf_Error* error)
{
    /* Total length of the extras seciton */
    uint32_t totalLength;
    /* Offset in the stream to the end of the section */
    nitf_Off sectionEndOffset;
    /* The current io offset */
    nitf_Off currentOffset;

    /* Read the total length of the "extras" section */
    TRY_READ_VALUE(reader, totalLengthValue, 5);

    /* Make sure it is a positive integer */
    NITF_TRY_GET_UINT32(totalLengthValue, &totalLength, error);

    if (totalLength)
    {
        /* Read the overflowOffset */
        TRY_READ_VALUE(reader, overflowOffsetValue, 3);

        currentOffset = nitf_IOInterface_tell(reader->input, error);
        if (!NITF_IO_SUCCESS(currentOffset))
            goto CATCH_ERROR;

        sectionEndOffset = currentOffset + (nitf_Off)totalLength - 3;

        while (currentOffset < sectionEndOffset)
        {
            if (!readTRE(reader, ext, error))
            {
                nitf_FieldWarning* fieldWarning;
                /* Get the current offset */
                currentOffset = nitf_IOInterface_tell(reader->input, error);

                if (!NITF_IO_SUCCESS(currentOffset))
                    goto CATCH_ERROR;

                /* Generate a warning */
                fieldWarning =
                        nitf_FieldWarning_construct(currentOffset,
                                                    "TRE",
                                                    NULL,
                                                    "Not properly formed",
                                                    error);
                if (fieldWarning == NULL)
                    goto CATCH_ERROR;

                /* Append the warning to the list */
                if (!nitf_List_pushBack(reader->warningList,
                                        fieldWarning,
                                        error))
                {
                    goto CATCH_ERROR;
                }

                /* Skip the remaining TRE's */
                currentOffset =
                        nitf_IOInterface_seek(reader->input,
                                              sectionEndOffset - currentOffset,
                                              NITF_SEEK_CUR,
                                              error);
                if (!NITF_IO_SUCCESS(currentOffset))
                    goto CATCH_ERROR;
            }

            currentOffset = nitf_IOInterface_tell(reader->input, error);
            if (!NITF_IO_SUCCESS(currentOffset))
                goto CATCH_ERROR;

        } /* End of while */
    }

    /* no problems */
    return NITF_SUCCESS;

CATCH_ERROR:
    return NITF_FAILURE;
}

NITFPRIV(NITF_BOOL)
readTRE(nitf_Reader* reader, nitf_Extensions* ext, nitf_Error* error)
{
    /* character array for the tag */
    char etag[NITF_ETAG_SZ + 1];

    /* tre object */
    nitf_TRE* tre = NULL;

    /* length of the TRE object */
    uint32_t length;
    nitf_Field* lengthValue;

    memset(etag, 0, NITF_ETAG_SZ + 1);

    /* A value to store the length in
       (we really just want the nice conversion functions ) */
    lengthValue = nitf_Field_construct(NITF_EL_SZ, NITF_BCS_N, error);
    if (!lengthValue)
        goto CATCH_ERROR;

    /* Read the tag header  */
    if (!readField(reader, etag, NITF_ETAG_SZ, error))
        goto CATCH_ERROR;

    /* blank trim the tag */
    nitf_Field_trimString(etag);

    TRY_READ_VALUE(reader, lengthValue, NITF_EL_SZ);
    NITF_TRY_GET_UINT32(lengthValue, &length, error);

    /*  We know our constraints, so build up the tre object  */
    tre = nitf_TRE_createSkeleton(etag, error);
    if (!tre)
        goto CATCH_ERROR;

    if (!handleTRE(reader, length, tre, error))
        goto CATCH_ERROR;

    /*  Insert the tre into the data store  */
    if (!nitf_Extensions_appendTRE(ext, tre, error))
        goto CATCH_ERROR;

    /* Destruct the value object we created */
    nitf_Field_destruct(&lengthValue);
    lengthValue = NULL;

    return NITF_SUCCESS;

CATCH_ERROR:
    /* Destruct the value object we created */
    if (lengthValue)
        nitf_Field_destruct(&lengthValue);
    if (tre)
        nitf_TRE_destruct(&tre);
    return NITF_FAILURE;
}

NITFPRIV(NITF_BOOL)
handleTRE(nitf_Reader* reader,
          uint32_t length,
          nitf_TRE* tre,
          nitf_Error* error)
{
    int ok = 0;
    int bad = 0;
    nitf_Off off;

    nitf_TREHandler* handler = NULL;

    nitf_PluginRegistry* reg = nitf_PluginRegistry_getInstance(error);
    if (reg)
    {
        handler = nitf_PluginRegistry_retrieveTREHandler(reg,
                                                         tre->tag,
                                                         &bad,
                                                         error);
        if (bad)
            goto CATCH_ERROR;
        if (handler)
        {
            tre->handler = handler;
            off = nitf_IOInterface_tell(reader->input, error);

            ok = handler->read(
                    reader->input, length, tre, reader->record, error);
            if (!ok)
            {
                /* move the IO back the size of the TRE */
                nitf_IOInterface_seek(reader->input, off, NITF_SEEK_SET, error);
            }
        }
    }

    /* if we couldn't parse it with the plug-in OR if no plug-in is found,
     * then, we use the default TRE handler */
    if (!ok || handler == NULL)
    {
        tre->handler = nitf_DefaultTRE_handler(error);
        ok = tre->handler->read(
                reader->input, length, tre, reader->record, error);
    }

    if (!ok)
        goto CATCH_ERROR;

    /* no problems */
    return NITF_SUCCESS;

CATCH_ERROR:
    return NITF_FAILURE;
}

NITFPRIV(NITF_BOOL)
readCorners(nitf_Reader* reader,
            nitf_ImageSubheader* subhdr,
            nitf_Version fver,
            nitf_Error* error)
{
    TRY_READ_MEMBER_VALUE(reader, subhdr, NITF_ICORDS);

    if ((IS_NITF20(fver) &&
         (subhdr->NITF_ICORDS->raw[0] == 'U' ||
          subhdr->NITF_ICORDS->raw[0] == 'G' ||
          subhdr->NITF_ICORDS->raw[0] == 'C')) ||
        ((IS_NITF21(fver) &&
          (subhdr->NITF_ICORDS->raw[0] == 'U' ||
           subhdr->NITF_ICORDS->raw[0] == 'G' ||
           subhdr->NITF_ICORDS->raw[0] == 'N' ||
           subhdr->NITF_ICORDS->raw[0] == 'S' ||
           subhdr->NITF_ICORDS->raw[0] == 'D'))))
    {
        TRY_READ_MEMBER_VALUE(reader, subhdr, NITF_IGEOLO);
    }
    else
    {
        /*printf("Note to programmer: there was no IGEOLO place in the file\n");
         */
    }

    return NITF_SUCCESS;

CATCH_ERROR:

    return NITF_FAILURE;
}

NITFPRIV(nitf_BandInfo**)
readBandInfo(nitf_Reader* reader, unsigned int nbands, nitf_Error* error)
{
    uint32_t i;
    uint32_t numLuts, bandEntriesPerLut;
    nitf_BandInfo** bandInfo = NULL;
    if (nbands > 0)
    {
        bandInfo =
                (nitf_BandInfo**)NITF_MALLOC(sizeof(nitf_BandInfo*) * nbands);
        if (!bandInfo)
        {
            nitf_Error_init(error,
                            NITF_STRERROR(NITF_ERRNO),
                            NITF_CTXT,
                            NITF_ERR_MEMORY);
            return NULL;
        }
    }

    /*  Make sure we are all NULL-inited  */
    for (i = 0; i < nbands; i++)
        bandInfo[i] = nitf_BandInfo_construct(error);

    /*  Now pick up our precious band info  */
    for (i = 0; i < nbands; i++)
    {
        TRY_READ_MEMBER_VALUE(reader, bandInfo[i], NITF_IREPBAND);
        TRY_READ_MEMBER_VALUE(reader, bandInfo[i], NITF_ISUBCAT);
        TRY_READ_MEMBER_VALUE(reader, bandInfo[i], NITF_IFC);
        TRY_READ_MEMBER_VALUE(reader, bandInfo[i], NITF_IMFLT);

        TRY_READ_MEMBER_VALUE(reader, bandInfo[i], NITF_NLUTS);
        NITF_TRY_GET_UINT32(bandInfo[i]->NITF_NLUTS, &numLuts, error);
        if (numLuts > 0)
        {
            TRY_READ_MEMBER_VALUE(reader, bandInfo[i], NITF_NELUT);
            NITF_TRY_GET_UINT32(bandInfo[i]->NITF_NELUT,
                                &bandEntriesPerLut,
                                error);

            bandInfo[i]->lut = nitf_LookupTable_construct(numLuts,
                                                          bandEntriesPerLut,
                                                          error);
            if (!bandInfo[i]->lut)
            {
                nitf_Error_init(error,
                                NITF_STRERROR(NITF_ERRNO),
                                NITF_CTXT,
                                NITF_ERR_MEMORY);
                return NULL;
            }

            // Be sure the product of `numLuts` and `bandEntriesPerLut` does not overflow.
            const uint64_t fieldLength_ = (uint64_t)numLuts * (uint64_t)bandEntriesPerLut;
            const int fieldLength = (int)fieldLength_; // readField() has an "int" length parameter
            if ((fieldLength_ > INT_MAX)  || (fieldLength < 0))
            {
                nitf_Error_init(error,
                                "fieldLength overflow",
                                NITF_CTXT,
                                NITF_ERR_MEMORY);
                return NULL;
            }
            if (!readField(reader,
                           (char*)bandInfo[i]->lut->table,
                           fieldLength,
                           error))
                goto CATCH_ERROR;
        }
    }
    return bandInfo;

CATCH_ERROR:
    return NULL;
}

NITFAPI(nitf_Record*)
nitf_Reader_read(nitf_Reader* reader, nitf_IOHandle ioHandle, nitf_Error* error)
{
    nitf_Record* record = NULL;
    nitf_IOInterface* io = NULL;

    io = nitf_IOHandleAdapter_construct(ioHandle, NRT_ACCESS_READONLY, error);
    if (!io)
        return NULL;

    record = nitf_Reader_readIO(reader, io, error);
    reader->ownInput = 1; /* we own the IOInterface */
    return record;
}

NITFAPI(nitf_Record*)
nitf_Reader_readIO(nitf_Reader* reader, nitf_IOInterface* io, nitf_Error* error)
{
    uint32_t i = 0; /* iterator */
    uint32_t num32; /* generic uint32 */
    uint32_t length32;
    uint64_t length;
    nitf_Version fver;

    reader->record = nitf_Record_construct(NITF_VER_21, error);
    if (!reader->record)
    {
        /* Couldnt make a record */
        return NULL;
    }

    resetIOInterface(reader);
    reader->input = io;
    if (!reader->input)
        goto CATCH_ERROR;

    /*  This part is trivial thanks to our readHeader accessor  */
    if (!readHeader(reader, error))
        goto CATCH_ERROR;

    fver = nitf_Record_getVersion(reader->record);
    NITF_TRY_GET_UINT32(reader->record->header->numImages, &num32, error);

    /*  Foreach image, read the header and skip to the end  */
    for (i = 0; i < num32; i++)
    {
        /* Construct a new segment */
        nitf_ImageSegment* imageSegment = nitf_ImageSegment_construct(error);
        if (!imageSegment)
            goto CATCH_ERROR;

        /* Push it onto the back of the list of segments */
        if (!nitf_List_pushBack(reader->record->images,
                                (NITF_DATA*)imageSegment,
                                error))
        {
            nitf_ImageSegment_destruct(&imageSegment);
            goto CATCH_ERROR;
        }

        /* Read the sub-header */
        if (!readImageSubheader(reader, i, fver, error))
            goto CATCH_ERROR;

        /* Allocate an IO object */
        imageSegment->imageOffset = nitf_IOInterface_tell(reader->input, error);

        /*  For now, we mark this as imageOffset + length of image data  */
        NITF_TRY_GET_UINT64(reader->record->header->NITF_LI(i), &length, error);
        imageSegment->imageEnd = imageSegment->imageOffset + length;

        /*  Now, we zoom to the end of the image, so we can pick up  */
        /*  afterward.                                               */
        if (!NITF_IO_SUCCESS(nitf_IOInterface_seek(reader->input,
                                                   imageSegment->imageEnd,
                                                   NITF_SEEK_SET,
                                                   error)))
        {
            goto CATCH_ERROR;
        }
    }

    NITF_TRY_GET_UINT32(reader->record->header->numGraphics, &num32, error);

    for (i = 0; i < num32; i++)
    {
        /* Construct a new segment */
        nitf_GraphicSegment* graphicSegment =
                nitf_GraphicSegment_construct(error);

        if (!graphicSegment)
            goto CATCH_ERROR;

        /* Push it onto the back of the list of segments */
        if (!nitf_List_pushBack(reader->record->graphics,
                                (NITF_DATA*)graphicSegment,
                                error))
        {
            nitf_GraphicSegment_destruct(&graphicSegment);
            goto CATCH_ERROR;
        }

        if (!readGraphicSubheader(reader, i, fver, error))
            goto CATCH_ERROR;
        graphicSegment->offset = nitf_IOInterface_tell(reader->input, error);

        /*  For now, we mark this as graphicOffset + length of graphic data  */
        NITF_TRY_GET_UINT32(reader->record->header->NITF_LS(i),
                            &length32,
                            error);

        graphicSegment->end = graphicSegment->offset + length32;

        /*  Now, we zoom to the end of the graphic, so we can pick up  */
        /*  afterward.                                               */
        if (!NITF_IO_SUCCESS(nitf_IOInterface_seek(
                    reader->input, graphicSegment->end, NITF_SEEK_SET, error)))
        {
            goto CATCH_ERROR;
        }
    }

    NITF_TRY_GET_UINT32(reader->record->header->numLabels, &num32, error);
    for (i = 0; i < num32; i++)
    {
        /* Construct a new segment */
        nitf_LabelSegment* labelSegment = nitf_LabelSegment_construct(error);
        if (!labelSegment)
            goto CATCH_ERROR;

        /* Push it onto the back of the list of segments */
        if (!nitf_List_pushBack(reader->record->labels,
                                (NITF_DATA*)labelSegment,
                                error))
        {
            nitf_LabelSegment_destruct(&labelSegment);
            goto CATCH_ERROR;
        }

        if (!readLabelSubheader(reader, i, fver, error))
            goto CATCH_ERROR;
        labelSegment->offset = nitf_IOInterface_tell(reader->input, error);

        /*  For now, we mark this as labelOffset + length of label data  */
        NITF_TRY_GET_UINT32(reader->record->header->NITF_LL(i),
                            &length32,
                            error);
        labelSegment->end = labelSegment->offset + length32;

        /*  Now, we zoom to the end of the label, so we can pick up  */
        /*  afterward.                                               */
        if (!NITF_IO_SUCCESS(nitf_IOInterface_seek(
                    reader->input, labelSegment->end, NITF_SEEK_SET, error)))
        {
            goto CATCH_ERROR;
        }
    }

    NITF_TRY_GET_UINT32(reader->record->header->numTexts, &num32, error);
    for (i = 0; i < num32; i++)
    {
        /* Construct a new segment */
        nitf_TextSegment* textSegment = nitf_TextSegment_construct(error);
        if (!textSegment)
            goto CATCH_ERROR;

        /* Push it onto the back of the list of segments */
        if (!nitf_List_pushBack(reader->record->texts,
                                (NITF_DATA*)textSegment,
                                error))
        {
            nitf_TextSegment_destruct(&textSegment);
            goto CATCH_ERROR;
        }

        if (!readTextSubheader(reader, i, fver, error))
            goto CATCH_ERROR;
        textSegment->offset = nitf_IOInterface_tell(reader->input, error);

        /*  For now, we mark this as textOffset + length of text data  */
        NITF_TRY_GET_UINT32(reader->record->header->NITF_LT(i),
                            &length32,
                            error);
        textSegment->end = textSegment->offset + length32;

        /*  Now, we zoom to the end of the text, so we can pick up  */
        /*  afterward.                                               */
        if (!NITF_IO_SUCCESS(nitf_IOInterface_seek(
                    reader->input, textSegment->end, NITF_SEEK_SET, error)))
        {
            goto CATCH_ERROR;
        }
    }

    NITF_TRY_GET_UINT32(reader->record->header->numDataExtensions,
                        &num32,
                        error);

    for (i = 0; i < num32; i++)
    {
        /* Construct a new segment */
        nitf_DESegment* deSegment = nitf_DESegment_construct(error);
        if (!deSegment)
            goto CATCH_ERROR;

        /* Push it onto the back of the list of segments */
        if (!nitf_List_pushBack(reader->record->dataExtensions,
                                (NITF_DATA*)deSegment,
                                error))
        {
            nitf_DESegment_destruct(&deSegment);
            goto CATCH_ERROR;
        }

        if (!readDESubheader(reader, i, fver, error))
            goto CATCH_ERROR;

        /* readDESubheader takes care of zooming/reading the DES Data */
        /* if it is a TRE_OVERFLOW, it reads it, so we can't always skip it */
    }

    NITF_TRY_GET_UINT32(reader->record->header->numReservedExtensions,
                        &num32,
                        error);
    for (i = 0; i < num32; i++)
    {
        /* Construct a new segment */
        nitf_RESegment* reSegment = nitf_RESegment_construct(error);
        if (!reSegment)
            goto CATCH_ERROR;

        /* Push it onto the back of the list of segments */
        if (!nitf_List_pushBack(reader->record->reservedExtensions,
                                (NITF_DATA*)reSegment,
                                error))
        {
            nitf_RESegment_destruct(&reSegment);
            goto CATCH_ERROR;
        }

        if (!readRESubheader(reader, i, fver, error))
            goto CATCH_ERROR;

        /*  Now, we zoom to the end of the RES, so we can pick up  */
        /*  afterward.                                               */
        if (!NITF_IO_SUCCESS(nitf_IOInterface_seek(
                    reader->input, reSegment->end, NITF_SEEK_SET, error)))
        {
            goto CATCH_ERROR;
        }
    }

    return reader->record;

CATCH_ERROR:
    nitf_Record_destruct(&reader->record);
    resetIOInterface(reader);
    return NULL;
}

NITFPRIV(nitf_DecompressionInterface*)
getDecompIface(const char* comp, int* bad, nitf_Error* error)
{
    nitf_PluginRegistry* reg;
    nitf_DecompressionInterface* decompIface;
    NITF_PLUGIN_DECOMPRESSION_CONSTRUCT_FUNCTION constructDecompIface;

    /*  Set bad to 0 (we are good so far) */
    *bad = 0;

    /*  Find the decompression interface here  */
    reg = nitf_PluginRegistry_getInstance(error);
    if (!reg)
    {
        /*  The plugin registry populated this error */
        *bad = 1;
        return NULL;
    }
    /*  Now retrieve the decomp iface creator  */
    constructDecompIface = nitf_PluginRegistry_retrieveDecompConstructor(reg,
                                                                         comp,
                                                                         bad,
                                                                         error);
    if (*bad)
    {
        /*  The plugin registry encountered an error */
        return NULL;
    }

    if (constructDecompIface == NULL)
    {
        /*  We have no decompressor registered, so ignore  */
        nitf_Debug_flogf(
                stderr, "****Setting NULL interface for decompressor!******\n");
        return NULL;
    }
    decompIface =
            (nitf_DecompressionInterface*)(*constructDecompIface)(comp, error);
    if (decompIface == NULL)
    {
        /*  We had a bad encounter while creating the */
        /*  decompression object.  The error should be populated, */
        /*  so go home... */
        *bad = 1;
    }
    return decompIface;
}

NITFPRIV(nitf_ImageIO*)
allocIO(nitf_ImageSegment* segment, nrt_HashTable* options, nitf_Error* error)
{
    char compBuf[NITF_IC_SZ + 1]; /* holds the compression string */
    int bad = 0;
    nitf_DecompressionInterface* decompIface = NULL;
    /*nitf_CompressionInterface* compIface = NULL; */
    if (!segment)
    {
        nitf_Error_init(error,
                        "This operation requires a valid ImageSegment!",
                        NITF_CTXT,
                        NITF_ERR_INVALID_OBJECT);
        return NULL;
    }
    if (!segment->subheader)
    {
        nitf_Error_init(error,
                        "This operation requires a valid ImageSubheader!",
                        NITF_CTXT,
                        NITF_ERR_INVALID_OBJECT);
        return NULL;
    }

    /* get the compression string */
    nitf_Field_get(segment->subheader->NITF_IC,
                   compBuf,
                   NITF_CONV_STRING,
                   NITF_IC_SZ + 1,
                   error);

    if (memcmp(compBuf, "NC", 2) != 0 && memcmp(compBuf, "NM", 2) != 0)
    {
        /* get the decompression interface */
        decompIface = getDecompIface(compBuf, &bad, error);

        if (bad)
        {
            /*  The getDecompIface() function failed  */
            return NULL;
        }
    }
    /* compIface = getCompIface(segment->subheader->NITF_IC,
     * bad, error);
     */

    /*  Shouldnt we also have the compression ratio??  */
    return nitf_ImageIO_construct(segment->subheader,
                                  segment->imageOffset,
                                  segment->imageEnd - segment->imageOffset,
                                  NULL,
                                  decompIface,
                                  options,
                                  error);
}

NITFAPI(nitf_ImageReader*)
nitf_Reader_newImageReader(nitf_Reader* reader,
                           int imageSegmentNumber,
                           nrt_HashTable* options,
                           nitf_Error* error)
{
    int i;
    nitf_ListIterator iter;
    nitf_ListIterator end;
    nitf_ImageSegment* segment = NULL;
    nitf_ImageReader* imageReader =
            (nitf_ImageReader*)NITF_MALLOC(sizeof(nitf_ImageReader));
    if (!imageReader)
    {
        nitf_Error_init(error,
                        NITF_STRERROR(NITF_ERRNO),
                        NITF_CTXT,
                        NITF_ERR_MEMORY);
        return NULL;
    }

    iter = nitf_List_begin(reader->record->images);
    end = nitf_List_end(reader->record->images);

    /*  Important, this is starting at zero  */
    for (i = 0; i <= imageSegmentNumber; i++)
    {
        segment = (nitf_ImageSegment*)nitf_ListIterator_get(&iter);
        /*  We have expired without finding the segment  */
        /*  they are talking about, so throw something at them  */
        if (nitf_ListIterator_equals(&iter, &end))
        {
            segment = NULL;
            break;
        }
        nitf_ListIterator_increment(&iter);
    }
    if (segment == NULL)
    {
        nitf_Error_initf(error,
            NITF_CTXT,
            NITF_ERR_INVALID_OBJECT,
            "Index [%d] is not a valid image segment",
            imageSegmentNumber);
        nitf_ImageReader_destruct(&imageReader);
        return NULL;
    }

    imageReader->input = reader->input;
    imageReader->imageDeblocker = allocIO(segment, options, error);
    if (!imageReader->imageDeblocker)
    {
        nitf_ImageReader_destruct(&imageReader);
        return NULL;
    }
    if (segment == NULL)
    {
        nitf_Error_initf(error,
                         NITF_CTXT,
                         NITF_ERR_INVALID_OBJECT,
                         "Index [%d] is not a valid image segment",
                         imageSegmentNumber);
        nitf_ImageReader_destruct(&imageReader);
        return NULL;
    }
    imageReader->directBlockRead = 0;
    return imageReader;
}

NITFAPI(nitf_SegmentReader*)
nitf_Reader_newTextReader(nitf_Reader* reader,
                          int textSegmentNumber,
                          nitf_Error* error)
{
    nitf_SegmentReader* textReader; /* The result */
    nitf_ListIterator iter; /* Iterators to used find segment in list */
    nitf_ListIterator end;
    nitf_TextSegment* text; /* Associated DE segment */
    int i;

    /*    Find the associated segment */
    text = NULL;
    iter = nitf_List_begin(reader->record->texts);
    end = nitf_List_end(reader->record->texts);
    for (i = 0; i <= textSegmentNumber; i++)
    {
        text = (nitf_TextSegment*)nitf_ListIterator_get(&iter);
        if (nitf_ListIterator_equals(&iter, &end))
        {
            text = NULL;
            break;
        }
        nitf_ListIterator_increment(&iter);
    }
    if (text == NULL)
    {
        nitf_Error_initf(error,
            NITF_CTXT,
            NITF_ERR_INVALID_OBJECT,
            "Index [%d] is not a valid Text segment",
            textSegmentNumber);
        return NULL;
    }

    /*    Allocate the object */
    textReader = (nitf_SegmentReader*)NITF_MALLOC(sizeof(nitf_SegmentReader));
    if (!textReader)
    {
        nitf_Error_init(error,
                        NITF_STRERROR(NITF_ERRNO),
                        NITF_CTXT,
                        NITF_ERR_MEMORY);
        return NULL;
    }

    /* set the fields */
    textReader->input = reader->input;
    textReader->dataLength = (uint32_t)(text->end - text->offset);
    textReader->baseOffset = text->offset;
    textReader->virtualOffset = 0;

    return (textReader);
}

NITFAPI(nitf_SegmentReader*)
nitf_Reader_newGraphicReader(nitf_Reader* reader, int index, nitf_Error* error)
{
    nitf_SegmentReader* segmentReader;
    nitf_ListIterator iter;
    nitf_ListIterator end;

    /*    Find the associated segment */
    iter = nitf_List_at(reader->record->graphics, index);
    end = nitf_List_end(reader->record->graphics);
    if (nitf_ListIterator_equals(&iter, &end))
    {
        nitf_Error_initf(error,
                         NITF_CTXT,
                         NITF_ERR_INVALID_OBJECT,
                         "Index [%d] is not a valid Graphic segment",
                         index);
        return NULL;
    }
    nitf_GraphicSegment* segment = (nitf_GraphicSegment*)nitf_ListIterator_get(&iter);

    /*    Allocate the object */
    segmentReader =
            (nitf_SegmentReader*)NITF_MALLOC(sizeof(nitf_SegmentReader));
    if (!segmentReader)
    {
        nitf_Error_init(error,
                        NITF_STRERROR(NITF_ERRNO),
                        NITF_CTXT,
                        NITF_ERR_MEMORY);
        return NULL;
    }

    /* set the fields */
    segmentReader->input = reader->input;
    segmentReader->dataLength = (uint32_t)(segment->end - segment->offset);
    segmentReader->baseOffset = segment->offset;
    segmentReader->virtualOffset = 0;

    return segmentReader;
}

NITFAPI(nitf_SegmentReader*)
nitf_Reader_newDEReader(nitf_Reader* reader, int index, nitf_Error* error)
{
    nitf_SegmentReader* segmentReader;
    nitf_ListIterator iter;
    nitf_ListIterator end;

    /*    Find the associated segment */
    iter = nitf_List_at(reader->record->dataExtensions, index);
    end = nitf_List_end(reader->record->dataExtensions);
    if (nitf_ListIterator_equals(&iter, &end))
    {
        nitf_Error_initf(error,
                         NITF_CTXT,
                         NITF_ERR_INVALID_OBJECT,
                         "Index [%d] is not a valid Graphic segment",
                         index);
        return NULL;
    }
    nitf_DESegment* segment = (nitf_DESegment*)nitf_ListIterator_get(&iter);

    /*    Allocate the object */
    segmentReader =
            (nitf_SegmentReader*)NITF_MALLOC(sizeof(nitf_SegmentReader));
    if (!segmentReader)
    {
        nitf_Error_init(error,
                        NITF_STRERROR(NITF_ERRNO),
                        NITF_CTXT,
                        NITF_ERR_MEMORY);
        return NULL;
    }

    /* set the fields */
    segmentReader->input = reader->input;
    segmentReader->dataLength = (uint32_t)(segment->end - segment->offset);
    segmentReader->baseOffset = segment->offset;
    segmentReader->virtualOffset = 0;

    return segmentReader;
}

NITFAPI(nitf_Version) nitf_Reader_getNITFVersionIO(nitf_IOInterface* io)
{
    nitf_Error error;
    char fhdr[NITF_FHDR_SZ];
    char fver[NITF_FVER_SZ];
    nitf_Version version = NITF_VER_UNKNOWN;
    nitf_Off offset = nitf_IOInterface_tell(io, &error);

    if (offset == (nitf_Off)-1)
    {
        return version;
    }

    if (!nitf_IOInterface_canSeek(io, &error))
    {
        return version;
    }

    if (!nitf_IOInterface_read(io, fhdr, NITF_FHDR_SZ, &error))
    {
        goto RESET_STREAM_AND_RETURN;
    }

    if (!nitf_IOInterface_read(io, fver, NITF_FVER_SZ, &error))
    {
        goto RESET_STREAM_AND_RETURN;
    }

    /* NSIF1.0 == NITF2.1 */
    if ((strncmp(fhdr, "NITF", NITF_FHDR_SZ) == 0 &&
         strncmp(fver, "02.10", NITF_FVER_SZ) == 0) ||
        (strncmp(fhdr, "NSIF", NITF_FHDR_SZ) == 0 &&
         strncmp(fver, "01.00", NITF_FVER_SZ) == 0))
    {
        version = NITF_VER_21;
    }
    else if (strncmp(fhdr, "NITF", NITF_FHDR_SZ) == 0 &&
             strncmp(fver, "02.00", NITF_FVER_SZ) == 0)
    {
        version = NITF_VER_20;
    }

RESET_STREAM_AND_RETURN:
    nitf_IOInterface_seek(io, offset, NITF_SEEK_SET, &error);
    return version;
}

NITFAPI(nitf_Version) nitf_Reader_getNITFVersion(const char* fileName)
{
    nitf_IOInterface* io;
    nitf_Error error;
    nitf_Version version = NITF_VER_UNKNOWN;

    io = nitf_IOHandleAdapter_open(fileName,
                                   NITF_ACCESS_READONLY,
                                   NITF_OPEN_EXISTING,
                                   &error);
    if (io)
    {
        version = nitf_Reader_getNITFVersionIO(io);
        nitf_IOInterface_close(io, &error);
        nitf_IOInterface_destruct(&io);
    }

    return version;
}
