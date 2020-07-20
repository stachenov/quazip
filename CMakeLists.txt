cmake_minimum_required(VERSION 3.14) # default install paths
project(QuaZip)

# CMP0042: Explicitly acknowledge MACOSX_RPATH
# (introduced in CMake 2.8.12, enabled by default in CMake 3.0,
# and producing a warning when unset since 3.7.1)
cmake_policy(SET CMP0042 NEW)

set(QUAZIP_LIB_VERSION 1.0.0)
set(QUAZIP_LIB_SOVERSION 1)

option(BUILD_SHARED_LIBS "" ON)
option(BUILD_WITH_QT4 "Build QuaZip with Qt4 no matter if Qt5 was found" OFF)

if(NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE RELEASE)
endif()

enable_testing()
set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_DEBUG_POSTFIX d)

set(QUAZIP_LIB_FILE_NAME quazip)
if(WIN32)
	set(QUAZIP_LIB_FILE_NAME ${QUAZIP_LIB_FILE_NAME}${QUAZIP_LIB_SOVERSION})
endif()
if(NOT BUILD_WITH_QT4)
	set(CMAKE_CXX_STANDARD 11)
	find_package(Qt5 REQUIRED COMPONENTS Core)
	set(QUAZIP_QTCORE_LIBRARIES Qt5::Core)
	set(QUAZIP_LIB_FILE_NAME ${QUAZIP_LIB_FILE_NAME}-qt5)
	set(QUAZIP_LIB_TARGET_NAME Qt5)
else()
	find_package(Qt4 4.5.0 REQUIRED COMPONENTS QtCore)
	set(QUAZIP_QTCORE_LIBRARIES Qt4::QtCore)
	set(QUAZIP_LIB_FILE_NAME ${QUAZIP_LIB_FILE_NAME}-qt4)
	set(QUAZIP_LIB_TARGET_NAME Qt4)
endif()

find_package(ZLIB REQUIRED)

set(QUAZIP_LIB_NAME ${QUAZIP_LIB_FILE_NAME} CACHE INTERNAL "Target name of libquazip" FORCE)

add_subdirectory(quazip)

add_library(QuaZip::QuaZip ALIAS ${QUAZIP_LIB_TARGET_NAME})
add_subdirectory(qztest EXCLUDE_FROM_ALL)
add_test(NAME qztest_test
	COMMAND qztest
	WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/quazip # preliminary hack to find the dll on windows
)
add_custom_target(check COMMAND ${CMAKE_CTEST_COMMAND} --verbose DEPENDS qztest)

configure_file(quazip.pc.cmakein quazip.pc @ONLY)
include(GNUInstallDirs) # configurable CMAKE_INSTALL_*DIR
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/quazip.pc DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig)
