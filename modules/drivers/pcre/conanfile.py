"""Conan recipe for zlib"""

from conans import ConanFile, CMake


class Pcre2Conan(ConanFile):
    name = "pcre2"
    version = "10.22"
    url = "https://github.com/mdaus/coda-oss/tree/master/modules/drivers/pcre"
    homepage = "https://www.pcre.org/"
    license = "BSD-3-Clause"
    description = "Perl Compatible Regular Expressions"
    settings = "os", "arch", "compiler", "build_type"
    options = {"shared": [True, False]}
    default_options = {"shared": False}
    exports_sources = [
        f"{name}-{version}.tar.gz",
        "CMakeLists.txt",
        "cmake/coda-oss_pcre2Config.cmake.in",
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
