"""Conan recipe for zlib"""

from conans import ConanFile, CMake


class ZlibConan(ConanFile):
    name = "zlib"
    version = "1.2.13"
    url = "https://github.com/mdaus/coda-oss/tree/master/modules/drivers/zlib"
    homepage = "https://zlib.net"
    license = "Zlib"
    description = ("A Massively Spiffy Yet Delicately Unobtrusive Compression Library "
                   "(Also Free, Not to Mention Unencumbered by Patents)")
    settings = "os", "arch", "compiler", "build_type"
    options = {"shared": [True, False]}
    default_options = {"shared": False}
    exports_sources = [
        f"{name}-{version}.tar",
        "CMakeLists.txt",
        "cmake/coda-oss_zlibConfig.cmake.in",
        "../../../cmake/CodaBuild.cmake"
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
