/* =========================================================================
 * This file is part of mt-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
 *
 * mt-c++ is free software; you can redistribute it and/or modify
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


#pragma once
#ifndef CODA_OSS_mt_import_mt_h_h_INCLUDED_
#define CODA_OSS_mt_import_mt_h_h_INCLUDED_

#if _MSC_VER
#pragma warning(push)
#pragma warning(disable: 5039) //	'...': pointer or reference to potentially throwing function passed to 'extern "C"' function under - EHc.Undefined behavior may occur if this function throws an exception.
#endif // _MSC_VER

#include "mt/RequestQueue.h"
#include "mt/ThreadPoolException.h"
#include "mt/BasicThreadPool.h"
#include "mt/GenericRequestHandler.h"
#include "mt/Singleton.h"
#include "mt/CriticalSection.h"
#include "mt/AbstractThreadPool.h"
#include "mt/WorkerThread.h"
#include "mt/AbstractTiedThreadPool.h"
#include "mt/TiedWorkerThread.h"
#include "mt/GenerationThreadPool.h"
#include "mt/ThreadGroup.h"
#include "mt/ThreadPlanner.h"
#include "mt/Runnable1D.h"
#include "mt/BalancedRunnable1D.h"
#include "mt/WorkSharingBalancedRunnable1D.h"

#include "mt/CPUAffinityInitializer.h"
#include "mt/CPUAffinityThreadInitializer.h"
#include "mt/Algorithm.h"

#if _MSC_VER
#pragma warning(pop)
#endif  // _MSC_VER

#endif  // CODA_OSS_mt_import_mt_h_h_INCLUDED_
