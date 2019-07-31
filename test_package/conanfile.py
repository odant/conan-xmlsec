# Xerces-C++ Conan package
# Dmitriy Vetutnev, ODANT, 2018


from conans import ConanFile, CMake


class PackageTestConan(ConanFile):
    settings = "os", "compiler", "build_type", "arch"
    generators = "cmake"

    def imports(self):
        self.copy("*.pdb", dst="bin", src="bin")
        self.copy("*.dll", dst="bin", src="bin")
        self.copy("*.so*", dst="bin", src="lib")

    def build(self):
        cmake = CMake(self)
        cmake.verbose = True
        cmake.configure()
        cmake.build()

    def test(self):
        if self.settings.os == "Windows":
            self.run("ctest --verbose --build-config %s" % self.settings.build_type)
        else:
            self.run("ctest --verbose")
