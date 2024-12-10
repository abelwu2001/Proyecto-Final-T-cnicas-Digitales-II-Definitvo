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
    return 0;  // Contraseña incorrecta
}

// Función para crear una ventana centrada con bordes
void crear_ventana(int startx, int starty, int width, int height) {
    WINDOW *win = newwin(height, width, starty, startx);  // Crear ventana
    box(win, 0, 0);  // Dibujar los bordes
    wrefresh(win);  // Actualizar la ventana
}

// Función para mostrar el menú principal en una ventana centrada
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

    // Inicializar ncurses
    initscr();             
    cbreak();              
    noecho();              
    keypad(stdscr, TRUE);  
    curs_set(0);           // Ocultar el cursor

    // Obtener dimensiones de la pantalla
    int altura, ancho;
    getmaxyx(stdscr, altura, ancho);

    // Definir dimensiones de la ventana centrada
    int ventana_height = 10;
    int ventana_width = 40;
    int startx = (ancho - ventana_width) / 2;
    int starty = (altura - ventana_height) / 2;

    // Crear la ventana centrada con bordes
    crear_ventana(startx, starty, ventana_width, ventana_height);

    while (1) {
        clear();  // Limpiar pantalla
        mvprintw(0, 0, "Seleccione una opcion:\n");
        
        // Dibujar las opciones centradas
        for (int i = 0; i < num_opciones; i++) {
            int x = startx + (ventana_width - strlen(opciones[i])) / 2;  // Centrar texto horizontalmente
            int y = starty + 1 + i;  // Ajustar la posición vertical
            if (i == seleccion) {
                attron(A_REVERSE);  // Resaltar opción seleccionada
                mvprintw(y, x, "%s", opciones[i]);
                attroff(A_REVERSE);
            } else {
                mvprintw(y, x, "%s", opciones[i]);
            }
        }

        // Mostrar lectura del ADC y la velocidad ajustada
        mvprintw(starty + 8, startx, "Lectura ADC: %d", adc_value);
        mvprintw(starty + 9, startx, "Velocidad ajustada: %dus", *velocidad);

        ch = getch();
        switch (ch) {
            case KEY_UP:  // Mover hacia arriba
                seleccion = (seleccion > 0) ? seleccion - 1 : num_opciones - 1;
                break;
            case KEY_DOWN:  // Mover hacia abajo
                seleccion = (seleccion < num_opciones - 1) ? seleccion + 1 : 0;
                break;
            case 10:  // Enter (seleccionar opción)
                if (seleccion == 5) {
                    endwin();  // Salir del menú
                    return 0;
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
                }
                break;
        }
    }

    endwin();  // Finalizar ncurses
    return 0;
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

