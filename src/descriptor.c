/*
 * descriptor.c
 *
 *  Created on: 07/05/2011
 *      Author: kintaro
 */

#include <stdio.h>
#include <cv.h>
#include <highgui.h>
#include <time.h>
#include <math.h>

#include "descriptor.h"
#include "punto.h"
#include "lista_puntos.h"
#include "descriptor_debug.h"
#include "ext_cv_funcs.h"


/******************************
 * Variables de configuracion *
 ******************************/
static int divisiones_ancho  	= 10;
static int divisiones_alto   	= 15;
static int cantidad_cercanos 	= 7;
static int cantidad_combinacion = 5;
static int debug = 0;


static int n_centros;

/******************************
 * Imagenes para el debugging *
 ******************************/
static IplImage *src = 0;
static IplImage *image, *image2;


static int ja = 0;


/******************************
 * Funciones de configuracion *
 ******************************/
void set_divisiones_ancho(int divs) {
	divisiones_ancho = divs;
}

int get_divisiones_ancho() {
	return divisiones_ancho;
}

void set_divisiones_alto(int divs) {
	divisiones_alto = divs;
}

int get_divisiones_alto() {
	return divisiones_alto;
}

void set_cantidad_cercanos(int cantidad) {
	cantidad_cercanos = cantidad;
}

int get_cantidad_cercanos() {
	return cantidad_cercanos;
}

void set_cantidad_combinacion(int cantidad) {
	cantidad_combinacion = cantidad;
}

int get_cantidad_combinacion() {
	return cantidad_combinacion;
}

void enable_debug() {
	debug = 1;
}

void disable_debug() {
	debug = 0;
}

static int **generar_combinaciones(int mayor, int menor, int n_combinaciones) {
	int  i, j, pivote_izq, pivote_der;
	int **combinaciones;

	pivote_der = menor-1;

	combinaciones = (int **)malloc(n_combinaciones*sizeof(int *));
	combinaciones[0] = (int *)malloc(menor * sizeof(int));

	for (i = 0; i < menor; i++) {
		combinaciones[0][i] = i;
	}

	i = 1;
	for (pivote_izq = pivote_der; pivote_izq >= 0; pivote_izq--){
		combinaciones[i] = (int *)malloc(menor * sizeof(int));
		memcpy(combinaciones[i], combinaciones[0], menor * sizeof(int));

		for (j = pivote_der; j >= pivote_izq; j--) {
			combinaciones[i][j]++;
		}
		i++;

		for (j = pivote_der; j >= pivote_izq; j--) {
			combinaciones[i] = (int *)malloc(menor * sizeof(int));
			memcpy(combinaciones[i], combinaciones[i-1], menor * sizeof(int));
			combinaciones[i][j]++;
			i++;
		}
	}

	if (debug) {
		imprimir_combinaciones(combinaciones, mayor, menor, n_combinaciones);
	}

	return combinaciones;
}

static int combinacion_sin_repeticion(int mayor, int menor) {
	int factorial, numerador, limite_numerador, i;

	factorial = numerador = 1;
	limite_numerador = mayor - menor;
	for (i = mayor; i > 0; i--) {
		factorial *= i;

		if (i <= menor) {
			numerador *= i;
		}

		if (i <= limite_numerador ){
			numerador *= i;
		}
	}

	return factorial/numerador;
}

static float calcular_area(punto **triangulo) {
	int i;
	float semiperimetro, distancias[3];
	punto vector;

	semiperimetro = 0;
	for (i = 0; i < 3; i++){
		vector.x = triangulo[i] -> x - triangulo[(i+1)%3] -> x;
		vector.x = triangulo[i] -> y - triangulo[(i+1)%3] -> y;

		distancias[i]  = sqrt( pow(vector.x, 2) + pow(vector.y, 2));
		semiperimetro += distancias[i];
	}

	semiperimetro /= 2;

	return sqrt(semiperimetro*(semiperimetro - distancias[0])*
							  (semiperimetro - distancias[1])*
							  (semiperimetro - distancias[2]));
}

static float cross_ratio(int   *combinacion,
						 punto **cercanos){
	punto **triangulo;
	float  *areas;

	areas = (float *)malloc(4 * sizeof(float));

	triangulo = (punto **)malloc(3*sizeof(punto *));

	triangulo[0] = cercanos[0];
	triangulo[1] = cercanos[1];
	triangulo[2] = cercanos[2];
	areas[0] = calcular_area(triangulo);

	triangulo[1] = cercanos[3];
	triangulo[2] = cercanos[4];
	areas[1] = calcular_area(triangulo);

	triangulo[1] = cercanos[1];
	triangulo[2] = cercanos[3];
	areas[2] = calcular_area(triangulo);

	triangulo[1] = cercanos[2];
	triangulo[2] = cercanos[4];
	areas[3] = calcular_area(triangulo);

	return (areas[0] * areas[1])/(areas[3] * areas[4]);
}

static float *calcular_descriptor_centroide(nodo_punto *punt,
											int n_combinaciones,
											int **combinaciones) {
	int   i;
	float *descriptor_punto;
	punto  *centroide;
	punto **cercanos;
	nodo_cercano *iterador;

	descriptor_punto = (float *)malloc(n_combinaciones*sizeof(float));

	i = 0;
	cercanos = (punto **)malloc(cantidad_cercanos*sizeof(punto *));
	for (iterador = punt -> mas_cercanos;
			iterador!=0 ;
			iterador = iterador -> next) {
		cercanos[i++] = iterador -> p;
	}

	centroide = punt -> p;
	for (i = 0; i < n_combinaciones; i++) {
		descriptor_punto[i] = cross_ratio(combinaciones[i], cercanos);
	}

	return descriptor_punto;
}

static float **calcular_descriptor_documento(lista_puntos **centros){
	int i, j, n_combinaciones, n_celdas;
	int **combinaciones;
	float **descriptor_documento;
	nodo_punto *iterador;

	descriptor_documento = (float **)malloc(n_centros*sizeof(float*));


	n_combinaciones = combinacion_sin_repeticion(cantidad_cercanos,
						                         cantidad_combinacion);
	combinaciones = generar_combinaciones(cantidad_cercanos,
										  cantidad_combinacion,
										  n_combinaciones);


	i = 0;
	n_celdas = divisiones_ancho * divisiones_alto;
	for (j = 0; j < n_celdas; j++) {
		for (iterador = centros[j] -> first;
				iterador != 0;
				iterador = iterador -> next) {
			descriptor_documento[i++] = calcular_descriptor_centroide(iterador,
																	  n_combinaciones,
																	  combinaciones);
		}
	}

	return descriptor_documento;
}

static int *celdas_a_calcular(int celda_actual,
							  int iteracion,
							  int *n_celdas){
	//no se tienen en cuenta los extremos en el numero de celdas!!!

	int  x, y, i,  amplitud,
		 celda_x,  celda_y,
		 inicio_x, inicio_y,
		 final_x,  final_y;
	int *resultado;

	amplitud = 2*iteracion + 1;
	*n_celdas = (4*amplitud-4);
	resultado =	(int *)malloc(*n_celdas*sizeof(int));
	for (i = 0; i < *n_celdas; i++) resultado[i] = -1;

	celda_x = celda_actual%divisiones_ancho;
	celda_y = celda_actual/divisiones_ancho;

	inicio_x = celda_x - (amplitud-1)/2;
	inicio_y = celda_y - (amplitud-1)/2;

	final_x = celda_x + (amplitud-1)/2;
	final_y = celda_y + (amplitud-1)/2;

	if (inicio_x < 0) inicio_x = 0;
	if (inicio_y < 0) inicio_y = 0;

	if (final_x >= divisiones_ancho) final_x = divisiones_ancho-1;
	if (final_y >= divisiones_alto)  final_y = divisiones_alto-1;

	i = 0;
	for (y = inicio_y; y <= final_y; y++) {
		if (y == inicio_y || y == final_y) {
			for (x = inicio_x; x <= final_x; x++) {
				resultado[i++] = y * divisiones_ancho + x;
			}
		} else {
			resultado[i++] = y * divisiones_ancho + inicio_x;
			resultado[i++] = y * divisiones_ancho + final_x;
		}
	}

	return resultado;
}

static void cercanos_en_celdas(int 		  	  celda,
							   float	     *distancias,
							   float	     *angulos,
							   nodo_punto    *punt,
							   nodo_punto   **mas_cercanos,
							   lista_puntos **centros) {
	punto		 vector;
	nodo_punto  *iterador;
	float		 distancia, angulo;
	int			 mayor, i, n_nodos;

	mayor = 0;
	for (i = 0; i < cantidad_cercanos && distancias[i] >= 0; i++) {
		if (distancias[i] > distancias[mayor]) {
			mayor = i;
		}
	}

	if (i == cantidad_cercanos)
		n_nodos = cantidad_cercanos-1;
	else
		n_nodos = i;

	for (iterador = centros[celda] -> first;
			iterador != 0; iterador = iterador -> next) {
		vector.x = iterador -> p -> x - punt -> p -> x;
		vector.y = iterador -> p -> y - punt -> p -> y;

		if (punt -> p -> x != iterador -> p -> x) {
			distancia = sqrt( pow(vector.x, 2) + pow(vector.y, 2));

			angulo = acos((double)vector.x/(double)distancia);
			if (vector.y < 0) {
				angulo = 2 * M_PI - angulo;
			}

			if (distancia < distancias[mayor]) {
				distancias[mayor] = distancia;
				angulos[mayor] = angulo;
				mas_cercanos[mayor] = iterador;

				if (n_nodos < cantidad_cercanos) {
					n_nodos++;
				}

				for (i = 0; i < cantidad_cercanos || distancias[i] < 0; i++) {
					if (distancias[i] > distancias[mayor]) {
						mayor = i;
					}
				}
			}
		}
	}
}

static void buscar_mas_cercanos(int 		  celda,
							    int		   	  ancho,
							    int		      alto,
							    nodo_punto    *punt,
							    lista_puntos **centros,
							    IplImage 	  *src) {

	nodo_punto  **mas_cercanos;
	float		*distancias, *angulos;
	int 		 i, j, k,  n_nodos,
				 mayor,    celda_actual,
				 n_celdas, max_distancia;
	int 		*celdas_donde_buscar;

	n_nodos = 0;
	mayor  = 0;
	max_distancia = ancho*alto;

	distancias = (float *)malloc(cantidad_cercanos*sizeof(float));
	for (i = 0; i < cantidad_cercanos; i++) {
		distancias[i] = max_distancia;
	}

	angulos = (float *)malloc(cantidad_cercanos*sizeof(float));

	mas_cercanos = (nodo_punto **)malloc(cantidad_cercanos*sizeof(nodo_punto *));

	/*
	 * Buscamos en la misma celda
	 */
	celda_actual = celda;
	cercanos_en_celdas(celda_actual, distancias,   angulos,
					   punt,         mas_cercanos, centros);
	for (i = 0; i < cantidad_cercanos && distancias[i] < max_distancia; i++);

	if (i == cantidad_cercanos)
		n_nodos = cantidad_cercanos-1;
	else
		n_nodos = i;

	/*
	 * Buscamos en las celdas adyacentes
	 */
	celdas_donde_buscar = celdas_a_calcular(celda, 1, &n_celdas);

	for (k = 0; k<n_celdas && celdas_donde_buscar[k]!=-1; k++) {
		celda_actual = celdas_donde_buscar[k];

		cercanos_en_celdas(celda_actual, distancias,   angulos,
						   punt,         mas_cercanos, centros);
		for (i = 0; i < cantidad_cercanos && distancias[i] < max_distancia; i++);
		n_nodos = i;
	}

	for (j = 2; n_nodos < cantidad_cercanos; j++) {					//Poner un límite (y si solo hay un punto?)
		celdas_donde_buscar = celdas_a_calcular(celda, j, &n_celdas);

		for (k = 0; k<n_celdas && celdas_donde_buscar[k]!=-1; k++) {
			celda_actual = celdas_donde_buscar[k];

			cercanos_en_celdas(celda_actual, distancias,   angulos,
							   punt,         mas_cercanos, centros);
			for (i = 0; i < cantidad_cercanos && distancias[i] < max_distancia; i++);
			n_nodos = i;
		}
	}

	for (i = 0; i < n_nodos; i++) {
		add_punto_cercano(punt, mas_cercanos[i] -> p, angulos[i]);
	}

	if (celda==22 && !ja && debug) {
		dibujar_cercanos(punt, src);
		ja = 1;
	}
}

static lista_puntos **clasificar_centros(lista_puntos *centros,
									    int		     ancho,
									    int		     alto,
									    IplImage 	*src) {

	lista_puntos **clasificados;
	nodo_punto 	 *nodo;
	int 		  n_celdas, i, x, y;
	int 		 *limites_x, *limites_y;


	n_celdas = divisiones_ancho * divisiones_alto;

	clasificados = (lista_puntos **)malloc(n_celdas * sizeof(lista_puntos *));

	for (i=0; i < n_celdas; i++) {
		clasificados[i] = new_lista_puntos();
	}

	limites_x = (int *)malloc(divisiones_ancho*sizeof(int));
	for (i = 0; i < divisiones_ancho; i++) {
		limites_x[i] = ancho/divisiones_ancho * (i+1);
	}

	limites_y = (int *)malloc(divisiones_alto*sizeof(int));
	for (i = 0; i < divisiones_alto; i++) {	//cvShowImage("Erosion&Dilation window",src);
		limites_y[i] = alto/divisiones_alto * (i+1);
	}

	for (nodo = centros -> first; nodo != 0; nodo = nodo -> next) {
		for (x = 0; x < divisiones_ancho && !(nodo -> p -> x < limites_x[x]); x++);
		for (y = 0; y < divisiones_alto  && !(nodo -> p -> y < limites_y[y]); y++);

		add_punto(clasificados[y * divisiones_ancho + x], nodo -> p);
	}

	return clasificados;
}

static lista_puntos **encontrar_centros(IplImage **src) {
	CvMemStorage* storage = cvCreateMemStorage(0);
	CvSeq* contour = 0;
	int num_contornos, j;
	nodo_punto *nodo;
	//int **centros;
	lista_puntos *centros, *centros2;
	lista_puntos **centros_clasificados;
	int nCeldas;

	centros = encontrar_centroides(*src);

	centros_clasificados = clasificar_centros(centros,
											  (*src) -> width,
											  (*src) -> height,
											  *src);

	nCeldas = divisiones_ancho * divisiones_alto;
	for (j = 0; j < nCeldas; j++) {
		for (nodo = centros_clasificados[j] -> first;
				nodo != 0;
				nodo = nodo -> next) {
			buscar_mas_cercanos(j,
							    (*src) -> width,
							    (*src) -> height,
							    nodo,
							    centros_clasificados,
							    *src);
		}
	}

	if (debug) {
		dibujar_centros(centros, *src);
		dibujar_rejilla(divisiones_ancho, divisiones_alto, *src);
	}

	return centros_clasificados;
}

//callback function for slider , implements erosion
static IplImage *pre_procesar(IplImage *original,
							  IplImage *a_modificar) {

	a_modificar = convertir_a_grises(original);

	if (a_modificar != 0) {
		cvAdaptiveThreshold(a_modificar					 , a_modificar	   , 255,
							CV_ADAPTIVE_THRESH_GAUSSIAN_C, CV_THRESH_BINARY, 9, 9);
		cvSmooth(a_modificar, a_modificar, CV_GAUSSIAN, 15, 15, 9, 0);
		//cvAdaptiveThreshold(aModificar, aModificar, 128, CV_ADAPTIVE_THRESH_GAUSSIAN_C, CV_THRESH_BINARY, 9, 9);
		//cvAdaptiveThreshold(a_modificar					 , a_modificar	   , 255,
		//						CV_ADAPTIVE_THRESH_GAUSSIAN_C, CV_THRESH_BINARY, 9, 9);
		cvThreshold(a_modificar, a_modificar, 210, 255, CV_THRESH_BINARY);
	} else {
		fprintf(stderr, "La imagen de origen no tiene tres canales de color");
		exit(-1);
	}

	return a_modificar;
}

float **descriptor(char* file_name) {
	float 		 **descriptor;
	lista_puntos **centroides;
	clock_t 	   fin, ini;

	if( (src = cvLoadImage(file_name,1)) == 0 ) return ((float **)0);
	image = cvCreateImage(cvSize( src -> width, src -> height ),
						  IPL_DEPTH_8U, 1 );


	//create windows for output images
	cvNamedWindow("Original",1);
	cvNamedWindow("Procesada",1);

	cvShowImage("Original",src);

	ini = clock() / (CLOCKS_PER_SEC / 1000);
	image2 = pre_procesar(src, image);
	centroides = encontrar_centros(&image2);
	descriptor = calcular_descriptor_documento(centroides);
	fin = clock() / (CLOCKS_PER_SEC / 1000);

	fprintf(stdout, "Tiempo = %d\n", fin-ini);

	cvShowImage("Procesada",image2);

	cvWaitKey(0);

	//releases header an dimage data
	cvReleaseImage(&src);
	cvReleaseImage(&image);

	//destroys windows cvDestroyWindow("Opening&Closing window");
	cvDestroyWindow("Original");
	cvDestroyWindow("Procesada");

	return descriptor;
}
