# Search for all source files in the current directory
# and save their names to the DIR_Fonts_SRCS variable, separated by spaces
aux_source_directory(. DIR_Fonts_SRCS)

# Generating Link Libraries
add_library(Fonts ${DIR_Fonts_SRCS})