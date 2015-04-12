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

#include <import/sys.h>

#if defined(_REENTRANT)

using namespace sys;
using namespace std;

class Getter : public sys::Runnable
{
public:
    Getter(sys::Mutex *by, int * val, int n) : theVal(val), syncBy(by), id(n)
    {}
    virtual ~Getter()
    {}

    virtual void run()
    {
        for (int i = 0; i < 250; i++)
        {

            std::cout << "Getter::run: " << std::endl;
            std::cout << typeid(this).name() << std::endl;
            if ( !syncBy->lock() )
            {
                cout << "Err: " << sys::Err().toString() << endl;
                assert(0);
            }
            int x = get();
            cout << "Thread id: "<< id << " got back " << x << endl;
            if ( !syncBy->unlock() )
            {
                cout << "Err: " << sys::Err().toString() << endl;
                assert(0);
            }
            sys::Thread::yield();
        }
    }
    int get()
    {

        return *theVal;
    }
protected:
    int *theVal;
    sys::Mutex *syncBy;
    int id;

};
class Putter : public sys::Runnable
{
public:
    Putter(sys::Mutex *by,int *val, int n) : theVal(val), syncBy(by), id(n)
    {}
    virtual ~Putter()
    {}

    virtual void run()
    {

        std::cout << "Putter::run: " << std::endl;
        std::cout << typeid(this).name() << std::endl;

        for (int i = 0; i < 250; i++)
        {
            if ( !syncBy->lock() )
            {
                cout << "Err: " << sys::Err().toString() << endl;
                assert(0);
            }

            set(i);
            cout << "Thread id: "<< id << " set to " << i << endl;
            if ( !syncBy->unlock() )
            {
                cout << "Err: " << sys::Err().toString() << endl;
                assert(0);
            }

            sys::Thread::yield();

        }

    }
    void set(int val)
    {
        *theVal = val;
    }
protected:
    int *theVal;
    sys::Mutex *syncBy;
    int id;
};

int main()
{
    try
    {
        int val = 24;
        sys::Mutex syncBy;
        sys::Thread *gT[5];
        sys::Thread *pT[5];

        for (int i = 0; i < 5; i++)
        {

            gT[i] = new sys::Thread(new Getter(&syncBy, &val, i));
            gT[i]->start();

            pT[i] = new sys::Thread(new Putter(&syncBy, &val, i));
            pT[i]->start();

            // 	    //printf("p (&) %x\n", p);
            // 	    sys::Thread(p).start();
            // 	    sys::Thread(new Putter(&syncBy, &val, i)).start();
        }

        for (int i = 0; i < 5; i++)
        {
            gT[i]->join();
            cout << "Joined on gT[" << i << "]" << endl;
            delete gT[i];
            pT[i]->join();
            delete pT[i];
            cout << "Joined on pT[" << i << "]" << endl;
        }
        //	sys::Thread::yield();

    }
    catch (except::Exception& e)
    {
        cout << "Caught Exception: " << e.toString() << endl;
    }
    catch (...)
    {
        cout << "Unknown exception" << endl;
    }
    return 0;
};

#else

int main()
{
    std::cout << "sys is not Multithreaded" << std::endl;
    return 0;
}

#endif
