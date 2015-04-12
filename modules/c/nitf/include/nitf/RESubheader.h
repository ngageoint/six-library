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

#ifndef __NITF_RE_SUBHEADER_H__
#define __NITF_RE_SUBHEADER_H__

/*  We get file security and some constants already  */
/*  defined by including this first                  */
#include "nitf/FileHeader.h"



#define NITF_RE_SZ      2
#define NITF_RESTAG_SZ  25
#define NITF_RESVER_SZ  2
#define NITF_RESCLAS_SZ 1
#define NITF_RESSHL_SZ  4

#define NITF_RE        filePartType
#define NITF_RESTAG    typeID
#define NITF_RESVER    version
#define NITF_RESCLAS   securityClass
#define NITF_RESSHL    subheaderFieldsLength

NITF_CXX_GUARD

/*!
 *  TODO: Add documentation
 */
typedef struct _nitf_RESubheader
{
    nitf_Field *filePartType;
    nitf_Field *typeID;
    nitf_Field *version;
    nitf_Field *securityClass;
    nitf_FileSecurity *securityGroup;
    nitf_Field *subheaderFieldsLength;
    char *subheaderFields;
    nitf_Uint64 dataLength;
}
nitf_RESubheader;


/*!
 *  Construct a new re subheader object.  This object will
 *  be null set for every field.  The object returned will be
 *  a full-fledged subheader, unless there is an error, in which
 *  case NULL will be returned, and the error object that was
 *  passed in will be populated.
 *
 *  \param error In case of error, when this function returns, this
 *  parameter will be populated
 *
 *  \return The re subheader, or NULL on failure.
 */
NITFAPI(nitf_RESubheader *) nitf_RESubheader_construct(nitf_Error * error);

/*!
 *  TODO: Add documentation
 */
NITFAPI(nitf_RESubheader *)
nitf_RESubheader_clone(nitf_RESubheader * source, nitf_Error * error);

/*!
 *  Destroy a re subheader object, and NULL-set the subheader.
 *  A double pointer is make the NULL-set persistant on function
 *  exit.
 *
 *  \param The address of the the re subheader pointer
 */
NITFAPI(void) nitf_RESubheader_destruct(nitf_RESubheader ** subhdr);


NITF_CXX_ENDGUARD

#endif
