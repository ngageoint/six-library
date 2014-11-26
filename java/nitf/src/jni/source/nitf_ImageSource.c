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
#include "nitf_ImageSource.h"
#include "nitf_ImageSource_Destructor.h"
#include "nitf_JNI.h"

/*  This creates the _SetObj and _GetObj accessors  */
NITF_JNI_DECLARE_OBJ(nitf_ImageSource)
/*
 * Class:     nitf_ImageSource
 * Method:    construct
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_nitf_ImageSource_construct
    (JNIEnv * env, jobject self)
{
    nitf_ImageSource *source;
    nitf_Error error;

    source = nitf_ImageSource_construct(&error);
    _SetObj(env, self, source);
}

JNIEXPORT jboolean JNICALL Java_nitf_ImageSource_00024Destructor_destructMemory
    (JNIEnv * env, jobject self, jlong address)
{
    nitf_ImageSource *imageSource = (nitf_ImageSource*)address;
    if (imageSource)
    {
        nitf_ImageSource_destruct(&imageSource);
        return JNI_TRUE;
    }
    return JNI_FALSE;
}


/*
 * Class:     nitf_ImageSource
 * Method:    getBandSources
 * Signature: ()[Lnitf/BandSource;
 */
JNIEXPORT jobjectArray JNICALL Java_nitf_ImageSource_getBandSources
    (JNIEnv * env, jobject self)
{
    nitf_ImageSource *imageSource = _GetObj(env, self);

    jclass bandSourceClass = (*env)->FindClass(env, "nitf/BandSource");
    nitf_Error error;
    jobjectArray sources;
    jobject element;
    jint index = 0;
    jint num, actualNum = 0;
    jmethodID methodID;
    nitf_ListIterator iter, end;
    jlong address;      /* temp address */

    num = imageSource->size;

    /* iterate over the bandsources to get the correct count */
    iter = nitf_List_begin(imageSource->bandSources);
    end = nitf_List_end(imageSource->bandSources);
    while (nitf_ListIterator_notEqualTo(&iter, &end))
    {
        actualNum++;
        nitf_ListIterator_increment(&iter);
    }

    /* check to see if the size member is WRONG */
    if (num != actualNum)
    {
        num = actualNum;
        /* set the size member to the RIGHT value */
        imageSource->size = num;
    }

    sources = (*env)->NewObjectArray(env, num, bandSourceClass, NULL);
    methodID = (*env)->GetStaticMethodID(env, bandSourceClass, "getByAddress",
        "(J)Lnitf/BandSource;");

    index = 0;
    /* Now, iterate over the bandsources to create an array */
    iter = nitf_List_begin(imageSource->bandSources);
    end = nitf_List_end(imageSource->bandSources);
    while (nitf_ListIterator_notEqualTo(&iter, &end))
    {
        address = (jlong)((nitf_BandSource *)nitf_ListIterator_get(&iter));
        element = (*env)->CallStaticObjectMethod(env, bandSourceClass,
                methodID, address);
        (*env)->SetObjectArrayElement(env, sources, index++, element);
        nitf_ListIterator_increment(&iter);
    }
    return sources;

  CATCH_ERROR:
    _ThrowNITFException(env, error.message);
    return NULL;
}


/*
 * Class:     nitf_ImageSource
 * Method:    getSize
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_nitf_ImageSource_getSize
    (JNIEnv * env, jobject self)
{
    nitf_ImageSource *imageSource = _GetObj(env, self);
    jint num, actualNum = 0;
    nitf_ListIterator iter, end;

    num = imageSource->size;

    /* iterate over the bandsources to get the correct count */
    iter = nitf_List_begin(imageSource->bandSources);
    end = nitf_List_end(imageSource->bandSources);
    while (nitf_ListIterator_notEqualTo(&iter, &end))
    {
        actualNum++;
        nitf_ListIterator_increment(&iter);
    }

    /* check to see if the size member is WRONG */
    if (num != actualNum)
    {
        /* set the size member to the RIGHT value */
        imageSource->size = num;
    }

    return imageSource->size;
}


/*
 * Class:     nitf_ImageSource
 * Method:    addBand
 * Signature: (Lnitf/BandSource;)Z
 */
JNIEXPORT jboolean JNICALL Java_nitf_ImageSource_addBand
    (JNIEnv * env, jobject self, jobject jBandSource)
{
    nitf_ImageSource *source = _GetObj(env, self);
    nitf_BandSource *bandSource;
    nitf_Error error;
    jclass bandSourceClass = (*env)->FindClass(env, "nitf/BandSource");
    jmethodID methodID =
        (*env)->GetMethodID(env, bandSourceClass, "getAddress", "()J");

    bandSource = (nitf_BandSource *) (*env)->CallLongMethod(
            env, jBandSource, methodID);

    if (!bandSource)
    {
        return JNI_FALSE;
    }

    if (!nitf_ImageSource_addBand(source, bandSource, &error))
    {
        _ThrowNITFException(env, error.message);
        return JNI_FALSE;
    }

    /* Now, tell Java not to manage it anymore */
    _ManageObject(env, (jlong)bandSource, JNI_FALSE);

    return JNI_TRUE;
}


/*
 * Class:     nitf_ImageSource
 * Method:    getBand
 * Signature: (I)Lnitf/BandSource;
 */
JNIEXPORT jobject JNICALL Java_nitf_ImageSource_getBand
    (JNIEnv * env, jobject self, jint number)
{
    nitf_ImageSource *source = _GetObj(env, self);
    nitf_BandSource *bandSource;
    jobject bandSourceObject;
    nitf_Error error;
    jclass bandSourceClass = (*env)->FindClass(env, "nitf/BandSource");
    jmethodID methodID =
        (*env)->GetMethodID(env, bandSourceClass, "<init>", "(J)V");

    bandSource = nitf_ImageSource_getBand(source, number, &error);

    if (!bandSource)
    {
        _ThrowNITFException(env, error.message);
        return NULL;
    }

    /* make the object */
    bandSourceObject = (*env)->NewObject(env,
                                         bandSourceClass,
                                         methodID, (jlong) bandSource);
    return bandSourceObject;
}

