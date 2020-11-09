/* =========================================================================
 * This file is part of NITRO
 * =========================================================================
 * 
 * (C) Copyright 2004 - 2010, MDA Information Systems LLC
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
 * Represents a source of an image band
 */
public class SegmentSource extends NITFObject
{
    /**
     * @see NITFObject#NITFObject(long)
     */
    SegmentSource(long address)
    {
        super(address);
    }

    /*
     * (non-Javadoc)
     * 
     * @see nitf.DestructibleObject#destructMemory()
     */
    // protected native synchronized void destructMemory();

    public static final native SegmentSource makeSegmentMemorySource(
            byte[] data, int size, int start, int byteSkip)
            throws NITFException;

}
