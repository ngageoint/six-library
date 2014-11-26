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
#include "nitf_FileSecurity.h"
#include "nitf_JNI.h"

NITF_JNI_DECLARE_OBJ(nitf_FileSecurity)
/*
 * Class:     nitf_FileSecurity
 * Method:    getClassificationAuthority
 * Signature: ()Lnitf/Field;
 */
JNIEXPORT jobject JNICALL Java_nitf_FileSecurity_getClassificationAuthority
    (JNIEnv * env, jobject self)
{
    nitf_FileSecurity *info = _GetObj(env, self);
    return _GetFieldObj(env, info->classificationAuthority);
}


/*
 * Class:     nitf_FileSecurity
 * Method:    getClassificationAuthorityType
 * Signature: ()Lnitf/Field;
 */
JNIEXPORT jobject JNICALL
    Java_nitf_FileSecurity_getClassificationAuthorityType(JNIEnv * env,
                                                          jobject self)
{
    nitf_FileSecurity *info = _GetObj(env, self);
    return _GetFieldObj(env, info->classificationAuthorityType);
}


/*
 * Class:     nitf_FileSecurity
 * Method:    getClassificationReason
 * Signature: ()Lnitf/Field;
 */
JNIEXPORT jobject JNICALL Java_nitf_FileSecurity_getClassificationReason
    (JNIEnv * env, jobject self)
{
    nitf_FileSecurity *info = _GetObj(env, self);
    return _GetFieldObj(env, info->classificationReason);
}


/*
 * Class:     nitf_FileSecurity
 * Method:    getClassificationSystem
 * Signature: ()Lnitf/Field;
 */
JNIEXPORT jobject JNICALL Java_nitf_FileSecurity_getClassificationSystem
    (JNIEnv * env, jobject self)
{
    nitf_FileSecurity *info = _GetObj(env, self);
    return _GetFieldObj(env, info->classificationSystem);
}


/*
 * Class:     nitf_FileSecurity
 * Method:    getClassificationText
 * Signature: ()Lnitf/Field;
 */
JNIEXPORT jobject JNICALL Java_nitf_FileSecurity_getClassificationText
    (JNIEnv * env, jobject self)
{
    nitf_FileSecurity *info = _GetObj(env, self);
    return _GetFieldObj(env, info->classificationText);
}


/*
 * Class:     nitf_FileSecurity
 * Method:    getCodewords
 * Signature: ()Lnitf/Field;
 */
JNIEXPORT jobject JNICALL Java_nitf_FileSecurity_getCodewords
    (JNIEnv * env, jobject self)
{
    nitf_FileSecurity *info = _GetObj(env, self);
    return _GetFieldObj(env, info->codewords);
}


/*
 * Class:     nitf_FileSecurity
 * Method:    getControlAndHandling
 * Signature: ()Lnitf/Field;
 */
JNIEXPORT jobject JNICALL Java_nitf_FileSecurity_getControlAndHandling
    (JNIEnv * env, jobject self)
{
    nitf_FileSecurity *info = _GetObj(env, self);
    return _GetFieldObj(env, info->controlAndHandling);
}


/*
 * Class:     nitf_FileSecurity
 * Method:    getDeclassificationDate
 * Signature: ()Lnitf/Field;
 */
JNIEXPORT jobject JNICALL Java_nitf_FileSecurity_getDeclassificationDate
    (JNIEnv * env, jobject self)
{
    nitf_FileSecurity *info = _GetObj(env, self);
    return _GetFieldObj(env, info->declassificationDate);
}


/*
 * Class:     nitf_FileSecurity
 * Method:    getDeclassificationExemption
 * Signature: ()Lnitf/Field;
 */
JNIEXPORT jobject JNICALL
    Java_nitf_FileSecurity_getDeclassificationExemption(JNIEnv * env,
                                                        jobject self)
{
    nitf_FileSecurity *info = _GetObj(env, self);
    return _GetFieldObj(env, info->declassificationExemption);
}


/*
 * Class:     nitf_FileSecurity
 * Method:    getDeclassificationType
 * Signature: ()Lnitf/Field;
 */
JNIEXPORT jobject JNICALL Java_nitf_FileSecurity_getDeclassificationType
    (JNIEnv * env, jobject self)
{
    nitf_FileSecurity *info = _GetObj(env, self);
    return _GetFieldObj(env, info->declassificationType);
}


/*
 * Class:     nitf_FileSecurity
 * Method:    getDowngrade
 * Signature: ()Lnitf/Field;
 */
JNIEXPORT jobject JNICALL Java_nitf_FileSecurity_getDowngrade
    (JNIEnv * env, jobject self)
{
    nitf_FileSecurity *info = _GetObj(env, self);
    return _GetFieldObj(env, info->downgrade);
}


/*
 * Class:     nitf_FileSecurity
 * Method:    getDowngradeDateTime
 * Signature: ()Lnitf/Field;
 */
JNIEXPORT jobject JNICALL Java_nitf_FileSecurity_getDowngradeDateTime
    (JNIEnv * env, jobject self)
{
    nitf_FileSecurity *info = _GetObj(env, self);
    return _GetFieldObj(env, info->downgradeDateTime);
}


/*
 * Class:     nitf_FileSecurity
 * Method:    getReleasingInstructions
 * Signature: ()Lnitf/Field;
 */
JNIEXPORT jobject JNICALL Java_nitf_FileSecurity_getReleasingInstructions
    (JNIEnv * env, jobject self)
{
    nitf_FileSecurity *info = _GetObj(env, self);
    return _GetFieldObj(env, info->releasingInstructions);
}


/*
 * Class:     nitf_FileSecurity
 * Method:    getSecurityControlNumber
 * Signature: ()Lnitf/Field;
 */
JNIEXPORT jobject JNICALL Java_nitf_FileSecurity_getSecurityControlNumber
    (JNIEnv * env, jobject self)
{
    nitf_FileSecurity *info = _GetObj(env, self);
    return _GetFieldObj(env, info->securityControlNumber);
}


/*
 * Class:     nitf_FileSecurity
 * Method:    getSecuritySourceDate
 * Signature: ()Lnitf/Field;
 */
JNIEXPORT jobject JNICALL Java_nitf_FileSecurity_getSecuritySourceDate
    (JNIEnv * env, jobject self)
{
    nitf_FileSecurity *info = _GetObj(env, self);
    return _GetFieldObj(env, info->securitySourceDate);
}

/*
 * Class:     nitf_FileSecurity
 * Method:    resizeForVersion
 * Signature: (Lnitf/Version;)V
 */
JNIEXPORT void JNICALL Java_nitf_FileSecurity_resizeForVersion
  (JNIEnv *env, jobject self, jobject versionObject)
{
    nitf_FileSecurity *info = _GetObj(env, self);
    nitf_Version version = _GetNITFVersion(env, versionObject);
    nitf_Error error;

    if (!nitf_FileSecurity_resizeForVersion(info, version, &error))
        _ThrowNITFException(env, error.message);
}
