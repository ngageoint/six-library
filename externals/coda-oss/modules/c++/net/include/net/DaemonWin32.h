#ifndef __NET_DAEMON_WIN32_H__
#define __NET_DAEMON_WIN32_H__

#if defined(WIN32) || defined(_WIN32)

#include "net/DaemonInterface.h"
#include <import/except.h>

namespace net
{

/*!
 *  \class DaemonWin32
 *  \brief Windows implementation of daemon class
 */
class DaemonWin32 : public DaemonInterface
{
public:
    DaemonWin32() : DaemonInterface() {}

    void start()
    {
        throw except::NotImplementedException(
            Ctxt("Windows service not yet implemented."));
    }

    void stop()
    {
        throw except::NotImplementedException(
            Ctxt("Windows service not yet implemented."));
    }

    void restart()
    {
        throw except::NotImplementedException(
            Ctxt("Windows service not yet implemented."));
    }

    //! Parse and execute command line option (start/stop/restart)
    void daemonize(int& argc, char**& argv)
    {
        bool foreground = false;

        /*! 
         * Use the --foreground argument to allow running from
         * within the console. Windows Services may still be the 
         * best way to daemonize the process, but this will allow 
         * that to be possible.
         *
         * TODO: Implement this class for daemonizing the process
         *       from within the executable
         */
        for (int i = 1; i < argc; ++i)
        {
            std::string arg = argv[i];
            if (arg == "--foreground")
            {
                foreground = true;
            }
        }
        if (!foreground)
        {
            throw except::NotImplementedException(
                Ctxt("Windows service not yet implemented."));
        }
    }

    void setTracefile(const std::string&) {}
    void setPidfile(const std::string&) {}
    std::string getTracefile() const { return ""; }
    std::string getPidfile() const { return ""; }
};

}

#endif
#endif
