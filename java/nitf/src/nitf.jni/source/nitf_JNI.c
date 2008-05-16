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

#include "nitf_JNI.h"

NITFPROT(jobject) _GetFieldObj(JNIEnv * env, nitf_Field * field)
{
    jobject fieldObj;
    jmethodID methodID;
    jclass fieldClass = (*env)->FindClass(env, "nitf/Field");
    methodID = (*env)->GetMethodID(env, fieldClass, "<init>", "(J)V");

    fieldObj = (*env)->NewObject(env, fieldClass, methodID, (jlong) field);
    return fieldObj;
}


NITFPROT(nitf_Version) _GetNITFVersion(JNIEnv * env, jobject versionObject)
{
    jclass versionClass = (*env)->GetObjectClass(env, versionObject);

    jfieldID nitf20_fieldID;
    jfieldID nitf21_fieldID;

    jobject nitf20_object;
    jobject nitf21_object;

    nitf_Version version;
    nitf_Error error;

    nitf20_fieldID =
        (*env)->GetStaticFieldID(env, versionClass, "NITF_20",
                                 "Lnitf/Version;");
    nitf21_fieldID =
        (*env)->GetStaticFieldID(env, versionClass, "NITF_21",
                                 "Lnitf/Version;");

    nitf20_object =
        (*env)->GetStaticObjectField(env, versionClass, nitf20_fieldID);
    nitf21_object =
        (*env)->GetStaticObjectField(env, versionClass, nitf21_fieldID);

    if ((*env)->IsSameObject(env, versionObject, nitf20_object) ==
        JNI_TRUE)
        version = NITF_VER_20;
    else if ((*env)->IsSameObject(env, versionObject, nitf21_object) ==
             JNI_TRUE)
        version = NITF_VER_21;
    else
        version = -1;

    return version;
}


NITFPROT(void) _ThrowNITFException(JNIEnv *env, const char *message)
{
    static jclass exceptClass = NULL;
    if (!exceptClass)
    {
        jclass localClass = (*env)->FindClass(env, "nitf/NITFException");
        exceptClass = (*env)->NewGlobalRef(env, localClass);
        (*env)->DeleteLocalRef(env, localClass);
    }
    (*env)->ThrowNew(env, exceptClass, message);
}
