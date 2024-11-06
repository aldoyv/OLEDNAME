#include "hardware_i2c.h"

// Dirección I2C de la pantalla OLED
#define OLED_I2C_ADDR 0x3C

// Dimensiones de la pantalla OLED
#define OLED_WIDTH 128
#define OLED_HEIGHT 64
#define OLED_PAGES (OLED_HEIGHT / 8)

// Comandos SSD1306
#define OLED_CMD_DISPLAY_OFF 0xAE
#define OLED_CMD_DISPLAY_ON 0xAF
#define OLED_CMD_SET_DISPLAY_CLOCK_DIV 0xD5
#define OLED_CMD_SET_MULTIPLEX 0xA8
#define OLED_CMD_SET_DISPLAY_OFFSET 0xD3
#define OLED_CMD_SET_START_LINE 0x40
#define OLED_CMD_CHARGE_PUMP 0x8D
#define OLED_CMD_MEMORY_MODE 0x20
#define OLED_CMD_SEG_REMAP 0xA1
#define OLED_CMD_COM_SCAN_DEC 0xC8
#define OLED_CMD_SET_CONTRAST 0x81
#define OLED_CMD_SET_PRECHARGE 0xD9
#define OLED_CMD_SET_VCOM_DETECT 0xDB
#define OLED_CMD_DISPLAY_ALL_ON_RESUME 0xA4
#define OLED_CMD_NORMAL_DISPLAY 0xA6

// Fuente 5x7 para caracteres ASCII (0x20 - 0x7F)
const uint8_t FONT_5x7[][5] = {
    // Definir caracteres ASCII de 0x20 (espacio) a 0x7F
    {0x00, 0x00, 0x00, 0x00, 0x00}, // Espacio
    {0x00, 0x00, 0x5F, 0x00, 0x00}, // !
    {0x00, 0x07, 0x00, 0x07, 0x00}, // "
    // ... Continuar para todos los caracteres
};

// Clase OLED con definición de caracteres y mapeo de píxeles
class OLED {
public:
    OLED(i2c_inst_t *i2c) : i2c(i2c) {}

    void init() {
        i2c_init(i2c, 100000);

        // Secuencia de inicialización SSD1306
        sendCommand(OLED_CMD_DISPLAY_OFF);
        sendCommand(OLED_CMD_SET_DISPLAY_CLOCK_DIV);
        sendCommand(0x80);
        sendCommand(OLED_CMD_SET_MULTIPLEX);
        sendCommand(OLED_HEIGHT - 1);
        sendCommand(OLED_CMD_SET_DISPLAY_OFFSET);
        sendCommand(0x00);
        sendCommand(OLED_CMD_SET_START_LINE | 0x00);
        sendCommand(OLED_CMD_CHARGE_PUMP);
        sendCommand(0x14);
        sendCommand(OLED_CMD_MEMORY_MODE);
        sendCommand(0x00);
        sendCommand(OLED_CMD_SEG_REMAP | 0x1);
        sendCommand(OLED_CMD_COM_SCAN_DEC);
        sendCommand(OLED_CMD_SET_CONTRAST);
        sendCommand(0x8F);
        sendCommand(OLED_CMD_SET_PRECHARGE);
        sendCommand(0xF1);
        sendCommand(OLED_CMD_SET_VCOM_DETECT);
        sendCommand(0x40);
        sendCommand(OLED_CMD_DISPLAY_ALL_ON_RESUME);
        sendCommand(OLED_CMD_NORMAL_DISPLAY);
        sendCommand(OLED_CMD_DISPLAY_ON);
    }

    // Muestra texto en cualquier posición de la pantalla
    void displayText(const char *text, uint8_t x, uint8_t y) {
        while (*text) {
            drawChar(*text++, x, y);
            x += 6; // Ancho de un carácter más un espacio
            if (x + 5 >= OLED_WIDTH) { // Si se excede el ancho de la pantalla
                x = 0;
                y += 8; // Avanza a la siguiente fila
            }
            if (y + 7 >= OLED_HEIGHT) break; // Evita escribir fuera de la pantalla
        }
        updateScreen();
    }

private:
    i2c_inst_t *i2c;
    uint8_t buffer[OLED_WIDTH * OLED_PAGES] = {0}; // Buffer de la pantalla

    void sendCommand(uint8_t command) {
        uint8_t data[2] = {0x00, command};
        i2c_write_blocking(i2c, OLED_I2C_ADDR, data, 2, false);
    }

    void sendData(uint8_t *data, size_t len) {
        i2c_write_blocking(i2c, OLED_I2C_ADDR, data, len, false);
    }

    // Dibuja un carácter en la posición (x, y) del buffer
    void drawChar(char c, uint8_t x, uint8_t y) {
        if (c < 0x20 || c > 0x7F) return; // Ignora caracteres fuera de rango

        uint8_t charIndex = c - 0x20;
        for (uint8_t i = 0; i < 5; i++) {
            uint8_t line = FONT_5x7[charIndex][i];
            for (uint8_t j = 0; j < 8; j++) {
                if (line & (1 << j)) {
                    setPixel(x + i, y + j, true);
                } else {
                    setPixel(x + i, y + j, false);
                }
            }
        }
    }

    // Configura un píxel en el buffer
    void setPixel(uint8_t x, uint8_t y, bool on) {
        if (x >= OLED_WIDTH || y >= OLED_HEIGHT) return;

        uint16_t index = x + (y / 8) * OLED_WIDTH;
        if (on) {
            buffer[index] |= (1 << (y % 8));
        } else {
            buffer[index] &= ~(1 << (y % 8));
        }
    }

    // Actualiza toda la pantalla con el contenido del buffer
    void updateScreen() {
        for (uint8_t page = 0; page < OLED_PAGES; page++) {
            sendCommand(0xB0 + page);  // Selecciona la página
            sendCommand(0x00);         // Columna baja
            sendCommand(0x10);         // Columna alta

            sendData(&buffer[OLED_WIDTH * page], OLED_WIDTH);
        }
    }
};

int main() {
    i2c_inst_t *i2c = i2c0;

    OLED oled(i2c);
    oled.init();
    oled.displayText("Nombre Apellido", 10, 10); // Mostrar en posición (10, 10)

    return 0;
}

