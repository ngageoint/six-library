from conans import ConanFile, CMake
import os

class OpenjpegTestConan(ConanFile):
    settings = "os", "compiler", "build_type", "arch"
    generators = ("cmake", "cmake_paths")

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()

    def test(self):
        self.run(os.path.join("bin", "test"), run_environment=True)
