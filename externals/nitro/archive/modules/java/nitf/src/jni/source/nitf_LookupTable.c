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
#include "nitf_LookupTable.h"
#include "nitf_LookupTable_Destructor.h"
#include "nitf_JNI.h"

NITF_JNI_DECLARE_OBJ( nitf_LookupTable)
/*
 * Class:     nitf_LookupTable
 * Method:    construct
 * Signature: (II[B)V
 */
JNIEXPORT void JNICALL Java_nitf_LookupTable_construct
(JNIEnv * env, jobject self, jint numTables, jint numEntries,
        jbyteArray lutData)
{
    nitf_LookupTable *lut = NULL;
    nitf_Error error;
    char *dataBuf = NULL;

    /* construct the LUT */
    lut = nitf_LookupTable_construct((uint32_t) numTables,
            (uint32_t) numEntries, &error);

    if (!lut)
    {
        /* throw an error */
        _ThrowNITFException(env, error.message);
        return;
    }

    dataBuf = (char *) (*env)->GetByteArrayElements(env, lutData, 0);
    if (!dataBuf)
    {
        _ThrowNITFException(env, "Out of memory!");
        return;
    }

    /* initialize the data */
    nitf_LookupTable_init(lut, (uint32_t) numTables,
            (uint32_t) numEntries, (NITF_DATA *) dataBuf,
            &error);

    (*env)->ReleaseByteArrayElements(env, lutData, dataBuf, 0);
    _SetObj(env, self, lut);
}

JNIEXPORT jboolean JNICALL Java_nitf_LookupTable_00024Destructor_destructMemory
    (JNIEnv * env, jobject self, jlong address)
{
    nitf_LookupTable *lut = (nitf_LookupTable*)address;
    if (lut)
    {
        nitf_LookupTable_destruct(&lut);
        return JNI_TRUE;
    }
    return JNI_FALSE;
}

/*
 * Class:     nitf_LookupTable
 * Method:    getNumTables
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_nitf_LookupTable_getNumTables(JNIEnv * env,
                                                          jobject self)
{
    nitf_LookupTable *lut = _GetObj(env, self);
    return lut->tables;
}

/*
 * Class:     nitf_LookupTable
 * Method:    getNumEntries
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_nitf_LookupTable_getNumEntries(JNIEnv * env,
                                                           jobject self)
{
    nitf_LookupTable *lut = _GetObj(env, self);
    return lut->entries;
}

/*
 * Class:     nitf_LookupTable
 * Method:    getData
 * Signature: ()[B
 */
JNIEXPORT jbyteArray JNICALL Java_nitf_LookupTable_getData(JNIEnv * env,
                                                           jobject self)
{
    nitf_LookupTable *lut = _GetObj(env, self);
    jint length;
    jbyteArray byteArray;

    if (lut->table)
    {
        length = lut->entries * lut->tables;
        byteArray = (*env)->NewByteArray(env, length);
        (*env)->SetByteArrayRegion(env, byteArray, 0, length, lut->table);
        return byteArray;
    }
    return NULL;
}

