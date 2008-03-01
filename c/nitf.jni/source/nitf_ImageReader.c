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

#include "nitf_ImageReader.h"
#include "nitf_JNI.h"

NITF_JNI_DECLARE_OBJ(nitf_ImageReader)
/*
 * Class:     nitf_ImageReader
 * Method:    destructMemory
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_nitf_ImageReader_destructMemory
    (JNIEnv * env, jobject self)
{
    nitf_ImageReader *reader = _GetObj(env, self);
    if (reader)
    {
        nitf_ImageReader_destruct(&reader);
    }
    _SetObj(env, self, NULL);
}


/*
 * Class:     nitf_ImageReader
 * Method:    getInputHandle
 * Signature: ()Lnitf/IOHandle;
 */
JNIEXPORT jobject JNICALL Java_nitf_ImageReader_getInputHandle
    (JNIEnv * env, jobject self)
{
    nitf_ImageReader *reader = _GetObj(env, self);
    jclass ioHandleClass = (*env)->FindClass(env, "nitf/IOHandle");

    jmethodID methodID =
        (*env)->GetMethodID(env, ioHandleClass, "<init>", "(J)V");
    jobject handle = (*env)->NewObject(env,
                                       ioHandleClass,
                                       methodID,
                                       (jlong) reader->inputHandle);
    return handle;
}


/*
 * Class:     nitf_ImageReader
 * Method:    getBlockingInfo
 * Signature: ()Lnitf/BlockingInfo;
 */
JNIEXPORT jobject JNICALL Java_nitf_ImageReader_getBlockingInfo
    (JNIEnv * env, jobject self)
{
    nitf_ImageReader *reader = _GetObj(env, self);
    nitf_Error error;
    nitf_BlockingInfo *info;
    jobject blockingInfoObject;
    jclass exClass = (*env)->FindClass(env, "nitf/NITFException");
    jclass blockingInfoClass = (*env)->FindClass(env, "nitf/BlockingInfo");
    jmethodID methodID =
        (*env)->GetMethodID(env, blockingInfoClass, "<init>", "(J)V");

    info = nitf_ImageReader_getBlockingInfo(reader, &error);
    if (!info)
    {
        (*env)->ThrowNew(env, exClass, error.message);
        return NULL;
    }

    /* make the object */
    blockingInfoObject = (*env)->NewObject(env,
                                           blockingInfoClass,
                                           methodID, (jlong) info);
    return blockingInfoObject;
}


/*
 * Class:     nitf_ImageReader
 * Method:    read
 * Signature: (Lnitf/Subimage;[[B)Z
 */
JNIEXPORT jboolean JNICALL Java_nitf_ImageReader_read
    (JNIEnv * env, jobject self, jobject subWindow, jobjectArray userBuf)
{
    nitf_ImageReader *imReader = _GetObj(env, self);
    jclass subWindowClass = (*env)->FindClass(env, "nitf/SubWindow");
    jclass exClass = (*env)->FindClass(env, "nitf/NITFException");
    nitf_SubWindow *nitfSubWindow;
    nitf_Error error;
    jbyte **data;
    jbyteArray byteArray;
    jint padded;
    jsize bands;
    jsize length;
    jint i;

    jmethodID methodID =
        (*env)->GetMethodID(env, subWindowClass, "getAddress", "()J");
    nitfSubWindow =
        (nitf_SubWindow *) (*env)->CallLongMethod(env, subWindow,
                                                  methodID);

    bands = (*env)->GetArrayLength(env, userBuf);

    data = (jbyte **) malloc(8 * bands);
    for (i = 0; i < bands; ++i)
    {
        byteArray = (*env)->GetObjectArrayElement(env, userBuf, i);
        data[i] = (*env)->GetByteArrayElements(env, byteArray, 0);
    }
    /* TODO: remove later */
    assert(sizeof(nitf_Uint8) == sizeof(jbyte));
    if (!nitf_ImageReader_read(nitf_ImageReader,
                               nitfSubWindow,
                               (nitf_Uint8 **) data, &padded, &error))
    {
        free(data);
        (*env)->ThrowNew(env, exClass, error.message);
        return JNI_FALSE;
    }

    for (i = 0; i < bands; ++i)
    {
        byteArray = (*env)->GetObjectArrayElement(env, userBuf, i);
        length = (*env)->GetArrayLength(env, byteArray);
        (*env)->SetByteArrayRegion(env, byteArray, 0, length, data[i]);
    }
    free(data);
    return JNI_TRUE;
}

