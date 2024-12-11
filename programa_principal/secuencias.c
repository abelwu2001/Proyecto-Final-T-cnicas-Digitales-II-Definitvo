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
#include <time.h>
#include <stdlib.h>
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
int numero = 1; // Empezamos desde la luz más a la izquierda
int leds[8] = {0};
int ch;
int direccion = 1; // 1 para izquierda a derecha, -1 para derecha a izquierda
int rows, cols;
gpioInitialise();
initscr();
raw();
noecho();
keypad(stdscr, TRUE);
timeout(1);
// Obtener las dimensiones de la pantalla
getmaxyx(stdscr, rows, cols);
// Definir las dimensiones de la ventana para el menú de la secuencia
int ventana_height = 6; // Altura de la ventana (cuántas filas ocupará)
int ventana_width = 40; // Ancho de la ventana (cuántas columnas ocupará)
int ventana_start_y = (rows - ventana_height) / 2; // Centrar verticalmente
int ventana_start_x = (cols - ventana_width) / 2; // Centrar horizontalmente
// Crear una ventana en ncurses
WINDOW *sec_window = newwin(ventana_height, ventana_width, ventana_start_y, ventana_start_x);
box(sec_window, 0, 0); // Dibujar un borde alrededor de la ventana
while ((ch = getch()) != KEY_F(2)) { // Salir si se presiona F2
// Cambiar la velocidad con las flechas
if (ch == KEY_UP) {
*velocidad = (*velocidad > 100000) ? *velocidad - 50000 : *velocidad;
} else if (ch == KEY_DOWN) {
*velocidad = (*velocidad < 1000000) ? *velocidad + 50000 : *velocidad;
}
// Convertir el número a la representación binaria de los LEDs
itob(numero, leds);
interfaz(leds); // Actualizar los LEDs
// Mover la luz de izquierda a derecha o de derecha a izquierda
if (direccion == 1) { // Izquierda a derecha
if (numero < 128) { // No hemos llegado al final
numero = numero << 1;
} else {
direccion = -1; // Cambiar dirección a derecha a izquierda
}
} else { // Derecha a izquierda
if (numero > 1) { // No hemos llegado al inicio
numero = numero >> 1;
} else {
direccion = 1; // Cambiar dirección a izquierda a derecha
}
}
gpioDelay(*velocidad); // Pausar según la velocidad definida
// Actualizar la ventana con el nombre de la secuencia y la velocidad
wclear(sec_window); // Limpiar la ventana antes de actualizarla
box(sec_window, 0, 0); // Redibujar el borde de la ventana
// Escribir el nombre de la secuencia y la velocidad en la ventana
mvwprintw(sec_window, 1, 2, "Secuencia: Auto Fantástico");
mvwprintw(sec_window, 2, 2, "Velocidad: %dus", *velocidad);
mvwprintw(sec_window, 4, 2, "Usa las flechas para ajustar velocidad");
mvwprintw(sec_window, 5, 2, "Presiona F2 para volver al menu");
wrefresh(sec_window); // Actualizar la ventana
}
// Apagar todos los LEDs al salir de la secuencia
for (int i = 0; i < 8; i++) leds[i] = 0;
interfaz(leds); // Actualizar LEDs
gpioTerminate(); // Terminar la inicialización de GPIO
refresh(); // Actualizar la pantalla
endwin(); // Finalizar la ventana ncurses
}
// Secuencia "El choque"
void secuencia_choque(int *velocidad) {
int leds[8] = {0};
int ch;
int i, j;
int rows, cols;
gpioInitialise();
initscr();
raw();
noecho();
keypad(stdscr, TRUE);
timeout(1);
// Obtener las dimensiones de la pantalla
getmaxyx(stdscr, rows, cols);
// Crear ventana centrada
int ventana_height = 8;
int ventana_width = 40;
int ventana_start_y = (rows - ventana_height) / 2;
int ventana_start_x = (cols - ventana_width) / 2;
WINDOW *sec_window = newwin(ventana_height, ventana_width, ventana_start_y, ventana_start_x);
box(sec_window, 0, 0);
while ((ch = getch()) != KEY_F(2)) { // Salir si se presiona F2
// Cambiar la velocidad con las flechas
if (ch == KEY_UP) {
*velocidad = (*velocidad > 100000) ? *velocidad - 50000 : *velocidad;
} else if (ch == KEY_DOWN) {
*velocidad = (*velocidad < 1000000) ? *velocidad + 50000 : *velocidad;
}
// Limpiar los LEDs
for (int k = 0; k < 8; k++) leds[k] = 0;
// Mover luces desde extremos hacia el centro
for (i = 0, j = 7; i < 4 && j >= 4; i++, j--) {
if ((ch = getch()) == KEY_F(2)) {
for (int k = 0; k < 8; k++) leds[k] = 0;
interfaz(leds); // Actualizar LEDs
return; // Salir inmediatamente
} else if (ch == KEY_UP) {
*velocidad = (*velocidad > 100000) ? *velocidad - 50000 : *velocidad;
} else if (ch == KEY_DOWN) {
*velocidad = (*velocidad < 1000000) ? *velocidad + 50000 : *velocidad;
}
wclear(sec_window);
box(sec_window, 0, 0);
mvwprintw(sec_window, 1, 1, "Secuencia: Choque");
mvwprintw(sec_window, 3, 1, "Velocidad: %dus", *velocidad);
mvwprintw(sec_window, 5, 1, "Usa las flechas para ajustar velocidad");
mvwprintw(sec_window, 6, 1, "Presiona F2 para volver al menu");
wrefresh(sec_window);
leds[i] = 1; // Avanzar desde la izquierda
leds[j] = 1; // Avanzar desde la derecha
interfaz(leds);
gpioDelay(*velocidad);
// Apagar las luces anteriores
leds[i] = 0;
leds[j] = 0;
}
// Mover luces desde el centro hacia los extremos
for (; i < 8 && j >= 0; i++, j--) {
leds[i] = 1;
leds[j] = 1;
interfaz(leds);
gpioDelay(*velocidad);
// Apagar las luces anteriores
leds[i] = 0;
leds[j] = 0;
wclear(sec_window);
box(sec_window, 0, 0);
mvwprintw(sec_window, 1, 1, "Secuencia: Choque");
mvwprintw(sec_window, 3, 1, "Velocidad: %dus", *velocidad);
mvwprintw(sec_window, 5, 1, "Usa las flechas para ajustar velocidad");
mvwprintw(sec_window, 6, 1, "Presiona F2 para volver al menu");
wrefresh(sec_window);
if ((ch = getch()) == KEY_F(2)) {
for (int k = 0; k < 8; k++) leds[k] = 0;
interfaz(leds); // Actualizar LEDs
return; // Salir inmediatamente
} else if (ch == KEY_UP) {
*velocidad = (*velocidad > 100000) ? *velocidad - 50000 : *velocidad;
} else if (ch == KEY_DOWN) {
*velocidad = (*velocidad < 1000000) ? *velocidad + 50000 : *velocidad;
}
}
// Actualizar la ventana con el estado actual
wclear(sec_window);
box(sec_window, 0, 0);
mvwprintw(sec_window, 1, 1, "Secuencia: Choque");
mvwprintw(sec_window, 3, 1, "Velocidad: %dus", *velocidad);
mvwprintw(sec_window, 5, 1, "Usa las flechas para ajustar velocidad");
mvwprintw(sec_window, 6, 1, "Presiona F2 para volver al menu");
wrefresh(sec_window);
}
// Apagar todos los LEDs al salir de la secuencia
for (int k = 0; k < 8; k++) leds[k] = 0;
interfaz(leds);
// Limpiar y cerrar la ventana
delwin(sec_window);
gpioTerminate();
refresh();
endwin();
}
// Secuencia "La apilada"
void secuencia_apilada(int *velocidad) {
int leds[8] = {0}; // Array para representar el estado de los LEDs
int ch;
gpioInitialise(); // Inicialización de la librería de GPIO
initscr(); // Inicializar pantalla
raw(); // Configuración de entrada cruda
noecho(); // Desactivar el eco de las teclas
keypad(stdscr, TRUE);// Habilitar teclas especiales
timeout(1); // Configurar tiempo de espera en `getch`
// Crear ventana en ncurses para mostrar los mensajes
int rows, cols;
getmaxyx(stdscr, rows, cols); // Obtener dimensiones de la pantalla
WINDOW *sec_window = newwin(6, 40, (rows - 6) / 2, (cols - 40) / 2); // Ventana centrada
box(sec_window, 0, 0); // Dibujar borde alrededor de la ventana
// Instrucciones en la ventana
mvwprintw(sec_window, 1, 2, "Secuencia: Apilada");
mvwprintw(sec_window, 2, 2, "Velocidad: %dus", *velocidad);
mvwprintw(sec_window, 3, 2, "Usa las Flechas para cambiar velocidad");
mvwprintw(sec_window, 4, 2, "Presiona F2 para volver al menu");
wrefresh(sec_window); // Actualizar la ventana
while ((ch = getch()) != KEY_F(2)) { // Salir con F2
// Ajustar velocidad con flechas
if (ch == KEY_UP) {
*velocidad = (*velocidad > 100000) ? *velocidad - 50000 : *velocidad;
} else if (ch == KEY_DOWN) {
*velocidad = (*velocidad < 1000000) ? *velocidad + 50000 : *velocidad;
}
// Proceso principal: apilado de LEDs de derecha a izquierda
for (int limit = 0; limit < 8; limit++) { // Desde el LED más bajo hasta el primero
if ((ch = getch()) == KEY_F(2)) {
for (int k = 0; k < 8; k++) leds[k] = 0;
interfaz(leds); // Actualizar LEDs
return; // Salir inmediatamente
} else if (ch == KEY_UP) {
*velocidad = (*velocidad > 100000) ? *velocidad - 50000 : *velocidad;
} else if (ch == KEY_DOWN) {
*velocidad = (*velocidad < 1000000) ? *velocidad + 50000 : *velocidad;
}
// Actualizar la ventana con la nueva velocidad
wclear(sec_window); // Limpiar la ventana antes de actualizarla
box(sec_window, 0, 0); // Redibujar el borde de la ventana
mvwprintw(sec_window, 1, 2, "Secuencia: Apilada");
mvwprintw(sec_window, 2, 2, "Velocidad: %dus", *velocidad);
mvwprintw(sec_window, 3, 2, "Usa las flechas para Cambiar velocidad");
mvwprintw(sec_window, 4, 2, "Presiona F2 para Volver al menu");
wrefresh(sec_window); // Actualizar la ventana
for (int i = 7; i >= limit; i--) {
// Encender el LED actual y apagar el anterior
for (int j = 0; j < 8; j++) {
leds[j] = (j == i) || (j < limit); // LED actual o LEDs ya fijados
if ((ch = getch()) == KEY_F(2)) {
for (int k = 0; k < 8; k++) leds[k] = 0;
interfaz(leds); // Actualizar LEDs
return; // Salir inmediatamente
} else if (ch == KEY_UP) {
*velocidad = (*velocidad > 100000) ? *velocidad - 50000 : *velocidad;
} else if (ch == KEY_DOWN) {
*velocidad = (*velocidad < 1000000) ? *velocidad + 50000 : *velocidad;
}
}
interfaz(leds); // Actualizar la interfaz visual
gpioDelay(*velocidad); // Esperar la duración configurada
}
// Parpadeo del LED que se fija
for (int blink = 0; blink < 3; blink++) {
if ((ch = getch()) == KEY_F(2)) {
for (int k = 0; k < 8; k++) leds[k] = 0;
interfaz(leds); // Actualizar LEDs
return; // Salir inmediatamente
} else if (ch == KEY_UP) {
*velocidad = (*velocidad > 100000) ? *velocidad - 50000 : *velocidad;
} else if (ch == KEY_DOWN) {
*velocidad = (*velocidad < 1000000) ? *velocidad + 50000 : *velocidad;
}
// Actualizar la ventana con la nueva velocidad
wclear(sec_window); // Limpiar la ventana antes de actualizarla
box(sec_window, 0, 0); // Redibujar el borde de la ventana
mvwprintw(sec_window, 1, 2, "Secuencia: Apilada");
mvwprintw(sec_window, 2, 2, "Velocidad: %dus", *velocidad);
mvwprintw(sec_window, 3, 2, "Usa las flechas para Cambiar velocidad");
mvwprintw(sec_window, 4, 2, "Presiona F2 para Volver al menu");
wrefresh(sec_window); // Actualizar la ventana
leds[limit] = 0; // Apagar el LED
interfaz(leds);
gpioDelay(*velocidad); // Medio segundo apagado
leds[limit] = 1; // Encender el LED
interfaz(leds);
gpioDelay(*velocidad); // Medio segundo encendido
}
leds[limit] = 1; // Asegurarse de que el LED quede fijo
interfaz(leds);
}
// Actualizar la ventana con la nueva velocidad
wclear(sec_window); // Limpiar la ventana antes de actualizarla
box(sec_window, 0, 0); // Redibujar el borde de la ventana
mvwprintw(sec_window, 1, 2, "Secuencia: Apilada");
mvwprintw(sec_window, 2, 2, "Velocidad: %dus", *velocidad);
mvwprintw(sec_window, 3, 2, "Usa las flechas para Cambiar velocidad");
mvwprintw(sec_window, 4, 2, "Presiona F2 para Volver al menu");
wrefresh(sec_window); // Actualizar la ventana
}
// Apagar todos los LEDs y finalizar
for (int i = 0; i < 8; i++) leds[i] = 0;
interfaz(leds);
// Finalizar ncurses y limpiar
gpioTerminate(); // Terminar la inicialización de GPIO
refresh(); // Limpiar pantalla
endwin(); // Finalizar ncurses
}
// Secuencia "La carrera"


void secuencia_carrera(int *velocidad) {
    int leds[8] = {0};
    int primera_pos = 0;
    int segunda_pos = -1; // Segunda luz no activa hasta mitad del recorrido
    int ch;

    // Inicializar GPIO y ncurses
    gpioInitialise();
    initscr();
    raw();
    keypad(stdscr, TRUE);
    noecho();
    curs_set(0);
    timeout(0);

    // Crear ventana para la interfaz
    int rows, cols;
    getmaxyx(stdscr, rows, cols);
    WINDOW *sec_window = newwin(6, 40, (rows - 6) / 2, (cols - 40) / 2);
    box(sec_window, 0, 0);
    mvwprintw(sec_window, 0, 2, "Control de Secuencia");
    wrefresh(sec_window);

    // Inicializar LEDs en apagado
    for (int i = 0; i < 8; i++) {
        gpioSetMode(LED1 + i, PI_OUTPUT);
        gpioWrite(LED1 + i, 0);
    }

    // Mostrar configuración inicial
    while ((ch = getch()) != KEY_F(2)) { // Salir con F2
        // Ajustar velocidad con las flechas
        if (ch == KEY_UP) {
            *velocidad = (*velocidad > 100000) ? *velocidad - 50000 : *velocidad;
        } else if (ch == KEY_DOWN) {
            *velocidad = (*velocidad < 1000000) ? *velocidad + 50000 : *velocidad;
        }

        // Actualizar la interfaz
        mvwprintw(sec_window, 1, 2, "Secuencia: Carrera");
        mvwprintw(sec_window, 2, 2, "Velocidad: %dus", *velocidad);
        mvwprintw(sec_window, 3, 2, "Flechas arriba/abajo: Cambiar velocidad");
        mvwprintw(sec_window, 4, 2, "F2: Volver al menu");
        wrefresh(sec_window);

        // Actualizar la lógica de la secuencia
        if (primera_pos < 8) {
            leds[primera_pos] = 1; // Encender la luz de la primera posición
            if (segunda_pos >= 0 && segunda_pos < 8) {
                leds[segunda_pos] = 1; // Encender la luz de la segunda posición
            }

            // Enviar el estado de los LEDs al hardware
            interfaz(leds);

            // Apagar las luces previamente encendidas
            leds[primera_pos] = 0;
            if (segunda_pos >= 0 && segunda_pos < 8) {
                leds[segunda_pos] = 0;
            }

            usleep(*velocidad); // Esperar según la velocidad actual

            // Actualizar posiciones
            primera_pos++;
            if (primera_pos == 4) { // Segunda luz arranca cuando la primera está en la mitad
                segunda_pos = 0;
            }
            if (segunda_pos >= 0) {
                segunda_pos += 2; // La segunda luz avanza al doble de velocidad
            }
        } else {
            // Reiniciar posiciones para repetir la secuencia
            primera_pos = 0;
            segunda_pos = -1;
        }
    }

    // Limpiar ncurses y GPIO
    delwin(sec_window);
    endwin();
    gpioTerminate();
 for (int k = 0; k < 8; k++) leds[k] = 0;
            interfaz(leds); // Actualizar LEDs
    
}


// Secuencia "Escalera"
void secuencia_escalera(int *velocidad) {
int leds[8] = {0};
int ch;
int i, j;
gpioInitialise(); // Inicialización de GPIO
initscr(); // Inicializar interfaz ncurses
raw(); // Configuración de entrada cruda
noecho(); // Desactiva el eco de teclas
keypad(stdscr, TRUE); // Habilitar teclas especiales
timeout(1); // Configura tiempo de espera para getch()
// Crear ventana para mostrar las instrucciones
int rows, cols;
getmaxyx(stdscr, rows, cols); // Obtener dimensiones de la pantalla
WINDOW *sec_window = newwin(6, 40, (rows - 6) / 2, (cols - 40) / 2); // Ventana centrada
box(sec_window, 0, 0); // Dibujar borde alrededor de la ventana
// Instrucciones en la ventana
mvwprintw(sec_window, 1, 2, "Secuencia: Escalera");
mvwprintw(sec_window, 2, 2, "Velocidad: %dus", *velocidad);
mvwprintw(sec_window, 3, 2, "Flechas arriba/abajo: Cambiar velocidad");
mvwprintw(sec_window, 4, 2, "F2: Volver al menu");
wrefresh(sec_window); // Actualizar la ventana
while ((ch = getch()) != KEY_F(2)) { // Salir con F2
// Ajustar velocidad con las flechas
if (ch == KEY_UP) {
*velocidad = (*velocidad > 100000) ? *velocidad - 50000 : *velocidad;
} else if (ch == KEY_DOWN) {
*velocidad = (*velocidad < 1000000) ? *velocidad + 50000 : *velocidad;
}
// Inicializar LEDs apagados
for (i = 0; i < 8; i++) leds[i] = 0;
// Comenzar la secuencia de apilado (de izquierda a derecha)
for (i = 0; i < 8; i++) { // Iniciar desde el LED más a la izquierda
if ((ch = getch()) == KEY_F(2)) {
for (int k = 0; k < 8; k++) leds[k] = 0;
interfaz(leds); // Actualizar LEDs
return; // Salir inmediatamente
} else if (ch == KEY_UP) {
*velocidad = (*velocidad > 100000) ? *velocidad - 50000 : *velocidad;
} else if (ch == KEY_DOWN) {
*velocidad = (*velocidad < 1000000) ? *velocidad + 50000 : *velocidad;
}
// Actualizar la ventana con la nueva velocidad
wclear(sec_window); // Limpiar la ventana antes de actualizarla
box(sec_window, 0, 0); // Redibujar el borde de la ventana
mvwprintw(sec_window, 1, 2, "Secuencia: Escalera");
mvwprintw(sec_window, 2, 2, "Velocidad: %dus", *velocidad);
mvwprintw(sec_window, 3, 2, "Usa las flechas para Cambiar velocidad");
mvwprintw(sec_window, 4, 2, "Presiona F2 para Volver al menu");
wrefresh(sec_window); // Actualizar la ventana
// Encender los LEDs secuencialmente
for (j = 0; j <= i; j++) {
leds[j] = 1; // Encender el LED en la posición actual
}
interfaz(leds); // Actualizar la interfaz
gpioDelay(*velocidad); // Esperar un momento
// Parpadeo en la última posición fijada (de derecha a izquierda)
for (int blink = 0; blink < 3; blink++) { // Parpadeo de 3 veces
leds[i] = 1; // Encender el LED
interfaz(leds); // Actualizar la interfaz
gpioDelay(*velocidad); // Esperar medio segundo
leds[i] = 0; // Apagar el LED
interfaz(leds); // Actualizar la interfaz
gpioDelay(*velocidad); // Esperar medio segundo
if ((ch = getch()) == KEY_F(2)) {
for (int k = 0; k < 8; k++) leds[k] = 0;
interfaz(leds); // Actualizar LEDs
return; // Salir inmediatamente
} else if (ch == KEY_UP) {
*velocidad = (*velocidad > 100000) ? *velocidad - 50000 : *velocidad;
} else if (ch == KEY_DOWN) {
*velocidad = (*velocidad < 1000000) ? *velocidad + 50000 : *velocidad;
}
wclear(sec_window); // Limpiar la ventana antes de actualizarla
box(sec_window, 0, 0); // Redibujar el borde de la ventana
mvwprintw(sec_window, 1, 2, "Secuencia: Escalera");
mvwprintw(sec_window, 2, 2, "Velocidad: %dus", *velocidad);
mvwprintw(sec_window, 3, 2, "Usa las flechas para Cambiar velocidad");
mvwprintw(sec_window, 4, 2, "Presiona F2 para Volver al menu");
wrefresh(sec_window); // Actualizar la ventana
}
}
}
// Apagar todos los LEDs al salir de la secuencia
for (i = 0; i < 8; i++) leds[i] = 0;
interfaz(leds);
// Finalizar
gpioTerminate();
wrefresh(sec_window); // Actualizar la ventana
endwin(); // Cerrar la ventana ncurses
}


void secuencia_chispas(int *velocidad) {
   
        int leds[8] = {0};
    int ch;
    int i;
    srand(time(NULL)); // Inicializar el generador de números aleatorios

    gpioInitialise(); // Inicialización de GPIO
    initscr(); // Inicializar ncurses
    raw(); // Configuración de entrada cruda
    noecho(); // Desactivar eco de teclas
    keypad(stdscr, TRUE); // Habilitar teclas especiales
    timeout(1); // Establecer tiempo de espera para getch()

    // Crear ventana para instrucciones
    int rows, cols;
    getmaxyx(stdscr, rows, cols); // Obtener dimensiones de la pantalla
    WINDOW *sec_window = newwin(6, 40, (rows - 6) / 2, (cols - 40) / 2); // Ventana centrada
    box(sec_window, 0, 0); // Dibujar borde alrededor de la ventana

    // Instrucciones en la ventana
    mvwprintw(sec_window, 1, 2, "Secuencia: Chispas");
    mvwprintw(sec_window, 2, 2, "Velocidad: %dus", *velocidad);
    mvwprintw(sec_window, 3, 2, "Flechas arriba/abajo: Cambiar velocidad");
    mvwprintw(sec_window, 4, 2, "F2: Volver al menu");
    wrefresh(sec_window); // Actualizar la ventana

    while ((ch = getch()) != KEY_F(2)) { // Salir con F2
        // Ajustar velocidad con las flechas
        if (ch == KEY_UP) {
            *velocidad = (*velocidad > 100000) ? *velocidad - 50000 : *velocidad;
        } else if (ch == KEY_DOWN) {
            *velocidad = (*velocidad < 1000000) ? *velocidad + 50000 : *velocidad;
        }

        // Secuencia de chispas (encender LEDs aleatorios)
        for (i = 0; i < 8; i++) {
            // Apagar todos los LEDs
            for (int j = 0; j < 8; j++) leds[j] = 0;
            
            if ((ch = getch()) == KEY_F(2)) {
            for (int k = 0; k < 8; k++) leds[k] = 0;
            interfaz(leds); // Actualizar LEDs
            return; // Salir inmediatamente
            } else if (ch == KEY_UP) {
            *velocidad = (*velocidad > 100000) ? *velocidad - 50000 : *velocidad;
            } else if (ch == KEY_DOWN) {
            *velocidad = (*velocidad < 1000000) ? *velocidad + 50000 : *velocidad;
            }

        wclear(sec_window); 
        box(sec_window, 0, 0); 
        mvwprintw(sec_window, 1, 2, "Secuencia: Chispas");
        mvwprintw(sec_window, 2, 2, "Velocidad: %dus", *velocidad);
        mvwprintw(sec_window, 3, 2, "Flechas arriba/abajo: Cambiar velocidad");
        mvwprintw(sec_window, 4, 2, "F2: Volver al menu");
        wrefresh(sec_window); // Actualizar la ventana

            // Encender un LED aleatorio
            leds[rand() % 8] = 1;

            // Actualizar los LEDs
            interfaz(leds);
            usleep(*velocidad); // Esperar según la velocidad definida
        }

        // Limpiar la ventana antes de actualizarla
        wclear(sec_window); 
        box(sec_window, 0, 0); 
        mvwprintw(sec_window, 1, 2, "Secuencia: Chispas");
        mvwprintw(sec_window, 2, 2, "Velocidad: %dus", *velocidad);
        mvwprintw(sec_window, 3, 2, "Flechas arriba/abajo: Cambiar velocidad");
        mvwprintw(sec_window, 4, 2, "F2: Volver al menu");
        wrefresh(sec_window); // Actualizar la ventana
    }

    // Finalizar ncurses y GPIO
    endwin();
    gpioTerminate();
     for (int k = 0; k < 8; k++) leds[k] = 0;
            interfaz(leds); // Actualizar LEDs

}

void secuencia_sirena(int *velocidad) {
    int leds[8] = {0};  // Iniciar todos los LEDs apagados
    int ch;
    int tabla_sirena[16][8] = {
        {1, 0, 1, 0, 1, 0, 1, 0},  // 10101010
        {0, 1, 0, 1, 0, 1, 0, 1},  // 01010101
        {1, 1, 1, 1, 0, 0, 0, 0},  // 11110000
        {0, 0, 0, 0, 1, 1, 1, 1},  // 00001111
        {1, 1, 1, 1, 1, 1, 0, 0},  // 11111100
        {0, 0, 0, 0, 0, 0, 1, 1},  // 00000011
        {1, 0, 0, 0, 0, 0, 0, 0},  // 10000000
        {0, 0, 0, 0, 0, 0, 0, 1},  // 00000001
        {1, 1, 0, 0, 1, 1, 0, 0},  // 11001100
        {0, 0, 1, 1, 0, 0, 1, 1},  // 00110011
        {1, 0, 1, 0, 1, 0, 1, 0},  // 10101010 (Patrón alternado)
        {0, 1, 0, 1, 0, 1, 0, 1},  // 01010101 (Patrón alternado invertido)
        {1, 1, 1, 1, 0, 0, 0, 0},  // 11110000
        {0, 0, 0, 0, 1, 1, 1, 1},  // 00001111
        {1, 0, 0, 0, 0, 0, 0, 0},  // 10000000
    };

    // Inicializar ncurses
    initscr();  
    raw();     
    noecho();
    keypad(stdscr, TRUE);
    timeout(1);  // Tiempo de espera para getch()

    // Crear ventana para las instrucciones
    int rows, cols;
    getmaxyx(stdscr, rows, cols);  // Obtener dimensiones de la pantalla
    WINDOW *sec_window = newwin(6, 40, (rows - 6) / 2, (cols - 40) / 2); // Ventana centrada
    box(sec_window, 0, 0); // Dibujar borde alrededor de la ventana

    // Instrucciones en la ventana
    mvwprintw(sec_window, 1, 2, "Secuencia: Sirena de Luz");
    mvwprintw(sec_window, 2, 2, "Velocidad: %dus", *velocidad);
    mvwprintw(sec_window, 3, 2, "Flechas arriba/abajo: Cambiar velocidad");
    mvwprintw(sec_window, 4, 2, "F2: Volver al menu");
    wrefresh(sec_window);  // Actualizar la ventana

    while ((ch = getch()) != KEY_F(2)) { // Salir con F2
        // Ajustar velocidad con las flechas
        if (ch == KEY_UP) {
            *velocidad = (*velocidad > 100000) ? *velocidad - 50000 : *velocidad;
        } else if (ch == KEY_DOWN) {
            *velocidad = (*velocidad < 1000000) ? *velocidad + 50000 : *velocidad;
        }

        // Iterar a través de la tabla para mostrar la secuencia de la sirena
        for (int i = 0; i < 16; i++) {
            for (int j = 0; j < 8; j++) {

            
            if ((ch = getch()) == KEY_F(2)) {
            for (int k = 0; k < 8; k++) leds[k] = 0;
            interfaz(leds); // Actualizar LEDs
            return; // Salir inmediatamente
            } else if (ch == KEY_UP) {
            *velocidad = (*velocidad > 100000) ? *velocidad - 50000 : *velocidad;
            } else if (ch == KEY_DOWN) {
            *velocidad = (*velocidad < 1000000) ? *velocidad + 50000 : *velocidad;
            }
            wclear(sec_window);
            box(sec_window, 0, 0);  // Redibujar el borde de la ventana
            mvwprintw(sec_window, 1, 2, "Secuencia: Sirena de Luz");
            mvwprintw(sec_window, 2, 2, "Velocidad: %dus", *velocidad);
            mvwprintw(sec_window, 3, 2, "Flechas arriba/abajo: Cambiar velocidad");
            mvwprintw(sec_window, 4, 2, "F2: Volver al menu");
            wrefresh(sec_window);  // Actualizar la ventana

            leds[j] = tabla_sirena[i][j];  // Actualizar LEDs según la tabla
            }

            interfaz(leds);  // Actualizar la interfaz de LEDs

            // Limpiar y actualizar la ventana con la nueva velocidad
            wclear(sec_window);
            box(sec_window, 0, 0);  // Redibujar el borde de la ventana
            mvwprintw(sec_window, 1, 2, "Secuencia: Sirena de Luz");
            mvwprintw(sec_window, 2, 2, "Velocidad: %dus", *velocidad);
            mvwprintw(sec_window, 3, 2, "Flechas arriba/abajo: Cambiar velocidad");
            mvwprintw(sec_window, 4, 2, "F2: Volver al menu");
            wrefresh(sec_window);  // Actualizar la ventana

            usleep(*velocidad);  // Espera entre cada paso para simular la sirena

            // Si se presiona F2, terminar la secuencia
            if (ch == KEY_F(2)) {
                break;
            }
        }
    }

    // Finalizar ncurses
    endwin();
}


void secuencia_matrix(int *velocidad) {
    int leds[8] = {0};  // Iniciar todos los LEDs apagados
    int ch;
    int tabla_sirena[8][8] = {
        {1, 0, 1, 0, 1, 0, 1, 0},  // 10101010
        {0, 1, 0, 1, 0, 1, 0, 1},  // 01010101
        {1, 0, 1, 0, 1, 0, 1, 0},  // 11110000
        {1, 1, 0, 1, 1, 0, 1, 1},  // 00001111
        {0, 1, 1, 0, 1, 1, 0, 1},  // 11111100
        {0, 0, 1, 1, 0, 1, 1, 0},  // 00000011
        {0, 0, 0, 1, 1, 0, 1, 1},  // 10000000
    };

    // Inicializar ncurses
    initscr();  
    raw();     
    noecho();
    keypad(stdscr, TRUE);
    timeout(1);  // Tiempo de espera para getch()

    // Crear ventana para las instrucciones
    int rows, cols;
    getmaxyx(stdscr, rows, cols);  // Obtener dimensiones de la pantalla
    WINDOW *sec_window = newwin(6, 40, (rows - 6) / 2, (cols - 40) / 2); // Ventana centrada
    box(sec_window, 0, 0); // Dibujar borde alrededor de la ventana

    // Instrucciones en la ventana
    mvwprintw(sec_window, 1, 2, "Secuencia: Matrix");
    mvwprintw(sec_window, 2, 2, "Velocidad: %dus", *velocidad);
    mvwprintw(sec_window, 3, 2, "Flechas arriba/abajo: Cambiar velocidad");
    mvwprintw(sec_window, 4, 2, "F2: Volver al menu");
    wrefresh(sec_window);  // Actualizar la ventana

    while ((ch = getch()) != KEY_F(2)) { // Salir con F2
        // Ajustar velocidad con las flechas
        if (ch == KEY_UP) {
            *velocidad = (*velocidad > 100000) ? *velocidad - 50000 : *velocidad;
        } else if (ch == KEY_DOWN) {
            *velocidad = (*velocidad < 1000000) ? *velocidad + 50000 : *velocidad;
        }

        // Iterar a través de la tabla para mostrar la secuencia de la sirena
        for (int i = 0; i < 7; i++) {
            for (int j = 0; j < 8; j++) {

            
            if ((ch = getch()) == KEY_F(2)) {
            for (int k = 0; k < 8; k++) leds[k] = 0;
            interfaz(leds); // Actualizar LEDs
            return; // Salir inmediatamente
            } else if (ch == KEY_UP) {
            *velocidad = (*velocidad > 100000) ? *velocidad - 50000 : *velocidad;
            } else if (ch == KEY_DOWN) {
            *velocidad = (*velocidad < 1000000) ? *velocidad + 50000 : *velocidad;
            }
            wclear(sec_window);
            box(sec_window, 0, 0);  // Redibujar el borde de la ventana
            mvwprintw(sec_window, 1, 2, "Secuencia: Matrix");
            mvwprintw(sec_window, 2, 2, "Velocidad: %dus", *velocidad);
            mvwprintw(sec_window, 3, 2, "Flechas arriba/abajo: Cambiar velocidad");
            mvwprintw(sec_window, 4, 2, "F2: Volver al menu");
            wrefresh(sec_window);  // Actualizar la ventana

            leds[j] = tabla_sirena[i][j];  // Actualizar LEDs según la tabla
            }

            interfaz(leds);  // Actualizar la interfaz de LEDs

            // Limpiar y actualizar la ventana con la nueva velocidad
            wclear(sec_window);
            box(sec_window, 0, 0);  // Redibujar el borde de la ventana
            mvwprintw(sec_window, 1, 2, "Secuencia: Matrix");
            mvwprintw(sec_window, 2, 2, "Velocidad: %dus", *velocidad);
            mvwprintw(sec_window, 3, 2, "Flechas arriba/abajo: Cambiar velocidad");
            mvwprintw(sec_window, 4, 2, "F2: Volver al menu");
            wrefresh(sec_window);  // Actualizar la ventana

            usleep(*velocidad);  // Espera entre cada paso para simular la sirena

            // Si se presiona F2, terminar la secuencia
            if (ch == KEY_F(2)) {
                break;
            }
        }
    }

    // Finalizar ncurses
    endwin();
}

