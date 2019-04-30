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


#if !defined(WIN32)

#include <sys/wait.h>
#include <str/Manip.h>
#include <sys/Exec.h>

namespace 
{
static const size_t READ_PIPE = 0;
static const size_t WRITE_PIPE = 1;
}

namespace sys
{

FILE* ExecPipe::openPipe(const std::string& command,
                         const std::string& type)
{
    FILE* ioFile = NULL;
    int pIO[2];

    //! create the IO pipes for stdin/out
    if (pipe(pIO) < 0)
    {
        return NULL;
    }

    //! fork a subprocess for running our command --
    //  here we use the user-defined pid, which is one major
    //  differences between this and the normal popen()
    mProcess = fork();
    switch (mProcess)
    {
        case -1:
            // there was an error while forking
            close(pIO[READ_PIPE]);
            close(pIO[WRITE_PIPE]);
            return NULL;
        case 0:
        {
            // we are now in the forked process --
            // anything performed in this block only affects the subprocess
            //
            // connect the pipes we create to stdin or stdout
            if (type == "r")
            {
                // reset both stdout and stderr to the outpipe
                // only close the descriptor if it is not already one of them
                if (pIO[WRITE_PIPE] != fileno(stdout))
                {
                    dup2(pIO[WRITE_PIPE], fileno(stdout));
                    if (pIO[WRITE_PIPE] != fileno(stderr))
                    {
                        dup2(pIO[WRITE_PIPE], fileno(stderr));
                        close(pIO[WRITE_PIPE]);
                    }
                }
                else if (pIO[WRITE_PIPE] != fileno(stderr))
                {
                    dup2(pIO[WRITE_PIPE], fileno(stderr));
                }

                // close the in pipe accordingly
                close(pIO[READ_PIPE]);
            }
            else
            {
                // reset stdin to the inpipe if it isn't already
                if (pIO[READ_PIPE] != fileno(stdin))
                {
                    dup2(pIO[READ_PIPE], fileno(stdin));
                    close(pIO[READ_PIPE]);
                }

                // close the out pipe accordingly
                close(pIO[WRITE_PIPE]);
            }

            //! call our command --
            //  this command replaces the forked process with
            //  command the user specified
            execl("/bin/sh", "sh", "-c",
                  command.c_str(),
                  static_cast<char*>(NULL));

            //! exit the subprocess once it has completed
            exit(127);
        }break;
    }

    //! this is executed on the parent process
    //
    //  connect the pipes currently connected in the subprocess
    //  to the FILE* handle. Close the unwanted handle.
    if (type == "r")
    {
        ioFile = fdopen(pIO[READ_PIPE], type.c_str());
        close(pIO[WRITE_PIPE]);
    }
    else
    {
        ioFile = fdopen(pIO[WRITE_PIPE], type.c_str());
        close(pIO[READ_PIPE]);
    }
    return ioFile;
}

int ExecPipe::killProcess()
{
    //! issue a forceful removal of the process
    kill(mProcess, SIGKILL);

    //! now clean up the process --
    //  wait needs to be called to remove the
    //  zombie process.
    return closePipe();
}

int ExecPipe::closePipe()
{
    if (mOutStream)
    {
        fclose(mOutStream);
        mOutStream = NULL;
    }

    int exitStatus = 0;
    int encodedStatus = 0;
    waitpid(mProcess, &encodedStatus, 0);

    if (WIFEXITED(encodedStatus))
    {
        // get exit code from child process
        exitStatus = WEXITSTATUS(encodedStatus);
    }
    else
    {
        //! unix gives a little better granularity on errors

        // due to uncaught signal (ex. segFault)
        if (WIFSIGNALED(encodedStatus))
        {
            throw except::IOException(
                Ctxt("The child process was terminated by " \
                        "an uncaught signal: " +
                        str::toString<int>(WTERMSIG(encodedStatus))));
        }
        // due to unplanned stoppage
        if (WIFSTOPPED(encodedStatus))
        {
            throw except::IOException(
                Ctxt("The child process was unexpectedly stopped: " +
                        str::toString<int>(WSTOPSIG(encodedStatus))));
        }

        // all other errors
        sys::SocketErr err;
        throw except::IOException(
                Ctxt("Failure while closing stream to child process: " +
                     err.toString()));
    }

    return exitStatus;
}

}

#endif
