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

#ifndef __NITF_TEXT_SUBHEADER_H__
#define __NITF_TEXT_SUBHEADER_H__

/*  We get file security and some constants already  */
/*  defined by including this first                  */
#include "nitf/FileHeader.h"

#define NITF_TE_SZ       2
#define NITF_TEXTID_SZ   7
#define NITF_TXTALVL_SZ  3
#define NITF_TXTDT_SZ   14
#define NITF_TXTITL_SZ  80
#define NITF_TSCLAS_SZ   1
#define NITF_TXTFMT_SZ   3
#define NITF_TE        filePartType
#define NITF_TEXTID    textID
#define NITF_TXTALVL   attachmentLevel
#define NITF_TXTDT     dateTime
#define NITF_TXTITL    title
#define NITF_TSCLAS    securityClass
#define NITF_TXTFMT    format
#define NITF_TXSHDL_SZ 5
#define NITF_TXSOFL_SZ 3
#define NITF_TXSHDL extendedHeaderLength
#define NITF_TXSOFL extendedHeaderOverflow

NITF_CXX_GUARD

/*!
 *  TODO: Add documentation
 */
typedef struct _nitf_TextSubheader
{
    nitf_Field *filePartType;
    nitf_Field *textID;
    nitf_Field *attachmentLevel;
    nitf_Field *dateTime;
    nitf_Field *title;
    nitf_Field *securityClass;
    nitf_FileSecurity *securityGroup;
    nitf_Field *encrypted;
    nitf_Field *format;

    nitf_Field *extendedHeaderLength;
    nitf_Field *extendedHeaderOverflow;

    /*  This section (unfortunately), has an extendedSection  */
    nitf_Extensions *extendedSection;

}
nitf_TextSubheader;


/*!
 *  Construct a new text subheader object.  This object will
 *  be null set for every field.  The object returned will be
 *  a full-fledged subheader, unless there is an error, in which
 *  case NULL will be returned, and the error object that was
 *  passed in will be populated.
 *
 *  \param error In case of error, when this function returns, this
 *  parameter will be populated
 *
 *  \return The text subheader, or NULL on failure.
 */
NITFAPI(nitf_TextSubheader *)
nitf_TextSubheader_construct(nitf_Error * error);

/*!
 *  TODO: Add documentation
 */
NITFAPI(nitf_TextSubheader *)
nitf_TextSubheader_clone(nitf_TextSubheader * source, nitf_Error * error);

/*!
 *  Destroy a text subheader object, and NULL-set the subheader.
 *  A double pointer is make the NULL-set persistant on function
 *  exit.
 *
 *  \param The address of the the text subheader pointer
 */
NITFAPI(void) nitf_TextSubheader_destruct(nitf_TextSubheader ** subhdr);

NITF_CXX_ENDGUARD

#endif
