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
#include "nitf_FileHeader.h"
#include "nitf_JNI.h"

NITF_JNI_DECLARE_OBJ(nitf_FileHeader)
/*
 * Class:     nitf_FileHeader
 * Method:    getBackgroundColor
 * Signature: ()Lnitf/Field;
 */
JNIEXPORT jobject JNICALL Java_nitf_FileHeader_getBackgroundColor
    (JNIEnv * env, jobject self)
{
    nitf_FileHeader *header = _GetObj(env, self);
    return _GetFieldObj(env, header->backgroundColor);
}


/*
 * Class:     nitf_FileHeader
 * Method:    getClassification
 * Signature: ()Lnitf/Field;
 */
JNIEXPORT jobject JNICALL Java_nitf_FileHeader_getClassification
    (JNIEnv * env, jobject self)
{
    nitf_FileHeader *header = _GetObj(env, self);
    return _GetFieldObj(env, header->classification);
}


/*
 * Class:     nitf_FileHeader
 * Method:    getComplianceLevel
 * Signature: ()Lnitf/Field;
 */
JNIEXPORT jobject JNICALL Java_nitf_FileHeader_getComplianceLevel
    (JNIEnv * env, jobject self)
{

    nitf_FileHeader *header = _GetObj(env, self);
    return _GetFieldObj(env, header->complianceLevel);
}


/*
 * Class:     nitf_FileHeader
 * Method:    getEncrypted
 * Signature: ()Lnitf/Field;
 */
JNIEXPORT jobject JNICALL Java_nitf_FileHeader_getEncrypted
    (JNIEnv * env, jobject self)
{
    nitf_FileHeader *header = _GetObj(env, self);
    return _GetFieldObj(env, header->encrypted);
}


/*
 * Class:     nitf_FileHeader
 * Method:    getExtendedHeaderLength
 * Signature: ()Lnitf/Field;
 */
JNIEXPORT jobject JNICALL Java_nitf_FileHeader_getExtendedHeaderLength
    (JNIEnv * env, jobject self)
{
    nitf_FileHeader *header = _GetObj(env, self);
    return _GetFieldObj(env, header->extendedHeaderLength);
}


/*
 * Class:     nitf_FileHeader
 * Method:    getExtendedHeaderOverflow
 * Signature: ()Lnitf/Field;
 */
JNIEXPORT jobject JNICALL Java_nitf_FileHeader_getExtendedHeaderOverflow
    (JNIEnv * env, jobject self)
{
    nitf_FileHeader *header = _GetObj(env, self);
    return _GetFieldObj(env, header->extendedHeaderOverflow);
}


/*
 * Class:     nitf_FileHeader
 * Method:    getFileDateTime
 * Signature: ()Lnitf/Field;
 */
JNIEXPORT jobject JNICALL Java_nitf_FileHeader_getFileDateTime
    (JNIEnv * env, jobject self)
{
    nitf_FileHeader *header = _GetObj(env, self);
    return _GetFieldObj(env, header->fileDateTime);
}


/*
 * Class:     nitf_FileHeader
 * Method:    getFileHeader
 * Signature: ()Lnitf/Field;
 */
JNIEXPORT jobject JNICALL Java_nitf_FileHeader_getFileHeader
    (JNIEnv * env, jobject self)
{
    nitf_FileHeader *header = _GetObj(env, self);
    return _GetFieldObj(env, header->fileHeader);
}


/*
 * Class:     nitf_FileHeader
 * Method:    getFileLength
 * Signature: ()Lnitf/Field;
 */
JNIEXPORT jobject JNICALL Java_nitf_FileHeader_getFileLength
    (JNIEnv * env, jobject self)
{
    nitf_FileHeader *header = _GetObj(env, self);
    return _GetFieldObj(env, header->fileLength);
}


/*
 * Class:     nitf_FileHeader
 * Method:    getFileTitle
 * Signature: ()Lnitf/Field;
 */
JNIEXPORT jobject JNICALL Java_nitf_FileHeader_getFileTitle
    (JNIEnv * env, jobject self)
{
    nitf_FileHeader *header = _GetObj(env, self);
    return _GetFieldObj(env, header->fileTitle);
}


/*
 * Class:     nitf_FileHeader
 * Method:    getFileVersion
 * Signature: ()Lnitf/Field;
 */
JNIEXPORT jobject JNICALL Java_nitf_FileHeader_getFileVersion
    (JNIEnv * env, jobject self)
{
    nitf_FileHeader *header = _GetObj(env, self);
    return _GetFieldObj(env, header->fileVersion);
}


/*
 * Class:     nitf_FileHeader
 * Method:    getHeaderLength
 * Signature: ()Lnitf/Field;
 */
JNIEXPORT jobject JNICALL Java_nitf_FileHeader_getHeaderLength
    (JNIEnv * env, jobject self)
{

    nitf_FileHeader *header = _GetObj(env, self);
    return _GetFieldObj(env, header->headerLength);
}


/*
 * Class:     nitf_FileHeader
 * Method:    getMessageCopyNum
 * Signature: ()Lnitf/Field;
 */
JNIEXPORT jobject JNICALL Java_nitf_FileHeader_getMessageCopyNum
    (JNIEnv * env, jobject self)
{
    nitf_FileHeader *header = _GetObj(env, self);
    return _GetFieldObj(env, header->messageCopyNum);
}


/*
 * Class:     nitf_FileHeader
 * Method:    getMessageNumCopies
 * Signature: ()Lnitf/Field;
 */
JNIEXPORT jobject JNICALL Java_nitf_FileHeader_getMessageNumCopies
    (JNIEnv * env, jobject self)
{
    nitf_FileHeader *header = _GetObj(env, self);
    return _GetFieldObj(env, header->messageNumCopies);
}


/*
 * Class:     nitf_FileHeader
 * Method:    getNumDataExtensions
 * Signature: ()Lnitf/Field;
 */
JNIEXPORT jobject JNICALL Java_nitf_FileHeader_getNumDataExtensions
    (JNIEnv * env, jobject self)
{
    nitf_FileHeader *header = _GetObj(env, self);
    return _GetFieldObj(env, header->numDataExtensions);
}


/*
 * Class:     nitf_FileHeader
 * Method:    getNumGraphics
 * Signature: ()Lnitf/Field;
 */
JNIEXPORT jobject JNICALL Java_nitf_FileHeader_getNumGraphics
    (JNIEnv * env, jobject self)
{
    nitf_FileHeader *header = _GetObj(env, self);
    return _GetFieldObj(env, header->numGraphics);
}


/*
 * Class:     nitf_FileHeader
 * Method:    getNumImages
 * Signature: ()Lnitf/Field;
 */
JNIEXPORT jobject JNICALL Java_nitf_FileHeader_getNumImages
    (JNIEnv * env, jobject self)
{
    nitf_FileHeader *header = _GetObj(env, self);
    return _GetFieldObj(env, header->numImages);
}


/*
 * Class:     nitf_FileHeader
 * Method:    getNumLabels
 * Signature: ()Lnitf/Field;
 */
JNIEXPORT jobject JNICALL Java_nitf_FileHeader_getNumLabels
    (JNIEnv * env, jobject self)
{
    nitf_FileHeader *header = _GetObj(env, self);
    return _GetFieldObj(env, header->numLabels);
}


/*
 * Class:     nitf_FileHeader
 * Method:    getNumReservedExtensions
 * Signature: ()Lnitf/Field;
 */
JNIEXPORT jobject JNICALL Java_nitf_FileHeader_getNumReservedExtensions
    (JNIEnv * env, jobject self)
{
    nitf_FileHeader *header = _GetObj(env, self);
    return _GetFieldObj(env, header->numReservedExtensions);
}


/*
 * Class:     nitf_FileHeader
 * Method:    getNumTexts
 * Signature: ()Lnitf/Field;
 */
JNIEXPORT jobject JNICALL Java_nitf_FileHeader_getNumTexts
    (JNIEnv * env, jobject self)
{
    nitf_FileHeader *header = _GetObj(env, self);
    return _GetFieldObj(env, header->numTexts);
}


/*
 * Class:     nitf_FileHeader
 * Method:    getOriginatorName
 * Signature: ()Lnitf/Field;
 */
JNIEXPORT jobject JNICALL Java_nitf_FileHeader_getOriginatorName
    (JNIEnv * env, jobject self)
{
    nitf_FileHeader *header = _GetObj(env, self);
    return _GetFieldObj(env, header->originatorName);
}


/*
 * Class:     nitf_FileHeader
 * Method:    getOriginatorPhone
 * Signature: ()Lnitf/Field;
 */
JNIEXPORT jobject JNICALL Java_nitf_FileHeader_getOriginatorPhone
    (JNIEnv * env, jobject self)
{
    nitf_FileHeader *header = _GetObj(env, self);
    return _GetFieldObj(env, header->originatorPhone);
}


/*
 * Class:     nitf_FileHeader
 * Method:    getOriginStationID
 * Signature: ()Lnitf/Field;
 */
JNIEXPORT jobject JNICALL Java_nitf_FileHeader_getOriginStationID
    (JNIEnv * env, jobject self)
{
    nitf_FileHeader *header = _GetObj(env, self);
    return _GetFieldObj(env, header->originStationID);
}


/*
 * Class:     nitf_FileHeader
 * Method:    getSecurityGroup
 * Signature: ()Lnitf/FileSecurity
 */
JNIEXPORT jobject JNICALL Java_nitf_FileHeader_getSecurityGroup
    (JNIEnv * env, jobject self)
{
    nitf_FileHeader *header = _GetObj(env, self);
    jclass securityClass = (*env)->FindClass(env, "nitf/FileSecurity");
    jmethodID methodID =
        (*env)->GetMethodID(env, securityClass, "<init>", "(J)V");
    jobject securityGroup = (*env)->NewObject(env,
                                              securityClass,
                                              methodID,
                                              (jlong) header->
                                              securityGroup);
    return securityGroup;
}


/*
 * Class:     nitf_FileHeader
 * Method:    getSystemType
 * Signature: ()Lnitf/Field;
 */
JNIEXPORT jobject JNICALL Java_nitf_FileHeader_getSystemType
    (JNIEnv * env, jobject self)
{
    nitf_FileHeader *header = _GetObj(env, self);
    return _GetFieldObj(env, header->systemType);
}


/*
 * Class:     nitf_FileHeader
 * Method:    getUserDefinedHeaderLength
 * Signature: ()Lnitf/Field;
 */
JNIEXPORT jobject JNICALL Java_nitf_FileHeader_getUserDefinedHeaderLength
    (JNIEnv * env, jobject self)
{

    nitf_FileHeader *header = _GetObj(env, self);
    return _GetFieldObj(env, header->userDefinedHeaderLength);
}


/*
 * Class:     nitf_FileHeader
 * Method:    getUserDefinedOverflow
 * Signature: ()Lnitf/Field;
 */
JNIEXPORT jobject JNICALL Java_nitf_FileHeader_getUserDefinedOverflow
    (JNIEnv * env, jobject self)
{
    nitf_FileHeader *header = _GetObj(env, self);
    return _GetFieldObj(env, header->userDefinedOverflow);
}


/*
 * Class:     nitf_FileHeader
 * Method:    getImageInfo
 * Signature: ()[Lnitf/ComponentInfo;
 */
JNIEXPORT jobjectArray JNICALL Java_nitf_FileHeader_getImageInfo
    (JNIEnv * env, jobject self)
{

    nitf_FileHeader *header = _GetObj(env, self);

    jclass infoClass = (*env)->FindClass(env, "nitf/ComponentInfo");
    nitf_Error error;
    jobjectArray info;
    jobject element;
    jint i;
    uint32_t num;
    jmethodID methodID;

    NITF_TRY_GET_UINT32(header->numImages, &num, &error);
    info = (*env)->NewObjectArray(env, num, infoClass, NULL);
    methodID = (*env)->GetMethodID(env, infoClass, "<init>", "(J)V");
    for (i = 0; i < num; ++i)
    {
        element = (*env)->NewObject(env,
                                    infoClass,
                                    methodID,
                                    (jlong) header->imageInfo[i]);
        (*env)->SetObjectArrayElement(env, info, i, element);
    }
    return info;

  CATCH_ERROR:
    _ThrowNITFException(env, error.message);
    return NULL;
}


/*
 * Class:     nitf_FileHeader
 * Method:    getGraphicInfo
 * Signature: ()[Lnitf/ComponentInfo;
 */
JNIEXPORT jobjectArray JNICALL Java_nitf_FileHeader_getGraphicInfo
    (JNIEnv * env, jobject self)
{

    nitf_FileHeader *header = _GetObj(env, self);
    jclass infoClass = (*env)->FindClass(env, "nitf/ComponentInfo");
    nitf_Error error;
    jobjectArray info;
    jobject element;
    jint i;
    jint num;
    jmethodID methodID;

    NITF_TRY_GET_UINT32(header->numGraphics, &num, &error);
    info = (*env)->NewObjectArray(env, num, infoClass, NULL);
    methodID = (*env)->GetMethodID(env, infoClass, "<init>", "(J)V");
    for (i = 0; i < num; ++i)
    {
        element = (*env)->NewObject(env,
                                    infoClass,
                                    methodID,
                                    (jlong) header->graphicInfo[i]);
        (*env)->SetObjectArrayElement(env, info, i, element);
    }
    return info;

  CATCH_ERROR:
    _ThrowNITFException(env, error.message);
    return NULL;
}


/*
 * Class:     nitf_FileHeader
 * Method:    getLabelInfo
 * Signature: ()[Lnitf/ComponentInfo;
 */
JNIEXPORT jobjectArray JNICALL Java_nitf_FileHeader_getLabelInfo
    (JNIEnv * env, jobject self)
{

    nitf_FileHeader *header = _GetObj(env, self);
    jclass infoClass = (*env)->FindClass(env, "nitf/ComponentInfo");
    nitf_Error error;
    jobjectArray info;
    jobject element;
    jint i;
    jint num;
    jmethodID methodID;

    NITF_TRY_GET_UINT32(header->numLabels, &num, &error);
    info = (*env)->NewObjectArray(env, num, infoClass, NULL);
    methodID = (*env)->GetMethodID(env, infoClass, "<init>", "(J)V");
    for (i = 0; i < num; ++i)
    {
        element = (*env)->NewObject(env,
                                    infoClass,
                                    methodID,
                                    (jlong) header->labelInfo[i]);
        (*env)->SetObjectArrayElement(env, info, i, element);
    }
    return info;

  CATCH_ERROR:
    _ThrowNITFException(env, error.message);
    return NULL;
}


/*
 * Class:     nitf_FileHeader
 * Method:    getTextInfo
 * Signature: ()[Lnitf/ComponentInfo;
 */
JNIEXPORT jobjectArray JNICALL Java_nitf_FileHeader_getTextInfo
    (JNIEnv * env, jobject self)
{

    nitf_FileHeader *header = _GetObj(env, self);
    jclass infoClass = (*env)->FindClass(env, "nitf/ComponentInfo");
    nitf_Error error;
    jobjectArray info;
    jobject element;
    jint i;
    jint num;
    jmethodID methodID;
    NITF_TRY_GET_UINT32(header->numTexts, &num, &error);
    info = (*env)->NewObjectArray(env, num, infoClass, NULL);
    methodID = (*env)->GetMethodID(env, infoClass, "<init>", "(J)V");
    for (i = 0; i < num; ++i)
    {
        element = (*env)->NewObject(env,
                                    infoClass,
                                    methodID, (jlong) header->textInfo[i]);
        (*env)->SetObjectArrayElement(env, info, i, element);
    }
    return info;

  CATCH_ERROR:
    _ThrowNITFException(env, error.message);
    return NULL;
}


/*
 * Class:     nitf_FileHeader
 * Method:    getDataExtensionInfo
 * Signature: ()[Lnitf/ComponentInfo;
 */
JNIEXPORT jobjectArray JNICALL Java_nitf_FileHeader_getDataExtensionInfo
    (JNIEnv * env, jobject self)
{

    nitf_FileHeader *header = _GetObj(env, self);
    jclass infoClass = (*env)->FindClass(env, "nitf/ComponentInfo");
    nitf_Error error;
    jobjectArray info;
    jobject element;
    jint i;
    jint num;
    jmethodID methodID;

    NITF_TRY_GET_UINT32(header->numDataExtensions, &num, &error);
    info = (*env)->NewObjectArray(env, num, infoClass, NULL);
    methodID = (*env)->GetMethodID(env, infoClass, "<init>", "(J)V");
    for (i = 0; i < num; ++i)
    {
        element = (*env)->NewObject(env,
                                    infoClass,
                                    methodID,
                                    (jlong) header->dataExtensionInfo[i]);
        (*env)->SetObjectArrayElement(env, info, i, element);
    }
    return info;

  CATCH_ERROR:
    _ThrowNITFException(env, error.message);
    return NULL;
}


/*
 * Class:     nitf_FileHeader
 * Method:    getReservedExtensionInfo
 * Signature: ()[Lnitf/ComponentInfo;
 */
JNIEXPORT jobjectArray JNICALL
    Java_nitf_FileHeader_getReservedExtensionInfo(JNIEnv * env,
                                                  jobject self)
{

    nitf_FileHeader *header = _GetObj(env, self);
    jclass infoClass = (*env)->FindClass(env, "nitf/ComponentInfo");
    nitf_Error error;
    jobjectArray info;
    jobject element;
    jint i;
    jint num;
    jmethodID methodID;

    NITF_TRY_GET_UINT32(header->numReservedExtensions, &num, &error);
    info = (*env)->NewObjectArray(env, num, infoClass, NULL);
    methodID = (*env)->GetMethodID(env, infoClass, "<init>", "(J)V");
    for (i = 0; i < num; ++i)
    {
        element = (*env)->NewObject(env,
                                    infoClass,
                                    methodID,
                                    (jlong) header->
                                    reservedExtensionInfo[i]);
        (*env)->SetObjectArrayElement(env, info, i, element);
    }
    return info;

  CATCH_ERROR:
    _ThrowNITFException(env, error.message);
    return NULL;
}


/*
 * Class:     nitf_FileHeader
 * Method:    getUserDefinedSection
 * Signature: ()Lnitf/Extensions;
 */
JNIEXPORT jobject JNICALL Java_nitf_FileHeader_getUserDefinedSection
    (JNIEnv * env, jobject self)
{
    nitf_FileHeader *header = _GetObj(env, self);
    jclass extensionsClass = (*env)->FindClass(env, "nitf/Extensions");
    jmethodID methodID =
        (*env)->GetMethodID(env, extensionsClass, "<init>", "(J)V");
    nitf_Error error;
    jobject extensions;

    /* we need to construct it if it doesn't already exist */
    if (!header->userDefinedSection)
    {
        header->userDefinedSection = nitf_Extensions_construct(&error);
        if (!header->userDefinedSection)
            goto CATCH_ERROR;
    }

    extensions = (*env)->NewObject(env,
                                   extensionsClass,
                                   methodID,
                                   (jlong) header->userDefinedSection);
    return extensions;

  CATCH_ERROR:
    _ThrowNITFException(env, error.message);
    return NULL;
}


/*
 * Class:     nitf_FileHeader
 * Method:    getExtendedSection
 * Signature: ()Lnitf/Extensions;
 */
JNIEXPORT jobject JNICALL Java_nitf_FileHeader_getExtendedSection
    (JNIEnv * env, jobject self)
{
    nitf_FileHeader *header = _GetObj(env, self);
    jclass extensionsClass = (*env)->FindClass(env, "nitf/Extensions");
    jmethodID methodID =
        (*env)->GetMethodID(env, extensionsClass, "<init>", "(J)V");
    nitf_Error error;
    jobject extensions;

    /* we need to construct it if it doesn't already exist */
    if (!header->extendedSection)
    {
        header->extendedSection = nitf_Extensions_construct(&error);
        if (!header->extendedSection)
            goto CATCH_ERROR;
    }

    extensions = (*env)->NewObject(env,
                                   extensionsClass,
                                   methodID,
                                   (jlong) header->extendedSection);
    return extensions;

  CATCH_ERROR:
    _ThrowNITFException(env, error.message);
    return NULL;
}

