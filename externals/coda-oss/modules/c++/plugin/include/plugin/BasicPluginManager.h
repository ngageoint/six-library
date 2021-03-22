/* =========================================================================
 * This file is part of plugin-c++ 
 * =========================================================================
 * 
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
 *
 * plugin-c++ is free software; you can redistribute it and/or modify
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

#ifndef __PLUGIN_BASIC_PLUGIN_MANAGER_H__
#define __PLUGIN_BASIC_PLUGIN_MANAGER_H__

#include <vector>
#include <map>

#include <import/sys.h>
#include <import/str.h>
#include <import/except.h>

#include <mem/SharedPtr.h>

#include "plugin/PluginDefines.h"
#include "plugin/ErrorHandler.h"

namespace plugin
{
/*!
 *  \class BasicPluginManager
 *  \brief Starting point for management of classes using plugins
 *
 *  This is a class to manage "plugins," where each plugin is a
 *  class, within a DLL, with a default constructor and a destructor.
 *  To be effective, another tier is required beyond this class, wherein
 *  the developer declares a virtual base class with all of the methods
 *  exposed that he or she wants to expose to calling applications.
 *  At the same time, the developer will either choose to implement this
 *  class as is by specializing its template argument to the base plugin
 *  class, or alternately overload this classes, exposing extended
 *  functionality (such as life-cycle management using init/deleteHandler),
 *  etc.
 *
 *  This class picks simple, reasonable defaults, and all of the methods
 *  of interest are virtual, to make them easy to override while inheriting
 *  the onerous plugin management code.
 *
 *  The careful plugin developer will be cautious when throwing exceptions,
 *  researching whether such behavior is supported by the target OSes when
 *  passed back from a DSO to the client application.  For this reason,
 *  it is recommended that the developer of the Plugin API (class) pass in
 *  an ErrorHandler to all public virtual functions in the plugin, and
 *  that the PluginManager implementor be diligent about not throwing
 *  exceptions in derived ErrorHandler when an plugin exception occurs.
 *
 *  A note to developers: the basic underlying data structure consists
 *  of the plugin identity, which simply relays basic meta-information
 *  about the nature of the plugin, and a single "handler," per operation.
 *  Thus, when designing a plugin system, it is important to consider
 *  whether an instance of your plugin is shareable between transactions
 *  (and potentially operations, if your plugin serves multiple ops),
 *  or whether your design requires an instance of the class for each
 *  transactions.  In the former case, it may be possible to make the
 *  handler the actual plugin, but in most cases, the handler is not
 *  the actual "plugin" class, but some creator pattern to produce
 *  classes that only it knows about.  A plugin then, consists of
 *  1) a PluginIdentity-derived struct
 *  2) a creator (factory) pattern
 *  3) a worker class that inherits an interface which performs
 *  the tasks required of the plugin
 */
template<typename T> class BasicPluginManager
{
public:
    typedef mem::SharedPtr<PluginIdentity<T> > SharedPluginIdentity;
    typedef std::map<std::string,
                     std::pair<T*, SharedPluginIdentity> >
        HandlerRegistry;

    /*!
     *  This is a default constructor, but its use is discouraged, since
     *  it uses the major and minor version of this module's API as the
     *  manager's major and minor version.  Consider using the alternative
     *  constructor.
     */
    BasicPluginManager() : mMajorVersion(PLUGIN_API_MAJOR_VERSION),
            mMinorVersion(PLUGIN_API_MINOR_VERSION)
    {}


    /*!
     *  Constructor.  Takes the major and minor version of the plugin
     *  manager.  This helps the developer keep track of a given plugin's
     *  compatibility with the manager.  If you need to modify this behavior
     *  override the pluginVersionSupported() function in this class.
     *
     *  \param majorVersion  The major version of this plugin manager
     *  \param minorVersion  The minor version of this plugin manager
     *
     */
    BasicPluginManager(int majorVersion, int minorVersion) :
            mMajorVersion(majorVersion),
            mMinorVersion(minorVersion)
    {}

    /*!
     *  Destructor.  Unloads registry if it has elements still 
     *  inside.
     *  \see unload()
     *
     */
    virtual ~BasicPluginManager()
    {
        try
        {
            unload();
        }
        catch (...)
        {
        }
    }

    /*!
     *  Load a set of plugins from the path specified.
     *
     *  \param path      The path of directories to load plugins from
     *  \param eh        An error handler to pass
     */
    void load(const std::vector<std::string>& path, 
              ErrorHandler* eh)
    {
        sys::OS os;

        //! throw if not present
        for (size_t i = 0; i < path.size(); ++i)
        {
            if (!os.exists(path[i]))
            {
                eh->onPluginDirectoryNotFound(path[i]);
            }
        }

        //! load all the shared libraries found
        std::vector<std::string> sharedLibs = 
            os.search(path, "", PLUGIN_DSO_EXTENSION, false);
        for (size_t i = 0; i < sharedLibs.size(); ++i)
        {
            loadPlugin(sharedLibs[i], eh);
        }
    }

    /*!
     *  Two iterations.  First, go through and use each plugin identity
     *  to destroy the plugin class.  Then re-run through the list,
     *  destroying each DSO.
     *
     */
    void unload()
    {
        typename HandlerRegistry::iterator it;
        for (it = mHandlers.begin(); it != mHandlers.end(); ++it)
        {
            PluginIdentity<T>& identity = *(it->second.second);
            //std::cout << typeid(identity).name() << std::endl;
            identity.destroyHandler(it->second.first);
        }
        mHandlers.clear();

        for (size_t ii = 0; ii < mDSOs.size(); ++ii)
        {
            delete mDSOs[ii];
        }
        mDSOs.clear();
    }

    /*!
     *  Get the plugin (handler) identified by the name.
     *
     *  \param name The name of the plugin to retrieve.
     *  \return The plugin handler
     */
    T* getHandler(const std::string& name)
    {
        typename HandlerRegistry::const_iterator it =
            mHandlers.find( name );
        if ( it != mHandlers.end() )
            return it->second.first;
        return nullptr;
    }

    /*!
     *  Syntactic sugar alternative to getHandler.  Overloaded [] op.
     *  Get the plugin (handler) identified by the name.
     *
     *  \param name The name of the plugin to retrieve.
     *  \return The plugin handler
     */
    T* operator[] (const std::string& name )
    {
        return getHandler(name);
    }

    /*!
     *  Get all the names associated with this handler
     *
     *  \param handler Pointer to a handler owned by this object
     *  \param names All the names associated with this handler
     */
    void getNames(const T* handler, std::vector<std::string>& names) const
    {
        names.clear();

        for (typename HandlerRegistry::const_iterator iter = mHandlers.begin();
             iter != mHandlers.end();
             ++iter)
        {
            if (iter->second.first == handler)
            {
                names.push_back(iter->first);
            }
        }
    }

    /*!
     *  Check if a plugin by this name exists in the registry.
     *
     *  \param name The name of the plugin
     *  \return True if the plugin exists, false otherwise.
     */
    bool exists(const std::string& name) const
    {
        typename HandlerRegistry::const_iterator it =
            mHandlers.find( name );
        return ( it != mHandlers.end() );
    }

    /*!
     *  Register a new plugin by identifier.
     *  This may be used by the application to load any qualifying plugin
     *  directly, including staticly linked identifiers.
     *
     *  \param identity The plugin identifier
     *  \param eh The error handler to be used if something bad happens
     */
    virtual void addHandler(mem::SharedPtr<PluginIdentity<T> > identity,
                            ErrorHandler* eh)
    {
        try
        {
            /*
             *  Check that the plugin identity is a qualifying version.  You
             *  can change what constitutes a valid version by overriding
             *  the pluginVersionSupported function.  Then, we check the
             *  operations that we are supporting.  There is a key to
             *  designate each of the ops.  We enter a newly constructed
             *  handler in the handler registry for each operation.
             */

            int majorVersion = identity->getMajorVersion();
            int minorVersion = identity->getMinorVersion();

            const char** ops = identity->getOperations();
            if (! pluginVersionSupported( majorVersion, minorVersion ) )
            {
                std::ostringstream oss;

                for (unsigned int i = 0; ops[i] != nullptr; i++)
                    oss << ops[i] << ":";
                eh->onPluginVersionUnsupported(
                    FmtX("For plugin supporting ops %s version [%d.%d] not supported (%d.%d)",
                         oss.str().c_str(), majorVersion, minorVersion,
                         mMajorVersion, mMinorVersion
                        )
                );
                return;
            }

            for (size_t i = 0; ops[i] != nullptr; ++i)
            {
                T* pluginHandler = identity->spawnHandler();
                if (! pluginHandler )
                {
                    eh->onPluginLoadFailed(
                        FmtX("Failed to spawn handler for op %s", ops[i]));
                    // Keep going
                }
                mHandlers[ops[i]].first = pluginHandler;
                mHandlers[ops[i]].second = identity;
            }
        }
        catch (const except::Exception& ex)
        {
            eh->onPluginLoadFailed(ex.getMessage());
        }
    }

    /*!
     *  Load the plugin identified by the file name.
     *  This may be used by the application to load specific plugins
     *  directly without the use of a directory, but it is not necessary.
     *
     *  \param file The full path to the plugin
     *  \param eh The error handler to be used if something bad happens
     */
    virtual void loadPlugin(const std::string& file, ErrorHandler* eh)
    {
        try
        {
            sys::DLL* dso = nullptr;
            bool loadDSO = true;

            std::string baseFile = sys::Path(file).getBasePath();

            // First check if the DSO is already loaded
            for (unsigned int i = 0; i < mDSOs.size(); ++i)
            {

                std::string baseLib =
                    sys::Path(mDSOs[i]->getLibName()).getBasePath();

                if (baseLib == baseFile)
                {

                    // Give the caller a chance to exit out
                    eh->onPluginLoadedAlready(file);
                    
                    // And if we are still here, we need to resume loading
                    dso = mDSOs[i];
                    loadDSO = false;
                    break;
                }
            }

            if (loadDSO)
            {
                // Load the DSO
                std::unique_ptr<sys::DLL> autoDSO(new sys::DLL(file));
                mDSOs.push_back(autoDSO.get());
                dso = autoDSO.release();
            }

            // Retrieve the plugin identity and add a handler to the registry.

            const void*(*ident)(void) =
                (const void*(*)(void))
                dso->retrieve(getPluginIdentName());

            const SharedPluginIdentity* const plugin =
                static_cast<const SharedPluginIdentity*>((*ident)());

            addHandler(*plugin, eh);
        }
        catch (const except::Exception& ex)
        {
            eh->onPluginLoadFailed(ex.getMessage());
        }
    }

    void getAllHandlers(std::vector<T*>& handlers)
    {
        typename HandlerRegistry::const_iterator p;

        for (p = mHandlers.begin(); p != mHandlers.end(); ++p)
        {
            handlers.push_back(p->second.first);
        }
    }
    void getAllKeys(std::vector<std::string>& handlerKeys)
    {
        typename HandlerRegistry::const_iterator p;

        for (p = mHandlers.begin(); p != mHandlers.end(); ++p)
        {
            handlerKeys.push_back(p->first);
        }
    }

    /*!
     *  Load the directory identified by name.  This function is called
     *  by load() for each directory in the path.
     *
     *  \param dirName   The name of the directory
     *  \param eh        The error handler to pass in, in case something
     *                   fails.
     */
    void loadDir(const std::string& dirName, 
                 ErrorHandler* eh)
    {
        load(std::vector<std::string>(1, dirName), eh);
    }

protected:

    virtual const char* getPluginIdentName() const
    {
        return GET_PLUGIN_IDENT;
    }


    /*!
     *  Override this call, for example, if you want to only
     *  reject functions when the major version is different.  The
     *  default behavior is to reject a plugin if either the major
     *  or the minor versions are different.
     *
     *  \param majorVersion The major version of the plugin
     *  \param minorVersion The minor version of the plugin
     */
    virtual bool pluginVersionSupported( int majorVersion, int minorVersion )
    {
        return ( (majorVersion == mMajorVersion) &&
                 (minorVersion == mMinorVersion) );
    }
    int mMajorVersion;
    int mMinorVersion;

private:
    HandlerRegistry        mHandlers;
    std::vector<sys::DLL*> mDSOs;
};

}
#endif
