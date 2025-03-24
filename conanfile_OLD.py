from conan import ConanFile
from conan.tools.cmake import CMakeToolchain, CMake, cmake_layout, CMakeDeps

class BasicConanfile(ConanFile):
    name = "game"
    version = "0.1"
    package_type = "application"
    author = "Jonathan Rurka, jon.rurka@rukadynamics.net"
    description = "A basic recipe"
    license = "<Your project license goes here>"
    homepage = "<Your project homepage goes here>"

    #generators = "CMakeDeps", "cmake_find_package"
    settings = "os", "compiler", "build_type", "arch"
    options = {"shared": [True, False], "fPIC": [True, False]}
    default_options = {"shared": False, "fPIC": True}

    # Sources are located in the same place as this recipe, copy them to the recipe
    exports_sources = "CMakeLists.txt", "src/*"

    # The requirements method allows you to define the dependencies of your recipe
    def requirements(self):
        self.requires("vulkan-headers/1.3.243.0")
        self.requires("vulkan-memory-allocator/3.0.1")
        self.requires("glfw/3.3.8")
        self.requires("glm/1.0.1")
        pass
        
    def config_options(self):
        pass
    
    def configure(self):
        pass;

    def build_requirements(self):
        pass
        
    def layout(self):
        cmake_layout(self)

    def generate(self):
        deps = CMakeDeps(self)
        deps.generate()
        tc = CMakeToolchain(self, generator="Visual Studio 17 2022")
        tc.generate()
        pass

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()
        pass

    def package(self):
        cmake = CMake(self)
        cmake.install()
        pass
