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

#include "nitf/PluginRegistry.hpp"

void nitf::PluginRegistry::loadDir(std::string dirName) throw(nitf::NITFException)
{
    nitf_Error error;
    if (!nitf_PluginRegistry_loadDir(dirName.c_str(), &error))
        throw nitf::NITFException(&error);
}

void nitf::PluginRegistry::loadPlugin(std::string path) throw(nitf::NITFException)
{
    nitf_Error error;
    if (!nitf_PluginRegistry_loadPlugin(path.c_str(), &error))
        throw nitf::NITFException(&error);
}

void nitf::PluginRegistry::registerTREHandler(NITF_PLUGIN_INIT_FUNCTION init,
        NITF_PLUGIN_TRE_HANDLER_FUNCTION handler)
        throw(nitf::NITFException)
{
    nitf_Error error;
    if (!nitf_PluginRegistry_registerTREHandler(init, handler, &error))
        throw nitf::NITFException(&error);
}
