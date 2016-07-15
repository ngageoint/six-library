/* =========================================================================
 * This file is part of NITRO
 * =========================================================================
 *
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
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

namespace nitf
{
void PluginRegistry::loadDir(const std::string& dirName) throw(NITFException)
{
    nitf_Error error;
    if (!nitf_PluginRegistry_loadDir(dirName.c_str(), &error))
        throw NITFException(&error);
}

void PluginRegistry::loadPlugin(const std::string& path) throw(NITFException)
{
    nitf_Error error;
    if (!nitf_PluginRegistry_loadPlugin(path.c_str(), &error))
        throw NITFException(&error);
}

void PluginRegistry::registerTREHandler(NITF_PLUGIN_INIT_FUNCTION init,
        NITF_PLUGIN_TRE_HANDLER_FUNCTION handler)
        throw(NITFException)
{
    nitf_Error error;
    if (!nitf_PluginRegistry_registerTREHandler(init, handler, &error))
        throw NITFException(&error);
}

nitf_CompressionInterface* PluginRegistry::retrieveCompressionInterface(
        const std::string& comp) throw(NITFException)
{
    nitf_Error error;
    nitf_CompressionInterface* const compIface =
            nitf_PluginRegistry_retrieveCompInterface(comp.c_str(), &error);
    if (compIface == NULL)
    {
        throw NITFException(&error);
    }

    return compIface;
}

bool PluginRegistry::treHandlerExists(const std::string& ident)
{
    return nitf_PluginRegistry_TREHandlerExists(ident.c_str());
}
}
