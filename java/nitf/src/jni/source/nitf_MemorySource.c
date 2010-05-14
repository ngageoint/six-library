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

#include <import/nitf.h>
#include "nitf_MemorySource.h"
#include "nitf_JNI.h"

/*  This creates the _SetObj and _GetObj accessors  */
NITF_JNI_DECLARE_OBJ(nitf_BandSource)
/*
 * Class:     nitf_MemorySource
 * Method:    construct
 * Signature: ([BIII)V
 */
JNIEXPORT void JNICALL Java_nitf_MemorySource_construct
    (JNIEnv * env, jobject self, jbyteArray data, jint size, jint start,
     jint numBytesPerPixel, jint pixelSkip)
{
    nitf_Error error;
    char *buf;
    nitf_BandSource *memorySource;

    jclass bandSourceClass = (*env)->FindClass(env, "nitf/BandSource");
    jmethodID methodID = (*env)->GetStaticMethodID(env, bandSourceClass,
        "register", "(Lnitf/BandSource;)V");

    if (!data)
    {
        _ThrowNITFException(env, "ERROR, data array is null");
        return;
    }

    /* get the data */
    buf = (char *) (*env)->GetByteArrayElements(env, data, 0);
    if (!buf)
    {
        _ThrowNITFException(env, "ERROR getting data from array");
        return;
    }

    memorySource =
        nitf_MemorySource_construct(buf, size, start,
            numBytesPerPixel, pixelSkip, &error);
    if (!memorySource)
    {
        _ThrowNITFException(env, error.message);
        return;
    }

    _SetObj(env, self, memorySource);

    /* now, we must also register this type */
    (*env)->CallStaticVoidMethod(env, bandSourceClass,
        methodID, self);

    (*env)->ReleaseByteArrayElements(env, data, buf, 0);

    return;
}


/*
 * Class:     nitf_MemorySource
 * Method:    read
 * Signature: ([BI)V
 */
JNIEXPORT void JNICALL Java_nitf_MemorySource_read
    (JNIEnv * env, jobject self, jbyteArray buf, jint size)
{
    nitf_BandSource *source = _GetObj(env, self);
    jbyte *byteBuf;
    nitf_Error error;

    byteBuf = (*env)->GetByteArrayElements(env, buf, 0);
    if (!byteBuf)
    {
        _ThrowNITFException(env, "ERROR getting data from array");
        return;
    }

    if (!source->iface->read(source->data, (char *) byteBuf, size, &error))
    {
        _ThrowNITFException(env, error.message);
        return;
    }

    (*env)->ReleaseByteArrayElements(env, buf, byteBuf, 0);
    return;
}

