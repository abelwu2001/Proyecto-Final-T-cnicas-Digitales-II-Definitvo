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

// Variable global para el modo
int modo_actual = 0; // 0 para local, 1 para remoto
int velocidad = 0; // Velocidad inicial

// Funciones auxiliares para el programa
void inicializar_adc();
int leer_adc(int canal);
int pedir_password();
void menu_principal();
int seleccionar_modo();
int definir_velocidad_inicial(int *velocidad);
void iniciar_secuencias(int velocidad);

// Funciones

void inicializar_adc() {
    gpioInitialise();  // Inicializar pigpio
    i2cHandle = i2cOpen(1, PCF8591_I2C_ADDR, 0);  // Abrir comunicación I2C
    if (i2cHandle < 0) {
        endwin();
        fprintf(stderr, "Error al inicializar el PCF8591\n");
        exit(1);
    }
}

int leer_adc(int canal) {
    i2cWriteByte(i2cHandle, 0x40 | (canal & 0x03));  // Seleccionar canal
    usleep(1000);  // Esperar un poco para la conversión
    return i2cReadByte(i2cHandle);  // Leer el valor ADC
}

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
    start_color();
    init_pair(1, COLOR_WHITE, COLOR_BLUE); // Texto blanco sobre fondo azul

    // Calcular dimensiones de la ventana
    int rows, cols;
    getmaxyx(stdscr, rows, cols);
    int ventana_height = 10;
    int ventana_width = 40;
    int starty = (rows - ventana_height) / 2;
    int startx = (cols - ventana_width) / 2;

    // Crear ventana centrada
    WINDOW *menu_win = newwin(ventana_height, ventana_width, starty, startx);
    wbkgd(menu_win, COLOR_PAIR(1)); // Fondo azul
    box(menu_win, 0, 0); // Borde de la ventana
    mvwprintw(menu_win, 0, (ventana_width - 14) / 2, "Menu Principal");

    while (1) {
        werase(menu_win);  // Borrar solo el contenido interno de la ventana
        box(menu_win, 0, 0); // Redibujar el borde
        mvwprintw(menu_win, 0, (ventana_width - 14) / 2, "Menu Principal");

        for (int i = 0; i < 4; i++) {
            if (i == seleccion) {
                wattron(menu_win, A_REVERSE);
                mvwprintw(menu_win, i + 2, 2, "%s", opciones[i]);
                wattroff(menu_win, A_REVERSE);
            } else {
                mvwprintw(menu_win, i + 2, 2, "%s", opciones[i]);
            }
        }

        wrefresh(menu_win);
        ch = getch();

        switch (ch) {
            case KEY_UP:
                seleccion = (seleccion > 0) ? seleccion - 1 : 3;
                break;
            case KEY_DOWN:
                seleccion = (seleccion < 3) ? seleccion + 1 : 0;
                break;
            case 10: // Enter
                if (seleccion == 3) {
                    delwin(menu_win);
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

    delwin(menu_win);
    endwin();
}


int seleccionar_modo() {
    int seleccion = 0;
    int ch;

    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    curs_set(0);

    while (1) {
        clear();
        mvprintw(0, 0, "Seleccione el modo de trabajo:");
        mvprintw(1, 0, "1. Modo Local (Predeterminado)");
        mvprintw(2, 0, "2. Modo Remoto");
        refresh();

        ch = getch();
        if (ch == '1') {
            return 0; // Modo local
        } else if (ch == '2') {
            return 1; // Modo remoto
        }
    }

    endwin();
}

int definir_velocidad_inicial(int *velocidad) {
    int ch;
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    curs_set(0);

    while (1) {
        clear();
        mvprintw(0, 0, "Definir velocidad inicial:");
        mvprintw(1, 0, "1. Usar datos del ADC");
        mvprintw(2, 0, "2. Introducir valor manualmente");
        refresh();

        ch = getch();
        if (ch == '1') {
            *velocidad = leer_adc(0) * 1000; // Velocidad basada en ADC
            return 1;
        } else if (ch == '2') {
            echo();
            mvprintw(3, 0, "Ingrese la velocidad inicial (us): ");
            scanw("%d", velocidad);
            noecho();
            return 2;
        }
    }

    endwin();
}

void iniciar_secuencias(int velocidad) {
    const char *opciones[] = {
        "1. Secuencia 'Auto Fantastico'",
        "2. Secuencia 'Choque'",
        "3. Secuencia 'Apilada'",
        "4. Secuencia 'Carrera'",
        "5. Secuencia 'Escalera'",
        "6. Secuencia 'Chispas'",
        "7. Secuencia 'Sirena'",
        "8. Secuencia 'Matrix'",
        "9. Salir"
    };
    int seleccion = 0;
    int ch;

    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    curs_set(0);
    start_color();
    init_pair(1, COLOR_WHITE, COLOR_BLUE); // Texto blanco sobre fondo azul

    // Calcular dimensiones de la ventana
    int rows, cols;
    getmaxyx(stdscr, rows, cols);
    int ventana_height = 12;
    int ventana_width = 40;
    int starty = (rows - ventana_height) / 2;
    int startx = (cols - ventana_width) / 2;

    // Crear ventana centrada
    WINDOW *menu_win = newwin(ventana_height, ventana_width, starty, startx);
    wbkgd(menu_win, COLOR_PAIR(1)); // Fondo azul
    box(menu_win, 0, 0); // Borde de la ventana

    while (1) {
        werase(menu_win); // Borrar el contenido interno de la ventana
        box(menu_win, 0, 0); // Redibujar el borde
        mvwprintw(menu_win, 0, (ventana_width - 20) / 2, "Menu de Secuencias");

        // Mostrar las opciones del menú
        for (int i = 0; i < 9; i++) {
            if (i == seleccion) {
                wattron(menu_win, A_REVERSE); // Resaltar la opción seleccionada
                mvwprintw(menu_win, i + 2, 2, "%s", opciones[i]);
                wattroff(menu_win, A_REVERSE);
            } else {
                mvwprintw(menu_win, i + 2, 2, "%s", opciones[i]);
            }
        }

        wrefresh(menu_win); // Actualizar la ventana
        ch = getch(); // Leer entrada del usuario

        switch (ch) {
            case KEY_UP:
                seleccion = (seleccion > 0) ? seleccion - 1 : 8; // Ciclar hacia arriba
                break;
            case KEY_DOWN:
                seleccion = (seleccion < 8) ? seleccion + 1 : 0; // Ciclar hacia abajo
                break;
            case 10: // Enter
                if (seleccion == 8) { // Opción para salir
                    delwin(menu_win); // Destruir ventana
                    endwin();
                    return;
                }

                delwin(menu_win); // Destruir ventana antes de ejecutar la secuencia

                // Ejecutar la secuencia seleccionada
                switch (seleccion) {
                    case 0:
                        secuencia_auto_fantastico(&velocidad);
                        break;
                    case 1:
                        secuencia_choque(&velocidad);
                        break;
                    case 2:
                        secuencia_apilada(&velocidad);
                        break;
                    case 3:
                        secuencia_carrera(&velocidad);
                        break;
                    case 4:
                        secuencia_escalera(&velocidad);
                        break;
                    case 5:
                        secuencia_chispas(&velocidad);
                        break;
                    case 6:
                        secuencia_sirena(&velocidad);
                        break;
                    case 7:
                        secuencia_matrix(&velocidad);
                        break;
                }

                // Restaurar la ventana después de ejecutar una secuencia
                menu_win = newwin(ventana_height, ventana_width, starty, startx);
                wbkgd(menu_win, COLOR_PAIR(1)); // Fondo azul
                box(menu_win, 0, 0); // Borde de la ventana
                break;
        }
    }

    delwin(menu_win);
    endwin();
}



int main() {
    inicializar_adc();

    if (!pedir_password()) {
        printf("Contrase\u00f1a incorrecta. Saliendo...\n");
        return 1;
    }

    menu_principal();

    return 0;
}

