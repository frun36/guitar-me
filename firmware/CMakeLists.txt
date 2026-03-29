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
if(NOT IS_READABLE ${CMAKE_CURRENT_SOURCE_DIR}/boards/${BOARD})
  message(FATAL_ERROR "The specified BOARD directory '${BOARD}' does not exist")
endif()
add_subdirectory(boards/${BOARD})

FetchContent_MakeAvailable(cmsis_dsp)
target_compile_options(CMSISDSP PRIVATE -w -O3)

add_subdirectory(app)

# add_subdirectory(drivers)
#
# add_subdirectory(utils)
