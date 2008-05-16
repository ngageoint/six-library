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

#include "nitf_UserSegment.h"
#include "nitf_JNI.h"

NITF_JNI_DECLARE_OBJ(nitf_UserSegment)
/*
 * Class:     nitf_UserSegment
 * Method:    getDataLength
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_nitf_UserSegment_getDataLength
    (JNIEnv * env, jobject self)
{
    nitf_UserSegment *segment = _GetObj(env, self);
    return (jint) segment->dataLength;
}


/*
 * Class:     nitf_UserSegment
 * Method:    getBaseOffset
 * Signature: ()J
 */
JNIEXPORT jlong JNICALL Java_nitf_UserSegment_getBaseOffset
    (JNIEnv * env, jobject self)
{
    nitf_UserSegment *segment = _GetObj(env, self);
    return (jlong) segment->baseOffset;
}


/*
 * Class:     nitf_UserSegment
 * Method:    getVirtualLength
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_nitf_UserSegment_getVirtualLength
    (JNIEnv * env, jobject self)
{
    nitf_UserSegment *segment = _GetObj(env, self);
    return (jint) segment->virtualLength;
}


/*
 * Class:     nitf_UserSegment
 * Method:    getVirtualOffset
 * Signature: ()J
 */
JNIEXPORT jlong JNICALL Java_nitf_UserSegment_getVirtualOffset
    (JNIEnv * env, jobject self)
{
    nitf_UserSegment *segment = _GetObj(env, self);
    return (jlong) segment->virtualOffset;
}

