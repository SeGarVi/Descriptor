/*
 * ext_cv_funcs.h
 *
 *  Created on: Aug 11, 2011
 *      Author: kintaro
 */

#ifndef EXT_CV_FUNCS_H_
#define EXT_CV_FUNCS_H_

IplImage 	 *convertir_a_grises(IplImage *);
IplImage 	 *adaptive_threshold (IplImage *src, int max_value, int c);
IplImage 	 *threshold (IplImage *src, int max_value);
IplImage 	 *suavizar(IplImage *src);
lista_puntos *encontrar_centroides(IplImage *);

#endif /* EXT_CV_FUNCS_H_ */
