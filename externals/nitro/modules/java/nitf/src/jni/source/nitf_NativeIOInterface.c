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
#include <stdio.h>
#include "nitf_NativeIOInterface.h"
#include "nitf_IOInterface.h"
#include "nitf_JNI.h"


NITF_JNI_DECLARE_OBJ(nitf_IOInterface)


JNIEXPORT void JNICALL Java_nitf_NativeIOInterface_read
(JNIEnv *env, jobject self, jbyteArray buf, jint size)
{
    nitf_Error error;
    nitf_Off tell;
    jbyte *array = NULL;
    nitf_IOInterface *interface = _GetObj(env, self);

    array = (*env)->GetByteArrayElements(env, buf, 0);
    if (!array)
    {
        _ThrowNITFException(env, "Out of memory!");
        return;
    }

    if (!(interface->iface->read(interface->data, array, size, &error)))
    {
        _ThrowNITFException(env, error.message);
    }
    (*env)->ReleaseByteArrayElements(env, buf, array, 0);
}

JNIEXPORT void JNICALL Java_nitf_NativeIOInterface_write
(JNIEnv *env, jobject self, jbyteArray buf, jint size)
{
    nitf_Error error;
    nitf_Off tell;
    jbyte *array = NULL;
    nitf_IOInterface *interface = _GetObj(env, self);

    array = (*env)->GetByteArrayElements(env, buf, 0);
    if (!array)
    {
        _ThrowNITFException(env, "Out of memory!");
        return;
    }

    if (!(interface->iface->write(interface->data, array, size, &error)))
    {
        _ThrowNITFException(env, error.message);
    }
    (*env)->ReleaseByteArrayElements(env, buf, array, 0);
}

JNIEXPORT jlong JNICALL Java_nitf_NativeIOInterface_seek
(JNIEnv *env, jobject self, jlong offset, jint whence)
{
    nitf_Error error;
    int cWhence;
    nitf_IOInterface *interface = _GetObj(env, self);

    switch(whence)
    {
    case nitf_IOInterface_SEEK_CUR:
        cWhence = NITF_SEEK_CUR;
        break;
    case nitf_IOInterface_SEEK_END:
        cWhence = NITF_SEEK_END;
        break;
    case nitf_IOInterface_SEEK_SET:
        cWhence = NITF_SEEK_SET;
        break;
    default:
        cWhence = NITF_SEEK_SET;
        break;
    }

    if (interface->iface->seek(interface->data, offset, cWhence, &error))
    {
        return interface->iface->tell(interface->data, &error);
    }
    else
    {
        _ThrowNITFException(env, error.message);
        return -1;
    }
}


JNIEXPORT jlong JNICALL Java_nitf_NativeIOInterface_tell(JNIEnv *env,
        jobject self)
{
    nitf_Off tell;
    nitf_Error error;
    nitf_IOInterface *interface = _GetObj(env, self);

    tell = interface->iface->tell(interface->data, &error);
    if (tell == (nitf_Off)NITF_INVALID_HANDLE_VALUE)
    {
        _ThrowNITFException(env, error.message);
        return -1;
    }
    return (jlong)tell;
}

JNIEXPORT jlong JNICALL Java_nitf_NativeIOInterface_getSize(JNIEnv *env,
        jobject self)
{
    nitf_Error error;
    nitf_Off size;
    nitf_IOInterface *interface = _GetObj(env, self);

    size = interface->iface->getSize(interface->data, &error);
    if (size == (nitf_Off)NITF_INVALID_HANDLE_VALUE)
    {
        _ThrowNITFException(env, error.message);
        return -1;
    }
    return (jlong)size;
}

JNIEXPORT void JNICALL Java_nitf_NativeIOInterface_close
(JNIEnv *env, jobject self)
{
    nitf_Error error;
    nitf_IOInterface *interface = _GetObj(env, self);

    if (interface && interface->iface &&
            !interface->iface->close(interface->data, &error))
        _ThrowNITFException(env, error.message);
}

