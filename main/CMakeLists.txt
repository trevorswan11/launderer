set(DEPS nvs_flash esp_adc driver hd44780 pcf8574)
set(MAIN_SRCS "placeholder.c")

idf_component_register(
    SRCS ${MAIN_SRCS}
    INCLUDE_DIRS "." "${CMAKE_SOURCE_DIR}/include"
    REQUIRES ${DEPS}
)

include(${CMAKE_SOURCE_DIR}/cmake/zig-config.cmake)

set(IMPLICIT_INC "$<JOIN:${CMAKE_C_IMPLICIT_INCLUDE_DIRECTORIES},;>")
set(TARGET_INC "$<JOIN:$<TARGET_PROPERTY:${COMPONENT_LIB},INCLUDE_DIRECTORIES>,;>")

set(INCLUDE_PATH_FILE "${CMAKE_BINARY_DIR}/include_dirs.txt")
file(GENERATE 
    OUTPUT "${INCLUDE_PATH_FILE}"
    CONTENT "${TARGET_INC};${IMPLICIT_INC}"
)