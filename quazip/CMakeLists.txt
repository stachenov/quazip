project(QuaZip_Library VERSION ${QUAZIP_LIB_VERSION})

include(GNUInstallDirs) # configurable CMAKE_INSTALL_*DIR

aux_source_directory(. SRCS)
file(GLOB PUBLIC_HEADERS "*.h")
set(QUAZIP_INCLUDE_PATH ${QUAZIP_DIR_NAME}/quazip)

add_library(${QUAZIP_LIB_TARGET_NAME} ${SRCS})
set_target_properties(${QUAZIP_LIB_TARGET_NAME} PROPERTIES
	VERSION ${QUAZIP_LIB_VERSION}
	SOVERSION ${QUAZIP_LIB_SOVERSION}
)
target_include_directories(${QUAZIP_LIB_TARGET_NAME} PUBLIC 
	$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}>
	$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/${QUAZIP_DIR_NAME}>
	$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/${QUAZIP_INCLUDE_PATH}>
)
target_link_libraries(${QUAZIP_LIB_TARGET_NAME} ${QUAZIP_LIB_QT_LIBRARIES} ZLIB::ZLIB)

if(BUILD_SHARED_LIBS)
	target_compile_definitions(${QUAZIP_LIB_TARGET_NAME} PRIVATE QUAZIP_BUILD) # dllexport
else()
	target_compile_definitions(${QUAZIP_LIB_TARGET_NAME} PUBLIC QUAZIP_STATIC) # suppress dllimport
endif()

set_target_properties(${QUAZIP_LIB_TARGET_NAME} PROPERTIES
	OUTPUT_NAME ${QUAZIP_LIB_FILE_NAME}
	PUBLIC_HEADER "${PUBLIC_HEADERS}"
)

function(create_cmake_config _namespace _target _name _config_in)
	include(CMakePackageConfigHelpers)
	write_basic_package_version_file(${PROJECT_BINARY_DIR}/${_name}ConfigVersion.cmake
		COMPATIBILITY SameMajorVersion
	)
	configure_package_config_file(${_config_in} ${_name}Config.cmake
		INSTALL_DESTINATION lib/cmake/${QUAZIP_DIR_NAME}
	)
	install(FILES ${PROJECT_BINARY_DIR}/${_name}Config.cmake
				  ${PROJECT_BINARY_DIR}/${_name}ConfigVersion.cmake
		DESTINATION lib/cmake/${QUAZIP_DIR_NAME}
	)

	if(BUILD_SHARED_LIBS)
		set(Flavor Shared)
	else()
		set(Flavor Static)
	endif()
	set(ExportSet ${_name}_${Flavor}Targets)
	install(TARGETS ${_target}
		EXPORT ${ExportSet}
		PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${QUAZIP_INCLUDE_PATH}
	)
	install(EXPORT ${ExportSet}
		NAMESPACE ${_namespace}::
		DESTINATION lib/cmake/${QUAZIP_DIR_NAME}
	)
endfunction()

create_cmake_config(QuaZip ${QUAZIP_LIB_TARGET_NAME} ${QUAZIP_PACKAGE_NAME} QuaZipConfig.cmake.in)
