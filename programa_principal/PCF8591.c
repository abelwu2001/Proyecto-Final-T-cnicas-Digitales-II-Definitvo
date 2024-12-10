#include <pigpio.h>
#include <stdio.h>
#include <stdint.h>

#define PCF8591_I2C_ADDR 0x48 // Dirección I2C del PCF8591

int main() {
    if (gpioInitialise() < 0) {
        printf("Error al inicializar pigpio.\n");
        return 1;
    }

    int handle = i2cOpen(1, PCF8591_I2C_ADDR, 0); // Abrir conexión I2C
    if (handle < 0) {
        printf("Error al abrir conexión I2C.\n");
        gpioTerminate();
        return 1;
    }

    uint8_t command = 0x40; // Configurar canal ADC 0
    if (i2cWriteDevice(handle, &command, 1) < 0) {
        printf("Error al escribir comando I2C.\n");
        i2cClose(handle);
        gpioTerminate();
        return 1;
    }

    uint8_t value = i2cReadByte(handle); // Leer valor ADC
    printf("Valor leído del ADC: %d\n", value);

    command = 0x40; // Configurar DAC
    uint8_t dac_value = 128; // Salida DAC (50%)
    uint8_t data[2] = {command, dac_value};
    if (i2cWriteDevice(handle, data, 2) < 0) {
        printf("Error al escribir en DAC.\n");
    }

    i2cClose(handle);
    gpioTerminate();

    return 0;
}

