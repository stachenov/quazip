from conan import ConanFile

class Bncsutil(ConanFile):
    settings = "os", "compiler", "build_type", "arch"
    generators = "CMakeDeps", "CMakeToolchain"

    requires = (
        "zlib/1.3.1",
        "bzip2/1.0.8"
    )