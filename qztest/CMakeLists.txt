project(qztest)

find_package(Qt5 REQUIRED COMPONENTS Network Test)
if(NOT TARGET QuaZip::QuaZip)
    find_package(QuaZip REQUIRED)
endif()

set(QZTEST_SOURCES
        qztest.h
        testjlcompress.h
        testquachecksum32.h
        testquagzipfile.h
        testquaziodevice.h
        testquazip.h
        testquazipdir.h
        testquazipfile.h
        testquazipfileinfo.h
        testquazipnewinfo.h
        qztest.cpp
        testjlcompress.cpp
        testquachecksum32.cpp
        testquagzipfile.cpp
        testquaziodevice.cpp
        testquazip.cpp
        testquazipdir.cpp
        testquazipfile.cpp
        testquazipfileinfo.cpp
        testquazipnewinfo.cpp
)

add_executable(${PROJECT_NAME} ${QZTEST_SOURCES} qztest.qrc)
set_target_properties(${PROJECT_NAME} PROPERTIES AUTORCC ON)
target_link_libraries(${PROJECT_NAME}
    ${QUAZIP_TEST_QT_LIBRARIES}
    QuaZip::QuaZip
)
