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
#include "nitf_TRE_Destructor.h"
#include "nitf_JNI.h"

NITF_JNI_DECLARE_OBJ(nitf_TRE)


JNIEXPORT jboolean JNICALL Java_nitf_TRE_00024Destructor_destructMemory
    (JNIEnv * env, jobject self, jlong address)
{
    nitf_TRE *tre = (nitf_TRE*)address;
    if (tre)
    {
        nitf_TRE_destruct(&tre);
        return JNI_TRUE;
    }
    return JNI_FALSE;
}


JNIEXPORT void JNICALL Java_nitf_TRE_construct
  (JNIEnv *env, jobject self, jstring jTag, jstring jId)
{
    nitf_TRE *tre = NULL;
    const char* tag = NULL;
    const char* id = NULL;
    nitf_Error error;
    
    tag = jTag != NULL ? (*env)->GetStringUTFChars(env, jTag, 0) : NULL;
    id = jId != NULL ? (*env)->GetStringUTFChars(env, jId, 0) : NULL;
    
    tre = nitf_TRE_construct(tag, id, &error);
    
    if (tag)
        (*env)->ReleaseStringUTFChars(env, jTag, tag);
    if (id)
        (*env)->ReleaseStringUTFChars(env, jId, id);
    
    if (!tre)
    {
        _ThrowNITFException(env, error.message);
        return;
    }
    
    _SetObj(env, self, tre);
}


JNIEXPORT jint JNICALL Java_nitf_TRE_getCurrentSize(JNIEnv * env, jobject self)
{
    nitf_Error error;
    nitf_TRE *tre = _GetObj(env, self);
    return nitf_TRE_getCurrentSize(tre, &error);
}


JNIEXPORT jstring JNICALL Java_nitf_TRE_getTag(JNIEnv * env, jobject self)
{
    nitf_TRE *tre = _GetObj(env, self);
    jstring tag = (*env)->NewStringUTF(env, tre->tag);
    return tag;
}


JNIEXPORT jboolean JNICALL Java_nitf_TRE_exists
    (JNIEnv * env, jobject self, jstring jTag)
{
    nitf_TRE *tre = _GetObj(env, self);
    jboolean exists = JNI_FALSE;
    const char *tag = (*env)->GetStringUTFChars(env, jTag, 0);

    exists = nitf_TRE_exists(tre, tag) ? JNI_TRUE : JNI_FALSE;
    (*env)->ReleaseStringUTFChars(env, jTag, tag);
    return exists;
}


JNIEXPORT jobject JNICALL Java_nitf_TRE_getField
    (JNIEnv * env, jobject self, jstring jTag)
{
    nitf_TRE *tre = _GetObj(env, self);
    nitf_Field *field = NULL;
    const char *tag = NULL;
    jclass fieldClass;
    jmethodID methodID;
    jobject jField = NULL;
    
    fieldClass = (*env)->FindClass(env, "nitf/Field");
    methodID = (*env)->GetMethodID(env, fieldClass, "<init>", "(J)V");
    
    tag = (*env)->GetStringUTFChars(env, jTag, 0);
    field = nitf_TRE_getField(tre, tag);
    (*env)->ReleaseStringUTFChars(env, jTag, tag);
    
    if (field)
        jField = (*env)->NewObject(env, fieldClass, methodID, (jlong) field);
    
    return jField;
}


JNIEXPORT jboolean JNICALL Java_nitf_TRE_setField
    (JNIEnv * env, jobject self, jstring jTag, jbyteArray data)
{
    nitf_TRE *tre = _GetObj(env, self);
    const char *tag = NULL;
    jbyte *buf = NULL;
    NITF_BOOL success;
    nitf_Error error;
    jsize len;

    /* get the tag, data buffer, and length */
    tag = (*env)->GetStringUTFChars(env, jTag, 0);
    buf = (*env)->GetByteArrayElements(env, data, 0);

    if (!buf)
    {
        _ThrowNITFException(env, "Out of memory!");
        return JNI_FALSE;
    }

    len = (*env)->GetArrayLength(env, data);

    /* set to 0, to see if we need to actually throw an exception */
    error.level = 0;
    success = nitf_TRE_setField(tre, tag, (NITF_DATA*)buf, len, &error);
    
    (*env)->ReleaseStringUTFChars(env, jTag, tag);
    (*env)->ReleaseByteArrayElements(env, data, buf, 0);

    if (!success)
    {
        if (error.level)
            _ThrowNITFException(env, error.message);
        return JNI_FALSE;
    }
    return JNI_TRUE;
}


JNIEXPORT jobject JNICALL Java_nitf_TRE_find
  (JNIEnv *env, jobject self, jstring jPattern)
{
    nitf_TRE *tre = _GetObj(env, self);
    
    const char* pattern = NULL;
    nitf_List *list = NULL;
    nitf_Error error;
    
    pattern = (*env)->GetStringUTFChars(env, jPattern, 0);
    list = nitf_TRE_find(tre, pattern, &error);
    (*env)->ReleaseStringUTFChars(env, jPattern, pattern);
    
    if (!list)
    {
        /* TODO currently, we don't know if an error occurred or if no TREs were found */
        _ThrowNITFException(env, error.message);
        return NULL;
    }
    else
    {
        jclass vectorClass, fieldClass, fieldPairClass;
        jmethodID vectorMethodID, fieldInitMethod, fieldPairInitMethod;
        jfieldID nameFieldID, fieldFieldID;
        jobject vector, jField, jFieldPair;
        jstring jFieldName;
        uint32_t listSize;
        
        vectorClass = (*env)->FindClass(env, "java/util/Vector");
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
        vectorMethodID =
                (*env)->GetMethodID(env, vectorClass, "<init>", "(I)V");
        
        listSize = nitf_List_size(list);
        
        /* create a new vector to hold the results */
        vector = (*env)->NewObject(env, vectorClass, vectorMethodID, (jint)listSize);
        
        /* set the add() method */
        vectorMethodID =
                (*env)->GetMethodID(env, vectorClass, "add",
                        "(Ljava/lang/Object;)Z");
        
        /* iterate over the list and add to the vector */
        while (!nitf_List_isEmpty(list))
        {
            nitf_Pair* pair = (nitf_Pair*) nitf_List_popFront(list);
            jField = (*env)->NewObject(env,
                    fieldClass, fieldInitMethod, (jlong) pair->data);
            jFieldName = (*env)->NewStringUTF(env, pair->key);
            jFieldPair = (*env)->NewObject(env,
                    fieldPairClass, fieldPairInitMethod);
            
            (*env)->SetObjectField(env, jFieldPair, nameFieldID, jFieldName);
            (*env)->SetObjectField(env, jFieldPair, fieldFieldID, jField);
            
            /* add to the Vector */
            (*env)->CallBooleanMethod(env, vector, vectorMethodID, jFieldPair);
        }
        
        /* destroy the list since we are done with it - should be empty */
        nitf_List_destruct(&list);
        
        return vector;
    }
}
