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

#include "cgm_Metafile.h"
#include "cgm_Metafile_Destructor.h"
#include "cgm_JNI.h"

CGM_JNI_DECLARE_OBJ(cgm_Metafile)


JNIEXPORT jboolean JNICALL Java_cgm_Metafile_00024Destructor_destructMemory
  (JNIEnv *env, jobject self, jlong address)
{
    cgm_Metafile *metafile = (cgm_Metafile*)address;
    if (metafile)
    {
        cgm_Metafile_destruct(&metafile);
        return JNI_TRUE;
    }
    return JNI_FALSE;
}

JNIEXPORT void JNICALL Java_cgm_Metafile_setName
  (JNIEnv *env, jobject self, jstring name)
{
    cgm_Metafile *metafile = _GetObj(env, self);
    if (metafile->name)
        NITF_FREE(metafile->name);
    metafile->name = _NewCharArrayFromString(env, name);
}


JNIEXPORT jstring JNICALL Java_cgm_Metafile_getName
  (JNIEnv *env, jobject self)
{
    cgm_Metafile *metafile = _GetObj(env, self);
    return (*env)->NewStringUTF(env, metafile->name);
}

JNIEXPORT jshort JNICALL Java_cgm_Metafile_getVersion
  (JNIEnv *env, jobject self)
{
    cgm_Metafile *metafile = _GetObj(env, self);
    return metafile->version;
}

JNIEXPORT void JNICALL Java_cgm_Metafile_setVersion
  (JNIEnv *env, jobject self, jshort version)
{
    cgm_Metafile *metafile = _GetObj(env, self);
    metafile->version = version;
}


JNIEXPORT jstring JNICALL Java_cgm_Metafile_getDescription
  (JNIEnv *env, jobject self)
{
    cgm_Metafile *metafile = _GetObj(env, self);
    return (*env)->NewStringUTF(env, metafile->description);
}

JNIEXPORT void JNICALL Java_cgm_Metafile_setDescription
  (JNIEnv *env, jobject self, jstring description)
{
    cgm_Metafile *metafile = _GetObj(env, self);
    if (metafile->description)
        NITF_FREE(metafile->description);
    metafile->description = _NewCharArrayFromString(env, description);
}

JNIEXPORT jobjectArray JNICALL Java_cgm_Metafile_getFonts
  (JNIEnv *env, jobject self)
{
    return NULL;
}

JNIEXPORT void JNICALL Java_cgm_Metafile_setFonts
  (JNIEnv *env, jobject self, jobjectArray fonts)
{
    /* TODO */
}

JNIEXPORT void JNICALL Java_cgm_Metafile_removeFont
  (JNIEnv *env, jobject self, jstring font)
{
    /* TODO */
}

JNIEXPORT void JNICALL Java_cgm_Metafile_addFont
  (JNIEnv *env, jobject self, jstring font)
{
    /* TODO */
}

JNIEXPORT jobject JNICALL Java_cgm_Metafile_getPicture
  (JNIEnv *env, jobject self)
{
    cgm_Metafile *metafile = _GetObj(env, self);
    return _NewObject(env, "cgm/Picture", (jlong) metafile->picture);
}
