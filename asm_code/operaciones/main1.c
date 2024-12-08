#include <stdio.h>

// Declaración de la función ensambladora
extern int operaciones(int operando1, int operando2, int operacion);

int main() {
    int operando1, operando2, operacion, resultado;

    printf("Calculadora ARM (Usando ensamblador)\n");
    printf("=====================================\n");
    printf("Operaciones disponibles:\n");
    printf("0: Suma\n");
    printf("1: Resta\n");
    printf("2: AND lógico\n");
    printf("3: OR lógico\n");
    printf("\n");

    // Solicitar al usuario los operandos
    printf("Ingrese el primer operando (entero): ");
    scanf("%d", &operando1);

    printf("Ingrese el segundo operando (entero): ");
    scanf("%d", &operando2);

    // Solicitar la operación
    printf("Seleccione la operación (0-3): ");
    scanf("%d", &operacion);

    // Validar operación
    if (operacion < 0 || operacion > 3) {
        printf("Operación no válida. Por favor, seleccione un valor entre 0 y 3.\n");
        return 1;
    }

    // Llamar a la función ensambladora
    resultado = operaciones(operando1, operando2, operacion);

    // Mostrar el resultado
    printf("\nResultado: %d\n", resultado);

    return 0;
}
