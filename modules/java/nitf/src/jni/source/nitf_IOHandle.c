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

#include <import/nitf.h>
#include <stdio.h>
#include "nitf_IOHandle.h"
#include "nitf_JNI.h"

NITF_JNI_DECLARE_OBJ(nitf_IOInterface)

JNIEXPORT void JNICALL Java_nitf_IOHandle_createHandle
(JNIEnv *env, jobject this, jstring fileName,
        jint accessFlag, jint createFlag)
{
    /*  The IO handle  */
    nitf_IOHandle io;
    nitf_Error error;
    jint accessInt = 0;
    jint createInt = 0;
    nitf_IOInterface *interface = NULL;
    jmethodID methodID;
    jclass selfClass = (*env)->GetObjectClass(env, this);

    const char *fName = (*env)->GetStringUTFChars(env, fileName, 0);

    if (accessFlag == nitf_IOHandle_NITF_ACCESS_READONLY)
    {
        accessInt = NITF_ACCESS_READONLY;
    }
    else if (accessFlag == nitf_IOHandle_NITF_ACCESS_WRITEONLY)
    {
        accessInt = NITF_ACCESS_WRITEONLY;
    }
    else if (accessFlag == nitf_IOHandle_NITF_ACCESS_READWRITE)
    {
        accessInt = NITF_ACCESS_READWRITE;
    }

    if (createFlag & nitf_IOHandle_NITF_CREATE)
    {
        createInt = NITF_CREATE;
    }
    else if (createFlag & nitf_IOHandle_NITF_TRUNCATE)
    {
        createInt = NITF_TRUNCATE;
    }
    else if (createFlag & nitf_IOHandle_NITF_OPEN_EXISTING)
    {
        createInt = NITF_OPEN_EXISTING;
    }

    io = nitf_IOHandle_create(fName, accessInt, createInt, &error);
    (*env)->ReleaseStringUTFChars(env, fileName, fName);
    if ( NITF_INVALID_HANDLE(io))
    {
        _ThrowNITFException(env, error.message);
        return;
    }

    /* now, we must adapt this IOHandle to fit into the IOInterface */
    /* get a nitf_IOInterface* object... */
    if (!(interface = nitf_IOHandleAdapter_construct(io, accessInt, &error)))
    {
        _ThrowNITFException(env, error.message);
        return;
    }

    _SetObj(env, this, interface);
}

