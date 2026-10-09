cmake_minimum_required(VERSION 3.15)
project(QuaZipCombinedConsumer LANGUAGES CXX)

set(QUAZIP_QT_MAJOR_VERSION 6 CACHE STRING "Qt major version of the installed QuaZip")
find_package(QuaZip-Qt${QUAZIP_QT_MAJOR_VERSION} CONFIG REQUIRED)

option(EXAMPLE_EXPECT_SYMBOL_LEAKS
    "Expect shared-library export annotations in the selected static QuaZip archive" OFF)

# Use the separate static target when available, otherwise the canonical target.
if(TARGET QuaZip::QuaZip_static)
    set(quazip_target QuaZip::QuaZip_static)
    message(STATUS "Using QuaZip::QuaZip_static: separate static target is available")
else()
    set(quazip_target QuaZip::QuaZip)
    message(STATUS "Using QuaZip::QuaZip: no separate static target is available")
endif()

enable_testing()
include(../common/consumer.cmake)
add_consumer_example(combined_consumer ${quazip_target} ${EXAMPLE_EXPECT_SYMBOL_LEAKS})

# With a combined package, this exercises the shared variant as well.
if(TARGET QuaZip::QuaZip_static)
    add_consumer_example(canonical_consumer QuaZip::QuaZip OFF)
endif()
