idf_component_register(SRCS "main.c" 
                            "tasks/button_task.c"
                            "tasks/localization_task.c"
                            "tasks/web_page_management_task.c"
                            "tasks/state_machine_task.c"

                            "components/gps/gps.c"
                            "components/serial_comms/serial_comms.c"
                            "components/web_app/web_app.c"
                            "components/controller/state_machine.c"
                       PRIV_REQUIRES spi_flash
                       
                       REQUIRES driver 
                                freertos 
                                esp_driver_uart 
                                esp_driver_gpio
                                esp_adc
                                esp_wifi
                                esp_event
                                esp_netif
                                esp_timer
                                nvs_flash
                                log
                                esp_http_server
                                

                       INCLUDE_DIRS "."
                                   "components/controller"
                                   "components/gps"
                                   "components/serial_comms"
                                   "components/web_app"
                                   "defs"
                                   "tasks"
                        )