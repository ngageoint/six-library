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

#ifndef __NITF_DE_SUBHEADER_H__
#define __NITF_DE_SUBHEADER_H__

/*  We get file security and some constants already  */
/*  defined by including this first                  */
#include "nitf/FileHeader.h"

NITF_CXX_GUARD
#define NITF_DE_SZ      2
#define NITF_DESTAG_SZ  25
#define NITF_DESVER_SZ  2
#define NITF_DESCLAS_SZ 1
#define NITF_DESOFLW_SZ 6
#define NITF_DESITEM_SZ 3
#define NITF_DESSHL_SZ  4
#define NITF_DE        filePartType
#define NITF_DESTAG    typeID
#define NITF_DESVER    version
#define NITF_DESCLAS   securityClass
#define NITF_DESOFLW   overflowedHeaderType
#define NITF_DESITEM   dataItemOverflowed
#define NITF_DESSHL    subheaderFieldsLength

typedef struct _nitf_DESubheader
{
    nitf_Field *filePartType;
    nitf_Field *typeID;
    nitf_Field *version;
    nitf_Field *securityClass;
    nitf_FileSecurity *securityGroup;
    nitf_Field *overflowedHeaderType;
    nitf_Field *dataItemOverflowed;
    nitf_Field *subheaderFieldsLength;
    nitf_TRE *subheaderFields;
    nitf_Uint64 dataLength;

    nitf_Extensions *userDefinedSection;
}
nitf_DESubheader;


/*!
 *  Construct a new de subheader object.  This object will
 *  be null set for every field.  The object returned will be
 *  a full-fledged subheader, unless there is an error, in which
 *  case NULL will be returned, and the error object that was
 *  passed in will be populated.
 *
 *  \param error In case of error, when this function returns, this
 *  parameter will be populated
 *
 *  \return The de subheader, or NULL on failure.
 */
NITFAPI(nitf_DESubheader *) nitf_DESubheader_construct(nitf_Error * error);

/*!
 *  TODO: Add documentation
 */
NITFAPI(nitf_DESubheader *)
nitf_DESubheader_clone(nitf_DESubheader * source, nitf_Error * error);

/*!
 *  Destroy a de subheader object, and NULL-set the subheader.
 *  A double pointer is make the NULL-set persistant on function
 *  exit.
 *
 *  \param The address of the the de subheader pointer
 */
NITFAPI(void) nitf_DESubheader_destruct(nitf_DESubheader ** subhdr);


NITF_CXX_ENDGUARD

#endif
