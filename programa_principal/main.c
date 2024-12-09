#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ncurses.h>
#include <unistd.h>
#include <pigpio.h>
#include "secuencias.h"  // Incluir las funciones de secuencias

#define PCF8591_I2C_ADDR 0x48  // Dirección I2C del PCF8591

// Variable global para manejar el dispositivo I2C
int i2cHandle;

// Función para inicializar el PCF8591
void inicializar_adc() {
    gpioInitialise();  // Inicializar pigpio
    i2cHandle = i2cOpen(1, PCF8591_I2C_ADDR, 0);  // Abrir comunicación I2C
    if (i2cHandle < 0) {
        endwin();
        fprintf(stderr, "Error al inicializar el PCF8591\n");
        exit(1);
    }
}

// Función para leer un canal específico del PCF8591
int leer_adc(int canal) {
    i2cWriteByte(i2cHandle, 0x40 | (canal & 0x03));  // Seleccionar canal
    usleep(1000);  // Esperar un poco para la conversión
    return i2cReadByte(i2cHandle);  // Leer el valor ADC
}

// Función para mostrar el menú principal
int mostrar_menu(int *velocidad, int adc_value) {
    const char *opciones[] = {
        "1. Secuencia 'Auto Fantastico'",
        "2. Secuencia 'Choque'",
        "3. Secuencia 'Apilada'",
        "4. Secuencia 'Carrera'",
        "5. Secuencia 'Escalera'",
        "6. Salir"
    };
    int seleccion = 0;  // Índice de la opción seleccionada
    int num_opciones = sizeof(opciones) / sizeof(opciones[0]);
    int ch;

    initscr();             // Inicializar ncurses
    cbreak();              // Modo de entrada crudo
    noecho();              // No mostrar la entrada del usuario
    keypad(stdscr, TRUE);  // Habilitar teclas especiales
    curs_set(0);           // Ocultar el cursor

    while (1) {
        clear();  // Limpiar pantalla
        mvprintw(0, 0, "Seleccione una opcion:\n");
        for (int i = 0; i < num_opciones; i++) {
            if (i == seleccion) {
                attron(A_REVERSE);  // Resaltar opción seleccionada
                mvprintw(i + 1, 0, "%s", opciones[i]);
                attroff(A_REVERSE);
            } else {
                mvprintw(i + 1, 0, "%s", opciones[i]);
            }
        }

        // Mostrar lectura del ADC y la velocidad ajustada
        mvprintw(8, 0, "Lectura ADC: %d\n", adc_value);
        mvprintw(9, 0, "Velocidad ajustada: %dus\n", *velocidad);

        ch = getch();
        switch (ch) {
            case KEY_UP:  // Mover hacia arriba
                seleccion = (seleccion > 0) ? seleccion - 1 : num_opciones - 1;
                break;
            case KEY_DOWN:  // Mover hacia abajo
                seleccion = (seleccion < num_opciones - 1) ? seleccion + 1 : 0;
                break;
            case 10:  // Enter
                endwin();
                return seleccion + 1;  // Devolver opción seleccionada
        }
    }
}

int main() {
    int velocidad = 500000;  // Velocidad inicial en microsegundos
    int opcion, adc_value;

    inicializar_adc();  // Inicializar el ADC

    while (1) {
        adc_value = leer_adc(0);  // Leer el canal 0 del ADC
        velocidad = 100000 + (adc_value * 500);  // Ajustar velocidad según el ADC

        opcion = mostrar_menu(&velocidad, adc_value);
        switch (opcion) {
            case 1:
                secuencia_auto_fantastico(&velocidad);
                break;
            case 2:
                secuencia_choque(&velocidad);
                break;
            case 3:
                secuencia_apilada(&velocidad);
                break;
            case 4:
                secuencia_carrera(&velocidad);
                break;
            case 5:
                secuencia_escalera(&velocidad);
                break;
            case 6:
                endwin();
                gpioTerminate();
                i2cClose(i2cHandle);
                return 0;
            default:
                break;
        }
    }
}

