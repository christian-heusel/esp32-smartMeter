idf_component_register(SRCS "mqtt.c" "mqtt.cc"
                       INCLUDE_DIRS "include"
                       REQUIRES esp_event mqtt)
