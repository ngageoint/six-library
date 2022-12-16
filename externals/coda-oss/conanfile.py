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
from conans.client.conan_api import ConanAPIV1
import os
import sys

class CodaOssConan(ConanFile):
    name = "coda-oss"
    url = "https://github.com/mdaus/coda-oss"
    description = "Common Open Development Archive - OSS"
    scm = {
        "type": "git",
        "url": url + ".git",
        "revision": "auto",
        }
    settings = "os", "compiler", "build_type", "arch"
    options = {"shared": [True, False],
               "BOOST_HOME": "ANY",
               "PYTHON_HOME": "ANY",
               "PYTHON_VERSION": "ANY",
               "ENABLE_BOOST": [True, False],
               "ENABLE_PYTHON": [True, False],
               "ENABLE_SWIG": [True, False],
               "ENABLE_J2K": [True, False],
               "ENABLE_JARS": [True, False],
               "ENABLE_JPEG": [True, False],
               "ENABLE_PCRE": [True, False],
               "ENABLE_XML": [True, False],
               "ENABLE_ZIP": [True, False],
               "RE_ENABLE_STD_REGEX": [True, False],
               "CMAKE_DISABLE_FIND_PACKAGE_OpenSSL": [True, False],
               "CMAKE_DISABLE_FIND_PACKAGE_CURL": [True, False],
               "MT_DEFAULT_PINNING": [True, False],
               "CODA_BUILD_TESTS": [True, False],
               "CODA_INSTALL_TESTS": [True, False],
               }
    default_options = {"shared": False,
                       "BOOST_HOME": "",
                       "PYTHON_HOME": "",
                       "PYTHON_VERSION": "",
                       "ENABLE_BOOST": False,
                       "ENABLE_PYTHON": True,
                       "ENABLE_SWIG": False,
                       "ENABLE_J2K": True,
                       "ENABLE_JARS": True,
                       "ENABLE_JPEG": True,
                       "ENABLE_PCRE": True,
                       "ENABLE_XML": True,
                       "ENABLE_ZIP": True,
                       "RE_ENABLE_STD_REGEX": False,
                       "CMAKE_DISABLE_FIND_PACKAGE_OpenSSL": False,
                       "CMAKE_DISABLE_FIND_PACKAGE_CURL": False,
                       "MT_DEFAULT_PINNING": False,
                       "CODA_BUILD_TESTS": True,
                       "CODA_INSTALL_TESTS": False,
                       }
    license = "GNU LESSER GENERAL PUBLIC LICENSE Version 3"
    generators = ("cmake", "cmake_paths")

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

    def _get_in_tree_dependencies(self):
        # The in-tree dependencies ("drivers").
        # To automatically compile them when a package is not already built,
        # add the "--build outdated" option to the conan "create" or "install"
        # command line (otherwise it will fail).
        in_tree_dependencies = {
            "openjpeg": dict(
                version="2.3.1",
                user="coda",
                channel="driver",
                path=os.path.join("modules", "drivers", "j2k", "openjpeg"),
                enable_option="ENABLE_J2K",
            ),
            "libjpeg": dict(
                version="9d",
                user="coda",
                channel="driver",
                path=os.path.join("modules", "drivers", "jpeg"),
                enable_option="ENABLE_JPEG",
            ),
            "pcre2": dict(
                version="10.22",
                user="coda",
                channel="driver",
                path=os.path.join("modules", "drivers", "pcre"),
                enable_option="ENABLE_PCRE",
            ),
            "xerces-c": dict(
                version="3.2.3",
                user="coda",
                channel="driver",
                path=os.path.join("modules", "drivers", "xml", "xerces"),
                enable_option="ENABLE_XML",
            ),
            "zlib": dict(
                version="1.2.12",
                user="coda",
                channel="driver",
                path=os.path.join("modules", "drivers", "zlib"),
                enable_option="ENABLE_ZIP",
            ),
        }
        return in_tree_dependencies

    def export(self):
        # Export the driver dependency recipes when the coda-oss recipe is
        # exported.
        (cpm, _, _) = ConanAPIV1.factory()
        for dep_name, dep_info in self._get_in_tree_dependencies().items():
            cpm.export(os.path.join(self.recipe_folder, dep_info["path"]),
                       dep_name, dep_info["version"],
                       dep_info["user"], dep_info["channel"])

    def requirements(self):
        for dep_name, dep_info in self._get_in_tree_dependencies().items():
            if self.options.__getattr__(dep_info["enable_option"]):
                self.requires(f'{dep_name}/{dep_info["version"]}@{dep_info["user"]}/{dep_info["channel"]}')

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
