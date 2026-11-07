cmake_minimum_required(VERSION 3.22.1)
project(Advancements LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_SKIP_RPATH ON)

set(PRELOADER_INCLUDE_DIR "" CACHE PATH "Optional Preloader headers")
set(PRELOADER_LIBRARY "" CACHE FILEPATH "Optional Preloader ARM64 library")

add_library(Advancements SHARED src/main.cpp)
target_include_directories(Advancements PRIVATE "${PRELOADER_INCLUDE_DIR}" "${CMAKE_SOURCE_DIR}/compat/include" include "${CMAKE_SOURCE_DIR}/vendor/LeviLamroid/include")
target_compile_options(Advancements PRIVATE -include cstdlib -fexceptions -fvisibility=hidden -fvisibility-inlines-hidden -ffunction-sections -fdata-sections -Os -Wno-deprecated-volatile -Wno-return-type)
target_link_options(Advancements PRIVATE -Wl,-z,max-page-size=16384 -Wl,--gc-sections -Wl,--as-needed -Wl,-z,relro -Wl,-z,now -Wl,--exclude-libs,ALL -Wl,--strip-all)
target_link_libraries(Advancements PRIVATE android log dl c++_shared)
if(PRELOADER_LIBRARY AND EXISTS "${PRELOADER_LIBRARY}")
    add_library(preloader SHARED IMPORTED GLOBAL)
    set_target_properties(preloader PROPERTIES IMPORTED_LOCATION "${PRELOADER_LIBRARY}")
    target_link_libraries(Advancements PRIVATE preloader)
endif()

find_package(Python3 COMPONENTS Interpreter REQUIRED)
add_custom_command(TARGET Advancements POST_BUILD
    COMMAND "${Python3_EXECUTABLE}" "${CMAKE_SOURCE_DIR}/scripts/package_levipack.py"
            --library "$<TARGET_FILE:Advancements>"
            --icon "${CMAKE_SOURCE_DIR}/assets/icon.jpg"
            --output "$<TARGET_FILE_DIR:Advancements>/Advancements.levipack"
    VERBATIM)
