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
#include "nitf_FileSource.h"
#include "nitf_JNI.h"

/*  This creates the _SetObj and _GetObj accessors  */
NITF_JNI_DECLARE_OBJ(nitf_BandSource)
/*
 * Class:     nitf_FileSource
 * Method:    construct
 * Signature: (Lnitf/IOHandle;JI)V
 */
JNIEXPORT void JNICALL Java_nitf_FileSource_construct
    (JNIEnv * env, jobject self, jobject handle, jlong start,
     jint numBytesPerPixel, jint pixelSkip)
{
    jclass ioHandleClass = (*env)->FindClass(env, "nitf/IOHandle");
    nitf_Error error;
    jmethodID methodID;
    nitf_BandSource *fileSource;
    nitf_IOHandle ioHandle;

    jclass bandSourceClass = (*env)->FindClass(env, "nitf/BandSource");
    jmethodID bandSourceMethodID = (*env)->GetStaticMethodID(env,
        bandSourceClass, "register", "(Lnitf/BandSource;)V");

    methodID =
        (*env)->GetMethodID(env, ioHandleClass, "getIOHandle", "()J");
    ioHandle =
        (nitf_IOHandle) ((*env)->CallLongMethod(env, handle, methodID));

    if (pixelSkip <= 0)
        pixelSkip = 0;

    fileSource =
        nitf_FileSource_construct(ioHandle, start,
            numBytesPerPixel, pixelSkip, &error);
    if (!fileSource)
    {
        _ThrowNITFException(env, error.message);
        return;
    }

    _SetObj(env, self, fileSource);

    /* now, we must also register this type */
    (*env)->CallStaticVoidMethod(env, bandSourceClass,
        bandSourceMethodID, self);

    return;
}


/*
 * Class:     nitf_FileSource
 * Method:    read
 * Signature: ([BI)V
 */
JNIEXPORT void JNICALL Java_nitf_FileSource_read
    (JNIEnv * env, jobject self, jbyteArray buf, jint size)
{
    nitf_BandSource *source = _GetObj(env, self);
    char *byteBuf = NULL;
    nitf_Error error;

    byteBuf = (char*)(*env)->GetByteArrayElements(env, buf, NULL);
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

