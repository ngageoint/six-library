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

#include "nrt/Pair.h"
#include "nrt/Utils.h"

NRTAPI(void) nrt_Pair_init(nrt_Pair * pair, const char *key, NRT_DATA * data)
{
    pair->key = nrt_strdup(key);
    pair->data = data;
}

NRTAPI(void) nrt_Pair_copy(nrt_Pair * dst, nrt_Pair * src)
{
    nrt_Pair_init(dst, src->key, src->data);
}
