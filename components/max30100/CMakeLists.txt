idf_component_register(SRCS "iic.c" "max30100.c" "delay.c" "blood.c" "algorithm.c"
                    INCLUDE_DIRS "include" 
                    REQUIRES driver newlib log
                    )
