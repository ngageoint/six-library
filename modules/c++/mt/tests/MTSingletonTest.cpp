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
#if defined(__APPLE_CC__)
#include <iostream>
int main (int argc, char *argv[])
{
    std::cout << "Sorry no semaphores" << std::endl;
    return 0;
}

#else

#include <iostream>
#include <import/except.h>
#include <import/sys.h>
#include <import/mt.h>

using namespace except;
using namespace sys;
using namespace std;
using namespace mt;

class StopsWhenEmpty : public GenericRequestHandler
{
public:
    StopsWhenEmpty(RunnableRequestQueue* request) :
        GenericRequestHandler(request)
    {
    }

    ~StopsWhenEmpty()
    {
    }

    void run()
    {
        while (true)
        {

            if (mRequest->isEmpty())
            {
                cout << "exiting run()" << endl;
                return;
            }

            // Pull a runnable off the queue
            Runnable *handler;
            mRequest->dequeue(handler);

            // Run the runnable that we pulled off the queue
            handler->run();

            // Delete the runnable we pulled off the queue
            delete handler;

        }
    }
};

/*class ChangeRequest : public Runnable
 {
 public:
 ChangeRequest(HandlerInterface* newHandler) : mNH(newHandler) {}
 ~ChangeRequest() {}
 void run()
 {
 StatusHandler().changeHandler(mNH);
 //__status__("Changed the singleton handler");
 }
 protected:
 HandlerInterface *mNH;

 };*/

class Thespian : public Runnable
{
public:
    Thespian(const char* name) :
        mName(name)
    {
    }
    virtual ~Thespian()
    {
    }
    void run()
    {
        //__debugln__(FmtX("[%s]:", mName.c_str()));
        doLines();
    }
    virtual void doLines() = 0;
protected:
    std::string mName;
};

class Soothsayer : public Thespian
{
public:
    Soothsayer() :
        Thespian("Soothsayer")
    {
    }
    ~Soothsayer()
    {
    }

    void doLines()
    {
        //__warning__("Beware the ides of March.");
    }
};

class Calpurnia : public Thespian
{
public:
    Calpurnia() :
        Thespian("Calpurnia")
    {
    }
    ~Calpurnia()
    {
    }
    void doLines()
    {
        //__warning__("Caesar, I never stood on ceremonies,   Yet now they fright me. There is one within, 15  Besides the things that we have heard and seen,   Recounts most horrid sights seen by the watch.   A lioness hath whelped in the streets;   And graves have yawn'd, and yielded up their dead;   Fierce fiery warriors fought upon the clouds, 20  In ranks and squadrons and right form of war,   Which drizzled blood upon the Capitol;   The noise of battle hurtled in the air,   Horses did neigh, and dying men did groan,   And ghosts did shriek and squeal about the streets. 25  O Caesar! these things are beyond all use,   And I do fear them.");
    }

};
class Caesar : public Thespian
{
public:
    Caesar() :
        Thespian("Caesar")
    {
    }
    ~Caesar()
    {
    }
    void doLines()
    {
        //__status__("Cowards die many times before their deaths;   The valiant never taste of death but once. 35  Of all the wonders that I yet have heard.   It seems to me most strange that men should fear;   Seeing that death, a necessary end,   Will come when it will come.");
    }

};

class Brutus : public Thespian
{
public:
    Brutus() :
        Thespian("Brutus")
    {
    }
    virtual ~Brutus()
    {
    }

    virtual void doLines()
    {
        //__status__("Be patient till the last.   Romans, countrymen, and lovers! hear me for my 15  cause, and be silent, that you may hear: believe me   for mine honour, and have respect to mine honour, that   you may believe: censure me in your wisdom, and   awake your senses, that you may the better judge.   If there be any in this assembly, any dear friend of 20  Caesar's, to him I say, that Brutus' love to Caesar   was no less than his. If then that friend demand   why Brutus rose against Caesar, this is my answer:   --Not that I loved Caesar less, but that I loved   Rome more. Had you rather Caesar were living and 25  die all slaves, than that Caesar were dead, to live   all free men? As Caesar loved me, I weep for him;   as he was fortunate, I rejoice at it; as he was   valiant, I honour him: but, as he was ambitious, I   slew him. There is tears for his love; joy for his 30  fortune; honour for his valour; and death for his   ambition. Who is here so base that would be a   bondman? If any, speak; for him have I offended.   Who is here so rude that would not be a Roman? If   any, speak; for him have I offended. Who is here so 35  vile that will not love his country? If any, speak;   for him have I offended. I pause for a reply.");

    }
};

class Antony : public Thespian
{
public:
    Antony() :
        Thespian("Antony")
    {
    }
    ~Antony()
    {
    }

    void doLines()
    {
        //__status__("Villains, you did not so, when your vile daggers   Hack'd one another in the sides of Caesar:   You show'd your teeth like apes, and fawn'd like hounds, 45  And bow'd like bondmen, kissing Caesar's feet;   Whilst damned Casca, like a cur, behind   Struck Caesar on the neck. O you flatterers!");
    }
};

class Octavius : public Thespian
{
public:
    Octavius() :
        Thespian("Ocativius")
    {
    }
    ~Octavius()
    {
    }

    void doLines()
    {
        //__status__("According to his virtue let us use him,   With all respect and rites of burial.   Within my tent his bones to-night shall lie, 85  Most like a soldier, order'd honourably.   So call the field to rest; and let's away,   To part the glories of this happy day.");
    }
};

int main(int argc, char *argv[])
{
    try
    {
        BasicThreadPool<StopsWhenEmpty>pool(2);

        //         pool.addRequest(new ChangeRequest(
        //                         new LogExHandler(
        //                         new StandardErrStream()
        // 			)
        // 			)
        // 			);
        pool.addRequest(new Soothsayer());
        pool.addRequest(new Calpurnia());

        //  	pool.addRequest(new ChangeRequest(new LogExHandler(new StandardOutStream())));

        pool.addRequest(new Caesar());
        pool.addRequest(new Brutus());
        pool.addRequest(new Antony());
        pool.addRequest(new Octavius());

        pool.start();
        pool.join();
    }
    catch (except::Throwable& t)
    {
        cout << "Exception Caught: " << t.toString() << endl;
    }
    catch (...)
    {
        cout << "Exception Caught!" << endl;
        return -1;
    }

    return 0;
}

#endif
