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
#include "nitf_Reader.h"
#include "nitf_Reader_Destructor.h"
#include "nitf_JNI.h"

NITF_JNI_DECLARE_OBJ( nitf_Reader)
/*
 * Class:     nitf_Reader
 * Method:    construct
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_nitf_Reader_construct
(JNIEnv * env, jobject self)
{
    nitf_Reader *reader;
    nitf_Error error;

    reader = nitf_Reader_construct(&error);
    _SetObj(env, self, reader);
}


JNIEXPORT jboolean JNICALL Java_nitf_Reader_00024Destructor_destructMemory
    (JNIEnv * env, jobject self, jlong address)
{
    nitf_Reader *reader = (nitf_Reader*)address;
    if (reader)
    {
        /* release our hold on the record/io */
        if (reader->record)
        {
            _ManageObject(env, (jlong)reader->record, JNI_TRUE);
        }
        if (reader->input && !reader->ownInput)
        {
            _ManageObject(env, (jlong)reader->input, JNI_TRUE);
        }
        nitf_Reader_destruct(&reader);
        return JNI_TRUE;
    }
    return JNI_FALSE;
}

/*
 * Class:     nitf_Reader
 * Method:    read
 * Signature: (Lnitf/IOInterface;)Lnitf/Record;
 */
JNIEXPORT jobject JNICALL Java_nitf_Reader_read(JNIEnv * env, jobject self,
                                                jobject interface)
{
    nitf_Reader *reader = _GetObj(env, self);
    nitf_Error error;
    jobject record;
    nitf_IOInterface* io;
    jclass recordClass, inputClass;
    jmethodID methodID;

    /* release our hold on a pre-existing record/io */
    if (reader->record)
    {
        _ManageObject(env, (jlong)reader->record, JNI_TRUE);
        reader->record = NULL;
    }
    if (reader->input && !reader->ownInput)
    {
        _ManageObject(env, (jlong)reader->input, JNI_TRUE);
        reader->input = NULL;
    }

    /* get some classIDs */
    inputClass = (*env)->GetObjectClass(env, interface);
    recordClass = (*env)->FindClass(env, "nitf/Record");

    methodID = (*env)->GetMethodID(env, inputClass, "getAddress", "()J");
    io = (nitf_IOInterface *) (*env)->CallLongMethod(env, interface, methodID);
    /* mark the io as being safe from Java GC destruction */
    _ManageObject(env, (jlong)io, JNI_FALSE);

    if (!nitf_Reader_readIO(reader, io, &error))
        goto CATCH_ERROR;

    record = _NewObject(env, (jlong)reader->record, "nitf/Record");
    /* mark the record as being safe from Java GC destruction */
    _ManageObject(env, (jlong)reader->record, JNI_FALSE);

    return record;

  CATCH_ERROR:
    _ThrowNITFException(env, error.message);
    return NULL;
}

/*
 * Class:     nitf_Reader
 * Method:    getNewImageReader
 * Signature: (I)Lnitf/ImageReader;
 */
JNIEXPORT jobject JNICALL Java_nitf_Reader_getNewImageReader(
                                                             JNIEnv * env,
                                                             jobject self,
                                                             jint imageSegmentNumber)
{
    nitf_Reader *reader = _GetObj(env, self);
    nitf_ImageReader *imageReader;
    nitf_Error error;
    jobject imageReaderObject;

    /* TODO: Support reader options? */
    imageReader
            = nitf_Reader_newImageReader(reader, imageSegmentNumber, NULL, &error);

    if (!imageReader)
    {
        _ThrowNITFException(env, error.message);
        return NULL;
    }

    imageReaderObject = _NewObject(env, (jlong)imageReader, "nitf/ImageReader");

    /* mark the io as being safe from Java GC destruction - since the reader needs it during its lifetime */
    _ManageObject(env, (jlong)imageReader->input, JNI_FALSE);

    return imageReaderObject;
}

/*
 * Class:     nitf_Reader
 * Method:    getNewGraphicReader
 * Signature: (I)Lnitf/SegmentReader;
 */
JNIEXPORT jobject JNICALL Java_nitf_Reader_getNewGraphicReader(
                                                               JNIEnv * env,
                                                               jobject self,
                                                               jint graphicSegmentNumber)
{
    nitf_Reader *reader = _GetObj(env, self);
    nitf_SegmentReader *segmentReader;
    nitf_Error error;
    jobject segmentReaderObject;

    segmentReader = nitf_Reader_newGraphicReader(reader, graphicSegmentNumber,
                                                 &error);

    if (!segmentReader)
    {
        _ThrowNITFException(env, error.message);
        return NULL;
    }

    segmentReaderObject = _NewObject(env, (jlong)segmentReader, "nitf/SegmentReader");

    /* mark the io as being safe from Java GC destruction - since the reader needs it during its lifetime */
    _ManageObject(env, (jlong)segmentReader->input, JNI_FALSE);

    return segmentReaderObject;
}

/*
 * Class:     nitf_Reader
 * Method:    getNewTextReader
 * Signature: (I)Lnitf/SegmentReader;
 */
JNIEXPORT jobject JNICALL Java_nitf_Reader_getNewTextReader(
                                                            JNIEnv * env,
                                                            jobject self,
                                                            jint textSegmentNumber)
{
    nitf_Reader *reader = _GetObj(env, self);
    nitf_SegmentReader *segmentReader;
    nitf_Error error;
    jobject segmentReaderObject;

    segmentReader
            = nitf_Reader_newTextReader(reader, textSegmentNumber, &error);

    if (!segmentReader)
    {
        _ThrowNITFException(env, error.message);
        return NULL;
    }

    segmentReaderObject = _NewObject(env, (jlong)segmentReader, "nitf/SegmentReader");

    /* mark the io as being safe from Java GC destruction - since the reader needs it during its lifetime */
    _ManageObject(env, (jlong)segmentReader->input, JNI_FALSE);

    return segmentReaderObject;
}

/*
 * Class:     nitf_Reader
 * Method:    getNewDEReader
 * Signature: (I)Lnitf/SegmentReader;
 */
JNIEXPORT jobject JNICALL Java_nitf_Reader_getNewDEReader(JNIEnv * env,
                                                          jobject self,
                                                          jint deSegmentNumber)
{
    nitf_Reader *reader = _GetObj(env, self);
    nitf_SegmentReader *deReader;
    nitf_Error error;
    jobject readerObject = NULL;

    deReader = nitf_Reader_newDEReader(reader, deSegmentNumber, &error);

    if (!deReader)
    {
        _ThrowNITFException(env, error.message);
        return NULL;
    }

    readerObject = _NewObject(env, (jlong)deReader, "nitf/SegmentReader");

    /* mark the io as being safe from Java GC destruction - since the reader needs it during its lifetime */
    _ManageObject(env, (jlong)deReader->input, JNI_FALSE);

    return readerObject;
}

/*
 * Class:     nitf_Reader
 * Method:    getInput
 * Signature: ()Lnitf/IOInterface;
 */
JNIEXPORT jobject JNICALL Java_nitf_Reader_getInput(JNIEnv * env, jobject self)
{
    nitf_Reader *reader = _GetObj(env, self);
    return _NewObject(env, (jlong) reader->input, "nitf/NativeIOInterface");
}

/*
 * Class:     nitf_Reader
 * Method:    getRecord
 * Signature: ()Lnitf/Record;
 */
JNIEXPORT jobject JNICALL Java_nitf_Reader_getRecord(JNIEnv * env, jobject self)
{
    nitf_Reader *reader = _GetObj(env, self);
    return _NewObject(env, (jlong) reader->record, "nitf/Record");
}

