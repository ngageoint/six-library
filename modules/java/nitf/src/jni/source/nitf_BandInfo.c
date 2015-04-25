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
#include "nitf_BandInfo.h"
#include "nitf_JNI.h"

NITF_JNI_DECLARE_OBJ(nitf_BandInfo)
/*
 * Class:     nitf_BandInfo
 * Method:    getBandEntriesPerLUT
 * Signature: ()Lnitf/Field;
 */
JNIEXPORT jobject JNICALL Java_nitf_BandInfo_getBandEntriesPerLUT
    (JNIEnv * env, jobject self)
{
    nitf_BandInfo *info = _GetObj(env, self);
    return _GetFieldObj(env, info->bandEntriesPerLUT);
}


/*
 * Class:     nitf_BandInfo
 * Method:    getImageFilterCode
 * Signature: ()Lnitf/Field;
 */
JNIEXPORT jobject JNICALL Java_nitf_BandInfo_getImageFilterCode
    (JNIEnv * env, jobject self)
{
    nitf_BandInfo *info = _GetObj(env, self);
    return _GetFieldObj(env, info->imageFilterCode);
}


/*
 * Class:     nitf_BandInfo
 * Method:    getImageFilterCondition
 * Signature: ()Lnitf/Field;
 */
JNIEXPORT jobject JNICALL Java_nitf_BandInfo_getImageFilterCondition
    (JNIEnv * env, jobject self)
{
    nitf_BandInfo *info = _GetObj(env, self);
    return _GetFieldObj(env, info->imageFilterCondition);
}


/*
 * Class:     nitf_BandInfo
 * Method:    getNumLUTs
 * Signature: ()Lnitf/Field;
 */
JNIEXPORT jobject JNICALL Java_nitf_BandInfo_getNumLUTs
    (JNIEnv * env, jobject self)
{
    nitf_BandInfo *info = _GetObj(env, self);
    return _GetFieldObj(env, info->numLUTs);
}


/*
 * Class:     nitf_BandInfo
 * Method:    getRepresentation
 * Signature: ()Lnitf/Field;
 */
JNIEXPORT jobject JNICALL Java_nitf_BandInfo_getRepresentation
    (JNIEnv * env, jobject self)
{

    nitf_BandInfo *info = _GetObj(env, self);
    return _GetFieldObj(env, info->representation);
}


/*
 * Class:     nitf_BandInfo
 * Method:    getSubcategory
 * Signature: ()Lnitf/Field;
 */
JNIEXPORT jobject JNICALL Java_nitf_BandInfo_getSubcategory
    (JNIEnv * env, jobject self)
{
    nitf_BandInfo *info = _GetObj(env, self);
    return _GetFieldObj(env, info->subcategory);
}


/*
 * Class:     nitf_BandInfo
 * Method:    getLookupTable
 * Signature: ()Lnitf/LookupTable;
 */
JNIEXPORT jobject JNICALL Java_nitf_BandInfo_getLookupTable
    (JNIEnv * env, jobject self)
{
    nitf_BandInfo *info = _GetObj(env, self);
    jobject lut = NULL;

    if (info->lut)
    {
        lut = _NewObject(env, (jlong)info->lut, "nitf/LookupTable");
        /* tell Java not to manage it */
        _ManageObject(env, (jlong)info->lut, JNI_FALSE);
    }
    return lut;
}


/*
 * Class:     nitf_BandInfo
 * Method:    setLookupTable
 * Signature: (Lnitf/LookupTable;)V
 */
JNIEXPORT void JNICALL Java_nitf_BandInfo_setLookupTable
    (JNIEnv * env, jobject self, jobject lookupTable)
{
    nitf_BandInfo *info = _GetObj(env, self);
    jclass lutClass;            /* the class for the LUT */
    jmethodID methodID;
    nitf_LookupTable *lut = NULL;       /* the LUT */
    nitf_Error error;           /* temp error object */

    /* get the class, method, and C object */
    lutClass = (*env)->GetObjectClass(env, lookupTable);
    methodID = (*env)->GetMethodID(env, lutClass, "getAddress", "()J");
    lut =
        (nitf_LookupTable *) (*env)->CallLongMethod(env, lookupTable,
                                                    methodID);

    /* if alreay has a LUT, release it to Java */
    if (info->lut)
    {
        _ManageObject(env, (jlong)info->lut, JNI_TRUE);
    }
    /* set the lut */
    info->lut = lut;

    /* set the tables and entries field in the BandInfo object */
    nitf_Field_setUint32(info->numLUTs, lut->tables, &error);
    nitf_Field_setUint32(info->bandEntriesPerLUT, lut->entries, &error);
    
    /* tell Java not to manage it */
    _ManageObject(env, (jlong)info->lut, JNI_FALSE);

    return;
}

