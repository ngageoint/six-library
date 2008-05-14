/* =========================================================================
 * This file is part of NITRO
 * =========================================================================
 * 
 * (C) Copyright 2004 - 2008, General Dynamics - Advanced Information Systems
 *
 * NITRO is free software; you can redistribute it and/or modify
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
 * License along with this program; if not, If not, 
 * see <http://www.gnu.org/licenses/>.
 *
 */

package nitf;

/**
 * Class representing a NITF object that has underlying
 * memory that can be independently destructed.
 */
public abstract class DestructibleObject extends NITFObject
{

    /**
     * Default Constructor
     */
    protected DestructibleObject()
    {
        super();
    }

    /**
     * Constructor
     *
     * @param address the memory address of the underlying object
     */
    protected DestructibleObject(long address)
    {
        super(address);
        NITFUtils.incrementRefCount(this);
    }

    /**
     * Sets the memory address, but also increments the reference count
     *
     * @param address
     */
    synchronized void setAddress(long address)
    {
        super.setAddress(address);
        NITFUtils.incrementRefCount(this);
    }

    /**
     * Actually destructs the underlying memory.
     * <p/>
     * This should only be called by NITFUtils.decrementRefCount()
     */
    protected abstract void destructMemory();

    /**
     * Attempts to destruct the underlying object, if it is not referenced elsewhere
     */
    public void destruct()
    {
        NITFUtils.decrementRefCount(this);
    }

    /**
     * If Java finalizes this object, we want to destruct it first
     *
     * @throws Throwable
     */
    protected void finalize() throws Throwable
    {
        destruct();
        super.finalize();
    }
}

