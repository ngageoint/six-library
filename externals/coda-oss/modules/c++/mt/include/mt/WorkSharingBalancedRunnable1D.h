/* =========================================================================
 * This file is part of mt-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2017, MDA Information Systems LLC
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
#ifndef __MT_WORK_SHARING_BALANCED_RUNNABLE_1D_H__
#define __MT_WORK_SHARING_BALANCED_RUNNABLE_1D_H__

#include <vector>
#include <sstream>

#include <sys/Conf.h>
#include <sys/Runnable.h>
#include <sys/AtomicCounter.h>
#include <except/Exception.h>
#include <mt/ThreadPlanner.h>
#include <mt/ThreadGroup.h>
#include <types/Range.h>

namespace mt
{
typedef std::vector<mem::SharedPtr<sys::AtomicCounter> > SharedAtomicCounterVec;

/*!
 *  \class WorkSharingBalancedRunnable1D
 *  \tparam OpT The type of functor that will be used to process elements
 *
 *  This runnable takes both a reference to the associated thread's
 *  atomic counter as well as atomic counter and range information for all
 *  other threads in the thread pool.
 *
 *  Each runnable will operate on a contiguous range of elements
 *  ([startElement, startElement + numElements]). Once all work has been
 *  completed in its range, the atomic counters and terminating indices of
 *  other threads will be used to grab additional work rather than let
 *  the thread die.
 *
 *  This runnable is useful in cases where work needs to be
 *  done across a range of elements, but when dividing these elements
 *  across threads leads to balancing issues i.e certain threads
 *  terminate earlier than other threads. By giving each thread
 *  its own range and atomic counter rather than one shared atomic counter
 *  used to grab elements from a global range, we get better locality of
 *  reference and in practice better caching.
 *
 */
template <typename OpT>
class WorkSharingBalancedRunnable1D : public sys::Runnable
{
public:

    /*!
     *  Constructor
     *
     *  \param range Range of elements for this runnable to work on
     *
     *  \param[in,out] atomicCounter Atomic counter this thread will initially
     *  use to fetch elements within its range to process
     *
     *  \param threadPoolCounters Atomic counters of other threads - if
     *  this thread finishes processing the elements within its range, these
     *  counters will be used to grab additional work
     *
     *  \param threadPoolEndElements Terminating indices (exclusive) for each
     *  thread
     *
     */
    WorkSharingBalancedRunnable1D(
            const types::Range& range,
            sys::AtomicCounter& counter,
            const SharedAtomicCounterVec& threadCounters,
            const std::vector<size_t>& threadPoolEndElements,
            const OpT& op) :
        mStartElement(range.mStartElement),
        mEndElement(mStartElement + range.mNumElements),
        mCounter(counter),
        mThreadPoolCounters(threadCounters),
        mThreadPoolEndElements(threadPoolEndElements),
        mOp(op)
    {
    }

    virtual void run()
    {
        // Operate over this thread's range
        processElements(mCounter, mEndElement);

        // Help other threads that have yet to process every element in
        // their range
        for (size_t ii = 0; ii < mThreadPoolEndElements.size(); ++ii)
        {
            const size_t threadEndElement = mThreadPoolEndElements[ii];
            sys::AtomicCounter& threadCounter = *mThreadPoolCounters[ii];
            processElements(threadCounter, threadEndElement);
        }
    }

private:
    void processElements(sys::AtomicCounter& counter, size_t endElement)
    {
        while (true)
        {
            const size_t element = counter.getThenIncrement();
            if (element < endElement)
            {
                mOp(element);
            }
            else
            {
                break;
            }
        }
    }

    const size_t mStartElement;
    const size_t mEndElement;
    sys::AtomicCounter& mCounter;
    const SharedAtomicCounterVec& mThreadPoolCounters;
    const std::vector<size_t >& mThreadPoolEndElements;
    const OpT& mOp;
};

/*!
 *  This method will divide numElements across numThreads, associating with
 *  each thread a range of elements to work on as well as an atomic counter
 *  used to fetch those elements for processing by the provided functor.
 *
 *  Threads that finish working on their range will use the atomic counters
 *  of other threads to grab additional elements. This behavior prevents
 *  balancing issues from occurring by having certain threads that would
 *  otherwise die grab any available work.
 *
 *  By supplying each thread with its own atomic counter rather than a single
 *  atomic counter shared across threads, each thread can initially operate
 *  over a contiguous range of elements. This behavior provides better locality
 *  of reference and in practice better caching.
 *
 *  \tparam OpT The type of functor that will be used to process elements
 *
 *  \param numElements Number of elements of work
 *  \param numThreads Number of threads
 *  \param op Functor to use
 */
template <typename OpT>
void runWorkSharingBalanced1D(size_t numElements,
                              size_t numThreads,
                              const OpT& op)
{
    std::vector<size_t> threadPoolEndElements;
    SharedAtomicCounterVec threadPoolCounters;
    if (numThreads <= 1)
    {
        threadPoolEndElements.push_back(numElements);
        threadPoolCounters.push_back(
                mem::SharedPtr<sys::AtomicCounter>(
                        new sys::AtomicCounter(0)));

        const types::Range range(0, numElements);
        WorkSharingBalancedRunnable1D<OpT>(range,
                                           *threadPoolCounters[0],
                                           threadPoolCounters,
                                           threadPoolEndElements,
                                           op).run();
    }
    else
    {
        size_t threadNum = 0;
        size_t startElement = 0;
        size_t numElementsThisThread = 0;
        const ThreadPlanner planner(numElements, numThreads);
        std::vector<types::Range> threadPoolRange;
        while (planner.getThreadInfo(
                threadNum++, startElement, numElementsThisThread))
        {
            const types::Range range(startElement, numElementsThisThread);
            threadPoolRange.push_back(range);

            threadPoolCounters.push_back(
                    mem::SharedPtr<sys::AtomicCounter>(
                            new sys::AtomicCounter(startElement)));

            threadPoolEndElements.push_back(
                    startElement + numElementsThisThread);
        }

        ThreadGroup threads;
        for (size_t ii = 0; ii < threadPoolRange.size(); ++ii)
        {
            threads.createThread(
                    new WorkSharingBalancedRunnable1D<OpT>(
                            threadPoolRange[ii],
                            *threadPoolCounters[ii],
                            threadPoolCounters,
                            threadPoolEndElements,
                            op));
        }
        threads.joinAll();
    }
}

/*!
 *  Same as above, but instead of sharing a functor across runnables,
 *  each runnable will receive its own.
 *
 *  \tparam OpT The type of functor that will be used to process elements
 *
 *  \param numElements Number of elements of work
 *  \param numThreads Number of threads
 *  \param ops Vector of functors to use
 */
template <typename OpT>
void runWorkSharingBalanced1D(size_t numElements,
                              size_t numThreads,
                              const std::vector<OpT>& ops)
{
    if (ops.size() != numThreads)
    {
        std::ostringstream ostr;
        ostr << "Got " << numThreads << " threads but " << ops.size()
             << " functors";
        throw except::Exception(Ctxt(ostr.str()));
    }

    std::vector<size_t> threadPoolEndElements;
    SharedAtomicCounterVec threadPoolCounters;
    if (numThreads <= 1)
    {
        threadPoolEndElements.push_back(numElements);
        threadPoolCounters.push_back(
                mem::SharedPtr<sys::AtomicCounter>(
                        new sys::AtomicCounter(0)));

        const types::Range range(0, numElements);
        WorkSharingBalancedRunnable1D<OpT>(range,
                                           *threadPoolCounters[0],
                                           threadPoolCounters,
                                           threadPoolEndElements,
                                           ops[0]).run();
    }
    else
    {
        size_t threadNum = 0;
        size_t startElement = 0;
        size_t numElementsThisThread = 0;
        const ThreadPlanner planner(numElements, numThreads);
        std::vector<types::Range> threadPoolRange;
        while (planner.getThreadInfo(
                  threadNum++, startElement, numElementsThisThread))
        {
              const types::Range range(startElement, numElementsThisThread);
              threadPoolRange.push_back(range);

              threadPoolCounters.push_back(
                      mem::SharedPtr<sys::AtomicCounter>(
                              new sys::AtomicCounter(startElement)));

              threadPoolEndElements.push_back(
                      startElement + numElementsThisThread);
        }

        ThreadGroup threads;
        for (size_t ii = 0; ii < threadPoolRange.size(); ++ii)
        {
            threads.createThread(
                    new WorkSharingBalancedRunnable1D<OpT>(
                            threadPoolRange[ii],
                            *threadPoolCounters[ii],
                            threadPoolCounters,
                            threadPoolEndElements,
                            ops[ii]));
        }
        threads.joinAll();
    }
}

/*!
 *  Convenience wrapper for providing each runnable with a copy of op.
 *  This is useful in cases where each runnable should use a functor with
 *  its own local storage.
 *
 *  \tparam OpT The type of functor that will be used to process elements
 *
 *  \param numElements Number of elements of work
 *  \param numThreads Number of threads
 *  \param op Functor to use
 */
template <typename OpT>
void runWorkSharingBalanced1DWithCopies(size_t numElements,
                                        size_t numThreads,
                                        const OpT& op)
{
    const std::vector<OpT> ops(numThreads, op);
    runWorkSharingBalanced1D(numElements, numThreads, ops);
}
}

#endif
