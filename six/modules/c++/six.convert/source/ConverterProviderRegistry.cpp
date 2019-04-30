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
#include <six/convert/ConverterProviderRegistry.h>

namespace six
{
namespace convert
{
ConverterProvider* ConverterProviderRegistry::findPlugin(
        const std::string& pathname)
{
    std::vector<ConverterProvider*> allHandlers;
    getAllHandlers(allHandlers);

    for (size_t ii = 0; ii < allHandlers.size(); ++ii)
    {
        if (allHandlers[ii]->supports(pathname))
        {
            return allHandlers[ii];
        }
    }

    throw except::Exception(Ctxt(
            "No Converters able to support " + pathname));
}

ConverterProvider* ConverterProviderRegistry::loadAndFind(
        const std::vector<std::string>& pluginPathnames,
        const std::string& filePathname)
{
    plugin::DefaultErrorHandler errorHandler;
    plugin::BasicPluginManager<ConverterProvider>::load(pluginPathnames,
            &errorHandler);
    return findPlugin(filePathname);
}

ConverterProvider* ConverterProviderRegistry::loadAndFind(
        const std::string& pluginPathname,
        const std::string& filePathname)
{
    plugin::DefaultErrorHandler errorHandler;
    plugin::BasicPluginManager<ConverterProvider>::loadDir(pluginPathname,
            &errorHandler);
    return findPlugin(filePathname);
}

}
}

