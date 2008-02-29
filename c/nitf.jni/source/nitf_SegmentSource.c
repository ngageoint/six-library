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

#include "nitf_SegmentSource.h"
#include "nitf_JNI.h"

/*  This creates the _SetObj and _GetObj accessors  */
/*NITF_JNI_DECLARE_OBJ(nitf_SegmentSource)*/

/*
 * Class:     nitf_SegmentSource
 * Method:    makeSegmentMemorySource
 * Signature: ([BIII)Lnitf/SegmentSource;
 */
JNIEXPORT jobject JNICALL Java_nitf_SegmentSource_makeSegmentMemorySource
  (JNIEnv *env, jclass thisClass, jbyteArray data, jint size, jint start, jint byteSkip)
{
	
	jclass exClass = (*env)->FindClass(env, "nitf/NITFException");
    nitf_Error error;
    char *buf;
    nitf_SegmentSource *memorySource;
    jmethodID methodID;

    if (!data)
    {
        (*env)->ThrowNew(env, exClass, "ERROR, data array is null");
        return NULL;
    }

    /* get the data */
    buf = (char *) (*env)->GetByteArrayElements(env, data, NULL);
    if (!buf)
    {
        (*env)->ThrowNew(env, exClass, "ERROR getting data from array");
        return NULL;
    }

    memorySource =
        nitf_SegmentMemorySource_construct(buf, size, start,
            byteSkip, &error);
    if (!memorySource)
    {
        (*env)->ThrowNew(env, exClass, error.message);
        return NULL;
    }

    methodID =
        (*env)->GetMethodID(env, thisClass, "<init>", "(J)V");
    return (*env)->NewObject(env,
                             thisClass,
                             methodID, (jlong) memorySource);
}


