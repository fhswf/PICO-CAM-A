# Search for all source files in the current directory
# and save their names to the DIR_LCD_SRCS variable, separated by spaces
aux_source_directory(. DIR_LCD_SRCS)

include_directories(../Config)

# Generating Link Libraries
add_library(LCD ${DIR_LCD_SRCS})
target_link_libraries(LCD PUBLIC Config)