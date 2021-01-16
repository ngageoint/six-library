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
#include "nitf_ImageSubheader.h"
#include "nitf_JNI.h"

NITF_JNI_DECLARE_OBJ(nitf_ImageSubheader)
/*
 * Class:     nitf_ImageSubheader
 * Method:    getActualBitsPerPixel
 * Signature: ()Lnitf/Field;
 */
JNIEXPORT jobject JNICALL Java_nitf_ImageSubheader_getActualBitsPerPixel
    (JNIEnv * env, jobject self)
{
    nitf_ImageSubheader *header = _GetObj(env, self);
    return _GetFieldObj(env, header->actualBitsPerPixel);
}


/*
 * Class:     nitf_ImageSubheader
 * Method:    getBandInfo
 * Signature: ()[Lnitf/BandInfo;
 */
JNIEXPORT jobjectArray JNICALL Java_nitf_ImageSubheader_getBandInfo
    (JNIEnv * env, jobject self)
{
    nitf_ImageSubheader *header = _GetObj(env, self);

    jclass infoClass = (*env)->FindClass(env, "nitf/BandInfo");
    nitf_Error error;
    jobjectArray info;
    jobject element;
    jint i;
    jint numBands;
    jint numXBands;
    jmethodID methodID;

    NITF_TRY_GET_UINT32(header->numImageBands, &numBands, &error);
    NITF_TRY_GET_UINT32(header->numMultispectralImageBands, &numXBands,
                        &error);
    numBands = numXBands > 0 ? numXBands : numBands;
    info = (*env)->NewObjectArray(env, numBands, infoClass, NULL);
    methodID = (*env)->GetMethodID(env, infoClass, "<init>", "(J)V");
    for (i = 0; i < numBands; ++i)
    {
        element = (*env)->NewObject(env,
                                    infoClass,
                                    methodID, (jlong) header->bandInfo[i]);
        (*env)->SetObjectArrayElement(env, info, i, element);
    }
    return info;

  CATCH_ERROR:
    _ThrowNITFException(env, error.message);
    return NULL;
}


/*
 * Class:     nitf_ImageSubheader
 * Method:    getCompressionRate
 * Signature: ()Lnitf/Field;
 */
JNIEXPORT jobject JNICALL Java_nitf_ImageSubheader_getCompressionRate
    (JNIEnv * env, jobject self)
{
    nitf_ImageSubheader *header = _GetObj(env, self);
    return _GetFieldObj(env, header->compressionRate);
}


/*
 * Class:     nitf_ImageSubheader
 * Method:    getCornerCoordinates
 * Signature: ()Lnitf/Field;
 */
JNIEXPORT jobject JNICALL Java_nitf_ImageSubheader_getCornerCoordinates
    (JNIEnv * env, jobject self)
{
    nitf_ImageSubheader *header = _GetObj(env, self);
    return _GetFieldObj(env, header->cornerCoordinates);
}


/*
 * Class:     nitf_ImageSubheader
 * Method:    getEncrypted
 * Signature: ()Lnitf/Field;
 */
JNIEXPORT jobject JNICALL Java_nitf_ImageSubheader_getEncrypted
    (JNIEnv * env, jobject self)
{
    nitf_ImageSubheader *header = _GetObj(env, self);
    return _GetFieldObj(env, header->encrypted);
}


/*
 * Class:     nitf_ImageSubheader
 * Method:    getExtendedHeaderLength
 * Signature: ()Lnitf/Field;
 */
JNIEXPORT jobject JNICALL Java_nitf_ImageSubheader_getExtendedHeaderLength
    (JNIEnv * env, jobject self)
{
    nitf_ImageSubheader *header = _GetObj(env, self);
    return _GetFieldObj(env, header->extendedHeaderLength);
}


/*
 * Class:     nitf_ImageSubheader
 * Method:    getExtendedHeaderOverflow
 * Signature: ()Lnitf/Field;
 */
JNIEXPORT jobject JNICALL
    Java_nitf_ImageSubheader_getExtendedHeaderOverflow(JNIEnv * env,
                                                       jobject self)
{
    nitf_ImageSubheader *header = _GetObj(env, self);
    return _GetFieldObj(env, header->extendedHeaderOverflow);
}


/*
 * Class:     nitf_ImageSubheader
 * Method:    getFilePartType
 * Signature: ()Lnitf/Field;
 */
JNIEXPORT jobject JNICALL Java_nitf_ImageSubheader_getFilePartType
    (JNIEnv * env, jobject self)
{
    nitf_ImageSubheader *header = _GetObj(env, self);
    return _GetFieldObj(env, header->filePartType);
}


/*
 * Class:     nitf_ImageSubheader
 * Method:    getImageAttachmentLevel
 * Signature: ()Lnitf/Field;
 */
JNIEXPORT jobject JNICALL Java_nitf_ImageSubheader_getImageAttachmentLevel
    (JNIEnv * env, jobject self)
{
    nitf_ImageSubheader *header = _GetObj(env, self);
    return _GetFieldObj(env, header->imageAttachmentLevel);
}


/*
 * Class:     nitf_ImageSubheader
 * Method:    getImageCategory
 * Signature: ()Lnitf/Field;
 */
JNIEXPORT jobject JNICALL Java_nitf_ImageSubheader_getImageCategory
    (JNIEnv * env, jobject self)
{
    nitf_ImageSubheader *header = _GetObj(env, self);
    return _GetFieldObj(env, header->imageCategory);
}


/*
 * Class:     nitf_ImageSubheader
 * Method:    getImageComments
 * Signature: ()[Lnitf/Field;
 */
JNIEXPORT jobjectArray JNICALL Java_nitf_ImageSubheader_getImageComments
    (JNIEnv * env, jobject self)
{
    nitf_ImageSubheader *header = _GetObj(env, self);
    uint32_t numComments;
    nitf_Error error;
    jint i;
    jobjectArray array;
    jobject fieldObj;
    nitf_ListIterator iter, end;
    jclass fieldClass = (*env)->FindClass(env, "nitf/Field");

    NITF_TRY_GET_UINT32(header->numImageComments, &numComments, &error);
    if (numComments > 0)
    {
        array =
            (*env)->NewObjectArray(env, numComments, fieldClass, NULL);
        
        iter = nitf_List_begin(header->imageComments);
        end = nitf_List_end(header->imageComments);
        for (i = 0; nitf_ListIterator_notEqualTo(&iter, &end); ++i)
        {
            fieldObj = _GetFieldObj(env,
                (nitf_Field*) nitf_ListIterator_get(&iter));
            (*env)->SetObjectArrayElement(env, array, i, fieldObj);
            nitf_ListIterator_increment(&iter);
        }
    }
    else
    {
        array = (*env)->NewObjectArray(env, 0, fieldClass, NULL);
    }
    return array;

  CATCH_ERROR:
    _ThrowNITFException(env, error.message);
    return NULL;
}


/*
 * Class:     nitf_ImageSubheader
 * Method:    getImageCompression
 * Signature: ()Lnitf/Field;
 */
JNIEXPORT jobject JNICALL Java_nitf_ImageSubheader_getImageCompression
    (JNIEnv * env, jobject self)
{
    nitf_ImageSubheader *header = _GetObj(env, self);
    return _GetFieldObj(env, header->imageCompression);
}


/*
 * Class:     nitf_ImageSubheader
 * Method:    getImageCoordinateSystem
 * Signature: ()Lnitf/Field;
 */
JNIEXPORT jobject JNICALL Java_nitf_ImageSubheader_getImageCoordinateSystem
    (JNIEnv * env, jobject self)
{
    nitf_ImageSubheader *header = _GetObj(env, self);
    return _GetFieldObj(env, header->imageCoordinateSystem);
}


/*
 * Class:     nitf_ImageSubheader
 * Method:    getImageDateAndTime
 * Signature: ()Lnitf/Field;
 */
JNIEXPORT jobject JNICALL Java_nitf_ImageSubheader_getImageDateAndTime
    (JNIEnv * env, jobject self)
{
    nitf_ImageSubheader *header = _GetObj(env, self);
    return _GetFieldObj(env, header->imageDateAndTime);
}


/*
 * Class:     nitf_ImageSubheader
 * Method:    getImageDisplayLevel
 * Signature: ()Lnitf/Field;
 */
JNIEXPORT jobject JNICALL Java_nitf_ImageSubheader_getImageDisplayLevel
    (JNIEnv * env, jobject self)
{
    nitf_ImageSubheader *header = _GetObj(env, self);
    return _GetFieldObj(env, header->imageDisplayLevel);
}


/*
 * Class:     nitf_ImageSubheader
 * Method:    getImageId
 * Signature: ()Lnitf/Field;
 */
JNIEXPORT jobject JNICALL Java_nitf_ImageSubheader_getImageId
    (JNIEnv * env, jobject self)
{
    nitf_ImageSubheader *header = _GetObj(env, self);
    return _GetFieldObj(env, header->imageId);
}


/*
 * Class:     nitf_ImageSubheader
 * Method:    getImageLocation
 * Signature: ()Lnitf/Field;
 */
JNIEXPORT jobject JNICALL Java_nitf_ImageSubheader_getImageLocation
    (JNIEnv * env, jobject self)
{
    nitf_ImageSubheader *header = _GetObj(env, self);
    return _GetFieldObj(env, header->imageLocation);
}


/*
 * Class:     nitf_ImageSubheader
 * Method:    getImageMagnification
 * Signature: ()Lnitf/Field;
 */
JNIEXPORT jobject JNICALL Java_nitf_ImageSubheader_getImageMagnification
    (JNIEnv * env, jobject self)
{
    nitf_ImageSubheader *header = _GetObj(env, self);
    return _GetFieldObj(env, header->imageMagnification);
}


/*
 * Class:     nitf_ImageSubheader
 * Method:    getImageMode
 * Signature: ()Lnitf/Field;
 */
JNIEXPORT jobject JNICALL Java_nitf_ImageSubheader_getImageMode
    (JNIEnv * env, jobject self)
{
    nitf_ImageSubheader *header = _GetObj(env, self);
    return _GetFieldObj(env, header->imageMode);
}


/*
 * Class:     nitf_ImageSubheader
 * Method:    getImageRepresentation
 * Signature: ()Lnitf/Field;
 */
JNIEXPORT jobject JNICALL Java_nitf_ImageSubheader_getImageRepresentation
    (JNIEnv * env, jobject self)
{
    nitf_ImageSubheader *header = _GetObj(env, self);
    return _GetFieldObj(env, header->imageRepresentation);
}


/*
 * Class:     nitf_ImageSubheader
 * Method:    getImageSecurityClass
 * Signature: ()Lnitf/Field;
 */
JNIEXPORT jobject JNICALL Java_nitf_ImageSubheader_getImageSecurityClass
    (JNIEnv * env, jobject self)
{
    nitf_ImageSubheader *header = _GetObj(env, self);
    return _GetFieldObj(env, header->imageSecurityClass);
}


/*
 * Class:     nitf_ImageSubheader
 * Method:    getImageSource
 * Signature: ()Lnitf/Field;
 */
JNIEXPORT jobject JNICALL Java_nitf_ImageSubheader_getImageSource
    (JNIEnv * env, jobject self)
{
    nitf_ImageSubheader *header = _GetObj(env, self);
    return _GetFieldObj(env, header->imageSource);
}


/*
 * Class:     nitf_ImageSubheader
 * Method:    getImageSyncCode
 * Signature: ()Lnitf/Field;
 */
JNIEXPORT jobject JNICALL Java_nitf_ImageSubheader_getImageSyncCode
    (JNIEnv * env, jobject self)
{
    nitf_ImageSubheader *header = _GetObj(env, self);
    return _GetFieldObj(env, header->imageSyncCode);
}


/*
 * Class:     nitf_ImageSubheader
 * Method:    getImageTitle
 * Signature: ()Lnitf/Field;
 */
JNIEXPORT jobject JNICALL Java_nitf_ImageSubheader_getImageTitle
    (JNIEnv * env, jobject self)
{
    nitf_ImageSubheader *header = _GetObj(env, self);
    return _GetFieldObj(env, header->imageTitle);
}


/*
 * Class:     nitf_ImageSubheader
 * Method:    getNumBitsPerPixel
 * Signature: ()Lnitf/Field;
 */
JNIEXPORT jobject JNICALL Java_nitf_ImageSubheader_getNumBitsPerPixel
    (JNIEnv * env, jobject self)
{
    nitf_ImageSubheader *header = _GetObj(env, self);
    return _GetFieldObj(env, header->numBitsPerPixel);
}


/*
 * Class:     nitf_ImageSubheader
 * Method:    getNumBlocksPerCol
 * Signature: ()Lnitf/Field;
 */
JNIEXPORT jobject JNICALL Java_nitf_ImageSubheader_getNumBlocksPerCol
    (JNIEnv * env, jobject self)
{
    nitf_ImageSubheader *header = _GetObj(env, self);
    return _GetFieldObj(env, header->numBlocksPerCol);
}


/*
 * Class:     nitf_ImageSubheader
 * Method:    getNumBlocksPerRow
 * Signature: ()Lnitf/Field;
 */
JNIEXPORT jobject JNICALL Java_nitf_ImageSubheader_getNumBlocksPerRow
    (JNIEnv * env, jobject self)
{
    nitf_ImageSubheader *header = _GetObj(env, self);
    return _GetFieldObj(env, header->numBlocksPerRow);
}


/*
 * Class:     nitf_ImageSubheader
 * Method:    getNumCols
 * Signature: ()Lnitf/Field;
 */
JNIEXPORT jobject JNICALL Java_nitf_ImageSubheader_getNumCols
    (JNIEnv * env, jobject self)
{
    nitf_ImageSubheader *header = _GetObj(env, self);
    return _GetFieldObj(env, header->numCols);
}


/*
 * Class:     nitf_ImageSubheader
 * Method:    getNumImageBands
 * Signature: ()Lnitf/Field;
 */
JNIEXPORT jobject JNICALL Java_nitf_ImageSubheader_getNumImageBands
    (JNIEnv * env, jobject self)
{
    nitf_ImageSubheader *header = _GetObj(env, self);
    return _GetFieldObj(env, header->numImageBands);
}


/*
 * Class:     nitf_ImageSubheader
 * Method:    getNumImageComments
 * Signature: ()Lnitf/Field;
 */
JNIEXPORT jobject JNICALL Java_nitf_ImageSubheader_getNumImageComments
    (JNIEnv * env, jobject self)
{
    nitf_ImageSubheader *header = _GetObj(env, self);
    return _GetFieldObj(env, header->numImageComments);
}


/*
 * Class:     nitf_ImageSubheader
 * Method:    getNumMultispectralImageBands
 * Signature: ()Lnitf/Field;
 */
JNIEXPORT jobject JNICALL
    Java_nitf_ImageSubheader_getNumMultispectralImageBands(JNIEnv * env,
                                                           jobject self)
{
    nitf_ImageSubheader *header = _GetObj(env, self);
    return _GetFieldObj(env, header->numMultispectralImageBands);
}


/*
 * Class:     nitf_ImageSubheader
 * Method:    getNumPixelsPerHorizBlock
 * Signature: ()Lnitf/Field;
 */
JNIEXPORT jobject JNICALL
    Java_nitf_ImageSubheader_getNumPixelsPerHorizBlock(JNIEnv * env,
                                                       jobject self)
{
    nitf_ImageSubheader *header = _GetObj(env, self);
    return _GetFieldObj(env, header->numPixelsPerHorizBlock);
}


/*
 * Class:     nitf_ImageSubheader
 * Method:    getNumPixelsPerVertBlock
 * Signature: ()Lnitf/Field;
 */
JNIEXPORT jobject JNICALL Java_nitf_ImageSubheader_getNumPixelsPerVertBlock
    (JNIEnv * env, jobject self)
{
    nitf_ImageSubheader *header = _GetObj(env, self);
    return _GetFieldObj(env, header->numPixelsPerVertBlock);
}


/*
 * Class:     nitf_ImageSubheader
 * Method:    getNumRows
 * Signature: ()Lnitf/Field;
 */
JNIEXPORT jobject JNICALL Java_nitf_ImageSubheader_getNumRows
    (JNIEnv * env, jobject self)
{
    nitf_ImageSubheader *header = _GetObj(env, self);
    return _GetFieldObj(env, header->numRows);
}


/*
 * Class:     nitf_ImageSubheader
 * Method:    getPixelJustification
 * Signature: ()Lnitf/Field;
 */
JNIEXPORT jobject JNICALL Java_nitf_ImageSubheader_getPixelJustification
    (JNIEnv * env, jobject self)
{
    nitf_ImageSubheader *header = _GetObj(env, self);
    return _GetFieldObj(env, header->pixelJustification);
}


/*
 * Class:     nitf_ImageSubheader
 * Method:    getPixelValueType
 * Signature: ()Lnitf/Field;
 */
JNIEXPORT jobject JNICALL Java_nitf_ImageSubheader_getPixelValueType
    (JNIEnv * env, jobject self)
{
    nitf_ImageSubheader *header = _GetObj(env, self);
    return _GetFieldObj(env, header->pixelValueType);
}


/*
 * Class:     nitf_ImageSubheader
 * Method:    getSecurityGroup
 * Signature: ()Lnitf/FileSecurity;
 */
JNIEXPORT jobject JNICALL Java_nitf_ImageSubheader_getSecurityGroup
    (JNIEnv * env, jobject self)
{
    nitf_ImageSubheader *header = _GetObj(env, self);
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
 * Class:     nitf_ImageSubheader
 * Method:    getTargetId
 * Signature: ()Lnitf/Field;
 */
JNIEXPORT jobject JNICALL Java_nitf_ImageSubheader_getTargetId
    (JNIEnv * env, jobject self)
{
    nitf_ImageSubheader *header = _GetObj(env, self);
    return _GetFieldObj(env, header->targetId);
}


/*
 * Class:     nitf_ImageSubheader
 * Method:    getUserDefinedImageDataLength
 * Signature: ()Lnitf/Field;
 */
JNIEXPORT jobject JNICALL
    Java_nitf_ImageSubheader_getUserDefinedImageDataLength(JNIEnv * env,
                                                           jobject self)
{
    nitf_ImageSubheader *header = _GetObj(env, self);
    return _GetFieldObj(env, header->userDefinedImageDataLength);
}


/*
 * Class:     nitf_ImageSubheader
 * Method:    getUserDefinedOverflow
 * Signature: ()Lnitf/Field;
 */
JNIEXPORT jobject JNICALL Java_nitf_ImageSubheader_getUserDefinedOverflow
    (JNIEnv * env, jobject self)
{
    nitf_ImageSubheader *header = _GetObj(env, self);
    return _GetFieldObj(env, header->userDefinedOverflow);
}


/*
 * Class:     nitf_ImageSubheader
 * Method:    getUserDefinedSection
 * Signature: ()Lnitf/Extensions;
 */
JNIEXPORT jobject JNICALL Java_nitf_ImageSubheader_getUserDefinedSection
    (JNIEnv * env, jobject self)
{
    nitf_ImageSubheader *header = _GetObj(env, self);
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
 * Class:     nitf_ImageSubheader
 * Method:    getExtendedSection
 * Signature: ()Lnitf/Extensions;
 */
JNIEXPORT jobject JNICALL Java_nitf_ImageSubheader_getExtendedSection
    (JNIEnv * env, jobject self)
{
    nitf_ImageSubheader *header = _GetObj(env, self);

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


/*
 * Class:     nitf_ImageSubheader
 * Method:    insertImageComment
 * Signature: (Ljava/lang/String;I)I
 */
JNIEXPORT jint JNICALL Java_nitf_ImageSubheader_insertImageComment
    (JNIEnv * env, jobject self, jstring comment, jint index)
{
    nitf_ImageSubheader *header = _GetObj(env, self);
    nitf_Error error;
    const jbyte *comBuf = NULL;
    jint indexUsed;

    if (comment != NULL)
        comBuf = (*env)->GetStringUTFChars(env, comment, 0);

    indexUsed = nitf_ImageSubheader_insertImageComment(
            header, comBuf, index, &error);
    
    if (indexUsed < 0)
    {
        _ThrowNITFException(env, error.message);
    }
    
    if (comment != NULL)
        (*env)->ReleaseStringUTFChars(env, comment, comBuf);
    
    return indexUsed;
}


/*
 * Class:     nitf_ImageSubheader
 * Method:    removeImageComment
 * Signature: (I)Z
 */
JNIEXPORT jboolean JNICALL Java_nitf_ImageSubheader_removeImageComment
    (JNIEnv * env, jobject self, jint index)
{
    nitf_ImageSubheader *header = _GetObj(env, self);
    nitf_Error error;

    if (!nitf_ImageSubheader_removeImageComment(header, index, &error))
    {
        _ThrowNITFException(env, error.message);
        return JNI_FALSE;
    }
    return JNI_TRUE;
}


/*
 * Class:     nitf_ImageSubheader
 * Method:    getBandCount
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_nitf_ImageSubheader_getBandCount
    (JNIEnv * env, jobject self)
{
    nitf_ImageSubheader *header = _GetObj(env, self);
    nitf_Error error;
    jint bandCount;

    bandCount = (jint) nitf_ImageSubheader_getBandCount(header, &error);
    bandCount = bandCount < 0 ? 0 : bandCount;
    return bandCount;
}


/*
 * Class:     nitf_ImageSubheader
 * Method:    createBands
 * Signature: (I)Z
 */
JNIEXPORT jboolean JNICALL Java_nitf_ImageSubheader_createBands
    (JNIEnv * env, jobject self, jint numBands)
{
    nitf_ImageSubheader *header = _GetObj(env, self);
    nitf_Error error;

    if (!nitf_ImageSubheader_createBands
        (header, (uint32_t) numBands, &error))
    {
        _ThrowNITFException(env, error.message);
        return JNI_FALSE;
    }
    return JNI_TRUE;
}

/*
 * Class:     nitf_ImageSubheader
 * Method:    removeBand
 * Signature: (I)V
 */
JNIEXPORT void JNICALL Java_nitf_ImageSubheader_removeBand
  (JNIEnv *env, jobject self, jint index)
{
    nitf_ImageSubheader *header = _GetObj(env, self);
    nitf_Error error;

    if (!nitf_ImageSubheader_removeBand(header, (uint32_t) index, &error))
    {
        _ThrowNITFException(env, error.message);
    }
}
