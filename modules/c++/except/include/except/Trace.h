/* =========================================================================
 * This file is part of except-c++ 
 * =========================================================================
 * 
 * (C) Copyright 2004 - 2009, General Dynamics - Advanced Information Systems
 *
 * except-c++ is free software; you can redistribute it and/or modify
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

#ifndef __EXCEPT_TRACE_H__
#define __EXCEPT_TRACE_H__

#include <list>
#include <sstream>
#include "except/Context.h"

/*!
 * \file Trace.h
 * \brief Class for holding exception traces
 *
 */

namespace except
{
/*!
 * \class Trace
 * \brief Holds stack of context information
 */
class Trace
{
public:
    /*!
     * Default Constructor
     */
    Trace()
    {
    }

    /*!
     * Gets size of stack
     * \return the size
     */
    size_t getSize() const
    {
        return mStack.size();
    }

    /*!
     * Pushes Context onto the stack
     * \param c The Context
     */
    void pushContext(const Context& c)
    {
        mStack.push_front(c);
    }

    /*!
     * Pops Context off the stack
     */
    void popContext()
    {
        mStack.pop_front();
    }

    /*!
     * Gets first Context on stack
     */
    const Context& getContext() const
    {
        return mStack.front();
    }

    /*!
     * Get the stack
     * \return The stack (const)
     */
    const std::list<Context>& getStack() const
    {
        return mStack;
    }

    /*!
     * Get the stack
     * \return The stack (non-const)
     */
    std::list<Context>& getStack()
    {
        return mStack;
    }

private:
    //! The name of the internal stack wrapped by the Trace
    std::list<Context> mStack;
};
}

std::ostream& operator<<(std::ostream& os, const except::Trace& t);

#endif
