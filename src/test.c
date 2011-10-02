/*
 * main.c
 *
 *  Created on: 07/05/2011
 *      Author: kintaro
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "descriptor.h"

int main(int argc, char** argv) {

	int i, j, k;
	char* file_name = argc == 2 ? argv[1] : "imgs/00000796.jpg";
	float *profiling;
	double **tiempos;
	double *medias = (double*) malloc (14 * sizeof(double));
	double *desviaciones = (double*) malloc (14 * sizeof(double));
	int n_ejecuciones = 11;
	double aux;

	tiempos = (double**) malloc (n_ejecuciones * sizeof(double*));

	for (i = 0; i < n_ejecuciones; i++) {
		profiling = descriptor(file_name);

		tiempos[i] = (double*) malloc (14 * sizeof(double));

		for (j = 0; j < 14; j++)
			tiempos[i][j] = profiling[j+3];
	}

	for (j = 0; j < 14; j++) {
		for (k = 2; k < n_ejecuciones ; k++) {
			for (i = 0; i < n_ejecuciones - 1; i++) {
				if (tiempos[i][j] > tiempos[i+1][j]) {
					aux = tiempos[i][j];
					tiempos[i][j] = tiempos[i+1][j];
					tiempos[i+1][j] = aux;
				}
			}
		}
	}

	for (j = 0; j < 14; j++)
		medias[j] = 0;

	for (j = 0; j<14; j++) {
		for (i = 0; i < n_ejecuciones; i++)
			medias[j] += tiempos[i][j];
		medias[j] /= n_ejecuciones;
	}

	for (j = 0; j < 14; j++)
		medias[j] = tiempos[5][j];

	for (j = 0; j<14; j++) {
		for (i = 0; i < n_ejecuciones; i++)
			desviaciones[j] += pow(tiempos[i][j] - medias[j], 2);
		desviaciones[j] /= n_ejecuciones;
		desviaciones[j] = sqrt(desviaciones[j]);
	}

	fprintf(stdout, "%d;%d;%d;", (int)profiling[0], (int)profiling[1], (int)profiling[2]);
	for (i = 0; i < 14; i++)
		fprintf(stdout, "%f;", (float)medias[i]);

	for (i = 0; i < 13; i++)
		fprintf(stdout, "%f;", (float)desviaciones[i]);
	fprintf(stdout, "%f", (float)desviaciones[13]);

	return 0;
}
