idf_component_register(SRCS "main.c" "localization_logic.c" "web_page_management.c" "button_logic.c"
                       PRIV_REQUIRES spi_flash
                       REQUIRES esp_driver_uart esp_driver_gpio
                       INCLUDE_DIRS ".")
