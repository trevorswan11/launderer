set(DEPS esp_adc driver)

idf_component_register(
    SRCS "pins.cpp"
    INCLUDE_DIRS "include"
    REQUIRES espressif__esp-idf-cxx
    PRIV_REQUIRES "${DEPS}"
)
