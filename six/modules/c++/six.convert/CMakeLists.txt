coda_add_module(
    six.convert
    DEPS io-c++ plugin-c++ six.sicd-c++ sys-c++ xml.lite-c++
    SOURCES
        source/BaseConverter.cpp
        source/ConverterProviderRegistry.cpp
        source/ConvertingReadControl.cpp)
