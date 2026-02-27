cmake_minimum_required(VERSION 3.16)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(launderer)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_custom_target(format
    COMMAND ${PYTHON} scripts/format_files.py
    WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
)
