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
#include "nitf_DESubheader.h"
#include "nitf_JNI.h"

NITF_JNI_DECLARE_OBJ(nitf_DESubheader)
/*
 * Class:     nitf_DESubheader
 * Method:    getFilePartType
 * Signature: ()Lnitf/Field;
 */
JNIEXPORT jobject JNICALL Java_nitf_DESubheader_getFilePartType
    (JNIEnv * env, jobject self)
{
    nitf_DESubheader *header = _GetObj(env, self);
    return _GetFieldObj(env, header->filePartType);
}


/*
 * Class:     nitf_DESubheader
 * Method:    getTypeID
 * Signature: ()Lnitf/Field;
 */
JNIEXPORT jobject JNICALL Java_nitf_DESubheader_getTypeID
    (JNIEnv * env, jobject self)
{
    nitf_DESubheader *header = _GetObj(env, self);
    return _GetFieldObj(env, header->typeID);
}


/*
 * Class:     nitf_DESubheader
 * Method:    getSecurityClass
 * Signature: ()Lnitf/Field;
 */
JNIEXPORT jobject JNICALL Java_nitf_DESubheader_getSecurityClass
    (JNIEnv * env, jobject self)
{

    nitf_DESubheader *header = _GetObj(env, self);
    return _GetFieldObj(env, header->securityClass);
}


/*
 * Class:     nitf_DESubheader
 * Method:    getSecurityGroup
 * Signature: ()Lnitf/FileSecurity;
 */
JNIEXPORT jobject JNICALL Java_nitf_DESubheader_getSecurityGroup
    (JNIEnv * env, jobject self)
{
    nitf_DESubheader *header = _GetObj(env, self);
    jclass securityClass = (*env)->FindClass(env, "nitf/FileSecurity");
    jobject security;
    jmethodID methodID =
        (*env)->GetMethodID(env, securityClass, "<init>", "(J)V");
    security =
        (*env)->NewObject(env, securityClass, methodID,
                          (jlong) header->securityGroup);
    return security;
}


/*
 * Class:     nitf_DESubheader
 * Method:    getVersion
 * Signature: ()Lnitf/Field;
 */
JNIEXPORT jobject JNICALL Java_nitf_DESubheader_getVersion
    (JNIEnv * env, jobject self)
{
    nitf_DESubheader *header = _GetObj(env, self);
    return _GetFieldObj(env, header->version);
}


/*
 * Class:     nitf_DESubheader
 * Method:    getOverflowedHeaderType
 * Signature: ()Lnitf/Field;
 */
JNIEXPORT jobject JNICALL Java_nitf_DESubheader_getOverflowedHeaderType
    (JNIEnv * env, jobject self)
{
    nitf_DESubheader *header = _GetObj(env, self);
    return _GetFieldObj(env, header->overflowedHeaderType);
}


/*
 * Class:     nitf_DESubheader
 * Method:    getDataItemOverflowed
 * Signature: ()Lnitf/Field;
 */
JNIEXPORT jobject JNICALL Java_nitf_DESubheader_getDataItemOverflowed
    (JNIEnv * env, jobject self)
{

    nitf_DESubheader *header = _GetObj(env, self);
    return _GetFieldObj(env, header->dataItemOverflowed);
}


/*
 * Class:     nitf_DESubheader
 * Method:    getSubheaderFieldsLength
 * Signature: ()Lnitf/Field;
 */
JNIEXPORT jobject JNICALL Java_nitf_DESubheader_getSubheaderFieldsLength
    (JNIEnv * env, jobject self)
{

    nitf_DESubheader *header = _GetObj(env, self);
    return _GetFieldObj(env, header->subheaderFieldsLength);
}


/*
 * Class:     nitf_DESubheader
 * Method:    getSubheaderFields
 * Signature: ()Lnitf/TRE;
 */
JNIEXPORT jobject JNICALL Java_nitf_DESubheader_getSubheaderFields
    (JNIEnv * env, jobject self)
{
    nitf_DESubheader *header = _GetObj(env, self);
    jobject tre = NULL;

    if (header->subheaderFields)
    {
        tre = _NewObject(env, (jlong)header->subheaderFields, "nitf/TRE");
        _ManageObject(env, (jlong)header->subheaderFields, JNI_FALSE);
    }
    return tre;
}


JNIEXPORT jobject JNICALL Java_nitf_DESubheader_setSubheaderFields
  (JNIEnv *env, jobject self, jobject subheaderFields)
{
    nitf_DESubheader *header = _GetObj(env, self);
    jobject tre = NULL;
    jmethodID initMethod = NULL, addressMethod = NULL;
    nitf_TRE *newTRE = NULL, *clonedTRE = NULL;
    jclass treClass = (*env)->FindClass(env, "nitf/TRE");
    nitf_Error error;

    if (subheaderFields != NULL)
    {
        initMethod = (*env)->GetMethodID(env, treClass, "<init>", "(J)V");
        addressMethod = (*env)->GetMethodID(env, treClass, "getAddress", "()J");

        /* get the address of the new one passed in */
        newTRE = (nitf_TRE*) (*env)->CallLongMethod(env, subheaderFields,
                                                    addressMethod);

        /* clone it */
        clonedTRE = nitf_TRE_clone(newTRE, &error);
        if (!clonedTRE)
        {
            _ThrowNITFException(env, error.message);
            return NULL;
        }
    }

    /* get the current TRE object, and tell Java we're done with it */
    if (header->subheaderFields)
    {
        _ManageObject(env, (jlong)header->subheaderFields, JNI_TRUE);
    }

    /* set the cloned one to the subheaderFields */
    header->subheaderFields = clonedTRE;

    if (clonedTRE != NULL)
    {
        /* create a new Java TRE from the clone and tell Java not to manage it */
        tre = _NewObject(env, (jlong)header->subheaderFields, "nitf/TRE");
        _ManageObject(env, (jlong)header->subheaderFields, JNI_FALSE);
    }
    return tre;
}

/*
 * Class:     nitf_DESubheader
 * Method:    getDataLength
 * Signature: ()J
 */
JNIEXPORT jlong JNICALL Java_nitf_DESubheader_getDataLength
    (JNIEnv * env, jobject self)
{
    nitf_DESubheader *header = _GetObj(env, self);
    return header->dataLength;
}


/*
 * Class:     nitf_DESubheader
 * Method:    setDataLength
 * Signature: (J)V
 */
JNIEXPORT void JNICALL Java_nitf_DESubheader_setDataLength
    (JNIEnv * env, jobject self, jlong dataLength)
{
    nitf_DESubheader *header = _GetObj(env, self);
    header->dataLength = dataLength;
}


/*
 * Class:     nitf_DESubheader
 * Method:    getUserDefinedSection
 * Signature: ()Lnitf/Extensions;
 */
JNIEXPORT jobject JNICALL Java_nitf_DESubheader_getUserDefinedSection
    (JNIEnv * env, jobject self)
{
    nitf_DESubheader *header = _GetObj(env, self);
    jclass extensionsClass = (*env)->FindClass(env, "nitf/Extensions");
    jmethodID methodID =
        (*env)->GetMethodID(env, extensionsClass, "<init>", "(J)V");
    jobject extensions = (*env)->NewObject(env,
                                           extensionsClass,
                                           methodID,
                                           (jlong) header->
                                           userDefinedSection);
    return extensions;
}

