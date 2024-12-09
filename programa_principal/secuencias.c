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

void secuencia_apilada(int *velocidad) {
    int leds[8] = {0};  // Array para representar el estado de los LEDs
    int ch;

    gpioInitialise();    // Inicialización de la librería de GPIO
    initscr();           // Inicializar pantalla
    raw();               // Configuración de entrada cruda
    noecho();            // Desactivar el eco de las teclas
    keypad(stdscr, TRUE);// Habilitar teclas especiales
    timeout(1);          // Configurar tiempo de espera en `getch`

    while ((ch = getch()) != KEY_F(2)) { // Salir con F2
        // Ajustar velocidad con flechas
        if (ch == KEY_UP) {
            *velocidad = (*velocidad > 100000) ? *velocidad - 50000 : *velocidad;
        } else if (ch == KEY_DOWN) {
            *velocidad = (*velocidad < 1000000) ? *velocidad + 50000 : *velocidad;
        }

        // Proceso principal: apilado de LEDs de derecha a izquierda
        for (int limit = 0; limit < 8; limit++) { // Desde el LED más bajo hasta el primero
            for (int i = 7; i >= limit; i--) {
                // Encender el LED actual y apagar el anterior
                for (int j = 0; j < 8; j++) {
                    leds[j] = (j == i) || (j < limit); // LED actual o LEDs ya fijados
                }

                interfaz(leds);       // Actualizar la interfaz visual
                gpioDelay(*velocidad); // Esperar la duración configurada
            }

            // Parpadeo del LED que se fija
            for (int blink = 0; blink < 3; blink++) {
                leds[limit] = 0; // Apagar el LED
                interfaz(leds);
                gpioDelay(300000); // Medio segundo apagado
                leds[limit] = 1; // Encender el LED
                interfaz(leds);
                gpioDelay(300000); // Medio segundo encendido
            }

            leds[limit] = 1; // Asegurarse de que el LED quede fijo
            interfaz(leds);
        }
    }

    // Apagar todos los LEDs y finalizar
    for (int i = 0; i < 8; i++) leds[i] = 0;
    interfaz(leds);
    gpioTerminate();
    refresh();
    endwin();
}

// Secuencia "La carrera"

void secuencia_carrera(int *velocidad) {
    int leds[8] = {0};  // Array para representar los LEDs, todos apagados inicialmente
    int ch;

    gpioInitialise();   // Inicializa la librería de GPIO
    initscr();           // Inicializa la interfaz ncurses
    raw();               // Configura la entrada cruda
    noecho();            // Desactiva el eco de teclas
    keypad(stdscr, TRUE); // Habilita las teclas especiales
    timeout(1);          // Configura un tiempo de espera para getch()

    while ((ch = getch()) != KEY_F(2)) {  // Salir si se presiona F2
        // Ajustar la velocidad con las teclas de flecha
        if (ch == KEY_UP) {
            *velocidad = (*velocidad > 100) ? *velocidad - 50 : *velocidad; // Decrecer velocidad
        } else if (ch == KEY_DOWN) {
            *velocidad = (*velocidad < 2000) ? *velocidad + 50 : *velocidad; // Aumentar velocidad
        }

        // Mover la primera luz a la mitad del recorrido (hasta el LED 4)
        for (int i = 0; i < 4; i++) {
            leds[i] = 1;               // Encender el LED actual
            interfaz(leds);            // Actualizar la interfaz de LEDs
            gpioDelay(*velocidad);     // Esperar con la velocidad definida
            leds[i] = 0;               // Apagar el LED actual
        }

        // Mover la segunda luz (a la mitad de la velocidad) comenzando desde el primer LED
        for (int i = 0; i < 4; i++) {
            leds[i] = 1;               // Encender el LED actual para la segunda luz
            interfaz(leds);            // Actualizar la interfaz de LEDs
            gpioDelay(*velocidad / 2); // Esperar con la velocidad rápida para la segunda luz
            leds[i] = 0;               // Apagar el LED actual
        }

        // Ambas luces continúan hasta el último LED
        for (int i = 4; i < 8; i++) {
            leds[i] = 1;               // Encender el LED correspondiente
            interfaz(leds);            // Actualizar la interfaz de LEDs
            gpioDelay(*velocidad);     // Esperar con la velocidad normal para la primera luz
            leds[i] = 0;               // Apagar el LED actual para la primera luz
        }

        // La segunda luz sigue hasta el último LED con el doble de velocidad
        for (int i = 4; i < 8; i++) {
            leds[i] = 1;               // Encender el LED correspondiente para la segunda luz
            interfaz(leds);            // Actualizar la interfaz de LEDs
            gpioDelay(*velocidad / 2); // Esperar con la velocidad rápida para la segunda luz
            leds[i] = 0;               // Apagar el LED actual para la segunda luz
        }
    }

    // Apagar todos los LEDs al salir de la secuencia
    for (int i = 0; i < 8; i++) {
        leds[i] = 0;
    }
    interfaz(leds);  // Apagar la interfaz de LEDs
    gpioTerminate(); // Terminar la librería de GPIO
    refresh();       // Actualizar la pantalla
    endwin();        // Finalizar ncurses
}
