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

#include "plugin/ErrorHandler.h"

plugin::DefaultErrorHandler::DefaultErrorHandler() {}
plugin::DefaultErrorHandler::~DefaultErrorHandler() {}

void plugin::DefaultErrorHandler::
onPluginDirectoryNotFound(const std::string& dir)
{
    throw except::FileNotFoundException(
        Ctxt(std::string("Plugin directory not found: ") +
             dir)

    );
}

void plugin::DefaultErrorHandler::onPluginLoadFailed(const std::string& file)
{
    std::cout << "Warning: plugin manager failed to load: " << file << std::endl;
}

void plugin::DefaultErrorHandler::onPluginLoadedAlready(const std::string& file)
{
    std::cout << "Warning: plugin manager already loaded: " << file << std::endl;
}


void plugin::DefaultErrorHandler::onPluginVersionUnsupported(const std::string& message)
{
    std::cout << "Warning: " << message << std::endl;
}
void plugin::DefaultErrorHandler::onPluginError(except::Context&)
{
}

