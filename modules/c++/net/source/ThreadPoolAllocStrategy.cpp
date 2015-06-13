#include "net/ThreadPoolAllocStrategy.h"

using namespace net;

ThreadPoolAllocStrategy::~ThreadPoolAllocStrategy()
{
    if (mPool)
        delete mPool;
}
void ThreadPoolAllocStrategy::initialize()
{
    mPool = new ConnectionThreadPool(mNumThreads, mRequestHandlerFactory);
    mPool->start();
}
void ThreadPoolAllocStrategy::handleConnection(NetConnection* conn)
{
    mPool->addRequest(conn);
}
