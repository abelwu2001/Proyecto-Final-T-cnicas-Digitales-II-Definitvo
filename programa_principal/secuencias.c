#include <pigpio.h>
#include <ncurses.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#define LED1 24
#define LED2 25
#define LED3 8
#define LED4 7
#define LED5 12
#define LED6 16
#define LED7 20
#define LED8 21

void itob(int numero, int *leds) {
    for (int i = 0; i < 8; i++) {
        leds[i] = (numero >> (7 - i)) & 1;
    }
}

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

// Función base para ejecutar una secuencia con control remoto
void ejecutar_secuencia(void (*secuencia)(int *), int *velocidad) {
    int ch;
    initscr();
    raw();
    noecho();
    keypad(stdscr, TRUE);
    timeout(1);

    while ((ch = getch()) != KEY_F(2)) { // Salir con F2 desde el maestro
        if (ch == KEY_UP) {
            *velocidad = (*velocidad > 100000) ? *velocidad - 50000 : *velocidad;
        } else if (ch == KEY_DOWN) {
            *velocidad = (*velocidad < 1000000) ? *velocidad + 50000 : *velocidad;
        }
        secuencia(velocidad);
    }

    endwin();
}

// Secuencia "Auto Fantástico"
void secuencia_auto_fantastico(int *velocidad) {
    ejecutar_secuencia([](int *vel) {
        int numero = 1;
        int leds[8] = {0};
        int direccion = 1;

        while (1) {
            itob(numero, leds);
            interfaz(leds);

            if (direccion == 1) {
                numero = (numero < 128) ? numero << 1 : 64;
                if (numero == 64) direccion = -1;
            } else {
                numero = (numero > 1) ? numero >> 1 : 2;
                if (numero == 2) direccion = 1;
            }
            gpioDelay(*vel);
        }
    }, velocidad);
}

// Secuencia "El choque"
void secuencia_choque(int *velocidad) {
    ejecutar_secuencia([](int *vel) {
        int leds[8] = {0};

        while (1) {
            for (int i = 0, j = 7; i < 4; i++, j--) {
                memset(leds, 0, sizeof(leds));
                leds[i] = 1;
                leds[j] = 1;
                interfaz(leds);
                gpioDelay(*vel);
            }
            for (int i = 4, j = 3; i < 8; i++, j--) {
                memset(leds, 0, sizeof(leds));
                leds[i] = 1;
                leds[j] = 1;
                interfaz(leds);
                gpioDelay(*vel);
            }
        }
    }, velocidad);
}

// Secuencia "La apilada"
void secuencia_apilada(int *velocidad) {
    ejecutar_secuencia([](int *vel) {
        int leds[8] = {0};

        for (int limit = 0; limit < 8; limit++) {
            for (int i = 7; i >= limit; i--) {
                memset(leds, 0, sizeof(leds));
                leds[i] = 1;
                interfaz(leds);
                gpioDelay(*vel);
            }
        }
    }, velocidad);
}

// Secuencia "Carrera"
void secuencia_carrera(int *velocidad) {
    ejecutar_secuencia([](int *vel) {
        int leds[8] = {0};
        int pos1 = 0, pos2 = -1;

        while (1) {
            memset(leds, 0, sizeof(leds));
            leds[pos1] = 1;
            if (pos2 >= 0) leds[pos2] = 1;

            interfaz(leds);
            gpioDelay(*vel);

            pos1 = (pos1 + 1) % 8;
            if (pos1 == 4) pos2 = 0;
            if (pos2 >= 0) pos2 = (pos2 + 2) % 8;
        }
    }, velocidad);
}

// Secuencia "Escalera"
void secuencia_escalera(int *velocidad) {
    ejecutar_secuencia([](int *vel) {
        int leds[8] = {0};

        for (int i = 0; i < 8; i++) {
            leds[i] = 1;
            interfaz(leds);
            gpioDelay(*vel);
        }
    }, velocidad);
}

// Secuencia "Chispas"
void secuencia_chispas(int *velocidad) {
    ejecutar_secuencia([](int *vel) {
        int leds[8] = {0};
        srand(time(NULL));

        while (1) {
            memset(leds, 0, sizeof(leds));
            leds[rand() % 8] = 1;
            interfaz(leds);
            gpioDelay(*vel);
        }
    }, velocidad);
}

// Secuencia "Sirena"
void secuencia_sirena(int *velocidad) {
    ejecutar_secuencia([](int *vel) {
        int tabla_sirena[8] = {0b10101010, 0b01010101, 0b11110000, 0b00001111,
                               0b11111100, 0b00000011, 0b10000000, 0b00000001};
        int leds[8];

        for (int i = 0; i < 8; i++) {
            itob(tabla_sirena[i], leds);
            interfaz(leds);
            gpioDelay(*vel);
        }
    }, velocidad);
}

// Secuencia "Matrix"
void secuencia_matrix(int *velocidad) {
    ejecutar_secuencia([](int *vel) {
        int leds[8] = {0};

        for (int i = 0; i < 8; i++) {
            leds[i] = 1;
            interfaz(leds);
            gpioDelay(*vel);
        }
        for (int i = 0; i < 8; i++) {
            leds[i] = 0;
            interfaz(leds);
            gpioDelay(*vel);
        }
    }, velocidad);
}
