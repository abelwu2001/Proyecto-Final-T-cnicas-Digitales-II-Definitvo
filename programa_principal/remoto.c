#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <ncurses.h>
#include "remoto.h"
#include "secuencias.h"
#include  <stdlib.h>
extern int velocidad; // Velocidad inicial, compartida con el programa principal

void key_event(int key);

// Configura el UART para la comunicación serie
int configurar_uart(const char *dispositivo) {
    int fd = open(dispositivo, O_RDWR | O_NOCTTY);
    if (fd == -1) {
        perror("Error al abrir UART");
        return -1;
    }

    struct termios options;
    tcgetattr(fd, &options);
    cfsetispeed(&options, B9600); // Velocidad de entrada
    cfsetospeed(&options, B9600); // Velocidad de salida
    options.c_cflag |= (CLOCAL | CREAD); // Habilitar lectura y mantener conexión activa
    options.c_cflag &= ~CSIZE; // Limpiar tamaño de los bits
    options.c_cflag |= CS8;    // 8 bits por palabra
    options.c_cflag &= ~PARENB; // Sin paridad
    options.c_cflag &= ~CSTOPB; // 1 bit de parada
    options.c_cflag &= ~CRTSCTS; // Sin control de hardware
    options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG); // Modo sin buffer
    options.c_iflag &= ~(IXON | IXOFF | IXANY); // Sin control de flujo
    options.c_oflag &= ~OPOST; // Salida sin procesamiento
    tcsetattr(fd, TCSANOW, &options);

    printf("UART configurado en: %s\n", dispositivo);
    return fd;
}

void key_event(int key) {
    MEVENT event;
    memset(&event, 0, sizeof(event));
    event.bstate = key;
}

// Función para modo esclavo
void modo_esclavo() {
    const char *dispositivo = "/dev/ttyUSB0"; // Ajustar al puerto UART correcto
    int fd = configurar_uart(dispositivo);
    if (fd == -1) return; // Si no se abre el UART, salir

    char comando[32];
    int velocidad_actual = leer_adc(0) * 1000; // Usar velocidad del ADC por defecto
    int ejecutando_secuencia = 0;
    char secuencia_actual[16] = "";

    printf("Modo esclavo activo en %s. Esperando comandos del Maestro...\n", dispositivo);
    printf("Escriba 'SALIR' en el Maestro para salir del modo esclavo.\n");

    while (1) {
        memset(comando, 0, sizeof(comando)); // Limpiar el buffer de comandos
        int bytes_leidos = read(fd, comando, sizeof(comando) - 1);

        if (bytes_leidos > 0) {
            comando[bytes_leidos] = '\0'; // Asegurar el final de la cadena
            printf("Comando recibido: %s\n", comando);

            // Comprobar si el comando es "SALIR"
            if (strncmp(comando, "SALIR", 5) == 0) {
                printf("Cerrando modo esclavo...\n");
                break;
            }

            // Comprobar si el comando es "STOP"
            if (strncmp(comando, "STOP", 4) == 0) {
                printf("Deteniendo secuencia actual...\n");
                ejecutando_secuencia = 0;
                continue;
            }

            // Procesar el comando en formato "SECUENCIA:VELOCIDAD"
            char *token = strtok(comando, ":");
            if (token != NULL) {
                strcpy(secuencia_actual, token);
                token = strtok(NULL, ":");
                if (token != NULL) {
                    velocidad_actual = atoi(token); // Obtener la nueva velocidad
                } else {
                    velocidad_actual = leer_adc(0) * 1000; // Usar ADC si no hay velocidad en el comando
                }
            }

            // Si la secuencia es válida, iniciarla en un hilo separado
            if (ejecutando_secuencia == 0) {
                ejecutando_secuencia = 1;
                if (strncmp(secuencia_actual, "AUTO", 4) == 0) {
                    secuencia_auto_fantastico(&velocidad_actual);
                } else if (strncmp(secuencia_actual, "CHOQUE", 6) == 0) {
                    secuencia_choque(&velocidad_actual);
                } else if (strncmp(secuencia_actual, "APILADA", 7) == 0) {
                    secuencia_apilada(&velocidad_actual);
                } else if (strncmp(secuencia_actual, "CARRERA", 7) == 0) {
                    secuencia_carrera(&velocidad_actual);
                } else if (strncmp(secuencia_actual, "ESCALERA", 8) == 0) {
                    secuencia_escalera(&velocidad_actual);
                } else if (strncmp(secuencia_actual, "CHISPAS", 7) == 0) {
                    secuencia_chispas(&velocidad_actual);
                } else if (strncmp(secuencia_actual, "SIRENA", 6) == 0) {
                    secuencia_sirena(&velocidad_actual);
                } else if (strncmp(secuencia_actual, "MATRIX", 6) == 0) {
                    secuencia_matrix(&velocidad_actual);
                } else {
                    printf("Comando no reconocido: %s\n", secuencia_actual);
                    ejecutando_secuencia = 0;
                }
            }
        }

        usleep(100000); // Reducir uso de CPU
    }

    close(fd); // Cerrar UART antes de salir
    printf("Modo esclavo cerrado correctamente. Volviendo al menú principal.\n");
}


// Función para modo maestro

// Menú simple para modo maestro
void modo_maestro() {
    const char *dispositivo = "/dev/ttyUSB0"; // Ajustar al puerto UART correcto
    int fd = configurar_uart(dispositivo);
    if (fd == -1) return;

    const char *secuencias[] = {
        "AUTO", "CHOQUE", "APILADA", "CARRERA",
        "ESCALERA", "CHISPAS", "SIRENA", "MATRIX"
    };

    int opcion_secuencia = 0;
    int velocidad_actual = leer_adc(0) * 1000; // Velocidad inicial desde el ADC
    char comando[32];
    int ch;

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
        mvprintw(10, 0, "Velocidad actual: %d us", velocidad_actual);
        mvprintw(11, 0, "Flechas Arriba/Abajo para cambiar velocidad");
        mvprintw(12, 0, "ENTER para iniciar secuencia");
        mvprintw(13, 0, "'s' para detener secuencia, 'q' para salir.");
        refresh();

        ch = getch();
        switch (ch) {
            case KEY_UP:
                velocidad_actual = (velocidad_actual > 100000) ? velocidad_actual - 50000 : velocidad_actual;
                break;
            case KEY_DOWN:
                velocidad_actual = (velocidad_actual < 1000000) ? velocidad_actual + 50000 : velocidad_actual;
                break;
            case 10: // ENTER
                snprintf(comando, sizeof(comando), "%s", secuencias[opcion_secuencia]);
                write(fd, comando, strlen(comando));
                write(fd, "\n", 1);
                break;
            case 'v': // Enviar secuencia con velocidad manual
                echo();
                mvprintw(15, 0, "Ingrese la velocidad en us: ");
                refresh();
                scanw("%d", &velocidad_actual);
                noecho();
                snprintf(comando, sizeof(comando), "%s:%d", secuencias[opcion_secuencia], velocidad_actual);
                write(fd, comando, strlen(comando));
                write(fd, "\n", 1);
                break;
            case 's': // Detener secuencia en esclavo
                write(fd, "STOP\n", 5);
                break;
            case 'q': // Regresar al menú principal
                close(fd);
                endwin();
                return;
        }
    }

    close(fd);
    endwin();
}

