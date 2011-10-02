/*
 * debug.c
 *
 *  Created on: 09/05/2011
 *      Author: kintaro
 */

#include <stdlib.h>
#include <stdio.h>

#include "descriptor_debug.h"



void dibujar_centros(lista_puntos *centros, IplImage *src) {
	nodo_punto *nodo;

	for (nodo = centros -> first; nodo != 0; nodo = nodo -> next) {
		cvCircle(src, cvPoint(nodo -> p ->x , nodo -> p -> y), 1,
							  CV_RGB(255, 255, 255), 0, 1, 0);
		cvCircle(src, cvPoint(nodo -> p ->x , nodo -> p -> y), 2,
									  CV_RGB(0, 0, 0), 0, 1, 0);
	}
}

void dibujar_rejilla(int divisiones_ancho, int divisiones_alto, IplImage *src) {
	int i, ancho, alto;

	ancho = src -> width;
	alto  = src -> height;

	for (i = 0; i < divisiones_ancho; i++) {
		cvLine(src, cvPoint(ancho/divisiones_ancho * (i+1), 0),
				cvPoint(ancho/divisiones_ancho * (i+1), src->height),
				CV_RGB(0, 0, 0), 0, 1, 0);
	}

	for (i = 0; i < divisiones_alto; i++) {
		cvLine(src, cvPoint(0, alto/divisiones_alto * (i+1)),
			   cvPoint(src->width, alto/divisiones_alto * (i+1)),
			   CV_RGB(0, 0, 0), 0, 1, 0);
	}
}

void dibujar_cercanos(nodo_punto *punt, IplImage *src) {

	int i;
	nodo_cercano *iter_cercano;

	fprintf(stdout, "Puntos más cercanos a %d - %d\n",
							punt -> p ->x,
							punt -> p ->y);
	i = 0;
	for (iter_cercano = punt -> mas_cercanos;
			iter_cercano != 0;
			iter_cercano = iter_cercano -> next){
		cvLine(src, cvPoint(punt -> p ->x, punt -> p ->y),
					cvPoint(iter_cercano -> p ->x, iter_cercano -> p ->y),
						CV_RGB(255, 255, 255), 0, 1, 0);
		cvLine(src, cvPoint(punt -> p ->x+1, punt -> p ->y),
					cvPoint(iter_cercano -> p ->x+1, iter_cercano -> p ->y),
						CV_RGB(0, 0, 0), 0, 1, 0);
		cvLine(src, cvPoint(punt -> p ->x-1, punt -> p ->y),
					cvPoint(iter_cercano -> p ->x-1, iter_cercano -> p ->y),
						CV_RGB(0, 0, 0), 0, 1, 0);
		i++;
		fprintf(stdout, "Punto %d - %d -- angulo %f\n",
				iter_cercano -> p ->x,
				iter_cercano -> p ->y,
				iter_cercano -> angulo * 180 / M_PI);
	}
	fprintf(stdout, "Encontrados %d nodos\n\n", i);

}

void imprimir_combinaciones(int **combinaciones, int mayor,
							int   menor, 		 int n_combinaciones) {
	int i, j;

	fprintf(stderr,
			"Combinaciones de %d elementos sobre un conjunto de %d elementos.\n",
			menor, mayor);
	fprintf(stderr,	"Hay %d combinaciones, que son las siguientes:\n\n",
			n_combinaciones);

	for (i = 0; i < n_combinaciones; i++) {
		fprintf(stderr, "{");

		for(j = 0; j < menor; j++) {
			fprintf(stderr, "%d", combinaciones[i][j]);

			if (j < menor - 1) {
				fprintf(stderr, ", ");
			}
		}

		fprintf(stderr, "}");
		if (i == n_combinaciones - 1 ){
			fprintf(stderr, "\n");
		} else {
			fprintf(stderr, ",");
		}

		if (i % 4 == 3) {
			fprintf(stderr, "\n");
		} else {
			fprintf(stderr, " ");
		}
	}
}
