#ifndef REMOTO_H
#define REMOTO_H
extern int i2cHandle;
// Configura el UART para la comunicación serie
int configurar_uart();

// Modo esclavo: recibe comandos y ejecuta las secuencias
void modo_esclavo();

// Modo maestro: selecciona y envía secuencias al esclavo
void modo_maestro();

void inicializar_adc1();

int leer_adc1();

#endif // REMOTO_H

