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

#include "nitf/Writer.h"

/*  This writer basically allows exceptions. It uses the  */
/*  error object of the given Writer, thus simplifying the*/
/*  code.  Many of the MACROS used in this file should be */
/*  used throughout any helper functions, but be careful  */
/*  with it.  You must note the assumptions.              */
/*  ASSUMPTIONS:                                          */
/*  1)  A nitf_Writer object named writer exists          */
/*  2)  A parseInfo object exists                         */
/*  3)  A label is in scope called CATCH_ERROR            */

/*  These define chars for filling fields  */
#define SPACE ' '
#define ZERO '0'
#define FILL_LEFT NITF_WRITER_FILL_LEFT
#define FILL_RIGHT NITF_WRITER_FILL_RIGHT

/* define some maximum data lengths */
#define NITF_MAX_IMAGE_LENGTH NITF_INT64(9999999999)

/*  This is the size of each num* (numi, numx, nums, numdes, numres)  */
#define NITF_IVAL_SZ 3

/* This MACRO writes the given value, and pads it as specified */
/* Example: NITF_WRITE_VALUE(io, securityGroup, NITF_CLSY, SPACE, FILL_RIGHT); */
/* It jumps to the CATCH_ERROR label if an error occurs. */
#define NITF_WRITE_VALUE(own_, fld_, fil_, dir_) \
    if (!writeValue(writer, own_->fld_, fld_##_SZ, fil_, dir_, error)) \
        goto CATCH_ERROR;

#define NITF_WRITE_STR_FIELD(own_, fld_, fil_, dir_) \
    if (!writeStringField(writer, own_->fld_, fld_##_SZ, fil_, dir_, error)) \
        goto CATCH_ERROR;

#define NITF_WRITE_INT_FIELD(value_, fld_, fil_, dir_) \
    if (!writeIntField(writer, value_, fld_##_SZ, fil_, dir_, error)) \
        goto CATCH_ERROR;

#define NITF_WRITE_INT64_FIELD(value_, fld_, fil_, dir_) \
    if (!nitf_Writer_writeInt64Field(writer, value_, fld_##_SZ, fil_, dir_, error)) \
        goto CATCH_ERROR;



/*  We use the writeComponentInfo() method underneath this */
/*  method, as we do for all of the components.  The macro */
/*  allows us to specialize this functionality for images  */
#define NITF_WRITE_IMAGE_INFO(com_,num_) \
    if (!writeComponentInfo(writer, com_, num_, \
                            NITF_LISH_SZ, NITF_LI_SZ, error)) \
        goto CATCH_ERROR;

/*  We use the writeComponentInfo() method underneath this */
/*  method, as we do for all of the components.  The macro */
/*  allows us to specialize this functionality for         */
/*  graphics                                               */
#define NITF_WRITE_GRAPHICS_INFO(com_,num_) \
    if (!writeComponentInfo(writer, com_, num_, \
                            NITF_LSSH_SZ, NITF_LS_SZ, error)) \
        goto CATCH_ERROR;

/*  The label info for the 2.1 file appears to be a riddle */
/*  wrapped in enigma, shrouded in mystery.  It is a blank */
/*  space that is set to 000, and therefore, while         */
/*  technically a component info section, not meaningful   */
/*  whatsoever.  Still, we use the writeComponentInfo()    */
/*  method underneath to write it.                         */
#define NITF_WRITE_LABEL_INFO(com_,num_) \
    if (!writeComponentInfo(writer, com_, num_, 0, 0, error)) \
        goto CATCH_ERROR;

/*  We use the writeComponentInfo() method underneath this */
/*  method, as we do for all of the components.  The macro */
/*  allows us to specialize this functionality for texts   */
#define NITF_WRITE_TEXT_INFO(com_,num_) \
    if (!writeComponentInfo(writer, com_, num_, \
                            NITF_LTSH_SZ, NITF_LT_SZ, error)) \
        goto CATCH_ERROR;

/*  We use the writeComponentInfo() method underneath this */
/*  method, as we do for all of the components.  The macro */
/*  allows us to specialize this functionality for data    */
/*  extension segments                                     */
#define NITF_WRITE_DATA_EXT_INFO(com_,num_) \
    if (!writeComponentInfo(writer, com_, num_, \
                            NITF_LDSH_SZ, NITF_LD_SZ, error)) \
        goto CATCH_ERROR;

/*  We use the writeComponentInfo() method underneath this */
/*  method, as we do for all of the components.  The macro */
/*  allows us to specialize this functionality for         */
/*  reserved extension segments                            */
#define NITF_WRITE_RES_EXT_INFO(com_,num_) \
    if (!writeComponentInfo(writer, com_, num_, \
                            NITF_LRESH_SZ, NITF_LRE_SZ, error)) \
        goto CATCH_ERROR;

/*  This macro makes it easier to write the user-defined   */
/*  header data section.  The writeExtras() method supplies*/
/*  the underlying driving call, but it can be generalized */
/*  for this case, and for the extended header components  */
#define NITF_WRITE_USER_HDR_INFO(ext_,hdl_,ofl_) \
    if (!writeExtras(writer, ext_, hdl_, ofl_, \
                     NITF_UDHDL_SZ, NITF_UDHOFL_SZ, error)) \
        goto CATCH_ERROR;

/*  This macro makes it easier to write the extended       */
/*  header data section.  The writeExtras() method supplies*/
/*  the underlying driving call, but it can be generalized */
/*  for this case, and for the extended header components  */
#define NITF_WRITE_EXT_HDR_INFO(ext_,hdl_,ofl_) \
    if (!writeExtras(writer, ext_, hdl_, ofl_, \
                     NITF_XHDL_SZ, NITF_XHDLOFL_SZ, error)) \
        goto CATCH_ERROR;

/* ------------------------------------------------------------------ */
/*                PRIVATE PROTOTYPES                                  */
/* ------------------------------------------------------------------ */

/*  This is a simple write method.  Note that this is NOT a finalized */
/*  method.  It is simply pending on error handling.  Exiting is not  */
/*  even an option.                                                   */
NITFPRIV(NITF_BOOL) writeField(nitf_Writer * writer,
                               char *field,
                               uint32_t length, nitf_Error * error);

/*  Pads a string with a fill character                              */
/*  field The input string                                             */
/*  length The total length of the string                            */
/*  fill The fill character                                          */
/*  fillDir The fill direction (either FILL_LEFT or FILL_RIGHT)      */
NITFPRIV(NITF_BOOL) padString(char *field,
                              uint32_t length,
                              char fill,
                              const uint32_t fillDir,
                              nitf_Error * error);

/*  Writes the given TRE  */
NITFPRIV(NITF_BOOL) writeExtension(nitf_Writer * writer,
                                   nitf_TRE * tre, nitf_Error * error);

/* ------------------------------------------------------------------ */
/*                PRIVATE AREA                                        */
/* ------------------------------------------------------------------ */

NITFPRIV(void) resetIOInterface(nitf_Writer * writer)
{
    if (writer->output && writer->ownOutput)
        nitf_IOInterface_destruct( &(writer->output) );
    writer->output = NULL;
    writer->ownOutput = 0;
}

NITFPRIV(NITF_BOOL) writeStringField(nitf_Writer * writer,
                                     char *field,
                                     uint32_t length,
                                     char fill,
                                     const uint32_t fillDir,
                                     nitf_Error * error)
{
    const size_t length_ = ((size_t)length) + 1;
    char *buf = (char *) NITF_MALLOC(length_);
    if (!buf)
    {
        nitf_Error_init(error, NITF_STRERROR(NITF_ERRNO),
                        NITF_CTXT, NITF_ERR_MEMORY);
        goto CATCH_ERROR;
    }

    memset(buf, '\0', length_);
    memcpy(buf, field, length);

    if (!padString(buf, length, fill, fillDir, error))
        goto CATCH_ERROR;

    if (!writeField(writer, buf, length, error))
        goto CATCH_ERROR;

    NITF_FREE(buf);
    return NITF_SUCCESS;

CATCH_ERROR:
    if (buf) NITF_FREE(buf);
    return NITF_FAILURE;
}


NITFPRIV(NITF_BOOL) writeValue(nitf_Writer * writer,
                               nitf_Field * field,
                               uint32_t length,
                               char fill,
                               const uint32_t fillDir,
                               nitf_Error * error)
{
    const size_t length_ = ((size_t)length) + 1;
    char *buf = (char *) NITF_MALLOC(length_);
    if (!buf)
    {
        nitf_Error_init(error, NITF_STRERROR(NITF_ERRNO),
                        NITF_CTXT, NITF_ERR_MEMORY);
        goto CATCH_ERROR;
    }

    memset(buf, '\0', length_);

    /* first, check to see if we need to swap bytes */
    if (field->type == NITF_BINARY)
    {
        if (length == NITF_INT16_SZ)
        {
            int16_t int16 =
                (int16_t)NITF_HTONS(*((int16_t *) field->raw));
            memcpy(buf, (char*)&int16, length);
        }
        else if (length == NITF_INT32_SZ)
        {
            int32_t int32 =
                (int32_t)NITF_HTONL(*((int32_t *) field->raw));
            memcpy(buf, (char*)&int32, length);
        }
        else
        {
            /* TODO what to do??? 8 bit is ok, but what about 64? */
            memcpy(buf, field->raw, length);
        }

        /* we do not pad binary values, for obvious reasons... */
    }
    else
    {
        memcpy(buf, field->raw, length);

        if (!padString(buf, length, fill, fillDir, error))
            goto CATCH_ERROR;
    }

    if (!writeField(writer, buf, length, error))
        goto CATCH_ERROR;

    NITF_FREE(buf);
    return NITF_SUCCESS;

CATCH_ERROR:
    if (buf) NITF_FREE(buf);
    return NITF_FAILURE;
}


NITFPRIV(NITF_BOOL) writeIntField(nitf_Writer * writer,
                                  uint32_t field,
                                  uint32_t length,
                                  char fill,
                                  const uint32_t fillDir,
                                  nitf_Error * error)
{
    char buf[20];

    memset(buf, '\0', 20);
    NITF_SNPRINTF(buf, 20, "%d", field);

    if (padString(buf, length, fill, fillDir, error))
    {
        return writeField(writer, buf, length, error);
    }

    /* error padding */
    nitf_Error_init(error, NITF_STRERROR(NITF_ERRNO),
                    NITF_CTXT, NITF_ERR_UNK);
    return NITF_FAILURE;
}


/* This function pads the given uint64_t, and writes it to the IOHandle */
NITFPROT(NITF_BOOL) nitf_Writer_writeInt64Field(nitf_Writer* writer,
                                                uint64_t field,
                                                uint32_t length,
                                                char fill,
                                                uint32_t fillDir,
                                                nitf_Error* error)
{
    char buf[20];

    memset(buf, '\0', 20);
    NITF_SNPRINTF(buf, 20, "%lld", (long long int)field);

    if (padString(buf, length, fill, fillDir, error))
    {
        return writeField(writer, buf, length, error);
    }

    /* error padding */
    nitf_Error_init(error, NITF_STRERROR(NITF_ERRNO),
                    NITF_CTXT, NITF_ERR_UNK);
    return NITF_FAILURE;
}


NITFPRIV(NITF_BOOL) writeField(nitf_Writer * writer,
                               char *field,
                               uint32_t length,
                               nitf_Error * error)
{
    if (!nitf_IOInterface_write(writer->output, field, length, error))
    {
        nitf_Error_init(error, NITF_STRERROR(NITF_ERRNO),
                        NITF_CTXT, NITF_ERR_WRITING_TO_FILE);
        return NITF_FAILURE;
    }
    return NITF_SUCCESS;
}


/* This function writes NITF 2.0 specific security information */
NITFPRIV(NITF_BOOL) write20FileSecurity(nitf_Writer * writer,
                                        nitf_FileSecurity * securityGroup,
                                        nitf_Error * error)
{
    if (!writeStringField(writer, securityGroup->NITF_CODE->raw,
                               NITF_CODE_20_SZ, SPACE, FILL_RIGHT, error))
        goto CATCH_ERROR;
    if (!writeStringField(writer, securityGroup->NITF_CTLH->raw,
                               NITF_CTLH_20_SZ, SPACE, FILL_RIGHT, error))
        goto CATCH_ERROR;
    if (!writeStringField(writer, securityGroup->NITF_REL->raw,
                               NITF_REL_20_SZ, SPACE, FILL_RIGHT, error))
        goto CATCH_ERROR;
    if (!writeStringField(writer, securityGroup->NITF_CAUT->raw,
                               NITF_CAUT_20_SZ, SPACE, FILL_RIGHT, error))
        goto CATCH_ERROR;
    if (!writeStringField(writer, securityGroup->NITF_CTLN->raw,
                               NITF_CTLN_20_SZ, SPACE, FILL_RIGHT, error))
        goto CATCH_ERROR;
    if (!writeStringField(writer, securityGroup->NITF_DGDT->raw,
                               NITF_DGDT_20_SZ, SPACE, FILL_RIGHT, error))
        goto CATCH_ERROR;

    if (strncmp(securityGroup->NITF_DGDT->raw, "999998", 6) == 0)
        if (!writeStringField(writer, securityGroup->NITF_CLTX->raw,
                              NITF_CLTX_20_SZ, ZERO, FILL_LEFT, error))
            goto CATCH_ERROR;

    return NITF_SUCCESS;

CATCH_ERROR:
    return NITF_FAILURE;
}


/* This function writes NITF 2.1 specific security information */
NITFPRIV(NITF_BOOL) write21FileSecurity(nitf_Writer * writer,
                                        nitf_FileSecurity * securityGroup,
                                        nitf_Error * error)
{
    NITF_WRITE_VALUE(securityGroup, NITF_CLSY, SPACE, FILL_RIGHT);
    NITF_WRITE_VALUE(securityGroup, NITF_CODE, SPACE, FILL_RIGHT);
    NITF_WRITE_VALUE(securityGroup, NITF_CTLH, SPACE, FILL_RIGHT);
    NITF_WRITE_VALUE(securityGroup, NITF_REL, SPACE, FILL_RIGHT);
    NITF_WRITE_VALUE(securityGroup, NITF_DCTP, SPACE, FILL_RIGHT);
    NITF_WRITE_VALUE(securityGroup, NITF_DCDT, SPACE, FILL_RIGHT);
    NITF_WRITE_VALUE(securityGroup, NITF_DCXM, SPACE, FILL_RIGHT);
    NITF_WRITE_VALUE(securityGroup, NITF_DG, SPACE, FILL_RIGHT);
    NITF_WRITE_VALUE(securityGroup, NITF_DGDT, SPACE, FILL_RIGHT);
    NITF_WRITE_VALUE(securityGroup, NITF_CLTX, SPACE, FILL_RIGHT);
    NITF_WRITE_VALUE(securityGroup, NITF_CATP, SPACE, FILL_RIGHT);
    NITF_WRITE_VALUE(securityGroup, NITF_CAUT, SPACE, FILL_RIGHT);
    NITF_WRITE_VALUE(securityGroup, NITF_CRSN, SPACE, FILL_RIGHT);
    NITF_WRITE_VALUE(securityGroup, NITF_RDT, SPACE, FILL_RIGHT);
    NITF_WRITE_VALUE(securityGroup, NITF_CTLN, SPACE, FILL_RIGHT);

    return NITF_SUCCESS;

CATCH_ERROR:
    return NITF_FAILURE;
}


/* This function pads the given string with the fill character */
NITFPRIV(NITF_BOOL) padString(char *field,
                              uint32_t length,
                              char fill,
                              const uint32_t fillDir,
                              nitf_Error * error)
{
    /*  size and remainder  */
    size_t size = 0, offset = 0;

    /* check to see if we even need to pad it */
    if (!field)
    {
        nitf_Error_initf(error, NITF_CTXT, NITF_ERR_INVALID_PARAMETER,
                "Trying to use NULL field. padString failed.");
        return NITF_FAILURE;
    }
    size = (uint32_t)strlen(field);
    if (size >= length)
    {
        /* Dont need to pad at all */
        return NITF_SUCCESS; /* No error occurred */
    }

    offset = (size_t)length - size;
    /* Set the buffer to the fill character */
    if (fillDir == FILL_RIGHT)
    {
        memset((char*)(field + size), fill, offset);
    }
    else if (fillDir == FILL_LEFT)
    {
        memmove((char*)(field + offset), field, size);
        memset(field, fill, offset);
    }
    field[length] = '\0';

    return NITF_SUCCESS;
}


/* This function writes the given ComponentInfo section */
NITFPRIV(NITF_BOOL) writeComponentInfo(nitf_Writer * writer,
                                       nitf_ComponentInfo ** info,
                                       uint32_t num,
                                       uint32_t subHdrSize,
                                       uint32_t segmentSize,
                                       nitf_Error * error)
{
    uint32_t i;

    /*  First, write the num*  */
    if (!writeIntField(writer, num, NITF_IVAL_SZ, ZERO, FILL_LEFT, error))
        goto CATCH_ERROR;

    /*  Write the image info  */
    for (i = 0; i < num; i++)
    {
        if (!writeStringField(writer, info[i]->lengthSubheader->raw,
                              subHdrSize, ZERO, FILL_LEFT, error))
            goto CATCH_ERROR;
        if (!writeStringField(writer, info[i]->lengthData->raw,
                              segmentSize, ZERO, FILL_LEFT, error))
            goto CATCH_ERROR;
    }

    return NITF_SUCCESS;

    /* Error */
CATCH_ERROR:
    return NITF_FAILURE;
}


/* This function writes the given Extensions */
NITFPRIV(NITF_BOOL) writeExtras(nitf_Writer * writer,
                                nitf_Extensions * section,
                                uint32_t * dataLength,
                                uint32_t * dataOverflow,
                                const uint32_t hdlFieldSize,
                                const uint32_t oflFieldSize,
                                nitf_Error * error)
{
    nitf_ExtensionsIterator iter, end;
    uint32_t totalLength = 0;
    nitf_Version version;
    nitf_TRE *tre = NULL;

    /* get the version */
    version = nitf_Record_getVersion(writer->record);

    /* compute the length of the section */
    totalLength = nitf_Extensions_computeLength(section, version, error);

    /* must add the oflFieldSize if we have non-zero data length */
    *dataLength = (totalLength > 0 || *dataOverflow != 0) ?
                   totalLength + oflFieldSize : totalLength;

    /*  First, write length and overflow fields */
    if (!writeIntField(writer, *dataLength, hdlFieldSize,
                       ZERO, FILL_LEFT, error))
        goto CATCH_ERROR;

    if (*dataLength != 0)
    {
        if (!writeIntField(writer, *dataOverflow,
                           oflFieldSize, ZERO, FILL_LEFT, error))
            goto CATCH_ERROR;
    }

    if (section != NULL)
    {
        /* set up iterators */
        iter = nitf_Extensions_begin(section);
        end = nitf_Extensions_end(section);

        /* Now, write the TREs */
        while (nitf_ExtensionsIterator_notEqualTo(&iter, &end))
        {
            tre = (nitf_TRE *) nitf_ExtensionsIterator_get(&iter);

            /* write it! */
            if (!writeExtension(writer, tre, error))
                goto CATCH_ERROR;

            /* increment */
            nitf_ExtensionsIterator_increment(&iter);
        }
    }

    /* Normal completion */
    return NITF_SUCCESS;

    /* Error */
CATCH_ERROR:
    return NITF_FAILURE;
}


/* This function writes the given TRE */
NITFPRIV(NITF_BOOL) writeExtension(nitf_Writer * writer,
                                   nitf_TRE * tre, nitf_Error * error)
{

    uint32_t length;

    /* write the cetag and cel */
    if (!writeStringField(writer, tre->tag,
            NITF_ETAG_SZ, SPACE, FILL_RIGHT, error))
        goto CATCH_ERROR;


    length = tre->handler->getCurrentSize(tre, error);
    if (length == -1)
      goto CATCH_ERROR;

    if (!writeIntField(writer, length, NITF_EL_SZ, ZERO, FILL_LEFT, error))
        goto CATCH_ERROR;

    /* write the data, then free the buf */

    if (!tre->handler->write(writer->output, tre, writer->record, error))
      goto CATCH_ERROR;

    return NITF_SUCCESS;

    /* Error */
CATCH_ERROR:
    return NITF_FAILURE;
}


/* This function writes the IGEOLO field */
NITFPRIV(NITF_BOOL) writeCorners(nitf_Writer* writer,
                                 const nitf_ImageSubheader* subhdr,
                                 nitf_Version fver, nitf_Error* error)
{
    if ((IS_NITF21(fver) &&
            (subhdr->NITF_ICORDS->raw[0] == 'U' ||
             subhdr->NITF_ICORDS->raw[0] == 'G' ||
             subhdr->NITF_ICORDS->raw[0] == 'N' ||
             subhdr->NITF_ICORDS->raw[0] == 'S' ||
             subhdr->NITF_ICORDS->raw[0] == 'D'))
            || (IS_NITF20(fver) && subhdr->NITF_ICORDS->raw[0] != 'N'))
    {
        NITF_WRITE_VALUE(subhdr, NITF_IGEOLO, SPACE, FILL_RIGHT);
    }

    return NITF_SUCCESS;

CATCH_ERROR:
    return NITF_FAILURE;
}


/* This function writes the given BandInfo */
NITFPRIV(NITF_BOOL) writeBandInfo(nitf_Writer * writer,
                                  nitf_BandInfo ** bandInfo,
                                  uint32_t nbands, nitf_Error * error)
{
    uint32_t i;
    uint32_t numLuts, bandEntriesPerLut;

    for (i = 0; i < nbands; ++i)
    {
        NITF_WRITE_VALUE(bandInfo[i], NITF_IREPBAND, SPACE, FILL_RIGHT);
        NITF_WRITE_VALUE(bandInfo[i], NITF_ISUBCAT, SPACE, FILL_RIGHT);
        NITF_WRITE_VALUE(bandInfo[i], NITF_IFC, SPACE, FILL_RIGHT);
        NITF_WRITE_VALUE(bandInfo[i], NITF_IMFLT, SPACE, FILL_RIGHT);
        NITF_WRITE_VALUE(bandInfo[i], NITF_NLUTS, ZERO, FILL_LEFT);

        NITF_TRY_GET_UINT32(bandInfo[i]->NITF_NLUTS, &numLuts, error);
        if (numLuts > 0)
        {
            NITF_WRITE_VALUE(bandInfo[i], NITF_NELUT, ZERO, FILL_LEFT);
            NITF_TRY_GET_UINT32(bandInfo[i]->NITF_NELUT,
                                &bandEntriesPerLut, error);

            if (!writeField(writer, (char *) bandInfo[i]->lut->table,
                    numLuts * bandEntriesPerLut, error))
                goto CATCH_ERROR;
        }
    }

    /* Normal completion */
    return NITF_SUCCESS;

    /* Error */
CATCH_ERROR:
    return NITF_FAILURE;
}


/* ------------------------------------------------------------------ */
/*                PUBLIC AREA                                         */
/* ------------------------------------------------------------------ */

NITFAPI(nitf_Writer *) nitf_Writer_construct(nitf_Error * error)
{
    /*  Create the writer */
    nitf_Writer *writer = (nitf_Writer *) NITF_MALLOC(sizeof(nitf_Writer));

    if (!writer)
    {
        nitf_Error_init(error, NITF_STRERROR(NITF_ERRNO),
                        NITF_CTXT, NITF_ERR_MEMORY);
        return NULL;
    }

    /* NULL-initialize everything first */
    writer->imageWriters = NULL;
    writer->graphicWriters = NULL;
    writer->textWriters = NULL;
    writer->dataExtensionWriters = NULL;
    writer->output = NULL;
    writer->ownOutput = 0;
    writer->record = NULL;
    writer->numImageWriters = 0;
    writer->numTextWriters = 0;
    writer->numGraphicWriters = 0;
    writer->numDataExtensionWriters = 0;

    writer->warningList = nitf_List_construct(error);
    if (!writer->warningList)
    {
        nitf_Writer_destruct(&writer);
        return NULL;
    }

    /*  Return our results  */
    return writer;
}

/*!
 * Destroy all of the Writers attached to this Writer
 */
NITFPRIV(void) nitf_Writer_destructWriters(nitf_Writer* writer)
{
    if (writer)
    {
        int i;
        for (i = 0; i < (writer->numImageWriters) && writer->imageWriters; i++)
        {
            nitf_WriteHandler_destruct(&writer->imageWriters[i]);
        }

        for (i = 0; i < (writer->numTextWriters) && writer->textWriters; i++)
        {
            nitf_WriteHandler_destruct(&writer->textWriters[i]);
        }

        for (i = 0; i < (writer->numGraphicWriters) && writer->graphicWriters; i++)
        {
            nitf_WriteHandler_destruct(&writer->graphicWriters[i]);
        }

        for (i = 0; i < (writer->numDataExtensionWriters)
                && writer->dataExtensionWriters; i++)
        {
            nitf_WriteHandler_destruct(&writer->dataExtensionWriters[i]);
        }

        if (writer->imageWriters)
        {
            NITF_FREE(writer->imageWriters);
        }
        if (writer->textWriters)
        {
            NITF_FREE(writer->textWriters);
        }
        if (writer->graphicWriters)
        {
            NITF_FREE(writer->graphicWriters);
        }
        if (writer->dataExtensionWriters)
        {
            NITF_FREE(writer->dataExtensionWriters);
        }

        writer->imageWriters = NULL;
        writer->textWriters = NULL;
        writer->graphicWriters = NULL;
        writer->dataExtensionWriters = NULL;

        writer->numImageWriters = 0;
        writer->numTextWriters = 0;
        writer->numGraphicWriters = 0;
        writer->numDataExtensionWriters = 0;
    }
}


NITFAPI(NITF_BOOL) nitf_Writer_prepareIO(nitf_Writer* writer,
                                            nitf_Record* record,
                                            nitf_IOInterface* io,
                                            nitf_Error* error)
{
    int32_t i;
    int32_t numImages;
    int32_t numTexts;
    int32_t numGraphics;
    int32_t numDEs;
    nitf_ListIterator iter;

    if (!writer)
    {
        nitf_Error_init(error, "NULL writer", NITF_CTXT,
                        NITF_ERR_INVALID_PARAMETER);
        return NITF_FAILURE;
    }

    /*  Create overflow DE segments if needed */

    if(!nitf_Record_unmergeTREs(record, error))
        return NITF_FAILURE;

    if (!nitf_Field_get
            (record->header->numImages, &numImages, NITF_CONV_INT,
             NITF_INT32_SZ, error))
    {
        nitf_Error_init(error, "Could not retrieve number of images",
                        NITF_CTXT, NITF_ERR_INVALID_OBJECT);
        return NITF_FAILURE;
    }
    if (numImages < 0 || numImages > 999)
    {
        nitf_Error_init(error, "Invalid number of images", NITF_CTXT,
                        NITF_ERR_INVALID_OBJECT);
        return NITF_FAILURE;
    }

    if (!nitf_Field_get
            (record->header->numTexts, &numTexts, NITF_CONV_INT,
             NITF_INT32_SZ, error))
    {
        nitf_Error_init(error, "Could not retrieve number of texts",
                        NITF_CTXT, NITF_ERR_INVALID_OBJECT);
        return NITF_FAILURE;
    }
    if (numTexts < 0 || numTexts > 999)
    {
        nitf_Error_init(error, "Invalid number of texts", NITF_CTXT,
                        NITF_ERR_INVALID_OBJECT);
        return NITF_FAILURE;
    }

    if (!nitf_Field_get
            (record->header->numGraphics, &numGraphics, NITF_CONV_INT,
             NITF_INT32_SZ, error))
    {
        nitf_Error_init(error, "Could not retrieve number of graphics",
                        NITF_CTXT, NITF_ERR_INVALID_OBJECT);
        return NITF_FAILURE;
    }
    if (numGraphics < 0 || numGraphics > 999)
    {
        nitf_Error_init(error, "Invalid number of graphics", NITF_CTXT,
                        NITF_ERR_INVALID_OBJECT);
        return NITF_FAILURE;
    }

    if (!nitf_Field_get
            (record->header->numDataExtensions, &numDEs, NITF_CONV_INT,
             NITF_INT32_SZ, error))
    {
        nitf_Error_init(error, "Could not retrieve number of data extensions",
                        NITF_CTXT, NITF_ERR_INVALID_OBJECT);
        return NITF_FAILURE;
    }
    if (numDEs < 0 || numDEs > 999)
    {
        nitf_Error_init(error, "Invalid number of data extensions", NITF_CTXT,
                        NITF_ERR_INVALID_OBJECT);
        return NITF_FAILURE;
    }

    /* first, destroy any writers attached to this Writer */
    nitf_Writer_destructWriters(writer);

    writer->record = record;
    resetIOInterface(writer);
    writer->output = io;

    /* setup image writers */
    if (numImages > 0)
    {
        /* must free the buf if it was already allocated */
        if (writer->imageWriters)
            NITF_FREE(writer->imageWriters);

        writer->imageWriters =
            (nitf_WriteHandler **) NITF_MALLOC(sizeof(nitf_WriteHandler *) *
                                              numImages);
        if (!writer->imageWriters)
        {
            nitf_Error_init(error, "Bad alloc for image writers", NITF_CTXT,
                            NITF_ERR_MEMORY);
            return NITF_FAILURE;
        }

        /* first, set the writer to NULL */
        for (i = 0; i < numImages; i++)
        {
            writer->imageWriters[i] = NULL;
        }

        writer->numImageWriters = numImages;
        for (i = 0; i < numImages; i++)
        {
            nitf_ImageSegment *segment = NULL;
            nitf_ImageSubheader *subheader = NULL;
            uint32_t nbpp, nbands, xbands, nrows, ncols;
            uint64_t length;

            /* guard against an overflowing data length */
            iter = nitf_List_at(record->images, i);
            segment = (nitf_ImageSegment*) nitf_ListIterator_get(&iter);
            subheader = segment->subheader;

            /* calculate the length */
            NITF_TRY_GET_UINT32(subheader->numBitsPerPixel, &nbpp, error);
            NITF_TRY_GET_UINT32(subheader->numImageBands, &nbands, error);
            NITF_TRY_GET_UINT32(subheader->numMultispectralImageBands, &xbands, error);
            NITF_TRY_GET_UINT32(subheader->numRows, &nrows, error);
            NITF_TRY_GET_UINT32(subheader->numCols, &ncols, error);

            const uint64_t bands_ = ((uint64_t)nbands) + xbands;
            length = (uint64_t)ncols * (uint64_t)nrows *
                    NITF_NBPP_TO_BYTES(nbpp) * bands_;

            if (length > NITF_MAX_IMAGE_LENGTH)
            {
                nitf_Error_init(error, "Image Length is too large", NITF_CTXT,
                        NITF_ERR_INVALID_OBJECT);
                return NITF_FAILURE;
            }
        }
    }

    /* setup text writers */
    if (numTexts > 0)
    {
        writer->textWriters =
            (nitf_WriteHandler **) NITF_MALLOC(sizeof(nitf_WriteHandler *) *
                                                numTexts);
        if (!writer->textWriters)
        {
            nitf_Error_init(error, "Bad alloc for text writers", NITF_CTXT,
                            NITF_ERR_MEMORY);
            return NITF_FAILURE;
        }
        writer->numTextWriters = numTexts;
        for (i = 0; i < numTexts; i++)
        {
            writer->textWriters[i] = NULL;
        }
    }

    /* setup graphic writers */
    if (numGraphics > 0)
    {
        writer->graphicWriters =
            (nitf_WriteHandler **) NITF_MALLOC(sizeof(nitf_WriteHandler *) *
                                                numGraphics);
        if (!writer->graphicWriters)
        {
            nitf_Error_init(error, "Bad alloc for graphic writers", NITF_CTXT,
                            NITF_ERR_MEMORY);
            return NITF_FAILURE;
        }
        writer->numGraphicWriters = numGraphics;
        for (i = 0; i < numGraphics; i++)
        {
            writer->graphicWriters[i] = NULL;
        }
    }

    if (numDEs > 0)
    {
        writer->dataExtensionWriters =
            (nitf_WriteHandler **) NITF_MALLOC(sizeof(nitf_WriteHandler *) *
                                                numDEs);
        if (!writer->dataExtensionWriters)
        {
            nitf_Error_init(error, "Bad alloc for data extension writers",
                            NITF_CTXT, NITF_ERR_MEMORY);
            return NITF_FAILURE;
        }
        writer->numDataExtensionWriters = numDEs;
        for (i = 0; i < numDEs; i++)
        {
            writer->dataExtensionWriters[i] = NULL;
        }
    }

    return NITF_SUCCESS;

  CATCH_ERROR:
    return NITF_FAILURE;

}

NITFAPI(NITF_BOOL) nitf_Writer_prepare(nitf_Writer * writer,
                                       nitf_Record * record,
                                       nitf_IOHandle ioHandle,
                                       nitf_Error * error)
{
    NITF_BOOL rc;
    nitf_IOInterface* io = nitf_IOHandleAdapter_construct(ioHandle,
                                                          NRT_ACCESS_WRITEONLY,
                                                          error);
    if (!io)
        return NITF_FAILURE;

    rc = nitf_Writer_prepareIO(writer, record, io, error);
    writer->ownOutput = 1;
    return rc;
}


NITFAPI(void) nitf_Writer_destruct(nitf_Writer ** writer)
{
    /*  If the writer has already been destructed, or was never  */
    /*  Inited, dont dump core                                   */
    if (*writer)
    {
        nitf_Writer_destructWriters(*writer);

        if ((*writer)->warningList)
        {
            nitf_List_destruct(&(*writer)->warningList);
        }

        resetIOInterface(*writer);
        NITF_FREE(*writer);
        *writer = NULL;
    }
}


NITFPROT(NITF_BOOL) nitf_Writer_writeHeader(nitf_Writer* writer,
                                            nitf_Off* fileLenOff,
                                            uint32_t* hdrLen,
                                            nitf_Error* error)
{
    uint32_t numImages, numGraphics, numLabels;
    uint32_t numTexts, numDES, numRES;
    uint32_t udhdl, udhofl, xhdl, xhdlofl;
    nitf_Version fver;
    char buf[256];              /* temp buf */

    fver = nitf_Record_getVersion(writer->record);

    /* start writing */
    NITF_WRITE_VALUE(writer->record->header, NITF_FHDR, SPACE, FILL_RIGHT);
    NITF_WRITE_VALUE(writer->record->header, NITF_FVER, ZERO, FILL_LEFT);
    NITF_WRITE_VALUE(writer->record->header, NITF_CLEVEL, ZERO, FILL_LEFT);
    NITF_WRITE_VALUE(writer->record->header, NITF_STYPE, SPACE, FILL_RIGHT);
    NITF_WRITE_VALUE(writer->record->header, NITF_OSTAID, SPACE, FILL_RIGHT);
    NITF_WRITE_VALUE(writer->record->header, NITF_FDT, SPACE, FILL_RIGHT);
    NITF_WRITE_VALUE(writer->record->header, NITF_FTITLE, SPACE, FILL_RIGHT);
    NITF_WRITE_VALUE(writer->record->header, NITF_FSCLAS, SPACE, FILL_RIGHT);

    if (IS_NITF20(fver))
    {
        if (!write20FileSecurity(writer,
                writer->record->header->securityGroup, error))
            goto CATCH_ERROR;
    }
    else if (IS_NITF21(fver))
    {
        if (!write21FileSecurity(writer,
                writer->record->header->securityGroup, error))
            goto CATCH_ERROR;
    }
    else
    {
        nitf_Error_init(error, "Invalid NITF Version",
                        NITF_CTXT, NITF_ERR_UNK);
        goto CATCH_ERROR;
    }

    NITF_WRITE_VALUE(writer->record->header, NITF_FSCOP, ZERO, FILL_LEFT);
    NITF_WRITE_VALUE(writer->record->header, NITF_FSCPYS, ZERO, FILL_LEFT);
    NITF_WRITE_VALUE(writer->record->header, NITF_ENCRYP, ZERO, FILL_LEFT);

    if (IS_NITF20(fver))
    {
        if (!writeValue(writer, writer->record->header->NITF_ONAME,
                        NITF_FBKGC_SZ + NITF_ONAME_SZ, SPACE, FILL_RIGHT, error))
            goto CATCH_ERROR;
    }
    else
    {
        NITF_WRITE_VALUE(writer->record->header, NITF_FBKGC, 0, FILL_LEFT);
        NITF_WRITE_VALUE(writer->record->header, NITF_ONAME, SPACE, FILL_RIGHT);
    }

    NITF_WRITE_VALUE(writer->record->header, NITF_OPHONE, SPACE, FILL_RIGHT);

    *fileLenOff = nitf_IOInterface_tell(writer->output, error);
    if (!NITF_IO_SUCCESS(*fileLenOff))
        goto CATCH_ERROR;

    NITF_WRITE_VALUE(writer->record->header, NITF_FL, ZERO, FILL_LEFT);
    NITF_WRITE_VALUE(writer->record->header, NITF_HL, ZERO, FILL_LEFT);

    /*  Write component info  */
    NITF_TRY_GET_UINT32(writer->record->header->numImages,
                        &numImages, error);
    NITF_TRY_GET_UINT32(writer->record->header->numGraphics,
                        &numGraphics, error);
    NITF_TRY_GET_UINT32(writer->record->header->numLabels,
                        &numLabels, error);
    NITF_TRY_GET_UINT32(writer->record->header->numTexts,
                        &numTexts, error);
    NITF_TRY_GET_UINT32(writer->record->header->numDataExtensions,
                        &numDES, error);
    NITF_TRY_GET_UINT32(writer->record->header->numReservedExtensions,
                        &numRES, error);

    NITF_WRITE_IMAGE_INFO(writer->record->header->imageInfo, numImages);

    /*
        Do not try to write anything but images, graphics, texts and DEs for now
    */
    numLabels = 0;
    numRES = 0;

    NITF_WRITE_GRAPHICS_INFO(writer->record->header->graphicInfo,
                             numGraphics);

    NITF_WRITE_LABEL_INFO(writer->record->header->labelInfo, numLabels);

    NITF_WRITE_TEXT_INFO(writer->record->header->textInfo, numTexts);

    NITF_WRITE_DATA_EXT_INFO(writer->record->header->dataExtensionInfo,
                             numDES);

    NITF_WRITE_RES_EXT_INFO(writer->record->header->reservedExtensionInfo,
                            numRES);

    NITF_TRY_GET_UINT32(writer->record->header->userDefinedHeaderLength,
        &udhdl, error);
    NITF_TRY_GET_UINT32(writer->record->header->userDefinedOverflow,
        &udhofl, error);
    NITF_TRY_GET_UINT32(writer->record->header->extendedHeaderLength,
        &xhdl, error);
    NITF_TRY_GET_UINT32(writer->record->header->extendedHeaderOverflow,
        &xhdlofl, error);

    NITF_WRITE_USER_HDR_INFO(writer->record->header->userDefinedSection,
                             &udhdl, &udhofl);
    NITF_WRITE_EXT_HDR_INFO(writer->record->header->extendedSection,
                            &xhdl, &xhdlofl);

    /* just to be nice, let's set these values in the record */
    NITF_SNPRINTF(buf, 256, "%.*d", NITF_UDHDL_SZ, udhdl);
    nitf_Field_setRawData(writer->record->header->userDefinedHeaderLength,
                          buf, NITF_UDHDL_SZ, error);

    NITF_SNPRINTF(buf, 256, "%.*d", NITF_UDHOFL_SZ, udhofl);
    nitf_Field_setRawData(writer->record->header->userDefinedOverflow,
                          buf, NITF_UDHOFL_SZ, error);

    NITF_SNPRINTF(buf, 256, "%.*d", NITF_XHDL_SZ, xhdl);
    nitf_Field_setRawData(writer->record->header->extendedHeaderLength,
                          buf, NITF_XHDL_SZ, error);

    NITF_SNPRINTF(buf, 256, "%.*d", NITF_XHDLOFL_SZ, xhdlofl);
    nitf_Field_setRawData(writer->record->header->extendedHeaderOverflow,
                          buf, NITF_XHDLOFL_SZ, error);

    /*   Get the header length */
    const nitf_Off hdrLen_ = nitf_IOInterface_tell(writer->output, error);
    if (!NITF_IO_SUCCESS(hdrLen_))
        goto CATCH_ERROR;
    *hdrLen = (uint32_t)hdrLen_;

    /* Normal completion */
    return NITF_SUCCESS;

    /* Error */
CATCH_ERROR:
    return NITF_FAILURE;
}


NITFPROT(NITF_BOOL)
nitf_Writer_writeImageSubheader(nitf_Writer* writer,
                                const nitf_ImageSubheader* subhdr,
                                nitf_Version fver,
                                nitf_Off* comratOff,
                                nitf_Error* error)
{
    uint32_t bands;
    uint32_t i;
    uint32_t numComments;
    uint32_t udidl, udofl, ixshdl, ixsofl;
    nitf_ListIterator iter, end;

    NITF_WRITE_VALUE(subhdr, NITF_IM, SPACE, FILL_RIGHT);
    NITF_WRITE_VALUE(subhdr, NITF_IID1, SPACE, FILL_RIGHT);
    NITF_WRITE_VALUE(subhdr, NITF_IDATIM, SPACE, FILL_RIGHT);
    NITF_WRITE_VALUE(subhdr, NITF_TGTID, SPACE, FILL_RIGHT);
    NITF_WRITE_VALUE(subhdr, NITF_IID2, SPACE, FILL_RIGHT);
    NITF_WRITE_VALUE(subhdr, NITF_ISCLAS, SPACE, FILL_RIGHT);

    if (IS_NITF20(fver))
    {
        if (!write20FileSecurity(writer, subhdr->securityGroup, error))
            goto CATCH_ERROR;
    }
    else if (IS_NITF21(fver))
    {
        if (!write21FileSecurity(writer, subhdr->securityGroup, error))
            goto CATCH_ERROR;
    }
    else
    {
        /* !!!we should check this earlier, not here */
        nitf_Error_init(error, "Invalid NITF Version",
                        NITF_CTXT, NITF_ERR_UNK);
        goto CATCH_ERROR;
    }

    NITF_WRITE_VALUE(subhdr, NITF_ENCRYP, ZERO, FILL_LEFT);
    NITF_WRITE_VALUE(subhdr, NITF_ISORCE, SPACE, FILL_RIGHT);

    NITF_WRITE_VALUE(subhdr, NITF_NROWS, ZERO, FILL_LEFT);
    NITF_WRITE_VALUE(subhdr, NITF_NCOLS, ZERO, FILL_LEFT);

    NITF_WRITE_VALUE(subhdr, NITF_PVTYPE, SPACE, FILL_RIGHT);
    NITF_WRITE_VALUE(subhdr, NITF_IREP, SPACE, FILL_RIGHT);
    NITF_WRITE_VALUE(subhdr, NITF_ICAT, SPACE, FILL_RIGHT);

    NITF_WRITE_VALUE(subhdr, NITF_ABPP, ZERO, FILL_LEFT);
    NITF_WRITE_VALUE(subhdr, NITF_PJUST, SPACE, FILL_RIGHT);

    NITF_WRITE_VALUE(subhdr, NITF_ICORDS, SPACE, FILL_RIGHT);

    if (!writeCorners(writer, subhdr, fver, error))
        goto CATCH_ERROR;

    /* image comments */
    NITF_WRITE_VALUE(subhdr, NITF_NICOM, ZERO, FILL_LEFT);
    NITF_TRY_GET_UINT32(subhdr->numImageComments, &numComments, error);

    /* loop through and write the comments */
    iter = nitf_List_begin(subhdr->imageComments);
    end = nitf_List_end(subhdr->imageComments);
    i = 0;
    while (nitf_ListIterator_notEqualTo(&iter, &end) && i < numComments)
    {
        nitf_Field* commentField = (nitf_Field*) nitf_ListIterator_get(&iter);
        if (!writeStringField(writer, commentField->raw, NITF_ICOM_SZ,
                SPACE, FILL_RIGHT, error))
            goto CATCH_ERROR;
        nitf_ListIterator_increment(&iter);
        ++i;
    }

    NITF_WRITE_VALUE(subhdr, NITF_IC, SPACE, FILL_RIGHT);

    if (strncmp(subhdr->imageCompression->raw, "NC", 2) != 0
            && strncmp(subhdr->imageCompression->raw, "NM", 2) != 0)
    {
        *comratOff = nitf_IOInterface_tell(writer->output, error);
        NITF_WRITE_VALUE(subhdr, NITF_COMRAT, SPACE, FILL_RIGHT);
    }

    /* deal with bands */
    NITF_WRITE_VALUE(subhdr, NITF_NBANDS, ZERO, FILL_LEFT);
    NITF_TRY_GET_UINT32(subhdr->numImageBands, &bands, error);

    if ((!bands) && IS_NITF21(fver))
    {
        NITF_WRITE_VALUE(subhdr, NITF_XBANDS, ZERO, FILL_LEFT);
        NITF_TRY_GET_UINT32(subhdr->numMultispectralImageBands, &bands,
                            error);
    }
    if (!writeBandInfo(writer, subhdr->bandInfo, bands, error))
        goto CATCH_ERROR;

    NITF_WRITE_VALUE(subhdr, NITF_ISYNC, ZERO, FILL_LEFT);
    NITF_WRITE_VALUE(subhdr, NITF_IMODE, SPACE, FILL_RIGHT);

    NITF_WRITE_VALUE(subhdr, NITF_NBPR, ZERO, FILL_LEFT);
    NITF_WRITE_VALUE(subhdr, NITF_NBPC, ZERO, FILL_LEFT);
    NITF_WRITE_VALUE(subhdr, NITF_NPPBH, ZERO, FILL_LEFT);
    NITF_WRITE_VALUE(subhdr, NITF_NPPBV, ZERO, FILL_LEFT);
    NITF_WRITE_VALUE(subhdr, NITF_NBPP, ZERO, FILL_LEFT);

    NITF_WRITE_VALUE(subhdr, NITF_IDLVL, ZERO, FILL_LEFT);
    NITF_WRITE_VALUE(subhdr, NITF_IALVL, ZERO, FILL_LEFT);
    NITF_WRITE_VALUE(subhdr, NITF_ILOC, ZERO, FILL_LEFT);
    NITF_WRITE_VALUE(subhdr, NITF_IMAG, SPACE, FILL_RIGHT);

    /* deal with extensions */
    NITF_TRY_GET_UINT32(subhdr->NITF_UDIDL, &udidl, error);
    NITF_TRY_GET_UINT32(subhdr->NITF_UDOFL, &udofl, error);
    NITF_TRY_GET_UINT32(subhdr->NITF_IXSHDL, &ixshdl, error);
    NITF_TRY_GET_UINT32(subhdr->NITF_IXSOFL, &ixsofl, error);

    NITF_WRITE_USER_HDR_INFO(subhdr->userDefinedSection, &udidl, &udofl);
    NITF_WRITE_EXT_HDR_INFO(subhdr->extendedSection, &ixshdl, &ixsofl);

    /* Normal completion */
    return NITF_SUCCESS;

    /* Error */
CATCH_ERROR:
    return NITF_FAILURE;
}


NITFPRIV(NITF_BOOL) writeGraphicSubheader(nitf_Writer * writer,
                                          nitf_GraphicSubheader * subhdr,
                                          nitf_Version fver,
                                          nitf_Error * error)
{
    uint32_t sxshdl, sxsofl;

    NITF_WRITE_VALUE(subhdr, NITF_SY, SPACE, FILL_RIGHT);
    NITF_WRITE_VALUE(subhdr, NITF_SID, SPACE, FILL_RIGHT);
    NITF_WRITE_VALUE(subhdr, NITF_SNAME, SPACE, FILL_RIGHT);
    NITF_WRITE_VALUE(subhdr, NITF_SSCLAS, SPACE, FILL_RIGHT);

    if (IS_NITF20(fver))
    {
        if (!write20FileSecurity(writer, subhdr->securityGroup, error))
            goto CATCH_ERROR;
    }
    else if (IS_NITF21(fver))
    {
        if (!write21FileSecurity(writer, subhdr->securityGroup, error))
            goto CATCH_ERROR;
    }
    else
    {
        /* !!!we should check this earlier, not here */
        nitf_Error_init(error, "Invalid NITF Version",
                        NITF_CTXT, NITF_ERR_UNK);
        goto CATCH_ERROR;
    }

    NITF_WRITE_VALUE(subhdr, NITF_ENCRYP, ZERO, FILL_LEFT);
    NITF_WRITE_VALUE(subhdr, NITF_SFMT, SPACE, FILL_RIGHT);
    NITF_WRITE_VALUE(subhdr, NITF_SSTRUCT, SPACE, FILL_RIGHT);
    NITF_WRITE_VALUE(subhdr, NITF_SDLVL, SPACE, FILL_RIGHT);
    NITF_WRITE_VALUE(subhdr, NITF_SALVL, SPACE, FILL_RIGHT);
    NITF_WRITE_VALUE(subhdr, NITF_SLOC, SPACE, FILL_RIGHT);
    NITF_WRITE_VALUE(subhdr, NITF_SBND1, SPACE, FILL_RIGHT);
    NITF_WRITE_VALUE(subhdr, NITF_SCOLOR, SPACE, FILL_RIGHT);
    NITF_WRITE_VALUE(subhdr, NITF_SBND2, SPACE, FILL_RIGHT);
    NITF_WRITE_VALUE(subhdr, NITF_SRES2, SPACE, FILL_RIGHT);

    /* deal with extensions */
    NITF_TRY_GET_UINT32(subhdr->extendedHeaderLength, &sxshdl, error);
    NITF_TRY_GET_UINT32(subhdr->extendedHeaderOverflow, &sxsofl, error);
    NITF_WRITE_EXT_HDR_INFO(subhdr->extendedSection, &sxshdl, &sxsofl);

    return NITF_SUCCESS;

CATCH_ERROR:
    return NITF_FAILURE;
}

NITFPRIV(NITF_BOOL) writeTextSubheader(nitf_Writer * writer,
                                       nitf_TextSubheader * subhdr,
                                       nitf_Version fver,
                                       nitf_Error * error)
{
    uint32_t txshdl, txsofl;

    NITF_WRITE_VALUE(subhdr, NITF_TE, SPACE, FILL_RIGHT);
    NITF_WRITE_VALUE(subhdr, NITF_TEXTID, SPACE, FILL_RIGHT);
    NITF_WRITE_VALUE(subhdr, NITF_TXTALVL, SPACE, FILL_RIGHT);
    NITF_WRITE_VALUE(subhdr, NITF_TXTDT, SPACE, FILL_RIGHT);
    NITF_WRITE_VALUE(subhdr, NITF_TXTITL, SPACE, FILL_RIGHT);
    NITF_WRITE_VALUE(subhdr, NITF_TSCLAS, SPACE, FILL_RIGHT);

    if (IS_NITF20(fver))
    {
        if (!write20FileSecurity(writer, subhdr->securityGroup, error))
            goto CATCH_ERROR;
    }
    else if (IS_NITF21(fver))
    {
        if (!write21FileSecurity(writer, subhdr->securityGroup, error))
            goto CATCH_ERROR;
    }
    else
    {
        /* !!!we should check this earlier, not here */
        nitf_Error_init(error, "Invalid NITF Version",
                        NITF_CTXT, NITF_ERR_UNK);
        goto CATCH_ERROR;
    }

    NITF_WRITE_VALUE(subhdr, NITF_ENCRYP, ZERO, FILL_LEFT);
    NITF_WRITE_VALUE(subhdr, NITF_TXTFMT, SPACE, FILL_RIGHT);

    /* deal with extensions */
    NITF_TRY_GET_UINT32(subhdr->extendedHeaderLength, &txshdl, error);
    NITF_TRY_GET_UINT32(subhdr->extendedHeaderOverflow, &txsofl, error);
    NITF_WRITE_EXT_HDR_INFO(subhdr->extendedSection, &txshdl, &txsofl);

    return NITF_SUCCESS;

CATCH_ERROR:
    return NITF_FAILURE;
}


NITFPROT(NITF_BOOL) nitf_Writer_writeDESubheader(nitf_Writer* writer,
                                                 const nitf_DESubheader* subhdr,
                                                 uint32_t* userSublen,
                                                 nitf_Version fver,
                                                 nitf_Error* error)
{
    char* des_data = NULL;

    /* DE type ID */
    char desID[NITF_DESTAG_SZ + 1];

    /* Get the desID */
    if (!nitf_Field_get(subhdr->typeID,
                        (NITF_DATA *) desID, NITF_CONV_STRING,
                        NITF_DESTAG_SZ + 1, error))
        goto CATCH_ERROR;

    nitf_Field_trimString(desID);

    NITF_WRITE_VALUE(subhdr, NITF_DE, SPACE, FILL_RIGHT);
    NITF_WRITE_VALUE(subhdr, NITF_DESTAG, SPACE, FILL_RIGHT);
    NITF_WRITE_VALUE(subhdr, NITF_DESVER, SPACE, FILL_RIGHT);
    NITF_WRITE_VALUE(subhdr, NITF_DESCLAS, SPACE, FILL_RIGHT);

    if (IS_NITF20(fver))
    {
        if (!write20FileSecurity(writer, subhdr->securityGroup, error))
            goto CATCH_ERROR;
    }
    else if (IS_NITF21(fver))
    {
        if (!write21FileSecurity(writer, subhdr->securityGroup, error))
            goto CATCH_ERROR;
    }
    else
    {
        /* !!!we should check this earlier, not here */
        nitf_Error_init(error, "Invalid NITF Version",
                        NITF_CTXT, NITF_ERR_UNK);
        goto CATCH_ERROR;
    }

    if (strcmp(desID, "TRE_OVERFLOW") == 0)
    {
        NITF_WRITE_VALUE(subhdr, NITF_DESOFLW, SPACE, FILL_RIGHT);
        NITF_WRITE_VALUE(subhdr, NITF_DESITEM, SPACE, FILL_RIGHT);
    }

    int32_t subLen;
    if (subhdr->subheaderFields)
    {
      subLen = subhdr->subheaderFields->handler->getCurrentSize(subhdr->subheaderFields, error);
      if (subLen < 0)
          goto CATCH_ERROR;
    }
    else
        subLen = 0;

    nitf_Field_setUint32(subhdr->NITF_DESSHL, subLen, error);
    NITF_WRITE_VALUE(subhdr, NITF_DESSHL, ZERO, FILL_LEFT);

    *userSublen = subLen;

    if (subLen > 0)
    {
        if (!subhdr->subheaderFields->handler->write(writer->output,
                                                     subhdr->subheaderFields,
                                                     writer->record,
                                                     error))
            goto CATCH_ERROR;
    }

    if (des_data)
        NITF_FREE(des_data);

    return NITF_SUCCESS;

CATCH_ERROR:
    if (des_data)
        NITF_FREE(des_data);

    return NITF_FAILURE;
}


/* For now, we won't support writing RES
NITFPRIV(NITF_BOOL) writeRESubheader(nitf_Writer * writer,
                                     nitf_RESubheader * subhdr,
                                     nitf_Version fver, nitf_Error * error)
{
    uint32_t subLen;

    NITF_WRITE_VALUE(subhdr, NITF_RE, SPACE, FILL_RIGHT);
    NITF_WRITE_VALUE(subhdr, NITF_RESTAG, SPACE, FILL_RIGHT);
    NITF_WRITE_VALUE(subhdr, NITF_RESVER, SPACE, FILL_RIGHT);
    NITF_WRITE_VALUE(subhdr, NITF_RESCLAS, SPACE, FILL_RIGHT);

    if (IS_NITF20(fver))
    {
        if (!write20FileSecurity(writer, subhdr->securityGroup, error))
            goto CATCH_ERROR;
    }
    else if (IS_NITF21(fver))
    {
        if (!write21FileSecurity(writer, subhdr->securityGroup, error))
            goto CATCH_ERROR;
    }
    else
    {
        nitf_Error_init(error, "Invalid NITF Version",
                        NITF_CTXT, NITF_ERR_UNK);
        goto CATCH_ERROR;
    }

    NITF_WRITE_VALUE(subhdr, NITF_RESSHL, SPACE, FILL_RIGHT);
    NITF_TRY_GET_UINT32(subhdr->subheaderFieldsLength, &subLen, error);
    if (subLen > 0)
    {
        if (!writeField(writer, subhdr->subheaderFields, subLen, error))
            goto CATCH_ERROR;
    }


    nitf_Error_init(error,
                    "RES Error... TODO: Change code in readRESubheader",
                    NITF_CTXT, NITF_ERR_UNK);
    goto CATCH_ERROR;

    return NITF_SUCCESS;

CATCH_ERROR:
    return NITF_FAILURE;
}
*/

NITFPRIV(NITF_BOOL) writeImage(nitf_WriteHandler * imageWriter,
                               nitf_IOInterface* output,
                               nitf_Error * error)
{
    if (!imageWriter)
    {
        nitf_Error_initf(error, NITF_CTXT, NITF_ERR_INVALID_PARAMETER,
                         "Trying to use uninitialized image writer.  Write failed.");
        goto CATCH_ERROR;
    }
    return (*imageWriter->iface->write)(imageWriter->data, output, error);

CATCH_ERROR:
    return NITF_FAILURE;
}


NITFPRIV(NITF_BOOL) writeText(nitf_WriteHandler *textWriter,
                              nitf_IOInterface* output,
                              nitf_Error * error)
{
    if (!textWriter)
    {
        nitf_Error_initf(error, NITF_CTXT, NITF_ERR_INVALID_PARAMETER,
                         "Trying to use uninitialized Text Writer.  Write failed.");
        goto CATCH_ERROR;
    }
    return (*textWriter->iface->write)(textWriter->data, output, error);
CATCH_ERROR:
    return NITF_FAILURE;
}


NITFPRIV(NITF_BOOL) writeGraphic(nitf_WriteHandler * graphicWriter,
                                 nitf_IOInterface* output,
                                 nitf_Error * error)
{
    if (!graphicWriter)
    {
        nitf_Error_initf(error, NITF_CTXT, NITF_ERR_INVALID_PARAMETER,
                         "Trying to use uninitialized Graphic SegmentWriter.  Write failed.");
        goto CATCH_ERROR;
    }
    return (*graphicWriter->iface->write)(graphicWriter->data, output, error);
CATCH_ERROR:
    return NITF_FAILURE;
}

NITFPRIV(NITF_BOOL) writeDE(nitf_Writer* writer,
                            nitf_WriteHandler * deWriter,
                            nitf_DESubheader *subheader,
                            nitf_IOInterface* output,
                            nitf_Error *error)
{
    /* DESID for overflow check */
    char desid[NITF_DESTAG_SZ+1];

    /*  Check for overflow segment */
    if(!nitf_Field_get(subheader->NITF_DESTAG,(NITF_DATA *) desid,
                    NITF_CONV_STRING,NITF_DESTAG_SZ+1, error))
    {
        nitf_Error_init(error,
                "Could not retrieve DE segment id",
                NITF_CTXT, NITF_ERR_INVALID_OBJECT);
        return NITF_FAILURE;
    }

    nitf_Field_trimString(desid);
    if ((strcmp(desid, "TRE_OVERFLOW") == 0) ||
        (strcmp(desid, "Registered Extensions") == 0) ||
        (strcmp(desid, "Controlled Extensions") == 0))
    {
        /* TRE iterator */
        nitf_ExtensionsIterator iter;

        /* End iterator */
        nitf_ExtensionsIterator end;
        nitf_TRE *tre = NULL;

        iter = nitf_Extensions_begin(subheader->userDefinedSection);
        end = nitf_Extensions_end(subheader->userDefinedSection);

        while (nitf_ExtensionsIterator_notEqualTo(&iter, &end))
        {
            tre = (nitf_TRE *) nitf_ExtensionsIterator_get(&iter);

            if(!writeExtension(writer, tre, error))
            goto CATCH_ERROR;

            nitf_ExtensionsIterator_increment(&iter);
        }
        return NITF_SUCCESS;
    }
    else
    {
        if (!deWriter)
        {
            nitf_Error_initf(error, NITF_CTXT, NITF_ERR_INVALID_PARAMETER,
                    "Trying to use uninitialized DE SegmentWriter.  Write failed.");
            goto CATCH_ERROR;
        }
        return (*deWriter->iface->write)(deWriter->data, output, error);
    }

  CATCH_ERROR:
    return NITF_FAILURE;
}

NITFAPI(NITF_BOOL) nitf_Writer_write(nitf_Writer * writer,
                                     nitf_Error * error)
{
    nitf_ListIterator iter;
    nitf_ListIterator end;

    /* Offset to the file length field */
    nitf_Off fileLenOff;

    /* The final file length */
    nitf_Off fileLen;

    /* Length of teh file header */
    uint32_t hdrLen;
    uint32_t i = 0;
    int skipBytes = 0;
    nitf_Version fver;

    /* Number of images */
    uint32_t numImgs = 0;

    /* Lengths of image subheaders */
    nitf_Off *imageSubLens = NULL;

    /* Lengths of image data */
    nitf_Off *imageDataLens = NULL;

    /* Number of texts */
    uint32_t numTexts = 0;

    /* Lengths of text subheaders */
    nitf_Off *textSubLens = NULL;

    /* Lengths of text data */
    nitf_Off *textDataLens = NULL;

    /* Number of graphics */
    uint32_t numGraphics = 0;

    /* Lengths of graphic subheaders */
    nitf_Off *graphicSubLens = NULL;

    /* Lengths of graphic data */
    nitf_Off *graphicDataLens = NULL;

    /* Number of data extensions */
    uint32_t numDEs = 0;

    /* Lengths of data extension subheaders */
    nitf_Off *deSubLens = NULL;

    /* Lengths of data extensions data */
    nitf_Off *deDataLens = NULL;

    /* Start file size */
    nitf_Off startSize;

    /* End file size */
    nitf_Off endSize;

    nitf_FileHeader* header = writer->record->header;

    if (!nitf_Writer_writeHeader(writer, &fileLenOff, &hdrLen, error))
        return NITF_FAILURE;

    fver = nitf_Record_getVersion(writer->record);

    /*******************************************************************/
    /*                   START DEALING WITH IMAGES                     */
    /*******************************************************************/
    if (!nitf_Field_get(header->numImages, &numImgs,
                        NITF_CONV_INT, NITF_INT32_SZ, error))
    {
        nitf_Error_init(error, "Could not retrieve number of images",
                        NITF_CTXT, NITF_ERR_INVALID_OBJECT);
        return NITF_FAILURE;
    }

    imageSubLens = NULL;        /* Void uninitialized variable warning */
    imageDataLens = NULL;       /* Void uninitialized variable warning */
    if (numImgs != 0)
    {
        imageSubLens = (nitf_Off *) NITF_MALLOC(numImgs * sizeof(nitf_Off));
        if (!imageSubLens)
        {
            nitf_Error_init(error, NITF_STRERROR(NITF_ERRNO),
                            NITF_CTXT, NITF_ERR_MEMORY);
            return NITF_FAILURE;
        }
        imageDataLens = (nitf_Off *) NITF_MALLOC(numImgs * sizeof(nitf_Off));
        if (!imageDataLens)
        {
            NITF_FREE(imageSubLens);
            nitf_Error_init(error, NITF_STRERROR(NITF_ERRNO),
                            NITF_CTXT, NITF_ERR_MEMORY);
            return NITF_FAILURE;
        }

        iter = nitf_List_begin(writer->record->images);
        end = nitf_List_end(writer->record->images);

        startSize = nitf_IOInterface_getSize(writer->output, error);
        if (!NITF_IO_SUCCESS(startSize))
        {
            NITF_FREE(imageSubLens);
            NITF_FREE(imageDataLens);
            return NITF_FAILURE;
        }
        i = 0; /* reset the counter */
        while (nitf_ListIterator_notEqualTo(&iter, &end))
        {
            nitf_Off comratOff = 0;
            nitf_ImageSegment *segment = NULL;

            segment = (nitf_ImageSegment *) nitf_ListIterator_get(&iter);
            if (!nitf_Writer_writeImageSubheader(writer, segment->subheader,
                                                 fver, &comratOff, error))
            {
                NITF_FREE(imageSubLens);
                NITF_FREE(imageDataLens);
                return NITF_FAILURE;
            }
            endSize = nitf_IOInterface_getSize(writer->output, error);
            if (!NITF_IO_SUCCESS(endSize))
            {
                NITF_FREE(imageSubLens);
                NITF_FREE(imageDataLens);
                return NITF_FAILURE;
            }
            imageSubLens[i] = endSize - startSize;
            startSize = endSize;

            /* TODO - we need to check to make sure the imageWriter exists */
            if (!writeImage(writer->imageWriters[i],
                            writer->output, error))
            {
                NITF_FREE(imageSubLens);
                NITF_FREE(imageDataLens);
                return NITF_FAILURE;
            }

            endSize = nitf_IOInterface_getSize(writer->output, error);
            if (!NITF_IO_SUCCESS(endSize))
            {
                NITF_FREE(imageSubLens);
                NITF_FREE(imageDataLens);
                return NITF_FAILURE;
            }
            imageDataLens[i] = endSize - startSize;
            startSize = endSize;

            /* the comrat field may have changed during the write, so we
             * need to update if it is compressed
             */
            if (comratOff > 0)
            {
                nitf_IOInterface_seek(writer->output, comratOff, NITF_SEEK_SET,
                                      error);
                NITF_WRITE_VALUE(segment->subheader, NITF_COMRAT, SPACE,
                                 FILL_RIGHT);
                nitf_IOInterface_seek(writer->output, endSize, NITF_SEEK_SET,
                                      error);
            }

            /*
               TODO - should we check the data length written
               against NITF_MAX_IMAGE_LENGTH?

               DP: Should not have to, we did it during the prepare

            */

            nitf_ListIterator_increment(&iter);
            ++i;
        }
    }

    /*******************************************************************/
    /*                   START DEALING WITH GRAPHICS                   */
    /*******************************************************************/
    if (!nitf_Field_get(header->numGraphics, &numGraphics,
                        NITF_CONV_INT, NITF_INT32_SZ, error))
    {
        nitf_Error_init(error, "Could not retrieve number of Graphics",
                        NITF_CTXT, NITF_ERR_INVALID_OBJECT);
        return NITF_FAILURE;
    }

    /* TODO - WE NEED TO CHECK IF A WRITER IS SETUP FOR EACH SEGMENT */
    /* IF NOT, WE SHOULD NOT WRITE THE SEGMENT AT ALL, OR SET THE DATA TO 0 */

    graphicSubLens = NULL;
    graphicDataLens = NULL;

    if (numGraphics != 0)
    {
        graphicSubLens = (nitf_Off *) NITF_MALLOC(numGraphics * sizeof(nitf_Off));
        if (!graphicSubLens)
        {
            nitf_Error_init(error, NITF_STRERROR(NITF_ERRNO),
                            NITF_CTXT, NITF_ERR_MEMORY);
            return NITF_FAILURE;
        }
        graphicDataLens = (nitf_Off *) NITF_MALLOC(numGraphics * sizeof(nitf_Off));
        if (!graphicDataLens)
        {
            NITF_FREE(graphicSubLens);
            nitf_Error_init(error, NITF_STRERROR(NITF_ERRNO),
                            NITF_CTXT, NITF_ERR_MEMORY);
            return NITF_FAILURE;
        }

        iter = nitf_List_begin(writer->record->graphics);
        end = nitf_List_end(writer->record->graphics);

        startSize = nitf_IOInterface_getSize(writer->output, error);
        if (!NITF_IO_SUCCESS(startSize))
        {
            NITF_FREE(graphicSubLens);
            NITF_FREE(graphicDataLens);
            return NITF_FAILURE;
        }
        i = 0; /* reset the counter */
        while (nitf_ListIterator_notEqualTo(&iter, &end))
        {
            nitf_GraphicSegment *segment =
                (nitf_GraphicSegment *) nitf_ListIterator_get(&iter);
            if (!writeGraphicSubheader(writer,
                                       segment->subheader, fver,
                                       error))
            {
                NITF_FREE(graphicSubLens);
                NITF_FREE(graphicDataLens);
                return NITF_FAILURE;
            }
            endSize = nitf_IOInterface_getSize(writer->output, error);
            if (!NITF_IO_SUCCESS(endSize))
            {
                NITF_FREE(graphicSubLens);
                NITF_FREE(graphicDataLens);
                return NITF_FAILURE;
            }
            graphicSubLens[i] = endSize - startSize;
            startSize = endSize;
            /* TODO - we need to check to make sure the imageWriter exists */
            if (!writeGraphic(writer->graphicWriters[i],
                              writer->output, error))
            {
                NITF_FREE(graphicSubLens);
                NITF_FREE(graphicDataLens);
                return NITF_FAILURE;
            }
            endSize = nitf_IOInterface_getSize(writer->output, error);
            if (!NITF_IO_SUCCESS(endSize))
            {
                NITF_FREE(graphicSubLens);
                NITF_FREE(graphicDataLens);
                return NITF_FAILURE;
            }
            graphicDataLens[i] = endSize - startSize;
            startSize = endSize;

            nitf_ListIterator_increment(&iter);
            ++i;
        }
    }

    /*******************************************************************/
    /*                   START DEALING WITH TEXTS                      */
    /*******************************************************************/
    if (!nitf_Field_get(header->numTexts, &numTexts,
                        NITF_CONV_INT, NITF_INT32_SZ, error))
    {
        nitf_Error_init(error, "Could not retrieve number of texts",
                        NITF_CTXT, NITF_ERR_INVALID_OBJECT);
        return NITF_FAILURE;
    }

    /* TODO - WE NEED TO CHECK IF A WRITER IS SETUP FOR EACH SEGMENT */
    /* IF NOT, WE SHOULD NOT WRITE THE SEGMENT AT ALL, OR SET THE DATA TO 0 */

    textSubLens = NULL;
    textDataLens = NULL;
    if (numTexts != 0)
    {
        textSubLens = (nitf_Off *) NITF_MALLOC(numTexts * sizeof(nitf_Off));
        if (!textSubLens)
        {
            nitf_Error_init(error, NITF_STRERROR(NITF_ERRNO),
                            NITF_CTXT, NITF_ERR_MEMORY);
            return NITF_FAILURE;
        }
        textDataLens = (nitf_Off *) NITF_MALLOC(numTexts * sizeof(nitf_Off));
        if (!textDataLens)
        {
            NITF_FREE(textSubLens);
            nitf_Error_init(error, NITF_STRERROR(NITF_ERRNO),
                            NITF_CTXT, NITF_ERR_MEMORY);
            return NITF_FAILURE;
        }

        iter = nitf_List_begin(writer->record->texts);
        end = nitf_List_end(writer->record->texts);

        startSize = nitf_IOInterface_getSize(writer->output, error);
        if (!NITF_IO_SUCCESS(startSize))
        {
            NITF_FREE(textSubLens);
            NITF_FREE(textDataLens);
            return NITF_FAILURE;
        }
        i = 0; /* reset the counter */
        while (nitf_ListIterator_notEqualTo(&iter, &end))
        {
            nitf_TextSegment *segment =
                (nitf_TextSegment *) nitf_ListIterator_get(&iter);
            if (!writeTextSubheader(writer,
                                    segment->subheader, fver,
                                    error))
            {
                NITF_FREE(textSubLens);
                NITF_FREE(textDataLens);
                return NITF_FAILURE;
            }
            endSize = nitf_IOInterface_getSize(writer->output, error);
            if (!NITF_IO_SUCCESS(endSize))
            {
                NITF_FREE(textSubLens);
                NITF_FREE(textDataLens);
                return NITF_FAILURE;
            }
            textSubLens[i] = endSize - startSize;
            startSize = endSize;
            /* TODO - we need to check to make sure the imageWriter exists */
            if (!writeText(writer->textWriters[i],
                           writer->output, error))
            {
                NITF_FREE(textSubLens);
                NITF_FREE(textDataLens);
                return NITF_FAILURE;
            }
            endSize = nitf_IOInterface_getSize(writer->output, error);
            if (!NITF_IO_SUCCESS(endSize))
            {
                NITF_FREE(textSubLens);
                NITF_FREE(textDataLens);
                return NITF_FAILURE;
            }
            textDataLens[i] = endSize - startSize;
            startSize = endSize;

            nitf_ListIterator_increment(&iter);
            ++i;
        }
    }

    /*******************************************************************/
    /*                   START DEALING WITH DATA EXTENSIONS            */
    /*******************************************************************/
    if (!nitf_Field_get(header->numDataExtensions, &numDEs,
                        NITF_CONV_INT, NITF_INT32_SZ, error))
    {
        nitf_Error_init(error, "Could not retrieve number of data extensions",
                        NITF_CTXT, NITF_ERR_INVALID_OBJECT);
        return NITF_FAILURE;
    }

    /* TODO - WE NEED TO CHECK IF A WRITER IS SETUP FOR EACH SEGMENT */
    /* IF NOT, WE SHOULD NOT WRITE THE SEGMENT AT ALL, OR SET THE DATA TO 0 */

    deSubLens = NULL;
    deDataLens = NULL;
    if (numDEs != 0)
    {

        /* Length of current user subheader */
        uint32_t userSublen;

        deSubLens = (nitf_Off *) NITF_MALLOC(numDEs * sizeof(nitf_Off));
        if (!deSubLens)
        {
            nitf_Error_init(error, NITF_STRERROR(NITF_ERRNO),
                            NITF_CTXT, NITF_ERR_MEMORY);
            return NITF_FAILURE;
        }
        deDataLens = (nitf_Off *) NITF_MALLOC(numDEs * sizeof(nitf_Off));
        if (!deDataLens)
        {
            NITF_FREE(deSubLens);
            nitf_Error_init(error, NITF_STRERROR(NITF_ERRNO),
                            NITF_CTXT, NITF_ERR_MEMORY);
            return NITF_FAILURE;
        }

        iter = nitf_List_begin(writer->record->dataExtensions);
        end = nitf_List_end(writer->record->dataExtensions);

        startSize = nitf_IOInterface_getSize(writer->output, error);
        if (!NITF_IO_SUCCESS(startSize))
        {
            NITF_FREE(deSubLens);
            NITF_FREE(deDataLens);
            return NITF_FAILURE;
        }
        i = 0; /* reset the counter */
        while (nitf_ListIterator_notEqualTo(&iter, &end))
        {
            nitf_DESegment *segment =
                (nitf_DESegment *) nitf_ListIterator_get(&iter);
            if (!nitf_Writer_writeDESubheader(writer,
                                              segment->subheader,
                                              &userSublen, fver, error))
            {
                NITF_FREE(deSubLens);
                NITF_FREE(deDataLens);
                return NITF_FAILURE;
            }
            endSize = nitf_IOInterface_getSize(writer->output, error);
            if (!NITF_IO_SUCCESS(endSize))
            {
                NITF_FREE(deSubLens);
                NITF_FREE(deDataLens);
                return NITF_FAILURE;
            }
            deSubLens[i] = endSize - startSize;
            startSize = endSize;
            /* TODO - we need to check to make sure the imageWriter exists */
            if (!writeDE(writer, writer->dataExtensionWriters[i],
                    segment->subheader, writer->output, error))
            {
                NITF_FREE(deSubLens);
                NITF_FREE(deDataLens);
                return NITF_FAILURE;
            }
            endSize = nitf_IOInterface_getSize(writer->output, error);
            if (!NITF_IO_SUCCESS(endSize))
            {
                NITF_FREE(deSubLens);
                NITF_FREE(deDataLens);
                return NITF_FAILURE;
            }
            deDataLens[i] = endSize - startSize;
            startSize = endSize;

            nitf_ListIterator_increment(&iter);
            ++i;
        }
    }

    /* Fix file and header lengths */

    /*   Get the file length */
    fileLen = nitf_IOInterface_getSize(writer->output, error);

    if (!NITF_IO_SUCCESS(fileLen))
        goto CATCH_ERROR;

    if (!NITF_IO_SUCCESS(nitf_IOInterface_seek(writer->output,
                                               fileLenOff,
                                               NITF_SEEK_SET,
                                               error)))
        goto CATCH_ERROR;

    NITF_WRITE_INT64_FIELD(fileLen, NITF_FL, ZERO, FILL_LEFT);
    if (!nitf_Field_setUint64(header->NITF_FL, fileLen, error))
        goto CATCH_ERROR;

    NITF_WRITE_INT64_FIELD(hdrLen, NITF_HL, ZERO, FILL_LEFT);
    if (!nitf_Field_setUint64(header->NITF_HL, hdrLen, error))
        goto CATCH_ERROR;



    /*    Fix the image subheader and data lengths */
    NITF_WRITE_INT_FIELD(numImgs, NITF_NUMI, ZERO, FILL_LEFT);
    if (!nitf_Field_setUint32(header->NITF_NUMI, numImgs, error))
        goto CATCH_ERROR;
    for (i = 0; i < numImgs; i++)
    {
        NITF_WRITE_INT64_FIELD(imageSubLens[i], NITF_LISH, ZERO, FILL_LEFT);
        if (!nitf_Field_setUint64(header->NITF_LISH(i), imageSubLens[i], error))
            goto CATCH_ERROR;

        NITF_WRITE_INT64_FIELD(imageDataLens[i], NITF_LI, ZERO, FILL_LEFT);
        if (!nitf_Field_setUint64(header->NITF_LI(i), imageDataLens[i], error))
            goto CATCH_ERROR;
    }
    if (numImgs != 0)
    {
        NITF_FREE(imageSubLens);
        NITF_FREE(imageDataLens);
    }

    /*    Fix the graphic subheader and data lengths */
    NITF_WRITE_INT_FIELD(numGraphics, NITF_NUMS, ZERO, FILL_LEFT);
    if (!nitf_Field_setUint32(header->NITF_NUMS, numGraphics, error))
        goto CATCH_ERROR;

    for (i = 0; i < numGraphics; i++)
    {
        NITF_WRITE_INT64_FIELD(graphicSubLens[i], NITF_LSSH, ZERO, FILL_LEFT);
        if (!nitf_Field_setUint64(header->NITF_LSSH(i), graphicSubLens[i], error))
            goto CATCH_ERROR;

        NITF_WRITE_INT64_FIELD(graphicDataLens[i], NITF_LS, ZERO, FILL_LEFT);
        if (!nitf_Field_setUint64(header->NITF_LS(i), graphicDataLens[i], error))
            goto CATCH_ERROR;

    }
    if (numGraphics != 0)
    {
        NITF_FREE(graphicSubLens);
        NITF_FREE(graphicDataLens);
    }

    /* NOW, we need to seek past the other count */
    skipBytes = NITF_NUMX_SZ;
    if (!NITF_IO_SUCCESS(nitf_IOInterface_seek(writer->output,
                                               skipBytes,
                                               NITF_SEEK_CUR,
                                               error)))
        goto CATCH_ERROR;

    /*    Fix the text subheader and data lengths */
    NITF_WRITE_INT_FIELD(numTexts, NITF_NUMT, ZERO, FILL_LEFT);
    if (!nitf_Field_setUint32(header->NITF_NUMT, numTexts, error))
        goto CATCH_ERROR;

    for (i = 0; i < numTexts; i++)
    {
        NITF_WRITE_INT64_FIELD(textSubLens[i], NITF_LTSH, ZERO, FILL_LEFT);
        if (!nitf_Field_setUint64(header->NITF_LTSH(i), textSubLens[i], error))
            goto CATCH_ERROR;


        NITF_WRITE_INT64_FIELD(textDataLens[i], NITF_LT, ZERO, FILL_LEFT);
        if (!nitf_Field_setUint64(header->NITF_LT(i), textSubLens[i], error))
            goto CATCH_ERROR;

    }
    if (numTexts != 0)
    {
        NITF_FREE(textSubLens);
        textSubLens = NULL;
        NITF_FREE(textDataLens);
        textDataLens = NULL;
    }

    /*    Fix the data extension subheader and data lengths */
    NITF_WRITE_INT_FIELD(numDEs, NITF_NUMDES, ZERO, FILL_LEFT);
    if (!nitf_Field_setUint32(header->NITF_NUMDES, numDEs, error))
        goto CATCH_ERROR;

    for (i = 0; i < numDEs; i++)
    {
        NITF_WRITE_INT64_FIELD(deSubLens[i], NITF_LDSH, ZERO, FILL_LEFT);
        if (!nitf_Field_setUint64(header->NITF_LDSH(i), deSubLens[i], error))
            goto CATCH_ERROR;


        NITF_WRITE_INT64_FIELD(deDataLens[i], NITF_LD, ZERO, FILL_LEFT);
        if (!nitf_Field_setUint64(header->NITF_LD(i), deDataLens[i], error))
            goto CATCH_ERROR;


    }
    if (numDEs != 0)
    {
        NITF_FREE(deSubLens);
        deSubLens = NULL;
        NITF_FREE(deDataLens);
        deDataLens = NULL;
    }

    /* Now its time to check if we should be measuring the CLEVEL */
    if (strncmp(header->NITF_CLEVEL->raw, "00", 2) == 0)
    {
        NITF_CLEVEL clevel =
            nitf_ComplexityLevel_measure(writer->record, error);

        if (clevel == NITF_CLEVEL_CHECK_FAILED)
            goto CATCH_ERROR;

        nitf_ComplexityLevel_toString(clevel,
                                      header->NITF_CLEVEL->raw);

        if (!NITF_IO_SUCCESS(nitf_IOInterface_seek(writer->output,
                                                   NITF_FHDR_SZ + NITF_FVER_SZ,
                                                   NITF_SEEK_SET,
                                                   error)))
            goto CATCH_ERROR;


        if (!writeField(writer, header->NITF_CLEVEL->raw, 2, error))
            goto CATCH_ERROR;
    }

    /* if there wasn't a file datetime set, let's set one automatically */
    if (nitf_Utils_isBlank(header->NITF_FDT->raw))
    {
        char *dateFormat = (nitf_Record_getVersion(writer->record) == NITF_VER_20 ?
                NITF_DATE_FORMAT_20 : NITF_DATE_FORMAT_21);

        if (!nitf_Field_setDateTime(header->NITF_FDT, NULL, dateFormat, error))
            goto CATCH_ERROR;

        if (!NITF_IO_SUCCESS(nitf_IOInterface_seek(writer->output,
                NITF_FHDR_SZ + NITF_FVER_SZ + NITF_CLEVEL_SZ + NITF_STYPE_SZ + NITF_OSTAID_SZ,
                NITF_SEEK_SET, error)))
            goto CATCH_ERROR;

        if (!writeField(writer, header->NITF_FDT->raw, NITF_FDT_SZ, error))
            goto CATCH_ERROR;
    }

    nitf_Writer_destructWriters(writer);

    /*  We dont handle anything cool yet  */
    return NITF_SUCCESS;

CATCH_ERROR:

    nitf_Writer_destructWriters(writer);

    if (numImgs != 0)
    {
        NITF_FREE(imageSubLens);
        NITF_FREE(imageDataLens);
    }
    if (numGraphics != 0)
    {
        NITF_FREE(graphicSubLens);
        NITF_FREE(graphicDataLens);
    }
    if (numTexts != 0)
    {
        NITF_FREE(textSubLens);
        NITF_FREE(textDataLens);
    }
    if (numDEs != 0)
    {
        NITF_FREE(deSubLens);
        NITF_FREE(deDataLens);
    }

    return NITF_FAILURE;
}


NITFAPI(NITF_BOOL) nitf_Writer_setImageWriteHandler(nitf_Writer *writer,
        int index, nitf_WriteHandler *writeHandler, nitf_Error * error)
{
    if (index >= writer->numImageWriters)
    {
        nitf_Error_initf(error, NITF_CTXT, NITF_ERR_INVALID_PARAMETER,
                         "index is greater than number of images");
        goto CATCH_ERROR;
    }
    /* destroy any previously existing one */
    if (writer->imageWriters[index])
        nitf_WriteHandler_destruct(&writer->imageWriters[index]);
    writer->imageWriters[index] = writeHandler;
    return NITF_SUCCESS;

CATCH_ERROR:
    return NITF_FAILURE;
}

NITFAPI(NITF_BOOL) nitf_Writer_setGraphicWriteHandler(nitf_Writer *writer,
        int index, nitf_WriteHandler *writeHandler, nitf_Error * error)
{
    if (index >= writer->numGraphicWriters)
    {
        nitf_Error_initf(error, NITF_CTXT, NITF_ERR_INVALID_PARAMETER,
                         "index is greater than number of graphics");
        goto CATCH_ERROR;
    }
    /* destroy any previously existing one */
    if (writer->graphicWriters[index])
        nitf_WriteHandler_destruct(&writer->graphicWriters[index]);
    writer->graphicWriters[index] = writeHandler;
    return NITF_SUCCESS;

CATCH_ERROR:
    return NITF_FAILURE;
}

NITFAPI(NITF_BOOL) nitf_Writer_setTextWriteHandler(nitf_Writer *writer,
        int index, nitf_WriteHandler *writeHandler, nitf_Error * error)
{
    if (index >= writer->numTextWriters)
    {
        nitf_Error_initf(error, NITF_CTXT, NITF_ERR_INVALID_PARAMETER,
                         "index is greater than number of texts");
        goto CATCH_ERROR;
    }
    /* destroy any previously existing one */
    if (writer->textWriters[index])
        nitf_WriteHandler_destruct(&writer->textWriters[index]);
    writer->textWriters[index] = writeHandler;
    return NITF_SUCCESS;

CATCH_ERROR:
    return NITF_FAILURE;
}

NITFAPI(NITF_BOOL) nitf_Writer_setDEWriteHandler(nitf_Writer *writer,
        int index, nitf_WriteHandler *writeHandler, nitf_Error * error)
{
    if (index >= writer->numDataExtensionWriters)
    {
        nitf_Error_initf(error, NITF_CTXT, NITF_ERR_INVALID_PARAMETER,
                         "index is greater than number of images");
        goto CATCH_ERROR;
    }
    /* destroy any previously existing one */
    if (writer->dataExtensionWriters[index])
        nitf_WriteHandler_destruct(&writer->dataExtensionWriters[index]);
    writer->dataExtensionWriters[index] = writeHandler;
    return NITF_SUCCESS;

CATCH_ERROR:
    return NITF_FAILURE;
}


NITFAPI(nitf_ImageWriter *)
nitf_Writer_newImageWriter(nitf_Writer *writer,
                           int index,
                           nrt_HashTable* options,
                           nitf_Error * error)
{
    nitf_ListIterator iter;
    nitf_ImageWriter *imageWriter = NULL;
    nitf_ImageSegment *currentSegment = NULL;

    /* currently always NULL */
    /*nitf_CompressionInterface *compIface = NULL;*/

    if (index >= writer->numImageWriters)
    {
        nitf_Error_initf(error, NITF_CTXT, NITF_ERR_INVALID_PARAMETER,
                         "index is greater than number of images");
        goto CATCH_ERROR;
    }

    iter = nitf_List_at(writer->record->images, index);
    /* this operation will assert if it is the end of the list */
    currentSegment = (nitf_ImageSegment *) nitf_ListIterator_get(&iter);

    assert(currentSegment);
    assert(currentSegment->subheader);

    imageWriter = nitf_ImageWriter_construct(currentSegment->subheader,
                                             options, error);
    if (!imageWriter)
        goto CATCH_ERROR;

    if (!nitf_Writer_setImageWriteHandler(writer, index, imageWriter, error))
        goto CATCH_ERROR;

    return imageWriter;

CATCH_ERROR:
    if (imageWriter)
        nitf_WriteHandler_destruct(&imageWriter);
    return NULL;
}


NITFAPI(nitf_SegmentWriter *) nitf_Writer_newTextWriter
(
    nitf_Writer * writer,
    int index,
    nitf_Error * error
)
{
    nitf_SegmentWriter *segmentWriter = NULL;

    if (index >= writer->numTextWriters)
    {
        nitf_Error_initf(error, NITF_CTXT, NITF_ERR_INVALID_PARAMETER,
                         "i is greater than number of texts");
        return NULL;
    }

    segmentWriter = nitf_SegmentWriter_construct(error);
    if (!segmentWriter)
        return NULL;

    if (!nitf_Writer_setTextWriteHandler(writer, index, segmentWriter, error))
        return NULL;

    return segmentWriter;
}

NITFAPI(nitf_SegmentWriter *) nitf_Writer_newDEWriter
(
    nitf_Writer * writer,
    int index,
    nitf_Error * error
)
{
    nitf_SegmentWriter *segmentWriter = NULL;

    if (index >= writer->numDataExtensionWriters)
    {
        nitf_Error_initf(error, NITF_CTXT, NITF_ERR_INVALID_PARAMETER,
                         "i is greater than number of DE segments");
        return NULL;
    }

    segmentWriter = nitf_SegmentWriter_construct(error);
    if (!segmentWriter)
        return NULL;

    if (!nitf_Writer_setDEWriteHandler(writer, index, segmentWriter, error))
        return NULL;

    return segmentWriter;
}


NITFAPI(nitf_SegmentWriter *) nitf_Writer_newGraphicWriter
(
    nitf_Writer * writer,
    int index,
    nitf_Error * error
)
{
    nitf_SegmentWriter *segmentWriter = NULL;

    if (index >= writer->numGraphicWriters)
    {
        nitf_Error_initf(error, NITF_CTXT, NITF_ERR_INVALID_PARAMETER,
                         "i is greater than number of graphics");
        return NULL;
    }

    segmentWriter = nitf_SegmentWriter_construct(error);
    if (!segmentWriter)
        return NULL;

    if (!nitf_Writer_setGraphicWriteHandler(writer, index, segmentWriter, error))
        return NULL;

    return segmentWriter;
}

