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
        version = NITF_VER_UNKNOWN;

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

NITFPROT(int) _GetJNIEnv(JavaVM** vm, JNIEnv** env)
{
    jsize num = 0;
    jint status = JNI_GetCreatedJavaVMs(vm, 1, &num);
    status = (**vm)->GetEnv(*vm, (void**)env, JNI_VERSION_1_4);
    
    if (env == NULL && status == JNI_EDETACHED)
    {
        //attach the current thread
        status = (**vm)->AttachCurrentThread(*vm, (void**)env, NULL);
        return 1;
    }
    return 0;
}

NITFPROT(void) _ManageObject(JNIEnv * env, jlong address, jboolean flag)
{
    /*jclass clazz = (*env)->GetObjectClass(env, object);
    jmethodID methodID = (*env)->GetMethodID(env, clazz, "setManaged", "(Z)V");
    (*env)->CallVoidMethod(env, object, methodID, flag);*/
    jobject manager;
    jclass managerClass;
    jmethodID methodID;
    managerClass = (*env)->FindClass(env, "nitf/NITFResourceManager");
    methodID = (*env)->GetStaticMethodID(env, managerClass, "getInstance",
                                              "()Lnitf/NITFResourceManager;");
    manager = (*env)->CallStaticObjectMethod(env, managerClass, methodID);
    if (flag)
    {
        methodID = (*env)->GetMethodID(env, managerClass,
                                       "decrementRefCount", "(JZ)V");
    }
    else
    {
        methodID = (*env)->GetMethodID(env, managerClass,
                                       "incrementRefCount", "(JZ)V");
    }
    (*env)->CallVoidMethod(env, manager, methodID, address, JNI_TRUE);
}


NITFPROT(jobject) _NewObject(JNIEnv* env, jlong address, const char* clazzName)
{
    jclass clazz = (*env)->FindClass(env, clazzName);
    jmethodID methodID = (*env)->GetMethodID(env, clazz, "<init>", "(J)V");
    return (*env)->NewObject(env, clazz, methodID, address);
}
