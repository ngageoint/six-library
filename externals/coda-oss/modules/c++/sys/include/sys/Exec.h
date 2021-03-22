/* =========================================================================
 * This file is part of sys-c++ 
 * =========================================================================
 * 
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
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
struct ExecPipe : Exec
{
    /*!
    *  Constructor --
    *  Kicks off child process and connects a pipe to the std::cout
    *
    *  \param cmd           - command line string to run
    */
    ExecPipe(const std::string& cmd) : 
        Exec(cmd),
        mOutStream(nullptr)
    {
    }

    //! start the child process and connect the pipe
    virtual void run() 
    {
        mOutStream = openPipe(mCmd, "r");
        if (mOutStream == nullptr)
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
                killProcess();
            }
            catch (...)
            {
            }
        }
    }

    //! make available the pipe
    const FILE* getPipe() const { return mOutStream; }

    //! make available the pipe
    FILE* getPipe() { return mOutStream; }

    //! closes the stream connected to the child process --
    //  this is a blocking call until the process is complete
    int closePipe();

    ExecPipe(const ExecPipe&) = delete;
    ExecPipe& operator=(const ExecPipe&) = delete;

protected:

#ifdef _WIN32
    STARTUPINFO mStartInfo;
    PROCESS_INFORMATION mProcessInfo;
#else
    pid_t mProcess;
#endif

    FILE* mOutStream;

    //! popen with user access to process id
    FILE* openPipe(const std::string& command,
                   const std::string& type);

    //! forcefully kill the process and call closePipe
    int killProcess();
};

}

#endif
