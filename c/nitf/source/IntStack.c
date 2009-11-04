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

#include "nitf/IntStack.h"

NITFPROT(nitf_IntStack *) nitf_IntStack_construct(nitf_Error * error)
{
    nitf_IntStack *stk =
        (nitf_IntStack *) NITF_MALLOC(sizeof(nitf_IntStack));
    if (!stk)
    {
        nitf_Error_init(error, NITF_STRERROR(NITF_ERRNO),
                        NITF_CTXT, NITF_ERR_MEMORY);
        return NULL;
    }

    stk->sp = -1;
    return stk;
}

NITFPROT(nitf_IntStack*) nitf_IntStack_clone(nitf_IntStack* stack,
                                             nitf_Error* error)
{
    int i;
    nitf_IntStack* copy = nitf_IntStack_construct(error);
    if (!copy) return NULL;

    copy->sp = stack->sp;

    for (i = 0; i < NITF_INT_STACK_DEPTH; i++)
    {
        copy->st[ i ] = stack->st[i];
    }
    return copy;

}

NITFPROT(void) nitf_IntStack_destruct(nitf_IntStack ** stk)
{
    if (*stk)
    {
        NITF_FREE(*stk);
        *stk = NULL;
    }
}


NITFPROT(int) nitf_IntStack_top(nitf_IntStack * stack, nitf_Error * error)
{
    /*  TODO: Someone should rewrite this class to not be so quirky */
    /*  It is inconsistent with the rest of the library             */
    if ((stack->sp >= 0) && (stack->sp < NITF_INT_STACK_DEPTH))
    {
        return stack->st[stack->sp];
    }

    nitf_Error_initf(error, NITF_CTXT, NITF_ERR_INT_STACK_OVERFLOW,
                     "Stack pointing at depth: %d", stack->sp);
    return 0;
}


NITFPROT(int) nitf_IntStack_push(nitf_IntStack * stack,
                                 int n, nitf_Error * error)
{

    /*  TODO: See above  */
    if ((stack->sp >= -1) && (stack->sp < NITF_INT_STACK_DEPTH - 1))
    {
        stack->st[++(stack->sp)] = n;
        return 1;
    }

    nitf_Error_initf(error, NITF_CTXT, NITF_ERR_INT_STACK_OVERFLOW,
                     "Stack pointing at depth: %d", stack->sp);
    return 0;
}


NITFPROT(int) nitf_IntStack_pop(nitf_IntStack * stack, nitf_Error * error)
{
    if ((stack->sp >= 0) && (stack->sp < NITF_INT_STACK_DEPTH))
    {
        return stack->st[(stack->sp)--];
    }
    /*  This return value is nonsense.  We need to */
    nitf_Error_initf(error, NITF_CTXT, NITF_ERR_INT_STACK_OVERFLOW,
                     "Stack pointing at depth: %d", stack->sp);
    return 2147483647;
}


NITFPROT(int) nitf_IntStack_depth(nitf_IntStack * stack,
                                  nitf_Error * error)
{
    return stack->sp;
}
