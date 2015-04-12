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


#ifndef __SYS_RUNNABLE_H__
#define __SYS_RUNNABLE_H__


namespace sys
{
/*!
 *  \class Runnable
 *  \brief Defines interface for runnables
 *
 *  This class defines the interface for action objects
 *  If you need an object that runs in its own space
 *  you should inherit this class.  You may ONLY assign
 *  dynamically allocated runnables to this class, because
 *  auto-deletion is done.
 *
 *  The intention of the Runnable is that it behave identically
 *  to the java Runnable class.  Runnable implements the Command
 *  Design Pattern.
 *
 */
class Runnable
{
public:
    /*! Constructor  */
    Runnable()
    {}

    //!  Destructor
    virtual ~Runnable()
    {}

    /*!
     *  Overload this function in order with an action to make
     *  this object runnable.
     */
    virtual void run() = 0;
};
}

#endif
