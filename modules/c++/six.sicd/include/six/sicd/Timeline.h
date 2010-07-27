/* =========================================================================
 * This file is part of six-c++ 
 * =========================================================================
 * 
 * (C) Copyright 2004 - 2009, General Dynamics - Advanced Information Systems
 *
 * six-c++ is free software; you can redistribute it and/or modify
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
#ifndef __SIX_TIMELINE_H__
#define __SIX_TIMELINE_H__

#include "six/Types.h"
#include "six/Init.h"
#include "six/Parameter.h"

namespace six
{
namespace sicd
{
/*!
 *  \class TimelineSet
 *  \brief SICD/Timeline/IPP/Set parameters
 *
 *  Identifies a set of IPP parameters.  Minimum of 1 set of
 *  parameters are required for IPP
 */
struct TimelineSet
{
    //! Start time for the period relative to collect start
    double tStart;

    //! End time for the period relative to collect start
    double tEnd;

    //! Starting ipp index for the period described
    int interPulsePeriodStart;

    //! Ending ipp index for the period described
    int interPulsePeriodEnd;

    /*! 
     *  ipp index poly coefs yield IPP index as a function of time t
     *  Starting tStart to tEnd
     */
    Poly1D interPulsePeriodPoly;

    //!  Constructor
    TimelineSet();
    
    //!  Destructor
    ~TimelineSet() {}

    //!  Clone this object
    TimelineSet* clone() const
    {
        return new TimelineSet(*this);
    }

};

/*!
 *  \class InterPulsePeriod
 *  \brief SICD 'IPP' parameter
 *  Describes the interpulse period (IPP) parameters.  This section
 *  of SICD is optional, and contains one or more Sets (TimelineSet)
 */
struct InterPulsePeriod
{
    /*!
     *  Constructor.
     */
    InterPulsePeriod()
    {
    }
    //!  Destrucctor.  Goes through sets and deletes every item
    ~InterPulsePeriod();

    //!  Clone this object and its underlying TimelineSets
    InterPulsePeriod* clone() const;
    
    //!  Vector of TimelineSet objects
    std::vector<TimelineSet*> sets;
};

/*!
 *  \struct Timeline
 *  \brief SICD Timeline block
 *
 *  Describes the imaging collection timeline.  This is a required block,
 *  and it requires that collectStart and collectDuration are written (or
 *  read).  The IPP parameter (referred to here as interPulsePeriod) is
 *  optional.
 *
 */
struct Timeline
{
    //! Collection date/time UTC, measured from collection start
    DateTime collectStart;

    //! Duration of collection period
    double collectDuration;

    //! Optional IPP parameter description
    InterPulsePeriod* interPulsePeriod;

    //! Constructor
    Timeline() :
        interPulsePeriod(NULL)
    {
        collectStart = Init::undefined<DateTime>();
        collectDuration = Init::undefined<double>();

    }
    //!  Delete optional interPulsePeriod if non-null
    ~Timeline();

    //! Clones object and underlying interPulsePeriod if non-null
    Timeline* clone() const;

};
}
}
#endif
