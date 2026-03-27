set(DEPS driver hd44780 pcf8574)

idf_component_register(
    SRCS "lcd.cpp"
    INCLUDE_DIRS "include"
    REQUIRES i2cdev
    PRIV_REQUIRES "${DEPS}"
)
