"""Conan recipe for zlib"""

from conans import ConanFile, CMake


class LibjpegConan(ConanFile):
    name = "libjpeg"
    version = "9d"
    url = "https://github.com/mdaus/coda-oss/tree/master/modules/drivers/jpeg"
    homepage = "https://ijg.org/"
    license = "https://ijg.org/files/README"
    description = "Libjpeg is a widely used C library for reading and writing JPEG image files."
    settings = "os", "arch", "compiler", "build_type"
    options = {"shared": [True, False]}
    default_options = {"shared": False}
    exports_sources = [
        f"jpegsrc.v{version}.tar",
        "CMakeLists.txt",
        "jconfig.h.in",
        "cmake/coda-oss_jpegConfig.cmake.in",
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
