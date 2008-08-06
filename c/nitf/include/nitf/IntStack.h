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

#ifndef __NITF_INT_STACK_H__
#define __NITF_INT_STACK_H__

#include "nitf/System.h"
#include "nitf/Error.h"

#define NITF_INT_STACK_DEPTH 10

NITF_CXX_GUARD

/*!
 *  TODO: Add documentation
 */
typedef struct _protected_nitf_IntStack
{
    int sp;
    int st[NITF_INT_STACK_DEPTH];
}
nitf_IntStack;

/*!
 *  TODO: Add documentation
 */
NITFPROT(nitf_IntStack *) nitf_IntStack_construct(nitf_Error * error);


NITFPROT(nitf_IntStack *) nitf_IntStack_clone(nitf_IntStack* stack, 
                                              nitf_Error* error);

/*!
 *  TODO: Add documentation
 */
NITFPROT(void) nitf_IntStack_destruct(nitf_IntStack ** stack);

/*!
 *  TODO: Add documentation
 */
NITFPROT(int) nitf_IntStack_top(nitf_IntStack * stack, nitf_Error * error);

/*!
 *  TODO: Add documentation
 */
NITFPROT(int) nitf_IntStack_push(nitf_IntStack * stack,
                                 int n, nitf_Error * error);

/*!
 *  TODO: Add documentation
 */
NITFPROT(int) nitf_IntStack_pop(nitf_IntStack * stack, nitf_Error * error);

/*!
 *  TODO: Add documentation
 */
NITFPROT(int) nitf_IntStack_depth(nitf_IntStack * stack,
                                  nitf_Error * error);

NITF_CXX_ENDGUARD

#endif
