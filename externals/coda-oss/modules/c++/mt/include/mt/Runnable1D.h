#ifndef __MT_RUNNABLE_1D_H__
#define __MT_RUNNABLE_1D_H__

#include <vector>
#include <sstream>

#include <sys/Conf.h>
#include <sys/Runnable.h>
#include <except/Exception.h>
#include "mt/ThreadPlanner.h"
#include "mt/ThreadGroup.h"

namespace mt
{
template <typename OpT>
class Runnable1D : public sys::Runnable
{
public:
    Runnable1D(size_t startElement,
               size_t numElements,
               const OpT& op) :
        mStartElement(startElement),
        mEndElement(startElement + numElements),
        mOp(op)
    {
    }

    virtual void run() override
    {
        for (size_t ii = mStartElement; ii < mEndElement; ++ii)
        {
            mOp(ii);
        }
    }

private:
    const size_t mStartElement;
    const size_t mEndElement;
    const OpT& mOp;
};

template <typename OpT>
void run1D(size_t numElements, size_t numThreads, const OpT& op)
{
    if (numThreads <= 1)
    {
        Runnable1D<OpT>(0, numElements, op).run();
    }
    else
    {
        ThreadGroup threads;
        const ThreadPlanner planner(numElements, numThreads);
 
        size_t threadNum(0);
        size_t startElement(0);
        size_t numElementsThisThread(0);
        while(planner.getThreadInfo(threadNum++, startElement, numElementsThisThread))
        {
            threads.createThread(new Runnable1D<OpT>(
                startElement, numElementsThisThread, op));
        }
        threads.joinAll();
    }
}

// Same as above but each thread gets their own 'op'
// This is useful when each thread needs its own local storage and/or you
// need access to a per-thread result afterwards (make these member variables
// mutable since operator() is const).
template <typename OpT>
void run1D(size_t numElements, size_t numThreads, const std::vector<OpT>& ops)
{
    if (ops.size() != numThreads)
    {
        std::ostringstream ostr;
        ostr << "Got " << numThreads << " threads but " << ops.size()
             << " functors";
        throw except::Exception(Ctxt(ostr.str()));
    }

    if (numThreads <= 1)
    {
        Runnable1D<OpT>(0, numElements, ops[0]).run();
    }
    else
    {
        ThreadGroup threads;
        const ThreadPlanner planner(numElements, numThreads);

        size_t threadNum(0);
        size_t startElement(0);
        size_t numElementsThisThread(0);
        while(planner.getThreadInfo(threadNum, startElement, numElementsThisThread))
        {
            threads.createThread(new Runnable1D<OpT>(
                startElement, numElementsThisThread, ops[threadNum++]));
        }
        threads.joinAll();
    }
}

// Same as above but each thread gets their own copy-constructed copy of 'op'
// This is useful when each thread needs its own local storage (make this
// scratch space mutable since operator() is const).
template <typename OpT>
void run1DWithCopies(size_t numElements, size_t numThreads, const OpT& op)
{
    const std::vector<OpT> ops(numThreads, op);
    run1D(numElements, numThreads, ops);
}
}

#endif
