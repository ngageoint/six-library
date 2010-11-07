/* =========================================================================
 * This file is part of NITRO
 * =========================================================================
 *
 * (C) Copyright 2004 - 2010, General Dynamics - Advanced Information Systems
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

#ifndef __NITF_PLUGINREGISTRY_HPP__
#define __NITF_PLUGINREGISTRY_HPP__

#include "nitf/System.hpp"
#include "nitf/PluginRegistry.h"
#include "nitf/NITFException.hpp"
#include <string>

/*!
 *  \file PluginRegistry.hpp
 *  \brief  Contains wrapper implementation for PluginRegistry
 */

namespace nitf
{
/*!
 *  \class PluginRegistry
 *  \brief  The C++ wrapper for the nitf_PluginRegistry
 */
class PluginRegistry
{
public:

    /*!
     * Load plugins from the given directory
     * \param dirName  The name of the directory to load
     */
    static void loadDir(std::string dirName) throw(nitf::NITFException);

    static void loadPlugin(std::string path) throw(nitf::NITFException);

    /*!
     *  This function allows you to register your own TRE handlers.  It
     *  will override any handlers that are currently handling the identifier.
     */
    static void registerTREHandler(NITF_PLUGIN_INIT_FUNCTION init,
            NITF_PLUGIN_TRE_HANDLER_FUNCTION handler)
            throw(nitf::NITFException);

private:
    PluginRegistry()
    {
    }
    ~PluginRegistry()
    {
    }
};
}
#endif
