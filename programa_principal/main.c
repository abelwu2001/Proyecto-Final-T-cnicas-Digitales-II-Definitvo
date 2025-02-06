/* main.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ncurses.h>
#include <unistd.h>
#include <pigpio.h>
#include "secuencias.h"
#include "remoto.h"
#define PCF8591_I2C_ADDR 0x48
#define CLAVE_CORRECTA "12345"
#define MAX_INTENTOS 3

extern int comparar_contrasena(const char *entrada, const char *correcta);

int i2cHandle;
int modo_actual = 0;
int velocidad = 0;

void inicializar_adc();
int leer_adc(int canal);
int pedir_password();
void menu_principal();
int seleccionar_modo();
int definir_velocidad_inicial(int *velocidad);
void iniciar_secuencias(int velocidad);

void inicializar_adc() {
    gpioInitialise();
    i2cHandle = i2cOpen(1, PCF8591_I2C_ADDR, 0);
    if (i2cHandle < 0) {
        endwin();
        fprintf(stderr, "Error al inicializar el PCF8591\n");
        exit(1);
    }
}

int leer_adc(int canal) {
    i2cWriteByte(i2cHandle, 0x40 | (canal & 0x02));
    usleep(1000);
    return i2cReadByte(i2cHandle);
}

int pedir_password() {
    char password[6];
    int intentos = 0;

    initscr();
    noecho();
    raw();
    keypad(stdscr, TRUE);

    while (intentos < MAX_INTENTOS) {
        memset(password, 0, sizeof(password));
        mvprintw(0, 0, "Ingrese su password de 5 digitos: ");
        for (int i = 0; i < 5; i++) {
            char ch = getch();
            if (ch == 10)
                break;
            password[i] = ch;
            mvaddch(1, i, '*');
        }
        refresh();
        if (comparar_contrasena(password, CLAVE_CORRECTA) == 1) {
            mvprintw(2, 0, "Bienvenido al Sistema\n");
            refresh();
            usleep(1000000);
            endwin();
            return 1;
        } else {
            mvprintw(2, 0, "Password no valida\n");
            refresh();
            usleep(1000000);
            intentos++;
        }
    }

    endwin();
    return 0;
}

void menu_principal() {
    const char *opciones[] = {
        "1. Iniciar Secuencias",
        "2. Configurar Local/Remoto",
        "3. Configurar Velocidad Inicial",
        "4. Salir"
    };
    int seleccion = 0;
    int ch;

    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    curs_set(0);

    while (1) {
        clear();
        mvprintw(0, 0, "Menu Principal");
        for (int i = 0; i < 4; i++) {
            if (i == seleccion) {
                attron(A_REVERSE);
                mvprintw(i + 2, 0, "%s", opciones[i]);
                attroff(A_REVERSE);
            } else {
                mvprintw(i + 2, 0, "%s", opciones[i]);
            }
        }
        refresh();
        ch = getch();

        switch (ch) {
            case KEY_UP:
                seleccion = (seleccion > 0) ? seleccion - 1 : 3;
                break;
            case KEY_DOWN:
                seleccion = (seleccion < 3) ? seleccion + 1 : 0;
                break;
            case 10:
                if (seleccion == 3) {
                    endwin();
                    exit(0);
                }
                switch (seleccion) {
                    case 0:
                        iniciar_secuencias(velocidad);
                        break;
                    case 1:
                        modo_actual = seleccionar_modo();
                        break;
                    case 2:
                        definir_velocidad_inicial(&velocidad);
                        break;
                }
                break;
        }
    }
    endwin();
}

int main() {
    inicializar_adc();
    if (!pedir_password()) {
        printf("Contraseña incorrecta. Saliendo...\n");
        return 1;
    }
    printf("Bienvenido al sistema de control de secuencias.\n");
    menu_principal();
    printf("Saliendo del programa...\n");
    return 0;
}
