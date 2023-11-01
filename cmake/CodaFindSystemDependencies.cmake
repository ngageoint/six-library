# find and import system dependencies
macro(coda_find_system_dependencies)
    # creates imported target Threads::Threads
    # see https://cmake.org/cmake/help/latest/module/FindThreads.html
    set(THREADS_PREFER_PTHREAD_FLAG TRUE)
    find_package(Threads)

    # creates imported target CURL::libcurl, if found
    # see https://cmake.org/cmake/help/latest/module/FindCURL.html
    find_package(CURL)

    # sets OPENSSL_FOUND to TRUE if found, and creates targets
    # OpenSSL:SSL and OpenSSL::Crypto
    # see https://cmake.org/cmake/help/latest/module/FindOpenSSL.html
    find_package(OpenSSL)
endmacro()
