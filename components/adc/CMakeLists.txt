set(DEPS esp_adc driver)

idf_component_register(
    SRCS "adc.cpp"
    INCLUDE_DIRS "."
    PRIV_REQUIRES "${DEPS}"
)
