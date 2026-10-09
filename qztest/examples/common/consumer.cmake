set(example_common_dir "${CMAKE_CURRENT_LIST_DIR}")

function(add_consumer_example name quazip_target expect_leaks)
    # Embed the selected library in a shared library. A static QuaZip must have
    # been compiled with PIC and hidden visibility to test export isolation.
    add_library(${name}_wrapper SHARED "${example_common_dir}/wrapper.cpp")
    target_link_libraries(${name}_wrapper PRIVATE ${quazip_target})
    target_compile_definitions(${name}_wrapper PRIVATE EXAMPLE_BUILD_WRAPPER)
    set_target_properties(${name}_wrapper PROPERTIES
        CXX_VISIBILITY_PRESET hidden
        VISIBILITY_INLINES_HIDDEN ON)

    add_executable(${name} "${example_common_dir}/main.cpp")
    target_link_libraries(${name} PRIVATE ${name}_wrapper ${quazip_target})
    target_compile_features(${name} PRIVATE cxx_std_17)
    target_compile_features(${name}_wrapper PRIVATE cxx_std_17)

    get_target_property(quazip_type ${quazip_target} TYPE)
    if(quazip_type STREQUAL "STATIC_LIBRARY")
        target_compile_definitions(${name} PRIVATE EXAMPLE_EXPECT_STATIC)
    elseif(expect_leaks)
        message(FATAL_ERROR "Symbol leakage is only expected when embedding static QuaZip")
    endif()
    add_test(NAME ${name}_roundtrip COMMAND ${name})

    # GNU/LLVM nm on ELF distinguishes our own exports from undefined imports.
    # Other platforms can run the consumer but need a different export inspector.
    if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
        if(NOT CMAKE_NM)
            message(FATAL_ERROR "nm is required for the Linux symbol visibility test")
        endif()
        add_test(NAME ${name}_symbols COMMAND "${CMAKE_COMMAND}"
            "-DNM=${CMAKE_NM}"
            "-DLIBRARY=$<TARGET_FILE:${name}_wrapper>"
            "-DEXPECT_LEAKS=${expect_leaks}"
            -P "${example_common_dir}/check-symbols.cmake")
    else()
        message(STATUS "Symbol visibility checks are implemented for Linux only")
    endif()
endfunction()
