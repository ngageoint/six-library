/* =========================================================================
 * This file is part of NITRO
 * =========================================================================
 * 
 * (C) Copyright 2004 - 2008, General Dynamics - Advanced Information Systems
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

#ifndef __NITF_PAIR_H__
#define __NITF_PAIR_H__

#include "nitf/System.h"

NITF_CXX_GUARD

/*!
 *  \struct nitf_Pair
 *  \brief  This is an object containing the data and the key
 *
 *  The nitf_Pair is an object similar to a std::pair<char, nitf_Data
 *
 */
typedef struct _nitf_Pair
{
    char *key;
    NITF_DATA *data;
}
nitf_Pair;


/*!
 *  Copy the key, maintain a pointer to the data
 *  \param pair The structure to initialize
 *  \param key  The key in the pair (is copied)
 *  \param data The data in the pair (not a copy)
 */
NITFAPI(void) nitf_Pair_init(nitf_Pair * pair, const char *key,
                             NITF_DATA * data);


/*!
 *  This simply calls the init method
 *  \param dst The destination
 *  \param src The source
 *
 */
NITFAPI(void) nitf_Pair_copy(nitf_Pair * dst, nitf_Pair * src);

NITF_CXX_ENDGUARD

#endif
