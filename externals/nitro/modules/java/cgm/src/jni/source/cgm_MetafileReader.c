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

#include "cgm_MetafileReader.h"
#include "cgm_MetafileReader_Destructor.h"
#include "cgm_JNI.h"

CGM_JNI_DECLARE_OBJ(cgm_MetafileReader)

JNIEXPORT void JNICALL Java_cgm_MetafileReader_construct
  (JNIEnv *env, jobject self)
{
    cgm_MetafileReader *reader = NULL;
    nitf_Error error;

    reader = cgm_MetafileReader_construct(&error);
    if (!reader)
        _ThrowNITFException(env, "Unable to create new MetafileReader");
    _SetObj(env, self, reader);
}

JNIEXPORT jboolean JNICALL Java_cgm_MetafileReader_00024Destructor_destructMemory
  (JNIEnv *env, jobject self, jlong address)
{
    cgm_MetafileReader *reader = (cgm_MetafileReader*)address;
    if (reader)
    {
        cgm_MetafileReader_destruct(&reader);
        return JNI_TRUE;
    }
    return JNI_FALSE;
}

JNIEXPORT jobject JNICALL Java_cgm_MetafileReader_read
  (JNIEnv *env, jobject self, jobject interface)
{
    nitf_Error error;
    nitf_IOInterface* io;
    cgm_Metafile *metafile;
    cgm_MetafileReader *reader = _GetObj(env, self);

    /* get some classIDs */
    jclass ioInterfaceClass = (*env)->GetObjectClass(env, interface);
    jclass metafileClass = (*env)->FindClass(env, "cgm/Metafile");

    /* get the IOInterface */
    jmethodID methodID =
        (*env)->GetMethodID(env, ioInterfaceClass, "getAddress", "()J");
    io = (nitf_IOInterface*) ((*env)->CallLongMethod(env, interface, methodID));

    /* just to be sure, seek to start of file */
    if (nitf_IOInterface_seek(io, 0, NITF_SEEK_SET, &error) == -1)
        goto CATCH_ERROR;

    if (!(metafile = cgm_MetafileReader_read(reader, io, &error)))
        goto CATCH_ERROR;

    methodID = (*env)->GetMethodID(env, metafileClass, "<init>", "(J)V");
    return (*env)->NewObject(env, metafileClass, methodID, (jlong) metafile);

  CATCH_ERROR:
    _ThrowNITFException(env, error.message);
    return NULL;
}
