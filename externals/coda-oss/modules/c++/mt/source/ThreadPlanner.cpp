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
