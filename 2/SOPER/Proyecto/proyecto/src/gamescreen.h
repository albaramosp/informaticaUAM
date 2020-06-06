#ifndef SRC_GAMESCREEN_H_
#define SRC_GAMESCREEN_H_

/* Inicializa el modo pantalla en el terminal. Debe hacerse antes que cualquier otra función screen */
void screen_init();

/* Fija en pantalla un símbolo en la posición fila , columna. Siendo 0,0 la esquina superior izquierda.
 * El símbolo no se mostrará hasta el próximo screen_refresh() */
void screen_addch(int row, int col, char symbol);

/* Refresca lo que muestra la pantalla. En principio, no hay que hacer refresh cada vez que se añade un
 * símbolo con screen_addch()*/
void screen_refresh();

/* Finaliza el modo pantalla. Hay que hacerlo antes de finalizar el programa */
void screen_end();

#endif /* SRC_GAMESCREEN_H_ */
