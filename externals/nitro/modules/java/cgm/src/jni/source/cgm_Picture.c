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

#include "cgm_Picture.h"
#include "cgm_JNI.h"

CGM_JNI_DECLARE_OBJ(cgm_Picture)

JNIEXPORT jstring JNICALL Java_cgm_Picture_getName
(JNIEnv *env, jobject self)
{
    cgm_Picture *picture = _GetObj(env, self);
    return (*env)->NewStringUTF(env, picture->name);
}

JNIEXPORT void JNICALL Java_cgm_Picture_setName
(JNIEnv *env, jobject self, jstring name)
{
    cgm_Picture *picture = _GetObj(env, self);
    if (picture->name)
        NITF_FREE(picture->name);
    picture->name = _NewCharArrayFromString(env, name);
}

JNIEXPORT jobject JNICALL Java_cgm_Picture_getPictureBody
(JNIEnv *env, jobject self)
{
    cgm_Picture *picture = _GetObj(env, self);
    return _NewObject(env, "cgm/PictureBody", (jlong) picture->body);
}

JNIEXPORT jobject JNICALL Java_cgm_Picture_getVDCExtent
(JNIEnv *env, jobject self)
{
    cgm_Picture *picture = _GetObj(env, self);
    return _NewObject(env, "cgm/Rectangle", (jlong) picture->vdcExtent);
}
