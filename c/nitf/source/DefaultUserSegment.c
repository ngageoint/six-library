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

#include "nitf/DefaultUserSegment.h"
#include "nitf/TREUtils.h"

NITFPRIV(nitf_TRE *) createHeader(struct _nitf_IUserSegment *iface,
                                  nitf_Record * record,
                                  nitf_DESubheader * subhdr,
                                  NITF_DATA * data, nitf_Error * error)
{
    nitf_Uint32 headerLen;              /* Length of the user header */
    nitf_TRE *header = NULL;            /* The user header */
    nitf_TREDescription *descr = NULL;  /* holds the TREDescriptions */
    nitf_Field *field = NULL;
	
    /* get the length */
    if (!nitf_Field_get(subhdr->subheaderFieldsLength,
                        &headerLen, NITF_CONV_UINT, NITF_INT32_SZ, error))
        return NULL;

    if (headerLen < 0)
    {
        nitf_Error_init(error, "Invalid header length",
                        NITF_CTXT, NITF_ERR_READING_FROM_FILE);
        goto CATCH_ERROR;
    }

    /* set the description */
    descr =
        (nitf_TREDescription *) NITF_MALLOC(2 *
                                            sizeof(nitf_TREDescription));
    if (!descr)
    {
        nitf_Error_init(error, NITF_STRERROR(NITF_ERRNO),
                        NITF_CTXT, NITF_ERR_INVALID_OBJECT);
        goto CATCH_ERROR;
    }

    descr[0].data_type = NITF_BINARY;
    descr[0].data_count = headerLen;
    descr[0].label = "Unknown raw data";
    descr[0].tag = "raw_data";
    descr[1].data_type = NITF_END;
    descr[1].data_count = 0;
    descr[1].label = NULL;
    descr[1].tag = NULL;

	
	// How on earth?
	
    header = nitf_TRE_createSkeleton("DES", headerLen, error);
	
    if (header == NULL)
        goto CATCH_ERROR;
	header->priv = descr;
   // header->descrip = descr;

    /* just add the raw data to the hash table */
    field = nitf_Field_construct(headerLen, NITF_BINARY, error);
    if (field == NULL)
        goto CATCH_ERROR;
    if (!nitf_Field_setRawData(field, data, headerLen, error))
        goto CATCH_ERROR;
    nitf_HashTable_insert(header->hash, "raw_data", field, error);

    /* no problems */
    return header;

CATCH_ERROR:
    if (header) nitf_TRE_destruct(&header);
    if (descr) NITF_FREE(descr);
    return NULL;
}


static nitf_IUserSegment interfaceTable =
    {
        /* DES descriptive name */
        "Default DE User Segment Implementation",
        NULL,                       /* Header TRE description (none) */
        createHeader,               /* Create user header */
        NULL,                       /* Alloc. and initialize instance data (None) */
        NULL,                       /* Read virtual file (None) */
        NULL,                       /* Seek in virtual file (None) */
        NULL,                       /* Create user data source (None) */
        NULL,                       /* Write user header (NONE) */
        NULL,                       /* User specific destructor (None) */
        NULL,                       /* BandSource implementation (None) */
        NULL
    };

NITFAPI(void *) nitf_DefaultUserSegment_construct(nitf_Error * error)
{
    return ((void *) &interfaceTable);
}
