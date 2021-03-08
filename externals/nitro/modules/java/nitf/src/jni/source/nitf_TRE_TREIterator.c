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
#include "nitf_TRE.h"
#include "nitf_JNI.h"

NITF_JNI_DECLARE_OBJ(nitf_TREEnumerator)


JNIEXPORT jboolean JNICALL Java_nitf_TRE_00024TREIterator_hasNext
  (JNIEnv *env, jobject self)
{
    nitf_TREEnumerator *enumerator = _GetObj(env, self);
    jboolean rv = JNI_FALSE;
    if (enumerator)
    {
        rv = enumerator->hasNext(&enumerator) ? JNI_TRUE : JNI_FALSE;
        _SetObj(env, self, enumerator);
    }
    return rv;
}

JNIEXPORT jobject JNICALL Java_nitf_TRE_00024TREIterator_next
  (JNIEnv *env, jobject self)
{
    nitf_TREEnumerator *enumerator = _GetObj(env, self);
    nitf_Pair *pair = NULL;
    jclass fieldClass, fieldPairClass;
    jmethodID fieldInitMethod, fieldPairInitMethod;
    jfieldID nameFieldID, fieldFieldID;
    jobject jField, jFieldPair;
    jstring jFieldName;
    nitf_Error error;

    if (!enumerator)
        return NULL;

    /* get the next value */
    pair = enumerator->next(enumerator, &error);

    if (!pair)
    {
        _ThrowNITFException(env, "NULL pair");
        return NULL;
    }

    fieldClass = (*env)->FindClass(env, "nitf/Field");
    fieldPairClass = (*env)->FindClass(env, "nitf/TRE$FieldPair");

    nameFieldID = (*env)->GetFieldID(env, fieldPairClass, "name",
            "Ljava/lang/String;");
    fieldFieldID = (*env)->GetFieldID(env, fieldPairClass, "field",
            "Lnitf/Field;");

    fieldInitMethod =
            (*env)->GetMethodID(env, fieldClass, "<init>", "(J)V");
    fieldPairInitMethod =
            (*env)->GetMethodID(env, fieldPairClass, "<init>", "()V");

    jField = (*env)->NewObject(env,
            fieldClass, fieldInitMethod, (jlong) pair->data);
    jFieldName = (*env)->NewStringUTF(env, pair->key);
    jFieldPair = (*env)->NewObject(env,
            fieldPairClass, fieldPairInitMethod);

    (*env)->SetObjectField(env, jFieldPair, nameFieldID, jFieldName);
    (*env)->SetObjectField(env, jFieldPair, fieldFieldID, jField);

    return jFieldPair;
}

JNIEXPORT void JNICALL Java_nitf_TRE_00024TREIterator_construct
  (JNIEnv *env, jobject self, jobject jTREObject)
{
    jclass treClass;
    jmethodID treMethodId;
    nitf_TRE *tre = NULL;
    nitf_TREEnumerator *enumerator = NULL;
    nitf_Error error;

    treClass = (*env)->GetObjectClass(env, jTREObject);
    treMethodId = (*env)->GetMethodID(env, treClass, "getAddress", "()J");
    tre = (nitf_TRE*)(*env)->CallLongMethod(env, jTREObject, treMethodId);

    enumerator = nitf_TRE_begin(tre, &error);
    _SetObj(env, self, enumerator);
}

JNIEXPORT jstring JNICALL Java_nitf_TRE_00024TREIterator_getFieldDescription
  (JNIEnv *env, jobject self)
{
    nitf_TREEnumerator *enumerator = _GetObj(env, self);
    nitf_Error error;
    jstring string = NULL;

    if (enumerator)
    {
        const char* desc = enumerator->getFieldDescription(enumerator, &error);
        if (desc)
        {
            /* get the string */
            string = (*env)->NewStringUTF(env, desc);
        }
    }

    return string;
}

