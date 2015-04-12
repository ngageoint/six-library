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


#if defined(__APPLE_CC__)
#include <iostream>
int main (int argc, char *argv[])
{
    std::cout << "Sorry no semaphores" << std::endl;
    return 0;
}

#else

#include <sstream>
#include <iostream>
#include "sys/Thread.h"
#include "sys/ReadWriteMutex.h"

using namespace sys;
const int MAX_THREADS = 10;

ReadWriteMutex rwLock(10);
Mutex mutexCout;

std::string itos(int arg)
{
    std::ostringstream os;
    os << arg;
    return os.str();
}

std::string BUFFER[1];

class ReadWriteThread : public Thread
{
protected:
   void run() {
        
         for(int i=0; i < 5; ++i)
         {
             rwLock.lockWrite();
             BUFFER[0] = mName.substr(7);
             mutexCout.lock();
             std::cout << mName << " write " << BUFFER[0] << std::endl;
             mutexCout.unlock();
             sleep(1);
             rwLock.unlockWrite();
         }
        
         
         rwLock.lockRead();
         std::string b = BUFFER[0];
         for(int i=0; i < 5; ++i)
         {
           assert(b == BUFFER[0]);
           b = BUFFER[0];
           mutexCout.lock();
	   std::cout << mName << " read " << b << std::endl;
           mutexCout.unlock();
           sleep(1);
         }
         rwLock.unlockRead();
   }

public:
   ReadWriteThread(std::string name) : Thread(name) {};
};

int main (int argc, char* argv[])
{
   Thread *threads[MAX_THREADS];
   int numWriters(0);
   for (int i = 0 ; i < MAX_THREADS ; i++ ) {
      threads[i] = new ReadWriteThread( "Thread " + itos(i) );
      threads[i]->start();
   }
   for (int j = 0; j < MAX_THREADS; j++ ) {
      threads[j]->join();
   }
}
#endif
