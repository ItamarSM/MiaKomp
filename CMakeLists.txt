cmake_minimum_required(VERSION 3.16)

project(MiaKomp LANGUAGES CXX)

add_executable(miakomp src/main.cpp)

target_compile_features(miakomp PRIVATE cxx_std_17)

if (MSVC)
    target_compile_options(miakomp PRIVATE /W4)

else()
    target_compile_options(miakomp PRIVATE -Wall -Wextra -pedantic)
    
endif()