set(XML_HOME "" CACHE PATH "path to pre-existing XML library installation, if not provided the XML library will be built")

set(CODA_XML_LAYER "XercesC" CACHE STRING "XML backend to use (currently only XercesC is supported)")
set("USE_${CODA_XML_LAYER}" TRUE CACHE INTERNAL "")

if (CODA_XML_LAYER STREQUAL "XercesC")
    add_subdirectory("xerces")
endif()
