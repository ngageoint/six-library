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
 * Enumeration class for the NITF version
 */
public final class Version
{

    /**
     * Represents a NITF 2.0 type
     */
    public final static Version NITF_20 = new Version("NITF 2.0");

    /**
     * Represents a NITF 2.1 type
     */
    public final static Version NITF_21 = new Version("NITF 2.1");

    // a name that can be queried that describes the version
    private String name;

    /**
     * Return a name that describes the type
     * 
     * @return a descriptive name
     */
    public String getName()
    {
        return name;
    }

    public String toString()
    {
        return getName();
    }

    // keep private
    private Version(String name)
    {
        this.name = name;
    }

}

