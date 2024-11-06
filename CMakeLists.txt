# Especificar la versión mínima de CMake
cmake_minimum_required(VERSION 3.13)

# Si la ruta no existe, mostrar un mensaje de error
if (NOT EXISTS ${PICO_SDK_PATH})
    message(FATAL_ERROR "La ruta PICO_SDK_PATH no es válida. Por favor, verifica la ruta en CMakeLists.txt.")
endif()

# Incluir el SDK de Pico
include(${PICO_SDK_PATH}/external/pico_sdk_import.cmake)

# Inicializar el SDK de Pico
pico_sdk_init()

# Añadir el ejecutable principal
add_executable(oled_display main.cpp)

# Enlazar el ejecutable con las bibliotecas necesarias
target_link_libraries(oled_display pico_stdlib hardware_i2c)

# Habilitar la salida extra (formato UF2 para cargar en la Raspberry Pi Pico)
pico_add_extra_outputs(oled_display)

# Configuración opcional para habilitar la consola USB y la salida UART para la depuración
pico_enable_stdio_usb(oled_display 1)
pico_enable_stdio_uart(oled_display 0)
