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

/*
 *  To do a java iterator, we actually need a pointer to the extension
 *  segment itself
 */
typedef struct _java_ExtensionsIterator
{
    nitf_Extensions* ext;
    nitf_ExtensionsIterator iter;
    int counter;
} java_ExtensionsIterator;

NITF_JNI_DECLARE_OBJ(java_ExtensionsIterator)

JNIEXPORT jboolean JNICALL Java_nitf_Extensions_00024ExtensionsIterator_hasNext
  (JNIEnv *env, jobject self)
{
    java_ExtensionsIterator* iter = _GetObj(env, self);

    nitf_ExtensionsIterator end = nitf_Extensions_end(iter->ext);
    return (nitf_ExtensionsIterator_notEqualTo(&(iter->iter), &end));
    /*
    {
    We might need to finalize!!!
        NITF_FREE(iter);
        _SetObj(env, self, NULL);
        }
    return iter != NULL ? JNI_TRUE : JNI_FALSE;
    */
}

JNIEXPORT jobject JNICALL Java_nitf_Extensions_00024ExtensionsIterator_next
  (JNIEnv *env, jobject self)
{
    java_ExtensionsIterator* iter = _GetObj(env, self);

    jclass treClass;
    jmethodID treInitMethod;
    jobject jTRE;

    /* Get our TRE out */
    nitf_TRE* tre = nitf_ExtensionsIterator_get(&(iter->iter));

    treClass = (*env)->FindClass(env, "nitf/TRE");

    treInitMethod =
            (*env)->GetMethodID(env, treClass, "<init>", "(J)V");

    /* Set up the java TRE */
    jTRE = (*env)->NewObject(env,
            treClass, treInitMethod, (jlong) tre);


    /* Next get the next one */
    nitf_ExtensionsIterator_increment(&(iter->iter));
    iter->counter++;
    return jTRE;
}

/*
 *  In order to remove from our list, we actually have to get the previous
 *  C pointer, which requires some finagling.
 */
JNIEXPORT void JNICALL Java_nitf_Extensions_00024ExtensionsIterator_remove
  (JNIEnv *env, jobject self)
{
    nitf_Error error;
    java_ExtensionsIterator* iter = _GetObj(env, self);
    nitf_ExtensionsIterator end = nitf_Extensions_end(iter->ext);
    int i = 0;
    /*nitf_ExtensionsIterator old = iter->iter;*/
    iter->iter = nitf_Extensions_begin(iter->ext);

    for (; i < iter->counter - 1; i++)
    {
        nitf_ExtensionsIterator_increment(&(iter->iter));
    }

    if (nitf_ExtensionsIterator_equals(&(iter->iter), &end))
    {
        printf("ERROR! Trying to remove past end of extensions.!\n");
    }
    iter->counter--;
    nitf_Extensions_remove(iter->ext, &(iter->iter), &error);
}


JNIEXPORT void JNICALL Java_nitf_Extensions_00024ExtensionsIterator_construct
  (JNIEnv *env, jobject self, jobject jExtObject)
{
    jclass extClass;
    jmethodID extMethodId;
    nitf_Extensions *ext = NULL;
    nitf_Error error;
    java_ExtensionsIterator *it = (java_ExtensionsIterator*)NITF_MALLOC(sizeof(java_ExtensionsIterator));

    assert(it);

    extClass = (*env)->GetObjectClass(env, jExtObject);
    extMethodId = (*env)->GetMethodID(env, extClass, "getAddress", "()J");
    ext = (nitf_Extensions*)
        (*env)->CallLongMethod(env, jExtObject, extMethodId);

    it->ext = ext;
    it->iter = nitf_Extensions_begin(ext);
    it->counter = 0;
    _SetObj(env, self, it);
}
