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

#ifndef __NITF_LABEL_SUBHEADER_H__
#define __NITF_LABEL_SUBHEADER_H__

/*  We get file security and some constants already  */
/*  defined by including this first                  */
#include "nitf/FileHeader.h"

#define NITF_LA_SZ      2
#define NITF_LID_SZ    10
#define NITF_LSCLAS_SZ  1
#define NITF_LFS_SZ     1
#define NITF_LCW_SZ     2
#define NITF_LCH_SZ     2
#define NITF_LDLVL_SZ   3
#define NITF_LALVL_SZ   3
#define NITF_LLOCR_SZ   5
#define NITF_LLOCC_SZ   5
#define NITF_LTC_SZ     3
#define NITF_LBC_SZ     3
#define NITF_LA     filePartType
#define NITF_LID    labelID
#define NITF_LSCLAS securityClass
#define NITF_LFS    fontStyle
#define NITF_LCW    cellWidth
#define NITF_LCH    cellHeight
#define NITF_LDLVL  displayLevel
#define NITF_LALVL  attachmentLevel
#define NITF_LLOCR  locationRow
#define NITF_LLOCC  locationColumn
#define NITF_LTC    textColor
#define NITF_LBC    backgroundColor
#define NITF_LXSHDL_SZ 5
#define NITF_LXSOFL_SZ 3
#define NITF_LXSHDL extendedHeaderLength
#define NITF_LXSOFL extendedHeaderOverflow

NITF_CXX_GUARD

/*!
 *  TODO: Add documentation
 */
typedef struct _nitf_LabelSubheader
{
    nitf_Field *filePartType;
    nitf_Field *labelID;
    nitf_Field *securityClass;
    nitf_FileSecurity *securityGroup;
    nitf_Field *encrypted;
    nitf_Field *fontStyle;
    nitf_Field *cellWidth;
    nitf_Field *cellHeight;
    nitf_Field *displayLevel;
    nitf_Field *attachmentLevel;
    nitf_Field *locationRow;
    nitf_Field *locationColumn;
    nitf_Field *textColor;      /* 3 raw bytes RGB */
    nitf_Field *backgroundColor;        /* 3 raw bytes RGB */

    nitf_Field *extendedHeaderLength;
    nitf_Field *extendedHeaderOverflow;

    /*  This section (unfortunately), has an extendedSection  */
    nitf_Extensions *extendedSection;

}
nitf_LabelSubheader;


/*!
 *  Construct a new label subheader object.  This object will
 *  be null set for every field.  The object returned will be
 *  a full-fledged subheader, unless there is an error, in which
 *  case NULL will be returned, and the error object that was
 *  passed in will be populated.
 *
 *  \param error In case of error, when this function returns, this
 *  parameter will be populated
 *
 *  \return The label subheader, or NULL on failure.
 */
NITFAPI(nitf_LabelSubheader *)
nitf_LabelSubheader_construct(nitf_Error * error);

/*!
 *  TODO: Add documentation
 */
NITFAPI(nitf_LabelSubheader *)
nitf_LabelSubheader_clone(nitf_LabelSubheader * source,
                          nitf_Error * error);

/*!
 *  Destroy a label subheader object, and NULL-set the subheader.
 *  A double pointer is make the NULL-set persistant on function
 *  exit.
 *
 *  \param The address of the the label subheader pointer
 */
NITFAPI(void) nitf_LabelSubheader_destruct(nitf_LabelSubheader ** subhdr);

NITF_CXX_ENDGUARD

#endif
