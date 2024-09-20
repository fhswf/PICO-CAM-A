cmake_minimum_required(VERSION 3.12)
include(pico_sdk_import.cmake)
project(rp2040_cam_code)
pico_sdk_init()

# Add subdirectories for compilation
add_subdirectory(lib/Config)
add_subdirectory(lib/Infrared)
add_subdirectory(lib/LCD)
add_subdirectory(lib/Fonts)
add_subdirectory(lib/GUI)
add_subdirectory(lib/CAM)
add_subdirectory(examples)

# Include header file directories
include_directories(examples)
include_directories(./lib/Config)
include_directories(./lib/GUI)
include_directories(./lib/LCD)
include_directories(./lib/Infrared)
include_directories(./lib/CAM)
include_directories(examples)

# Generate executable file
add_executable(main 
main.c
)

pico_generate_pio_header(main ${CMAKE_CURRENT_LIST_DIR}/lib/CAM/image.pio OUTPUT_DIR ${CMAKE_CURRENT_LIST_DIR}/lib/CAM/)

# enable usb output, disable uart output
pico_enable_stdio_usb(main 1)
pico_enable_stdio_uart(main 1)

# create map/bin/hex/uf2 file etc.
pico_add_extra_outputs(main)

target_link_libraries(main
	examples
	Config
	Infrared
	GUI
	CAM
	LCD
	Fonts
	pico_stdlib
	hardware_dma
	hardware_i2c
	hardware_pio
	hardware_pwm
	hardware_spi
	pico_multicore
)