#include "mapa.h"
#include <stdbool.h>
#include <math.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

char symbol_equipos[N_EQUIPOS] ={'A','B','C'};

int mapa_clean_casilla(tipo_mapa *mapa, int posy, int posx)
{
	mapa->casillas[posy][posx].equipo=-1;
	mapa->casillas[posy][posx].numNave=-1;
	mapa->casillas[posy][posx].simbolo=SYMB_VACIO;
	return 0;
}

tipo_casilla mapa_get_casilla(tipo_mapa *mapa, int posy, int posx)
{
	return mapa->casillas[posy][posx];
}

int mapa_get_distancia(tipo_mapa *mapa, int oriy,int orix,int targety,int targetx)
{
	int distx,disty;

	distx=abs(targetx - orix);
	disty=abs(targety - oriy);
	return (distx > disty)? distx:disty;
}

tipo_nave mapa_get_nave(tipo_mapa *mapa, int equipo, int num_nave)
{
	return mapa->info_naves[equipo][num_nave];
}

int mapa_get_num_naves(tipo_mapa *mapa, int equipo)
{
	return mapa->num_naves[equipo];
}

char mapa_get_symbol(tipo_mapa *mapa, int posy, int posx)
{
	return mapa->casillas[posy][posx].simbolo;
}

bool mapa_is_casilla_vacia(tipo_mapa *mapa, int posy, int posx)
{
	return (mapa->casillas[posy][posx].equipo < 0);
}

void mapa_restore(tipo_mapa *mapa)
{
	int i,j;

	for(j=0;j<MAPA_MAXY;j++) {
		for(i=0;i<MAPA_MAXX;i++) {
			tipo_casilla cas = mapa_get_casilla(mapa,j, i);
			if (cas.equipo < 0) {
				mapa_set_symbol(mapa,j, i, SYMB_VACIO);
			}
			else {
				mapa_set_symbol(mapa, j, i,symbol_equipos[cas.equipo]);
			}
		}
	}
}

void mapa_set_symbol(tipo_mapa *mapa, int posy, int posx, char symbol)
{
	mapa->casillas[posy][posx].simbolo=symbol;
}


int mapa_set_nave(tipo_mapa *mapa, tipo_nave nave)
{
	if (nave.equipo >= N_EQUIPOS) return -1;
	if (nave.numNave >= N_NAVES) return -1;
	mapa->info_naves[nave.equipo][nave.numNave]=nave;
	if (nave.viva) {
		mapa->casillas[nave.posy][nave.posx].equipo=nave.equipo;
		mapa->casillas[nave.posy][nave.posx].numNave=nave.numNave;
		mapa->casillas[nave.posy][nave.posx].simbolo=symbol_equipos[nave.equipo];
	}
	else {
		mapa_clean_casilla(mapa,nave.posy, nave.posx);
	}
	return 0;
}

void mapa_set_num_naves(tipo_mapa *mapa, int equipo, int numNaves)
{
	mapa->num_naves[equipo]=numNaves;
}

void mapa_send_misil(tipo_mapa *mapa, int origeny, int origenx, int targety, int targetx)
{
	int px=origenx;
	int py=origeny;
	int tx=targetx;
	int ty=targety;
	char ps = mapa_get_symbol(mapa,py, px);
	int nextx, nexty;
	char nexts;

	int run = tx - origenx;
	int rise = ty - origeny;
	float m = ((float) rise) / ((float) run);
	float b = origeny - (m * origenx);
	int inc = (origenx < tx)? 1:-1;

	for (nextx = origenx; (( origenx < tx) && (nextx <= tx)) || (( origenx > tx) && (nextx >= tx)); nextx+=inc)
	{
		// solve for y
		float y = (m * nextx) + b;

		// round to nearest int
		nexty = (y > 0.0) ? floor(y + 0.5) : ceil(y - 0.5);

		if ((nexty < 0) || (nexty >= MAPA_MAXY)) {
			continue;
		}
		nexts = mapa_get_symbol(mapa,nexty, nextx);
		mapa_set_symbol(mapa,nexty,nextx,'*');
		mapa_set_symbol(mapa,py,px,ps);
		usleep(50000);
		px = nextx;
		py= nexty;
		ps = nexts;
	}

	mapa_set_symbol(mapa,py, px,ps);
}
