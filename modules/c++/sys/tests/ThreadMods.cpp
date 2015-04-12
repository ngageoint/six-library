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

/* ==================== DO NOT EDIT THIS HEADER ======================== 
 * 
 * $RCSfile: ThreadMods.cpp,v $
 * $Revision: 1.5 $
 * $Name: not supported by cvs2svn $
 * $Date: 2004-08-31 02:13:09 $
 * $Author: dpressel $
 * 
 * Copyright (c) 2004 General Dynamics - AIS
 * Ann Arbor, MI. 
 * 
 * All Rights Reserved 
 * 
 * THIS IS UNPUBLISHED COPYRIGHTED SOURCE CODE containing
 * proprietary information of General Dynamics - AIS
 * This copy has been provided on the basis of strict
 * confidentiality and on the express understanding that
 * it may not be reproduced, or revealed to any person,
 * in whole or in part, without express written permission
 * from General Dynamics - AIS, which is sole owner of
 * copyright and all other rights therein.
 * 
 * $Log: not supported by cvs2svn $
 * Revision 1.2  2004/02/24 13:10:42  dpressel
 * Took 1.1 revision out of banner
 * 
 * 
 * ==================== DO NOT EDIT THIS HEADER ======================== */
/* ==================== DO NOT EDIT THIS HEADER ======================== 
 * 
 * $RCSfile: ThreadMods.cpp,v $
 * $Revision: 1.5 $
 * $Name: not supported by cvs2svn $
 * $Date: 2004-08-31 02:13:09 $
 * $Author: dpressel $
 * 
 * Copyright (c) 2004 General Dynamics - AIS
 * Ann Arbor, MI. 
 * 
 * All Rights Reserved 
 * 
 * THIS IS UNPUBLISHED COPYRIGHTED SOURCE CODE containing
 * proprietary information of General Dynamics - AIS
 * This copy has been provided on the basis of strict
 * confidentiality and on the express understanding that
 * it may not be reproduced, or revealed to any person,
 * in whole or in part, without express written permission
 * from General Dynamics - AIS, which is sole owner of
 * copyright and all other rights therein.
 * 
 * $Log: not supported by cvs2svn $
 * Revision 1.4  2004/08/31 01:06:03  dpressel
 * Added failure case
 *
 * Revision 1.3  2004/08/31 00:58:25  dpressel
 * Added louder thread
 *
 * Revision 1.2  2004/08/31 00:53:17  dpressel
 * Mods to mods
 *
 * Revision 1.2  2004/02/24 13:10:42  dpressel
 * Took 1.1 revision out of banner
 * 
 * 
 * ==================== DO NOT EDIT THIS HEADER ======================== */

/* Okay, here is the problem
 *
 *  Once I do a copy, my runnable target is copied over, 
 *  but Im supposed to be pointing at myself.
 *  So if I make T1 at address 0xbf, it then points at itself
 *  with the runnable, and so I have
 *
 *  0xbf: [T1] -> mTarget = 0xbf
 *  But if I copy T1 to T2...
 *  0xbc: [T2] -> mTarget = T1.mTarget = 0xbf
 *      -- I really want (in this case) mTarget pointing at T2's address
 *  Now I have a shallow copy which is very dangerous
 *
 *  This can be verified by applying the following method:
 *
 * 	ThreadPosix(const ThreadPosix& p) 
 {
 std::cout << "Copying constructing thread" << std::endl;
 mName      = p.mName;
 mPriority  = p.mPriority;
 mTarget    = this;
 printf("mTarget = %p\n", mTarget);
 printf("p.mTarget = %p\n", p.mTarget);
 mIsRunning = p.mIsRunning;
 }
 *  Note that instead of assigning mTarget to p.mTarget, I have assigned
 *  it to this.  Note further that this may be highly undesirable behavior,
 *  if a thread is attempting to run a child.  In that case it should be
 *  mTarget = p.mTarget;
 */
#include <iostream>
#include <import/sys.h>
using namespace sys;

// Works fine on the linux box
// Okay, since adding the vector, it fails on the linux box

#if defined(_REENTRANT)

class LoudThread : public Thread
{
public:
    LoudThread()
    {   std::cout << "Constructed a thread" << std::endl;}
    void run()
    {   std::cout << "Running a thread" << std::endl;}

    ~LoudThread()
    {   std::cout << "Completed a thread. Destroying..." << std::endl;}

};
class LouderThread : public LoudThread
{
public:
    LouderThread()
    {
        std::cout << "HEY!!! PAY ATTENTION TO ME!!!!" << std::endl;
    }
    void run()
    {   std::cout << "RUNNING A THREAD, LOOK!!!" << std::endl;}

    ~LouderThread()
    {   std::cout << "I'M DONE, WATCH!!! DESTROYING..." << std::endl;}

};

#endif
int main(int argc, char** argv)
{
#if defined(_REENTRANT)
    try
    {
        if (argc != 2)
        die_printf("Usage: %s <num-threads>\n", argv[0]);

        int numThr = atoi(argv[1]);
        if ( numThr < 1 ) numThr = 1;
        if ( numThr > 8) numThr = 8;
        std::vector<LoudThread> lThrVec;
        for (int i = 0; i < numThr; i++)
        {
            std::cout << ">>Added thread: " << i << std::endl;

            lThrVec.push_back(LoudThread());
        }

        for (int i = 0; i < numThr; i++)
        {
            std::cout << ">>Starting thread: " << i << std::endl;
            lThrVec[i].start();
        }

        for (int i = 0; i < numThr; i++)
        {
            lThrVec[i].join();
            std::cout << ">>Joining on thread: " << i << std::endl;
        }
        std::cout << ">>Exiting" << std::endl;
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

