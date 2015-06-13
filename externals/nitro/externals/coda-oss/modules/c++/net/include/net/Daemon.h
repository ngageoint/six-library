#ifndef __NET_DAEMON_H__
#define __NET_DAEMON_H__

#if defined(WIN32)
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
