# Search for all source files in the current directory
# and save their names to the DIR_Config_SRCS variable, separated by spaces
aux_source_directory(. DIR_Config_SRCS)

# Generating Link Libraries
add_library(Config ${DIR_Config_SRCS})
target_link_libraries(Config PUBLIC pico_stdlib hardware_spi hardware_i2c hardware_pwm hardware_adc)