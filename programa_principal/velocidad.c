 #include <wiringPi.h>
 #include <wiringPiI2C.h>
 #define ADDRESS 0x48
 #define BASE 64
 #define A0 BASE +0
 #define A1 BASE +1
 #define A2 BASE +2
 #define A3 BASE +3
 #define N_LED
 #include <stdio.h>


 int main ( void )
 {
 int i, y, x, z, c;
 wiringPiSetup ();
 pcf8591Setup (BASE , ADDRESS );
 while (1){
 analogRead (A0);
 i= analogRead (A0);
 delay (100) ;
 analogRead (A1);
 y= analogRead (A1);
 delay (100) ;
 analogRead (A2);
 x= analogRead (A2);
 delay (100) ;
analogRead (A3);
 z= analogRead (A3);
 printf (" %d:El valor de la lectura es %.2f V %d %d %d\n",c, (i*(3.3/255) ), y, x, z);
 c++;
delay (100) ;
}
 return 0;
 }
