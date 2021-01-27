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
#include "nitf_BlockingInfo.h"
#include "nitf_BlockingInfo_Destructor.h"
#include "nitf_JNI.h"

/*  This creates the _SetObj and _GetObj accessors  */
NITF_JNI_DECLARE_OBJ(nitf_BlockingInfo)

JNIEXPORT jboolean JNICALL Java_nitf_BlockingInfo_00024Destructor_destructMemory
    (JNIEnv * env, jobject self, jlong address)
{
    nitf_BlockingInfo *blockingInfo = (nitf_BlockingInfo*)address;
    if (blockingInfo)
    {
        NITF_FREE(blockingInfo);
        return JNI_TRUE;
    }
    return JNI_FALSE;
}


/*
 * Class:     nitf_BlockingInfo
 * Method:    construct
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_nitf_BlockingInfo_construct
    (JNIEnv * env, jobject self)
{
    nitf_BlockingInfo *blockingInfo =
        (nitf_BlockingInfo *) NITF_MALLOC(sizeof(nitf_BlockingInfo));

    /* set some values */
    blockingInfo->numBlocksPerRow = 0;
    blockingInfo->numBlocksPerCol = 0;
    blockingInfo->numRowsPerBlock = 0;
    blockingInfo->numColsPerBlock = 0;
    blockingInfo->length = 0;

    /*  Set the thing in Java  */
    _SetObj(env, self, blockingInfo);
}


/*
 * Class:     nitf_BlockingInfo
 * Method:    getNumBlocksPerRow
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_nitf_BlockingInfo_getNumBlocksPerRow
    (JNIEnv * env, jobject self)
{
    nitf_BlockingInfo *blockingInfo = _GetObj(env, self);
    return blockingInfo->numBlocksPerRow;
}


/*
 * Class:     nitf_BlockingInfo
 * Method:    setNumBlocksPerRow
 * Signature: (I)V
 */
JNIEXPORT void JNICALL Java_nitf_BlockingInfo_setNumBlocksPerRow
    (JNIEnv * env, jobject self, jint numBlocksPerRow)
{
    nitf_BlockingInfo *blockingInfo = _GetObj(env, self);
    blockingInfo->numBlocksPerRow = numBlocksPerRow;
}


/*
 * Class:     nitf_BlockingInfo
 * Method:    getNumBlocksPerCol
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_nitf_BlockingInfo_getNumBlocksPerCol
    (JNIEnv * env, jobject self)
{
    nitf_BlockingInfo *blockingInfo = _GetObj(env, self);
    return blockingInfo->numBlocksPerCol;
}


/*
 * Class:     nitf_BlockingInfo
 * Method:    setNumBlocksPerCol
 * Signature: (I)V
 */
JNIEXPORT void JNICALL Java_nitf_BlockingInfo_setNumBlocksPerCol
    (JNIEnv * env, jobject self, jint numBlocksPerCol)
{
    nitf_BlockingInfo *blockingInfo = _GetObj(env, self);
    blockingInfo->numBlocksPerCol = numBlocksPerCol;
}


/*
 * Class:     nitf_BlockingInfo
 * Method:    getNumRowsPerBlock
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_nitf_BlockingInfo_getNumRowsPerBlock
    (JNIEnv * env, jobject self)
{
    nitf_BlockingInfo *blockingInfo = _GetObj(env, self);
    return blockingInfo->numRowsPerBlock;
}


/*
 * Class:     nitf_BlockingInfo
 * Method:    setNumRowsPerBlock
 * Signature: (I)V
 */
JNIEXPORT void JNICALL Java_nitf_BlockingInfo_setNumRowsPerBlock
    (JNIEnv * env, jobject self, jint numRowsPerBlock)
{
    nitf_BlockingInfo *blockingInfo = _GetObj(env, self);
    blockingInfo->numRowsPerBlock = numRowsPerBlock;
}


/*
 * Class:     nitf_BlockingInfo
 * Method:    getNumColsPerBlock
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_nitf_BlockingInfo_getNumColsPerBlock
    (JNIEnv * env, jobject self)
{
    nitf_BlockingInfo *blockingInfo = _GetObj(env, self);
    return blockingInfo->numColsPerBlock;
}


/*
 * Class:     nitf_BlockingInfo
 * Method:    setNumColsPerBlock
 * Signature: (I)V
 */
JNIEXPORT void JNICALL Java_nitf_BlockingInfo_setNumColsPerBlock
    (JNIEnv * env, jobject self, jint numColsPerBlock)
{
    nitf_BlockingInfo *blockingInfo = _GetObj(env, self);
    blockingInfo->numColsPerBlock = numColsPerBlock;
}


/*
 * Class:     nitf_BlockingInfo
 * Method:    getLength
 * Signature: ()J
 */
JNIEXPORT jlong JNICALL Java_nitf_BlockingInfo_getLength
    (JNIEnv * env, jobject self)
{

    nitf_BlockingInfo *blockingInfo = _GetObj(env, self);
    return (jlong)blockingInfo->length;
}


/*
 * Class:     nitf_BlockingInfo
 * Method:    setLength
 * Signature: (J)V
 */
JNIEXPORT void JNICALL Java_nitf_BlockingInfo_setLength
    (JNIEnv * env, jobject self, jlong length)
{
    nitf_BlockingInfo *blockingInfo = _GetObj(env, self);
    blockingInfo->length = (size_t)length;
}

