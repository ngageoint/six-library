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
#include "nitf_GraphicSubheader.h"
#include "nitf_JNI.h"

NITF_JNI_DECLARE_OBJ(nitf_GraphicSubheader)
/*
 * Class:     nitf_GraphicSubheader
 * Method:    getFilePartType
 * Signature: ()Lnitf/Field;
 */
JNIEXPORT jobject JNICALL Java_nitf_GraphicSubheader_getFilePartType
    (JNIEnv * env, jobject self)
{
    nitf_GraphicSubheader *header = _GetObj(env, self);
    return _GetFieldObj(env, header->filePartType);
}


/*
 * Class:     nitf_GraphicSubheader
 * Method:    getGraphicID
 * Signature: ()Lnitf/Field;
 */
JNIEXPORT jobject JNICALL Java_nitf_GraphicSubheader_getGraphicID
    (JNIEnv * env, jobject self)
{
    nitf_GraphicSubheader *header = _GetObj(env, self);
    return _GetFieldObj(env, header->graphicID);
}


/*
 * Class:     nitf_GraphicSubheader
 * Method:    getName
 * Signature: ()Lnitf/Field;
 */
JNIEXPORT jobject JNICALL Java_nitf_GraphicSubheader_getName
    (JNIEnv * env, jobject self)
{
    nitf_GraphicSubheader *header = _GetObj(env, self);
    return _GetFieldObj(env, header->name);
}


/*
 * Class:     nitf_GraphicSubheader
 * Method:    getSecurityClass
 * Signature: ()Lnitf/Field;
 */
JNIEXPORT jobject JNICALL Java_nitf_GraphicSubheader_getSecurityClass
    (JNIEnv * env, jobject self)
{
    nitf_GraphicSubheader *header = _GetObj(env, self);
    return _GetFieldObj(env, header->securityClass);
}


/*
 * Class:     nitf_GraphicSubheader
 * Method:    getSecurityGroup
 * Signature: ()Lnitf/FileSecurity;
 */
JNIEXPORT jobject JNICALL Java_nitf_GraphicSubheader_getSecurityGroup
    (JNIEnv * env, jobject self)
{
    nitf_GraphicSubheader *header = _GetObj(env, self);
    jclass securityClass = (*env)->FindClass(env, "nitf/FileSecurity");

    jmethodID methodID = (*env)->GetMethodID(env,
                                             securityClass,
                                             "<init>",
                                             "(J)V");
    jobject security = (*env)->NewObject(env,
                                         securityClass,
                                         methodID,
                                         (jlong) header->securityGroup);
    return security;
}


/*
 * Class:     nitf_GraphicSubheader
 * Method:    getEncrypted
 * Signature: ()Lnitf/Field;
 */
JNIEXPORT jobject JNICALL Java_nitf_GraphicSubheader_getEncrypted
    (JNIEnv * env, jobject self)
{
    nitf_GraphicSubheader *header = _GetObj(env, self);
    return _GetFieldObj(env, header->encrypted);
}


/*
 * Class:     nitf_GraphicSubheader
 * Method:    getStype
 * Signature: ()Lnitf/Field;
 */
JNIEXPORT jobject JNICALL Java_nitf_GraphicSubheader_getStype
    (JNIEnv * env, jobject self)
{
    nitf_GraphicSubheader *header = _GetObj(env, self);
    return _GetFieldObj(env, header->stype);
}


/*
 * Class:     nitf_GraphicSubheader
 * Method:    getRes1
 * Signature: ()Lnitf/Field;
 */
JNIEXPORT jobject JNICALL Java_nitf_GraphicSubheader_getRes1
    (JNIEnv * env, jobject self)
{
    nitf_GraphicSubheader *header = _GetObj(env, self);
    return _GetFieldObj(env, header->res1);
}


/*
 * Class:     nitf_GraphicSubheader
 * Method:    getDisplayLevel
 * Signature: ()Lnitf/Field;
 */
JNIEXPORT jobject JNICALL Java_nitf_GraphicSubheader_getDisplayLevel
    (JNIEnv * env, jobject self)
{
    nitf_GraphicSubheader *header = _GetObj(env, self);
    return _GetFieldObj(env, header->displayLevel);
}


/*
 * Class:     nitf_GraphicSubheader
 * Method:    getAttachmentLevel
 * Signature: ()Lnitf/Field;
 */
JNIEXPORT jobject JNICALL Java_nitf_GraphicSubheader_getAttachmentLevel
    (JNIEnv * env, jobject self)
{
    nitf_GraphicSubheader *header = _GetObj(env, self);
    return _GetFieldObj(env, header->attachmentLevel);
}


/*
 * Class:     nitf_GraphicSubheader
 * Method:    getLocation
 * Signature: ()Lnitf/Field;
 */
JNIEXPORT jobject JNICALL Java_nitf_GraphicSubheader_getLocation
    (JNIEnv * env, jobject self)
{
    nitf_GraphicSubheader *header = _GetObj(env, self);
    return _GetFieldObj(env, header->location);
}


/*
 * Class:     nitf_GraphicSubheader
 * Method:    getBound1Loc
 * Signature: ()Lnitf/Field;
 */
JNIEXPORT jobject JNICALL Java_nitf_GraphicSubheader_getBound1Loc
    (JNIEnv * env, jobject self)
{
    nitf_GraphicSubheader *header = _GetObj(env, self);
    return _GetFieldObj(env, header->bound1Loc);
}


/*
 * Class:     nitf_GraphicSubheader
 * Method:    getColor
 * Signature: ()Lnitf/Field;
 */
JNIEXPORT jobject JNICALL Java_nitf_GraphicSubheader_getColor
    (JNIEnv * env, jobject self)
{
    nitf_GraphicSubheader *header = _GetObj(env, self);
    return _GetFieldObj(env, header->color);
}


/*
 * Class:     nitf_GraphicSubheader
 * Method:    getBound2Loc
 * Signature: ()Lnitf/Field;
 */
JNIEXPORT jobject JNICALL Java_nitf_GraphicSubheader_getBound2Loc
    (JNIEnv * env, jobject self)
{
    nitf_GraphicSubheader *header = _GetObj(env, self);
    return _GetFieldObj(env, header->bound2Loc);
}


/*
 * Class:     nitf_GraphicSubheader
 * Method:    getRes2
 * Signature: ()Lnitf/Field;
 */
JNIEXPORT jobject JNICALL Java_nitf_GraphicSubheader_getRes2
    (JNIEnv * env, jobject self)
{
    nitf_GraphicSubheader *header = _GetObj(env, self);
    return _GetFieldObj(env, header->res2);
}


/*
 * Class:     nitf_GraphicSubheader
 * Method:    getExtendedHeaderLength
 * Signature: ()Lnitf/Field;
 */
JNIEXPORT jobject JNICALL
    Java_nitf_GraphicSubheader_getExtendedHeaderLength(JNIEnv * env,
                                                       jobject self)
{
    nitf_GraphicSubheader *header = _GetObj(env, self);
    return _GetFieldObj(env, header->extendedHeaderLength);
}


/*
 * Class:     nitf_GraphicSubheader
 * Method:    getExtendedHeaderOverflow
 * Signature: ()Lnitf/Field;
 */
JNIEXPORT jobject JNICALL
    Java_nitf_GraphicSubheader_getExtendedHeaderOverflow(JNIEnv * env,
                                                         jobject self)
{
    nitf_GraphicSubheader *header = _GetObj(env, self);
    return _GetFieldObj(env, header->extendedHeaderOverflow);
}


/*
 * Class:     nitf_GraphicSubheader
 * Method:    getExtendedSection
 * Signature: ()Lnitf/Extensions;
 */
JNIEXPORT jobject JNICALL Java_nitf_GraphicSubheader_getExtendedSection
    (JNIEnv * env, jobject self)
{

    nitf_GraphicSubheader *header = _GetObj(env, self);
    jclass extensionsClass = (*env)->FindClass(env, "nitf/Extensions");
    jmethodID methodID = (*env)->GetMethodID(env,
                                             extensionsClass,
                                             "<init>",
                                             "(J)V");
    jobject extensions = (*env)->NewObject(env,
                                           extensionsClass,
                                           methodID,
                                           (jlong) header->
                                           extendedSection);
    return extensions;
}

