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
#include "nitf_ComponentInfo.h"
#include "nitf_JNI.h"

NITF_JNI_DECLARE_OBJ(nitf_ComponentInfo)
/*
 * Class:     nitf_ComponentInfo
 * Method:    getLengthSubheader
 * Signature: ()Lnitf/Field;
 */
JNIEXPORT jobject JNICALL Java_nitf_ComponentInfo_getLengthSubheader
    (JNIEnv * env, jobject self)
{
    nitf_ComponentInfo *info = _GetObj(env, self);
    return _GetFieldObj(env, info->lengthSubheader);
}


/*
 * Class:     nitf_ComponentInfo
 * Method:    getLengthData
 * Signature: ()Lnitf/Field;
 */
JNIEXPORT jobject JNICALL Java_nitf_ComponentInfo_getLengthData
    (JNIEnv * env, jobject self)
{

    nitf_ComponentInfo *info = _GetObj(env, self);
    return _GetFieldObj(env, info->lengthData);
}

