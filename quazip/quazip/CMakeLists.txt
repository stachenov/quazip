# set all include directories for in and out of source builds
include_directories(
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_BINARY_DIR}
	${ZLIB_INCLUDE_DIRS}
)

file(GLOB SRCS "*.c" "*.cpp")
file(GLOB PUBLIC_HEADERS "*.h")

# Must be added to enable export macro
ADD_DEFINITIONS(-DQUAZIP_BUILD)

qt_wrap_cpp(MOC_SRCS ${PUBLIC_HEADERS})
set(SRCS ${SRCS} ${MOC_SRCS})

add_library(${QUAZIP_LIB_TARGET_NAME} SHARED ${SRCS})
set_target_properties(${QUAZIP_LIB_TARGET_NAME} PROPERTIES VERSION 1.0.0 SOVERSION 1)
# Link against ZLIB_LIBRARIES if needed (on Windows this variable is empty)
target_link_libraries(${QUAZIP_LIB_TARGET_NAME} ${QT_QTMAIN_LIBRARY} ${QTCORE_LIBRARIES} ${ZLIB_LIBRARIES})

install(FILES ${PUBLIC_HEADERS} DESTINATION include/quazip)
install(TARGETS ${QUAZIP_LIB_TARGET_NAME} LIBRARY DESTINATION ${LIB_DESTINATION} ARCHIVE DESTINATION ${LIB_DESTINATION} RUNTIME DESTINATION ${LIB_DESTINATION})
