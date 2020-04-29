cmake_minimum_required(VERSION 2.8.5)
project(QuaZip)

# CMP0042: Explicitly acknowledge MACOSX_RPATH
# (introduced in CMake 2.8.12, enabled by default in CMake 3.0,
# and producing a warning when unset since 3.7.1)
cmake_policy(SET CMP0042 NEW)

set(QUAZIP_LIB_VERSION 1.0.0)
set(QUAZIP_LIB_SOVERSION 1)

option(BUILD_WITH_QT4 "Build QuaZip with Qt4 no matter if Qt5 was found" OFF)

include(GNUInstallDirs)

if(NOT BUILD_WITH_QT4)
    # try Qt5 first, and prefer that if found
    find_package(Qt5Core QUIET)
endif()

if(Qt5Core_FOUND)
    set(CMAKE_CXX_STANDARD 11)
    set(QTCORE_LIBRARIES ${Qt5Core_LIBRARIES})
    set(QUAZIP_LIB_VERSION_PREFIX Qt5)
    # if there is no QT_ROOT, try to deduce it from Qt QtCore include
    if("${QT_ROOT}" STREQUAL "")
        set(QT_ROOT ${QT_QTCORE_INCLUDE_DIR}/../..)
    endif()
    include_directories(${Qt5Core_INCLUDE_DIRS})

    macro(qt_wrap_cpp)
        qt5_wrap_cpp(${ARGN})
    endmacro()
else()
    set(qt_min_version "4.5.0")
    find_package(Qt4 REQUIRED)
    set(QT_USE_QTGUI false)
    include(${QT_USE_FILE})
    include_directories(${QT_INCLUDES})
    set(QTCORE_LIBRARIES ${QT_QTCORE_LIBRARY})
    set(QUAZIP_LIB_VERSION_PREFIX Qt)

    macro(qt_wrap_cpp)
        qt4_wrap_cpp(${ARGN})
    endmacro()
endif()

find_package(ZLIB REQUIRED)

set(QUAZIP_LIB_TARGET_NAME ${QUAZIP_LIB_VERSION_PREFIX}Quazip CACHE
    INTERNAL "Target name of libquazip" FORCE)

add_subdirectory(quazip)

if(UNIX AND NOT APPLE)
    configure_file(quazip.pc.cmakein quazip.pc @ONLY)
    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/quazip.pc DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig)
endif()

install(FILES QuaZipConfig.cmake
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/QuaZip${QUAZIP_LIB_VERSION_SUFFIX}
        RENAME QuaZip${QUAZIP_LIB_VERSION_SUFFIX}Config.cmake)
