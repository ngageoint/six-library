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

#include "cgm_JNI.h"

NITFPROT(void) _ThrowNITFException(JNIEnv *env, const char *message)
{
    static jclass exceptClass = NULL;
    if (!exceptClass)
    {
        jclass localClass = (*env)->FindClass(env, "nitf/NITFException");
        exceptClass = (*env)->NewGlobalRef(env, localClass);
        (*env)->DeleteLocalRef(env, localClass);
    }
    (*env)->ThrowNew(env, exceptClass, message);
}

NITFPROT(char*) _NewCharArrayFromString(JNIEnv *env, jstring string)
{
    const char* tempString = NULL;
    char* cString = NULL;
    size_t len;
    
    tempString = (*env)->GetStringUTFChars(env, string, 0);
    len = strlen(tempString);
    
    cString = (char*)NITF_MALLOC(len + 1);
    strcpy(cString, tempString);
    
    return cString;
}

NITFPROT(jobject) _NewObject(JNIEnv *env, const char* className, jlong address)
{
    jclass clazz = (*env)->FindClass(env, className);
    jmethodID methodID = (*env)->GetMethodID(env, clazz, "<init>", "(J)V");
    return address ? (*env)->NewObject(env, clazz, methodID, address) : NULL;
}
