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
#include "nitf_Extensions.h"
#include "nitf_JNI.h"

NITF_JNI_DECLARE_OBJ(nitf_Extensions)
/*
 * Class:     nitf_Extensions
 * Method:    appendTRE
 * Signature: (Lnitf/TRE;)V
 */
JNIEXPORT void JNICALL Java_nitf_Extensions_appendTRE
  (JNIEnv *env, jobject self, jobject treObject)
{
    nitf_Extensions *extensions = _GetObj(env, self);
    jclass treClass = (*env)->FindClass(env, "nitf/TRE");
    nitf_Error error;
    jmethodID methodID =
        (*env)->GetMethodID(env, treClass, "getAddress", "()J");
    nitf_TRE *tre =
        (nitf_TRE *) (*env)->CallLongMethod(env, treObject, methodID);

    if (!nitf_Extensions_appendTRE(extensions, tre, &error))
    {
        _ThrowNITFException(env, error.message);
        return;
    }
    
    /* tell Java not to manage it */
    _ManageObject(env, (jlong)tre, JNI_FALSE);
}


JNIEXPORT jobject JNICALL Java_nitf_Extensions_getTREsByName
  (JNIEnv *env, jobject self, jstring jName)
{
    nitf_Extensions *extensions = _GetObj(env, self);
    jclass treClass = (*env)->FindClass(env, "nitf/TRE");
    jclass listClass = (*env)->FindClass(env, "java/util/LinkedList");
    jmethodID treInitMethodID =
        (*env)->GetMethodID(env, treClass, "<init>", "(J)V");
    jmethodID listAddMethodID =
        (*env)->GetMethodID(env, listClass, "add", "(Ljava/lang/Object;)Z");
    nitf_ListIterator iter, end;
    nitf_TRE *tre;
    jobject treObject;
    const char *name = NULL;
    nitf_List* list;
    jobject linkedList = NULL;
    
    linkedList = (*env)->NewObject(env, listClass,
        (*env)->GetMethodID(env, listClass, "<init>", "()V"));
    
    if (extensions != NULL)
    {
        name = (*env)->GetStringUTFChars(env, jName, 0);
        /* get the list */
        list = nitf_Extensions_getTREsByName(extensions, name);
        (*env)->ReleaseStringUTFChars(env, jName, name);
        if (list == NULL)
        {
        	_ThrowNITFException(env, "TRE not found");
        	return NULL;
        }
        
        /* set up iterators */
        iter = nitf_List_begin(list);
        end = nitf_List_end(list);

        while (nitf_ListIterator_notEqualTo(&iter, &end))
        {
            tre = (nitf_TRE *) nitf_ListIterator_get(&iter);

            /* get the object and add it to the array */
            treObject =
                (*env)->NewObject(env, treClass, treInitMethodID, (jlong) tre);
            
            /* tell Java not to manage it */
            _ManageObject(env, (jlong)tre, JNI_FALSE);
            (*env)->CallBooleanMethod(env, linkedList, listAddMethodID, treObject);
            nitf_ListIterator_increment(&iter);
        }
    }
    return linkedList;
}


/*
 * Class:     nitf_Extensions
 * Method:    exists
 * Signature: (Ljava/lang/String;)Z
 */
JNIEXPORT jboolean JNICALL Java_nitf_Extensions_exists
    (JNIEnv * env, jobject self, jstring jName)
{
    nitf_Extensions *extensions = _GetObj(env, self);
    const char *name = NULL;
    jboolean exists = JNI_FALSE;

    if (extensions)
    {
        name = (*env)->GetStringUTFChars(env, jName, 0);
        exists = nitf_Extensions_exists(extensions, name) ? JNI_TRUE : JNI_FALSE;
        (*env)->ReleaseStringUTFChars(env, jName, name);
    }
    return exists;
}


/*
 * Class:     nitf_Extensions
 * Method:    removeTREsByName
 * Signature: (Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_nitf_Extensions_removeTREsByName
  (JNIEnv *env, jobject self, jstring jName)
{
    nitf_Extensions *extensions = _GetObj(env, self);
    const char *name = NULL;

    if (extensions)
    {
        name = (*env)->GetStringUTFChars(env, jName, 0);
        nitf_Extensions_removeTREsByName(extensions, name);
        (*env)->ReleaseStringUTFChars(env, jName, name);
    }
}


JNIEXPORT jobject JNICALL Java_nitf_Extensions_getAll
  (JNIEnv *env, jobject self)
{
    nitf_Extensions *extensions = _GetObj(env, self);
    nitf_ExtensionsIterator startIter, endIter;
    jclass treClass = (*env)->FindClass(env, "nitf/TRE");
    jclass listClass = (*env)->FindClass(env, "java/util/LinkedList");
    jmethodID listAddMethodID =
        (*env)->GetMethodID(env, listClass, "add", "(Ljava/lang/Object;)Z");
    jmethodID treInitMethodID =
        (*env)->GetMethodID(env, treClass, "<init>", "(J)V");
    nitf_TRE *tre;
    jobject element;
    jobject linkedList;
    
    linkedList = (*env)->NewObject(env, listClass,
        (*env)->GetMethodID(env, listClass, "<init>", "()V"));

    if (extensions)
    {
        startIter = nitf_Extensions_begin(extensions);
        endIter = nitf_Extensions_end(extensions);

        /* now, iterate and add to the array */
        while (nitf_ExtensionsIterator_notEqualTo(&startIter, &endIter))
        {
            tre = nitf_ExtensionsIterator_get(&startIter);
            element = (*env)->NewObject(env,
                                        treClass, treInitMethodID, (jlong) tre);
            
            /* tell Java not to manage it */
            _ManageObject(env, (jlong)tre, JNI_FALSE);
            
            (*env)->CallBooleanMethod(env, linkedList, listAddMethodID, element);
            nitf_ExtensionsIterator_increment(&startIter);
        }
    }
    return linkedList;
}


/*
 * Class:     nitf_Extensions
 * Method:    computeLength
 * Signature: (Lnitf/Version;)J
 */
JNIEXPORT jlong JNICALL Java_nitf_Extensions_computeLength
    (JNIEnv * env, jobject self, jobject versionObject)
{
    nitf_Extensions *extensions = _GetObj(env, self);
    nitf_Error error;
    nitf_Version version = _GetNITFVersion(env, versionObject);

    return (jlong) nitf_Extensions_computeLength(extensions, version,
                                                 &error);
}

