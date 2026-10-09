execute_process(COMMAND "${NM}" -D --defined-only --demangle "${LIBRARY}"
    RESULT_VARIABLE result OUTPUT_VARIABLE symbols ERROR_VARIABLE error)
if(NOT result EQUAL 0)
    message(FATAL_ERROR "Cannot inspect ${LIBRARY}: ${error}")
endif()

# Prevent an empty or unsuitable symbol listing from passing the no-leak test.
if(NOT symbols MATCHES "[ \t]example_compress([\r\n]|$)")
    message(FATAL_ERROR "The wrapper's public example_compress export is missing")
endif()

# Print the complete export list so every leaked symbol is visible in CI.
if(symbols MATCHES "JlCompress|QuaZip")
    message(STATUS "QuaZip symbols leaked through ${LIBRARY}. Complete exports:\n${symbols}")
    if(NOT EXPECT_LEAKS)
        message(FATAL_ERROR "Unexpected QuaZip exports in ${LIBRARY}:\n${symbols}")
    endif()
    message(STATUS "Observed the expected QuaZip symbol leakage from reused objects")
elseif(EXPECT_LEAKS)
    message(FATAL_ERROR "Expected QuaZip symbol leakage from reused objects, but found none")
else()
    message(STATUS "No QuaZip symbols leaked through the wrapper")
endif()
