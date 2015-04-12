/* =========================================================================
 * This file is part of NITRO
 * =========================================================================
 * 
 * (C) Copyright 2004 - 2010, General Dynamics - Advanced Information Systems
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

#ifndef __NITF_COMPONENT_INFO_H__
#define __NITF_COMPONENT_INFO_H__

#include "nitf/System.h"
#include "nitf/Field.h"

NITF_CXX_GUARD

/*!
  *  \struct nitf_ComponentInfo
  *  \brief Carries information about the images, graphics, text, or extension
  *  components contained within the file.
  *
  *  The fields from this class are described in the
  *  file header.  They contain information about some component section
  *  contained in a NITF 2.1 file.
  */
typedef struct _nitf_ComponentInfo
{
    nitf_Field *lengthSubheader;
    nitf_Field *lengthData;
}
nitf_ComponentInfo;

/*!
 *  Construct the component info object.  We allocate a new object,
 *  and initialize the subheader and data sizes to zero.
 *
 *  \param  subheaderFieldWidth size of the subheader field
 *  \param  dataFieldWidth size of data field width
 *  \param  error error to populate on problem
 *  \return Return the newly created object.
 */
NITFAPI(nitf_ComponentInfo *)
nitf_ComponentInfo_construct(nitf_Uint32 subheaderFieldWidth,
                             nitf_Uint32 dataFieldWidth,
                             nitf_Error * error);

/*!
 *  Clone this object.  This is a deep copy operation.
 *
 *  \param source The source object
 *  \param error  An error to populate upon failure
 *  \return A new object that is identical to the old
 */
NITFAPI(nitf_ComponentInfo *)
nitf_ComponentInfo_clone(nitf_ComponentInfo * source, nitf_Error * error);

/*!
 *  Destroy the image info structure and set the object to NULL.
 *
 *  \param info This is the object to destruct and NULLify
 *
 */
NITFAPI(void) nitf_ComponentInfo_destruct(nitf_ComponentInfo ** info);


NITF_CXX_ENDGUARD

#endif
