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

#include "nitf_Reader.h"
#include "nitf_JNI.h"

NITF_JNI_DECLARE_OBJ(nitf_Reader)
/*
 * Class:     nitf_Reader
 * Method:    construct
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_nitf_Reader_construct
    (JNIEnv * env, jobject self)
{
    nitf_Reader *reader;
    nitf_Error error;

    reader = nitf_Reader_construct(&error);
    _SetObj(env, self, reader);
}


/*
 * Class:     nitf_Reader
 * Method:    destructMemory
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_nitf_Reader_destructMemory
    (JNIEnv * env, jobject self)
{
    nitf_Reader *reader = _GetObj(env, self);
    if (reader)
    {
        nitf_Reader_destruct(&reader);
    }
    _SetObj(env, self, NULL);
}


/*
 * Class:     nitf_Reader
 * Method:    read
 * Signature: (Lnitf/IOHandle;)Lnitf/Record;
 */
JNIEXPORT jobject JNICALL Java_nitf_Reader_read
    (JNIEnv * env, jobject self, jobject handle)
{
    nitf_Reader *reader = _GetObj(env, self);
    nitf_Error error;
    jobject record;
    nitf_IOHandle ioHandle;

    /* get some classIDs */
    jclass ioHandleClass = (*env)->FindClass(env, "nitf/IOHandle");
    jclass recordClass = (*env)->FindClass(env, "nitf/Record");

    /* get the IOHandle */
    jmethodID methodID =
        (*env)->GetMethodID(env, ioHandleClass, "getIOHandle", "()J");
    ioHandle =
        (nitf_IOHandle) ((*env)->CallLongMethod(env, handle, methodID));

    /* just to be sure, seek to start of file */
    if (nitf_IOHandle_seek(ioHandle, 0, NITF_SEEK_SET, &error) == -1)
    {
        goto CATCH_ERROR;
    }

    if (!nitf_Reader_read(reader, ioHandle, &error))
        goto CATCH_ERROR;

    methodID = (*env)->GetMethodID(env, recordClass, "<init>", "(J)V");
    record = (*env)->NewObject(env,
                               recordClass,
                               methodID, (jlong) reader->record);
    return record;

  CATCH_ERROR:
    _ThrowNITFException(env, error.message);
    return NULL;
}


/*
 * Class:     nitf_Reader
 * Method:    getNewImageReader
 * Signature: (I)Lnitf/ImageReader;
 */
JNIEXPORT jobject JNICALL Java_nitf_Reader_getNewImageReader
    (JNIEnv * env, jobject self, jint imageSegmentNumber)
{
    nitf_Reader *reader = _GetObj(env, self);
    nitf_ImageReader *imageReader;
    nitf_Error error;
    jclass imageReaderClass = (*env)->FindClass(env, "nitf/ImageReader");
    jobject imageReaderObject;

    jmethodID methodID =
        (*env)->GetMethodID(env, imageReaderClass, "<init>", "(J)V");
    imageReader =
        nitf_Reader_newImageReader(reader, imageSegmentNumber, &error);

    if (!imageReader)
    {
        _ThrowNITFException(env, error.message);
        return NULL;
    }

    imageReaderObject = (*env)->NewObject(env,
                                          imageReaderClass,
                                          methodID, (jlong) imageReader);
    return imageReaderObject;
}


/*
 * Class:     nitf_Reader
 * Method:    getNewGraphicReader
 * Signature: (I)Lnitf/SegmentReader;
 */
JNIEXPORT jobject JNICALL Java_nitf_Reader_getNewGraphicReader
    (JNIEnv * env, jobject self, jint graphicSegmentNumber)
{
    nitf_Reader *reader = _GetObj(env, self);
    nitf_SegmentReader *segmentReader;
    nitf_Error error;
    jclass segmentReaderClass =
        (*env)->FindClass(env, "nitf/SegmentReader");
    jobject segmentReaderObject;

    jmethodID methodID =
        (*env)->GetMethodID(env, segmentReaderClass, "<init>", "(J)V");
    segmentReader =
        nitf_Reader_newGraphicReader(reader, graphicSegmentNumber, &error);

    if (!segmentReader)
    {
        _ThrowNITFException(env, error.message);
        return NULL;
    }

    segmentReaderObject = (*env)->NewObject(env,
                                            segmentReaderClass,
                                            methodID,
                                            (jlong) segmentReader);
    return segmentReaderObject;
}


/*
 * Class:     nitf_Reader
 * Method:    getNewTextReader
 * Signature: (I)Lnitf/SegmentReader;
 */
JNIEXPORT jobject JNICALL Java_nitf_Reader_getNewTextReader
    (JNIEnv * env, jobject self, jint textSegmentNumber)
{
    nitf_Reader *reader = _GetObj(env, self);
    nitf_SegmentReader *segmentReader;
    nitf_Error error;
    jclass segmentReaderClass =
        (*env)->FindClass(env, "nitf/SegmentReader");
    jobject segmentReaderObject;

    jmethodID methodID =
        (*env)->GetMethodID(env, segmentReaderClass, "<init>", "(J)V");
    segmentReader =
        nitf_Reader_newTextReader(reader, textSegmentNumber, &error);

    if (!segmentReader)
    {
        _ThrowNITFException(env, error.message);
        return NULL;
    }

    segmentReaderObject = (*env)->NewObject(env,
                                            segmentReaderClass,
                                            methodID,
                                            (jlong) segmentReader);
    return segmentReaderObject;
}


/*
 * Class:     nitf_Reader
 * Method:    getNewDEReader
 * Signature: (I)Lnitf/DEReader;
 */
JNIEXPORT jobject JNICALL Java_nitf_Reader_getNewDEReader
    (JNIEnv * env, jobject self, jint deSegmentNumber)
{
    nitf_Reader *reader = _GetObj(env, self);
    nitf_DEReader *deReader;
    nitf_Error error;
    jclass readerClass = (*env)->FindClass(env, "nitf/DEReader");
    jobject readerObject = NULL;

    jmethodID methodID =
        (*env)->GetMethodID(env, readerClass, "<init>", "(J)V");
    deReader = nitf_Reader_newDEReader(reader, deSegmentNumber, &error);

    if (!deReader)
    {
        _ThrowNITFException(env, error.message);
        return NULL;
    }

    readerObject = (*env)->NewObject(env,
                                     readerClass,
                                     methodID, (jlong) deReader);
    return readerObject;
}


/*
 * Class:     nitf_Reader
 * Method:    getInputHandle
 * Signature: ()Lnitf/IOHandle;
 */
JNIEXPORT jobject JNICALL Java_nitf_Reader_getInputHandle
    (JNIEnv * env, jobject self)
{
    nitf_Reader *reader = _GetObj(env, self);
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
 * Class:     nitf_Reader
 * Method:    getRecord
 * Signature: ()Lnitf/Record;
 */
JNIEXPORT jobject JNICALL Java_nitf_Reader_getRecord
    (JNIEnv * env, jobject self)
{
    nitf_Reader *reader = _GetObj(env, self);
    jclass recordClass = (*env)->FindClass(env, "nitf/Record");

    jmethodID methodID =
        (*env)->GetMethodID(env, recordClass, "<init>", "(J)V");
    jobject handle = (*env)->NewObject(env,
                                       recordClass,
                                       methodID,
                                       (jlong) reader->record);
    
    /* tell Java not to manage the ImageSource memory */
    methodID = (*env)->GetMethodID(env, recordClass, "setManaged", "(Z)V");
    (*env)->CallVoidMethod(env, handle, methodID, JNI_FALSE);
    
    return handle;
}

