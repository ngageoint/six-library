find_package(Threads REQUIRED)

add_library(scene-c++ source/AdjustableParams.cpp
                      source/CoordinateTransform.cpp
                      source/ECEFToLLATransform.cpp
                      source/EllipsoidModel.cpp
                      source/Errors.cpp
                      source/FrameType.cpp
                      source/GridECEFTransform.cpp
                      source/GridGeometry.cpp
                      source/LLAToECEFTransform.cpp
                      source/LocalCoordinateTransform.cpp
                      source/ProjectionModel.cpp
                      source/ProjectionPolynomialFitter.cpp
                      source/SceneGeometry.cpp
                      source/Types.cpp
                      source/Utilities.cpp)

common_module_config(scene-c++)
target_link_libraries(scene-c++ io-c++ math.linear-c++ polygon-c++ mem-c++
                                math-c++ sys-c++ str-c++
                                except-c++ types-c++ Threads::Threads)
add_dependencies(scene-c++ coda-oss)
