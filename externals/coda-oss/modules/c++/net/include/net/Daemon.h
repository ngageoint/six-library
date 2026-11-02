/* =========================================================================
 * This file is part of net-c++
 * =========================================================================
 *
 * © 2010, MDA Information Systems LLC
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
#ifndef __NET_DAEMON_H__
#define __NET_DAEMON_H__

#ifdef _WIN32
#  include "net/DaemonWin32.h"
namespace net
{
typedef DaemonWin32 Daemon;
}
#
#else
#  include "net/DaemonUnix.h"
namespace net
{
typedef DaemonUnix Daemon;
}
#endif

#endif
