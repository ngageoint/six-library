include(FindMatlab)
find_package(Matlab)

function(add_mex_module name extension dependencies)
    matlab_add_mex(NAME ${name}
                   SRC nitf/${name}.${extension}
                       nitf/${name}.def
                       nitf/${name}-64.def
                   LINK_TO ${dependencies})
    install(TARGETS ${name} DESTINATION "mex")
endfunction()

if (Matlab_FOUND)
    add_mex_module(nitf_image c nitf-c)
    add_mex_module(nitf_metadata c nitf-c)

    set(xml_metadata_dependencies nitf-c++ xml.lite-c++)
    add_mex_module(xml_metadata cpp "${xml_metadata_dependencies}")
else()
    message(WARNING "MATLAB installation not found. Skipping mex modules")
endif()
