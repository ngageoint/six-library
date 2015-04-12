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


#if defined(USE_NSPR_THREADS)
#include "sys/ConditionVarNSPR.h"

sys::ConditionVarNSPR::~ConditionVarNSPR()
{
    PR_DestroyCondVar(mNative);
}

bool sys::ConditionVarNSPR::signal()
{
    return (PR_NotifyCondVar(mNative) == PR_SUCCESS);

}

bool sys::ConditionVarNSPR::wait()
{
    return (PR_WaitCondVar(mNative,
                           PR_INTERVAL_NO_WAIT) == PR_SUCCESS);
}

bool sys::ConditionVarNSPR::wait(double seconds)
{
    double milli = seconds * 1000;
    return (PR_WaitCondVar(mNative,
                           PR_MillisecondsToInterval((PRUint32)milli)) == PR_SUCCESS);
}

bool sys::ConditionVarNSPR::broadcast()
{
    return ( PR_NotifyAllCondVar(mNative) == PR_SUCCESS );
}

#endif

