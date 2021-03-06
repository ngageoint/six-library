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
    jbyte *buf, *copyBuf;
    jboolean isCopy;
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
    buf = (*env)->GetByteArrayElements(env, data, &isCopy);
    if (!buf)
    {
        _ThrowNITFException(env, "ERROR getting data from array");
        return;
    }

    if (isCopy == JNI_TRUE)
    {
    	/* this will almost always  be true - most JVMs always copy... */
    	copyBuf = buf;
    }
    else
    {
    	/* we must copy ourselves */
    	copyBuf = (jbyte*)NITF_MALLOC(size);
    	if (!copyBuf)
		{
			_ThrowNITFException(env, "Out of memory");
			return;
		}
    	memcpy(copyBuf, buf, size);
    }

    memorySource =
        nitf_MemorySource_construct(copyBuf, size, start,
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

    if (isCopy != JNI_TRUE)
    {
    	/* if the JVM did not make a copy, we need to tell it we're done with it */
    	(*env)->ReleaseByteArrayElements(env, data, buf, JNI_ABORT);
    }

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

JNIEXPORT jlong JNICALL Java_nitf_MemorySource_getSize
  (JNIEnv *env, jobject self)
{
    nitf_BandSource *source = _GetObj(env, self);
    nitf_Error error;
    nitf_Off size;

    if (0 > (size = source->iface->getSize(source->data, &error)))
    {
        _ThrowNITFException(env, error.message);
        return -1;
    }
    return (jlong)size;
}

JNIEXPORT void JNICALL Java_nitf_MemorySource_setSize
  (JNIEnv *env, jobject self, jlong size)
{
    nitf_BandSource *source = _GetObj(env, self);
    nitf_Error error;

    if (!source->iface->setSize(source->data, (nitf_Off)size, &error))
    {
        _ThrowNITFException(env, error.message);
        return;
    }
}

