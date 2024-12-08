#include <pigpio.h>
#include <ncurses.h>
#include <unistd.h>

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

// Secuencia "Auto fantástico"
void secuencia_auto_fantastico(int *velocidad) {
    int numero = 1;  // Empezamos desde la luz más a la izquierda
    int leds[8] = {0};
    int ch;
    int direccion = 1; // 1 para izquierda a derecha, -1 para derecha a izquierda

    gpioInitialise();
    initscr();
    raw();
    noecho();
    keypad(stdscr, TRUE);
    timeout(1);

    while ((ch = getch()) != KEY_F(2)) {  // Salir si se presiona F2
        // Cambiar la velocidad con las flechas
        if (ch == KEY_UP) {
            *velocidad = (*velocidad > 100000) ? *velocidad - 50000 : *velocidad;
        } else if (ch == KEY_DOWN) {
            *velocidad = (*velocidad < 1000000) ? *velocidad + 50000 : *velocidad;
        }

        itob(numero, leds);
        interfaz(leds);

        // Mover la luz de izquierda a derecha o de derecha a izquierda
        if (direccion == 1) {  // Izquierda a derecha
            if (numero < 128) {  // No hemos llegado al final
                numero = numero << 1;
            } else {
                direccion = -1;  // Cambiar dirección a derecha a izquierda
            }
        } else {  // Derecha a izquierda
            if (numero > 1) {  // No hemos llegado al inicio
                numero = numero >> 1;
            } else {
                direccion = 1;  // Cambiar dirección a izquierda a derecha
            }
        }

        gpioDelay(*velocidad);  // Pausar según la velocidad definida
    }

    // Apagar los LEDs al salir de la secuencia
    for (int i = 0; i < 8; i++) leds[i] = 0;
    interfaz(leds);
    gpioTerminate();
    refresh();
    endwin();
}


// Secuencia "El choque"

void secuencia_choque(int *velocidad) {
    int leds[8] = {0};
    int ch;
    int izquierda = 1;  // Luz en la izquierda
    int derecha = 128;  // Luz en la derecha
    int i, j;

    gpioInitialise();
    initscr();
    raw();
    noecho();
    keypad(stdscr, TRUE);
    timeout(1);

    while ((ch = getch()) != KEY_F(2)) {  // Salir si se presiona F2
        // Cambiar la velocidad con las flechas
        if (ch == KEY_UP) {
            *velocidad = (*velocidad > 100000) ? *velocidad - 50000 : *velocidad;
        } else if (ch == KEY_DOWN) {
            *velocidad = (*velocidad < 1000000) ? *velocidad + 50000 : *velocidad;
        }

        // Apagar todos los LEDs antes de actualizar
        for (int i = 0; i < 8; i++) leds[i] = 0;

        // Colocar las luces en las posiciones adecuadas
        leds[0] = (izquierda & 1);  // Luz de la izquierda
        leds[7] = (derecha & 1);    // Luz de la derecha

        // Avanzar las luces de izquierda y derecha
        for (i = 0, j = 7; i < 4 && j >= 4; i++, j--) {
            leds[i] = 1;   // Luz de la izquierda avanza
            leds[j] = 1;   // Luz de la derecha avanza
            interfaz(leds);
            gpioDelay(*velocidad);
            
            // Apagar las luces anteriores
            leds[i] = 0;
            leds[j] = 0;
        }

        // Después de cruzarse, las luces siguen avanzando hasta los extremos
        while (i < 8 && j >= 0) {
            leds[i] = 1;
            leds[j] = 1;
            interfaz(leds);
            gpioDelay(*velocidad);

            // Apagar las luces anteriores
            leds[i] = 0;
            leds[j] = 0;
            i++;
            j--;
        }
    }

    // Apagar todos los LEDs al salir de la secuencia
    for (int i = 0; i < 8; i++) leds[i] = 0;
    interfaz(leds);
    gpioTerminate();
    refresh();
    endwin();
}


// Secuencia "La apilada"

// Secuencia "La apilada" (de derecha a izquierda)

void secuencia_apilada(int *velocidad) {
    int leds[8] = {0};
    int ch;
    int i, j;

    gpioInitialise();
    initscr();
    raw();
    noecho();
    keypad(stdscr, TRUE);
    timeout(1);

    while ((ch = getch()) != KEY_F(2)) {  // Salir si se presiona F2
        // Cambiar la velocidad con las flechas
        if (ch == KEY_UP) {
            *velocidad = (*velocidad > 100000) ? *velocidad - 50000 : *velocidad;
        } else if (ch == KEY_DOWN) {
            *velocidad = (*velocidad < 1000000) ? *velocidad + 50000 : *velocidad;
        }

        // Inicializar LEDs apagados
        for (i = 0; i < 8; i++) leds[i] = 0;

        // Comenzar la secuencia de apilado (de derecha a izquierda)
        for (i = 7; i >= 0; i--) {  // Iniciar desde el LED más a la derecha
            // Encender las luces que ya están fijas (de derecha a izquierda)
            for (j = 7; j >= i; j--) leds[j] = 1;

            // Mostrar el LED actual moviéndose de derecha a izquierda
            for (j = i - 1; j >= 0; j--) {
                leds[j] = 1;  // Encender la luz en la posición actual
                interfaz(leds);  // Actualizar la interfaz
                gpioDelay(*velocidad);  // Esperar un momento
                leds[j] = 0;  // Apagar la luz al moverse
            }
        }
    }

    // Apagar todos los LEDs al salir de la secuencia
    for (i = 0; i < 8; i++) leds[i] = 0;
    interfaz(leds);
    gpioTerminate();
    refresh();
    endwin();
}

// Secuencia "La carrera"
void secuencia_carrera(int *velocidad) {
    int leds[8] = {0};
    int left = 1, right = 128;  // Inicializar dos luces

    gpioInitialise();
    initscr();
    raw();
    noecho();
    keypad(stdscr, TRUE);
    timeout(1);

    while ((getch()) != KEY_F(2)) {  // Salir si se presiona F2
        itob(left, leds);
        itob(right, leds + 4);  // Colocar la segunda luz en la mitad
        interfaz(leds);

        left <<= 1;  // Mover luz de izquierda a derecha
        right >>= 1;  // Mover luz de derecha a izquierda

        // Iniciar la segunda luz cuando la primera llega a la mitad
        if (left == 64) {
            right = 128;  // Reiniciar la segunda luz
        }

        gpioDelay(*velocidad);
    }

    // Apagar los LEDs al salir de la secuencia
    for (int i = 0; i < 8; i++) leds[i] = 0;
    interfaz(leds);
    gpioTerminate();
    refresh();
    endwin();
}

