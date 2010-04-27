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
#include "nitf_ImageReader.h"
#include "nitf_JNI.h"

NITF_JNI_DECLARE_OBJ( nitf_ImageReader)
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
 * Method:    getInput
 * Signature: ()Lnitf/IOInterface;
 */
JNIEXPORT jobject JNICALL Java_nitf_ImageReader_getInput(JNIEnv * env,
                                                         jobject self)
{
    nitf_ImageReader *reader = _GetObj(env, self);
    jclass ioClass = (*env)->FindClass(env, "nitf/IOInterface");

    jmethodID methodID = (*env)->GetMethodID(env, ioClass, "<init>", "(J)V");
    jobject handle = (*env)->NewObject(env, ioClass, methodID,
                                       (jlong) reader->input);
    return handle;
}

/*
 * Class:     nitf_ImageReader
 * Method:    getBlockingInfo
 * Signature: ()Lnitf/BlockingInfo;
 */
JNIEXPORT jobject JNICALL Java_nitf_ImageReader_getBlockingInfo(JNIEnv * env,
                                                                jobject self)
{
    nitf_ImageReader *reader = _GetObj(env, self);
    nitf_Error error;
    nitf_BlockingInfo *info;
    jobject blockingInfoObject;
    jclass blockingInfoClass = (*env)->FindClass(env, "nitf/BlockingInfo");
    jmethodID methodID = (*env)->GetMethodID(env, blockingInfoClass, "<init>",
                                             "(J)V");

    info = nitf_ImageReader_getBlockingInfo(reader, &error);
    if (!info)
    {
        _ThrowNITFException(env, error.message);
        return NULL;
    }

    /* make the object */
    blockingInfoObject = (*env)->NewObject(env, blockingInfoClass, methodID,
                                           (jlong) info);

    /* tell Java not to manage it */
    methodID
            = (*env)->GetMethodID(env, blockingInfoClass, "setManaged", "(Z)V");
    (*env)->CallVoidMethod(env, blockingInfoObject, methodID, JNI_FALSE);

    return blockingInfoObject;
}

/*
 * Class:     nitf_ImageReader
 * Method:    read
 * Signature: (Lnitf/Subimage;[[B)Z
 */
JNIEXPORT jboolean JNICALL Java_nitf_ImageReader_read(JNIEnv *env,
                                                      jobject self,
                                                      jobject subWindow,
                                                      jobjectArray userBuf)
{
    nitf_ImageReader *imReader = _GetObj(env, self);
    jclass subWindowClass = (*env)->FindClass(env, "nitf/SubWindow");
    nitf_SubWindow *nitfSubWindow;
    nitf_Error error;
    jbyte **data;
    jbyteArray byteArray;
    jint padded;
    jsize bands;
    jint i;

    jmethodID methodID = (*env)->GetMethodID(env, subWindowClass, "getAddress",
                                             "()J");
    nitfSubWindow = (nitf_SubWindow *) (*env)->CallLongMethod(env, subWindow,
                                                              methodID);

    bands = (*env)->GetArrayLength(env, userBuf);

    data = (jbyte **) malloc(bands * sizeof(jbyte*));
    for (i = 0; i < bands; ++i)
    {
        byteArray = (*env)->GetObjectArrayElement(env, userBuf, i);
        data[i] = (*env)->GetByteArrayElements(env, byteArray, 0);
        if (!data[i])
        {
            free(data);
            _ThrowNITFException(env, "Out of memory!");
            return JNI_FALSE;
        }
    }
    /* TODO: remove later */
    assert(sizeof(nitf_Uint8) == sizeof(jbyte));
    if (!nitf_ImageReader_read(imReader, nitfSubWindow, (nitf_Uint8 **) data,
                               &padded, &error))
    {
        free(data);
        _ThrowNITFException(env, error.message);
        return JNI_FALSE;
    }

    for (i = 0; i < bands; ++i)
    {
        byteArray = (*env)->GetObjectArrayElement(env, userBuf, i);
        (*env)->ReleaseByteArrayElements(env, byteArray, data[i], 0);
    }
    free(data);
    return padded ? JNI_TRUE : JNI_FALSE;
}

