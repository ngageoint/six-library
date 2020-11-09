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
#include "nitf_Record.h"
#include "nitf_Record_Destructor.h"
#include "nitf_JNI.h"

NITF_JNI_DECLARE_OBJ(nitf_Record)
/*
 * Class:     nitf_Record
 * Method:    construct
 * Signature: (Lnitf/Version;)V
 */
JNIEXPORT void JNICALL Java_nitf_Record_construct
    (JNIEnv * env, jobject self, jobject version)
{
    nitf_Record *record;
    nitf_Error error;

    nitf_Version nitfVersion = _GetNITFVersion(env, version);

    record = nitf_Record_construct(nitfVersion, &error);
    if (!record)
    {
        /* throw an error */
        _ThrowNITFException(env, error.message);
    }
    _SetObj(env, self, record);
}



JNIEXPORT jboolean JNICALL Java_nitf_Record_00024Destructor_destructMemory
    (JNIEnv * env, jobject self, jlong address)
{
    nitf_Record *record = (nitf_Record*)address;
    if (record)
    {
        nitf_Record_destruct(&record);
        return JNI_TRUE;
    }
    return JNI_FALSE;
}


/*
 * Class:     nitf_Record
 * Method:    makeClone
 * Signature: ()Lnitf/Cloneable;
 */
JNIEXPORT jobject JNICALL Java_nitf_Record_makeClone
    (JNIEnv * env, jobject self)
{
    nitf_Record *record = _GetObj(env, self);
    nitf_Record *clone;
    jobject cloneObject;
    jclass recordClass = (*env)->FindClass(env, "nitf/Record");
    jmethodID methodID =
        (*env)->GetMethodID(env, recordClass, "<init>", "(J)V");
    nitf_Error error;

    /* clone it */
    clone = nitf_Record_clone(record, &error);

    if (!clone)
    {
        _ThrowNITFException(env, error.message);
        return NULL;
    }

    /* create the object */
    cloneObject = (*env)->NewObject(env,
                                    recordClass, methodID, (jlong) clone);

    /* set the clone function */
    methodID = (*env)->GetMethodID(env, recordClass, "setClone", "(Z)V");
    (*env)->CallVoidMethod(env, cloneObject, methodID, JNI_TRUE);

    return cloneObject;
}


/*
 * Class:     nitf_Record
 * Method:    getHeader
 * Signature: ()Lnitf/FileHeader;
 */
JNIEXPORT jobject JNICALL Java_nitf_Record_getHeader
    (JNIEnv * env, jobject self)
{

    nitf_Record *record = _GetObj(env, self);
    jclass fileHeaderClass = (*env)->FindClass(env, "nitf/FileHeader");

    jmethodID methodID =
        (*env)->GetMethodID(env, fileHeaderClass, "<init>", "(J)V");
    jobject header = (*env)->NewObject(env,
                                       fileHeaderClass,
                                       methodID,
                                       (jlong) record->header);
    return header;
}


/*
 * Class:     nitf_Record
 * Method:    getImages
 * Signature: ()[Lnitf/ImageSegment;
 */
JNIEXPORT jobjectArray JNICALL Java_nitf_Record_getImages
    (JNIEnv * env, jobject self)
{
    nitf_Record *record = _GetObj(env, self);
    jclass segmentClass = (*env)->FindClass(env, "nitf/ImageSegment");

    jobjectArray images;
    jobject segment;
    jint count = 0;
    nitf_ImageSegment *imageSegment;
    nitf_ListIterator iter, end;

    /* get the initial count */
    iter = nitf_List_begin(record->images);
    end = nitf_List_end(record->images);
    while (nitf_ListIterator_notEqualTo(&iter, &end))
    {
        count++;
        nitf_ListIterator_increment(&iter);
    }

    /* make the array */
    images = (*env)->NewObjectArray(env, count, segmentClass, NULL);

    iter = nitf_List_begin(record->images);
    end = nitf_List_end(record->images);
    count = 0;
    while (nitf_ListIterator_notEqualTo(&iter, &end))
    {
        jmethodID methodID =
            (*env)->GetMethodID(env, segmentClass, "<init>", "(J)V");
        imageSegment = (nitf_ImageSegment *) nitf_ListIterator_get(&iter);

        segment = (*env)->NewObject(env,
                                    segmentClass,
                                    methodID, (jlong) imageSegment);
        (*env)->SetObjectArrayElement(env, images, count, segment);
        count++;

        nitf_ListIterator_increment(&iter);
    }
    return images;
}


/*
 * Class:     nitf_Record
 * Method:    getDataExtensions
 * Signature: ()[Lnitf/DESegment;
 */
JNIEXPORT jobjectArray JNICALL Java_nitf_Record_getDataExtensions
    (JNIEnv * env, jobject self)
{
    nitf_Record *record = _GetObj(env, self);
    jclass segmentClass = (*env)->FindClass(env, "nitf/DESegment");

    jobjectArray des;
    jobject segment;
    jint count = 0;
    nitf_DESegment *deSegment;
    nitf_ListIterator iter, end;

    /* get the initial count */
    iter = nitf_List_begin(record->dataExtensions);
    end = nitf_List_end(record->dataExtensions);
    while (nitf_ListIterator_notEqualTo(&iter, &end))
    {
        count++;
        nitf_ListIterator_increment(&iter);
    }

    /* make the array */
    des = (*env)->NewObjectArray(env, count, segmentClass, NULL);

    iter = nitf_List_begin(record->dataExtensions);
    end = nitf_List_end(record->dataExtensions);
    count = 0;
    while (nitf_ListIterator_notEqualTo(&iter, &end))
    {
        jmethodID methodID =
            (*env)->GetMethodID(env, segmentClass, "<init>", "(J)V");
        /*  Get the image segment as its proper object  */
        deSegment = (nitf_DESegment *) nitf_ListIterator_get(&iter);

        segment = (*env)->NewObject(env,
                                    segmentClass,
                                    methodID, (jlong) deSegment);
        (*env)->SetObjectArrayElement(env, des, count, segment);
        count++;

        /*  Increment the iterator so we can continue  */
        nitf_ListIterator_increment(&iter);
    }
    return des;
}


/*
 * Class:     nitf_Record
 * Method:    getGraphics
 * Signature: ()[Lnitf/GraphicSegment;
 */
JNIEXPORT jobjectArray JNICALL Java_nitf_Record_getGraphics
    (JNIEnv * env, jobject self)
{
    nitf_Record *record = _GetObj(env, self);

    jclass segmentClass = (*env)->FindClass(env, "nitf/GraphicSegment");

    jobjectArray graphics;
    jobject segment;
    jint count = 0;
    nitf_GraphicSegment *graphicSegment;
    nitf_ListIterator iter, end;

    /* get the initial count */
    iter = nitf_List_begin(record->graphics);
    end = nitf_List_end(record->graphics);
    while (nitf_ListIterator_notEqualTo(&iter, &end))
    {
        count++;
        nitf_ListIterator_increment(&iter);
    }

    /* make the array */
    graphics = (*env)->NewObjectArray(env, count, segmentClass, NULL);

    iter = nitf_List_begin(record->graphics);
    end = nitf_List_end(record->graphics);
    count = 0;
    while (nitf_ListIterator_notEqualTo(&iter, &end))
    {
        jmethodID methodID =
            (*env)->GetMethodID(env, segmentClass, "<init>", "(J)V");
        /*  Get the image segment as its proper object  */
        graphicSegment =
            (nitf_GraphicSegment *) nitf_ListIterator_get(&iter);

        segment = (*env)->NewObject(env,
                                    segmentClass,
                                    methodID, (jlong) graphicSegment);
        (*env)->SetObjectArrayElement(env, graphics, count, segment);
        count++;

        /*  Increment the iterator so we can continue  */
        nitf_ListIterator_increment(&iter);
    }
    return graphics;
}


/*
 * Class:     nitf_Record
 * Method:    getLabels
 * Signature: ()[Lnitf/LabelSegment;
 */
JNIEXPORT jobjectArray JNICALL Java_nitf_Record_getLabels
    (JNIEnv * env, jobject self)
{
    nitf_Record *record = _GetObj(env, self);

    jclass segmentClass = (*env)->FindClass(env, "nitf/LabelSegment");

    jobjectArray labels;
    jobject segment;
    jint count = 0;
    nitf_LabelSegment *labelSegment;
    nitf_ListIterator iter, end;

    /* get the initial count */
    iter = nitf_List_begin(record->labels);
    end = nitf_List_end(record->labels);
    while (nitf_ListIterator_notEqualTo(&iter, &end))
    {
        count++;
        nitf_ListIterator_increment(&iter);
    }

    /* make the array */
    labels = (*env)->NewObjectArray(env, count, segmentClass, NULL);

    iter = nitf_List_begin(record->labels);
    end = nitf_List_end(record->labels);
    count = 0;
    while (nitf_ListIterator_notEqualTo(&iter, &end))
    {
        jmethodID methodID =
            (*env)->GetMethodID(env, segmentClass, "<init>", "(J)V");
        /*  Get the image segment as its proper object  */
        labelSegment = (nitf_LabelSegment *) nitf_ListIterator_get(&iter);

        segment = (*env)->NewObject(env,
                                    segmentClass,
                                    methodID, (jlong) labelSegment);
        (*env)->SetObjectArrayElement(env, labels, count, segment);
        count++;

        /*  Increment the iterator so we can continue  */
        nitf_ListIterator_increment(&iter);
    }
    return labels;
}


/*
 * Class:     nitf_Record
 * Method:    getTexts
 * Signature: ()[Lnitf/TextSegment;
 */
JNIEXPORT jobjectArray JNICALL Java_nitf_Record_getTexts
    (JNIEnv * env, jobject self)
{
    nitf_Record *record = _GetObj(env, self);
    jclass segmentClass = (*env)->FindClass(env, "nitf/TextSegment");

    jobjectArray texts;
    jobject segment;
    jint count = 0;
    nitf_TextSegment *textSegment;
    nitf_ListIterator iter, end;

    /* get the initial count */
    iter = nitf_List_begin(record->texts);
    end = nitf_List_end(record->texts);
    while (nitf_ListIterator_notEqualTo(&iter, &end))
    {
        count++;
        nitf_ListIterator_increment(&iter);
    }

    /* make the array */
    texts = (*env)->NewObjectArray(env, count, segmentClass, NULL);

    iter = nitf_List_begin(record->texts);
    end = nitf_List_end(record->texts);
    count = 0;
    while (nitf_ListIterator_notEqualTo(&iter, &end))
    {
        jmethodID methodID =
            (*env)->GetMethodID(env, segmentClass, "<init>", "(J)V");
        /*  Get the image segment as its proper object  */
        textSegment = (nitf_TextSegment *) nitf_ListIterator_get(&iter);

        segment = (*env)->NewObject(env,
                                    segmentClass,
                                    methodID, (jlong) textSegment);
        (*env)->SetObjectArrayElement(env, texts, count, segment);
        count++;

        /*  Increment the iterator so we can continue  */
        nitf_ListIterator_increment(&iter);
    }
    return texts;
}


/*
 * Class:     nitf_Record
 * Method:    getReservedExtensions
 * Signature: ()[Lnitf/RESegment;
 */
JNIEXPORT jobjectArray JNICALL Java_nitf_Record_getReservedExtensions
    (JNIEnv * env, jobject self)
{
    nitf_Record *record = _GetObj(env, self);
    jclass segmentClass = (*env)->FindClass(env, "nitf/RESegment");

    jobjectArray res;
    jobject segment;
    jint count = 0;
    nitf_TextSegment *reSegment;
    nitf_ListIterator iter, end;

    /* get the initial count */
    iter = nitf_List_begin(record->reservedExtensions);
    end = nitf_List_end(record->reservedExtensions);
    while (nitf_ListIterator_notEqualTo(&iter, &end))
    {
        count++;
        nitf_ListIterator_increment(&iter);
    }

    /* make the array */
    res = (*env)->NewObjectArray(env, count, segmentClass, NULL);

    iter = nitf_List_begin(record->reservedExtensions);
    end = nitf_List_end(record->reservedExtensions);
    count = 0;
    while (nitf_ListIterator_notEqualTo(&iter, &end))
    {
        jmethodID methodID =
            (*env)->GetMethodID(env, segmentClass, "<init>", "(J)V");
        /*  Get the image segment as its proper object  */
        reSegment = (nitf_TextSegment *) nitf_ListIterator_get(&iter);

        segment = (*env)->NewObject(env,
                                    segmentClass,
                                    methodID, (jlong) reSegment);
        (*env)->SetObjectArrayElement(env, res, count, segment);
        count++;

        /*  Increment the iterator so we can continue  */
        nitf_ListIterator_increment(&iter);
    }
    return res;
}


/*
 * Class:     nitf_Record
 * Method:    newImageSegment
 * Signature: ()Lnitf/ImageSegment;
 */
JNIEXPORT jobject JNICALL Java_nitf_Record_newImageSegment
    (JNIEnv * env, jobject self)
{
    nitf_Record *record = _GetObj(env, self);
    nitf_ImageSegment *segment;
    nitf_Error error;

    jmethodID methodID;
    jobject segmentObject;
    jclass segmentClass = (*env)->FindClass(env, "nitf/ImageSegment");

    segment = nitf_Record_newImageSegment(record, &error);
    if (!segment)
        goto CATCH_ERROR;

    /* return it */
    methodID = (*env)->GetMethodID(env, segmentClass, "<init>", "(J)V");
    segmentObject = (*env)->NewObject(env,
                                      segmentClass,
                                      methodID, (jlong) segment);
    return segmentObject;

  CATCH_ERROR:
    _ThrowNITFException(env, error.message);
    return NULL;
}


/*
 * Class:     nitf_Record
 * Method:    newGraphicSegment
 * Signature: ()Lnitf/GraphicSegment;
 */
JNIEXPORT jobject JNICALL Java_nitf_Record_newGraphicSegment
    (JNIEnv * env, jobject self)
{
    nitf_Record *record = _GetObj(env, self);
    nitf_GraphicSegment *segment;
    nitf_Error error;

    jmethodID methodID;
    jobject segmentObject;
    jclass segmentClass = (*env)->FindClass(env, "nitf/GraphicSegment");

    segment = nitf_Record_newGraphicSegment(record, &error);
    if (!segment)
        goto CATCH_ERROR;

    /* return it */
    methodID = (*env)->GetMethodID(env, segmentClass, "<init>", "(J)V");
    segmentObject = (*env)->NewObject(env,
                                      segmentClass,
                                      methodID, (jlong) segment);
    return segmentObject;

  CATCH_ERROR:
    _ThrowNITFException(env, error.message);
    return NULL;
}


/*
 * Class:     nitf_Record
 * Method:    newTextSegment
 * Signature: ()Lnitf/TextSegment;
 */
JNIEXPORT jobject JNICALL Java_nitf_Record_newTextSegment
    (JNIEnv * env, jobject self)
{
    nitf_Record *record = _GetObj(env, self);
    nitf_TextSegment *segment;
    nitf_Error error;

    jmethodID methodID;
    jobject segmentObject;
    jclass segmentClass = (*env)->FindClass(env, "nitf/TextSegment");

    segment = nitf_Record_newTextSegment(record, &error);
    if (!segment)
        goto CATCH_ERROR;

    /* return it */
    methodID = (*env)->GetMethodID(env, segmentClass, "<init>", "(J)V");
    segmentObject = (*env)->NewObject(env,
                                      segmentClass,
                                      methodID, (jlong) segment);
    return segmentObject;

  CATCH_ERROR:
    _ThrowNITFException(env, error.message);
    return NULL;
}

JNIEXPORT jobject JNICALL Java_nitf_Record_newDESegment
  (JNIEnv *env, jobject self)
{
    nitf_Record *record = _GetObj(env, self);
    nitf_DESegment *segment;
    nitf_Error error;

    jmethodID methodID;
    jobject segmentObject;
    jclass segmentClass = (*env)->FindClass(env, "nitf/DESegment");

    segment = nitf_Record_newDataExtensionSegment(record, &error);
    if (!segment)
        goto CATCH_ERROR;

    /* return it */
    methodID = (*env)->GetMethodID(env, segmentClass, "<init>", "(J)V");
    segmentObject = (*env)->NewObject(env,
                                      segmentClass,
                                      methodID, (jlong) segment);
    return segmentObject;

  CATCH_ERROR:
    _ThrowNITFException(env, error.message);
    return NULL;
}


/*
 * Class:     nitf_Record
 * Method:    getVersion
 * Signature: ()Lnitf/NITFVersion;
 */
JNIEXPORT jobject JNICALL Java_nitf_Record_getVersion
    (JNIEnv * env, jobject self)
{
    nitf_Record *record = _GetObj(env, self);
    jclass versionClass = (*env)->FindClass(env, "nitf/Version");
    jfieldID fieldID;
    nitf_Version version;

    version = nitf_Record_getVersion(record);
    switch (version)
    {
    case NITF_VER_20:
        fieldID =
            (*env)->GetStaticFieldID(env, versionClass, "NITF_20",
                                     "Lnitf/Version;");
        break;
    case NITF_VER_21:
        fieldID =
            (*env)->GetStaticFieldID(env, versionClass, "NITF_21",
                                     "Lnitf/Version;");
        break;
    default:
        fieldID =
            (*env)->GetStaticFieldID(env, versionClass, "UNKNOWN",
                                     "Lnitf/Version;");
        break;
    }

    return (*env)->GetStaticObjectField(env, versionClass, fieldID);
}


/*
 * Class:     nitf_Record
 * Method:    removeImageSegment
 * Signature: (I)V
 */
JNIEXPORT void JNICALL Java_nitf_Record_removeImageSegment
  (JNIEnv *env, jobject self, jint segmentNumber)
{
	nitf_Record *record = _GetObj(env, self);
	nitf_Error error;

	if (!nitf_Record_removeImageSegment(record, (uint32_t)segmentNumber, &error))
	{
		_ThrowNITFException(env, error.message);
	}
	return;
}

/*
 * Class:     nitf_Record
 * Method:    removeGraphicSegment
 * Signature: (I)V
 */
JNIEXPORT void JNICALL Java_nitf_Record_removeGraphicSegment
  (JNIEnv *env, jobject self, jint segmentNumber)
{
	nitf_Record *record = _GetObj(env, self);
	nitf_Error error;

	if (!nitf_Record_removeGraphicSegment(record, (uint32_t)segmentNumber, &error))
	{
		_ThrowNITFException(env, error.message);
	}
	return;
}

/*
 * Class:     nitf_Record
 * Method:    removeLabelSegment
 * Signature: (I)V
 */
JNIEXPORT void JNICALL Java_nitf_Record_removeLabelSegment
  (JNIEnv *env, jobject self, jint segmentNumber)
{
	nitf_Record *record = _GetObj(env, self);
	nitf_Error error;

	if (!nitf_Record_removeLabelSegment(record, (uint32_t)segmentNumber, &error))
	{
		_ThrowNITFException(env, error.message);
	}
	return;
}

/*
 * Class:     nitf_Record
 * Method:    removeTextSegment
 * Signature: (I)V
 */
JNIEXPORT void JNICALL Java_nitf_Record_removeTextSegment
  (JNIEnv *env, jobject self, jint segmentNumber)
{
	nitf_Record *record = _GetObj(env, self);
	nitf_Error error;

	if (!nitf_Record_removeTextSegment(record, (uint32_t)segmentNumber, &error))
	{
		_ThrowNITFException(env, error.message);
	}
	return;
}

/*
 * Class:     nitf_Record
 * Method:    removeDataExtensionSegment
 * Signature: (I)V
 */
JNIEXPORT void JNICALL Java_nitf_Record_removeDataExtensionSegment
  (JNIEnv *env, jobject self, jint segmentNumber)
{
	nitf_Record *record = _GetObj(env, self);
	nitf_Error error;

	if (!nitf_Record_removeDataExtensionSegment(record, (uint32_t)segmentNumber, &error))
	{
		_ThrowNITFException(env, error.message);
	}
	return;
}

/*
 * Class:     nitf_Record
 * Method:    removeReservedExtensionSegment
 * Signature: (I)V
 */
JNIEXPORT void JNICALL Java_nitf_Record_removeReservedExtensionSegment
  (JNIEnv *env, jobject self, jint segmentNumber)
{
	nitf_Record *record = _GetObj(env, self);
	nitf_Error error;

	if (!nitf_Record_removeReservedExtensionSegment(record, (uint32_t)segmentNumber, &error))
	{
		_ThrowNITFException(env, error.message);
	}
	return;
}

JNIEXPORT void JNICALL Java_nitf_Record_moveImageSegment
  (JNIEnv *env, jobject self, jint oldIndex, jint newIndex)
{
    nitf_Record *record = _GetObj(env, self);
    nitf_Error error;

    if (!nitf_Record_moveImageSegment(record,
            (uint32_t)oldIndex, (uint32_t)newIndex, &error))
    {
        _ThrowNITFException(env, error.message);
    }
    return;
}

JNIEXPORT void JNICALL Java_nitf_Record_moveGraphicSegment
(JNIEnv *env, jobject self, jint oldIndex, jint newIndex)
{
    nitf_Record *record = _GetObj(env, self);
    nitf_Error error;

    if (!nitf_Record_moveGraphicSegment(record,
                    (uint32_t)oldIndex, (uint32_t)newIndex, &error))
    {
        _ThrowNITFException(env, error.message);
    }
    return;
}

JNIEXPORT void JNICALL Java_nitf_Record_moveTextSegment
(JNIEnv *env, jobject self, jint oldIndex, jint newIndex)
{
    nitf_Record *record = _GetObj(env, self);
    nitf_Error error;

    if (!nitf_Record_moveTextSegment(record,
                    (uint32_t)oldIndex, (uint32_t)newIndex, &error))
    {
        _ThrowNITFException(env, error.message);
    }
    return;
}

JNIEXPORT void JNICALL Java_nitf_Record_moveLabelSegment
(JNIEnv *env, jobject self, jint oldIndex, jint newIndex)
{
    nitf_Record *record = _GetObj(env, self);
    nitf_Error error;

    if (!nitf_Record_moveLabelSegment(record,
                    (uint32_t)oldIndex, (uint32_t)newIndex, &error))
    {
        _ThrowNITFException(env, error.message);
    }
    return;
}

JNIEXPORT void JNICALL Java_nitf_Record_moveDataExtensionSegment
(JNIEnv *env, jobject self, jint oldIndex, jint newIndex)
{
    nitf_Record *record = _GetObj(env, self);
    nitf_Error error;

    if (!nitf_Record_moveDataExtensionSegment(record,
                    (uint32_t)oldIndex, (uint32_t)newIndex, &error))
    {
        _ThrowNITFException(env, error.message);
    }
    return;
}

JNIEXPORT void JNICALL Java_nitf_Record_moveReservedExtensionSegment
(JNIEnv *env, jobject self, jint oldIndex, jint newIndex)
{
    nitf_Record *record = _GetObj(env, self);
    nitf_Error error;

    if (!nitf_Record_moveReservedExtensionSegment(record,
                    (uint32_t)oldIndex, (uint32_t)newIndex, &error))
    {
        _ThrowNITFException(env, error.message);
    }
    return;
}

JNIEXPORT void JNICALL Java_nitf_Record_mergeTREs
  (JNIEnv *env, jobject self)
{
	nitf_Record *record = _GetObj(env, self);
    nitf_Error error;

    if (!nitf_Record_mergeTREs(record, &error))
    {
        _ThrowNITFException(env, error.message);
    }
    return;
}

JNIEXPORT void JNICALL Java_nitf_Record_unmergeTREs
  (JNIEnv *env, jobject self)
{
	nitf_Record *record = _GetObj(env, self);
    nitf_Error error;

    if (!nitf_Record_unmergeTREs(record, &error))
    {
        _ThrowNITFException(env, error.message);
    }
    return;
}
