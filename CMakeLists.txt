cmake_minimum_required(VERSION 3.16)

project(MiaKomp LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(miakomp
               src/main.cpp
               src/vm.cpp
               src/lexer.cpp
               src/parser.cpp)

target_compile_features(miakomp PRIVATE cxx_std_17)

if (MSVC)
    target_compile_options(miakomp PRIVATE /W4)

else()
    target_compile_options(miakomp PRIVATE -Wall -Wextra -pedantic)
    
endif()