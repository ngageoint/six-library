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

#ifndef __SYS_EXEC_PIPE_H__
#define __SYS_EXEC_PIPE_H__

#include <stdlib.h>
#include <import/except.h>
#include <str/Convert.h>

#include "sys/Runnable.h"
#include "sys/Err.h"
#include "sys/ProcessInterface.h"

#ifdef _WIN32
#define popen _popen
#define pclose _pclose
#define fileno _fileno
#endif


namespace sys
{

/*!
 *  \class Exec
 *  \brief A good, old-fashioned exec call
 *
 *  This Exec class defines an API for a system-independent
 *  exec method.  The fact that it inherits the Runnable interface
 *  allows us to use it from within a Process to create a
 *  'system()'-like functionality.
 */
class Exec : public sys::Runnable
{
public:
    /*!
     *  Execute a command on the run() function
     *  \param cmd The command to exec()
     */
    Exec( const std::string& cmd ) :
        mCmd(cmd)
    {
    }

    //!  Destructor
    virtual ~Exec()
    {}

    /*!
     *  Execute a command
     */
    virtual void run() 
    { 
        if (::system(mCmd.c_str()) == -1)
        {
            sys::Err err;
            throw except::IOException(
                Ctxt("Unable to run system command: " + err.toString()));
        }
    }

protected:
    //!  A command
    std::string mCmd;
};

/*!
 *  \class ExecPipe
 *  \brief opens a child process and connects a pipe
 *         to read back the std::cout
 */
class ExecPipe : Exec
{

public:

    /*!
    *  Constructor --
    *  Kicks off child process and connects a pipe to the std::cout
    *
    *  \param cmd           - command line string to run
    */
    ExecPipe(const std::string& cmd) : 
        Exec(cmd),
        mOutStream(NULL)
    {
    }

    //! start the child process and connect the pipe
    virtual void run() 
    {
        mOutStream = popen(mCmd.c_str(), "r");
        if (mOutStream == NULL)
        {
            sys::Err err;
            throw except::IOException(
                Ctxt("Unable to open stream: " + err.toString()));
        }
    }

    //! cleanup the stream if not done already
    ~ExecPipe()
    {
        if (mOutStream)
        {
            try 
            {
                closePipe();
            }
            catch (...)
            {
            }
        }
    }
    
    //! make available the pipe
    virtual const FILE* getPipe() const { return mOutStream; }

    //! closes the stream connected to the child process --
    //! platform specific implementation 
    int closePipe();

protected:

    FILE* mOutStream;

private:

    //! Noncopyable
    ExecPipe(const ExecPipe& );
    const ExecPipe& operator=(const ExecPipe& );
};

}

#endif
