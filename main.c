#include <stdio.h>
#include <stdlib.h>
#include "3dmath.h"

int main(int argc, char *argv[]) {
	if (argc != 3){
		fprintf(stderr, "The program was not passed the correct number of arguments.");
		return 1;
	}
	if (argv[1] == "3"){
		if (!convertToP3(argv[2]))
			return 1;
	}
	else if (argv[1] == "6"){
		if (!convertToP6(argv[2]))
			return 1;
	}
	else{
		fprintf(stderr, "The program was not passed a PPM format of '3' or '6'.");
		return 1;
	}

	return 0;
}

int convertToP6(char *input){
	FILE *file;
	if (file = fopen(input, "r")){
		char* format = fread(format, 2, 1, file);
		if (format == "P6"){
			fprintf(stderr, "The input file is already in P6 format.");
			return 1;
		}
		else if (format != "P3"){
			fprintf(stderr, "The input file is of an unrecognized format.");
			return 1;
		}

		//do actual conversion here
	}
	else{
		fprintf(stderr, "The input file specified could not be opened or does not exist.");
		return 1;
	}

	return 0;
}

int convertToP3(char *input){
	FILE *file;
	if (file = fopen(input, "r")){
		char* format = fread(format, 2, 1, file);
		if (format == "P3"){
			fprintf(stderr, "The input file is already in P3 format.");
			return 1;
		}
		else if (format != "P6"){
			fprintf(stderr, "The input file is of an unrecognized format.");
			return 1;
		}
		
		//do actual conversion here

	}
	else{
		fprintf(stderr, "The input file specified could not be opened or does not exist.");
		return 1;
	}
	
	return 0;
}

/*
FILE *fh = fopen("filename", 'w');
//fread, fwrite
int c = fgetc(fh);
if (c != 'P') {
fprintf(stderr, "Error: This is not a PPM file. \n");
}
unsigned char gray = 30;
fprintf(fh, "P6\n");
fprintf(fh, "%d ", gray);
*/