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
 * The handle that gives access to writing images
 */
public final class ImageWriter extends DestructibleObject
{

    /**
     * @see DestructibleObject#DestructibleObject(long)
     */
    ImageWriter(long address)
    {
        super(address);
    }

    /**
     * Destructs the object
     */
    protected native synchronized void destructMemory();

    /**
     * Returns the IOHandle associated with this ImageWriter
     * 
     * @return
     */
    public native IOHandle getOutputHandle();

    /**
     * Returns the ImageSource
     * 
     * @return the ImageSource associated with this ImageWriter
     * @throws NITFException
     */
    public native ImageSource getImageSource() throws NITFException;

    /**
     * Method used by the actual Writer to write an image out. <p/> This
     * function writes out an image from a source, by pulling the required
     * amounts from the source attached. This function is probably only useable
     * from the Writer.write() function. The image source must be attached at
     * this point, or an exception will occur.
     * 
     * @param numBitsPerPixel
     *            The number of bits per pixel in the image
     * @param numImageBands
     *            The number of bands in the image
     * @param numMultispectralImageBands
     *            Number of multi-spectral image bands
     * @param numRows
     *            The number of rows
     * @param numCols
     *            The number of columns
     * @return
     * @throws NITFException
     */
    public native boolean write(int numBitsPerPixel, int numImageBands,
            int numMultispectralImageBands, int numRows, int numCols)
            throws NITFException;

    /**
     * Attaches the specified ImageSource to this ImageWriter
     * 
     * @param imageSource
     *            the imageSource to attach
     * @return true if it attached successfully, false otherwise
     * @throws NITFException
     *             if an ImageSource has already been attached, or if an error
     *             occurs
     */
    public native boolean attachSource(ImageSource imageSource)
            throws NITFException;

    /**
     * Enables/disables cached writes. Enabling cached writes causes the system
     * to accumulate full blocks of data prior to writing. This is more efficent
     * in terms of writing but requires more memory. For blocking modes, R, P,
     * and B blocking modes, one block sized buffer is required for each block
     * column (number of blocks/row). For S mode one block is required for each
     * band for each block column, however for the same iamge dimensions, pixel
     * size and number of bands it amount to the same storage since the blocks
     * of the S mode image are smaller (each contains only one band of data)
     * 
     * @param flag
     * @return Returns the current enable/disable state
     */
    public native boolean setWriteCaching(boolean flag);

}
