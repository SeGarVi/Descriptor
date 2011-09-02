/*
 * main.c
 *
 *  Created on: 07/05/2011
 *      Author: kintaro
 */

#include <stdio.h>
#include <stdlib.h>

#include "descriptor.h"

int main(int argc, char** argv) {

	int i, j;
	char* file_name = argc == 2 ? argv[1] : "imgs/rxm78d00-page02_2.tif.jpg";
	float *profiling;
	double *tiempos = (double*) malloc (14 * sizeof(double));
	int n_ejecuciones = 10;

	for (j = 0; j < 14; j++)
		tiempos[j] = 0.0;

	for (i = 0; i < n_ejecuciones; i++) {
		profiling = descriptor(file_name);

		for (j = 0; j < 14; j++)
			tiempos[j] += profiling[j+3];
	}

	fprintf(stdout, "%d;%d;%d;", (int)profiling[0], (int)profiling[1], (int)profiling[2]);
	for (i = 0; i < 14; i++)
		fprintf(stdout, "%f;", (float)(tiempos[i]/n_ejecuciones));
	fprintf(stdout, "%f", (float)(tiempos[13]/n_ejecuciones));

	return 0;
}
