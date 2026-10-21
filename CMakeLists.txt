cmake_minimum_required(VERSION 3.22.1)
project(AdvancementsNMDK LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

set(NMDK_DIR "/tmp/NMDK" CACHE PATH "NMDK source tree")
set(NMDK_INCLUDE_DIR "${NMDK_DIR}/nmdk/include" CACHE PATH "NMDK public headers")
set(NMDK_LIBRARY "${NMDK_DIR}/nmdk/libs/arm64-v8a/libnmdk.so" CACHE FILEPATH "NMDK ARM64 library")
set(PRELOADER_INCLUDE_DIR "/home/ubuntu/.xmake/packages/p/preloader/@default/701c7a7bb9754ed4ad012d4198fbf39f/include" CACHE PATH "Preloader headers")
set(PRELOADER_LIBRARY "/home/ubuntu/.xmake/packages/p/preloader/@default/701c7a7bb9754ed4ad012d4198fbf39f/lib/libpreloader.so" CACHE FILEPATH "Preloader ARM64 library")
set(FMT_INCLUDE_DIR "" CACHE PATH "fmt include directory")

foreach(required_file "${NMDK_INCLUDE_DIR}/nmdk/NMDK.hpp" "${NMDK_LIBRARY}" "${PRELOADER_LIBRARY}" "${FMT_INCLUDE_DIR}/fmt/format.h")
    if(NOT EXISTS "${required_file}")
        message(FATAL_ERROR "Required NMDK/Preloader dependency not found: ${required_file}")
    endif()
endforeach()

add_library(nmdk SHARED IMPORTED GLOBAL)
set_target_properties(nmdk PROPERTIES IMPORTED_LOCATION "${NMDK_LIBRARY}")
add_library(preloader SHARED IMPORTED GLOBAL)
set_target_properties(preloader PROPERTIES
    IMPORTED_LOCATION "${PRELOADER_LIBRARY}"
    INTERFACE_INCLUDE_DIRECTORIES "${PRELOADER_INCLUDE_DIR}")

add_library(AdvancementsNMDK SHARED src/main.cpp)
target_include_directories(AdvancementsNMDK PRIVATE
    include "${NMDK_INCLUDE_DIR}" "${PRELOADER_INCLUDE_DIR}" "${FMT_INCLUDE_DIR}")
target_compile_options(AdvancementsNMDK PRIVATE
    -include cstdlib -fdeclspec -fexceptions -fvisibility=hidden -fvisibility-inlines-hidden
    -ffunction-sections -fdata-sections -Os -Wno-deprecated-volatile -Wno-return-type)
target_link_options(AdvancementsNMDK PRIVATE
    -Wl,-z,max-page-size=16384 -Wl,--gc-sections -Wl,--as-needed
    -Wl,-z,relro -Wl,-z,now -Wl,--exclude-libs,ALL -Wl,--strip-all)
target_link_libraries(AdvancementsNMDK PRIVATE nmdk preloader android log)

find_package(Python3 COMPONENTS Interpreter REQUIRED)
add_custom_command(TARGET AdvancementsNMDK POST_BUILD
    COMMAND "${Python3_EXECUTABLE}" "${CMAKE_SOURCE_DIR}/scripts/package_levipack.py"
            --library "$<TARGET_FILE:AdvancementsNMDK>"
            --icon "${CMAKE_SOURCE_DIR}/assets/icon.jpg"
            --output "$<TARGET_FILE_DIR:AdvancementsNMDK>/AdvancementsNMDK.levipack"
    VERBATIM)
