/*
 * main.c
 *
 *  Created on: 07/05/2011
 *      Author: kintaro
 */

#include "descriptor.h"


int main(int argc, char** argv) {

	char* file_name = argc == 2 ? argv[1] : "imgs/img1.jpg";
	float **desc;

	enable_debug();
	desc = descriptor(file_name);

	return 0;
}
