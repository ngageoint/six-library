set(TARGET_LANGUAGE c++)
set(JSON_ImplicitConversions OFF)
coda_add_module(nlohmann VERSION 1.0)
target_compile_definitions(nlohmann-c++ INTERFACE ENABLE_NLOHMANN)