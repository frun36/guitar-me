function(add_unity_test TEST_NAME)
    set(options "")
    set(oneValueArgs "")
    set(multiValueArgs INCLUDE SOURCES LINK_LIBS)

    cmake_parse_arguments(PARSE_ARGV 1 ARG "${options}" "${oneValueArgs}" "${multiValueArgs}")

    add_executable(${TEST_NAME} ${ARG_SOURCES})
    target_include_directories(${TEST_NAME} PRIVATE ${ARG_INCLUDE})
    target_link_libraries(${TEST_NAME} PRIVATE unity ${ARG_LINK_LIBS})
    add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME})
endfunction()

add_subdirectory(common)
