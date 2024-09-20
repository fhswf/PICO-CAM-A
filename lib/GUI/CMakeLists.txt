# Search for all source files in the current directory
# and save their names to the DIR_GUI_SRCS variable, separated by spaces
aux_source_directory(. DIR_GUI_SRCS)

include_directories(../Config)

# Generating Link Libraries
add_library(GUI ${DIR_GUI_SRCS})
target_link_libraries(GUI PUBLIC Config)