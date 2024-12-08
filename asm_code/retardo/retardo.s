.global retardo

retardo:
    CMP r0, #0          @ ¿r0 == 0?
    BEQ fin             @ Si r0 es 0, salta al final
    SUB r0, r0, #1      @ r0 = r0 - 1
    B retardo           @ Vuelve a llamar a retardo

fin:
    BX lr               @ Retorna al programa en C
