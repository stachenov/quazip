cmake_minimum_required(VERSION 3.15)

project(QuaZipDualBuildConsumer LANGUAGES CXX)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core)
find_package(QuaZip-Qt${QT_VERSION_MAJOR} CONFIG REQUIRED)

# Loading the package more than once must preserve all imported targets.
find_package(QuaZip-Qt${QT_VERSION_MAJOR} CONFIG REQUIRED)

foreach(quazip_target
        QuaZip::QuaZip
        QuaZip::QuaZipShared
        QuaZip::QuaZipStatic)
    if(NOT TARGET ${quazip_target})
        message(FATAL_ERROR "Missing imported target: ${quazip_target}")
    endif()
endforeach()

get_target_property(quazip_default_type QuaZip::QuaZip TYPE)
get_target_property(quazip_shared_type QuaZip::QuaZipShared TYPE)
get_target_property(quazip_static_type QuaZip::QuaZipStatic TYPE)
if(NOT quazip_default_type STREQUAL "SHARED_LIBRARY"
        OR NOT quazip_shared_type STREQUAL "SHARED_LIBRARY"
        OR NOT quazip_static_type STREQUAL "STATIC_LIBRARY")
    message(FATAL_ERROR "Unexpected imported target types")
endif()

get_target_property(
    quazip_static_definitions
    QuaZip::QuaZipStatic
    INTERFACE_COMPILE_DEFINITIONS
)
if(NOT "QUAZIP_STATIC" IN_LIST quazip_static_definitions)
    message(FATAL_ERROR "QuaZip::QuaZipStatic must define QUAZIP_STATIC")
endif()

enable_testing()
function(add_quazip_consumer flavor quazip_target)
    add_executable(consumer${flavor} main.cpp)
    target_link_libraries(consumer${flavor} PRIVATE ${quazip_target})
    add_test(NAME consumer${flavor} COMMAND consumer${flavor})
endfunction()

add_quazip_consumer(Default QuaZip::QuaZip)
add_quazip_consumer(Shared QuaZip::QuaZipShared)
add_quazip_consumer(Static QuaZip::QuaZipStatic)
