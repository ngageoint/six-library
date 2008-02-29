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

#include "nitf_TRE.h"
#include "nitf_JNI.h"

NITF_JNI_DECLARE_OBJ(nitf_TRE)
/*
 * Class:     nitf_TRE
 * Method:    destructMemory
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_nitf_TRE_destructMemory
    (JNIEnv * env, jobject self)
{
    nitf_TRE *tre = _GetObj(env, self);

    if (tre)
    {
        nitf_TRE_destruct(&tre);
    }

    _SetObj(env, self, NULL);
}


/*
 * Class:     nitf_TRE
 * Method:    construct
 * Signature: (Ljava/lang/String;I[Lnitf/TREDescription;)Z
 */
JNIEXPORT jboolean JNICALL Java_nitf_TRE_construct
  (JNIEnv *env, jobject self, jstring tag, jint size, jobjectArray descArray)
{
    nitf_TRE *tre = _GetObj(env, self);
    char* tag_id = NULL;
    nitf_TREDescription *desc = NULL;
    int i;      /* used for iterating */
    int arrayLength;
    jobject element;
    nitf_TREDescription *tempDesc = NULL; /* temp description */
    jclass treDescripClass = (*env)->FindClass(env, "nitf/TREDescription");
    jmethodID methodID =
        (*env)->GetMethodID(env, treDescripClass, "getAddress", "()J");
    nitf_Error error;
    
    tag_id = (*env)->GetStringUTFChars(env, tag, 0);

    /* just do a conversion so that the C-API
     * value is used for sure */
    if (size == nitf_TRE_DEFAULT_LENGTH)
        size = NITF_TRE_DEFAULT_LENGTH;
        
    
    if (descArray)
    {
        arrayLength = (int) (*env)->GetArrayLength(env, descArray);

        /* TODO: this creates a memory leak */
        desc =
            (nitf_TREDescription *) NITF_MALLOC(sizeof(nitf_TREDescription)
                                                * arrayLength);
        for (i = 0; i < arrayLength; ++i)
        {
            element = (*env)->GetObjectArrayElement(env, descArray, i);
            /* get the object */
            tempDesc = (nitf_TREDescription *)
                (*env)->CallLongMethod(env, element, methodID);

            desc[i].data_type = tempDesc->data_type;
            desc[i].data_count = tempDesc->data_count;

            desc[i].label =
                (char *) NITF_MALLOC(strlen(tempDesc->label) + 1);
            strcpy(desc[i].label, tempDesc->label);
            desc[i].tag = (char *) NITF_MALLOC(strlen(tempDesc->tag) + 1);
            strcpy(desc[i].tag, tempDesc->tag);
        }
    }
    else
    {
        desc = NULL;
    }
    
    tre = nitf_TRE_construct(tag_id, desc, size, &error);
    _SetObj(env, self, tre);
}


/*
 * Class:     nitf_TRE
 * Method:    getLength
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_nitf_TRE_getLength(JNIEnv * env, jobject self)
{
    nitf_TRE *tre = _GetObj(env, self);
    return tre->length;
}


/*
 * Class:     nitf_TRE
 * Method:    computeLength
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_nitf_TRE_computeLength
    (JNIEnv * env, jobject self)
{
    nitf_TRE *tre = _GetObj(env, self);
    return nitf_TRE_computeLength(tre);
}


/*
 * Class:     nitf_TRE
 * Method:    getTREDescription
 * Signature: ()[Lnitf/TREDescription;
 */
JNIEXPORT jobjectArray JNICALL Java_nitf_TRE_getTREDescription
    (JNIEnv * env, jobject self)
{
    nitf_TRE *tre = _GetObj(env, self);
    jclass treDescripClass = (*env)->FindClass(env, "nitf/TREDescription");
    jmethodID methodID =
        (*env)->GetMethodID(env, treDescripClass, "<init>", "(J)V");
    jobjectArray descriptions;
    jobject element;
    nitf_TREDescription *desc_ptr;

    nitf_TREDescription *dptr;
    jint items = 0;
    jint i;

    /* Get the count, or number of description fields */
    dptr = tre->descrip;
    while (dptr->data_type)
    {
        items++;
        dptr++;
    }

    /* create the array */
    descriptions =
        (*env)->NewObjectArray(env, items, treDescripClass, NULL);

    /* Now, loop and create an array */
    dptr = tre->descrip;
    for (i = 0; i < items; ++i)
    {
        desc_ptr = &dptr[i];
        element = (*env)->NewObject(env,
                                    treDescripClass,
                                    methodID, (jlong) desc_ptr);
        (*env)->SetObjectArrayElement(env, descriptions, i, element);
    }
    return descriptions;
}


/*
 * Class:     nitf_TRE
 * Method:    getTag
 * Signature: ()Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_nitf_TRE_getTag(JNIEnv * env, jobject self)
{
    nitf_TRE *tre = _GetObj(env, self);
    jstring tag = (*env)->NewStringUTF(env, tre->tag);
    return tag;
}



/*
 * Class:     nitf_TRE
 * Method:    setLength
 * Signature: (I)V
 */
JNIEXPORT void JNICALL Java_nitf_TRE_setLength
    (JNIEnv * env, jobject self, jint length)
{
    nitf_TRE *tre = _GetObj(env, self);
    tre->length = length;
}


/*
 * Class:     nitf_TRE
 * Method:    exists
 * Signature: (Ljava/lang/String;)Z
 */
JNIEXPORT jboolean JNICALL Java_nitf_TRE_exists
    (JNIEnv * env, jobject self, jstring tag)
{
    nitf_TRE *tre = _GetObj(env, self);
    char *tag_id;
    tag_id = (*env)->GetStringUTFChars(env, tag, 0);

    return nitf_TRE_exists(tre, tag_id) ? JNI_TRUE : JNI_FALSE;
}


/*
 * Class:     nitf_TRE
 * Method:    getField
 * Signature: (Ljava/lang/String;)Lnitf/Field;
 */
JNIEXPORT jobject JNICALL Java_nitf_TRE_getField
    (JNIEnv * env, jobject self, jstring tag)
{
    nitf_TRE *tre = _GetObj(env, self);
    nitf_Pair *pair;
    nitf_Field *field;
    char *tag_id;
    jmethodID methodID;
    jclass nsfExClass =
        (*env)->FindClass(env, "java/lang/NoSuchFieldException");
    jclass fieldClass = (*env)->FindClass(env, "nitf/Field");
    jobject object = NULL;

    /* get the tag, data buffer, and length */
    tag_id = (*env)->GetStringUTFChars(env, tag, 0);
    pair = nitf_HashTable_find(tre->hash, tag_id);
    if (!pair)
    {
        (*env)->ThrowNew(env, nsfExClass, "Field does not exist in TRE");
        return NULL;
    }
    else
    {
        field = (nitf_Field *) pair->data;
        methodID = (*env)->GetMethodID(env, fieldClass, "<init>", "(J)V");
        object = (*env)->NewObject(env,
                                   fieldClass, methodID, (jlong) field);
    }
    return object;
}


/*
 * Class:     nitf_TRE
 * Method:    setField
 * Signature: (Ljava/lang/String;[B)Z
 */
JNIEXPORT jboolean JNICALL Java_nitf_TRE_setField
    (JNIEnv * env, jobject self, jstring tag, jbyteArray data)
{
    nitf_TRE *tre = _GetObj(env, self);
    char *tag_id;
    NITF_DATA *buf;
    NITF_BOOL success;
    nitf_Error error;
    jsize len;
    jclass exClass = (*env)->FindClass(env, "nitf/NITFException");

    /* get the tag, data buffer, and length */
    tag_id = (*env)->GetStringUTFChars(env, tag, 0);
    buf = (NITF_DATA *) ((*env)->GetByteArrayElements(env, data, 0));
    len = (*env)->GetArrayLength(env, data);

    /* set to 0, to see if we need to actually throw an exception */
    error.level = 0;
    success = nitf_TRE_setValue(tre, tag_id, buf, len, &error);

    if (!success)
    {
        if (error.level)
        {
            /* throw the error */
            (*env)->ThrowNew(env, exClass, error.message);
        }
        return JNI_FALSE;
    }
    return JNI_TRUE;
}


/*
 * Class:     nitf_TRE
 * Method:    isSane
 * Signature: ()Z
 */
JNIEXPORT jboolean JNICALL Java_nitf_TRE_isSane(JNIEnv * env, jobject self)
{
    nitf_TRE *tre = _GetObj(env, self);
    return nitf_TRE_isSane(tre) ? JNI_TRUE : JNI_FALSE;
}


/*
 * Class:     nitf_TRE
 * Method:    print
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_nitf_TRE_print(JNIEnv * env, jobject self)
{
    nitf_TRE *tre = _GetObj(env, self);
    nitf_Error error;
    nitf_TRE_print(tre, &error);
}


/*
 * Class:     nitf_TRE
 * Method:    getFields
 * Signature: ()[Lnitf/TRE$TREField;
 */
JNIEXPORT jobjectArray JNICALL Java_nitf_TRE_getFields
    (JNIEnv * env, jobject self)
{
    nitf_TRE *tre = _GetObj(env, self);
    int status = 1;
    int tempStatus = 1;
    nitf_Pair *pair;            /* temp nitf_Pair */
    nitf_Field *field;          /* temp nitf_Field */
    nitf_Error error;

    jobject vector;
    jobject fieldObject;
    jobject descripObject;
    jobject treFieldObject;
    jstring qualifiedTag;

    jmethodID vectorMethodID;
    jmethodID fieldMethodID;
    jmethodID treFieldMethodID;
    jmethodID descripMethodID;
    jmethodID qualifiedTagMethodID;

    jclass vectorClass = (*env)->FindClass(env, "java/util/Vector");
    jclass fieldClass = (*env)->FindClass(env, "nitf/Field");
    jclass treFieldClass = (*env)->FindClass(env, "nitf/TRE$TREField");
    jclass treDescripClass = (*env)->FindClass(env, "nitf/TREDescription");
    jclass exClass = (*env)->FindClass(env, "nitf/NITFException");
    
    /* the cursor */
    nitf_TRECursor cursor = nitf_TRE_begin(tre);

    /* create a vector */
    vectorMethodID =
        (*env)->GetMethodID(env, vectorClass, "<init>", "()V");
    vector = (*env)->NewObject(env, vectorClass, vectorMethodID);

    /* set the add() method */
    vectorMethodID =
        (*env)->GetMethodID(env, vectorClass, "add",
                            "(Ljava/lang/Object;)Z");

    /* set constructors */
    fieldMethodID = (*env)->GetMethodID(env, fieldClass, "<init>", "(J)V");
    treFieldMethodID =
        (*env)->GetMethodID(env, treFieldClass, "<init>",
                            "(Lnitf/Field;Lnitf/TREDescription;)V");
    qualifiedTagMethodID =
        (*env)->GetMethodID(env, treFieldClass, "setQualifiedTag",
                            "(Ljava/lang/String;)V");
    descripMethodID =
        (*env)->GetMethodID(env, treDescripClass, "<init>", "(J)V");

    while (!nitf_TRE_isDone(&cursor) && status && tempStatus)
    {
        if ((tempStatus = nitf_TRE_iterate(&cursor, &error)) == NITF_SUCCESS)
        {
            pair = nitf_HashTable_find(tre->hash, cursor.tag_str);
            if (!pair || !pair->data)
            {
                /* TODO do something about error message here */
                /*nitf_Error_init(&error, "Unable to find tag in TRE hash",
                   NITF_CTXT, NITF_ERR_UNK); */
                status = 0;
            }
            else
            {
                field = (nitf_Field *) pair->data;
                /* construct the Field */
                fieldObject = (*env)->NewObject(env,
                                                fieldClass,
                                                fieldMethodID,
                                                (jlong) field);

                /* construct the descrip */
                descripObject = (*env)->NewObject(env,
                                                  treDescripClass,
                                                  descripMethodID,
                                                  (jlong) cursor.desc_ptr);

                /* construct the TREField */
                treFieldObject = (*env)->NewObject(env,
                                                   treFieldClass,
                                                   treFieldMethodID,
                                                   fieldObject,
                                                   descripObject);

                /* set the qualified tag */
                qualifiedTag = (*env)->NewStringUTF(env, cursor.tag_str);
                (*env)->CallVoidMethod(env, treFieldObject, qualifiedTagMethodID,
                                          qualifiedTag);
                /* add to the Vector */
                (*env)->CallBooleanMethod(env, vector, vectorMethodID,
                                          treFieldObject);
            }
        }
    }
    nitf_TRE_cleanup(&cursor);

    if (!status)
    {
        (*env)->ThrowNew(env, exClass,
                         "Unable to find tag in TRE hash"
                         /*error.message */ );
        return NULL;
    }
    
    /* now, return an Array */
    vectorMethodID =
        (*env)->GetMethodID(env, vectorClass, "toArray",
                            "()[Ljava/lang/Object;");
    return (*env)->CallObjectMethod(env, vector, vectorMethodID);
}

