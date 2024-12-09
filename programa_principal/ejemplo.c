#include <wiringPi.h>
#include <stdlib.h>
#include <stdio.h>
#include <pcf8591.h>

#define Address 0x48
#define BASE 64
#define A0 BASE+0
#define A1 BASE+1
#define A2 BASE+2
#define A3 BASE+3

int main(void){
	int val;
	if (wiringPiSetup () == -1) exit (1);	
	pcf8591Setup(BASE, Address);
	while(1)
	{
		val = analogRead(A0);
		printf("A0: %dmv\n",val*3300/255);
		val = analogRead(A1);
		printf("A1: %dmv\n",val*3300/255);
		val = analogRead(A2);
		printf("A2: %dmv\n\n",val*3300/255);
		delay(1000);
	}
return 0;
}
