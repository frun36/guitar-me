add_unity_test(test_ring_buffer
    INCLUDE
        ${CMAKE_SOURCE_DIR}/common/include/
    SOURCES 
        ${CMAKE_SOURCE_DIR}/common/src/ring_buffer.c
        test_ring_buffer.c
)
