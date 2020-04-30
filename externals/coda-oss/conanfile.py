"""Conan recipe for CODA-oss

Note: building packages on Windows usually requires enabling Conan's short_path
workaround due to issues handling paths longer than 260 characters on Windows.
To do so, set use_always_short_paths = True in ~/.conan/conan.conf, or set the
environment variable CONAN_USE_ALWAYS_SHORT_PATHS=1. To control where the build
directories are placed, set user_home_short in ~/.conan/conan.conf, or set the
environment variable CONAN_USER_HOME_SHORT. For further information see
https://docs.conan.io/en/latest/reference/conanfile/attributes.html#short-paths
"""

from conans import ConanFile, CMake, tools
import os
import sys

class CodaOssConan(ConanFile):
    name = "coda-oss"
    url = "https://github.com/mdaus/coda-oss"
    description = "Common Open Development Archive - OSS"
    settings = "os", "compiler", "build_type", "arch"
    options = {"shared": [True, False],
               "BOOST_HOME": "ANY",
               "PYTHON_HOME": "ANY",
               "PYTHON_VERSION": "ANY",
               "ENABLE_BOOST": [True, False],
               "ENABLE_PYTHON": [True, False],
               "MT_DEFAULT_PINNING": [True, False],
               }
    default_options = {"shared": False,
                       "BOOST_HOME": "",
                       "PYTHON_HOME": "",
                       "PYTHON_VERSION": "",
                       "ENABLE_BOOST": False,
                       "ENABLE_PYTHON": True,
                       "MT_DEFAULT_PINNING": False,
                       }
    exports_sources = ("CMakeLists.txt",
                       "LICENSE",
                       "README.md",
                       "build/*",
                       "cmake/*",
                       "modules/*",
                       )
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
        self.version = "%s_%s" % (git.get_branch(), git.get_revision()[:16])

    def _configure_cmake(self):
        cmake = CMake(self)
        # automatically foward all uppercase arguments to CMake
        for name, val in self.options.iteritems():
            if name.isupper() and val is not None:
                cmake.definitions[name] = val
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

        if len(self.info.options.BOOST_HOME) > 0:
            self.info.options.ENABLE_BOOST = True
        if len(self.info.options.PYTHON_HOME) > 0:
            self.info.options.ENABLE_PYTHON = True

        # make ABI independent of specific paths
        for name, val in self.info.options.as_list():
            if name.endswith("_HOME"):
                self.info.options.remove(name)

    def package_info(self):
        self.cpp_info.builddirs = ["lib/cmake"]
