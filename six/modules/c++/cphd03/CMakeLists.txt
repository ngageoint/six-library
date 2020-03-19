add_library(cphd03-c++ source/Antenna.cpp
                       source/CPHDReader.cpp
                       source/CPHDWriter.cpp
                       source/CPHDXMLControl.cpp
                       source/Channel.cpp
                       source/Data.cpp
                       source/FileHeader.cpp
                       source/Global.cpp
                       source/Metadata.cpp
                       source/SRP.cpp
                       source/Utilities.cpp
                       source/VBM.cpp
                       source/VectorParameters.cpp)

common_module_config(cphd03-c++)
target_link_libraries(cphd03-c++ cphd-c++)

set(unittest_srcs test_cphd_read_unscaled_int
                  test_cphd_write
                  test_vbm)
add_unittests("${unittest_srcs}" cpp cphd03-c++ cphd03)
