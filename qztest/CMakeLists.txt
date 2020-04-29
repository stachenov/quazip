project(qztest)

find_package(Qt5 REQUIRED COMPONENTS Network Test)
if(NOT TARGET QuaZip::QuaZip)
	find_package(QuaZip REQUIRED)
endif()

aux_source_directory(. SRCS)
add_executable(${PROJECT_NAME} ${SRCS} qztest.qrc)
set_target_properties(${PROJECT_NAME} PROPERTIES AUTORCC ON)
target_link_libraries(${PROJECT_NAME}
	QuaZip::QuaZip
	Qt5::Network
	Qt5::Test
)
