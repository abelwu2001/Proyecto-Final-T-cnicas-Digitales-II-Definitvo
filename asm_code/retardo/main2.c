#include <stdio.h>

// Declaración de la función en ensamblador
extern void retardo(unsigned long int a);

int main() {
    printf("Mensaje 1\n");
    retardo(500000);  // Llamada al retardo
    printf("Mensaje 2\n");
    retardo(500000);
    printf("Mensaje 3\n");

    return 0;
}
