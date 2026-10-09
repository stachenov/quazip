find_package(QuaZip-Qt${QUAZIP_QT_MAJOR_VERSION} CONFIG REQUIRED)

set(discovered_quazip_targets QuaZip::QuaZip)
if(TARGET QuaZip::QuaZip_static)
    list(APPEND discovered_quazip_targets QuaZip::QuaZip_static)
endif()
set(discovered_quazip_targets "${discovered_quazip_targets}" PARENT_SCOPE)
