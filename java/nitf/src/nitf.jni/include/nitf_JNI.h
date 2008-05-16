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

#ifndef __NITF_JNI_H__
#define __NITF_JNI_H__

#include <jni.h>
#include <import/nitf.h>


#define NITF_JNI_DECLARE_OBJ(OBJ_TY) \
static OBJ_TY* _GetObj(JNIEnv* env, jobject self) \
{ \
    jclass thisClass = (*env)->GetObjectClass(env, self); \
    jmethodID methodID = (*env)->GetMethodID(env, thisClass, "getAddress", "()J"); \
   return (OBJ_TY*)((*env)->CallLongMethod(env, self, methodID)); \
} \
static void _SetObj(JNIEnv* env, jobject self, OBJ_TY* info) \
{ \
    jclass thisClass = (*env)->GetObjectClass(env, self); \
    jmethodID methodID = (*env)->GetMethodID(env, thisClass, "setAddress", "(J)V"); \
    (*env)->CallVoidMethod(env, self, methodID, (jlong)info); \
}

NITF_CXX_GUARD


NITFPROT(jobject) _GetFieldObj(JNIEnv* env, nitf_Field* value);

NITFPROT(nitf_Version) _GetNITFVersion(JNIEnv * env, jobject versionObject);

NITFPROT(void) _ThrowNITFException(JNIEnv *env, const char *message);

NITF_CXX_ENDGUARD

#endif

