/* =========================================================================
 * This file is part of sys-c++ 
 * =========================================================================
 * 
 * (C) Copyright 2004 - 2009, General Dynamics - Advanced Information Systems
 *
 * sys-c++ is free software; you can redistribute it and/or modify
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


#include <sstream>
#include <iostream>
#include "sys/Thread.h"
#include "sys/Mutex.h"


#if defined(_REENTRANT)

using namespace sys;
const int MAX_THREADS = 9;
const int THREAD_LOOPS = 3;

Mutex         mutexCout;

std::string itos (int arg) {
   std::ostringstream buffer;
   buffer << arg;
   return buffer.str();
}

class DemoThread : public Thread
{
protected:
   void run() {
      for (int i = 0 ; i < THREAD_LOOPS ; i++ ) {
         mutexCout.lock();
         std::cout << mName + " is running" << std::endl;
         mutexCout.unlock();
         sleep(1);
      }
      mutexCout.lock();
      std::cout << mName + " is finished" << std::endl;
      mutexCout.unlock();
   }

public:
   DemoThread(std::string name) : Thread(name) {};
};


int main (int argc, char* argv[])
{
   DemoThread *threads[MAX_THREADS];
   for (int i = 0 ; i < MAX_THREADS ; i++ ) {
      threads[i] = new DemoThread( "Thread " + itos(i+1) );
      threads[i]->start();
   }
   for (int j = 0; j < MAX_THREADS; j++ ) {
      threads[j]->join();
   }
}
#else
int main()
{
	std::cout << "Sorry, your implementation of sys is not thread-enabled" << std::endl;
	return 0;
}
#endif


