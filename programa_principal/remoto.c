#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <ncurses.h>
#include "remoto.h"
#include "secuencias.h"

extern int velocidad; // Velocidad inicial, compartida con el programa principal

// Función para configurar el UART
int configurar_uart() {
    int fd = open("/dev/serial0", O_RDWR | O_NOCTTY);
    if (fd == -1) {
        perror("Error al abrir UART");
        return -1;
    }

    struct termios options;
    tcgetattr(fd, &options);
    cfsetispeed(&options, B9600);
    cfsetospeed(&options, B9600);
    options.c_cflag |= (CLOCAL | CREAD);
    tcsetattr(fd, TCSANOW, &options);

    return fd;
}

// Función para modo esclavo
void modo_esclavo() {
    int fd = configurar_uart();
    if (fd == -1) return;

    char comando[16];
    printf("Modo esclavo: esperando comandos...\n");

    while (1) {
        memset(comando, 0, sizeof(comando));
        int bytes_leidos = read(fd, comando, sizeof(comando) - 1);
        if (bytes_leidos > 0) {
            comando[bytes_leidos] = '\0'; // Asegurarse de que termine en '\0'
            printf("Comando recibido: %s\n", comando);

            // Ejecutar secuencia según el comando recibido
            if (strcmp(comando, "AUTO") == 0) secuencia_auto_fantastico(&velocidad);
            else if (strcmp(comando, "CHOQUE") == 0) secuencia_choque(&velocidad);
            else if (strcmp(comando, "APILADA") == 0) secuencia_apilada(&velocidad);
            else if (strcmp(comando, "CARRERA") == 0) secuencia_carrera(&velocidad);
            else if (strcmp(comando, "ESCALERA") == 0) secuencia_escalera(&velocidad);
            else if (strcmp(comando, "CHISPAS") == 0) secuencia_chispas(&velocidad);
            else if (strcmp(comando, "SIRENA") == 0) secuencia_sirena(&velocidad);
            else if (strcmp(comando, "MATRIX") == 0) secuencia_matrix(&velocidad);
            else printf("Comando no reconocido: %s\n", comando); // Manejo de error
        }
    }

    close(fd);
}

// Función para modo maestro
void modo_maestro() {
    int fd = configurar_uart();
    if (fd == -1) return;

    const char *secuencias[] = {"AUTO", "CHOQUE", "APILADA", "CARRERA", "ESCALERA", "CHISPAS", "SIRENA", "MATRIX"};
    int seleccion = 0;
    int ch;

    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    curs_set(0);

    while (1) {
        clear();
        mvprintw(0, 0, "Modo Maestro: Control de Secuencias");
        for (int i = 0; i < 8; i++) {
            if (i == seleccion) {
                attron(A_REVERSE);
                mvprintw(i + 1, 0, "Secuencia %d: %s", i + 1, secuencias[i]);
                attroff(A_REVERSE);
            } else {
                mvprintw(i + 1, 0, "Secuencia %d: %s", i + 1, secuencias[i]);
            }
        }
        refresh();

        ch = getch();
        switch (ch) {
            case KEY_UP:
                seleccion = (seleccion > 0) ? seleccion - 1 : 7;
                break;
            case KEY_DOWN:
                seleccion = (seleccion < 7) ? seleccion + 1 : 0;
                break;
            case 10: // Enter
                write(fd, secuencias[seleccion], strlen(secuencias[seleccion])); // Enviar comando
                write(fd, "\n", 1); // Línea nueva para finalizar
                mvprintw(10, 0, "Comando enviado: %s", secuencias[seleccion]); // Mostrar mensaje
                refresh();
                usleep(1500000); // Pausar 1.5 segundos
                break;
            case 'q': // Salir
                close(fd);
                endwin();
                return;
        }
    }

    close(fd);
    endwin();
}

