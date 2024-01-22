"""Conan recipe for zlib"""

from conans import ConanFile, CMake


class OpenjpegConan(ConanFile):
    name = "openjpeg"
    version = "2.3.1"
    url = "https://github.com/mdaus/coda-oss/tree/master/modules/drivers/j2k/openjpeg"
    homepage = "https://www.openjpeg.org/"
    license = "BSD 2-Clause"
    description = "OpenJPEG is an open-source JPEG 2000 codec written in C language."
    settings = "os", "arch", "compiler", "build_type"
    options = {"shared": [True, False]}
    default_options = {"shared": False}
    exports_sources = [
        f"{name}-{version}_mod.tar",
        "CMakeLists.txt",
        "opj_config_private.h.in",
        "opj_config.h.in",
        "cmake/coda-oss_openjpegConfig.cmake.in",
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
