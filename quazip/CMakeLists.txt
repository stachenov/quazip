project(QuaZip_Library VERSION ${QUAZIP_LIB_VERSION})

include(GNUInstallDirs) # configurable CMAKE_INSTALL_*DIR

cmake_minimum_required(VERSION 3.13)

set(QUAZIP_HEADERS
    ${CMAKE_CURRENT_BINARY_DIR}/quazip_config.h
    JlCompress.h
    QuaCompress.h
    QuaExtract.h
    ioapi.h
    minizip_crypt.h
    quaadler32.h
    quachecksum32.h
    quacrc32.h
    quagzipfile.h
    quaziodevice.h
    quazip.h
    quazip_global.h
    quazip_qt_compat.h
    quazip_textcodec.h
    quazipdir.h
    quazipfile.h
    quazipfileinfo.h
    quazipnewinfo.h
    unzip.h
    zip.h
)

set(QUAZIP_SOURCES
    ${QUAZIP_HEADERS}
    unzip.c
    zip.c
    JlCompress.cpp
    JlOptions.cpp
    QuaCompress.cpp
    QuaExtract.cpp
    qioapi.cpp
    quaadler32.cpp
    quachecksum32.cpp
    quacrc32.cpp
    quagzipfile.cpp
    quaziodevice.cpp
    quazip.cpp
    quazip_textcodec.cpp
    quazipdir.cpp
    quazipfile.cpp
    quazipfileinfo.cpp
    quazipnewinfo.cpp)

set(QUAZIP_INCLUDE_PATH ${QUAZIP_DIR_NAME}/quazip)
set(QUAZIP_INSTALL_CONFIGDIR ${CMAKE_INSTALL_LIBDIR}/cmake)
set(QUAZIP_PKGCONFIG_NAME
    quazip${QuaZip_VERSION_MAJOR}-qt${QUAZIP_QT_MAJOR_VERSION})

add_library(${QUAZIP_LIB_TARGET_NAME}_object OBJECT ${QUAZIP_SOURCES})

target_include_directories(${QUAZIP_LIB_TARGET_NAME}_object
    PRIVATE
        $<BUILD_INTERFACE:${QUAZIP_INC}>
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>)

target_link_libraries(${QUAZIP_LIB_TARGET_NAME}_object
    PUBLIC ${QUAZIP_LIB_LIBRARIES} ${QUAZIP_LIB})

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(${QUAZIP_LIB_TARGET_NAME}_object
        PRIVATE -Wshadow -Werror=shadow)
endif()

# don't dllexport unless building shared
if(NOT QUAZIP_BUILD_SHARED)
    target_compile_definitions(${QUAZIP_LIB_TARGET_NAME}_object
        PRIVATE QUAZIP_STATIC)
endif()

# commonalities between shared and static
macro(quazip_lib libname)
    target_include_directories(${libname} PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/${QUAZIP_DIR_NAME}>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/${QUAZIP_INCLUDE_PATH}>)

    if(QUAZIP_LBD)
        target_link_directories(${libname} PUBLIC ${QUAZIP_LBD})
    endif()

    target_link_libraries(${libname}
        PUBLIC ${QUAZIP_LIB_LIBRARIES} ${QUAZIP_LIB})

    list(APPEND TO_INSTALL ${libname})
endmacro()

## SHARED ##
if(QUAZIP_BUILD_SHARED)
    set(libname ${QUAZIP_LIB_TARGET_NAME}_shared)
    add_library(${libname} SHARED
        $<TARGET_OBJECTS:${QUAZIP_LIB_TARGET_NAME}_object>)

    # shared libs require PIC
    set_property(TARGET ${QUAZIP_LIB_TARGET_NAME}_object
        PROPERTY POSITION_INDEPENDENT_CODE ON)

    # always export symbols when building a shared lib
    target_compile_definitions(${libname} PRIVATE QUAZIP_BUILD)
    target_compile_definitions(${QUAZIP_LIB_TARGET_NAME}_object
        PRIVATE QUAZIP_BUILD)

    set_target_properties(${libname} PROPERTIES
        VERSION ${QUAZIP_LIB_VERSION}
        SOVERSION ${QUAZIP_LIB_SOVERSION}
        OUTPUT_NAME ${QUAZIP_LIB_FILE_NAME}
        PUBLIC_HEADER "${QUAZIP_HEADERS}"
        EXPORT_NAME QuaZip)

    add_library(QuaZip::QuaZip ALIAS ${libname})

    quazip_lib(${libname})
endif()

## STATIC ##
if(QUAZIP_BUILD_STATIC)
    set(libname ${QUAZIP_LIB_TARGET_NAME}_static)
    add_library(${libname} STATIC
        $<TARGET_OBJECTS:${QUAZIP_LIB_TARGET_NAME}_object>)

    # consumers should not dllimport
    target_compile_definitions(${libname} PUBLIC QUAZIP_STATIC)

    set_target_properties(${libname} PROPERTIES
        OUTPUT_NAME ${QUAZIP_LIB_FILE_NAME}${QUAZIP_STATIC_SUFFIX})

    if(QUAZIP_BUILD_SHARED)
        add_library(QuaZip::QuaZip_static ALIAS ${libname})
        set_target_properties(${libname} PROPERTIES
            EXPORT_NAME QuaZip_static)
    else()
        add_library(QuaZip::QuaZip ALIAS ${libname})
        set_target_properties(${libname} PROPERTIES
            EXPORT_NAME QuaZip
            PUBLIC_HEADER "${QUAZIP_HEADERS}")
    endif()

    quazip_lib(${libname})
endif()

## INSTALL ##

include(CMakePackageConfigHelpers)

if(QUAZIP_INSTALL)
    if(QUAZIP_BUILD_SHARED AND QUAZIP_BUILD_STATIC)
        set(Flavor Both)
    elseif(QUAZIP_BUILD_SHARED)
        set(Flavor Shared)
    else()
        set(Flavor Static)
    endif()

    set(QUAZIP_EXPORT_SET ${QUAZIP_PACKAGE_NAME}_${Flavor}Targets)

    write_basic_package_version_file(
        ${PROJECT_BINARY_DIR}/${QUAZIP_PACKAGE_NAME}ConfigVersion.cmake
        COMPATIBILITY SameMajorVersion)

    configure_package_config_file(QuaZipConfig.cmake.in
        ${QUAZIP_PACKAGE_NAME}Config.cmake
        INSTALL_DESTINATION ${QUAZIP_INSTALL_CONFIGDIR}/${QUAZIP_DIR_NAME})

    install(TARGETS ${TO_INSTALL} ${QUAZIP_DEP}
        EXPORT ${QUAZIP_EXPORT_SET}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        PUBLIC_HEADER DESTINATION
            ${CMAKE_INSTALL_INCLUDEDIR}/${QUAZIP_INCLUDE_PATH})

    install(EXPORT ${QUAZIP_EXPORT_SET}
        NAMESPACE QuaZip::
        DESTINATION ${QUAZIP_INSTALL_CONFIGDIR}/${QUAZIP_DIR_NAME})

    install(FILES ${PROJECT_BINARY_DIR}/${QUAZIP_PACKAGE_NAME}Config.cmake
        ${PROJECT_BINARY_DIR}/${QUAZIP_PACKAGE_NAME}ConfigVersion.cmake
        DESTINATION ${QUAZIP_INSTALL_CONFIGDIR}/${QUAZIP_DIR_NAME})

    configure_file(quazip.pc.cmakein ${QUAZIP_PKGCONFIG_NAME}.pc @ONLY)
    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${QUAZIP_PKGCONFIG_NAME}.pc
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig)
endif()
