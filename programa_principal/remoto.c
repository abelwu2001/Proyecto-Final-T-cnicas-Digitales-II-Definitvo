#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <ncurses.h>
#include "remoto.h"
#include "secuencias.h"

extern int velocidad; // Velocidad inicial, compartida con el programa principal


#include "secuencias.h"  // Incluir funciones de secuencias
#include <pigpio.h>      // Para el ADC

#define PCF8591_I2C_ADDR 0x48

int i2cHandle;

// Inicializar ADC
void inicializar_adc() {
    gpioInitialise();
    i2cHandle = i2cOpen(1, PCF8591_I2C_ADDR, 0);
    if (i2cHandle < 0) {
        fprintf(stderr, "Error al inicializar el PCF8591\n");
        exit(1);
    }
}

// Leer ADC
int leer_adc() {
    i2cWriteByte(i2cHandle, 0x40);  // Leer canal 0
    usleep(1000);
    return i2cReadByte(i2cHandle) * 1000;  // Convertir ADC a microsegundos
}


// Función para configurar el UART
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
    const char *dispositivo = "/dev/serial0";
    int fd = configurar_uart(dispositivo); // Configuración UART
    if (fd == -1) return;

    inicializar_adc(); // Inicializar el ADC

    char comando[32]; // Buffer para recibir el comando
    char secuencia[16];
    int velocidad = 0;

    printf("Modo esclavo: esperando comandos en %s...\n", dispositivo);

    while (1) {
        memset(comando, 0, sizeof(comando)); // Limpiar el buffer
        int bytes_leidos = read(fd, comando, sizeof(comando) - 1);
        if (bytes_leidos > 0) {
            comando[bytes_leidos] = '\0'; // Asegurar terminación de cadena
            printf("Comando recibido: %s\n", comando);

            // Parsear secuencia y velocidad
            if (sscanf(comando, "%15[^:]:%d", secuencia, &velocidad) == 2) {
                printf("Secuencia: %s, Velocidad: %d us\n", secuencia, velocidad);
            } else if (sscanf(comando, "%15[^:]:", secuencia) == 1) {
                velocidad = leer_adc(); // Leer velocidad desde el ADC
                printf("Secuencia: %s, Velocidad (ADC): %d us\n", secuencia, velocidad);
            } else {
                printf("Formato de comando inválido: %s\n", comando);
                continue;
            }

            // Ejecutar la secuencia correspondiente
            if (strcmp(secuencia, "AUTO") == 0) secuencia_auto_fantastico(&velocidad);
            else if (strcmp(secuencia, "CHOQUE") == 0) secuencia_choque(&velocidad);
            else if (strcmp(secuencia, "APILADA") == 0) secuencia_apilada(&velocidad);
            else if (strcmp(secuencia, "CARRERA") == 0) secuencia_carrera(&velocidad);
            else if (strcmp(secuencia, "ESCALERA") == 0) secuencia_escalera(&velocidad);
            else if (strcmp(secuencia, "CHISPAS") == 0) secuencia_chispas(&velocidad);
            else if (strcmp(secuencia, "SIRENA") == 0) secuencia_sirena(&velocidad);
            else if (strcmp(secuencia, "MATRIX") == 0) secuencia_matrix(&velocidad);
            else printf("Secuencia no reconocida: %s\n", secuencia);
        }
    }

    close(fd);
    gpioTerminate(); // Finalizar el ADC
}



// Función para modo maestro

void modo_maestro() {
    const char *dispositivo = "/dev/ttyUSB0";
    int fd = configurar_uart(dispositivo);
    if (fd == -1) return;

    const char *secuencias[] = {
        "AUTO", "CHOQUE", "APILADA", "CARRERA", 
        "ESCALERA", "CHISPAS", "SIRENA", "MATRIX"
    };
    int seleccion = 0, modo_velocidad = 0, velocidad_manual;
    char comando[32];
    int ch;

    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    curs_set(0);

    // Selección del modo de velocidad
    while (1) {
        clear();
        mvprintw(0, 0, "Modo Maestro: Seleccione el modo de velocidad");
        mvprintw(1, 0, "1. Manual (escriba la velocidad)");
        mvprintw(2, 0, "2. ADC (velocidad predeterminada en el esclavo)");
        refresh();

        ch = getch();
        if (ch == '1') modo_velocidad = 1;  // Manual
        else if (ch == '2') modo_velocidad = 2;  // ADC
        else continue;
        break;
    }

    // Menú para seleccionar las secuencias
    while (1) {
        clear();
        mvprintw(0, 0, "Modo Maestro: Seleccione la secuencia (ENTER para enviar):");

        for (int i = 0; i < 8; i++) {
            if (i == seleccion) {
                attron(A_REVERSE);
                mvprintw(i + 1, 0, "%d. %s", i + 1, secuencias[i]);
                attroff(A_REVERSE);
            } else {
                mvprintw(i + 1, 0, "%d. %s", i + 1, secuencias[i]);
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
            case 10: { // ENTER para enviar comando
                memset(comando, 0, sizeof(comando));

                if (modo_velocidad == 1) {  // Manual
                    echo();
                    mvprintw(10, 0, "Ingrese velocidad (us): ");
                    scanw("%d", &velocidad_manual);
                    noecho();
                    snprintf(comando, sizeof(comando), "%s:%d", secuencias[seleccion], velocidad_manual);
                } else {  // ADC
                    snprintf(comando, sizeof(comando), "%s:", secuencias[seleccion]);
                }

                // Enviar el comando al esclavo
                write(fd, comando, strlen(comando));
                write(fd, "\n", 1);

                mvprintw(12, 0, "Comando enviado: %s", comando);
                refresh();
                usleep(1500000);
                break;
            }
            case 'q': // Salir
                close(fd);
                endwin();
                return;
        }
    }

    close(fd);
    endwin();
}
