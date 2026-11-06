coda_add_module(
    scene
    DEPS io-c++ math.poly-c++ math.linear-c++
         polygon-c++ mem-c++ math-c++ sys-c++ str-c++
         except-c++ types-c++ config-c++ std-c++
    SOURCES
        source/AdjustableParams.cpp
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
