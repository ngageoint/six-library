/* =========================================================================
 * This file is part of six.convert-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2017, MDA Information Systems LLC
 *
 * six.convert-c++ is free software; you can redistribute it and/or modify
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

#ifndef __SIX_CONVERTER_PROVIDER_REGISTRY_H__
#define __SIX_CONVERTER_PROVIDER_REGISTRY_H__

#include <string>
#include <vector>

#include <scene/sys_Conf.h>
#include <import/plugin.h>
#include <plugin/PluginDefines.h>
#include <six/convert/ConverterProvider.h>

namespace six
{
namespace convert
{
/*!
 * \class ConverterProviderRegistry
 * \brief Responsible for loading plugins to convert foreign files to SICDs
 */
class ConverterProviderRegistry : public
        plugin::BasicPluginManager<ConverterProvider>
{
public:
    /*!
     * Load a plugin able to support given file
     * Throw if unable to find supporting plugin
     *
     * \param pluginPathnames Paths to search for plugins
     * \param filePathname File to be converted by plugin
     */
    ConverterProvider* loadAndFind(
            const std::vector<std::string>& pluginPathnames,
            const std::string& filePathname);

    /*!
     * Load a plugin able to support given file
     * Throw if unable to find supporting plugin
     *
     * \param pluginPathname Path to search for plugins
     * \param filePathname File to be converted by plugin
     */
    ConverterProvider* loadAndFind(
            const std::string& pluginPathname,
            const std::string& filePathname);

    /*!
     * Get a pointer to the plugin that supports the file
     * loadAndFind() must be called first
     *
     * \param pathname File to be converted
     * \return pointer to appropriate converter
     */
    ConverterProvider* findPlugin(const std::string& pathname);
};
}
}

#endif
