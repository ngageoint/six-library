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
#include "nitf_SegmentWriter.h"
#include "nitf_SegmentWriter_Destructor.h"
#include "nitf_JNI.h"

/*  This creates the _SetObj and _GetObj accessors  */
NITF_JNI_DECLARE_OBJ(nitf_SegmentWriter)

JNIEXPORT jboolean JNICALL Java_nitf_SegmentWriter_00024Destructor_destructMemory
    (JNIEnv * env, jobject self, jlong address)
{
	nitf_WriteHandler *writer = (nitf_WriteHandler*)address;
    if (writer)
    {
        nitf_WriteHandler_destruct(&writer);
        return JNI_TRUE;
    }
    return JNI_FALSE;
}

/*
 * Class:     nitf_SegmentWriter
 * Method:    attachSource
 * Signature: (Lnitf/SegmentSource;)Z
 */
JNIEXPORT jboolean JNICALL Java_nitf_SegmentWriter_attachSource
  (JNIEnv *env, jobject self, jobject segmentSource)
{
	nitf_SegmentWriter *writer = _GetObj(env, self);
    jclass segmentSourceClass = (*env)->FindClass(env, "nitf/SegmentSource");
    nitf_Error error;

    nitf_SegmentSource *source;
    jmethodID methodID =
        (*env)->GetMethodID(env, segmentSourceClass, "getAddress", "()J");
    source =
        (nitf_SegmentSource *) (*env)->CallLongMethod(env, segmentSource,
                                                    methodID);

    if (!nitf_SegmentWriter_attachSource(writer, source, &error))
    {
        _ThrowNITFException(env, error.message);
        return JNI_FALSE;
    }
    return JNI_TRUE;
}

