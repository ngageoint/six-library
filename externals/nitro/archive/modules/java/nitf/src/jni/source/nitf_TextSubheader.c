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
#include "nitf_TextSubheader.h"
#include "nitf_JNI.h"

NITF_JNI_DECLARE_OBJ(nitf_TextSubheader)
/*
 * Class:     nitf_TextSubheader
 * Method:    getFilePartType
 * Signature: ()Lnitf/Field;
 */
JNIEXPORT jobject JNICALL Java_nitf_TextSubheader_getFilePartType
    (JNIEnv * env, jobject self)
{
    nitf_TextSubheader *subheader = _GetObj(env, self);
    return _GetFieldObj(env, subheader->filePartType);
}


/*
 * Class:     nitf_TextSubheader
 * Method:    getTextID
 * Signature: ()Lnitf/Field;
 */
JNIEXPORT jobject JNICALL Java_nitf_TextSubheader_getTextID
    (JNIEnv * env, jobject self)
{
    nitf_TextSubheader *subheader = _GetObj(env, self);
    return _GetFieldObj(env, subheader->textID);
}


/*
 * Class:     nitf_TextSubheader
 * Method:    getAttachmentLevel
 * Signature: ()Lnitf/Field;
 */
JNIEXPORT jobject JNICALL Java_nitf_TextSubheader_getAttachmentLevel
    (JNIEnv * env, jobject self)
{
    nitf_TextSubheader *subheader = _GetObj(env, self);
    return _GetFieldObj(env, subheader->attachmentLevel);
}


/*
 * Class:     nitf_TextSubheader
 * Method:    getDateTime
 * Signature: ()Lnitf/Field;
 */
JNIEXPORT jobject JNICALL Java_nitf_TextSubheader_getDateTime
    (JNIEnv * env, jobject self)
{
    nitf_TextSubheader *subheader = _GetObj(env, self);
    return _GetFieldObj(env, subheader->dateTime);
}


/*
 * Class:     nitf_TextSubheader
 * Method:    getTitle
 * Signature: ()Lnitf/Field;
 */
JNIEXPORT jobject JNICALL Java_nitf_TextSubheader_getTitle
    (JNIEnv * env, jobject self)
{
    nitf_TextSubheader *subheader = _GetObj(env, self);
    return _GetFieldObj(env, subheader->title);
}


/*
 * Class:     nitf_TextSubheader
 * Method:    getSecurityClass
 * Signature: ()Lnitf/Field;
 */
JNIEXPORT jobject JNICALL Java_nitf_TextSubheader_getSecurityClass
    (JNIEnv * env, jobject self)
{
    nitf_TextSubheader *subheader = _GetObj(env, self);
    return _GetFieldObj(env, subheader->securityClass);
}


/*
 * Class:     nitf_TextSubheader
 * Method:    getSecurityGroup
 * Signature: ()Lnitf/FileSecurity;
 */
JNIEXPORT jobject JNICALL Java_nitf_TextSubheader_getSecurityGroup
    (JNIEnv * env, jobject self)
{
    nitf_TextSubheader *subheader = _GetObj(env, self);
    jclass securityClass = (*env)->FindClass(env, "nitf/FileSecurity");
    jmethodID methodID =
        (*env)->GetMethodID(env, securityClass, "<init>", "(J)V");
    jobject security = (*env)->NewObject(env,
                                         securityClass,
                                         methodID,
                                         (jlong) subheader->securityGroup);
    return security;
}


/*
 * Class:     nitf_TextSubheader
 * Method:    getEncrypted
 * Signature: ()Lnitf/Field;
 */
JNIEXPORT jobject JNICALL Java_nitf_TextSubheader_getEncrypted
    (JNIEnv * env, jobject self)
{
    nitf_TextSubheader *subheader = _GetObj(env, self);
    return _GetFieldObj(env, subheader->encrypted);
}


/*
 * Class:     nitf_TextSubheader
 * Method:    getFormat
 * Signature: ()Lnitf/Field;
 */
JNIEXPORT jobject JNICALL Java_nitf_TextSubheader_getFormat
    (JNIEnv * env, jobject self)
{
    nitf_TextSubheader *subheader = _GetObj(env, self);
    return _GetFieldObj(env, subheader->format);
}


/*
 * Class:     nitf_TextSubheader
 * Method:    getExtendedHeaderLength
 * Signature: ()Lnitf/Field;
 */
JNIEXPORT jobject JNICALL Java_nitf_TextSubheader_getExtendedHeaderLength
    (JNIEnv * env, jobject self)
{
    nitf_TextSubheader *subheader = _GetObj(env, self);
    return _GetFieldObj(env, subheader->extendedHeaderLength);
}


/*
 * Class:     nitf_TextSubheader
 * Method:    getExtendedHeaderOverflow
 * Signature: ()Lnitf/Field;
 */
JNIEXPORT jobject JNICALL Java_nitf_TextSubheader_getExtendedHeaderOverflow
    (JNIEnv * env, jobject self)
{
    nitf_TextSubheader *subheader = _GetObj(env, self);
    return _GetFieldObj(env, subheader->extendedHeaderOverflow);
}


/*
 * Class:     nitf_TextSubheader
 * Method:    getExtendedSection
 * Signature: ()Lnitf/Extensions;
 */
JNIEXPORT jobject JNICALL Java_nitf_TextSubheader_getExtendedSection
    (JNIEnv * env, jobject self)
{
    nitf_TextSubheader *subheader = _GetObj(env, self);
    jclass extensionsClass = (*env)->FindClass(env, "nitf/Extensions");
    jmethodID methodID =
        (*env)->GetMethodID(env, extensionsClass, "<init>", "(J)V");
    jobject extensions = (*env)->NewObject(env,
                                           extensionsClass,
                                           methodID,
                                           (jlong) subheader->
                                           extendedSection);
    return extensions;
}

