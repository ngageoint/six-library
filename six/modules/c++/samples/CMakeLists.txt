# add an executable
#
# Arguments:
#   name    - Target name of the executable. The source file should be ${name}.cpp
#   ARGN    - (remaining arguments) The dependencies of the executable.
function(add_sample name)
    add_executable(${name} "${name}.cpp")
    target_link_libraries(${name} ${ARGN})
    install(TARGETS ${name}
            ${CODA_INSTALL_OPTION}
            RUNTIME DESTINATION "bin")
endfunction()

add_sample(check_valid_six                      cli-c++ six.sicd-c++ six.sidd-c++)
add_sample(crop_sicd                            cli-c++ six.sicd-c++)
add_sample(crop_sidd                            cli-c++ six.sidd-c++)
add_sample(extract_cphd_xml                     cli-c++ cphd-c++ xml.lite-c++)
add_sample(image_to_scene                       six.sicd-c++ six.sidd-c++)
add_sample(project_slant_to_output              cli-c++ io-c++ six-c++ six.sicd-c++ sio.lite-c++)
add_sample(round_trip_six                       cli-c++ six.convert-c++ six.sicd-c++ six.sidd-c++)
add_sample(sicd_output_plane_pixel_to_lat_lon   cli-c++ six.sicd-c++)
add_sample(test_compare_sidd                    cli-c++ six.sicd-c++ six.sidd-c++)
add_sample(test_create_sicd                     cli-c++ six.sicd-c++ sio.lite-c++)
add_sample(test_create_sicd_from_mem            cli-c++ six.sicd-c++)
add_sample(test_create_sidd                     cli-c++ six.sicd-c++ six.sidd-c++ sio.lite-c++)
add_sample(test_create_sidd_from_mem            cli-c++ six.sicd-c++ six.sidd-c++)
add_sample(test_create_sidd_legend              cli-c++ six.sidd-c++)
add_sample(test_create_sidd_with_compressed_byte_provider   cli-c++ six.sidd-c++)
add_sample(test_dump_images                     cli-c++ six.sicd-c++ six.sidd-c++ sio.lite-c++)
add_sample(test_extract_xml                     cli-c++ nitf-c++ xml.lite-c++)
add_sample(test_image_to_scene                  six.sicd-c++ six.sidd-c++)
add_sample(test_large_offset                    six.sicd-c++ six.sidd-c++ io-c++ mem-c++)
add_sample(test_parse_xml                       six.sicd-c++ six.sidd-c++)
add_sample(test_six_xml_parsing                 six.sicd-c++ six.sidd-c++)
add_sample(update_sicd_version                  cli-c++ six.sicd-c++)
add_sample(update_sidd_version                  cli-c++ six.sidd-c++)
