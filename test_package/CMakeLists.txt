# Test Xerces-C++ Conan package
# Dmitriy Vetutnev, ODANT, 2018


project(PackageTest CXX)
cmake_minimum_required(VERSION 3.0)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup(TARGETS)

enable_testing()

add_definitions(
    -DXSEC_BUILDING_TOOLS
)

# CONAN_PKG imported targets

add_executable(xtest_CONAN_PKG xtest.cpp)
target_link_libraries(xtest_CONAN_PKG CONAN_PKG::xmlsec-c)
add_test(
    NAME xtest_CONAN_PKG
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/bin
    COMMAND xtest_CONAN_PKG
)
if(UNIX)
    set_property(TEST xtest_CONAN_PKG PROPERTY ENVIRONMENT LD_DEBUG=libs LD_LIBRARY_PATH=${CMAKE_BINARY_DIR}/bin)
endif()


# Normal imported targets
find_package(XmlsecC REQUIRED)
add_executable(xtest xtest.cpp)
target_link_libraries(xtest XmlsecC::XmlsecC)
add_test(
    NAME xtest
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/bin
    COMMAND xtest
)
if(UNIX)
    set_property(TEST xtest PROPERTY ENVIRONMENT LD_DEBUG=libs LD_LIBRARY_PATH=${CMAKE_BINARY_DIR}/bin)
endif()
