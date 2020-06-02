project(QuaZip_Library VERSION ${QUAZIP_LIB_VERSION})

include(GNUInstallDirs) # configurable CMAKE_INSTALL_*DIR

aux_source_directory(. SRCS)
file(GLOB PUBLIC_HEADERS "*.h")
set(QUAZIP_INCLUDE_PATH ${QUAZIP_LIB_FILE_NAME}/quazip)

foreach(_lib ${_libs})
	add_library(${_lib} ${SRCS})
	set_target_properties(${_lib} PROPERTIES
		VERSION ${QUAZIP_LIB_VERSION}
		SOVERSION ${QUAZIP_LIB_SOVERSION}
	)
	target_include_directories(${_lib} PUBLIC 
		$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}>
		$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/${QUAZIP_INCLUDE_PATH}>
		$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/${QUAZIP_LIB_FILE_NAME}>
	)
	target_link_libraries(${_lib} ${QUAZIP_QTCORE_LIBRARIES} ZLIB::ZLIB)

	if(BUILD_SHARED_LIBS)
		target_compile_definitions(${_lib} PRIVATE QUAZIP_BUILD) # dllexport
	else()
		target_compile_definitions(${_lib} PUBLIC QUAZIP_STATIC) # suppress dllimport
	endif()
endforeach()

set_target_properties(${QUAZIP_LIB_TARGET_NAME} PROPERTIES
	OUTPUT_NAME ${QUAZIP_LIB_FILE_NAME}
	PUBLIC_HEADER "${PUBLIC_HEADERS}"
)

function(create_cmake_config _prefix _target _name _config_in)
	include(CMakePackageConfigHelpers)
	write_basic_package_version_file(${PROJECT_BINARY_DIR}/${_name}ConfigVersion.cmake
		COMPATIBILITY AnyNewerVersion
	)
	configure_package_config_file(${_config_in} ${_name}Config.cmake
		INSTALL_DESTINATION lib/cmake/${_name}
	)
	install(FILES ${PROJECT_BINARY_DIR}/${_name}Config.cmake
				  ${PROJECT_BINARY_DIR}/${_name}ConfigVersion.cmake
		DESTINATION lib/cmake/${_name}
	)

	if(BUILD_SHARED_LIBS)
		set(Flavor Shared)
	else()
		set(Flavor Static)
	endif()
	set(ExportSet ${_name}${Flavor}${_target}Targets)
	install(TARGETS ${_target}
		EXPORT ${ExportSet}
		PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${QUAZIP_INCLUDE_PATH}
	)
	install(EXPORT ${ExportSet}
		NAMESPACE ${_prefix}::
		DESTINATION lib/cmake/${_name}
	)
endfunction()

create_cmake_config(QuaZip ${QUAZIP_LIB_TARGET_NAME} QuaZip QuaZipConfig.cmake.in)

install(TARGETS quazip)
