#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ncurses.h>
#include <unistd.h>
#include "secuencias.h"  // Incluir las funciones de secuencias

#define CLAVE_CORRECTA "12345"  // La contraseña correcta de 5 dígitos
#define MAX_INTENTOS 3  // Número máximo de intentos fallidos

// Función para pedir la contraseña y validarla
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

// Función para mostrar el menú de opciones
void mostrar_menu() {
    int opcion;
    int velocidad = 500000; // Velocidad para las secuencias de LEDs

    while (1) {
        clear();  // Limpiar pantalla
        mvprintw(0, 0, "Seleccione una opcion:\n");
        mvprintw(1, 0, "1. Secuencia 'Auto fantastico'\n");
        mvprintw(2, 0, "2. Secuencia 'Choque'\n");
        mvprintw(3, 0, "3. Secuencia 'Apilada'\n");
        mvprintw(4, 0, "4. Secuencia 'Carrera'\n");
        mvprintw(5, 0, "Presione F2 para salir.\n");
        refresh();

        opcion = getch();  // Esperar entrada de usuario

        switch(opcion) {
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
            case KEY_F(2):  // F2 para salir
                return;
            default:
                mvprintw(6, 0, "Opción inválida.\n");
                refresh();
                usleep(1000000);  // Pausar por 1 segundo
                break;
        }
    }
}

int main() {
    if (pedir_password()) {  // Si la contraseña es correcta
        mostrar_menu();  // Mostrar el menú de opciones
    }
    return 0;
}

