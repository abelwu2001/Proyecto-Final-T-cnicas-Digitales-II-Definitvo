#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ncurses.h>
#include <unistd.h>
#include <pigpio.h>
#include "secuencias.h"  
#include "remoto.h"  

#define PCF8591_I2C_ADDR 0x48  // Dirección I2C del PCF8591
#define CLAVE_CORRECTA "12345" // Contraseña correcta
#define MAX_INTENTOS 3         // Máximo de intentos

// Prototipos de funciones
void inicializar_adc();
int leer_adc(int canal);
int pedir_password();
void menu_principal();
int seleccionar_modo();
int definir_velocidad_inicial(int *velocidad);
void iniciar_secuencias(int velocidad);

// Variables globales
int i2cHandle;
int modo_actual = 0; // 0 = Local, 1 = Remoto
int velocidad = 0;   // Velocidad inicial

// Inicializa el ADC en la Raspberry Pi
void inicializar_adc() {
    gpioInitialise();  
    i2cHandle = i2cOpen(1, PCF8591_I2C_ADDR, 0);  

    if (i2cHandle < 0) {
        endwin();
        fprintf(stderr, "Error al inicializar el PCF8591\n");
        exit(1);
    }
}

// Lee un canal del ADC
int leer_adc(int canal) {
    i2cWriteByte(i2cHandle, 0x40 | (canal & 0x02));  
    usleep(1000);  
    return i2cReadByte(i2cHandle);
}

// Solicita la contraseña de acceso al sistema
int pedir_password() {
    char password[6];
    int intentos = 0;

    initscr();
    noecho();
    raw();
    keypad(stdscr, TRUE);

    while (intentos < MAX_INTENTOS) {
        memset(password, 0, sizeof(password));
        mvprintw(0, 0, "Ingrese su password de 5 dígitos: ");
        
        for (int i = 0; i < 5; i++) {
            char ch = getch();
            if (ch == 10) break;
            password[i] = ch;
            mvaddch(1, i, '*');
        }

        refresh();

        if (strcmp(password, CLAVE_CORRECTA) == 0) {
            mvprintw(2, 0, "Bienvenido al Sistema\n");
            refresh();
            usleep(1000000);
            endwin();
            return 1;
        } else {
            mvprintw(2, 0, "Password incorrecta\n");
            refresh();
            usleep(1000000);
            intentos++;
        }
    }

    endwin();
    return 0;
}

// 📌 **Menú principal**
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
        mvprintw(0, 0, "Menú Principal");
        for (int i = 0; i < 4; i++) {
            if (i == seleccion) attron(A_REVERSE);
            mvprintw(i + 2, 2, "%s", opciones[i]);
            if (i == seleccion) attroff(A_REVERSE);
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
            case 10: // Enter
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
                    case 3:
                        endwin();
                        exit(0);
                }
                break;
        }
    }
}

// 📌 **Seleccionar Modo (Local/Remoto)**
int seleccionar_modo() {
    int opcion = 0;
    int ch;

    while (1) {
        clear();
        mvprintw(0, 0, "Seleccione el modo de trabajo:");
        mvprintw(1, 0, "1. Modo Local");
        mvprintw(2, 0, "2. Modo Remoto");
        mvprintw(3, 0, "Presione 'q' para regresar.");
        refresh();

        ch = getch();
        if (ch == '1') return 0; // Modo Local
        if (ch == '2') return 1; // Modo Remoto
        if (ch == 'q') return -1; 
    }
}

// 📌 **Configurar velocidad inicial**
int definir_velocidad_inicial(int *velocidad_inicial) {
    int opcion;
    char entrada[16]; 
    int nueva_velocidad;

    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    curs_set(0);

    while (1) {
        clear();
        mvprintw(0, 0, "Configuración de Velocidad Inicial:");
        mvprintw(1, 0, "1. Usar datos del ADC");
        mvprintw(2, 0, "2. Introducir valor manualmente");
        mvprintw(3, 0, "Presione 'q' para regresar.");
        refresh();

        opcion = getch();
        if (opcion == '1') {  
            int valor_adc = leer_adc(0);
            *velocidad_inicial = valor_adc * 1000;

            clear();
            mvprintw(0, 0, "Velocidad configurada desde ADC: %d us", *velocidad_inicial);
            refresh();
            usleep(1500000);
            break;
        } else if (opcion == '2') {  
            echo();
            clear();
            mvprintw(0, 0, "Ingrese la velocidad en microsegundos: ");
            refresh();
            getstr(entrada);  
            nueva_velocidad = atoi(entrada);

            if (nueva_velocidad > 0) {
                *velocidad_inicial = nueva_velocidad;
                mvprintw(2, 0, "Velocidad configurada manualmente: %d us", *velocidad_inicial);
                refresh();
                usleep(1500000);
                break;
            }
            noecho();
        } else if (opcion == 'q') break;
    }

    endwin();
    return 0;
}

// 📌 **Iniciar Secuencias**
void iniciar_secuencias(int velocidad) {
    clear();
    mvprintw(0, 0, "Iniciando secuencias...");
    refresh();
    usleep(1000000);

    // Aquí puedes agregar lógica para ejecutar una secuencia
    secuencia_auto_fantastico(&velocidad);
}

// 📌 **Main**
int main() {
    inicializar_adc();
    if (!pedir_password()) {
        printf("Contraseña incorrecta. Saliendo...\n");
        return 1;
    }

    menu_principal();
    return 0;
}
