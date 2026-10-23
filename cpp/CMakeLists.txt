cmake_minimum_required(VERSION 3.20)

project(cyn_cpp_vsc LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 源码使用 UTF-8（含中文注释 / 字符串），显式告诉 MSVC 按 UTF-8 解析
if(MSVC)
    add_compile_options(/utf-8)
endif()

# 多配置生成器（Visual Studio）下，把可执行文件统一放到 build/bin
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
foreach(config_name Debug Release RelWithDebInfo MinSizeRel)
    string(TOUPPER "${config_name}" config_upper)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${config_upper} "${CMAKE_BINARY_DIR}/bin")
endforeach()

add_library(cyn_formula STATIC src/random_formula.cpp)
target_include_directories(cyn_formula PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")

add_executable(cyn_cpp_app src/main.cpp)
target_link_libraries(cyn_cpp_app PRIVATE cyn_formula)

enable_testing()
add_executable(cyn_cpp_tests tests/test_main.cpp)
target_link_libraries(cyn_cpp_tests PRIVATE cyn_formula)
add_test(NAME cpp_random_formula_tests COMMAND cyn_cpp_tests)
