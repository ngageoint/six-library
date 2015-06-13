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
#include "nitf_TextSegment.h"
#include "nitf_JNI.h"

NITF_JNI_DECLARE_OBJ(nitf_TextSegment)
/*
 * Class:     nitf_TextSegment
 * Method:    getSubheader
 * Signature: ()Lnitf/TextSubheader;
 */
JNIEXPORT jobject JNICALL Java_nitf_TextSegment_getSubheader
    (JNIEnv * env, jobject self)
{

    nitf_TextSegment *segment = _GetObj(env, self);
    jclass subheaderClass = (*env)->FindClass(env, "nitf/TextSubheader");
    jmethodID methodID =
        (*env)->GetMethodID(env, subheaderClass, "<init>", "(J)V");

    return (*env)->NewObject(env,
                             subheaderClass,
                             methodID, (jlong) segment->subheader);
}


/*
 * Class:     nitf_TextSegment
 * Method:    getOffset
 * Signature: ()J
 */
JNIEXPORT jlong JNICALL Java_nitf_TextSegment_getOffset
    (JNIEnv * env, jobject self)
{

    nitf_TextSegment *segment = _GetObj(env, self);
    return segment->offset;
}


/*
 * Class:     nitf_TextSegment
 * Method:    getEnd
 * Signature: ()J
 */
JNIEXPORT jlong JNICALL Java_nitf_TextSegment_getEnd
    (JNIEnv * env, jobject self)
{
    nitf_TextSegment *segment = _GetObj(env, self);
    return segment->end;
}

