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
#include "nitf_PluginRegistry.h"

/*
 * Class:     nitf_PluginRegistry
 * Method:    loadPluginDir
 * Signature: (Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_nitf_PluginRegistry_loadPluginDir
  (JNIEnv *env, jclass thisClass, jstring dirName)
{
    char *dir;
    nitf_Error error;

    dir = (char *) (*env)->GetStringUTFChars(env, dirName, 0);
    if (nitf_PluginRegistry_loadDir(dir, &error) != NITF_SUCCESS)
    {
        _ThrowNITFException(env, error.message);
    }
    (*env)->ReleaseStringUTFChars(env, dirName, dir);
    return;
}


/*
 * Class:     nitf_PluginRegistry
 * Method:    canHandleTRE
 * Signature: (Ljava/lang/String;)Z
 */
JNIEXPORT jboolean JNICALL Java_nitf_PluginRegistry_canHandleTRE
  (JNIEnv *env, jclass thisClass, jstring name)
{
    char *treName;
    nitf_Error error;
    int had_err;
    nitf_PluginRegistry* reg = NULL;
    
    reg = nitf_PluginRegistry_getInstance(&error);
    treName = (char *) (*env)->GetStringUTFChars(env, name, 0);
    return nitf_PluginRegistry_retrieveTREHandler(reg, treName, &had_err, &error) == NULL ?
        JNI_FALSE : JNI_TRUE;
}

