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

/*
 *    Implementation of the UserSegment object
 */

#include "nitf/Debug.h"
#include "nitf/PluginRegistry.h"
#include "nitf/UserSegment.h"

/*=====================  nitf_UserSegment_getInterface =======================*/

NITFAPI(nitf_IUserSegment *) nitf_UserSegment_getInterface(char *desID,
        int *bad,
        nitf_Error *
        error)
{
    nitf_PluginRegistry *reg;   /* The plug-in registry */
    /* Constructor for interface */
    NITF_PLUGIN_DES_CONSTRUCT_FUNCTION constructDesIface;
    nitf_IUserSegment *desIface;        /* The DE interface (the result) */

    *bad = 0;

    /*    Get the plug-in registry */

    reg = nitf_PluginRegistry_getInstance(error);
    if (!reg)
    {
        *bad = 1;               /* Error as opposed to not there */
        return (NULL);
    }

    /*   Get the constructor */

    constructDesIface =
        nitf_PluginRegistry_retrieveDESConstructor(reg, desID, bad, error);

    if (*bad)                   /*  The plugin registry encountered an error */
        return (NULL);

    /*    Check for  no DES registered, log and ignore  */

    if (constructDesIface == NULL)
    {
        /* set to the default DE interface */
        desIface = (nitf_IUserSegment *)
                   nitf_DefaultUserSegment_construct(error);
    }
    else
    {
        desIface = (nitf_IUserSegment *) (*constructDesIface) (error);
    }
    if (desIface == NULL)
    {
        *bad = 1;
        return (NULL);
    }

    return (desIface);
}


/*=====================  nitf_UserSegment_decodeUserHeader ===================*/

NITFAPI(nitf_TRE *) nitf_UserSegment_decodeUserHeader(nitf_Record * record,
        nitf_DESubheader *
        subhdr,
        NITF_DATA * buffer,
        nitf_Error * error)
{
    nitf_IUserSegment *iface;   /* The interface */
    nitf_Uint32 headerLen;      /* Length of the user header */
    nitf_TRE *header;           /* The user header */
    int bad;                    /* DES look-up failure flag */
    char desID[NITF_DESTAG_SZ + 2];     /* DE type ID */

    /* get the desID */
    if (!nitf_Field_get(subhdr->typeID,
                        (NITF_DATA *) desID, NITF_CONV_STRING,
                        NITF_DESTAG_SZ + 1, error))
    {
        return (NULL);
    }
    nitf_Field_trimString(desID);

    /*   Get the interface object */
    if ((iface =
                nitf_UserSegment_getInterface(desID, &bad, error)) == NULL)
        return (NULL);

    /*   Get required values from DES segment */

    if (!nitf_Field_get(subhdr->subheaderFieldsLength,
                        &headerLen, NITF_CONV_UINT, NITF_INT32_SZ, error))
        return (NULL);

    /*   Call the create header interface function or TRE constructor */

    if (iface->createHeader != NULL)
    {
        header =
            (*(iface->createHeader)) (iface, record, subhdr, buffer,
                                      error);
        if (header == NULL)
            return (NULL);
    }
    else if (iface->headerDesc != NULL)
    {
        /* no need to call TRE_construct b/c we don't want the auto-filled fields */
        header = nitf_TRE_createSkeleton("DES", headerLen, error);
        if (header == NULL)
            return (NULL);
        header->descrip = iface->headerDesc;

        if (buffer != NULL)
        {
            if (!nitf_TRE_parse(header, (char *) buffer, error))
            {
                nitf_TRE_destruct(&header);
                return (NULL);
            }
        }
    }
    else
    {
        nitf_Error_initf(error, NITF_CTXT, NITF_ERR_INVALID_OBJECT,
                         "Invalid User segment inteface, no user header constructor");
        return (NULL);
    }

    subhdr->subheaderFields = header;

    return (header);
}


/*=====================  nitf_UserSegment_construct ==========================*/

NITFAPI(nitf_UserSegment *) nitf_UserSegment_construct(nitf_Record *
        record,
        nitf_DESegment *
        des,
        NITF_BOOL forWrite,
        nitf_Error * error)
{
    nitf_IUserSegment *iface;   /* The interface */
    nitf_UserSegment *user;     /* The result */
    nitf_Uint32 usrHdrLen;      /* Length of the user header */
    int bad;                    /* DES look-up failure flag */
    char desID[NITF_DESTAG_SZ + 2];     /* DE type ID */

    /* get the desID */
    if (!nitf_Field_get(des->subheader->NITF_DESTAG,
                        (NITF_DATA *) desID, NITF_CONV_STRING,
                        NITF_DESTAG_SZ + 1, error))
    {
        return (NULL);
    }
    nitf_Field_trimString(desID);

    /*   Get the interface object */
    if ((iface =
                nitf_UserSegment_getInterface(desID, &bad, error)) == NULL)
        return (NITF_FAILURE);

    /*   Get The user subheader length */

    if (!nitf_Field_get(des->subheader->subheaderFieldsLength,
                        &usrHdrLen, NITF_CONV_UINT, NITF_INT32_SZ, error))
        return (NITF_FAILURE);

    /*    Allocate memory for the object */

    user = (nitf_UserSegment *) NITF_MALLOC(sizeof(nitf_UserSegment));
    if (user == NULL)
    {
        nitf_Error_init(error, NITF_STRERROR(NITF_ERRNO), NITF_CTXT,
                        NITF_ERR_MEMORY);
        return NULL;
    }
    memset(user, sizeof(nitf_UserSegment), 0);
    user->iface = iface;

    /*   Call the type specific instance setup function if it exists */

    user->typeData = NULL;
    if (iface->instanceSetup != NULL)
    {
        if (!
                ((*(iface->instanceSetup))
                 (user, record, des, forWrite, error)))
        {
            nitf_UserSegment_destruct(&user);
            return (NULL);
        }
    }

    /*    Setup the rest of the fields */

    /*
       Note: The offset points to the user data since the user header is part
       of the DES subheader
     */

    if (!forWrite)
    {
        user->dataLength = des->end - des->offset;
        user->baseOffset = des->offset;
        user->virtualLength = user->dataLength;
        user->virtualOffset = 0;
    }
    else
    {
        nitf_Error_initf(error, NITF_CTXT, NITF_ERR_INVALID_OBJECT,
                         "DES write not implemented\n", desID);
        nitf_UserSegment_destruct(&user);
        return (NULL);
    }

    return (user);
}


/*=====================  nitf_UserSegment_constructSource ===============*/

NITFPROT(nitf_DataSource *)
nitf_UserSegment_sourceConstruct(nitf_UserSegment * user,
                                 nitf_Record * record,
                                 nitf_DESegment * des, nitf_Error * error)
{
    nitf_DataSource *bandSource;        /* The result */

    bandSource = (nitf_DataSource *) NITF_MALLOC(sizeof(nitf_DataSource));
    if (!bandSource)
    {
        nitf_Error_init(error, NITF_STRERROR(NITF_ERRNO),
                        NITF_CTXT, NITF_ERR_MEMORY);
        return NULL;
    }

    bandSource->data = user;
    bandSource->iface = user->iface->bandImpl;

    if (!(*(user->iface->sourceConstruct)) (user, record, des, error))
    {
        NITF_FREE(bandSource);
        return (NULL);
    }
    return (bandSource);
}


/*=====================  nitf_UserSegment_destruct ===========================*/

NITFAPI(void) nitf_UserSegment_destruct(nitf_UserSegment ** user)
{

    /*   Destroy instance data, if it exists */

    if ((*user)->typeData != NULL)
        (*((*user)->iface->destruct)) ((*user));

    /*   Free the object */

    NITF_FREE(*user);

    *user = NULL;
    return;
}
