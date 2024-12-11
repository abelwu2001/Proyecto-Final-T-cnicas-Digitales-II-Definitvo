#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ncurses.h>
#include <unistd.h>
#include <pigpio.h>
#include "secuencias.h"  // Incluir las funciones de secuencias

#define PCF8591_I2C_ADDR 0x48  // Dirección I2C del PCF8591
#define CLAVE_CORRECTA "12345" // Contraseña correcta
#define MAX_INTENTOS 3       // Número máximo de intentos

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

    // Inicializar colores
    if (!has_colors()) {
        endwin();
        fprintf(stderr, "Este terminal no soporta colores\n");
        exit(1);
    }
    start_color();  // Inicializa el sistema de colores
    init_pair(1, COLOR_WHITE, COLOR_BLUE);  // Definir color blanco sobre fondo azul
    init_pair(2, COLOR_BLACK, COLOR_WHITE); // Definir color negro sobre fondo blanco para resaltado

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
    endwin();  // Termina ncurses
    return 0;  // Contraseña incorrecta después de varios intentos
}

// Función para mostrar el menú con contorno visible en la opción seleccionada
void mostrar_menu(int *velocidad, int adc_value) {
    int seleccion = 0;
    int num_opciones = 9;  // Hay 9 opciones en el menú
    int ch;

    // Inicializar ncurses
    initscr();          // Inicializa ncurses
    noecho();           // No mostrar lo que se escribe
    raw();              // Para que las teclas se ingresen sin esperar por ENTER
    keypad(stdscr, TRUE);  // Para permitir las teclas especiales

    // Inicializar colores
    if (!has_colors()) {
        endwin();
        fprintf(stderr, "Este terminal no soporta colores\n");
        exit(1);
    }
    start_color();  // Inicializa el sistema de colores
    init_pair(1, COLOR_WHITE, COLOR_BLUE);  // Definir color blanco sobre fondo azul
    init_pair(2, COLOR_BLACK, COLOR_WHITE); // Definir color negro sobre fondo blanco para resaltado

    // Crear una ventana de fondo azul para el menú
    int startx = 10, starty = 5, width = 45, height = 15;
    WINDOW *menu_win = newwin(height, width, starty, startx);
    wbkgd(menu_win, COLOR_PAIR(1));  // Fondo azul
    box(menu_win, 0, 0);  // Borde alrededor de la ventana
    refresh();
    wrefresh(menu_win);

    // Opciones del menú
    char *opciones[] = {
        "1. Secuencia Auto Fantástico",
        "2. Secuencia Choque",
        "3. Secuencia Apilada",
        "4. Secuencia Carrera",
        "5. Secuencia Escalera",
        "6. Secuencia Chispas",
        "7. Secuencia Sirena",
        "8. Secuencia Matrix",
        "9. Salir"
    };

    // Mostrar las opciones en el menú
    for (int i = 0; i < num_opciones; i++) {
        if (i == seleccion) {
            // Resaltar la opción seleccionada con un borde y texto invertido
            wattron(menu_win, A_REVERSE);  // Activar el atributo de texto invertido
            mvwprintw(menu_win, i + 1, 2, opciones[i]);
            wattroff(menu_win, A_REVERSE);  // Desactivar el atributo de texto invertido
        } else {
            mvwprintw(menu_win, i + 1, 2, opciones[i]);
        }
    }

    // Mostrar lectura del ADC y la velocidad ajustada
    mvwprintw(menu_win, height - 3, 2, "Lectura ADC: %d", adc_value);
    mvwprintw(menu_win, height - 2, 2, "Velocidad ajustada: %dus", *velocidad);

    // Actualizar la pantalla
    wrefresh(menu_win);

    while (1) {
        ch = getch();
        switch (ch) {
            case KEY_UP:  // Mover hacia arriba
                seleccion = (seleccion > 0) ? seleccion - 1 : num_opciones - 1;
                break;
            case KEY_DOWN:  // Mover hacia abajo
                seleccion = (seleccion < num_opciones - 1) ? seleccion + 1 : 0;
                break;
            case 10:  // Enter (seleccionar opción)
                if (seleccion == 8) {
                    endwin();  // Salir del menú
                    return;
                }
                // Aquí puedes ejecutar la secuencia correspondiente según la opción seleccionada
                switch (seleccion) {
                    case 0:
                        secuencia_auto_fantastico(velocidad);  // Ejecutar secuencia Auto Fantástico
                        break;
                    case 1:
                        secuencia_choque(velocidad);  // Ejecutar secuencia Choque
                        break;
                    case 2:
                        secuencia_apilada(velocidad);  // Ejecutar secuencia Apilada
                        break;
                    case 3:
                        secuencia_carrera(velocidad);  // Ejecutar secuencia Carrera
                        break;
                    case 4:
                        secuencia_escalera(velocidad);  // Ejecutar secuencia Escalera
                        break;
                    case 5:
                        secuencia_chispas(velocidad);
                    case 6:
                        secuencia_sirena(velocidad);
                    case 7:
                        secuencia_matrix(velocidad);
                }
                break;
        }

        // Volver a mostrar el menú después de seleccionar
        werase(menu_win);  // Limpiar la ventana
        box(menu_win, 0, 0);  // Borde alrededor de la ventana
        for (int i = 0; i < num_opciones; i++) {
            if (i == seleccion) {
                wattron(menu_win, A_REVERSE);
                mvwprintw(menu_win, i + 1, 2, opciones[i]);
                wattroff(menu_win, A_REVERSE);
            } else {
                mvwprintw(menu_win, i + 1, 2, opciones[i]);
            }
        }
        mvwprintw(menu_win, height - 3, 2, "Lectura ADC: %d", adc_value);
        mvwprintw(menu_win, height - 2, 2, "Velocidad ajustada: %dus", *velocidad);
        wrefresh(menu_win);  // Actualizar la pantalla
    }

    endwin();  // Finalizar ncurses
}

int main() {
    int velocidad = 0;
    int adc_value = 0;

    // Inicializar el ADC
    inicializar_adc();

    // Pedir la contraseña
    if (pedir_password() == 0) {
        printf("Contraseña incorrecta\n");
        return 0;
    }

    // Leer valor del ADC
    adc_value = leer_adc(0);  // Leer canal 0 del ADC

    // Mostrar el menú
    mostrar_menu(&velocidad, adc_value);

    return 0;
}

