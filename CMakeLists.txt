cmake_minimum_required(VERSION 3.15) # default install paths
project(QuaZip VERSION 1.0)

# CMP0042: Explicitly acknowledge MACOSX_RPATH
# (introduced in CMake 2.8.12, enabled by default in CMake 3.0,
# and producing a warning when unset since 3.7.1)
cmake_policy(SET CMP0042 NEW)

set(CMAKE_CXX_STANDARD 11)

set(QUAZIP_LIB_VERSION ${QuaZip_VERSION})
set(QUAZIP_LIB_SOVERSION 1.0.0)

option(BUILD_SHARED_LIBS "" ON)
set(QUAZIP_QT_MAJOR_VERSION 5 CACHE STRING "Qt version to use (4 or 5), defaults to 5")

if(NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE RELEASE)
endif()

enable_testing()
set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_DEBUG_POSTFIX d)

set(QUAZIP_LIB_FILE_NAME quazip${QuaZip_VERSION_MAJOR}-qt${QUAZIP_QT_MAJOR_VERSION})
set(QUAZIP_LIB_TARGET_NAME QuaZip)
set(QUAZIP_DIR_NAME QuaZip-Qt${QUAZIP_QT_MAJOR_VERSION}-${QUAZIP_LIB_VERSION})
set(QUAZIP_PACKAGE_NAME QuaZip-Qt${QUAZIP_QT_MAJOR_VERSION})
set(QUAZIP_PKGCONFIG_NAME quazip${QuaZip_VERSION_MAJOR}-qt${QUAZIP_QT_MAJOR_VERSION})
if(QUAZIP_QT_MAJOR_VERSION EQUAL 6)
	find_package(Qt6 REQUIRED COMPONENTS Core Core5Compat)
	set(QUAZIP_LIB_QT_LIBRARIES Qt6::Core Qt6::Core5Compat)
	set(QUAZIP_TEST_QT_LIBRARIES Qt6::Core Qt6::Core5Compat Qt6::Network Qt6::Test)
	set(QUAZIP_PKGCONFIG_REQUIRES Qt6Core)
elseif(QUAZIP_QT_MAJOR_VERSION EQUAL 5)
	find_package(Qt5 REQUIRED COMPONENTS Core)
	set(QUAZIP_LIB_QT_LIBRARIES Qt5::Core)
	set(QUAZIP_TEST_QT_LIBRARIES Qt5::Core Qt5::Network Qt5::Test)
	set(QUAZIP_PKGCONFIG_REQUIRES Qt5Core)
elseif(QUAZIP_QT_MAJOR_VERSION EQUAL 4)
	find_package(Qt4 4.5.0 REQUIRED COMPONENTS QtCore)
	set(QUAZIP_LIB_QT_LIBRARIES Qt4::QtCore)
	set(QUAZIP_TEST_QT_LIBRARIES Qt4::QtCore Qt4::QtNetwork Qt4::QtTest)
	set(QUAZIP_PKGCONFIG_REQUIRES QtCore)
else()
	message(FATAL_ERROR "Qt version ${QUAZIP_QT_MAJOR_VERSION} is not supported")
endif()

find_package(ZLIB REQUIRED)

add_subdirectory(quazip)

add_library(QuaZip::QuaZip ALIAS ${QUAZIP_LIB_TARGET_NAME})
add_subdirectory(qztest EXCLUDE_FROM_ALL)
add_test(NAME qztest_test
	COMMAND qztest
	WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/quazip # preliminary hack to find the dll on windows
)
add_custom_target(check COMMAND ${CMAKE_CTEST_COMMAND} --verbose DEPENDS qztest)

configure_file(quazip.pc.cmakein ${QUAZIP_PKGCONFIG_NAME}.pc @ONLY)
include(GNUInstallDirs) # configurable CMAKE_INSTALL_*DIR
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${QUAZIP_PKGCONFIG_NAME}.pc DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig)
