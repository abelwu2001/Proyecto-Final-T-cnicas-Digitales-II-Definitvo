#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <ncurses.h>
#include <stdlib.h>
#include "remoto.h"
#include "secuencias.h"

extern int velocidad; // Velocidad inicial, compartida con el programa principal
extern int leer_adc(int canal); // Función para leer el ADC en modo esclavo

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

// Función para modo esclavo
void modo_esclavo() {
    const char *dispositivo = "/dev/ttyUSB0"; // Por defecto usa USB0
    int fd = configurar_uart(dispositivo);
    if (fd == -1) return;

    char comando[32];
    int velocidad_actual = leer_adc(0) * 1000; // Usar velocidad del ADC por defecto
    int ejecutando = 1; // Variable de control para detener la secuencia

    printf("Modo esclavo: esperando comandos en %s...\n", dispositivo);
    printf("Escriba 'SALIR' para cerrar el modo esclavo.\n");

    while (ejecutando) {
        memset(comando, 0, sizeof(comando)); // Limpiar el buffer
        int bytes_leidos = read(fd, comando, sizeof(comando) - 1);

        if (bytes_leidos > 0) {
            comando[bytes_leidos] = '\0'; // Asegurar que la cadena termine en '\0'
            printf("Comando recibido: %s\n", comando);

            if (strcmp(comando, "SALIR") == 0 || strcmp(comando, "salir") == 0) {
                printf("Comando 'SALIR' recibido. Cerrando modo esclavo...\n");
                break;
            }

            // Si el comando incluye velocidad (formato: "SECUENCIA:VEL")
            char *token = strtok(comando, ":");
            char *velocidad_str = strtok(NULL, ":");
            if (velocidad_str) {
                velocidad_actual = atoi(velocidad_str);
            }

            // Ejecutar secuencia según el comando recibido
            if (strcmp(token, "AUTO") == 0) secuencia_auto_fantastico(&velocidad_actual);
            else if (strcmp(token, "CHOQUE") == 0) secuencia_choque(&velocidad_actual);
            else if (strcmp(token, "APILADA") == 0) secuencia_apilada(&velocidad_actual);
            else if (strcmp(token, "CARRERA") == 0) secuencia_carrera(&velocidad_actual);
            else if (strcmp(token, "ESCALERA") == 0) secuencia_escalera(&velocidad_actual);
            else if (strcmp(token, "CHISPAS") == 0) secuencia_chispas(&velocidad_actual);
            else if (strcmp(token, "SIRENA") == 0) secuencia_sirena(&velocidad_actual);
            else if (strcmp(token, "MATRIX") == 0) secuencia_matrix(&velocidad_actual);
            else printf("Comando no reconocido: %s\n", token); // Manejo de error
        }
    }

    close(fd); // Cerrar UART al salir
    printf("Modo esclavo cerrado correctamente.\n");
}

// Función para modo maestro
void modo_maestro() {
    const char *dispositivo = "/dev/ttyS0"; // Ajusta según el puerto correcto
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
        mvprintw(3, 0, "Presione 'q' para regresar al menú principal.");
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
        mvprintw(10, 0, "Presione 'q' para regresar al menú principal.");
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
                endwin();
                return;
        }
    }

    close(fd);
    endwin();
}
