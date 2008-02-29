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

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

/**
 * This is a utility class used both internally by the NITF library and by the
 * API user. <p/> This class provides the functionality to free the underlying
 * memory
 */
public final class NITFUtils
{
    private static Map memoryMap = new HashMap();

    private static List initialObjectList = new ArrayList();

    private static List ioHandles = new ArrayList();

    // this makes sure that the jni library gets loaded by NITFObject
    static
    {
        NITFObject.class.getName();
    }

    /**
     * Create a shutdown hook for cleaning up the underlying resources (memory
     * and open io handles)
     */
    static
    {
        Runtime.getRuntime().addShutdownHook(new Thread()
        {
            public void run()
            {
                NITFUtils.freeAllMemory();
            }
        });
    }

    /**
     * Increments the count of an address
     * 
     * @param object
     */
    static void incrementRefCount(DestructibleObject object)
    {
        if (object.getAddress() > 0)
        {
            synchronized (memoryMap)
            {
                String val = String.valueOf(object.getAddress());
                if (memoryMap.containsKey(val))
                {
                    Integer count = (Integer) memoryMap.get(val);
                    memoryMap.put(val, new Integer(count.intValue() + 1));
                }
                else
                {
                    memoryMap.put(val, new Integer(1));
                    initialObjectList.add(object);
                }
            }
        }
    }

    /**
     * Decrements the count of an address, and destructs the object if the count
     * is now < 1
     * 
     * @param object
     */
    static void decrementRefCount(DestructibleObject object)
    {
        if (object.getAddress() > 0)
        {
            synchronized (memoryMap)
            {
                String val = String.valueOf(object.getAddress());
                // if its in here, update its count, if not, forget about it
                if (memoryMap.containsKey(val))
                {
                    Integer count = (Integer) memoryMap.get(val);
                    int c = count.intValue() - 1;
                    if (c > 0)
                    {
                        memoryMap.remove(val);
                        memoryMap.put(val, new Integer(c));
                        return;
                    }

                    // if the count == 0, we remove it, but destruct it first
                    object.destructMemory();
                    memoryMap.remove(val);
                    initialObjectList.remove(object);
                }
            }
        }
    }

    /**
     * Tells the framework to keep track of the given IOHandle. This handle will
     * be freed with a call to
     * 
     * @param handle
     */
    static void trackIOHandle(IOHandle handle)
    {
        if (handle.getIOHandle() > 0)
        {
            synchronized (ioHandles)
            {
                ioHandles.add(handle);
            }
        }
    }

    /**
     * This function will close any IOHandle that was created. This function can
     * be called any time, so users must be careful not to use any closed
     * handles. <p/> This function gets called by freeAllMemory()
     */
    public static void closeAllIOHandles()
    {
        synchronized (ioHandles)
        {
            for (int i = 0; i < ioHandles.size(); i++)
            {
                IOHandle ioHandle = (IOHandle) ioHandles.get(i);
                ioHandle.close();
            }
            ioHandles.clear();
        }
    }

    /**
     * This function will free ALL of the underlying native memory, thus making
     * all of existing NITF Java objects usesless. This method calls
     * closeAllIOHandles(). <p/> Call this method when you are ready to exit the
     * system, or when you are surely never going to use any of the existing
     * Java objects or their functionalities anymore. <p/> One example use for
     * calling this function is if you are looping over a bunch of files. When
     * you are finished reading/writing each file, call this function, and you
     * will free up any memory used. <p/> This function will get called
     * automatically when the application using it shuts down, so you don't have
     * to add it to your programs.
     */
    public static void freeAllMemory()
    {
        synchronized (memoryMap)
        {
            for (int i = 0; i < initialObjectList.size(); i++)
            {
                DestructibleObject object = (DestructibleObject) initialObjectList
                        .get(i);
                // actually check if it is a valid address
                if (object.getAddress() > 0)
                {
                    // TODO fix the code to be able to free everything
                    // we are running into problems where we need
                    // to somehow always keep a static map of destructible
                    // objects that will now be destructed by something else
                    // internally, and shouldn't be destructed here

                    // for now, we will ONLY destruct Reader and Record
                    if (object.getClass().equals(Reader.class)
                            || object.getClass().equals(Record.class))
                    {
                        object.destructMemory();
                    }
                    object.setAddress(0);
                }
            }
            initialObjectList.clear();
            memoryMap.clear();
        }
        closeAllIOHandles();
    }

    /**
     * Returns true if the record is in 2.0 format, false otherwise
     * 
     * @param record
     * @return
     * @see Record#getVersion()
     * @deprecated
     */
    public static boolean isNITF20(Record record)
    {
        try
        {
            final FileHeader header = record.getHeader();
            final Field fileHeader = header.getFileHeader();
            final Field fileVersion = header.getFileVersion();

            final String headerString = fileHeader.getStringData();
            final String versionString = fileVersion.getStringData();
            return headerString.equals("NITF")
                    && (versionString.equals("02.00") || versionString
                            .equals("01.10"));
        }
        catch (NITFException e)
        {
            e.printStackTrace();
            return false;
        }
    }

    /**
     * Returns true if the record is in 2.1 format, false otherwise
     * 
     * @param record
     * @return
     * @see Record#getVersion()
     * @deprecated
     */
    public static boolean isNITF21(Record record)
    {
        try
        {
            final FileHeader header = record.getHeader();
            final Field fileHeader = header.getFileHeader();
            final Field fileVersion = header.getFileVersion();

            final String headerString = fileHeader.getStringData();
            final String versionString = fileVersion.getStringData();
            return (headerString.equals("NITF") && versionString
                    .equals("02.10"))
                    || (headerString.equals("NSIF") && versionString
                            .equals("01.00"));
        }
        catch (NITFException e)
        {
            e.printStackTrace();
            return false;
        }
    }

    /**
     * This attempts to load plugins from the directory given
     * 
     * @param dirName
     *            plugin directory to load
     * @throws NITFException
     */
    public static void loadPluginDir(String dirName) throws NITFException
    {
        PluginRegistry.loadPluginDir(dirName);
    }

}

