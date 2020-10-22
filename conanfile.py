# Xerces-C++ Conan package
# Dmitriy Vetutnev, ODANT, 2018


from conans import ConanFile, CMake, tools
from conans.errors import ConanException
import os, glob, shutil


def get_safe(options, name):
    try:
        return getattr(options, name, None)
    except ConanException:
        return None

class XmlsecConan(ConanFile):
    name = "xmlsec-c"
    version = "2.0.2+5"
    license = "Apache License v2.0"
    description = "Xml-Security-C"
    url = "https://github.com/odant/conan-xmlsec-c"
    settings = "os", "compiler", "build_type", "arch"
    options = {
        "dll_sign": [False, True],
        "ninja": [False, True],
    }
    default_options = "dll_sign=True", "ninja=True"
    generators = "cmake"
    exports_sources = "src/*", "cmake/*", "FindXmlsecC.cmake", "CMakeLists.txt", "winapi.patch", "config.h", "XSECConfig.hpp"
    no_copy_source = True
    build_policy = "missing"

    def configure(self):
        if self.settings.compiler.get_safe("libcxx") == "libstdc++":
            raise Exception("This package is only compatible with libstdc++11")
        # DLL sign, only on Windows
        if self.settings.os != "Windows":
            del self.options.dll_sign

    def build_requirements(self):
        if get_safe(self.options, "ninja"):
            self.build_requires("ninja/1.10.1")
        if get_safe(self.options, "dll_sign"):
            self.build_requires("windows_signtool/[>=1.0]@%s/stable" % self.user)

    def source(self):
        if self.settings.os == "Windows":
            tools.patch(patch_file="winapi.patch")
        else:
            shutil.copy("config.h", "src/xsec/framework/config.h")
            shutil.copy("XSECConfig.hpp", "src/xsec/framework/XSECConfig.hpp")

    def requirements(self):
        self.requires("xerces-c/3.2.3+1@odant/stable")
        self.requires("openssl/1.1.1h+0@odant/stable")

    def build(self):
        build_type = "Debug" if self.settings.build_type == "Debug" else "RelWithDebInfo"
        generator = "Ninja" if self.options.ninja == True else None
        cmake = CMake(self, build_type=build_type, generator=generator)
        cmake.verbose = False
        cmake.configure()
        cmake.build()
        cmake.install()

    def package(self):
        self.copy("FindXmlsecC.cmake", src=".", dst=".")
        # Sign DLL
        if get_safe(self.options, "dll_sign"):
            import windows_signtool
            pattern = os.path.join(self.package_folder, "bin", "*.dll")
            for fpath in glob.glob(pattern):
                fpath = fpath.replace("\\", "/")
                for alg in ["sha1", "sha256"]:
                    is_timestamp = True if self.settings.build_type != "Debug" else False
                    cmd = windows_signtool.get_sign_command(fpath, digest_algorithm=alg, timestamp=is_timestamp)
                    self.output.info("Sign %s" % fpath)
                    self.run(cmd)

    def package_id(self):
        self.info.options.ninja = "any"

    def package_info(self):
        self.cpp_info.libs = tools.collect_libs(self)
        if self.settings.os == "Windows":
            self.cpp_info.defines = [
                "XSEC_HAVE_OPENSSL=1",
                "XSEC_HAVE_WINCAPI=1",
				"XSEC_XKMS_ENABLED=1"
            ]


