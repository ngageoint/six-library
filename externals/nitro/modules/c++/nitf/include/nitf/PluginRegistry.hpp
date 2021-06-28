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

#ifndef __NITF_PLUGINREGISTRY_HPP__
#define __NITF_PLUGINREGISTRY_HPP__

#include <string>

#include "nitf/System.hpp"
#include "nitf/PluginRegistry.h"
#include "nitf/NITFException.hpp"
#include "nitf/exports.hpp"

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
namespace PluginRegistry
{
    /*!
     * Load plugins from the given directory
     * \param dirName  The name of the directory to load
     */
    extern NITRO_NITFCPP_API void loadDir(const std::string& dirName);

    extern NITRO_NITFCPP_API void loadPlugin(const std::string& path);

    /*!
     *  This function allows you to register your own TRE handlers.  It
     *  will override any handlers that are currently handling the identifier.
     */
    extern NITRO_NITFCPP_API void registerTREHandler(NITF_PLUGIN_INIT_FUNCTION init,
            NITF_PLUGIN_TRE_HANDLER_FUNCTION handler);

    /*!
     *  This function allows you to register your own compression handlers.  It
     *  will override any handlers that are currently handling the identifier.
     */
    extern NITRO_NITFCPP_API void registerCompressionHandler(NITF_PLUGIN_INIT_FUNCTION init,
            NITF_PLUGIN_COMPRESSION_CONSTRUCT_FUNCTION handler);

    /*!
     *  This function allows you to register your own decompression handlers.  It
     *  will override any handlers that are currently handling the identifier.
     */
    extern NITRO_NITFCPP_API void registerDecompressionHandler(NITF_PLUGIN_INIT_FUNCTION init,
            NITF_PLUGIN_DECOMPRESSION_CONSTRUCT_FUNCTION handler);

    extern NITRO_NITFCPP_API nitf_CompressionInterface* retrieveCompressionInterface(
            const std::string& comp);

    /*!
     * Checks if a TRE handler exists for 'ident'
     *
     * \param ident ID of the TRE
     *
     * \return true if a TRE handler exists, false otherwise
     */
    extern NITRO_NITFCPP_API bool treHandlerExists(const std::string& ident) noexcept;

    /*!
     * Checks if a compression handler exists for 'ident'
     *
     * \param ident ID of the compression
     *
     * \return true if a compression handler exists, false otherwise
     */
    extern NITRO_NITFCPP_API bool compressionHandlerExists(const std::string& ident) noexcept;

    /*!
     * Checks if a decompression handler exists for 'ident'
     *
     * \param ident ID of the decompression
     *
     * \return true if a decompression handler exists, false otherwise
     */
    extern NITRO_NITFCPP_API bool decompressionHandlerExists(const std::string& ident) noexcept;

    // For unit-testing
    extern NITRO_NITFCPP_API nitf_PluginRegistry* getInstance(nitf_Error&) noexcept;
    extern NITRO_NITFCPP_API nitf_TREHandler* retrieveTREHandler(nitf_PluginRegistry& reg, const std::string& treIdent, int& hadError, nitf_Error&) noexcept;
};
}
#endif
