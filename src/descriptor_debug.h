/*
 * debug.h
 *
 *  Created on: 09/05/2011
 *      Author: kintaro
 */

#ifndef DESCRIPTOR_DEBUG_H_
#define DESCRIPTOR_DEBUG_H_

#include <cv.h>

#include "lista_puntos.h"

void dibujar_centros (lista_puntos *centros, IplImage *src);
void dibujar_rejilla (int divisiones_ancho, int divisiones_alto, IplImage *src);
void dibujar_cercanos (nodo_punto *punt, IplImage *src);
void imprimir_combinaciones(int **combinaciones, int mayor,
							int   menor, 		 int n_combinaciones);

#endif /* DEBUG_H_ */
