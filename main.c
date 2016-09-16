#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#define FORMAT_LEN 3
#define HEAD_ELEM 4
#define HEAD_ELEM_MAX_LEN 64
#define HEAD_MAX_LEN 4096
#define MAX_PIXELS 2073600
#define PIXEL_CHAR 4

typedef struct PPMHeader {
	int header_size;
	char full_header[HEAD_MAX_LEN];
	char format[FORMAT_LEN];
	int width;
	int height;
	int max_depth;
} PPMHeader;

int convertToP6(char *input, char *output);
int convertToP3(char *input, char *output);
PPMHeader getHeaderInfo(FILE *file);

int main(int argc, char *argv[]) {
	if (argc != 4){
		fprintf(stderr, "Error: The program was not passed the correct number of arguments.\n");
		return 1;
	}

	if (!strcmp(argv[1], "3")){
		fprintf(stderr, "Debug1\n");
		convertToP3(argv[2], argv[3]);
	}
	else if (!strcmp(argv[1], "6")){
		if (convertToP6(argv[2], argv[3]))
			return 1;
	}

	else{
		fprintf(stderr, argv[1]);
		fprintf(stderr, "Error: The program was not passed a PPM format of '3' or '6'.\n");
		return 1;
	}

	return 0;
}

int convertToP6(char *input, char *output){
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
		myHeader = getHeaderInfo(file);
		printf("\n| %s |\n", myHeader.full_header);
		printf("\n| format:%s || width:%d || height:%d || max depth:%d |\n", myHeader.format, myHeader.width, myHeader.height, myHeader.max_depth);

		if (myHeader.max_depth != 255){
			fprintf(stderr, "Error: Max greyscale depth of file was not 255.\n");
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
		printf("\n\n||%s||", out_buffer);

		fclose(file);

		FILE *out_file = fopen("output1.ppm", "w");
		fwrite(out_buffer, 1, (myHeader.header_size + data_size)-1, out_file);
		fclose(out_file);

	}
	else{
		fprintf(stderr, "Error: The input file specified could not be opened or does not exist.\n");
		return 1;
	}

	return 0;
}

int convertToP3(char *input, char *output){
	FILE *file;
	if (file = fopen(input, "r")){
		char *format = malloc(FORMAT_LEN);
		fscanf(file, "%s", format);
		if (strcmp(format, "P3")){
			fprintf(stderr, "Error: The input file is already in P3 format.\n");
			return 1;
		}
		else if (!strcmp(format, "P6")){
			fprintf(stderr, "Error: The input file is of an unrecognized format.\n");
			return 1;
		}
		
		//get header info
		PPMHeader myHeader;
		myHeader = getHeaderInfo(file);
		printf("\n| %s |\n", myHeader.full_header);
		printf("\n| format:%s || width:%d || height:%d || max depth:%d |\n", myHeader.format, myHeader.width, myHeader.height, myHeader.max_depth);

		if (myHeader.max_depth != 255){
			fprintf(stderr, "Error: Max greyscale depth of file was not 255.\n");
			return 1;
		}
		
		char *data = malloc(MAX_PIXELS);
		char *data_temp = data;
		for (int i = 0; i < (myHeader.width*myHeader.height); i++){
			while (1){
				int data_char = getc(file);
				char *pixel_buffer = malloc(PIXEL_CHAR);
				char *pix_temp = pixel_buffer;
				if (data_char != -1 && (char)data_char != '\n'){
					*pix_temp = (char)data_char;
					*pix_temp++;
				}
				else {
					*pix_temp = '\0';
					*data_temp = atoi(pixel_buffer);
					*data_temp++;
					break;
				}
			}
		}

		*data_temp = '\0';
		//myHeader.full_header[1] = '3';
		char *out_buffer = myHeader.full_header;
		printf("%s", out_buffer);

		fclose(file);

		FILE *out_file = fopen("output.ppm", "ab+");
		fwrite(out_buffer, 1, sizeof(out_buffer), out_file);
		fclose(out_file);

	}
	else{
		fprintf(stderr, "Error: The input file specified could not be opened or does not exist.\n");
		return 1;
	}

	return 0;
}

PPMHeader getHeaderInfo(FILE *file){
	char header[HEAD_ELEM][HEAD_ELEM_MAX_LEN];
	char full[HEAD_MAX_LEN];
	char *full_temp = full;
	int header_size = 0;

	rewind(file);
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

	PPMHeader extracted_header;
	extracted_header.header_size = header_size;
	strcpy(extracted_header.full_header, full);
	strcpy(extracted_header.format, header[0]);
	extracted_header.width = atoi(header[1]);
	extracted_header.height = atoi(header[2]);
	extracted_header.max_depth = atoi(header[3]);

	return extracted_header;

	/*for (int i = 0; i < HEAD_ELEM; i++){
		fprintf(stderr, header[i]);
	}*/
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