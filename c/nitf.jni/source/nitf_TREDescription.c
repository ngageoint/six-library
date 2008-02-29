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

#include "nitf_TREDescription.h"
#include "nitf_Field.h"

#include "nitf_JNI.h"

NITF_JNI_DECLARE_OBJ(nitf_TREDescription)
/*
 * Class:     nitf_TREDescription
 * Method:    construct
 * Signature: (Lnitf/TREDescriptionDataType;ILjava/lang/String;Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_nitf_TREDescription_construct
    (JNIEnv * env, jobject self, jobject dataType,
     jint dataCount, jstring label, jstring tag)
{
    nitf_TREDescription *descrip;
    nitf_Error error;
    int length;                 /* used to temporarily hold length */
    jclass exClass = (*env)->FindClass(env, "nitf/NITFException");
    jclass mapClass = (*env)->FindClass(env, "java/util/Map");
    jclass stringClass = (*env)->FindClass(env, "java/lang/String");
    jclass dataTypeClass =
        (*env)->FindClass(env, "nitf/TREDescriptionDataType");
    jclass thisClass = (*env)->GetObjectClass(env, self);
    int type = NITF_BCS_A;
    jfieldID fieldID;
    jmethodID methodID;
    jmethodID stringMethodID;
    jobject userConstructedMap;
    jstring str;
    jobject enumBCSA, enumBCSN, enumBINARY, enumLOOP, enumENDLOOP, enumIF,
        enumENDIF, enumEND, enumCOMP_LEN;

    descrip =
        (nitf_TREDescription *) NITF_MALLOC(sizeof(nitf_TREDescription));
    if (!descrip)
    {
        /*nitf_Error_init(&error, NITF_STRERROR( NITF_ERRNO ),
           NITF_CTXT, NITF_ERR_MEMORY ); */
        (*env)->ThrowNew(env, exClass, "Out of memory");
        return;
    }

    /* BCS-A */
    fieldID =
        (*env)->GetStaticFieldID(env, dataTypeClass, "NITF_BCS_A",
                                 "Lnitf/TREDescriptionDataType;");
    enumBCSA = (*env)->GetStaticObjectField(env, dataTypeClass, fieldID);

    /* BCS-N */
    fieldID =
        (*env)->GetStaticFieldID(env, dataTypeClass, "NITF_BCS_N",
                                 "Lnitf/TREDescriptionDataType;");
    enumBCSN = (*env)->GetStaticObjectField(env, dataTypeClass, fieldID);

    /* BINARY */
    fieldID =
        (*env)->GetStaticFieldID(env, dataTypeClass, "NITF_BINARY",
                                 "Lnitf/TREDescriptionDataType;");
    enumBINARY = (*env)->GetStaticObjectField(env, dataTypeClass, fieldID);

    /* LOOP */
    fieldID =
        (*env)->GetStaticFieldID(env, dataTypeClass, "NITF_LOOP",
                                 "Lnitf/TREDescriptionDataType;");
    enumLOOP = (*env)->GetStaticObjectField(env, dataTypeClass, fieldID);

    /* ENDLOOP */
    fieldID =
        (*env)->GetStaticFieldID(env, dataTypeClass, "NITF_ENDLOOP",
                                 "Lnitf/TREDescriptionDataType;");
    enumENDLOOP =
        (*env)->GetStaticObjectField(env, dataTypeClass, fieldID);

    /* IF */
    fieldID =
        (*env)->GetStaticFieldID(env, dataTypeClass, "NITF_IF",
                                 "Lnitf/TREDescriptionDataType;");
    enumIF = (*env)->GetStaticObjectField(env, dataTypeClass, fieldID);

    /* ENDIF */
    fieldID =
        (*env)->GetStaticFieldID(env, dataTypeClass, "NITF_ENDIF",
                                 "Lnitf/TREDescriptionDataType;");
    enumENDIF = (*env)->GetStaticObjectField(env, dataTypeClass, fieldID);

    /* END */
    fieldID =
        (*env)->GetStaticFieldID(env, dataTypeClass, "NITF_END",
                                 "Lnitf/TREDescriptionDataType;");
    enumEND = (*env)->GetStaticObjectField(env, dataTypeClass, fieldID);
    
    /* COMP_LEN */
    fieldID =
        (*env)->GetStaticFieldID(env, dataTypeClass, "NITF_COMP_LEN",
                                 "Lnitf/TREDescriptionDataType;");
    enumCOMP_LEN = (*env)->GetStaticObjectField(env, dataTypeClass, fieldID);

    if (dataType == enumBCSA)
        type = NITF_BCS_A;
    else if (dataType == enumBCSN)
        type = NITF_BCS_N;
    else if (dataType == enumBINARY)
        type = NITF_BINARY;
    else if (dataType == enumLOOP)
        type = NITF_LOOP;
    else if (dataType == enumENDLOOP)
        type = NITF_ENDLOOP;
    else if (dataType == enumIF)
        type = NITF_IF;
    else if (dataType == enumENDIF)
        type = NITF_ENDIF;
    else if (dataType == enumEND)
        type = NITF_END;
    else if (dataType == enumCOMP_LEN)
        type = NITF_COMP_LEN;

    descrip->data_type = type;
    descrip->data_count = (int) dataCount;

    /* copy the label */
    length = (int) (*env)->GetStringUTFLength(env, label);
    descrip->label = (char *) NITF_MALLOC(length + 1);
    strcpy(descrip->label, (*env)->GetStringUTFChars(env, label, 0));
    descrip->label[length] = 0;

    /* copy the tag */
    length = (int) (*env)->GetStringUTFLength(env, tag);
    descrip->tag = (char *) NITF_MALLOC(length + 1);
    strcpy(descrip->tag, (*env)->GetStringUTFChars(env, tag, 0));
    descrip->tag[length] = 0;

    /* now, add it to the map FIRST, before calling setAddress */
    /* this flags it as a user constructed object */

    /* get the static map */
    fieldID =
        (*env)->GetStaticFieldID(env, thisClass, "userConstructed",
                                 "Ljava/util/Map;");
    userConstructedMap =
        (*env)->GetStaticObjectField(env, thisClass, fieldID);

    /* put it in the map */
    methodID =
        (*env)->GetMethodID(env, mapClass, "put",
                            "(Ljava/lang/Object;Ljava/lang/Object;)Ljava/lang/Object;");
    stringMethodID =
        (*env)->GetStaticMethodID(env, stringClass, "valueOf",
                                  "(J)Ljava/lang/String;");
    str =
        (jstring) (*env)->CallStaticObjectMethod(env, stringClass,
                                                 stringMethodID,
                                                 (jlong) descrip);
    (*env)->CallObjectMethod(env, userConstructedMap, methodID, str, str);

    /* NOW, set the address */
    _SetObj(env, self, descrip);
}


/*
 * Class:     nitf_TREDescription
 * Method:    destructMemory
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_nitf_TREDescription_destructMemory
    (JNIEnv * env, jobject self)
{
    nitf_TREDescription *descrip = _GetObj(env, self);

    if (descrip)
    {
        if (descrip->label)
        {
            NITF_FREE(descrip->label);
        }
        if (descrip->tag)
        {
            NITF_FREE(descrip->tag);
        }

        NITF_FREE(descrip);
        _SetObj(env, self, NULL);
    }
}


/*
 * Class:     nitf_TREDescription
 * Method:    getDataType
 * Signature: ()Lnitf/TREDescriptionDataType;
 */
JNIEXPORT jobject JNICALL Java_nitf_TREDescription_getDataType
    (JNIEnv * env, jobject self)
{
    nitf_TREDescription *descrip = _GetObj(env, self);
    int dataType = descrip->data_type;
    jobject type = NULL;        /* the return type */
    jobject enumBCSA, enumBCSN, enumBINARY, enumLOOP, enumENDLOOP, enumIF,
        enumENDIF, enumEND;
    jclass dataTypeClass =
        (*env)->FindClass(env, "nitf/TREDescriptionDataType");
    jfieldID fieldID;

    /* BCS-A */
    fieldID =
        (*env)->GetStaticFieldID(env, dataTypeClass, "NITF_BCS_A",
                                 "Lnitf/TREDescriptionDataType;");
    enumBCSA = (*env)->GetStaticObjectField(env, dataTypeClass, fieldID);

    /* BCS-N */
    fieldID =
        (*env)->GetStaticFieldID(env, dataTypeClass, "NITF_BCS_N",
                                 "Lnitf/TREDescriptionDataType;");
    enumBCSN = (*env)->GetStaticObjectField(env, dataTypeClass, fieldID);

    /* BINARY */
    fieldID =
        (*env)->GetStaticFieldID(env, dataTypeClass, "NITF_BINARY",
                                 "Lnitf/TREDescriptionDataType;");
    enumBINARY = (*env)->GetStaticObjectField(env, dataTypeClass, fieldID);

    /* LOOP */
    fieldID =
        (*env)->GetStaticFieldID(env, dataTypeClass, "NITF_LOOP",
                                 "Lnitf/TREDescriptionDataType;");
    enumLOOP = (*env)->GetStaticObjectField(env, dataTypeClass, fieldID);

    /* ENDLOOP */
    fieldID =
        (*env)->GetStaticFieldID(env, dataTypeClass, "NITF_ENDLOOP",
                                 "Lnitf/TREDescriptionDataType;");
    enumENDLOOP =
        (*env)->GetStaticObjectField(env, dataTypeClass, fieldID);

    /* IF */
    fieldID =
        (*env)->GetStaticFieldID(env, dataTypeClass, "NITF_IF",
                                 "Lnitf/TREDescriptionDataType;");
    enumIF = (*env)->GetStaticObjectField(env, dataTypeClass, fieldID);

    /* ENDIF */
    fieldID =
        (*env)->GetStaticFieldID(env, dataTypeClass, "NITF_ENDIF",
                                 "Lnitf/TREDescriptionDataType;");
    enumENDIF = (*env)->GetStaticObjectField(env, dataTypeClass, fieldID);

    /* END */
    fieldID =
        (*env)->GetStaticFieldID(env, dataTypeClass, "NITF_END",
                                 "Lnitf/TREDescriptionDataType;");
    enumEND = (*env)->GetStaticObjectField(env, dataTypeClass, fieldID);

    if (dataType == NITF_BCS_A)
        type = enumBCSA;
    else if (dataType == NITF_BCS_N)
        type = enumBCSN;
    else if (dataType == NITF_BINARY)
        type = enumBINARY;
    else if (dataType == NITF_LOOP)
        type = enumLOOP;
    else if (dataType == NITF_ENDLOOP)
        type = enumENDLOOP;
    else if (dataType == NITF_IF)
        type = enumIF;
    else if (dataType == NITF_ENDIF)
        type = enumENDIF;
    else if (dataType == NITF_END)
        type = enumEND;

    return type;
}


/*
 * Class:     nitf_TREDescription
 * Method:    getDataCount
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_nitf_TREDescription_getDataCount
    (JNIEnv * env, jobject self)
{
    nitf_TREDescription *descrip = _GetObj(env, self);
    return descrip->data_count;
}


/*
 * Class:     nitf_TREDescription
 * Method:    getLabel
 * Signature: ()Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_nitf_TREDescription_getLabel
    (JNIEnv * env, jobject self)
{
    nitf_TREDescription *descrip = _GetObj(env, self);
    jstring label = (*env)->NewStringUTF(env, descrip->label);
    return label;
}


/*
 * Class:     nitf_TREDescription
 * Method:    getTag
 * Signature: ()Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_nitf_TREDescription_getTag
    (JNIEnv * env, jobject self)
{
    nitf_TREDescription *descrip = _GetObj(env, self);
    jstring tag;

    if (descrip->tag)
    {
        tag = (*env)->NewStringUTF(env, descrip->tag);
        return tag;
    }
    return NULL;
}

