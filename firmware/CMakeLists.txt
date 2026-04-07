cmake_minimum_required(VERSION 3.25)

project(guitar_me LANGUAGES C ASM)
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

include(FetchContent)
FetchContent_Declare(
  cmsis_dsp
  GIT_REPOSITORY https://github.com/ARM-software/CMSIS-DSP.git
  GIT_TAG        v1.15.0
)

# Sources
if(BOARD)
    if(NOT IS_READABLE ${CMAKE_CURRENT_SOURCE_DIR}/boards/${BOARD})
      message(FATAL_ERROR "The specified BOARD directory '${BOARD}' does not exist")
    endif()
    add_subdirectory(boards/${BOARD})

    FetchContent_MakeAvailable(cmsis_dsp)
    target_compile_options(CMSISDSP PRIVATE -w -O3)

    add_subdirectory(common)

    add_subdirectory(app)

    add_subdirectory(drivers)
endif()

# Tests
option(ENABLE_TESTING "Enable Unity unit tests" OFF)

if(ENABLE_TESTING)
    enable_testing()

    add_compile_options(-fsanitize=address -g -fno-omit-frame-pointer)
    add_link_options(-fsanitize=address)

    FetchContent_Declare(
        unity
        GIT_REPOSITORY https://github.com/ThrowTheSwitch/Unity.git
        GIT_TAG        v2.6.1
    )
    FetchContent_MakeAvailable(unity)

    add_subdirectory(tests)
endif()

