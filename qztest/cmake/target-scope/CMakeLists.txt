cmake_minimum_required(VERSION 3.15)
project(QuaZipTargetScope LANGUAGES CXX)

set(QUAZIP_QT_MAJOR_VERSION 6 CACHE STRING "Qt major version of the installed QuaZip")

# Discover the package in a child directory, then use its targets in the parent.
add_subdirectory(discover)
foreach(target IN LISTS discovered_quazip_targets)
    if(NOT TARGET ${target})
        message(FATAL_ERROR "${target} is not visible outside the find_package directory")
    endif()
    get_target_property(is_global ${target} IMPORTED_GLOBAL)
    if(NOT is_global)
        message(FATAL_ERROR "${target} is not globally visible")
    endif()
    message(STATUS "${target} is visible outside the find_package directory")
endforeach()

# Repeated discovery from another directory must also work.
find_package(QuaZip-Qt${QUAZIP_QT_MAJOR_VERSION} CONFIG REQUIRED)
