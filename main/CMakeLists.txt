set(DEPS nvs_flash driver hd44780 pcf8574)
set(MAIN_SRCS "placeholder.c")

idf_component_register(
    SRCS ${MAIN_SRCS}
    INCLUDE_DIRS "." "${CMAKE_SOURCE_DIR}/include"
    REQUIRES ${DEPS}
)

include(${CMAKE_SOURCE_DIR}/cmake/zig-config.cmake)

# FIXME: need add <cmath> for build kalman
if(TARGET __idf_espressif__esp-dsp AND IDF_VERSION_MAJOR EQUAL 6)
    target_compile_options(__idf_espressif__esp-dsp PRIVATE
        $<$<COMPILE_LANGUAGE:CXX>:-includecmath>
    )
endif()

