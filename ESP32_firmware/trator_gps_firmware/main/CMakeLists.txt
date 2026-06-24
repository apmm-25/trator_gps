idf_component_register(SRCS "main.c" 
                            "tasks/button_task.c"
                            "tasks/localization_task.c"
                            "tasks/web_page_management_task.c"

                            "gps/gps.c"
                            "serial_comms/serial_comms.c"
                            "web_app/web_app.c"
                            "controller/state_machine.c"
                       PRIV_REQUIRES spi_flash
                       
                       REQUIRES driver 
                                freertos 
                                esp_driver_uart 
                                esp_driver_gpio
                                esp_adc
                                esp_wifi
                                esp_event
                                esp_netif
                                nvs_flash
                                log
                                esp_http_server

                       INCLUDE_DIRS "."
                                    "controller"
                                    "gps"
                                    "serial_comms"
                                    "web_app"
                                    "defs"
                                    "tasks"
                        )