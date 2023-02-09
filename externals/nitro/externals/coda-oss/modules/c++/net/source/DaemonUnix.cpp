#ifndef _WIN32
#include "net/DaemonUnix.h"

#include <iostream>
#include <fstream>
#include <errno.h> // errno
#include <unistd.h> // fork, umask, setsid, dup2, chdir, close
#include <sys/types.h> // fork, umask, open
#include <sys/stat.h> //open
#include <fcntl.h> // open
#include <signal.h> // kill
#include <string>

using namespace net;

DaemonUnix::DaemonUnix() :
    DaemonInterface(), mPidfile(""), mTracefile("/dev/null"), mForeground(false)
{
}

DaemonUnix::~DaemonUnix()
{
}

void DaemonUnix::start()
{
    // Check for running process from pidfile, if set
    sys::Pid_T pid = checkPidfile();
    if (pid > 0)
        throw except::Exception(Ctxt("Daemon is already running."));

    if (!mForeground)
    {
        this->fork();
        // ...now in the CHILD
    }

    // Write the pidfile, if set
    writePidfile();

    // Change the file mode mask
    ::umask(027);

    // Change the current working directory
    if (::chdir("/") != 0)
        throw except::Exception(Ctxt("Failed to change working directory."));

    // Redirect standard streams
    redirectStreamsTo(mTracefile);
}

void DaemonUnix::stop()
{
    if (mPidfile.empty())
        throw except::Exception(Ctxt("No pidfile set. Unable to stop daemon."));

    // exit regardless of return
    exit(!terminate(checkPidfile(), 0));
}

void DaemonUnix::restart()
{
    if (mPidfile.empty())
        throw except::Exception(Ctxt("No pidfile set. Unable to stop daemon."));

    if (terminate(checkPidfile()))
        start();
    else
        exit(1);
}

void DaemonUnix::daemonize(int& argc, char**& argv)
{
    enum {
        START,
        STOP,
        RESTART
    } command = START;

    //parse command line parameters
    for (int i = 1; i < argc; ++i)
    {
        std::string arg = argv[i];
        if (arg == "start")
        {
            command = START;
        }
        else if (arg == "stop")
        {
            command = STOP;
        }
        else if (arg == "restart")
        {
            command = RESTART;
        }
        else if (arg == "--foreground")
        {
            mForeground = true;
        }
        else if (arg == "--pidfile" && i < (argc - 1))
        {
            setPidfile(std::string(argv[++i]));
        }
        else if (arg == "--tracefile" && i < (argc - 1))
        {
            setTracefile(std::string(argv[++i]));
        }
    }

    if (command == STOP)
    {
        stop();
    }
    else if (command == RESTART)
    {
        restart();
    }
    else 
    {
        start();
    }
}

//! Set tracefile (file to redirect stdout and stderr)
void DaemonUnix::setTracefile(const std::string& tracefile)
{
    mTracefile = tracefile;
}

//! Set pidfile (file for locking application to single occurance)
void DaemonUnix::setPidfile(const std::string& pidfile)
{
    mPidfile = pidfile;
}

void DaemonUnix::fork()
{
    // Fork so the parent can exit, this returns control to the command line
    // or shell invoking the program. This step is required so that the new
    // process is guaranteed not to be a process group leader. The next step
    // fails if you're a process group leader. 
    pid_t pid = ::fork();
    if (pid < 0)
        throw except::Exception(Ctxt("Error in first fork for daemon."));
    else if (pid) // Parent
        ::_exit(0);
  
    // Create a new SID for the child process
    // Since a controlling terminal is associated with a session, and this new
    // session has not yet acquired a controlling terminal our process now has
    // no controlling terminal, which is a Good Thing for daemons.
    if (::setsid() < 0)
        throw except::Exception(Ctxt("Error setting new group session id."));

    // Fork again so the parent, (the session group leader), can exit.
    // This means that we, as a non-session group leader, can never regain
    // a controlling terminal. 
    pid = ::fork();
    if (pid < 0)
        throw except::Exception(Ctxt("Error in second fork for daemon."));
    else if (pid) // Parent
        ::_exit(0);
}

bool DaemonUnix::signal(sys::Pid_T pid, int sig)
{
    if (pid <= 0)
        return false;

    if (::kill(pid, sig) == 0)
    {
        return true;
    }
    else if (errno == EPERM)
    {
        throw except::Exception(
            Ctxt("Invalid permissions to signal existing daemon."));
    }
    else if (errno == EINVAL)
    {
        throw except::Exception(Ctxt("Invalid signal."));
    }

    return false;
}

sys::Pid_T DaemonUnix::checkPidfile()
{
    if (mPidfile.empty())
        return 0;

    // Try to read pidfile
    sys::OS os;
    sys::Pid_T pidFromFile = 0;
    if (os.exists(mPidfile) && os.isFile(mPidfile))
    {
        std::ifstream infile(mPidfile.c_str());
        infile >> pidFromFile;
        infile.close();
    }

    // Test if daemon process is running
    if (this->signal(pidFromFile, 0))
    {
        return pidFromFile;
    }

    return 0;
}

void DaemonUnix::writePidfile()
{
    if (!mPidfile.empty())
    {
        std::ofstream outfile(mPidfile.c_str(), std::ios::out|std::ios::trunc);
        outfile << ::getpid() << std::endl;
        outfile.close();
    }
}

//! \return Did terminate succeed?
bool DaemonUnix::terminate(sys::Pid_T pid, unsigned int retry)
{
    if (pid <= 0)
        return true;

    for (unsigned int i = 0; i <= retry; ++i)
    {
        // If process is running send terminate signal
        if (this->signal(pid, SIGTERM))
        {
            // Signal was sent, give process time to exit
            sleep(1);
            
            // Test if process is still running
            if (!this->signal(pid, 0))
            {
                return true;
            }
        }
        else
        {
            // Process does not exist or has finished
            return true;
        }
    }

    return false;
}

void DaemonUnix::redirectStreamsTo(const std::string& filename)
{
    if (openFileFor(STDIN_FILENO, "/dev/null", O_RDONLY) < 0)
    {
        throw except::Exception(
            Ctxt("Failed to open /dev/null for STDIN."));
    }
    if (openFileFor(STDOUT_FILENO, filename, O_WRONLY|O_CREAT|O_TRUNC) < 0)
    {
        throw except::Exception(
            Ctxt(FmtX("Failed to open file %s for STDOUT.", filename.c_str())));
    }
    if (openFileFor(STDERR_FILENO, filename, O_WRONLY|O_CREAT|O_TRUNC) < 0)
    {
        throw except::Exception(
            Ctxt(FmtX("Failed to open file %s for STDERR.", filename.c_str())));
    }
}

int DaemonUnix::openFileFor(int fd, const std::string& filename, int flags)
{
    int newfd = ::open(filename.c_str(), flags, 0644);
    if (newfd < 0)
        return -1;
    if (newfd == fd)
        return fd;
    if (::dup2(newfd,fd) < 0) // replace fd with a copy of newfd
        return -1;
    return fd;
}

#endif
