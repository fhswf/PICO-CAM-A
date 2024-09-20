# Search for all source files in the current directory
# and save their names to the DIR_examples_SRCS variable, separated by spaces
aux_source_directory(. DIR_examples_SRCS)

include_directories(../lib/Config)
include_directories(../lib/LCD)
include_directories(../lib/CAM)

# Generating Link Libraries
add_library(examples ${DIR_examples_SRCS})
target_link_libraries(examples pico_stdlib Config LCD CAM Fonts pico_multicore)