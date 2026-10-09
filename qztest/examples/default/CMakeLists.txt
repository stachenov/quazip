cmake_minimum_required(VERSION 3.15)
project(QuaZipDefaultConsumer LANGUAGES CXX)

set(QUAZIP_QT_MAJOR_VERSION 6 CACHE STRING "Qt major version of the installed QuaZip")
find_package(QuaZip-Qt${QUAZIP_QT_MAJOR_VERSION} CONFIG REQUIRED)

enable_testing()
include(../common/consumer.cmake)

# The same target works with shared-only, static-only, and combined packages.
add_consumer_example(default_consumer QuaZip::QuaZip OFF)
