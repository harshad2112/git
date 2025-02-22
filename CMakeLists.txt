cmake_minimum_required(VERSION 3.13)

project(git-starter-cpp)

find_package(OpenSSL REQUIRED)
find_package(ZLIB REQUIRED)

set(CMAKE_CXX_STANDARD 23) # Enable the C++23 standard

file(GLOB_RECURSE SOURCE_FILES src/*.cpp include/*.hpp)

add_executable(git-cpp ${SOURCE_FILES})

target_link_libraries(git-cpp PRIVATE ZLIB::ZLIB PRIVATE OpenSSL::Crypto)
