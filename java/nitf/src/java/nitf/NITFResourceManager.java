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

import java.util.HashMap;
import java.util.HashSet;
import java.util.Iterator;
import java.util.LinkedHashMap;
import java.util.Map;
import java.util.Set;

import org.apache.commons.logging.Log;
import org.apache.commons.logging.LogFactory;

/**
 * This class provides the functionality to manage the underlying memory
 */
public final class NITFResourceManager
{

    // this makes sure that the jni library gets loaded by NITFObject
    static
    {
        NITFObject.class.getName();
    }

    private static final Log log = LogFactory.getLog(NITFResourceManager.class);

    // the Singleton instance
    private static final NITFResourceManager singleton = new NITFResourceManager();

    /**
     * @return the singleton instance of the NITFResourceManager
     */
    public static NITFResourceManager getInstance()
    {
        return singleton;
    }

    /**
     * Create a shutdown hook for cleaning up the underlying resources
     */
    static
    {
        Runtime.getRuntime().addShutdownHook(new Thread()
        {
            public void run()
            {
                NITFResourceManager.getInstance().freeAllMemory();
            }
        });
    }

    private Map<String, Integer> refCountMap = new HashMap<String, Integer>();

    private Set<IOInterface> ioHandles = new HashSet<IOInterface>();

    private Map<String, DestructibleObject> destructiblesMap = new LinkedHashMap<String, DestructibleObject>();

    /**
     * Maps memory addresses to essentially a semaphore for management requests
     */
    private Map<String, Integer> nativeManagementMap = new LinkedHashMap<String, Integer>();

    /**
     * Increments the reference count of an object
     * 
     * @param object
     */
    protected void incrementRefCount(DestructibleObject object)
    {
        if (object.isValid())
        {
            synchronized (refCountMap)
            {
                String memVal = String.valueOf(object.getAddress());
                if (refCountMap.containsKey(memVal))
                {
                    refCountMap.put(memVal, refCountMap.get(memVal) + 1);
                    log.debug("Incremented ref count: " + object.getInfo());
                }
                else
                {
                    refCountMap.put(memVal, 1);
                    destructiblesMap.put(memVal, object);
                    log.debug("Managing new object: " + object.getInfo());
                }
            }
        }
    }

    /**
     * Decrements the reference count of an object, and destructs the object if
     * the count is now < 1
     * 
     * @param object
     */
    protected void decrementRefCount(DestructibleObject object)
    {
        if (object.isValid())
        {
            synchronized (refCountMap)
            {
                String val = String.valueOf(object.getAddress());
                log.debug("Decrementing ref count: " + object.getInfo());
                // if its in here, update its count, if not, forget about it
                if (refCountMap.containsKey(val))
                {
                    int newCount = refCountMap.get(val) - 1;
                    if (newCount > 0)
                        refCountMap.put(val, newCount);
                    else
                    {
                        destroyObject(val, object);
                    }
                }
            }
        }
    }

    /**
     * Request the Manager to manage (or not manage) the given object. If
     * manageIt is true, the management count is incremented. Otherwise, it is
     * decremented.
     * 
     * @param object
     * @param manageIt
     */
    protected void manageDestructibleObject(DestructibleObject object,
            boolean manageIt)
    {
        if (object.isValid())
        {
            synchronized (refCountMap)
            {
                String val = String.valueOf(object.getAddress());

                int newVal = getManagementCount(object) + (manageIt ? 1 : -1);
                nativeManagementMap.put(val, newVal);
                log.debug("Management count:  " + object.getInfo() + " = "
                        + newVal);
            }
        }
    }

    protected boolean isManaged(DestructibleObject object)
    {
        if (!object.isValid())
            return false;

        String val = String.valueOf(object.getAddress());
        // if nobody ever requested to manage or un-manage, then we own it
        if (!nativeManagementMap.containsKey(val))
            return true;
        // otherwise, see if we own it
        // if the difference between refcounts and management ownership requests
        // is greater than 1, we own it
        return getRefCount(object) + getManagementCount(object) > 1;
    }

    protected int getManagementCount(DestructibleObject object)
    {
        if (!object.isValid())
            return 0;

        String val = String.valueOf(object.getAddress());
        Integer curVal = nativeManagementMap.get(val);
        return curVal != null ? curVal : 0;
    }

    protected int getRefCount(NITFObject object)
    {
        if (!object.isValid())
            return 0;

        String val = String.valueOf(object.getAddress());
        Integer count = refCountMap.get(val);
        if (count == null)
            return 0;
        return count;
    }

    /**
     * Destroy the object and update all reference-counting maps, etc.
     * 
     * This should only be called within a synchronized block.
     * 
     * @param object
     */
    private void destroyObject(String memVal, DestructibleObject object)
    {
        if (object != null && object.isValid())
        {
            // only destroy if we are indeed managing the memory
            if (isManaged(object))
            {
                log.debug("Destroying object: " + object.getInfo());
                object.destructMemory();
                object.address = NITFObject.INVALID_ADDRESS;
            }
            refCountMap.remove(memVal);
            nativeManagementMap.remove(memVal);
        }
        else
            log.debug("Can't destroy object, already null: " + memVal);
        destructiblesMap.remove(memVal);
    }

    /**
     * This function will free ALL of the underlying native memory, thus making
     * all of existing Java objects usesless.
     * <p/>
     * Call this method when you are ready to exit the system, or when you are
     * surely never going to use any of the existing Java objects or their
     * functionalities anymore.
     * <p/>
     * This function will get called automatically when the application using it
     * shuts down, so you don't have to add it to your programs.
     */
    protected void freeAllMemory()
    {
        synchronized (refCountMap)
        {
            log.debug("Freeing all native memory");
            // copy the keyset so we don't get a list modification error
            Set<String> keySet = new HashSet<String>(destructiblesMap.keySet());
            for (String memVal : keySet)
            {
                DestructibleObject object = destructiblesMap.get(memVal);
                destroyObject(memVal, object);
            }
            destructiblesMap.clear();
            refCountMap.clear();

            // close the IOHandles as well
            closeAllIOHandles();
        }
    }

    /**
     * Tells the framework to keep track of the given IOHandle. This handle will
     * be freed with a call to {@link NITFResourceManager#closeAllIOHandles()}
     * 
     * @param handle
     */
    protected void trackIOHandle(IOInterface handle)
    {
        if (handle.getAddress() > 0)
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
     * handles.
     * <p/>
     * This function gets called by freeAllMemory()
     */
    protected void closeAllIOHandles()
    {
        synchronized (ioHandles)
        {
            for (Iterator<IOInterface> it = ioHandles.iterator(); it.hasNext();)
            {
                IOInterface handle = it.next();
                if (handle.isValid())
                {
                    try
                    {
                        handle.close();
                    }
                    catch (NITFException e)
                    {
                        // TODO
                    }
                }
            }
            ioHandles.clear();
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

    // private constructor
    private NITFResourceManager()
    {
    }

}
