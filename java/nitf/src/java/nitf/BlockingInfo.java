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

import java.io.IOException;
import java.io.PrintStream;

/**
 * A representation of the blocking information used
 * in the library.
 */
public final class BlockingInfo extends DestructibleObject
{

    public BlockingInfo() throws NITFException
    {
        construct();
    }

    BlockingInfo(long address)
    {
        super(address);
    }

    /**
     * Destruct the underlying memory
     */
    protected native synchronized void destructMemory();

    private native synchronized void construct() throws NITFException;

    /**
     * Returns the number of blocks per row
     *
     * @return
     */
    public native synchronized int getNumBlocksPerRow();

    /**
     * Sets the number of blocks per row
     *
     * @param numBlocksPerRow
     */
    public native synchronized void setNumBlocksPerRow(int numBlocksPerRow);

    /**
     * Returns the number of blocks per column
     *
     * @return
     */
    public native synchronized int getNumBlocksPerCol();

    /**
     * Sets the number of blocks per column
     *
     * @param numBlocksPerCol
     */
    public native synchronized void setNumBlocksPerCol(int numBlocksPerCol);

    /**
     * Returns the number of rows per block
     *
     * @return
     */
    public native synchronized int getNumRowsPerBlock();

    /**
     * Sets the number of rows per block
     *
     * @param numRowsPerBlock
     */
    public native synchronized void setNumRowsPerBlock(int numRowsPerBlock);

    /**
     * Returns the number of columns per block
     *
     * @return
     */
    public native synchronized int getNumColsPerBlock();

    /**
     * Sets the number of columns per block
     *
     * @param numColsPerBlock
     */
    public native synchronized void setNumColsPerBlock(int numColsPerBlock);

    /**
     * Returns the length
     *
     * @return
     */
    public native synchronized int getLength();

    /**
     * Sets the length
     *
     * @param length
     */
    public native synchronized void setLength(int length);

    /**
     * Prints the data pertaining to this object to an OutputStream
     *
     * @param out
     * @throws IOException
     */
    public void print(PrintStream out) throws IOException
    {
        out.write(("Length: " + getLength() + "\n").getBytes());
        out.write(
                ("NumBlocksPerRow: " + getNumBlocksPerRow() + "\n").getBytes());
        out.write(
                ("NumBlocksPerCol: " + getNumBlocksPerCol() + "\n").getBytes());
        out.write(
                ("NumRowsPerBlock: " + getNumRowsPerBlock() + "\n").getBytes());
        out.write(
                ("NumColsPerBlock: " + getNumColsPerBlock() + "\n").getBytes());
    }
}

