/*------------------------------------------------------------------------------
= Title: ppmrw: P6 and P3 conversion
= Author: Jack Barrett (jtb274)
= Date: 16 September 2016
= Comments: Converting to P3 may cause corruption near the middle of the file
=-------------------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define FORMAT_LEN 3
#define HEAD_ELEM 4
#define EXPECTED_ARGS 4
#define HEAD_ELEM_MAX_LEN 64
#define HEAD_MAX_LEN 4096
#define MAX_PIXELS 207360
#define PIXEL_CHAR 4
#define COLOR_DEPTH 255

typedef struct PPMHeader {
	int header_size;
	char full_header[HEAD_MAX_LEN];
	char format[FORMAT_LEN];
	int width;
	int height;
	int max_depth;
} PPMHeader;

int convert_to_p6(char *input, char *output);
int convert_to_p3(char *input, char *output);
PPMHeader get_header_info(FILE *file);

int main(int argc, char *argv[]) {
	if (argc != EXPECTED_ARGS){
		fprintf(stderr, "Error: The program was not passed the correct number of arguments.\n");
		return 1;
	}

	if (!strcmp(argv[1], "3")){
		convert_to_p3(argv[2], argv[3]);
	}
	else if (!strcmp(argv[1], "6")){
		if (convert_to_p6(argv[2], argv[3]))
			return 1;
	}

	else{
		fprintf(stderr, argv[1]);
		fprintf(stderr, "Error: The program was not passed a PPM format of '3' or '6'.\n");
		return 1;
	}

	printf("\nprogram exited\n");

	return 0;
}

//This method converts a P3 file given by 'input' and converts it's contents to P6 format.
int convert_to_p6(char *input, char *output){
	FILE *file;
	if (file = fopen(input, "r")){
		char *format = malloc(FORMAT_LEN);
		fscanf(file, "%s", format);
		if (strcmp(format, "P3")){
			fprintf(stderr, "Error: The input file is already in P6 format.\n");
			return 1;
		}
		else if (!strcmp(format, "P6")){
			fprintf(stderr, "Error: The input file is of an unrecognized format.\n");
			return 1;
		}

		//get header info
		PPMHeader myHeader;
		myHeader = get_header_info(file);
		printf("\n| format:%s || width:%d || height:%d || max depth:%d |\n", myHeader.format, myHeader.width, myHeader.height, myHeader.max_depth);

		if (myHeader.max_depth != COLOR_DEPTH){
			fprintf(stderr, "Error: Max color depth of file was not 255.\n");
			return 1;
		}

		char *data = malloc(MAX_PIXELS);
		char *data_temp = data;
		int data_size = 0;
		while (1){
			char *pixel_buffer = malloc(PIXEL_CHAR);
			char *pix_temp = pixel_buffer;
			int stop = 0;
			while (1){
				int data_char = getc(file);
				if (data_char != -1 && (char)data_char != '\n'){
					*pix_temp = (char)data_char;
					*pix_temp++;
					//printf("%c\n", (char)data_char);
				}
				else {
					*data_temp = atoi(pixel_buffer);
					*data_temp++;
					data_size++;
					if (data_char == -1){
						stop = 1;
					}

					char c = atoi(pixel_buffer);
					//printf("\n%s\n", c);
					break;
				}
			}
			if (stop){
				break;
			}
			//printf("\n%s\n", pixel_buffer);
		}

		myHeader.full_header[1] = '6';
		char *out_buffer = strcat(myHeader.full_header, data);

		fclose(file);

		FILE *out_file = fopen(output, "w");
		fwrite(out_buffer, 1, (myHeader.header_size + data_size)-1, out_file);
		fclose(out_file);

	}
	else{
		fprintf(stderr, "Error: The input file specified could not be opened or does not exist.\n");
		return 1;
	}

	return 0;
}

//This method converts a P6 file given by 'input' and converts it's contents to P3 format.
// !!!!!---WARNING---!!!!!!: This method is currently causing some data corruption near the middle of the file.
int convert_to_p3(char *input, char *output){
	FILE *file;
	if (file = fopen(input, "r")){
		char *format = malloc(FORMAT_LEN);
		fscanf(file, "%s", format);
		if (strcmp(format, "P6")){
			fprintf(stderr, "Error: The input file is already in P3 format.\n");
			return 1;
		}
		else if (!strcmp(format, "P3")){
			fprintf(stderr, "Error: The input file is of an unrecognized format.\n");
			return 1;
		}

		//get header info
		PPMHeader myHeader;
		myHeader = get_header_info(file);
		printf("\n| format:%s || width:%d || height:%d || max depth:%d |\n", myHeader.format, myHeader.width, myHeader.height, myHeader.max_depth);

		//check that depth is standard
		if (myHeader.max_depth != COLOR_DEPTH){
			fprintf(stderr, "Error: Max depth of file was not 255.\n");
			return 1;
		}

		//append edited header to new file
		myHeader.full_header[1] = '3';
		FILE *out_file = fopen(output, "w");
		fwrite(myHeader.full_header, 1, myHeader.header_size, out_file);
		fclose(out_file);
		
		//load in characters one by one and convert them to int values
		char *data = malloc((myHeader.width*myHeader.height) * 3);
		char *data_temp = data;
		int data_size = 0;
		int data_char;
		while((data_char = getc(file)) != -1){
			char str[PIXEL_CHAR];
			sprintf(str, "%d", data_char);
			strcat(str, "\n");
			strcat(data_temp, str);
			data_size += 4;
		}
		*data_temp = '\n';

		//first few bytes seem like junk
		data += 5;
		
		//append converted file body into the file.
		FILE *out_file2 = fopen(output, "a");
		fwrite(data, 1, data_size, out_file2);
		fclose(out_file2);	

	}
	else{
		fprintf(stderr, "Error: The input file specified could not be opened or does not exist.\n");
		return 1;
	}

	return 0;
}


//This method will parse the header of the PPM file and return the relevant information in a struct
PPMHeader get_header_info(FILE *file){
	char header[HEAD_ELEM][HEAD_ELEM_MAX_LEN];
	char full[HEAD_MAX_LEN];
	char *full_temp = full;
	int header_size = 0;

	rewind(file);

	//load each relevant header element separated by '\n' into an array of strings
	for (int i = 0; i < HEAD_ELEM; i++){
		char *current_str = malloc(HEAD_ELEM_MAX_LEN);
		char *temp = current_str;
		while (1){
			int current_char = getc(file);
			*full_temp = (char)current_char;
			*full_temp++;
			header_size++;
			if (current_char != ' ' && current_char != '\n' && current_char != '\r' && current_char != '#'){
				*temp = current_char;
				*temp++;
			}
			else if (current_char == '#'){
				while (1){
					int comment_char = getc(file);
					*full_temp = (char)comment_char;
					*full_temp++;
					header_size++;
					if (comment_char == '\n')
						break;
				}
			}
			else {
				*temp = '\0';
				break;
			}
		}
		strcpy(header[i], current_str);
	}

	//load info from string array into struct
	PPMHeader extracted_header;
	extracted_header.header_size = header_size;
	strcpy(extracted_header.full_header, full);
	strcpy(extracted_header.format, header[0]);
	extracted_header.width = atoi(header[1]);
	extracted_header.height = atoi(header[2]);
	extracted_header.max_depth = atoi(header[3]);

	return extracted_header;
}