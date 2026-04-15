set(DEPS driver hx711)

idf_component_register(
    SRCS "load_cell.cpp"
    INCLUDE_DIRS "."
    PRIV_REQUIRES "${DEPS}"
)
