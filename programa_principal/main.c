#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ncurses.h>
#include <unistd.h>
#include <pigpio.h>
#include "secuencias.h"  // Incluir las funciones de secuencias

#define PCF8591_I2C_ADDR 0x48  // Dirección I2C del PCF8591
#define CLAVE_CORRECTA "12345" // Contraseña correcta
#define MAX_INTENTOS 3        // Número máximo de intentos

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

// Función para pedir la contraseña
int pedir_password() {
    char password[6];  // La contraseña tiene 5 dígitos más el terminador
    int intentos = 0;

    // Inicializar ncurses para la entrada de la contraseña
    initscr();          // Inicializa ncurses
    noecho();           // No mostrar lo que se escribe
    raw();              // Para que las teclas se ingresen sin esperar por ENTER
    keypad(stdscr, TRUE);  // Para permitir las teclas especiales

    while (intentos < MAX_INTENTOS) {
        memset(password, 0, sizeof(password));  // Limpiar la variable password
        mvprintw(0, 0, "Ingrese su password de 5 digitos: ");
        for (int i = 0; i < 5; i++) {
            char ch = getch();  // Leer cada carácter
            if (ch == 10)  // Salir si se presiona ENTER
                break;
            password[i] = ch;
            mvaddch(1, i, '*');  // Mostrar '*' en lugar de cada dígito
        }
        refresh();  // Actualizar la pantalla

        // Compara la contraseña ingresada con la correcta
        if (strcmp(password, CLAVE_CORRECTA) == 0) {
            mvprintw(2, 0, "Bienvenido al Sistema\n");
            refresh();
            usleep(1000000);  // Pausar por 1 segundo
            endwin();  // Termina ncurses
            return 1;  // Contraseña correcta
        } else {
            mvprintw(2, 0, "Password no valida\n");
            refresh();
            usleep(1000000);  // Pausar por 1 segundo
            intentos++;
        }
    }

    mvprintw(3, 0, "Demasiados intentos fallidos. Aborting.\n");
    refresh();
    usleep(1000000);  // Pausar por 1 segundo
    endwin();  // Termina ncurses
    return 0;  // Intentos fallidos
}

// Función para mostrar el menú de opciones con fondo azul
void mostrar_menu() {
    int opcion;
    int velocidad = 500000; // Velocidad para las secuencias de LEDs
    int adc_value;

    initscr();             // Inicializar ncurses
    start_color();         // Habilitar colores
    init_pair(1, COLOR_WHITE, COLOR_BLUE);  // Fondo azul y texto blanco

    // Establecer el color de fondo azul
    bkgd(COLOR_PAIR(1));   // Cambiar fondo a azul con texto blanco
    cbreak();              // Modo de entrada crudo
    noecho();              // No mostrar la entrada del usuario
    keypad(stdscr, TRUE);  // Habilitar teclas especiales
    curs_set(0);           // Ocultar el cursor

    while (1) {
        clear();  // Limpiar pantalla
        mvprintw(0, 0, "Seleccione una opcion:\n");
        mvprintw(1, 0, "1. Secuencia 'Auto fantastico'\n");
        mvprintw(2, 0, "2. Secuencia 'Choque'\n");
        mvprintw(3, 0, "3. Secuencia 'Apilada'\n");
        mvprintw(4, 0, "4. Secuencia 'Carrera'\n");
        mvprintw(5, 0, "5. Secuencia 'Escalera'\n");
        mvprintw(6, 0, "Presione F2 para salir\n");

        // Leer el valor ADC
        adc_value = leer_adc(0);  // Suponiendo que se lee del canal 0

        // Mostrar lectura del ADC y la velocidad ajustada
        mvprintw(7, 0, "Lectura ADC: %d\n", adc_value);
        mvprintw(8, 0, "Velocidad ajustada: %dus\n", velocidad);

        opcion = getch();  // Leer la opción seleccionada

        switch (opcion) {
            case '1':
                secuencia_auto_fantastico(&velocidad);  // Llamar la secuencia auto fantástico
                break;
            case '2':
                secuencia_choque(&velocidad);  // Llamar la secuencia choque
                break;
            case '3':
                secuencia_apilada(&velocidad);  // Llamar la secuencia apilada
                break;
            case '4':
                secuencia_carrera(&velocidad);  // Llamar la secuencia carrera
                break;
            case '5':
                secuencia_escalera(&velocidad);  // Llamar la secuencia escalera
                break;
            case KEY_F(2):  // F2 para salir
                endwin();  // Terminar ncurses
                return;
            default:
                mvprintw(9, 0, "Opción inválida.\n");
                refresh();
                usleep(1000000);  // Pausar por 1 segundo
                break;
        }
        refresh();  // Actualizar pantalla
    }
}

int main() {
    // Inicializar la comunicación I2C y el sistema pigpio
    inicializar_adc();

    // Pedir la contraseña
    if (!pedir_password()) {
        return 0;  // Si la contraseña es incorrecta, terminar
    }

    // Mostrar el menú
    mostrar_menu();

    return 0;
}

