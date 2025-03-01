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

            // Si la secuencia es válida, iniciarla
            if (ejecutando_secuencia == 0) {
                ejecutando_secuencia = 1;

                if (strncmp(secuencia_actual, "AUTO", 4) == 0) {
                    secuencia_auto_fantastico(&velocidad_actual);
                    ejecutando_secuencia = 0;
                    endwin();
                } else if (strncmp(secuencia_actual, "CHOQUE", 6) == 0) {
                    secuencia_choque(&velocidad_actual);
                    ejecutando_secuencia = 0;
                    endwin();
                } else if (strncmp(secuencia_actual, "APILADA", 7) == 0) {
                    secuencia_apilada(&velocidad_actual);
                    ejecutando_secuencia = 0;
                    endwin();
                } else if (strncmp(secuencia_actual, "CARRERA", 7) == 0) {
                    secuencia_carrera(&velocidad_actual);
                    ejecutando_secuencia = 0;
                    endwin();
                } else if (strncmp(secuencia_actual, "ESCALERA", 8) == 0) {
                    secuencia_escalera(&velocidad_actual);
                    ejecutando_secuencia = 0;
                    endwin();
                } else if (strncmp(secuencia_actual, "CHISPAS", 7) == 0) {
                    secuencia_chispas(&velocidad_actual);
                    ejecutando_secuencia = 0;
                    endwin();
                } else if (strncmp(secuencia_actual, "SIRENA", 6) == 0) {
                    secuencia_sirena(&velocidad_actual);
                    ejecutando_secuencia = 0;
                    endwin();
                } else if (strncmp(secuencia_actual, "MATRIX", 6) == 0) {
                    secuencia_matrix(&velocidad_actual);
                    ejecutando_secuencia = 0;
                    endwin();
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
    const char *dispositivo = "/dev/ttyS0";// Ajusta según el puerto correcto
    int fd = configurar_uart(dispositivo);
    if (fd == -1) return;

    const char *secuencias[] = {
        "AUTO", "CHOQUE", "APILADA", "CARRERA",
        "ESCALERA", "CHISPAS", "SIRENA", "MATRIX"
    };
    int opcion_secuencia = 0, modo_velocidad = 0, velocidad_manual = 0;
    char comando[32];
    int ch;

    // Inicializar ncurses
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    curs_set(0);

    // Selección del modo de velocidad
    while (1) {
        clear();
        mvprintw(0, 0, "Modo Maestro: Seleccione el modo de velocidad");
        mvprintw(1, 0, "1. Manual (ingresar velocidad en us)");
        mvprintw(2, 0, "2. Automático (ADC en esclavo)");
        mvprintw(3, 0, "Presione 'q' para regresar al menu principal.");
        refresh();

        ch = getch();
        if (ch == '1') {
            modo_velocidad = 1;
            break;
        } else if (ch == '2') {
            modo_velocidad = 2;
            break;
        } else if (ch == 'q') {
            close(fd);
            return; // Regresa al menú principal sin salir del programa
        }
    }

    // Menú para seleccionar las secuencias
    while (1) {
        clear();
        mvprintw(0, 0, "Modo Maestro: Seleccione una secuencia (ENTER para enviar):");
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

                if (modo_velocidad == 1) {  // Modo Manual
                    echo();
                    mvprintw(12, 0, "Ingrese la velocidad en microsegundos: ");
                    scanw("%d", &velocidad_manual);
                    noecho();
                    snprintf(comando, sizeof(comando), "%s:%d", secuencias[opcion_secuencia], velocidad_manual);
                } else {  // Modo Automático (ADC)
                    snprintf(comando, sizeof(comando), "%s:", secuencias[opcion_secuencia]);
                }

                // Enviar comando al esclavo
                write(fd, comando, strlen(comando));
                write(fd, "\n", 1);

                mvprintw(14, 0, "Comando enviado: %s", comando);
                refresh();
                usleep(1500000); // Mostrar el mensaje durante 1.5 segundos
                break;
            }
            case 'q': // Regresar al menú principal
                close(fd);
                endwin(); // Finaliza ncurses correctamente
                return;  // Regresa al menú principal
        }
    }

    close(fd);
    endwin(); // Finaliza ncurses
}
