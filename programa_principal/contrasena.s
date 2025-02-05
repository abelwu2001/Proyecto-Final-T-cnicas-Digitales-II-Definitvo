.global comparar_contrasena  // Para hacer que la función sea accesible desde C

// Función que compara la entrada con la contraseña '12345'
comparar_contrasena:
    // Entradas:
    // R0 -> Apuntador a la contraseña ingresada (usuario)
    // R1 -> Apuntador a la contraseña correcta "12345"

    MOV R2, #0        // Inicializamos el contador de índice a 0

comparar_loop:
    LDRB R3, [R0, R2] // Cargar un byte de la entrada (usuario)
    LDRB R4, [R1, R2] // Cargar un byte de la contraseña correcta
    CMP R3, R4         // Comparar los bytes
    BNE contrasena_error // Si los bytes no coinciden, saltar a contrasena_error

    ADD R2, R2, #1     // Incrementar el índice
    CMP R3, #0         // Verificar si llegamos al final de la cadena (NULL terminador)
    BEQ contrasena_correcta  // Si encontramos el final (NULL), las contraseñas coinciden
    B comparar_loop     // Sino, seguir comparando

contrasena_correcta:
    MOV R0, #1         // Si las contraseñas coinciden, retornar 1
    BX LR              // Regresar a la función llamante

contrasena_error:
    MOV R0, #-1        // Si las contraseñas no coinciden, retornar -1
    BX LR              // Regresar a la función llamante
