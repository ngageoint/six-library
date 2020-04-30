set(MODULE_NAME nrt)

coda_generate_module_config_header(${MODULE_NAME})

coda_add_module(
    ${MODULE_NAME}
    DEPS ${CMAKE_DL_LIBS} config-c++
    SOURCES
        source/DateTime.c
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

coda_add_tests(
    MODULE_NAME ${MODULE_NAME}
    DIRECTORY "unittests"
    UNITTEST
    SOURCES
        test_buffer_adapter.c
        test_core_values.c
        test_list.c
        test_nrt_byte_swap.c
        test_nrt_datetime.c
        test_tree.c
        test_utils.c)
