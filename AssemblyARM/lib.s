.macro _exit p1
    mov r0,\p1
    mov r7,#1
    swi 0
.endm



