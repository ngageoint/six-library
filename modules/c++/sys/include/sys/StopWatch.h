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


#ifndef __SYS_STOPWATCH_H__
#define __SYS_STOPWATCH_H__

#include "sys/DateTime.h"

namespace sys
{

class StopWatch
{
public:
    StopWatch(){}
    virtual ~StopWatch(){}
    /*!
     *  Start the timer and return the time in millis
     */
    virtual double start() = 0;

    /*!
     *  Stop the timer and return the elapsed time in millis
     */
    virtual double stop() = 0;

    /*!
     *  Pause the timer and return the elapsed time in millis
     */
    virtual double pause() = 0;

    /*!
     *  Clear the timer
     */
    virtual void clear() = 0;
};

class RealTimeStopWatch : public StopWatch
{
protected:
    double mStartTime;
    double mTimePaused;
    double mPauseStartTime;
    bool mPaused;
public:
    RealTimeStopWatch();

    ~RealTimeStopWatch();

    double start();

    double stop();

    double pause();

    void clear();
};

class CPUStopWatch : public StopWatch
{
protected:
    clock_t mStartTime;
    clock_t mPauseStartTime;
    clock_t mTimePaused;
    bool mPaused;
    double mClocksPerMillis;
public:
    CPUStopWatch();

    ~CPUStopWatch();

    double start();

    double stop();

    double pause();

    void clear();
};

}

#endif
