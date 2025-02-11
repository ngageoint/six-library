#ifndef __NET_THREAD_ALLOC_STRATEGY_H__
#define __NET_THREAD_ALLOC_STRATEGY_H__

#include <import/mt.h>
#include "net/NetConnection.h"
#include "net/AllocStrategy.h"
#include "net/RequestHandler.h"

namespace net
{

/*!
 *  \class ConnectionThread
 *  \brief Worker thread satisfies net connections
 *
 *  This class is a very simple WorkerThread that handles
 *  incoming request messages.  Due to the orthogonality
 *  between AllocStrategy and RequestHandler, this class
 *  doesnt have to do much at all, it just calls the
 *  RequestHandler's operator().
 *
 *  Inheriting WorkerThread means that all boilerplate set-up
 *  is done for us, so we just have to override performTask.
 *  Also note that, since we are in a server, we will never shut down
 *
 */
class ConnectionThread: public mt::WorkerThread<NetConnection*>
{
    RequestHandler* mHandler;
public:
    //! Each thread gets 1 unique request handler
    ConnectionThread(mt::RequestQueue<NetConnection*>* connQueue,
            net::RequestHandler* handler) :
        mt::WorkerThread<NetConnection*>(connQueue), mHandler(handler)
    {
    }

    //! Even though ownership doesnt mean much at this point, delete ours
    ~ConnectionThread()
    {
        delete mHandler;
    }

    /*!
     *  Do this in a loop forever.
     */
    void performTask(net::NetConnection*& request)
    {
        (*mHandler)(request);
    }
};

/*!
 *  \class ConnectionThreadPool
 *  \brief Thread pool that creates ConnectionThread objects
 *
 *  Class implements abstract methods of AbstractThreadPool.
 *  It owns a factory that is used to initialize all of its
 *  WorkerThread instances.
 *
 *  Each ConnectionThread is guaranteed a unique RequestHandler,
 *  and the RequestHandler implementations are a nod to this,
 *  recognizing that all resources are safe within this thread
 */
class ConnectionThreadPool: public mt::AbstractThreadPool<net::NetConnection*>
{
    RequestHandlerFactory* mFactory;

public:
    ConnectionThreadPool(unsigned short numThreads,
            net::RequestHandlerFactory* factory) :
        mt::AbstractThreadPool<net::NetConnection*>(numThreads), mFactory(
                factory)
    {
    }
    ~ConnectionThreadPool()
    {
        delete mFactory;
    }

    mt::WorkerThread<net::NetConnection*>* newWorker()
    {
        return new ConnectionThread(&mRequestQueue, mFactory->create());
    }
};

/*!
 *  \class ThreadPoolAllocStrategy
 *  \brief Thread pool-backed AllocStrategy
 *
 *  The net package presents the allocation strategy as orthogonal
 *  to the handling.  This methodology makes sense, since a handler's
 *  job is to process a connection, and nothing more.  The strategy
 *  classes are concerned with allocation of resources across threads.
 *
 *  Therefore, each AllocStrategy must have a handle to the
 *  RequestHandlerFactory that will be producing RequestHandler's on
 *  its behalf.  However, maintaining good Separation of Concerns,
 *  the AllocStrategy is not required to know any details of how
 *  a request handler is manufactured -- it just produces handlers when
 *  the time is right.
 *
 *  The ThreadPoolAllocStrategy is very simple -- it contains a
 *  thread pool of workers that process connections.  Each worker gets
 *  its own RequestHandler from the owned mRequestHandlerFactory.
 *
 *  Whenever a handleConnection() message is received from the passive
 *  connection, it is placed onto the thread pool's consumer-producer
 *  buffer.
 *
 *  Then, when a worker/consumer is ready to process the connection, it
 *  picks it up from the queue and hands it to its RequestHandler
 *
 */
class ThreadPoolAllocStrategy: public AllocStrategy
{

    ConnectionThreadPool* mPool;
    unsigned short mNumThreads;
public:
    ThreadPoolAllocStrategy(unsigned short numThreads) :
        mPool(nullptr), mNumThreads(numThreads)
    {
    }

    ~ThreadPoolAllocStrategy();

    // AllocStrategy guarantees that mRequestHandlerFactory is initialized
    // by the time this function is called
    void initialize();

    void handleConnection(net::NetConnection* conn);

};
}

#endif
