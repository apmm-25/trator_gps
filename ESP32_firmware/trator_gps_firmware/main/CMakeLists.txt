idf_component_register(SRCS "tractor_controller_main.c"
                       PRIV_REQUIRES spi_flash
                       REQUIRES esp_driver_uart esp_driver_gpio
                       INCLUDE_DIRS ".")
