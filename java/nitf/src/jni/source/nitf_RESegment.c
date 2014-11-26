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
#include "nitf_RESegment.h"
#include "nitf_JNI.h"

NITF_JNI_DECLARE_OBJ(nitf_RESegment)
/*
 * Class:     nitf_RESegment
 * Method:    getSubheader
 * Signature: ()Lnitf/RESubheader;
 */
JNIEXPORT jobject JNICALL Java_nitf_RESegment_getSubheader
    (JNIEnv * env, jobject self)
{
    nitf_RESegment *segment = _GetObj(env, self);

    jclass subheaderClass = (*env)->FindClass(env, "nitf/RESubheader");
    jmethodID methodID =
        (*env)->GetMethodID(env, subheaderClass, "<init>", "(J)V");
    return (*env)->NewObject(env,
                             subheaderClass,
                             methodID, (jlong) segment->subheader);
}


/*
 * Class:     nitf_RESegment
 * Method:    getOffset
 * Signature: ()J
 */
JNIEXPORT jlong JNICALL Java_nitf_RESegment_getOffset
    (JNIEnv * env, jobject self)
{
    nitf_RESegment *segment = _GetObj(env, self);
    return segment->offset;
}


/*
 * Class:     nitf_RESegment
 * Method:    getEnd
 * Signature: ()J
 */
JNIEXPORT jlong JNICALL Java_nitf_RESegment_getEnd
    (JNIEnv * env, jobject self)
{
    nitf_RESegment *segment = _GetObj(env, self);
    return segment->end;
}

