#include <stdio.h>
#include <stdlib.h>
#include <wiringPi.h>
#include <pcf8591.h>

#define Address 0x48
#define BASE 64
#define A0 BASE+0
#define A1 BASE+1
#define A2 BASE+2
#define A3 BASE+3

void adcCrudo(int inicializar, int *potenciometro, int *fotocelula, int *thermistor){
    if(inicializar) {
        pcf8591Setup(BASE, Address);
        printf("Inicialización completa.\n");
    }

    *potenciometro = analogRead(A0);
    *fotocelula = analogRead(A1);
    *thermistor = analogRead(A2);

    printf("Lecturas: Potenciómetro = %d, Fotocélula = %d, Termistor = %d\n", *potenciometro, *fotocelula, *thermistor);
}

int main(void) {
    wiringPiSetup();
    int potenciometro, fotocelula, thermistor;

    adcCrudo(1, &potenciometro, &fotocelula, &thermistor);

    while(1) {
        adcCrudo(0, &potenciometro, &fotocelula, &thermistor);
        delay(1000);
    }
    return 0;
}

