if (MSVC)
    add_compile_options(/wd4996) # '...': was declared deprecated
    add_compile_options(/wd4996) # '...': The POSIX name for this item is deprecated. Instead, use the ISO C and C++ conformant name: .... See online help for details.
    add_compile_options(/wd4005) # '...': macro redefinition
endif()

set(XML_HOME "" CACHE PATH "path to pre-existing XML library installation, if not provided the XML library will be built")

set(CODA_XML_LAYER "XercesC" CACHE STRING "XML backend to use (currently only XercesC is supported)")
set("USE_${CODA_XML_LAYER}" TRUE CACHE INTERNAL "")

if (CODA_XML_LAYER STREQUAL "XercesC")
    add_subdirectory("xerces")
endif()
