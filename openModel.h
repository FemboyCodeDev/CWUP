#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>



//#ifndef _SSIZE_T_DEFINED
//typedef intptr_t ssize_t;
//#define _SSIZE_T_DEFINED
//#endif
#ifdef _WIN32
ssize_t getline(char **lineptr, size_t *n, FILE *stream) {
    size_t pos;
    int c;

    if (lineptr == NULL || stream == NULL || n == NULL) {
        return -1;
    }

    if (*lineptr == NULL) {
        *n = 128;
        if ((*lineptr = malloc(*n)) == NULL) return -1;
    }

    pos = 0;
    while ((c = fgetc(stream)) != EOF) {
        if (pos + 1 >= *n) {
            size_t new_len = *n + (*n >> 2) + 128;
            char *new_ptr = realloc(*lineptr, new_len);
            if (new_ptr == NULL) return -1;
            *lineptr = new_ptr;
            *n = new_len;
        }
        (*lineptr)[pos++] = c;
        if (c == '\n') break;
    }

    if (c == EOF && pos == 0) return -1;

    (*lineptr)[pos] = '\0';
    return (ssize_t)pos;
}
#endif




typedef struct {
	float x;
	float y;
	float z;

} Point3;

typedef struct {
	float x;
	float y;
} Point2;

typedef struct {
	float x;
	float y;
} UVCoord;

typedef struct {
        UVCoord a;
        UVCoord b;
        UVCoord c;
} UVTri2;

typedef struct {
	Point3 a;
	Point3 b;
	Point3 c;
	UVTri2 uv;

} Tri3;

typedef struct {
	int triCount;
	Tri3 *triangles;
} openModel;



openModel loadModel(char filename[]){
	FILE * fp;
	char * line = NULL;
	size_t len = 0;
	ssize_t read;
	openModel model;
	//printf(filename);
	fp = fopen(filename, "r");
	if (fp == NULL){return model;}


	int capacity = 2;
	int count = 0;
	model.triangles = malloc(capacity * sizeof(Tri3));

	bool eof = false;
	while ((read == getline(&line, &len, fp)) != -1 && !eof){
		//printf("Got line of length %zu:\n", read);
		if (strlen(line) > 2){
			if (strstr(line, "#") == NULL){
				//printf("%s",line);
				if (strstr(line, "eof") != NULL) {eof = true;} else{
					printf("%s",line);
					if (count >= capacity) {
						capacity *= 2;
						model.triangles = realloc(model.triangles, capacity * sizeof(Tri3));
					}
					float results[15];
					int num_values = openModel_parse_triangle(line,results);
					//printf("%d",num_values);
					if (num_values > 0){
						Point3 a = {results[0],results[1],results[2]};
						Point3 b = {results[3],results[4],results[5]};
						Point3 c = {results[6],results[7],results[8]};
						UVCoord uv1 = {results[9],results[10]};
						UVCoord uv2 = {results[11],results[12]};
						UVCoord uv3 = {results[13],results[14]};
						UVTri2 uv_tri = {uv1,uv2,uv3};
						Tri3 new_tri = {a,b,c,uv_tri};
						model.triangles[count] = new_tri;
						count++;
						model.triCount ++;
					}
				}
			}
		}
	}
	//printf("%d\n",count);
	model.triCount = count;
	fclose(fp);
	return model;
}




int openModel_parse_triangle(char *input, float *output) {
	char *token;
	int count = 0;
	// Get the first token (the shape type)
	token = strtok(input, "; ");

	// Check if it's a "tri"
	if (token == NULL || strcmp(token, "tri") != 0) {
        return -1; 
	}
	// Continue parsing the rest of the numbers
	// Delimiters include semicolon, comma, and space
	while ((token = strtok(NULL, ";, ")) != NULL) {
		output[count++] = atof(token);
	}
	return count;
}
