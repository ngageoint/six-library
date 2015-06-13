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
#include "nitf_SegmentReader.h"
#include "nitf_SegmentReader_Destructor.h"
#include "nitf_IOHandle.h"
#include "nitf_JNI.h"

NITF_JNI_DECLARE_OBJ(nitf_SegmentReader)

JNIEXPORT jboolean JNICALL Java_nitf_SegmentReader_00024Destructor_destructMemory
    (JNIEnv * env, jobject self, jlong address)
{
    nitf_SegmentReader *reader = (nitf_SegmentReader*)address;
    if (reader)
    {
        /* nitf_Reader.c: Java_nitf_Reader_getNewImageReader sets this */
        if (reader->input)
            _ManageObject(env, (jlong)reader->input, JNI_TRUE);

        nitf_SegmentReader_destruct(&reader);
        return JNI_TRUE;
    }
    return JNI_FALSE;
}


/*
 * Class:     nitf_SegmentReader
 * Method:    read
 * Signature: ([BI)Z
 */
JNIEXPORT jboolean JNICALL Java_nitf_SegmentReader_read
    (JNIEnv * env, jobject self, jbyteArray buf, jint size)
{
    nitf_SegmentReader *reader = _GetObj(env, self);
    nitf_Error error;
    jint success;

    jbyte *byteBuf = (*env)->GetByteArrayElements(env, buf, 0);
    if (!byteBuf)
    {
        _ThrowNITFException(env, "Out of memory!");
        return JNI_FALSE;
    }

    if (!nitf_SegmentReader_read(reader, (char *) byteBuf, size, &error))
    {
        _ThrowNITFException(env, error.message);
        return JNI_FALSE;
    }
    (*env)->ReleaseByteArrayElements(env, buf, byteBuf, 0);
    return JNI_TRUE;
}

/*
 * Class:     nitf_SegmentReader
 * Method:    seek
 * Signature: (JI)J
 */
JNIEXPORT jlong JNICALL Java_nitf_SegmentReader_seek
    (JNIEnv * env, jobject self, jlong offset, jint whence)
{
    nitf_SegmentReader *reader = _GetObj(env, self);
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

    seek = nitf_SegmentReader_seek(reader, offset, seekInt, &error);

    /* check for error */
    if (!NITF_IO_SUCCESS(seek))
    {
        _ThrowNITFException(env, error.message);
    }

    return seek;
}


/*
 * Class:     nitf_SegmentReader
 * Method:    tell
 * Signature: ()J
 */
JNIEXPORT jlong JNICALL Java_nitf_SegmentReader_tell
    (JNIEnv * env, jobject self)
{
    nitf_SegmentReader *reader = _GetObj(env, self);
    nitf_Error error;
    jlong tell;

    tell = nitf_SegmentReader_tell(reader, &error);

    /* check for error */
    if (tell == -1)
    {
        _ThrowNITFException(env, error.message);
    }
    return tell;
}


/*
 * Class:     nitf_SegmentReader
 * Method:    getSize
 * Signature: ()J
 */
JNIEXPORT jlong JNICALL Java_nitf_SegmentReader_getSize
    (JNIEnv * env, jobject self)
{
    nitf_SegmentReader *reader = _GetObj(env, self);
    nitf_Error error;
    jlong size;

    size = nitf_SegmentReader_getSize(reader, &error);

    /* check for error */
    if (size == -1)
    {
        _ThrowNITFException(env, error.message);
    }
    return size;
}

