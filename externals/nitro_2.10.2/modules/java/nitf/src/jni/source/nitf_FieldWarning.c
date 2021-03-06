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
#include "nitf_FieldWarning.h"
#include "nitf_FieldWarning_Destructor.h"
#include "nitf_JNI.h"

NITF_JNI_DECLARE_OBJ(nitf_FieldWarning)

JNIEXPORT jboolean JNICALL Java_nitf_FieldWarning_00024Destructor_destructMemory
    (JNIEnv * env, jobject self, jlong address)
{
    nitf_FieldWarning *warning = (nitf_FieldWarning*)address;
    if (warning)
    {
        nitf_FieldWarning_destruct(&warning);
        return JNI_TRUE;
    }
    return JNI_FALSE;
}

/*
 * Class:     nitf_FieldWarning
 * Method:    getFieldName
 * Signature: ()Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_nitf_FieldWarning_getFieldName
    (JNIEnv * env, jobject self)
{
    nitf_FieldWarning *warning = _GetObj(env, self);
    jstring string = NULL;
    if (warning->fieldName)
        string = (*env)->NewStringUTF(env, warning->fieldName);
    return string;
}


/*
 * Class:     nitf_FieldWarning
 * Method:    getField
 * Signature: ()Lnitf/Field;
 */
JNIEXPORT jobject JNICALL Java_nitf_FieldWarning_getField
    (JNIEnv * env, jobject self)
{
    nitf_FieldWarning *warning = _GetObj(env, self);
    jobject field = NULL;
    if (warning->field)
        field = _GetFieldObj(env, warning->field);
    return field;
}


/*
 * Class:     nitf_FieldWarning
 * Method:    getWarning
 * Signature: ()Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_nitf_FieldWarning_getWarning
    (JNIEnv * env, jobject self)
{
    nitf_FieldWarning *warning = _GetObj(env, self);
    jstring string = NULL;
    if (warning->expectation)
        string = (*env)->NewStringUTF(env, warning->expectation);
    return string;
}


/*
 * Class:     nitf_FieldWarning
 * Method:    getFileOffset
 * Signature: ()J
 */
JNIEXPORT jlong JNICALL Java_nitf_FieldWarning_getFileOffset
    (JNIEnv * env, jobject self)
{
    nitf_FieldWarning *warning = _GetObj(env, self);
    return (jlong) warning->fileOffset;
}

