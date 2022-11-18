#ifndef __NET_DAEMON_UNIX_H__
#define __NET_DAEMON_UNIX_H__

#if !(defined(WIN32) || defined(_WIN32))

#include "net/DaemonInterface.h"
#include <import/sys.h>
#include <string>

namespace net
{

/*!
 *  \class DaemonUnix
 *  \brief Unix implementation of daemon class
 */
class DaemonUnix : public DaemonInterface
{
public:
    DaemonUnix();
    virtual ~DaemonUnix();

    //! Start the daemon
    void start();

    //! Stop the daemon specified in pidfile
    void stop();

    //! Stop the daemon specified in pidfile and start a new one
    void restart();

    //! Parse and execute command line option (start/stop/restart)
    void daemonize(int& argc, char**& argv);


    //! Set pidfile (file for locking application to single occurance).
    void setPidfile(const std::string& pidfile);

    //! Get pidfile.
    std::string getPidfile() const { return mPidfile; }

    //! Set tracefile (file to redirect stdout and stderr).
    void setTracefile(const std::string& tracefile);

    //! Get tracefile.
    std::string getTracefile() const { return mTracefile; }

protected:
    std::string mPidfile;
    std::string mTracefile;
    bool mForeground;

    void fork();
    void redirectStreamsTo(const std::string& filename);
    int openFileFor(int fd, const std::string& filename, int flags);
    sys::Pid_T checkPidfile();
    void writePidfile();
    bool signal(sys::Pid_T pid, int sig);
    bool terminate(sys::Pid_T pid, unsigned int retry = 3);
};
}

#endif
#endif
