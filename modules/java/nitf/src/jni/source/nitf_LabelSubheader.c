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
#include "nitf_LabelSubheader.h"
#include "nitf_JNI.h"

NITF_JNI_DECLARE_OBJ(nitf_LabelSubheader)
/*
 * Class:     nitf_LabelSubheader
 * Method:    getFilePartType
 * Signature: ()Lnitf/Field;
 */
JNIEXPORT jobject JNICALL Java_nitf_LabelSubheader_getFilePartType
    (JNIEnv * env, jobject self)
{
    nitf_LabelSubheader *subheader = _GetObj(env, self);
    return _GetFieldObj(env, subheader->filePartType);
}


/*
 * Class:     nitf_LabelSubheader
 * Method:    getLabelID
 * Signature: ()Lnitf/Field;
 */
JNIEXPORT jobject JNICALL Java_nitf_LabelSubheader_getLabelID
    (JNIEnv * env, jobject self)
{
    nitf_LabelSubheader *subheader = _GetObj(env, self);
    return _GetFieldObj(env, subheader->labelID);
}


/*
 * Class:     nitf_LabelSubheader
 * Method:    getSecurityClass
 * Signature: ()Lnitf/Field;
 */
JNIEXPORT jobject JNICALL Java_nitf_LabelSubheader_getSecurityClass
    (JNIEnv * env, jobject self)
{
    nitf_LabelSubheader *subheader = _GetObj(env, self);
    return _GetFieldObj(env, subheader->securityClass);
}


/*
 * Class:     nitf_LabelSubheader
 * Method:    getSecurityGroup
 * Signature: ()Lnitf/FileSecurity;
 */
JNIEXPORT jobject JNICALL Java_nitf_LabelSubheader_getSecurityGroup
    (JNIEnv * env, jobject self)
{
    nitf_LabelSubheader *subheader = _GetObj(env, self);
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
 * Class:     nitf_LabelSubheader
 * Method:    getEncrypted
 * Signature: ()Lnitf/Field;
 */
JNIEXPORT jobject JNICALL Java_nitf_LabelSubheader_getEncrypted
    (JNIEnv * env, jobject self)
{
    nitf_LabelSubheader *subheader = _GetObj(env, self);
    return _GetFieldObj(env, subheader->encrypted);
}


/*
 * Class:     nitf_LabelSubheader
 * Method:    getFontStyle
 * Signature: ()Lnitf/Field;
 */
JNIEXPORT jobject JNICALL Java_nitf_LabelSubheader_getFontStyle
    (JNIEnv * env, jobject self)
{
    nitf_LabelSubheader *subheader = _GetObj(env, self);
    return _GetFieldObj(env, subheader->fontStyle);
}


/*
 * Class:     nitf_LabelSubheader
 * Method:    getCellWidth
 * Signature: ()Lnitf/Field;
 */
JNIEXPORT jobject JNICALL Java_nitf_LabelSubheader_getCellWidth
    (JNIEnv * env, jobject self)
{
    nitf_LabelSubheader *subheader = _GetObj(env, self);
    return _GetFieldObj(env, subheader->cellWidth);
}


/*
 * Class:     nitf_LabelSubheader
 * Method:    getCellHeight
 * Signature: ()Lnitf/Field;
 */
JNIEXPORT jobject JNICALL Java_nitf_LabelSubheader_getCellHeight
    (JNIEnv * env, jobject self)
{
    nitf_LabelSubheader *subheader = _GetObj(env, self);
    return _GetFieldObj(env, subheader->cellHeight);
}


/*
 * Class:     nitf_LabelSubheader
 * Method:    getDisplayLevel
 * Signature: ()Lnitf/Field;
 */
JNIEXPORT jobject JNICALL Java_nitf_LabelSubheader_getDisplayLevel
    (JNIEnv * env, jobject self)
{
    nitf_LabelSubheader *subheader = _GetObj(env, self);
    return _GetFieldObj(env, subheader->displayLevel);
}


/*
 * Class:     nitf_LabelSubheader
 * Method:    getAttachmentLevel
 * Signature: ()Lnitf/Field;
 */
JNIEXPORT jobject JNICALL Java_nitf_LabelSubheader_getAttachmentLevel
    (JNIEnv * env, jobject self)
{
    nitf_LabelSubheader *subheader = _GetObj(env, self);
    return _GetFieldObj(env, subheader->attachmentLevel);
}


/*
 * Class:     nitf_LabelSubheader
 * Method:    getLocationRow
 * Signature: ()Lnitf/Field;
 */
JNIEXPORT jobject JNICALL Java_nitf_LabelSubheader_getLocationRow
    (JNIEnv * env, jobject self)
{
    nitf_LabelSubheader *subheader = _GetObj(env, self);
    return _GetFieldObj(env, subheader->locationRow);
}


/*
 * Class:     nitf_LabelSubheader
 * Method:    getLocationColumn
 * Signature: ()Lnitf/Field;
 */
JNIEXPORT jobject JNICALL Java_nitf_LabelSubheader_getLocationColumn
    (JNIEnv * env, jobject self)
{
    nitf_LabelSubheader *subheader = _GetObj(env, self);
    return _GetFieldObj(env, subheader->locationColumn);
}


/*
 * Class:     nitf_LabelSubheader
 * Method:    getTextColor
 * Signature: ()Lnitf/Field;
 */
JNIEXPORT jobject JNICALL Java_nitf_LabelSubheader_getTextColor
    (JNIEnv * env, jobject self)
{
    nitf_LabelSubheader *subheader = _GetObj(env, self);
    return _GetFieldObj(env, subheader->textColor);
}


/*
 * Class:     nitf_LabelSubheader
 * Method:    getBackgroundColor
 * Signature: ()Lnitf/Field;
 */
JNIEXPORT jobject JNICALL Java_nitf_LabelSubheader_getBackgroundColor
    (JNIEnv * env, jobject self)
{
    nitf_LabelSubheader *subheader = _GetObj(env, self);
    return _GetFieldObj(env, subheader->backgroundColor);
}


/*
 * Class:     nitf_LabelSubheader
 * Method:    getExtendedHeaderLength
 * Signature: ()Lnitf/Field;
 */
JNIEXPORT jobject JNICALL Java_nitf_LabelSubheader_getExtendedHeaderLength
    (JNIEnv * env, jobject self)
{
    nitf_LabelSubheader *subheader = _GetObj(env, self);
    return _GetFieldObj(env, subheader->extendedHeaderLength);
}


/*
 * Class:     nitf_LabelSubheader
 * Method:    getExtendedHeaderOverflow
 * Signature: ()Lnitf/Field;
 */
JNIEXPORT jobject JNICALL
    Java_nitf_LabelSubheader_getExtendedHeaderOverflow(JNIEnv * env,
                                                       jobject self)
{
    nitf_LabelSubheader *subheader = _GetObj(env, self);
    return _GetFieldObj(env, subheader->extendedHeaderOverflow);
}


/*
 * Class:     nitf_LabelSubheader
 * Method:    getExtendedSection
 * Signature: ()Lnitf/Extensions;
 */
JNIEXPORT jobject JNICALL Java_nitf_LabelSubheader_getExtendedSection
    (JNIEnv * env, jobject self)
{
    nitf_LabelSubheader *subheader = _GetObj(env, self);
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

