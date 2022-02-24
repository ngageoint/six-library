/* =========================================================================
 * This file is part of NITRO
 * =========================================================================
 *
 * (C) Copyright 2004 - 2016, MDA Information Systems LLC
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


#ifndef __NITF_TRE_DESCRIPTION_H__
#define __NITF_TRE_DESCRIPTION_H__
#pragma once

/*!
 * The TREDescription structure encapsulates the metadata that describes one
 * field description in a TRE.
 */
typedef struct _nitf_TREDescription
{
    int data_type;                  /*!< the type of field */
    int data_count;                 /*!< the size of the field */
    const char *label;            /*!< description */
    const char *tag;              /*!< unique tag */
    char *special;                  /*!< special field, reserved for special cases */
} nitf_TREDescription;


#define NITF_TRE_DESC_NO_LENGTH      -1

/*!
 * Information about one TREDescription object
 */
typedef struct _nitf_TREDescriptionInfo
{
    char *name; /*! The name to associate with the Description */
    nitf_TREDescription *description;   /*! The TREDescription */
    int lengthMatch;    /*! The length to match against TREs with; used to choose TREs */
} nitf_TREDescriptionInfo;

/*!
 * Contains a set, or array, of TREDescriptionInfos*
 */
typedef struct _nitf_TREDescriptionSet
{
    int defaultIndex;   /*! Set to the index of the default description */
    nitf_TREDescriptionInfo* descriptions;
} nitf_TREDescriptionSet;


#endif

