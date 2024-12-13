#ifndef REMOTO_H
#define REMOTO_H

// Configura el UART para la comunicación serie
int configurar_uart();

// Modo esclavo: recibe comandos y ejecuta las secuencias
void modo_esclavo();

// Modo maestro: selecciona y envía secuencias al esclavo
void modo_maestro();

#endif // REMOTO_H

