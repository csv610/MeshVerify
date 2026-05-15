cmake_minimum_required(VERSION 3.10)
project(MeshVerify VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find Assimp
find_package(assimp REQUIRED)

# Find OpenMP
find_package(OpenMP)

# Python and nanobind integration
find_package(Python REQUIRED COMPONENTS Interpreter Development.Module)

include(FetchContent)
FetchContent_Declare(
  nanobind
  GIT_REPOSITORY https://github.com/wjakob/nanobind
  GIT_TAG        v2.4.0
  GIT_PROGRESS   TRUE
)
FetchContent_MakeAvailable(nanobind)

FetchContent_Declare(
  Catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG        v3.4.0
)
FetchContent_MakeAvailable(Catch2)

# Library target for reusable logic
add_library(mesh_verify_lib STATIC
    MeshVerifier.cpp 
    MeshLoader.cpp
    Predicates.cpp
    MeshTopology.cpp
)
target_include_directories(mesh_verify_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(mesh_verify_lib PRIVATE assimp::assimp)
if(OpenMP_CXX_FOUND)
    target_link_libraries(mesh_verify_lib PRIVATE OpenMP::OpenMP_CXX)
endif()

# CLI Executable
add_executable(mesh_verifier main.cpp)
target_link_libraries(mesh_verifier PRIVATE mesh_verify_lib)

# Python Module
nanobind_add_module(mesh_verify_py python_bindings.cpp)
target_link_libraries(mesh_verify_py PRIVATE mesh_verify_lib)

# Tests
add_executable(unit_tests 
    tests/unit_tests.cpp
    tests/functional_tests.cpp
    tests/integration_tests.cpp
)
target_link_libraries(unit_tests PRIVATE mesh_verify_lib Catch2::Catch2WithMain)
enable_testing()
add_test(NAME AllTests COMMAND unit_tests)

# Include directories
target_include_directories(mesh_verifier PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
