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

#include <algorithm>

namespace mt
{
    class ThreadPlanner
    {
    private:
       size_t mNumElements;
       size_t mNumThreads;
       size_t mNumElementsPerThread;
    public:
        ThreadPlanner(size_t numElements, size_t numThreads) : 
            mNumElements(numElements), mNumThreads(numThreads) 
        {
            mNumElementsPerThread = (mNumElements / mNumThreads) + (mNumElements % mNumThreads != 0);
        }

        size_t getNumElementsPerThread() const
        {
            return mNumElementsPerThread;
        }

        bool getThreadInfo(size_t threadNum, size_t& startElement, size_t& numElementsThisThread) const
        {
            startElement = threadNum * mNumElementsPerThread;
            if(startElement > mNumElements)
            {
                numElementsThisThread = 0;
            }
            else
            {
                size_t numElementsRemaining = mNumElements - startElement; 
                numElementsThisThread = std::min<size_t>(mNumElementsPerThread, numElementsRemaining);
            }
            return (numElementsThisThread != 0);
        }
    };
}

#endif

