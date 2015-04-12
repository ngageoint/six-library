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


#include "sys/StopWatch.h"

sys::RealTimeStopWatch::RealTimeStopWatch():mStartTime(0),mTimePaused(0),mPauseStartTime(0),mPaused(false)
{
}

sys::RealTimeStopWatch::~RealTimeStopWatch()
{
}

double sys::RealTimeStopWatch::start()
{
    sys::DateTime now;
    double nowInMillis = now.getTimeInMillis();
    
    // If we are in a paused state
    if(mPaused)
    {
	// accumulate the time we have been paused
	mTimePaused += nowInMillis - mPauseStartTime;
	mPaused = false;
    } // else set the start time to right now
    else
	mStartTime = nowInMillis;
    
    // return the current time
    return nowInMillis;
}

double sys::RealTimeStopWatch::stop()
{
    sys::DateTime now;
    double nowInMillis = now.getTimeInMillis();
    // If in a paused state, accumulate paused time
    if(mPaused)
    {
	mTimePaused += (nowInMillis - mPauseStartTime);
	mPaused = false;
    }    
    // If we have been started then calculate stop time,
    // otherwise don't bother
    if(mStartTime != 0)
    {
	double elapsed = (nowInMillis - mStartTime - mTimePaused);
	//mStartTime = 0;
	return elapsed;
    }
    return 0;
}

double sys::RealTimeStopWatch::pause()
{
    // If not already paused, set it to be so and the 
    // pause start time to be now
    if(!mPaused)
    {
	sys::DateTime now;
	mPauseStartTime = now.getTimeInMillis();
	mPaused = true;
    }
    return mPauseStartTime;
}

void sys::RealTimeStopWatch::clear()
{
    // reset the stopwatch
    mStartTime = 0;
    mPauseStartTime = 0;
    mTimePaused = 0;
    mPaused = false;
}

sys::CPUStopWatch::CPUStopWatch():mStartTime(-1),mPauseStartTime(0),mTimePaused(0),mPaused(false)
{
    mClocksPerMillis = CLOCKS_PER_SEC/1000;
}

sys::CPUStopWatch::~CPUStopWatch()
{
}

double sys::CPUStopWatch::start()
{
    clock_t now = clock();
    // If we're in a paused state, accumulate the paused time
    if(mPaused)
    {
	mTimePaused += (now - mPauseStartTime);
	mPaused = false;
    }
    else
	mStartTime = now;
    
    return ((double)now/mClocksPerMillis);
}

double sys::CPUStopWatch::stop()
{
    clock_t end = clock();
    // If in paused state, accumulate paused time before stopping
    if(mPaused)
    {
	mTimePaused += (end - mPauseStartTime);
	mPaused = false;
    }
    // If start time was never set (or reset) then don't bother calculating elapsed time
    if(mStartTime != -1)
	return ((double)(end - mStartTime - mTimePaused)/mClocksPerMillis);
    else
	return 0;
}

double sys::CPUStopWatch::pause()
{
    if(!mPaused)
    {
	mPauseStartTime = clock();
	mPaused = true;
    }
    return ((double)mPauseStartTime/mClocksPerMillis);
}

void sys::CPUStopWatch::clear()
{
    // 0 is valid for clock() so reset start to -1
    mStartTime = -1;
    mPauseStartTime = 0;
    mTimePaused = 0;
    mPaused = false;
}
