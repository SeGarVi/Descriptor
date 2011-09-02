/*
 * descriptor.h
 *
 *  Created on: 07/05/2011
 *      Author: kintaro
 */

#ifndef DESCRIPTOR_H_
#define DESCRIPTOR_H_

void set_divisiones_ancho(int divs);
int  get_divisiones_ancho();

void set_divisiones_alto(int divs);
int  get_divisiones_alto();

void set_cantidad_cercanos(int cantidad);
int  get_cantidad_cercanos();

void set_cantidad_combinacion(int cantidad);
int get_cantidad_combinacion();

void enable_debug();
void disable_debug();

float *descriptor(char* file_name);

#endif /* DESCRIPTOR_H_ */
