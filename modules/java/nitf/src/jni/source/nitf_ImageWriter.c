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
#include "nitf_ImageWriter.h"
#include "nitf_ImageWriter_Destructor.h"
#include "nitf_JNI.h"

/*  This creates the _SetObj and _GetObj accessors  */
NITF_JNI_DECLARE_OBJ(nitf_ImageWriter)

JNIEXPORT jboolean JNICALL Java_nitf_ImageWriter_00024Destructor_destructMemory
    (JNIEnv * env, jobject self, jlong address)
{
    nitf_WriteHandler *writer = (nitf_WriteHandler*)address;
    if (writer)
    {
        nitf_WriteHandler_destruct(&writer);
        return JNI_TRUE;
    }
    return JNI_FALSE;
}


/*
 * Class:     nitf_ImageWriter
 * Method:    attachSource
 * Signature: (Lnitf/ImageSource;)Z
 */
JNIEXPORT jboolean JNICALL Java_nitf_ImageWriter_attachSource
    (JNIEnv * env, jobject self, jobject jImageSource)
{
    nitf_ImageWriter *writer = _GetObj(env, self);
    jclass imageSourceClass = (*env)->FindClass(env, "nitf/ImageSource");
    nitf_Error error;
    nitf_ImageSource *imageSource = NULL;
    jmethodID methodID =
        (*env)->GetMethodID(env, imageSourceClass, "getAddress", "()J");
    
    imageSource = (nitf_ImageSource *) (*env)->CallLongMethod(
            env, jImageSource, methodID);

    if (!nitf_ImageWriter_attachSource(writer, imageSource, &error))
    {
        _ThrowNITFException(env, error.message);
        return JNI_FALSE;
    }
    
    /* tell Java not to manage the ImageSource memory */
    _ManageObject(env, (jlong)imageSource, JNI_FALSE);
    return JNI_TRUE;
}


/*
 * Class:     nitf_ImageWriter
 * Method:    setWriteCaching
 * Signature: (Z)Z
 */
JNIEXPORT jboolean JNICALL Java_nitf_ImageWriter_setWriteCaching
  (JNIEnv *env, jobject self, jboolean flag)
{
    int intFlag;
    nitf_ImageWriter *writer = _GetObj(env, self);
    
    intFlag = (flag == JNI_TRUE) ? 1 : 0;
    intFlag = nitf_ImageWriter_setWriteCaching(writer, intFlag);
    return intFlag ? JNI_TRUE : JNI_FALSE;
}

