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

#include "nitf_ImageWriter.h"
#include "nitf_JNI.h"

/*  This creates the _SetObj and _GetObj accessors  */
NITF_JNI_DECLARE_OBJ(nitf_ImageWriter)
/*
 * Class:     nitf_ImageWriter
 * Method:    destructMemory
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_nitf_ImageWriter_destructMemory
    (JNIEnv * env, jobject self)
{
    nitf_ImageWriter *writer = _GetObj(env, self);
    if (writer)
    {
        nitf_ImageWriter_destruct(&writer);
    }
    _SetObj(env, self, NULL);
}


/*
 * Class:     nitf_ImageWriter
 * Method:    getOutputHandle
 * Signature: ()Lnitf/IOHandle;
 */
JNIEXPORT jobject JNICALL Java_nitf_ImageWriter_getOutputHandle
    (JNIEnv * env, jobject self)
{
    nitf_ImageWriter *writer = _GetObj(env, self);
    jclass iohandleClass = (*env)->FindClass(env, "nitf/IOHandle");
    jmethodID methodID =
        (*env)->GetMethodID(env, iohandleClass, "<init>", "(J)V");
    return (*env)->NewObject(env,
                             iohandleClass,
                             methodID, (jlong) writer->outputHandle);
}


/*
 * Class:     nitf_ImageWriter
 * Method:    getImageSource
 * Signature: ()Lnitf/ImageSource;
 */
JNIEXPORT jobject JNICALL Java_nitf_ImageWriter_getImageSource
    (JNIEnv * env, jobject self)
{
    nitf_ImageWriter *writer = _GetObj(env, self);
    jclass imageSourceClass = (*env)->FindClass(env, "nitf/ImageSource");
    jmethodID methodID =
        (*env)->GetMethodID(env, imageSourceClass, "<init>", "(J)V");
    return (*env)->NewObject(env,
                             imageSourceClass,
                             methodID, (jlong) writer->imageSource);
}


/*
 * Class:     nitf_ImageWriter
 * Method:    write
 * Signature: (IIIII)Z
 */
JNIEXPORT jboolean JNICALL Java_nitf_ImageWriter_write
    (JNIEnv * env, jobject self, jint numBitsPerPixel, jint numImageBands,
     jint numMultispectralImageBands, jint numRows, jint numCols)
{
    nitf_ImageWriter *writer = _GetObj(env, self);
    jclass exClass = (*env)->FindClass(env, "nitf/NITFException");
    nitf_Error error;

    if (!nitf_ImageWriter_write
        (writer, numBitsPerPixel, numImageBands,
         numMultispectralImageBands, numRows, numCols, &error))
    {
        (*env)->ThrowNew(env, exClass, error.message);
        return JNI_FALSE;
    }
    return JNI_TRUE;
}


/*
 * Class:     nitf_ImageWriter
 * Method:    attachSource
 * Signature: (Lnitf/ImageSource;)Z
 */
JNIEXPORT jboolean JNICALL Java_nitf_ImageWriter_attachSource
    (JNIEnv * env, jobject self, jobject imageSource)
{
    nitf_ImageWriter *writer = _GetObj(env, self);
    jclass imageSourceClass = (*env)->FindClass(env, "nitf/ImageSource");
    jclass exClass = (*env)->FindClass(env, "nitf/NITFException");
    nitf_Error error;

    nitf_ImageSource *source;
    jmethodID methodID =
        (*env)->GetMethodID(env, imageSourceClass, "getAddress", "()J");
    source =
        (nitf_ImageSource *) (*env)->CallLongMethod(env, imageSource,
                                                    methodID);

    if (!nitf_ImageWriter_attachSource(writer, source, &error))
    {
        (*env)->ThrowNew(env, exClass, error.message);
        return JNI_FALSE;
    }
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

