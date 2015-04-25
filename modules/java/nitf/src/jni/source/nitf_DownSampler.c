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
#include "nitf_DownSampler.h"
#include "nitf_DownSampler_Destructor.h"
#include "nitf_JNI.h"

NITF_JNI_DECLARE_OBJ(nitf_DownSampler)

/*
 * Class:     nitf_DownSampler
 * Method:    construct
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_nitf_DownSampler_construct
  (JNIEnv *env, jobject self)
{
	/* Do nothing for now!!! */
	
	/* TODO - in the future provide a callback mechanism like
	 * nitf_BandSource does
	 */
}

JNIEXPORT jboolean JNICALL Java_nitf_DownSampler_00024Destructor_destructMemory
    (JNIEnv * env, jobject self, jlong address)
{
	nitf_DownSampler *downSampler = (nitf_DownSampler*)address;
    if (downSampler)
    {
        nitf_DownSampler_destruct(&downSampler);
        return JNI_TRUE;
    }
    return JNI_FALSE;
}


