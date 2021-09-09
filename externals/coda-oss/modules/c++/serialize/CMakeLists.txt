if (Boost_serialization_FOUND)
    coda_add_module(
        serialize
        VERSION 1.0
        DEPS Boost::serialization)
endif()
