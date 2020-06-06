/**
 *
 * Descripcion: Funciones de cabecera para medicion de tiempos 
 *
 * Fichero: tiempos.h
 * Autor: Carlos Aguirre
 * Version: 1.0
 * Fecha: 11-11-2016
 *
 */



short genera_tiempos_busqueda(pfunc_busqueda metodo, pfunc_generador_claves generador, 
                                int orden, char* fichero, 
                                int num_min, int num_max, 
                                int incr, int n_veces);

short tiempo_medio_busqueda(pfunc_busqueda metodo, pfunc_generador_claves generador,
                              int orden,
                              int N, 
                              int n_veces,
                              PTIEMPO ptiempo);




