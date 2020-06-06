//P3 arq 2019-2020
#ifndef _ARQO_P3_H_
#define _ARQO_P3_H_

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#if __x86_64__
	typedef double tipo;
#else
	typedef float tipo;
#endif

tipo ** generateMatrix(int);
tipo ** generateEmptyMatrix(int);
void freeMatrix(tipo **);

#endif /* _ARQO_P3_H_ */
