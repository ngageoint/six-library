"""Conan recipe for xerces-c"""

from conans import ConanFile, CMake

class ZlibConan(ConanFile):
    name = "xerces-c"
    version = "3.2.3"
    url = "https://github.com/mdaus/coda-oss/tree/master/modules/drivers/xml/xerces"
    homepage = "https://xerces.apache.org/xerces-c/index.html"
    license = "Apache-2.0"
    description = (
        "Xerces-C++ is a validating XML parser written in a portable subset of C++"
    )
    settings = "os", "arch", "compiler", "build_type"
    options = {"shared": [True, False]}
    default_options = {"shared": False}
    exports_sources = [
        f"{name}-{version}.tar",
        "CMakeLists.txt",
        "config.h.in",
        "Xerces_autoconf_config.hpp.in",
        "cmake/coda-oss_xerces-cConfig.cmake.in",
        "../../../../cmake/CodaBuild.cmake"
    ]
    generators = "cmake"

    def _configure_cmake(self):
        cmake = CMake(self)
        cmake.configure()
        return cmake

    def build(self):
        cmake = self._configure_cmake()
        cmake.build()

    def package(self):
        cmake = self._configure_cmake()
        cmake.install()

    def package_id(self):
        # Make any change in our dependencies' version require a new binary
        self.info.requires.full_version_mode()

    def package_info(self):
        self.cpp_info.builddirs = ["lib/cmake"]
