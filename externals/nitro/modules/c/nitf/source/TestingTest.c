/* =========================================================================
 * This file is part of NITRO
 * =========================================================================
 *
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
  * (C) Copyright 2022, Maxar Technologies, Inc.
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

#include "nitf/TestingTest.h"

#define NITF_Testing_F_SZ   4
#define NITF_Testing_F     f
#define NITF_Testing_F1_SZ   4
#define NITF_Testing_F1     f1

#define _NITF_CONSTRUCT_FIELD(OWNER, ID, TYPE) \
    OWNER->ID = nitf_Field_construct(ID##_SZ, TYPE, error); \
    if (!OWNER->ID) goto CATCH_ERROR;

#define _NITF_DESTRUCT_FIELD(OWNER, ID) \
    if (OWNER->ID) nitf_Field_destruct(OWNER->ID);

NITFAPI(nitf_testing_Test1a*) nitf_testing_Test1a_construct(nitf_Error * error)
{
    NITF_NEW(retval, nitf_testing_Test1a, error, CATCH_ERROR);

    /*  First, we initialize all of our sections to NULL */
    /*  This will prevent problems when we allocate them */
    retval->f1 = NULL;

    /*  Zero out a known block of memory, by ID */
    _NITF_CONSTRUCT_FIELD(retval, NITF_Testing_F1, NITF_BCS_A);

    return retval;

CATCH_ERROR:
    /* destruct if it was allocated */
    if (retval)
        nitf_testing_Test1a_destruct(&retval);
    return NULL;
}

/*
 *  Destruct all sub-objects, and make sure that we
 *  kill this object too.
 */
NITFAPI(void) nitf_testing_Test1a_destruct(nitf_testing_Test1a** obj)
{
    if (!*obj)
        return;

//CATCH_ERROR:     /* need this for the above calls */
    _NITF_DESTRUCT_FIELD(&(*obj), NITF_Testing_F1);

    NITF_FREE(*obj);
    *obj = NULL;
}

NITFAPI(nitf_testing_Test1b*) nitf_testing_Test1b_construct(nitf_Error* error)
{
    NITF_NEW(retval, nitf_testing_Test1b, error, CATCH_ERROR);

    /*  First, we initialize all of our sections to NULL */
    /*  This will prevent problems when we allocate them */
    retval->f = NULL;

    /*  Zero out a known block of memory, by ID */
    _NITF_CONSTRUCT_FIELD(retval, NITF_Testing_F, NITF_BCS_A);

    return retval;

CATCH_ERROR:
    /* destruct if it was allocated */
    if (retval)
        nitf_testing_Test1b_destruct(&retval);
    return NULL;
}

/*
 *  Destruct all sub-objects, and make sure that we
 *  kill this object too.
 */
NITFAPI(void) nitf_testing_Test1b_destruct(nitf_testing_Test1b** obj)
{
    if (!*obj)
        return;

    //CATCH_ERROR:     /* need this for the above calls */
    _NITF_DESTRUCT_FIELD(&(*obj), NITF_Testing_F);

    NITF_FREE(*obj);
    *obj = NULL;
}