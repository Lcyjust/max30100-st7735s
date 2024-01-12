
file(GLOB USER_INCLUDE_PATH ./src/*.c)


idf_component_register(SRCS "lcd.c" "spi.c" "delay.c"
                    INCLUDE_DIRS "include"
                    PRIV_REQUIRES driver  esp_rom log  esp_common newlib
                    )
