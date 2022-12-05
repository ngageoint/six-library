"""Conan recipe for building Nitro"""

from conans import ConanFile, CMake, tools
import os
import sys

class NitroConan(ConanFile):
    name = "nitro"
    url = "https://github.com/mdaus/nitro"
    description = "library for reading and writing the National Imagery Transmission Format (NITF)"
    settings = "os", "compiler", "build_type", "arch"
    requires = ("coda-oss/master_1ac97fe4897896fd", )
    options = {"shared": [True, False]}
    default_options = {"shared": False}
    exports_sources = ("CMakeLists.txt",
                       "LICENSE",
                       "README.md",
                       "cmake/*",
                       "modules/*",
                       )
    generators = "cmake_paths"
    license = "GNU LESSER GENERAL PUBLIC LICENSE Version 3"

    # default to short_paths mode (Windows only)
    short_paths = True
    # default the short_paths home to ~/.conan_short
    # this may be overridden by setting the environment variable
    # CONAN_USER_HOME_SHORT or setting user_home_short in ~/.conan/conan.conf
    if sys.platform.startswith('win32') and os.getenv("CONAN_USER_HOME_SHORT") is None:
        os.environ["CONAN_USER_HOME_SHORT"] = os.path.join(
            os.path.expanduser("~"), ".conan_short")

    def set_version(self):
        git = tools.Git(folder=self.recipe_folder)
        self.version = git.get_revision()[:16]

    def _configure_cmake(self):
        cmake = CMake(self)
        cmake.definitions["ENABLE_STATIC_TRES"] = True # always build static TRES
        cmake.definitions["ENABLE_J2K"] = self.options["coda-oss"].ENABLE_J2K
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
