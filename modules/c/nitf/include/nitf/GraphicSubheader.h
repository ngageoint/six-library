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

#ifndef __NITF_GRAPHIC_SUBHEADER_H__
#define __NITF_GRAPHIC_SUBHEADER_H__

/*  We get file security and some constants already  */
/*  defined by including this first                  */
#include "nitf/FileHeader.h"

#define NITF_SY_SZ      2
#define NITF_SID_SZ    10
#define NITF_SNAME_SZ  20
#define NITF_SSCLAS_SZ  1
#define NITF_SFMT_SZ    1
#define NITF_SSTRUCT_SZ  13
#define NITF_SDLVL_SZ   3
#define NITF_SALVL_SZ   3
#define NITF_SLOC_SZ   10
#define NITF_SBND1_SZ  10
#define NITF_SCOLOR_SZ  1
#define NITF_SBND2_SZ  10
#define NITF_SRES2_SZ   2
#define NITF_SY filePartType
#define NITF_SID graphicID
#define NITF_SNAME name
#define NITF_SSCLAS securityClass
#define NITF_SFMT stype
#define NITF_SSTRUCT res1
#define NITF_SDLVL displayLevel
#define NITF_SALVL attachmentLevel
#define NITF_SLOC location
#define NITF_SBND1 bound1Loc
#define NITF_SCOLOR color
#define NITF_SBND2 bound2Loc
#define NITF_SRES2 res2
#define NITF_SXSHDL_SZ 5
#define NITF_SXSOFL_SZ 3
#define NITF_SXSHDL extendedHeaderLength
#define NITF_SXSOFL extendedHeaderOverflow

NITF_CXX_GUARD

/*!
 *  TODO: Add documentation
 */
typedef struct _nitf_GraphicSubheader
{
    nitf_Field *filePartType;
    nitf_Field *graphicID;
    nitf_Field *name;
    nitf_Field *securityClass;
    nitf_FileSecurity *securityGroup;
    nitf_Field *encrypted;
    nitf_Field *stype;
    nitf_Field *res1;
    nitf_Field *displayLevel;
    nitf_Field *attachmentLevel;
    nitf_Field *location;
    nitf_Field *bound1Loc;
    nitf_Field *color;
    nitf_Field *bound2Loc;
    nitf_Field *res2;

    nitf_Field *extendedHeaderLength;
    nitf_Field *extendedHeaderOverflow;

    /*  This section (unfortunately), has an extendedSection  */
    nitf_Extensions *extendedSection;

}
nitf_GraphicSubheader;


/*!
 *  Construct a new graphic subheader object.  This object will
 *  be null set for every field.  The object returned will be
 *  a full-fledged subheader, unless there is an error, in which
 *  case NULL will be returned, and the error object that was
 *  passed in will be populated.
 *
 *  \param error In case of error, when this function returns, this
 *  parameter will be populated
 *
 *  \return The graphic subheader, or NULL on failure.
 */
NITFAPI(nitf_GraphicSubheader *)
nitf_GraphicSubheader_construct(nitf_Error * error);

/*!
 *  TODO: Add documentation
 */
NITFAPI(nitf_GraphicSubheader *)
nitf_GraphicSubheader_clone(nitf_GraphicSubheader * source,
                            nitf_Error * error);

/*!
 *  Destroy a graphic subheader object, and NULL-set the subheader.
 *  A double pointer is make the NULL-set persistant on function
 *  exit.
 *
 *  \param The address of the the graphic subheader pointer
 */
NITFAPI(void) nitf_GraphicSubheader_destruct(nitf_GraphicSubheader **
        subhdr);


NITF_CXX_ENDGUARD

#endif
