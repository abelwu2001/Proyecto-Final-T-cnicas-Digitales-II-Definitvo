#include <stdio.h>
#include <wiringPi.h>
#include <stdlib.h>
#include <unistd.h>

// Pines que deseas monitorear
#define GPIO_PIN1 2
#define GPIO_PIN2 3

int main() {
    // Inicializar la biblioteca WiringPi
    if (wiringPiSetupGpio() == -1) {
        printf("Error al inicializar WiringPi\n");
        return 1;
    }

    // Configurar los pines como entrada
    pinMode(GPIO_PIN1, INPUT);
    pinMode(GPIO_PIN2, INPUT);

    printf("Monitoreando GPIO 2 y GPIO 3. Presiona Ctrl+C para salir.\n");

    while (1) {
        // Leer y mostrar el estado de los pines
        int state1 = digitalRead(GPIO_PIN1);
        int state2 = digitalRead(GPIO_PIN2);

        printf("GPIO %d: %s\n", GPIO_PIN1, state1 ? "ALTO (1)" : "BAJO (0)");
        printf("GPIO %d: %s\n", GPIO_PIN2, state2 ? "ALTO (1)" : "BAJO (0)");
        usleep(500000); // Esperar 500 ms
    }

    return 0;
}

