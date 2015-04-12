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

#ifndef __TEST_IT_H__
#define __TEST_IT_H__

/*!
 *  \file
 *  \brief Test assertion library
 *
 *  This header provides a test assertion, TEST_IT().
 *  Whenever you think you are certain of a condition or state,
 *  you may test it using the macro, and it will report to you
 *  that your assertion has failed, and where.
 *
 */

void test_it_false_assertion(const char* file,
                             int line,
                             const char* id)
{

    printf("\tError: Test [%s] Failed at (%s, %d)\n", id, file, line);
    exit(EXIT_FAILURE);
}

#define FALSE_ASSERTION(F, L, ID ) \
    test_it_false_assertion(F, L, ID)

#define TEST_IT(TEST) ( (TEST) ? ( printf("\tSuccess: Test [%s] Passed\n", #TEST) ) : (FALSE_ASSERTION(NITF_FILE, NITF_LINE, #TEST) ) )


#endif
