cmake_minimum_required(VERSION 3.28)

# Eski dependency policy uyumluluğu
set(CMAKE_POLICY_VERSION_MINIMUM 3.5)
# VS Code'un başlık dosyalarını (FetchContent ile inenler dahil) bulması için
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
# DİKKAT: project(...) satırından ÖNCE derleyiciyi zorla tanımlıyoruz
if(APPLE)
    set(CMAKE_C_COMPILER "/usr/bin/clang")
    set(CMAKE_CXX_COMPILER "/usr/bin/clang++")
endif()

project(CMakeSFMLProject LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

include(FetchContent)

FetchContent_Declare(SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 3.0.0
    GIT_SHALLOW ON
    EXCLUDE_FROM_ALL
    SYSTEM
)

FetchContent_MakeAvailable(SFML)

add_executable(main src/main.cpp)

target_link_libraries(main PRIVATE SFML::Graphics)

if(APPLE)
    set_target_properties(main PROPERTIES
        MACOSX_BUNDLE TRUE
        INSTALL_RPATH "@executable_path/../Frameworks"
    )
endif()