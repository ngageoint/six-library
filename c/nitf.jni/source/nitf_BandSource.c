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

#include "nitf_BandSource.h"
#include "nitf_JNI.h"

/*  This creates the _SetObj and _GetObj accessors  */
NITF_JNI_DECLARE_OBJ(nitf_BandSource)
/* Temporary data structure for holding important data */
typedef struct _BandSourceImpl
{
    JNIEnv *env;                /* The JNI Environment */
    jobject self;               /* The current object */
} BandSourceImpl;

/*
 *  Private read implementation for file source.
 */
NITFPRIV(NITF_BOOL) BandSource_read
    (NITF_DATA * data, char *buf, size_t size, nitf_Error * error)
{
    jclass bandSourceClass = NULL;
    jmethodID methodID = NULL;
    BandSourceImpl *impl = NULL;
    jbyteArray byteArray = NULL;
    jbyte* temp;

    /* cast it to the structure we know about */
    impl = (BandSourceImpl *) data;
    
    bandSourceClass = (*impl->env)->GetObjectClass(impl->env, impl->self);
    methodID =
        (*impl->env)->GetMethodID(impl->env, bandSourceClass, "read",
                                  "([BI)V");

    /* create new array */
    byteArray = (*impl->env)->NewByteArray(impl->env, (jsize) size);
    
    if (!byteArray)
    {
    	return NITF_FAILURE;
    }

    /* read the data */
    (*impl->env)->CallVoidMethod(impl->env, impl->self, methodID,
                                 byteArray, size);

    /* copy to the char buffer */
    temp = (*impl->env)->GetByteArrayElements(impl->env, byteArray, 0);
    memcpy(buf, (char*)temp, size);
    
    /* delete the local refs */ 
    (*impl->env)->ReleaseByteArrayElements(impl->env, byteArray, temp, 0);
    (*impl->env)->DeleteLocalRef(impl->env, byteArray);
    
    return NITF_SUCCESS;
}


NITFPRIV(void) BandSource_destruct(NITF_DATA * data)
{
    jclass bandSourceClass = NULL;
    jmethodID methodID = NULL;
    BandSourceImpl *impl = NULL;

    if (data)
    {
        impl = (BandSourceImpl *) data;
        bandSourceClass =
            (*impl->env)->GetObjectClass(impl->env, (jobject) impl->self);

        /* Call the Java object to tell it we are done using it */
        /*(*impl->env)->CallVoidMethod(impl->env, impl->self, methodID); */

        /* Delete the global ref */
        (*impl->env)->DeleteGlobalRef(impl->env, impl->self);
        NITF_FREE(data);
    }
}


/*
 * Class:     nitf_BandSource
 * Method:    construct
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_nitf_BandSource_construct
    (JNIEnv * env, jobject self)
{
    /* make the interface */
    static nitf_IDataSource iBandSource = {
        &BandSource_read,
        &BandSource_destruct
    };

    /* the return bandSource */
    nitf_BandSource *bandSource = NULL;
    BandSourceImpl *impl;       /* gets malloc'd for this object */
    nitf_Error error;
    jclass exClass = (*env)->FindClass(env, "nitf/NITFException");
    jclass bandSourceClass = (*env)->FindClass(env, "nitf/BandSource");
    jmethodID methodID = (*env)->GetStaticMethodID(env, bandSourceClass,
        "register", "(Lnitf/BandSource;)V");

    /* construct the persisent one */
    impl = (BandSourceImpl *) NITF_MALLOC(sizeof(BandSourceImpl));
    if (!impl)
    {
        /* TODO do something about errors */
        /*nitf_Error_init(&error, NITF_STRERROR(NITF_ERRNO), NITF_CTXT,
           NITF_ERR_MEMORY); */
        (*env)->ThrowNew(env, exClass, "Out of memory");
        return;
    }

    /* fill the data structure */
    impl->env = env;

    /**************************************************************/
    /* THIS IS VERY IMPORTANT... WE MUST MAKE A STRONG GLOBAL REF */
    /**************************************************************/
    impl->self = (*env)->NewGlobalRef(env, self);

    bandSource = (nitf_BandSource *) NITF_MALLOC(sizeof(nitf_BandSource));
    if (!bandSource)
    {
        /*nitf_Error_init(&error, NITF_STRERROR(NITF_ERRNO), NITF_CTXT,
           NITF_ERR_MEMORY); */
        (*env)->ThrowNew(env, exClass, "Out of Memory");
        return;
    }
    bandSource->data = impl;
    bandSource->iface = &iBandSource;

    _SetObj(env, self, bandSource);

    /* now, we must also register this type */
    (*env)->CallStaticVoidMethod(env, bandSourceClass,
        methodID, self);
}


/*
 * Class:     nitf_BandSource
 * Method:    destructMemory
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_nitf_BandSource_destructMemory
    (JNIEnv * env, jobject self)
{
    nitf_BandSource *bandSource;
    jmethodID methodID;
    jclass bandSourceClass = (*env)->GetObjectClass(env, self);

    /* -- THIS IS A SPECIAL CASE --
     * Do not destroy if it is attached to an ImageSource
     * The ImageSource will destruct it within nitf_ImageSource_Destruct
     *
     * This is why we have to call the _GetObj function later on, after
     * we make sure it doesn't "belong" to an ImageSource, and may have
     * already been freed.
     */
    methodID =
        (*env)->GetMethodID(env, bandSourceClass, "isAttached", "()Z");
    if ((*env)->CallBooleanMethod(env, self, methodID) == JNI_FALSE)
    {
        bandSource = _GetObj(env, self);
        if (bandSource)
        {
            nitf_BandSource_destruct(&bandSource);
        }
        _SetObj(env, self, NULL);
    }
}

