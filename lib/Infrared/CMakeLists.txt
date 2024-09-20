# Search for all source files in the current directory
# and save their names to the DIR_Infrared_SRCS variable, separated by spaces
aux_source_directory(. DIR_Infrared_SRCS)

include_directories(../Config)

# Generating Link Libraries
add_library(Infrared ${DIR_Infrared_SRCS})
target_link_libraries(Infrared PUBLIC Config)