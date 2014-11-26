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
#include "nitf_ImageSegment.h"
#include "nitf_JNI.h"

NITF_JNI_DECLARE_OBJ(nitf_ImageSegment)
/*
 * Class:     nitf_ImageSegment
 * Method:    getSubheader
 * Signature: ()Lnitf/ImageSubheader;
 */
JNIEXPORT jobject JNICALL Java_nitf_ImageSegment_getSubheader
    (JNIEnv * env, jobject self)
{

    nitf_ImageSegment *segment = _GetObj(env, self);
    jclass imageClass = (*env)->FindClass(env, "nitf/ImageSubheader");
    jobject imageSubheader;
    jmethodID methodID =
        (*env)->GetMethodID(env, imageClass, "<init>", "(J)V");
    imageSubheader =
        (*env)->NewObject(env, imageClass, methodID,
                          (jlong) segment->subheader);
    return imageSubheader;
}


/*
 * Class:     nitf_ImageSegment
 * Method:    getImageOffset
 * Signature: ()J
 */
JNIEXPORT jlong JNICALL Java_nitf_ImageSegment_getImageOffset
    (JNIEnv * env, jobject self)
{
    nitf_ImageSegment *segment = _GetObj(env, self);
    return (jlong)segment->imageOffset;
}


/*
 * Class:     nitf_ImageSegment
 * Method:    getImageEnd
 * Signature: ()J
 */
JNIEXPORT jlong JNICALL Java_nitf_ImageSegment_getImageEnd
    (JNIEnv * env, jobject self)
{
    nitf_ImageSegment *segment = _GetObj(env, self);
    return (jlong)segment->imageEnd;
}

