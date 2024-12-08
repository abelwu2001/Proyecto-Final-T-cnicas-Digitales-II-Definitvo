.global operaciones

operaciones:
    CMP r2, #0            @ Comparar r2 con 0
    BEQ suma              @ Si r2 == 0, ir a la etiqueta suma
    CMP r2, #1            @ Comparar r2 con 1
    BEQ resta             @ Si r2 == 1, ir a la etiqueta resta
    CMP r2, #2            @ Comparar r2 con 2
    BEQ and_logico        @ Si r2 == 2, ir a la etiqueta AND lógico
    CMP r2, #3            @ Comparar r2 con 3
    BEQ or_logico         @ Si r2 == 3, ir a la etiqueta OR lógico

    MOV r0, #-1           @ Si no coincide con ninguna operación, devolver -1 en r0
    BX lr                 @ Retornar al llamador

suma:
    ADD r0, r0, r1        @ r0 = r0 + r1
    BX lr                 @ Retornar

resta:
    SUB r0, r0, r1        @ r0 = r0 - r1
    BX lr                 @ Retornar

and_logico:
    AND r0, r0, r1        @ r0 = r0 AND r1
    BX lr                 @ Retornar

or_logico:
    ORR r0, r0, r1        @ r0 = r0 OR r1
    BX lr                 @ Retornar
