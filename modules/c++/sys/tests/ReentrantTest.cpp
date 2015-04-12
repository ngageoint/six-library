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

#include <iostream>
#include <import/sys.h>
using namespace sys;

#if defined(_REENTRANT)
class NoteThread : public Thread
{
public:
    NoteThread()
    {   std::cout << "Constructed a thread" << std::endl;}
    void run()
    {   std::cout << "Running a thread" << std::endl;}

    ~NoteThread()
    {   std::cout << "Completed a thread. Destroying..." << std::endl;}

};
#endif
int main()
{
#if defined(_REENTRANT)
    try
    {
        std::cout << "Your implementation of sys is multi-threaded" << std::endl;
        NoteThread* nThr = new NoteThread;
        std::cout << "Your thread type is: " << nThr->getNativeType() << std::endl;

        nThr->start();
        nThr->join();
        delete nThr;
        std::cout << "Exiting" << std::endl;
    }

    catch (except::Throwable& t)
    {
        std::cout << "Caught throwable: " << t.toString() << std::endl;

    }
    catch (std::exception& e)
    {
        std::cout << "Caught stl exception: " << e.what() << std::endl;
    }
    catch (...)
    {
        std::cout << "Caught unknown exception" << std::endl;
    }
#else

    std::cout << "Your implementation of sys is single-threaded" << std::endl;
#endif
    return 0;

}

