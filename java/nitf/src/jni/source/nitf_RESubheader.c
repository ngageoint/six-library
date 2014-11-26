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
#include "nitf_RESubheader.h"
#include "nitf_JNI.h"

NITF_JNI_DECLARE_OBJ(nitf_RESubheader)
/*
 * Class:     nitf_RESubheader
 * Method:    getFilePartType
 * Signature: ()Lnitf/Field;
 */
JNIEXPORT jobject JNICALL Java_nitf_RESubheader_getFilePartType
    (JNIEnv * env, jobject self)
{
    nitf_RESubheader *subheader = _GetObj(env, self);
    return _GetFieldObj(env, subheader->filePartType);
}


/*
 * Class:     nitf_RESubheader
 * Method:    getTypeID
 * Signature: ()Lnitf/Field;
 */
JNIEXPORT jobject JNICALL Java_nitf_RESubheader_getTypeID
    (JNIEnv * env, jobject self)
{

    nitf_RESubheader *subheader = _GetObj(env, self);
    return _GetFieldObj(env, subheader->typeID);
}


/*
 * Class:     nitf_RESubheader
 * Method:    getSecurityClass
 * Signature: ()Lnitf/Field;
 */
JNIEXPORT jobject JNICALL Java_nitf_RESubheader_getSecurityClass
    (JNIEnv * env, jobject self)
{
    nitf_RESubheader *subheader = _GetObj(env, self);
    return _GetFieldObj(env, subheader->securityClass);
}


/*
 * Class:     nitf_RESubheader
 * Method:    getSecurityGroup
 * Signature: ()Lnitf/FileSecurity;
 */
JNIEXPORT jobject JNICALL Java_nitf_RESubheader_getSecurityGroup
    (JNIEnv * env, jobject self)
{
    nitf_RESubheader *subheader = _GetObj(env, self);
    jclass securityClass = (*env)->FindClass(env, "nitf/FileSecurity");
    jobject security;
    jmethodID methodID =
        (*env)->GetMethodID(env, securityClass, "<init>", "(J)V");
    security = (*env)->NewObject(env,
                                 securityClass,
                                 methodID,
                                 (jlong) subheader->securityGroup);
    return security;
}


/*
 * Class:     nitf_RESubheader
 * Method:    getVersion
 * Signature: ()Lnitf/Field;
 */
JNIEXPORT jobject JNICALL Java_nitf_RESubheader_getVersion
    (JNIEnv * env, jobject self)
{
    nitf_RESubheader *subheader = _GetObj(env, self);
    return _GetFieldObj(env, subheader->version);
}


/*
 * Class:     nitf_RESubheader
 * Method:    getSubheaderFieldsLength
 * Signature: ()Lnitf/Field;
 */
JNIEXPORT jobject JNICALL Java_nitf_RESubheader_getSubheaderFieldsLength
    (JNIEnv * env, jobject self)
{
    nitf_RESubheader *subheader = _GetObj(env, self);
    return _GetFieldObj(env, subheader->subheaderFieldsLength);
}


/*
 * Class:     nitf_RESubheader
 * Method:    getSubheaderFields
 * Signature: ()[C
 */
JNIEXPORT jcharArray JNICALL Java_nitf_RESubheader_getSubheaderFields
    (JNIEnv * env, jobject self)
{
    nitf_RESubheader *subheader = _GetObj(env, self);

    /* TODO */
    return NULL;
}


/*
 * Class:     nitf_RESubheader
 * Method:    getDataLength
 * Signature: ()J
 */
JNIEXPORT jlong JNICALL Java_nitf_RESubheader_getDataLength
    (JNIEnv * env, jobject self)
{
    nitf_RESubheader *subheader = _GetObj(env, self);
    return subheader->dataLength;
}


/*
 * Class:     nitf_RESubheader
 * Method:    setDataLength
 * Signature: (J)V
 */
JNIEXPORT void JNICALL Java_nitf_RESubheader_setDataLength
    (JNIEnv * env, jobject self, jlong dataLength)
{
    nitf_RESubheader *subheader = _GetObj(env, self);
    subheader->dataLength = dataLength;
}

