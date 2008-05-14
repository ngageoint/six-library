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

/**
 * ${NAME}
 *
 */
package nitf;

/**
 * TREDescriptionDataType <p/> This class specifies a data type for a
 * TREDescription
 */
public class TREDescriptionDataType extends nitf.FieldType
{
    /**
     * Specifies a dataType that is the start of a loop
     */
    public static final TREDescriptionDataType NITF_LOOP = new TREDescriptionDataType(
            "NITF_LOOP");

    /**
     * Specifies a dataType that is the end of a loop
     */
    public static final TREDescriptionDataType NITF_ENDLOOP = new TREDescriptionDataType(
            "NITF_ENDLOOP");

    /**
     * Specifies a dataType that is the start of an if
     */
    public static final TREDescriptionDataType NITF_IF = new TREDescriptionDataType(
            "NITF_IF");

    /**
     * Specifies a dataType that is an endif
     */
    public static final TREDescriptionDataType NITF_ENDIF = new TREDescriptionDataType(
            "NITF_ENDIF");

    /**
     * Specifies a dataType that is the end marker
     */
    public static final TREDescriptionDataType NITF_END = new TREDescriptionDataType(
            "NITF_END");

    /**
     * Specifies a dataType that is the end marker
     */
    public static final TREDescriptionDataType NITF_COMP_LEN = new TREDescriptionDataType(
            "NITF_COMP_LEN");

    protected TREDescriptionDataType(String name)
    {
        super(name);
    }

    public String toString()
    {
        return myName;
    }

}

