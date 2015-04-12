/* =========================================================================
 * This file is part of mt-c++ 
 * =========================================================================
 * 
 * (C) Copyright 2004 - 2009, General Dynamics - Advanced Information Systems
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


#include "mt/GenerationThreadPool.h"
#if !defined(__APPLE_CC__)

mt::TiedRequestHandler::~TiedRequestHandler() 
{
    if (mAffinityInit)
    {
	delete mAffinityInit;
	mAffinityInit = NULL;
    }
    
}

// If we have a thread initializer, tie down our handler to a CPU
void mt::TiedRequestHandler::initialize()
{
    if (mAffinityInit)
    {
	mAffinityInit->initialize();
    }
}
void mt::TiedRequestHandler::run()
{
    // Call our init (gets called from within the thread package's create fn)
    initialize();

    while (true)
    {   
	// Pull a runnable off the queue
	sys::Runnable *handler = NULL;
	
	mRequestQueue->dequeue(handler);
	if (!handler) return;
	
	// Run the runnable that we pulled off the queue
	handler->run();
	
	// Delete the runnable we pulled off the queue
	delete handler;
	
	// Signal to the thread pool that we are done
	// This will allow 1 wait() to complete
	while( !mSem->signal() );
    }
}

// Not set up for multiple producers 
void mt::GenerationThreadPool::addGroup(std::vector<sys::Runnable*>& toRun)
{
    
    if (mGenSize)
	throw mt::ThreadPoolException(Ctxt("The previous generation has not completed!"));
    
    mGenSize = toRun.size();
    for (int i = 0; i < mGenSize; ++i)
	addRequest(toRun[i]);
}

// Not set up for multiple producers 
void mt::GenerationThreadPool::waitGroup()
{
    while (mGenSize)
    {
	while(!mGenerationSync.wait());
	//std::cout << "Waited" << std::endl;
	--mGenSize;
    }
}

void mt::GenerationThreadPool::shutdown()
{
    size_t size = mPool.size();
    static sys::Runnable* shutdown = NULL;
    for(unsigned int i=0; i < size; i++)
    {
	addRequest(shutdown);
    }
}
#endif

