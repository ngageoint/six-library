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

#include <stdio.h>
#include "nitf_IOHandle.h"

/*
 * Class:     nitf_IOHandle
 * Method:    read
 * Signature: (I[BI)Z
 */
JNIEXPORT void JNICALL Java_nitf_IOHandle_read
    (JNIEnv * env, jobject this, jbyteArray buf, jint size)
{
    jclass thisClass = (*env)->GetObjectClass(env, this);
    jmethodID methodID =
        (*env)->GetMethodID(env, thisClass, "getIOHandle", "()J");
    jlong ioHandle = (*env)->CallLongMethod(env, this, methodID);
    nitf_Error error;
    jint success;

    jbyte *byteBuf = (*env)->GetByteArrayElements(env, buf, 0);
    success = nitf_IOHandle_read(ioHandle, (char *) byteBuf, size, &error);

    /* Check for errors */
    if (NITF_IO_SUCCESS(success))
    {
        (*env)->SetByteArrayRegion(env, buf, 0, size, byteBuf);
    }
    else
    {
        jclass exClass = (*env)->FindClass(env, "nitf/NITFException");
        (*env)->ThrowNew(env, exClass, error.message);
    }
}


/*
 * Class:     nitf_IOHandle
 * Method:    createHandle
 * Signature: (Ljava/lang/String;I)J
 */
JNIEXPORT jlong JNICALL Java_nitf_IOHandle_createHandle
    (JNIEnv * env, jobject this, jstring fileName, jint accessFlag,
     jint createFlag)
{
    /*  The IO handle  */
    nitf_IOHandle io;
    nitf_Error error;
    jint accessInt = 0;
    jint createInt = 0;
    jclass exClass;

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
        if (error.level)
        {
            exClass = (*env)->FindClass(env, "nitf/NITFException");
            (*env)->ThrowNew(env, exClass, error.message);
        }
        return -1;
    }
    return io;
}


/*
 * Class:     nitf_IOHandle
 * Method:    write
 * Signature: (I[BI)Z
 */
JNIEXPORT void JNICALL Java_nitf_IOHandle_write
    (JNIEnv * env, jobject this, jbyteArray buf, jint size)
{
    jclass thisClass = (*env)->GetObjectClass(env, this);
    jmethodID methodID =
        (*env)->GetMethodID(env, thisClass, "getIOHandle", "()J");
    jlong ioHandle = (*env)->CallLongMethod(env, this, methodID);
    jclass exClass = (*env)->FindClass(env, "nitf/NITFException");
    nitf_Error error;
    jint success;
    jbyte *byteBuf = (*env)->GetByteArrayElements(env, buf, 0);

    success =
        nitf_IOHandle_write(ioHandle, (char *) byteBuf, size, &error);

    /* Check for errors */
    if (!NITF_IO_SUCCESS(success))
    {
        (*env)->ThrowNew(env, exClass, error.message);
    }
}


/*
 * Class:     nitf_IOHandle
 * Method:    seek
 * Signature: (IJI)J
 */
JNIEXPORT jlong JNICALL Java_nitf_IOHandle_seek
    (JNIEnv * env, jobject this, jlong offset, jint whence)
{
    jclass thisClass = (*env)->GetObjectClass(env, this);
    jmethodID methodID =
        (*env)->GetMethodID(env, thisClass, "getIOHandle", "()J");
    jlong ioHandle = (*env)->CallLongMethod(env, this, methodID);
    jclass exClass = (*env)->FindClass(env, "nitf/NITFException");
    nitf_Error error;
    jint seekInt;
    jlong seek;

    if (whence == nitf_IOHandle_SEEK_CUR)
    {
        seekInt = NITF_SEEK_CUR;
    }
    else if (whence == nitf_IOHandle_SEEK_SET)
    {
        seekInt = NITF_SEEK_SET;
    }
    else if (whence == nitf_IOHandle_SEEK_END)
    {
        seekInt = NITF_SEEK_END;
    }
    else
    {
        return -1;
    }

    seek = nitf_IOHandle_seek(ioHandle, offset, seekInt, &error);

    /* check for error */
    if (!NITF_IO_SUCCESS(seek))
    {
        (*env)->ThrowNew(env, exClass, error.message);
    }

    return seek;
}


/*
 * Class:     nitf_IOHandle
 * Method:    tell
 * Signature: (I)J
 */
JNIEXPORT jlong JNICALL Java_nitf_IOHandle_tell(JNIEnv * env, jobject this)
{
    jclass thisClass = (*env)->GetObjectClass(env, this);
    jmethodID methodID =
        (*env)->GetMethodID(env, thisClass, "getIOHandle", "()J");
    jlong ioHandle = (*env)->CallLongMethod(env, this, methodID);
    jclass exClass = (*env)->FindClass(env, "nitf/NITFException");
    nitf_Error error;

    jlong tell = nitf_IOHandle_tell(ioHandle, &error);

    if (tell == -1)
    {
        (*env)->ThrowNew(env, exClass, error.message);
    }
    return tell;
}


/*
 * Class:     nitf_IOHandle
 * Method:    getSize
 * Signature: (I)J
 */
JNIEXPORT jlong JNICALL Java_nitf_IOHandle_getSize
    (JNIEnv * env, jobject this)
{
    jclass thisClass = (*env)->GetObjectClass(env, this);
    jmethodID methodID =
        (*env)->GetMethodID(env, thisClass, "getIOHandle", "()J");
    jlong ioHandle = (*env)->CallLongMethod(env, this, methodID);
    jclass exClass = (*env)->FindClass(env, "nitf/NITFException");
    nitf_Error error;

    jlong size = nitf_IOHandle_getSize(ioHandle, &error);

    if (size == -1)
    {
        (*env)->ThrowNew(env, exClass, error.message);
    }
    return size;
}


/*
 * Class:     nitf_IOHandle
 * Method:    close
 * Signature: (I)V
 */
JNIEXPORT void JNICALL Java_nitf_IOHandle_close(JNIEnv * env, jobject this)
{
    jclass thisClass = (*env)->GetObjectClass(env, this);
    jmethodID methodID =
        (*env)->GetMethodID(env, thisClass, "getIOHandle", "()J");
    jlong ioHandle = (*env)->CallLongMethod(env, this, methodID);
    jclass exClass = (*env)->FindClass(env, "nitf/NITFException");
    nitf_IOHandle_close(ioHandle);
    ioHandle = NULL;

    methodID = (*env)->GetMethodID(env, thisClass, "setIOHandle", "(J)V");
    (*env)->CallVoidMethod(env, this, methodID, ioHandle);
}

