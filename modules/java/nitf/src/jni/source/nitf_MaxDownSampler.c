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
#include "nitf_MaxDownSampler.h"
#include "nitf_JNI.h"

/*  This creates the _SetObj and _GetObj accessors  */
NITF_JNI_DECLARE_OBJ(nitf_DownSampler)

/*
 * Class:     nitf_MaxDownSampler
 * Method:    construct
 * Signature: (II)V
 */
JNIEXPORT void JNICALL Java_nitf_MaxDownSampler_construct
  (JNIEnv *env, jobject self, jint rowSkip, jint colSkip)
{
    nitf_Error error;
    jmethodID methodID;
    nitf_DownSampler *downSampler;

    jclass downSamplerClass = (*env)->FindClass(env, "nitf/DownSampler");
    jmethodID downSamplerMethodID = (*env)->GetStaticMethodID(env,
        downSamplerClass, "register", "(Lnitf/DownSampler;)V");

    if (rowSkip <= 0)
        rowSkip = 1;
    if (colSkip <= 0)
        colSkip = 1;

    downSampler =
        nitf_MaxDownSample_construct(rowSkip, colSkip, &error);
    if (!downSampler)
    {
        _ThrowNITFException(env, error.message);
        return;
    }

    _SetObj(env, self, downSampler);

    /* now, we must also register this type */
    (*env)->CallStaticVoidMethod(env, downSamplerClass,
        downSamplerMethodID, self);

    return;
}

/*
 * Class:     nitf_MaxDownSampler
 * Method:    apply
 * Signature: ([[B[[BIIIIIIIII)Z
 */
JNIEXPORT jboolean JNICALL Java_nitf_MaxDownSampler_apply
  (JNIEnv *env, jobject self, jobjectArray inputWindows, jobjectArray outputWindows,
  jint numBands, jint numWindowRows, jint numWindowCols, jint numInputCols,
  jint numCols, jint pixelType, jint pixelSize, jint rowsInLastWindow,
  jint colsInLastWindow)
{
	/* TODO for now, do nothing */
	return JNI_TRUE;	
}

/*
 * Class:     nitf_MaxDownSampler
 * Method:    getRowSkip
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_nitf_MaxDownSampler_getRowSkip
  (JNIEnv *env, jobject self)
{
	nitf_DownSampler *downSampler = _GetObj(env, self);
	return (jint)downSampler->rowSkip;
}

/*
 * Class:     nitf_MaxDownSampler
 * Method:    getColSkip
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_nitf_MaxDownSampler_getColSkip
  (JNIEnv *env, jobject self)
{
	nitf_DownSampler *downSampler = _GetObj(env, self);
	return (jint)downSampler->colSkip;
}

/*
 * Class:     nitf_MaxDownSampler
 * Method:    getMinSupportedBands
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_nitf_MaxDownSampler_getMinSupportedBands
  (JNIEnv *env, jobject self)
{
	nitf_DownSampler *downSampler = _GetObj(env, self);
	return (jint)downSampler->minBands;
}

/*
 * Class:     nitf_MaxDownSampler
 * Method:    getMaxSupportedBands
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_nitf_MaxDownSampler_getMaxSupportedBands
  (JNIEnv *env, jobject self)
{
	nitf_DownSampler *downSampler = _GetObj(env, self);
	return (jint)downSampler->maxBands;
}

/*
 * Class:     nitf_MaxDownSampler
 * Method:    isMultiBand
 * Signature: ()Z
 */
JNIEXPORT jboolean JNICALL Java_nitf_MaxDownSampler_isMultiBand
  (JNIEnv *env, jobject self)
{
	nitf_DownSampler *downSampler = _GetObj(env, self);
	return downSampler->multiBand == 0 ? JNI_FALSE : JNI_TRUE;
}


