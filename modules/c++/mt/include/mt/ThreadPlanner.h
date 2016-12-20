/* =========================================================================
 * This file is part of mt-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
 *
 * mt-c++ is free software; you can redistribute it and/or modify
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
 * License along with this program; If not,
 * see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef __MT_THREAD_PLANNER_H__
#define __MT_THREAD_PLANNER_H__

#include <stddef.h>

namespace mt
{
/*!
 * \class ThreadPlanner
 * \brief Assists with dividing up work evenly between threads
 */
class ThreadPlanner
{
public:
    /*!
     * Constructor
     *
     * \param numElements The total number of elements of work to be divided
     * among threads
     * \param numThreads The number of threads that will be used for the work
     */
    ThreadPlanner(size_t numElements, size_t numThreads);

    /*!
     * \return The number of elements each thread will work on (this is true
     * for the early threads.  If the work doesn't divide evenly, later
     * threads may have fewer elements or, in the extreme, no work at all.)
     */
    size_t getNumElementsPerThread() const
    {
        return mNumElementsPerThread;
    }

    /*!
     * Provides the start element and number of elements that 0-based thread
     * 'threadNum' should operate on
     *
     * \param threadNum The thread number
     * \param startElement Provides the start element this thread should
     * operate on
     * \param numEementsThisThread Provides the number of elements of work
     * this thread should operate on
     *
     * \return True if this thread has work to do, false otherwise.  Note that
     * the way work is divided among threads, it's possible that the last
     * thread will have no work at all.  This will occur for example with
     * 100 elements and 11 threads - the first 10 threads will have 10
     * elements of work and the last thread will have no work.  We could have
     * instead given all threads 9 elements and one thread 10 elements but
     * this would not have sped up the calculation since we'd still have to
     * wait for one thread to operate on 10 elements.
     */
    bool getThreadInfo(size_t threadNum,
                       size_t& startElement,
                       size_t& numElementsThisThread) const;

    /*!
     * Provides the number of threads that will actually be used.  For example,
     * with 100 elements and 11 threads, we will actually only use 10 threads.
     *
     * \return The number of threads that will be used
     */
    size_t getNumThreadsThatWillBeUsed() const;

private:
   size_t mNumElements;
   size_t mNumThreads;
   size_t mNumElementsPerThread;
};
}

#endif

