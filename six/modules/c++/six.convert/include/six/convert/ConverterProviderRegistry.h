#ifndef __CONVERT_CONVERTER_PROVIDER_REGISTRY_H__
#define __CONVERT_CONVERTER_PROVIDER_REGISTRY_H__

#include <string>
#include <vector>

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
