include(CheckIncludeFile)
include(CheckSymbolExists)

check_include_file("sys/time.h" HAVE_SYS_TIME_H)
check_symbol_exists("clock_gettime" "time.h" HAVE_CLOCK_GETTIME)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/include/nrt/nrt_config.h.cmake.in
               ${CMAKE_CURRENT_BINARY_DIR}/include/nrt/nrt_config.h)

add_library(nrt source/DateTime.c
                source/Debug.c
                source/DirectoryUnix.c
                source/DirectoryWin32.c
                source/DLLUnix.c
                source/DLLWin32.c
                source/Error.c
                source/HashTable.c
                source/IntStack.c
                source/IOHandleUnix.c
                source/IOHandleWin32.c
                source/IOInterface.c
                source/List.c
                source/Pair.c
                source/SyncIrix.c
                source/SyncUnix.c
                source/SyncWin32.c
                source/System.c
                source/Tree.c
                source/Utils.c)

target_link_libraries(nrt ${CMAKE_DL_LIBS})
common_module_config(nrt)

set(unittest_srcs test_buffer_adapter
                  test_core_values
                  test_list
                  test_nrt_byte_swap
                  test_nrt_datetime
                  test_tree
                  test_utils)

add_unittests("${unittest_srcs}" c nrt nrt)
