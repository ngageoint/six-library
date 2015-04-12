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

#ifndef __NRT_INT_STACK_H__
#define __NRT_INT_STACK_H__

#include "nrt/System.h"

#define NRT_INT_STACK_DEPTH 10

NRT_CXX_GUARD

typedef struct _protected_nrt_IntStack
{
    int sp;
    int st[NRT_INT_STACK_DEPTH];
} nrt_IntStack;

NRTPROT(nrt_IntStack *) nrt_IntStack_construct(nrt_Error * error);

NRTPROT(nrt_IntStack *) nrt_IntStack_clone(nrt_IntStack * stack,
                                           nrt_Error * error);

NRTPROT(void) nrt_IntStack_destruct(nrt_IntStack ** stack);

NRTPROT(int) nrt_IntStack_top(nrt_IntStack * stack, nrt_Error * error);

NRTPROT(int) nrt_IntStack_push(nrt_IntStack * stack, int n, nrt_Error * error);

NRTPROT(int) nrt_IntStack_pop(nrt_IntStack * stack, nrt_Error * error);

NRTPROT(int) nrt_IntStack_depth(nrt_IntStack * stack, nrt_Error * error);

NRT_CXX_ENDGUARD
#endif
