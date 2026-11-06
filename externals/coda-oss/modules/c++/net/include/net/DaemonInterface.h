/* =========================================================================
 * This file is part of net-c++
 * =========================================================================
 *
 * © 2010 MDA Information Systems LLC
 *
 * net-c++ is free software; you can redistribute it and/or modify
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
#ifndef __NET_DAEMON_INTERFACE_H__
#define __NET_DAEMON_INTERFACE_H__

#include <string>

namespace net
{

/*!
 *  \class DaemonInterface
 *  \brief The interface for any daemon class
 *
 *  Interface class that contains various methods for running an
 *  application as a background task.
 */
class DaemonInterface
{
public:
    //!  Constructor
    DaemonInterface() {}

    //!  Destructor
    virtual ~DaemonInterface() {}

    //! Start the daemon
    virtual void start() = 0;

    //! Stop the daemon specified in pidfile
    virtual void stop() = 0;

    //! Stop the daemon specified in pidfile and start a new one
    virtual void restart() = 0;

    /*!
     *  Parse and execute command line option (start/stop/restart)
     *
     *  \return Should processing continue?
     */
    virtual void daemonize(int& argc, char**& argv) = 0;


    //! Set pidfile (file for locking application to single occurance).
    virtual void setPidfile(const std::string& pidfile) = 0;

    //! Get pidfile.
    virtual std::string getPidfile() const = 0;

    //! Set tracefile (file to redirect stdout and stderr).
    virtual void setTracefile(const std::string& tracefile) = 0;

    //! Get tracefile.
    virtual std::string getTracefile() const = 0;
};
}

#endif
