all: MATRIZ.EXE

MATRIZ.EXE: MATRIZ.OBJ
        tlink /v MATRIZ
MATRIZ.OBJ: MATRIZ.ASM
        tasm /zi MATRIZ.ASM
