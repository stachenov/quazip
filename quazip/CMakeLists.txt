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
        quazipnewinfo.cpp
   )

set(QUAZIP_INCLUDE_PATH ${QUAZIP_DIR_NAME}/quazip)
set(QUAZIP_INSTALL_CONFIGDIR ${CMAKE_INSTALL_LIBDIR}/cmake)
set(QUAZIP_PKGCONFIG_NAME quazip${QuaZip_VERSION_MAJOR}-qt${QUAZIP_QT_MAJOR_VERSION})

set(QUAZIP_OBJECT_TARGET_NAME QuaZipObjects)
add_library(${QUAZIP_OBJECT_TARGET_NAME} OBJECT ${QUAZIP_SOURCES})
set_target_properties(${QUAZIP_OBJECT_TARGET_NAME} PROPERTIES
    POSITION_INDEPENDENT_CODE ${QUAZIP_BUILD_SHARED}
)
target_include_directories(${QUAZIP_OBJECT_TARGET_NAME} PRIVATE
    ${QUAZIP_INC}
    ${QUAZIP_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
)
target_link_libraries(${QUAZIP_OBJECT_TARGET_NAME} PRIVATE
    ${QUAZIP_LIB_LIBRARIES}
    ${QUAZIP_LIB}
)
target_link_directories(${QUAZIP_OBJECT_TARGET_NAME} PRIVATE ${QUAZIP_LBD})

# The object files need export annotations whenever a shared library is built.
# Static consumers still receive QUAZIP_STATIC through their selected target.
if(QUAZIP_BUILD_SHARED)
    target_compile_definitions(${QUAZIP_OBJECT_TARGET_NAME} PRIVATE QUAZIP_BUILD)
else()
    target_compile_definitions(${QUAZIP_OBJECT_TARGET_NAME} PRIVATE QUAZIP_STATIC)
endif()

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(${QUAZIP_OBJECT_TARGET_NAME} PRIVATE -Wshadow -Werror=shadow)
endif()

function(quazip_add_library target_name library_type output_name export_name)
    add_library(${target_name} ${library_type} $<TARGET_OBJECTS:${QUAZIP_OBJECT_TARGET_NAME}>)
    set_target_properties(${target_name} PROPERTIES
        OUTPUT_NAME ${output_name}
        EXPORT_NAME ${export_name}
    )
    if(library_type STREQUAL "SHARED")
        set_target_properties(${target_name} PROPERTIES
            VERSION ${QUAZIP_LIB_VERSION}
            SOVERSION ${QUAZIP_LIB_SOVERSION}
        )
    else()
        target_compile_definitions(${target_name} INTERFACE QUAZIP_STATIC)
    endif()
    target_include_directories(${target_name} PUBLIC
        $<BUILD_INTERFACE:${QUAZIP_SOURCE_DIR}>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/${QUAZIP_DIR_NAME}>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/${QUAZIP_INCLUDE_PATH}>
    )
    target_link_libraries(${target_name} PUBLIC
        ${QUAZIP_LIB_LIBRARIES}
        ${QUAZIP_LIB}
    )
    target_link_directories(${target_name} PUBLIC ${QUAZIP_LBD})
endfunction()

set(QUAZIP_INSTALL_TARGETS)
set(QUAZIP_STATIC_LIB_FILE_NAME ${QUAZIP_LIB_FILE_NAME})
if(QUAZIP_BUILD_SHARED
        AND CMAKE_STATIC_LIBRARY_PREFIX STREQUAL CMAKE_IMPORT_LIBRARY_PREFIX
        AND CMAKE_STATIC_LIBRARY_SUFFIX STREQUAL CMAKE_IMPORT_LIBRARY_SUFFIX)
    set(QUAZIP_STATIC_LIB_FILE_NAME "${QUAZIP_LIB_FILE_NAME}-static")
endif()

if(QUAZIP_BUILD_SHARED)
    if(QUAZIP_PRIMARY_FLAVOR STREQUAL "Shared")
        set(QUAZIP_SHARED_TARGET_NAME ${QUAZIP_LIB_TARGET_NAME})
        set(QUAZIP_SHARED_EXPORT_NAME QuaZip)
    else()
        set(QUAZIP_SHARED_TARGET_NAME QuaZipShared)
        set(QUAZIP_SHARED_EXPORT_NAME QuaZipShared)
    endif()
    quazip_add_library(
        ${QUAZIP_SHARED_TARGET_NAME}
        SHARED
        ${QUAZIP_LIB_FILE_NAME}
        ${QUAZIP_SHARED_EXPORT_NAME}
    )
    list(APPEND QUAZIP_INSTALL_TARGETS ${QUAZIP_SHARED_TARGET_NAME})
    add_library(QuaZip::QuaZipShared ALIAS ${QUAZIP_SHARED_TARGET_NAME})
endif()

if(QUAZIP_BUILD_STATIC)
    if(QUAZIP_PRIMARY_FLAVOR STREQUAL "Static")
        set(QUAZIP_STATIC_TARGET_NAME ${QUAZIP_LIB_TARGET_NAME})
        set(QUAZIP_STATIC_EXPORT_NAME QuaZip)
    else()
        set(QUAZIP_STATIC_TARGET_NAME QuaZipStatic)
        set(QUAZIP_STATIC_EXPORT_NAME QuaZipStatic)
    endif()
    quazip_add_library(
        ${QUAZIP_STATIC_TARGET_NAME}
        STATIC
        ${QUAZIP_STATIC_LIB_FILE_NAME}
        ${QUAZIP_STATIC_EXPORT_NAME}
    )
    list(APPEND QUAZIP_INSTALL_TARGETS ${QUAZIP_STATIC_TARGET_NAME})
    add_library(QuaZip::QuaZipStatic ALIAS ${QUAZIP_STATIC_TARGET_NAME})
endif()

add_library(QuaZip::QuaZip ALIAS ${QUAZIP_LIB_TARGET_NAME})
set_target_properties(${QUAZIP_LIB_TARGET_NAME} PROPERTIES
    PUBLIC_HEADER "${QUAZIP_HEADERS}"
)

if(QUAZIP_PRIMARY_FLAVOR STREQUAL "Shared")
    set(QUAZIP_PKGCONFIG_LIB_FILE_NAME ${QUAZIP_LIB_FILE_NAME})
else()
    set(QUAZIP_PKGCONFIG_LIB_FILE_NAME ${QUAZIP_STATIC_LIB_FILE_NAME})
endif()

include(CMakePackageConfigHelpers)

if(QUAZIP_INSTALL)
    if(QUAZIP_BUILD_SHARED AND QUAZIP_BUILD_STATIC)
        set(Flavor Dual)
    elseif(QUAZIP_BUILD_SHARED)
        set(Flavor Shared)
    else()
        set(Flavor Static)
    endif()
    set(QUAZIP_EXPORT_SET ${QUAZIP_PACKAGE_NAME}_${Flavor}Targets)
    write_basic_package_version_file(${PROJECT_BINARY_DIR}/${QUAZIP_PACKAGE_NAME}ConfigVersion.cmake
        COMPATIBILITY SameMajorVersion
    )
    configure_package_config_file(QuaZipConfig.cmake.in ${QUAZIP_PACKAGE_NAME}Config.cmake
        INSTALL_DESTINATION ${QUAZIP_INSTALL_CONFIGDIR}/${QUAZIP_DIR_NAME}
    )
    install(TARGETS ${QUAZIP_INSTALL_TARGETS} ${QUAZIP_DEP}
        EXPORT ${QUAZIP_EXPORT_SET}
        PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${QUAZIP_INCLUDE_PATH}
    )
    install(EXPORT ${QUAZIP_EXPORT_SET}
        NAMESPACE QuaZip::
        DESTINATION ${QUAZIP_INSTALL_CONFIGDIR}/${QUAZIP_DIR_NAME}
    )
    install(FILES ${PROJECT_BINARY_DIR}/${QUAZIP_PACKAGE_NAME}Config.cmake
                  ${PROJECT_BINARY_DIR}/${QUAZIP_PACKAGE_NAME}ConfigVersion.cmake
        DESTINATION ${QUAZIP_INSTALL_CONFIGDIR}/${QUAZIP_DIR_NAME}
    )

    configure_file(quazip.pc.cmakein ${QUAZIP_PKGCONFIG_NAME}.pc @ONLY)
    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${QUAZIP_PKGCONFIG_NAME}.pc DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig)
endif()
