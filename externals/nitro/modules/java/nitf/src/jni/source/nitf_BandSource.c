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
#include "nitf_BandSource.h"
#include "nitf_BandSource_Destructor.h"
#include "nitf_JNI.h"

/*  This creates the _SetObj and _GetObj accessors  */
NITF_JNI_DECLARE_OBJ(nitf_BandSource)
/* Temporary data structure for holding important data */
typedef struct _BandSourceImpl
{
    jobject self;               /* The current object */
} BandSourceImpl;


/*
 *  Private read implementation for file source.
 */
NITFPRIV(NITF_BOOL) BandSource_read
    (NITF_DATA * data, char *buf, nitf_Off size, nitf_Error * error)
{
    jclass bandSourceClass = NULL;
    jmethodID methodID = NULL;
    BandSourceImpl *impl = NULL;
    jbyteArray byteArray = NULL;
    jbyte* temp;
    JNIEnv *env = NULL;
    JavaVM *vm = NULL;
    int detach;

    /* cast it to the structure we know about */
    impl = (BandSourceImpl *) data;
    
    detach = _GetJNIEnv(&vm, &env);
    
    bandSourceClass = (*env)->GetObjectClass(env, impl->self);
    methodID =
        (*env)->GetMethodID(env, bandSourceClass, "read",
                                  "([BI)V");

    /* create new array */
    byteArray = (*env)->NewByteArray(env, (jsize) size);
    
    if (!byteArray)
    {
        if (detach)
            (*vm)->DetachCurrentThread(vm);
    	return NITF_FAILURE;
    }

    /* read the data */
    (*env)->CallVoidMethod(env, impl->self, methodID,
                                 byteArray, size);

    /* copy to the char buffer */
    temp = (*env)->GetByteArrayElements(env, byteArray, 0);
    if (!temp)
    {
        return NITF_FAILURE;
    }
    memcpy(buf, (char*)temp, size);
    
    /* delete the local refs */ 
    (*env)->ReleaseByteArrayElements(env, byteArray, temp, 0);
    (*env)->DeleteLocalRef(env, byteArray);
    
    if (detach)
        (*vm)->DetachCurrentThread(vm);
    return NITF_SUCCESS;
}


NITFPRIV(void) BandSource_destruct(NITF_DATA * data)
{
    jclass bandSourceClass = NULL;
    jmethodID methodID = NULL;
    BandSourceImpl *impl = NULL;
    JNIEnv *env = NULL;
    JavaVM *vm = NULL;
    int detach;
    
    detach = _GetJNIEnv(&vm, &env);

    if (data)
    {
        impl = (BandSourceImpl *) data;
        bandSourceClass =
            (*env)->GetObjectClass(env, (jobject) impl->self);

        /* Call the Java object to tell it we are done using it */
        /*(*env)->CallVoidMethod(env, impl->self, methodID); */

        /* Delete the global ref */
        (*env)->DeleteGlobalRef(env, impl->self);
        NITF_FREE(data);
    }
    
    if (detach)
        (*vm)->DetachCurrentThread(vm);
}

NITFPRIV(nitf_Off) BandSource_getSize(NITF_DATA *data, nitf_Error *error)
{
    jclass bandSourceClass = NULL;
    jmethodID methodID = NULL;
    BandSourceImpl *impl = NULL;
    JNIEnv *env = NULL;
    JavaVM *vm = NULL;
    int detach;
    jlong val = 0;

    detach = _GetJNIEnv(&vm, &env);

    impl = (BandSourceImpl *) data;
    bandSourceClass =
        (*env)->GetObjectClass(env, (jobject) impl->self);

    methodID = (*env)->GetMethodID(env, bandSourceClass, "getSize",
                                      "()L");
    val = (*env)->CallLongMethod(env, impl->self, methodID);

    if (detach)
        (*vm)->DetachCurrentThread(vm);
    return (nitf_Off)val;
}

NITFPRIV(NITF_BOOL) BandSource_setSize(NITF_DATA * data, nitf_Off size, nitf_Error *error)
{
    jclass bandSourceClass = NULL;
    jmethodID methodID = NULL;
    BandSourceImpl *impl = NULL;
    JNIEnv *env = NULL;
    JavaVM *vm = NULL;
    int detach;

    detach = _GetJNIEnv(&vm, &env);

    impl = (BandSourceImpl *) data;
    bandSourceClass =
        (*env)->GetObjectClass(env, (jobject) impl->self);

    methodID = (*env)->GetMethodID(env, bandSourceClass, "setSize",
                                      "(L)V");
    (*env)->CallVoidMethod(env, impl->self, methodID, (jlong)size);

    if (detach)
        (*vm)->DetachCurrentThread(vm);
    return NITF_SUCCESS;
}


JNIEXPORT void JNICALL Java_nitf_BandSource_construct
    (JNIEnv * env, jobject self)
{
    /* make the interface */
    static nitf_IDataSource iBandSource = {
        &BandSource_read,
        &BandSource_destruct,
        &BandSource_getSize,
        &BandSource_setSize
    };

    /* the return bandSource */
    nitf_BandSource *bandSource = NULL;
    BandSourceImpl *impl;       /* gets malloc'd for this object */
    nitf_Error error;
    jclass bandSourceClass = (*env)->FindClass(env, "nitf/BandSource");
    jmethodID methodID = (*env)->GetStaticMethodID(env, bandSourceClass,
        "register", "(Lnitf/BandSource;)V");

    /* construct the persisent one */
    impl = (BandSourceImpl *) NITF_MALLOC(sizeof(BandSourceImpl));
    if (!impl)
    {
        _ThrowNITFException(env, "Out of memory");
        return;
    }

    /**************************************************************/
    /* THIS IS VERY IMPORTANT... WE MUST MAKE A STRONG GLOBAL REF */
    /**************************************************************/
    impl->self = (*env)->NewGlobalRef(env, self);

    bandSource = (nitf_BandSource *) NITF_MALLOC(sizeof(nitf_BandSource));
    if (!bandSource)
    {
        _ThrowNITFException(env, "Out of Memory");
        return;
    }
    bandSource->data = impl;
    bandSource->iface = &iBandSource;

    _SetObj(env, self, bandSource);

    /* now, we must also register this type */
    (*env)->CallStaticVoidMethod(env, bandSourceClass,
        methodID, self);
}

JNIEXPORT jboolean JNICALL Java_nitf_BandSource_00024Destructor_destructMemory
    (JNIEnv * env, jobject self, jlong address)
{
    nitf_BandSource *bandSource = (nitf_BandSource*)address;
    if (bandSource)
    {
        nitf_BandSource_destruct(&bandSource);
        return JNI_TRUE;
    }
    return JNI_FALSE;
}

