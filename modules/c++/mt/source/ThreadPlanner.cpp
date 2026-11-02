/* =========================================================================
 * This file is part of mt-c++ 
 * =========================================================================
 * 
 * © 2016 MDA Information Systems LLC
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
#include <algorithm>

#include <math/Round.h>
#include <mt/ThreadPlanner.h>

namespace mt
{
ThreadPlanner::ThreadPlanner(size_t numElements, size_t numThreads) :
    mNumElements(numElements),
    mNumThreads(numThreads)
{
    // If we got lucky and the work divides up evenly, every thread simply
    // gets numElements / numThreads elements of work
    // If there are leftovers, add one more to the elements per thread value
    // What this will amount to meaning is that early threads will end up with
    // one more piece of work than later threads.
    mNumElementsPerThread = math::ceilingDivide(mNumElements, mNumThreads);
}

bool ThreadPlanner::getThreadInfo(size_t threadNum,
                                  size_t& startElement,
                                  size_t& numElementsThisThread) const
{
    startElement = threadNum * mNumElementsPerThread;
    if(startElement > mNumElements)
    {
        numElementsThisThread = 0;
    }
    else
    {
        size_t numElementsRemaining = mNumElements - startElement;
        numElementsThisThread =
                std::min(mNumElementsPerThread, numElementsRemaining);
    }
    return (numElementsThisThread != 0);
}

size_t ThreadPlanner::getNumThreadsThatWillBeUsed() const
{
    if (mNumElementsPerThread == 0)
    {
        return 0;
    }
    else
    {
        const size_t numThreads =
                math::ceilingDivide(mNumElements, mNumElementsPerThread);

        return numThreads;
    }
}
}
