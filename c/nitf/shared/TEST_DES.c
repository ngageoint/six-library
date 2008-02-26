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
*   Simple example DES for testing.

  This file defines a very simple Data Extension Segment. This example is
  used for test of the UserSegment object and can be used aa an example
  and starting point for the development of DES.

  This example defines a "standard" DE segment which means the user 
  header can be implemented via a TRE object and there are no out of
  segment dependencies

  The DES ID will be TEST_DES

  The user header will have three fields

    NITF_TEST_DES_COUNT     - Number of data values
    NITF_TEST_DES_START     - Start value in ramp
    NITF_TEST_DES_INCREMENT - Increment between values in ramp

  The data is an 8-bit ramp defined by the three values. in testing, the
  ramp will be setup to contain printable values.

  This example includes a plug-in interface
*/

#include <import/nitf.h>

/*    TRE access helper (-1 is returned on error) */

NITFPRIV(nitf_Uint32) getIntField(nitf_TRE * tre, char *name,
                                  nitf_Error * error)
{
    nitf_Pair *pair;            /* Pair from TRE hash */
    nitf_Uint32 value;          /* Result */

    pair = nitf_HashTable_find(tre->hash, name);
    if (pair == NULL)
        return (NULL);

    if (!nitf_Field_get((nitf_Field *) (pair->data),
                        &value, NITF_CONV_UINT, NITF_INT32_SZ, error))
        return (-1);

    return (value);
}

/*    Instance data for the DES example */

typedef struct _InstanceData
{
    nitf_Uint32 count;          /* Header value, count */
    nitf_Uint32 start;          /* Header value, start */
    nitf_Uint32 increment;      /* Header value, increment */
    nitf_Uint32 left;           /* Amount of bytes left to read */
    nitf_Uint8 current;         /* Current value */
} InstanceData;

/*    TRE description for user header */

static nitf_TREDescription description[] = {
    {NITF_BCS_N, 2, "Number of data values",
     "NITF_TEST_DES_COUNT",
     NITF_VAL_BCS_N_PLUS, NITF_NO_RANGE, NULL, NULL},
    {NITF_BCS_N, 3, "Start value in ramp", "NITF_TEST_DES_START",
     NITF_VAL_BCS_N_PLUS, NITF_NO_RANGE, NULL, NULL},
    {NITF_BCS_N, 2, "Increment between values in ramp",
     "NITF_TEST_DES_INCREMENT",
     NITF_VAL_BCS_N_PLUS, NITF_NO_RANGE, NULL, NULL},
    {NITF_END, 0, NULL, NULL}
};

/*   Band source interface read function */

NITFPRIV(NITF_BOOL) sourceRead(NITF_DATA * instanceData,
                               char *readBuffer, size_t amount,
                               nitf_Error * error)
{
    InstanceData *data;         /* The instance data correctly typed */
    nitf_Uint32 i;

    data = (InstanceData *) instanceData;

    if (amount > data->left)
    {
        nitf_Error_initf(error, NITF_CTXT, NITF_ERR_INVALID_OBJECT,
                         "Request past EOF for User segment source, requested: %d, left: %d",
                         amount, data->left);
        return (NULL);
    }

    for (i = 0; i < amount; i++)
    {
        readBuffer[i] = data->current;
        data->current += data->increment;
    }

    return (NITF_SUCCESS);
}

/* Band source interface destruct function */

NITFPRIV(void) sourceDestruct(NITF_DATA * instanceData)
{
/*  Nothing to do, the instance data is the UserSegment object */

    return;
}

NITFPRIV(size_t) sourceGetSize(NITF_DATA * instanceData)
{
    InstanceData *data = (InstanceData *) instanceData;
    return data->count;
}

/*   Band source interface structue */

static nitf_IDataSource bandImpl = {
    sourceRead,
    sourceDestruct,
    sourceGetSize
};

/*   The source construct function */

NITFPRIV(NITF_BOOL) sourceConstruct(nitf_UserSegment * user,
                                    nitf_Record * record,
                                    nitf_DESegment * des,
                                    nitf_Error * error)
{
/*  Nothing to do, the source instance data is the UserSegment object */

    return (NITF_SUCCESS);
}

/*   The instance setup function */

NITFPRIV(NITF_BOOL) instanceSetup(nitf_UserSegment * user,
                                  nitf_Record * record,
                                  nitf_DESegment * des, NITF_BOOL forWrite,
                                  nitf_Error * error)
{
    nitf_TRE *header;
    InstanceData *instance;     /* The instance data */

    header = des->subheader->subheaderFields;

    instance = (InstanceData *) NITF_MALLOC(sizeof(InstanceData));
    if (instance == NULL)
    {
        nitf_Error_init(error, NITF_STRERROR(NITF_ERRNO), NITF_CTXT,
                        NITF_ERR_MEMORY);
        return NULL;
    }

    if ((instance->count =
         getIntField(header, "NITF_TEST_DES_COUNT", error)) == -1)
    {
        NITF_FREE(instance);
        return (NITF_FAILURE);
    }

    if ((instance->start =
         getIntField(header, "NITF_TEST_DES_START", error)) == -1)
    {
        NITF_FREE(instance);
        return (NITF_FAILURE);
    }
    if ((instance->increment = getIntField(header,
                                           "NITF_TEST_DES_INCREMENT",
                                           error)) == -1)
    {
        NITF_FREE(instance);
        return (NITF_FAILURE);
    }

    instance->left = instance->count;
    instance->current = instance->start;

    user->typeData = (void *) instance;
    return (NITF_SUCCESS);
}

/*    Type specific destruct function */

NITFPRIV(void) destruct(nitf_UserSegment * user)
{

    if (user->typeData != NULL)
        NITF_FREE(user->typeData);
    user->typeData = NULL;
    return;
}


static nitf_IUserSegment interfaceTable = {
    /* DES descriptive name */
    "Example of a simple DE segment used for testing",
    description,                /* Header TRE description */
    NULL,                       /* Create user header (None) */
    instanceSetup,              /* Allocate and initialize instance data */
    NULL,                       /* Read virtual file (None) */
    NULL,                       /* Seek in virtual file (None) */
    sourceConstruct,            /* Create user data source */
    NULL,                       /* Write user header (NONE) */
    destruct,                   /* User specific destructor */
    &bandImpl,                  /* BandSource implementation */
    NULL
};

/*    Plug-in interface */

static char *ident[] = {
    "DES",
    "TEST_DES",
    NULL,
};


NITFAPI(char **) TEST_DES_init(nitf_Error * error)
{
    return ident;               /* Just return the ident */
}

NITFAPI(void *) DES_construct(nitf_Error * error)
{
    return ((void *) &interfaceTable);
}

NITFAPI(void) TEST_DES_cleanup(void)
{
    return;
}
