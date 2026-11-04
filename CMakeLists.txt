cmake_minimum_required(VERSION 3.22.1)
project(AdvancementsLeviLamroid LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

set(LEVILAMROID_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../LeviLamroid-reference" CACHE PATH "LeviLamroid source tree")
set(PRELOADER_INCLUDE_DIR "" CACHE PATH "Optional Preloader headers")
set(PRELOADER_LIBRARY "" CACHE FILEPATH "Optional Preloader ARM64 library")
foreach(required_file "${LEVILAMROID_DIR}/include/ll/api/ui/UI.h" "${LEVILAMROID_DIR}/include/ll/api/mod/RegisterHelper.h")
    if(NOT EXISTS "${required_file}")
        message(FATAL_ERROR "Required LeviLamroid/Preloader dependency not found: ${required_file}")
    endif()
endforeach()

add_library(AdvancementsLeviLamroid SHARED src/main.cpp)
target_include_directories(AdvancementsLeviLamroid PRIVATE "${CMAKE_SOURCE_DIR}/compat/include" include "${LEVILAMROID_DIR}/include" "${PRELOADER_INCLUDE_DIR}")
target_compile_options(AdvancementsLeviLamroid PRIVATE -include cstdlib -fexceptions -fvisibility=hidden -fvisibility-inlines-hidden -ffunction-sections -fdata-sections -Os -Wno-deprecated-volatile -Wno-return-type)
target_link_options(AdvancementsLeviLamroid PRIVATE -Wl,-z,max-page-size=16384 -Wl,--gc-sections -Wl,--as-needed -Wl,-z,relro -Wl,-z,now -Wl,--exclude-libs,ALL -Wl,--strip-all)
target_link_libraries(AdvancementsLeviLamroid PRIVATE android log dl c++_static c++abi)
if(PRELOADER_LIBRARY AND EXISTS "${PRELOADER_LIBRARY}")
    add_library(preloader SHARED IMPORTED GLOBAL)
    set_target_properties(preloader PROPERTIES IMPORTED_LOCATION "${PRELOADER_LIBRARY}")
    target_link_libraries(AdvancementsLeviLamroid PRIVATE preloader)
endif()

find_package(Python3 COMPONENTS Interpreter REQUIRED)
add_custom_command(TARGET AdvancementsLeviLamroid POST_BUILD
    COMMAND "${Python3_EXECUTABLE}" "${CMAKE_SOURCE_DIR}/scripts/package_levipack.py"
            --library "$<TARGET_FILE:AdvancementsLeviLamroid>"
            --icon "${CMAKE_SOURCE_DIR}/assets/icon.jpg"
            --output "$<TARGET_FILE_DIR:AdvancementsLeviLamroid>/AdvancementsLeviLamroid.levipack"
    VERBATIM)
