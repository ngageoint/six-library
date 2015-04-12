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


#ifndef __SYS_CONDITION_VAR_H__
#define __SYS_CONDITION_VAR_H__

#  if defined(_REENTRANT)

#    if defined(USE_NSPR_THREADS)
#        include "sys/ConditionVarNSPR.h"
namespace sys
{
typedef ConditionVarNSPR ConditionVar;
}
// If they explicitly want posix
#    elif defined(__POSIX)
#        include "sys/ConditionVarPosix.h"
namespace sys
{
typedef ConditionVarPosix ConditionVar;
}
#    elif defined(WIN32)
#        include "sys/ConditionVarWin32.h"
namespace sys
{
typedef ConditionVarWin32 ConditionVar;
}
#    elif defined(__sun) && !defined(__POSIX)
#        include "sys/ConditionVarSolaris.h"
namespace sys
{
typedef ConditionVarSolaris ConditionVar;
}
#    elif defined(__sgi) && !defined(__POSIX)
#        include "sys/ConditionVarIrix.h"
namespace sys
{
typedef ConditionVarIrix ConditionVar;
}
#    else
#        include "sys/ConditionVarPosix.h"
namespace sys
{
typedef ConditionVarPosix ConditionVar;
}
#    endif // Which thread package?

#  endif // Are we reentrant?

#endif // End of header

