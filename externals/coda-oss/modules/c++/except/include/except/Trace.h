/* =========================================================================
 * This file is part of except-c++ 
 * =========================================================================
 * 
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
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
#include <ostream>

#include <except/Context.h>

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
struct Trace final
{
    /*!
     * Gets size of stack
     * \return the size
     */
    size_t getSize() const noexcept
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
    void popContext() noexcept
    {
        mStack.pop_front();
    }

    /*!
     * Gets first Context on stack
     */
    const Context& getContext() const noexcept
    {
        return mStack.front();
    }

    /*!
     * Get the stack
     * \return The stack (const)
     */
    const std::list<Context>& getStack() const noexcept
    {
        return mStack;
    }

    /*!
     * Get the stack
     * \return The stack (non-const)
     */
    std::list<Context>& getStack() noexcept
    {
        return mStack;
    }

private:
    //! The name of the internal stack wrapped by the Trace
    std::list<Context> mStack;
};

std::ostream& operator<<(std::ostream& os, const Trace& t);
}

#endif
