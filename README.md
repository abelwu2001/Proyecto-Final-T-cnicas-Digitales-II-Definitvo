# Proyecto Final Digitales II

**Versión 1.2.1**

- Esta hecho el ADC completo 
- Intentos de hacer Puerto Serie con un Uart-usb Ch340g 8n1
- Chill de cojones

## SE DEBEN CUMPLIR LAS SIGUIENTES COSAS:

Este proyecto consiste en integrar dichos ejercicios de la siguiente manera:

- Realice un programa a fin de que el usuario pueda seleccionar desde un menú, una de ocho secuencias de luces posibles. Cuatro de ellas serán comunes para todos los proyectos y son: "El auto fantástico", "El choque", "La apilada" y "La carrera". Las otras cuatro serán propias de cada grupo y se deberán implementar dos de ellas con algoritmo y las dos restantes por medio de la técnica de tablas de datos. 
  - ✅ (Realizado) 

- Implemente el control de acceso a este menú mediante password. 
  - ✅ (Realizado) 

- Cada vez que el usuario seleccione una secuencia el programa deberá cambiar la pantalla para indicar cuál secuencia está ejecutándose y cómo hacer para salir de la misma. Al optar por abandonar la actual, el programa deberá regresar al menú principal inmediatamente sin completar la secuencia que se está desarrollando y apagando todas las luces. 
  - ✅ (Realizado) 

- Permita la posibilidad de controlar la velocidad de cada secuencia. Presionando la flecha hacia arriba se incrementará la velocidad y presionando la flecha hacia abajo se reducirá. Introduzca el sensado de las teclas oprimidas en el lugar apropiado de su programa a fin de percibir la reacción del sistema en forma inmediata, independiente de la velocidad actual. La velocidad ajustada en cada secuencia deberá conservarse entre llamadas a diferentes secuencias. 
  - ✅ (Realizado) 

- El valor inicial correspondiente a la velocidad de las secuencias deberá ingresarse mediante la lectura del estado de los potenciómetros que están conectados a las entradas analógicas del conversor A/D. 
  - ✅ (Realizado) 

- Generar una opción en el programa que permita establecer dos modos de trabajo: local y remoto. En modo local las secuencias de luces se ejecutarán en los leds que se encuentran en el hardware adicionado a la placa Raspberry donde se ejecuta el programa. Existen dos opciones para el modo remoto dependiendo de la disponibilidad de hardware de cada grupo, hacer una de las dos opciones siguientes:
  

1. En modo remoto las secuencias se ejecutarán sobre el hardware adicional colocado en otra Raspberry y conectada a la que ejecuta el programa mediante un cable serie RS-232. Se podrá usar el mismo programa para implementar esta opción en las dos Raspberry o realizar uno principal y otro secundario. 
  - ⚠️ (En proceso) 

2. En modo remoto la selección desde el menú de la secuencia de luces a ejecutar será similar al modo local, pero las secuencias se ejecutarán en el hardware remoto, enviando los datos de control a través del puerto serie.
  - No se va hacer este X

- Como opción genere una sección destinada a establecer las velocidades iniciales de las secuencias realizando el ajuste de los potenciómetros.
  - ✅ (Realizado) 

- Escriba una función externa en código ensamblador de ARM la cual será llamada desde el programa principal. Esta función deberá realizar algún proceso simple dentro del programa (switch case, loop, etc). El estudiante decidirá el proceso a resolver por la función y el lugar donde se usará.
  - ✅ (Realizado) 

