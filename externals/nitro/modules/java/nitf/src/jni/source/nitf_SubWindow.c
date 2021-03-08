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
#include "nitf_SubWindow.h"
#include "nitf_SubWindow_Destructor.h"
#include "nitf_JNI.h"

NITF_JNI_DECLARE_OBJ(nitf_SubWindow)

/*
 * Class:     nitf_SubWindow
 * Method:    construct
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_nitf_SubWindow_construct
    (JNIEnv * env, jobject self)
{
    nitf_SubWindow *subWindow;
    nitf_Error error;

    subWindow = nitf_SubWindow_construct(&error);
    if (!subWindow)
    {
        /* throw an error */
        _ThrowNITFException(env, error.message);
    }
    else
    {
        _SetObj(env, self, subWindow);
    }
}

JNIEXPORT jboolean JNICALL Java_nitf_SubWindow_00024Destructor_destructMemory
    (JNIEnv * env, jobject self, jlong address)
{
    nitf_SubWindow *subWindow = (nitf_SubWindow*)address;
    if (subWindow)
    {
        nitf_SubWindow_destruct(&subWindow);
        return JNI_TRUE;
    }
    return JNI_FALSE;
}


/*
 * Class:     nitf_SubWindow
 * Method:    getBandList
 * Signature: ()[I
 */
JNIEXPORT jintArray JNICALL Java_nitf_SubWindow_getBandList
    (JNIEnv * env, jobject self)
{
    nitf_SubWindow *subWindow = _GetObj(env, self);
    jintArray array;
    if (subWindow->bandList && subWindow->numBands > 0)
    {
        array = (*env)->NewIntArray(env, subWindow->numBands);
        (*env)->SetIntArrayRegion(env, array, 0, subWindow->numBands,
                                  subWindow->bandList);
        return array;
    }
    else
    {
        return (*env)->NewIntArray(env, 0);
    }
}


/*
 * Class:     nitf_SubWindow
 * Method:    setNumCols
 * Signature: (I)V
 */
JNIEXPORT void JNICALL Java_nitf_SubWindow_setNumCols
    (JNIEnv * env, jobject self, jint numCols)
{
    nitf_SubWindow *subWindow = _GetObj(env, self);
    subWindow->numCols = numCols;
}


/*
 * Class:     nitf_SubWindow
 * Method:    setBandList
 * Signature: ([I)V
 */
JNIEXPORT void JNICALL Java_nitf_SubWindow_setBandList
    (JNIEnv * env, jobject self, jintArray bands)
{
    nitf_SubWindow *subWindow = _GetObj(env, self);
    if (subWindow->bandList)
    {
        NITF_FREE(subWindow->bandList);
    }
    subWindow->bandList = (*env)->GetIntArrayElements(env, bands, 0);
}


/*
 * Class:     nitf_SubWindow
 * Method:    getStartRow
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_nitf_SubWindow_getStartRow
    (JNIEnv * env, jobject self)
{
    nitf_SubWindow *subWindow = _GetObj(env, self);
    return subWindow->startRow;
}


/*
 * Class:     nitf_SubWindow
 * Method:    setStartRow
 * Signature: (I)V
 */
JNIEXPORT void JNICALL Java_nitf_SubWindow_setStartRow
    (JNIEnv * env, jobject self, jint startRow)
{
    nitf_SubWindow *subWindow = _GetObj(env, self);
    subWindow->startRow = startRow;
}


/*
 * Class:     nitf_SubWindow
 * Method:    getNumRows
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_nitf_SubWindow_getNumRows
    (JNIEnv * env, jobject self)
{
    nitf_SubWindow *subWindow = _GetObj(env, self);
    return subWindow->numRows;
}


/*
 * Class:     nitf_SubWindow
 * Method:    setNumRows
 * Signature: (I)V
 */
JNIEXPORT void JNICALL Java_nitf_SubWindow_setNumRows
    (JNIEnv * env, jobject self, jint numRows)
{
    nitf_SubWindow *subWindow = _GetObj(env, self);
    subWindow->numRows = numRows;
}


/*
 * Class:     nitf_SubWindow
 * Method:    getStartCol
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_nitf_SubWindow_getStartCol
    (JNIEnv * env, jobject self)
{
    nitf_SubWindow *subWindow = _GetObj(env, self);
    return subWindow->startCol;
}


/*
 * Class:     nitf_SubWindow
 * Method:    setStartCol
 * Signature: (I)V
 */
JNIEXPORT void JNICALL Java_nitf_SubWindow_setStartCol
    (JNIEnv * env, jobject self, jint startCol)
{
    nitf_SubWindow *subWindow = _GetObj(env, self);
    subWindow->startCol = startCol;
}


/*
 * Class:     nitf_SubWindow
 * Method:    getNumCols
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_nitf_SubWindow_getNumCols
    (JNIEnv * env, jobject self)
{
    nitf_SubWindow *subWindow = _GetObj(env, self);
    return subWindow->numCols;
}


/*
 * Class:     nitf_SubWindow
 * Method:    getNumBands
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_nitf_SubWindow_getNumBands
    (JNIEnv * env, jobject self)
{
    nitf_SubWindow *subWindow = _GetObj(env, self);
    return subWindow->numBands;
}


/*
 * Class:     nitf_SubWindow
 * Method:    setNumBands
 * Signature: (I)V
 */
JNIEXPORT void JNICALL Java_nitf_SubWindow_setNumBands
    (JNIEnv * env, jobject self, jint numBands)
{
    nitf_SubWindow *subWindow = _GetObj(env, self);
    subWindow->numBands = numBands;
}


/*
 * Class:     nitf_SubWindow
 * Method:    setDownSampler
 * Signature: (Lnitf/DownSampler;)V
 */
JNIEXPORT void JNICALL Java_nitf_SubWindow_setDownSampler
    (JNIEnv * env, jobject self, jobject downSamplerObject)
{
    nitf_SubWindow *subWindow = _GetObj(env, self);
    nitf_DownSampler *downSampler;

    if (downSamplerObject != NULL)
    {
        downSampler = (nitf_DownSampler*)_GetObj(env, downSamplerObject);
        subWindow->downsampler = downSampler;
    }
}


/*
 * Class:     nitf_SubWindow
 * Method:    getDownSampler
 * Signature: ()Lnitf/DownSampler;
 */
JNIEXPORT jobject JNICALL Java_nitf_SubWindow_getDownSampler
    (JNIEnv * env, jobject self)
{
    nitf_SubWindow *subWindow = _GetObj(env, self);
    jobject downSampler;
    jmethodID methodID;
    jclass downSamplerClass = (*env)->FindClass(env, "nitf/DownSampler");

    if (subWindow->downsampler)
    {
        methodID =
            (*env)->GetMethodID(env, downSamplerClass, "<init>", "(J)V");
        downSampler =
            (*env)->NewObject(env, downSamplerClass, methodID,
                              subWindow->downsampler);
        return downSampler;
    }
    /* otherwise, return NULL */
    return NULL;
}

