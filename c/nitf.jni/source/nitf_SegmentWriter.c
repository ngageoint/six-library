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

#include "nitf_SegmentWriter.h"
#include "nitf_JNI.h"

/*  This creates the _SetObj and _GetObj accessors  */
NITF_JNI_DECLARE_OBJ(nitf_SegmentWriter)
/*
 * Class:     nitf_SegmentWriter
 * Method:    destructMemory
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_nitf_SegmentWriter_destructMemory
  (JNIEnv *env, jobject self)
{
	nitf_SegmentWriter *writer = _GetObj(env, self);
    if (writer)
    {
        nitf_SegmentWriter_destruct(&writer);
    }
    _SetObj(env, self, NULL);
}

/*
 * Class:     nitf_SegmentWriter
 * Method:    getOutputHandle
 * Signature: ()Lnitf/IOHandle;
 */
JNIEXPORT jobject JNICALL Java_nitf_SegmentWriter_getOutputHandle
  (JNIEnv *env, jobject self)
{
	nitf_SegmentWriter *writer = _GetObj(env, self);
    jclass iohandleClass = (*env)->FindClass(env, "nitf/IOHandle");
    jmethodID methodID =
        (*env)->GetMethodID(env, iohandleClass, "<init>", "(J)V");
    return (*env)->NewObject(env,
                             iohandleClass,
                             methodID, (jlong) writer->outputHandle);
}

/*
 * Class:     nitf_SegmentWriter
 * Method:    getSegmentSource
 * Signature: ()Lnitf/SegmentSource;
 */
JNIEXPORT jobject JNICALL Java_nitf_SegmentWriter_getSegmentSource
  (JNIEnv *env, jobject self)
{
	nitf_SegmentWriter *writer = _GetObj(env, self);
    jclass segmentSourceClass = (*env)->FindClass(env, "nitf/SegmentSource");
    jmethodID methodID =
        (*env)->GetMethodID(env, segmentSourceClass, "<init>", "(J)V");
    return (*env)->NewObject(env,
                             segmentSourceClass,
                             methodID, (jlong) writer->segmentSource);
}

/*
 * Class:     nitf_SegmentWriter
 * Method:    attachSource
 * Signature: (Lnitf/SegmentSource;)Z
 */
JNIEXPORT jboolean JNICALL Java_nitf_SegmentWriter_attachSource
  (JNIEnv *env, jobject self, jobject segmentSource)
{
	nitf_SegmentWriter *writer = _GetObj(env, self);
    jclass segmentSourceClass = (*env)->FindClass(env, "nitf/SegmentSource");
    jclass exClass = (*env)->FindClass(env, "nitf/NITFException");
    nitf_Error error;

    nitf_SegmentSource *source;
    jmethodID methodID =
        (*env)->GetMethodID(env, segmentSourceClass, "getAddress", "()J");
    source =
        (nitf_SegmentSource *) (*env)->CallLongMethod(env, segmentSource,
                                                    methodID);

    if (!nitf_SegmentWriter_attachSource(writer, source, &error))
    {
        (*env)->ThrowNew(env, exClass, error.message);
        return JNI_FALSE;
    }
    return JNI_TRUE;
}



