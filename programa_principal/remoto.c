#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <ncurses.h>
#include "remoto.h"
#include "secuencias.h"
#include <stdlib.h>

extern int velocidad; // Velocidad inicial compartida con el programa principal

// 📌 Configura el UART para la comunicación serie
int configurar_uart(const char *dispositivo) {
    int fd = open(dispositivo, O_RDWR | O_NOCTTY);
    if (fd == -1) {
        perror("Error al abrir UART");
        return -1;
    }

    struct termios options;
    tcgetattr(fd, &options);
    cfsetispeed(&options, B115200);
    cfsetospeed(&options, B115200);
    options.c_cflag |= (CLOCAL | CREAD);
    options.c_cflag &= ~CSIZE;
    options.c_cflag |= CS8;
    options.c_cflag &= ~PARENB;
    options.c_cflag &= ~CSTOPB;
    options.c_cflag &= ~CRTSCTS;
    options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
    options.c_iflag &= ~(IXON | IXOFF | IXANY);
    options.c_oflag &= ~OPOST;
    tcsetattr(fd, TCSANOW, &options);

    printf("UART configurado en: %s\n", dispositivo);
    return fd;
}

// 📌 **Modo Esclavo**
void modo_esclavo() {
    const char *dispositivo = "/dev/ttyUSB0";
    int fd = configurar_uart(dispositivo);
    if (fd == -1) return;

    char comando[32];
    int velocidad_actual;
    char secuencia_actual[16];

    printf("Modo esclavo activo en %s. Esperando comandos del Maestro...\n", dispositivo);
    printf("Escriba 'SALIR' en el Maestro para salir del modo esclavo.\n");

    while (1) {
        memset(comando, 0, sizeof(comando));
        int bytes_leidos = read(fd, comando, sizeof(comando) - 1);

        if (bytes_leidos > 0) {
            comando[bytes_leidos] = '\0';
            printf("Comando recibido: %s\n", comando);

            // Si el maestro envía "SALIR", finaliza el modo esclavo
            if (strcmp(comando, "SALIR") == 0) {
                printf("Cerrando modo esclavo...\n");
                break;
            }

            // Procesar el comando en formato "SECUENCIA:VELOCIDAD"
            char *token = strtok(comando, ":");
            if (token != NULL) {
                strcpy(secuencia_actual, token);
                token = strtok(NULL, ":");
                if (token != NULL) {
                    velocidad_actual = atoi(token);
                } else {
                    velocidad_actual = leer_adc(0) * 1000; // Usar ADC si no hay velocidad en el comando
                }

                // Ejecutar la secuencia recibida
                if (strcmp(secuencia_actual, "AUTO") == 0) {
                    secuencia_auto_fantastico(&velocidad_actual);
                } else if (strcmp(secuencia_actual, "CHOQUE") == 0) {
                    secuencia_choque(&velocidad_actual);
                } else if (strcmp(secuencia_actual, "APILADA") == 0) {
                    secuencia_apilada(&velocidad_actual);
                } else if (strcmp(secuencia_actual, "CARRERA") == 0) {
                    secuencia_carrera(&velocidad_actual);
                } else if (strcmp(secuencia_actual, "ESCALERA") == 0) {
                    secuencia_escalera(&velocidad_actual);
                } else if (strcmp(secuencia_actual, "CHISPAS") == 0) {
                    secuencia_chispas(&velocidad_actual);
                } else if (strcmp(secuencia_actual, "SIRENA") == 0) {
                    secuencia_sirena(&velocidad_actual);
                } else if (strcmp(secuencia_actual, "MATRIX") == 0) {
                    secuencia_matrix(&velocidad_actual);
                } else {
                    printf("Comando no reconocido: %s\n", secuencia_actual);
                }
            }
        }
        usleep(100000);
    }

    close(fd);
    printf("Modo esclavo cerrado correctamente.\n");
}

// 📌 **Modo Maestro**
void modo_maestro() {
    const char *dispositivo = "/dev/ttyAMA0"; // Ajusta según el puerto correcto
    int fd = configurar_uart(dispositivo);
    if (fd == -1) return;

    const char *secuencias[] = {
        "AUTO", "CHOQUE", "APILADA", "CARRERA",
        "ESCALERA", "CHISPAS", "SIRENA", "MATRIX"
    };

    int opcion_secuencia = 0, velocidad_manual = 0;
    char comando[32];
    int ch;

    // Inicializar ncurses
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    curs_set(0);

    while (1) {
        clear();
        mvprintw(0, 0, "Modo Maestro: Seleccione una secuencia (ENTER para enviar)");
        for (int i = 0; i < 8; i++) {
            if (i == opcion_secuencia) {
                attron(A_REVERSE);
                mvprintw(i + 1, 0, "%d. %s", i + 1, secuencias[i]);
                attroff(A_REVERSE);
            } else {
                mvprintw(i + 1, 0, "%d. %s", i + 1, secuencias[i]);
            }
        }
        mvprintw(10, 0, "Presione 'q' para regresar al menu principal.");
        refresh();

        ch = getch();
        switch (ch) {
            case KEY_UP:
                opcion_secuencia = (opcion_secuencia > 0) ? opcion_secuencia - 1 : 7;
                break;
            case KEY_DOWN:
                opcion_secuencia = (opcion_secuencia < 7) ? opcion_secuencia + 1 : 0;
                break;
            case 10: { // ENTER
                memset(comando, 0, sizeof(comando));

                // Preguntar si quiere ingresar velocidad manualmente
                echo();
                mvprintw(12, 0, "Ingrese velocidad (0 para usar ADC): ");
                refresh();
                scanw("%d", &velocidad_manual);
                noecho();

                if (velocidad_manual > 0) {
                    snprintf(comando, sizeof(comando), "%s:%d", secuencias[opcion_secuencia], velocidad_manual);
                } else {
                    snprintf(comando, sizeof(comando), "%s:", secuencias[opcion_secuencia]);
                }

                // Enviar comando al esclavo
                write(fd, comando, strlen(comando));
                write(fd, "\n", 1);
                mvprintw(14, 0, "Comando enviado: %s", comando);
                refresh();
                usleep(1500000);
                break;
            }
            case 'q': // Regresar al menú principal
                close(fd);
                endwin();
                return;
        }
    }

    close(fd);
    endwin();
}
