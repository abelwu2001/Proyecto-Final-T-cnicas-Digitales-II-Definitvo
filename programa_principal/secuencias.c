#include <pigpio.h>
#include <ncurses.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>

#define LED1 24
#define LED2 25
#define LED3 8
#define LED4 7
#define LED5 12
#define LED6 16
#define LED7 20
#define LED8 21

// Función para convertir un número a una representación binaria en los LEDs
void itob(int numero, int *leds) {
    for (int i = 0; i < 8; i++) {
        leds[i] = (numero >> (7 - i)) & 1;
    }
}

// Función para actualizar los LEDs físicos
void interfaz(int *leds) {
    gpioWrite(LED1, leds[0]);
    gpioWrite(LED2, leds[1]);
    gpioWrite(LED3, leds[2]);
    gpioWrite(LED4, leds[3]);
    gpioWrite(LED5, leds[4]);
    gpioWrite(LED6, leds[5]);
    gpioWrite(LED7, leds[6]);
    gpioWrite(LED8, leds[7]);
}

// Función genérica para ejecutar secuencias
void ejecutar_secuencia(void (*secuencia)(int *), int *velocidad) {
    gpioInitialise();
    secuencia(velocidad);
    gpioTerminate();
}

// 🔹 **Secuencia "Auto Fantástico"**
void secuencia_auto_fantastico_real(int *velocidad) {
    int numero = 1;
    int leds[8] = {0};
    int direccion = 1;

    while (1) {
        itob(numero, leds);
        interfaz(leds);

        if (direccion == 1) {
            if (numero < 128) numero <<= 1;
            else direccion = -1;
        } else {
            if (numero > 1) numero >>= 1;
            else direccion = 1;
        }
        gpioDelay(*velocidad);
    }
}

void secuencia_auto_fantastico(int *velocidad) {
    ejecutar_secuencia(secuencia_auto_fantastico_real, velocidad);
}

// 🔹 **Secuencia "Choque"**
void secuencia_choque_real(int *velocidad) {
    int leds[8] = {0};
    while (1) {
        for (int i = 0, j = 7; i < 4 && j >= 4; i++, j--) {
            leds[i] = 1;
            leds[j] = 1;
            interfaz(leds);
            gpioDelay(*velocidad);
            leds[i] = 0;
            leds[j] = 0;
        }
    }
}

void secuencia_choque(int *velocidad) {
    ejecutar_secuencia(secuencia_choque_real, velocidad);
}

// 🔹 **Secuencia "Apilada"**
void secuencia_apilada_real(int *velocidad) {
    int leds[8] = {0};
    for (int limit = 0; limit < 8; limit++) {
        for (int i = 7; i >= limit; i--) {
            leds[i] = 1;
            interfaz(leds);
            gpioDelay(*velocidad);
        }
    }
}

void secuencia_apilada(int *velocidad) {
    ejecutar_secuencia(secuencia_apilada_real, velocidad);
}

// 🔹 **Secuencia "Carrera"**
void secuencia_carrera_real(int *velocidad) {
    int leds[8] = {0};
    int primera_pos = 0, segunda_pos = -1;
    while (1) {
        leds[primera_pos] = 1;
        if (segunda_pos >= 0) leds[segunda_pos] = 1;

        interfaz(leds);
        gpioDelay(*velocidad);

        leds[primera_pos] = 0;
        if (segunda_pos >= 0) leds[segunda_pos] = 0;

        primera_pos++;
        if (primera_pos == 4) segunda_pos = 0;
        if (segunda_pos >= 0) segunda_pos += 2;

        if (primera_pos >= 8) {
            primera_pos = 0;
            segunda_pos = -1;
        }
    }
}

void secuencia_carrera(int *velocidad) {
    ejecutar_secuencia(secuencia_carrera_real, velocidad);
}

// 🔹 **Secuencia "Escalera"**
void secuencia_escalera_real(int *velocidad) {
    int leds[8] = {0};
    while (1) {
        for (int i = 0; i < 8; i++) {
            leds[i] = 1;
            interfaz(leds);
            gpioDelay(*velocidad);
        }
    }
}

void secuencia_escalera(int *velocidad) {
    ejecutar_secuencia(secuencia_escalera_real, velocidad);
}

// 🔹 **Secuencia "Chispas"**
void secuencia_chispas_real(int *velocidad) {
    int leds[8] = {0};
    srand(time(NULL));
    while (1) {
        for (int i = 0; i < 8; i++) leds[i] = 0;
        leds[rand() % 8] = 1;
        interfaz(leds);
        gpioDelay(*velocidad);
    }
}

void secuencia_chispas(int *velocidad) {
    ejecutar_secuencia(secuencia_chispas_real, velocidad);
}

// 🔹 **Secuencia "Sirena"**
void secuencia_sirena_real(int *velocidad) {
    int leds[8] = {0};
    int tabla_sirena[16][8] = {
        {1, 0, 1, 0, 1, 0, 1, 0}, {0, 1, 0, 1, 0, 1, 0, 1},
        {1, 1, 1, 1, 0, 0, 0, 0}, {0, 0, 0, 0, 1, 1, 1, 1},
        {1, 1, 1, 1, 1, 1, 0, 0}, {0, 0, 0, 0, 0, 0, 1, 1}
    };
    while (1) {
        for (int i = 0; i < 6; i++) {
            for (int j = 0; j < 8; j++) leds[j] = tabla_sirena[i][j];
            interfaz(leds);
            gpioDelay(*velocidad);
        }
    }
}

void secuencia_sirena(int *velocidad) {
    ejecutar_secuencia(secuencia_sirena_real, velocidad);
}

// 🔹 **Secuencia "Matrix"**
void secuencia_matrix_real(int *velocidad) {
    int leds[8] = {0};
    int tabla_matrix[8][8] = {
        {1, 0, 1, 0, 1, 0, 1, 0}, {0, 1, 0, 1, 0, 1, 0, 1},
        {1, 1, 0, 1, 1, 0, 1, 1}, {0, 1, 1, 0, 1, 1, 0, 1}
    };
    while (1) {
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 8; j++) leds[j] = tabla_matrix[i][j];
            interfaz(leds);
            gpioDelay(*velocidad);
        }
    }
}

void secuencia_matrix(int *velocidad) {
    ejecutar_secuencia(secuencia_matrix_real, velocidad);
}
