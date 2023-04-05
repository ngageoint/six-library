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

#ifndef __PLUGIN_ERROR_HANDLER_H__
#define __PLUGIN_ERROR_HANDLER_H__

#include <import/except.h>
#include <import/sys.h>
#include <import/logging.h>

namespace plugin
{
class ErrorHandler
{
public:
    ErrorHandler() {}

    virtual ~ErrorHandler() {}

    virtual void onPluginDirectoryNotFound(const std::string& dir) = 0;

    virtual void onPluginLoadedAlready(const std::string& file) = 0;

    virtual void onPluginLoadFailed(const std::string& file) = 0;

    virtual void onPluginVersionUnsupported(const std::string& message) = 0;

    virtual void onPluginError(except::Context& c) = 0;
};

class DefaultErrorHandler : public ErrorHandler
{
public:
    DefaultErrorHandler(logging::LoggerPtr logger = logging::LoggerPtr());

    void onPluginDirectoryNotFound(const std::string& dir) override;

    void onPluginLoadedAlready(const std::string& file) override;

    void onPluginLoadFailed(const std::string& file) override;

    void onPluginVersionUnsupported(const std::string& message) override;

    void onPluginError(except::Context& c) override;

protected:
    logging::LoggerPtr mLogger;
};

}

#endif
