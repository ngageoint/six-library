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

