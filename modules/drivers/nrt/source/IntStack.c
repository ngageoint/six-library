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

#include "nrt/IntStack.h"

NRTPROT(nrt_IntStack *) nrt_IntStack_construct(nrt_Error * error)
{
    nrt_IntStack *stk = (nrt_IntStack *) NRT_MALLOC(sizeof(nrt_IntStack));
    if (!stk)
    {
        nrt_Error_init(error, NRT_STRERROR(NRT_ERRNO), NRT_CTXT,
                       NRT_ERR_MEMORY);
        return NULL;
    }

    stk->sp = -1;
    return stk;
}

NRTPROT(nrt_IntStack *) nrt_IntStack_clone(nrt_IntStack * stack,
                                           nrt_Error * error)
{
    int i;
    nrt_IntStack *copy = nrt_IntStack_construct(error);
    if (!copy)
        return NULL;

    copy->sp = stack->sp;

    for (i = 0; i < NRT_INT_STACK_DEPTH; i++)
    {
        copy->st[i] = stack->st[i];
    }
    return copy;

}

NRTPROT(void) nrt_IntStack_destruct(nrt_IntStack ** stk)
{
    if (*stk)
    {
        NRT_FREE(*stk);
        *stk = NULL;
    }
}

NRTPROT(int) nrt_IntStack_top(nrt_IntStack * stack, nrt_Error * error)
{
    /* TODO: Someone should rewrite this class to not be so quirky */
    /* It is inconsistent with the rest of the library */
    if ((stack->sp >= 0) && (stack->sp < NRT_INT_STACK_DEPTH))
    {
        return stack->st[stack->sp];
    }

    nrt_Error_initf(error, NRT_CTXT, NRT_ERR_INT_STACK_OVERFLOW,
                    "Stack pointing at depth: %d", stack->sp);
    return 0;
}

NRTPROT(int) nrt_IntStack_push(nrt_IntStack * stack, int n, nrt_Error * error)
{
    /* TODO: See above */
    if ((stack->sp >= -1) && (stack->sp < NRT_INT_STACK_DEPTH - 1))
    {
        stack->st[++(stack->sp)] = n;
        return 1;
    }

    nrt_Error_initf(error, NRT_CTXT, NRT_ERR_INT_STACK_OVERFLOW,
                    "Stack pointing at depth: %d", stack->sp);
    return 0;
}

NRTPROT(int) nrt_IntStack_pop(nrt_IntStack * stack, nrt_Error * error)
{
    if ((stack->sp >= 0) && (stack->sp < NRT_INT_STACK_DEPTH))
    {
        return stack->st[(stack->sp)--];
    }
    /* This return value is nonsense.  We need to */
    nrt_Error_initf(error, NRT_CTXT, NRT_ERR_INT_STACK_OVERFLOW,
                    "Stack pointing at depth: %d", stack->sp);
    return 2147483647;
}

NRTPROT(int) nrt_IntStack_depth(nrt_IntStack * stack, nrt_Error * error)
{
    return stack->sp;
}
