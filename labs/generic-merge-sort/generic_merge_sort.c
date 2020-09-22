#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int numcmp(char *n1, char *n2);
void merge(char *arr[], int begin, int middle, int end, int (*comp)(void *, void *));
void mergeSort(char* arr[], int begin, int end, int isNumeric);

int main(int argc, char const *argv[])
{
	FILE *file;
	int isNumeric = 0;
	char* data[1000];

	if(argc == 2) {
		file = fopen(argv[1], "r");
	} else if (argc == 3 && strcmp(argv[1], "-n") == 0) {
		file = fopen(argv[2], "r");
		isNumeric = 1;
	} else {
		printf("Missing parameters\n");
		return 0;
	}

	char* line = NULL;
	size_t len = 0;
	ssize_t read;

	if(file == NULL) {
		printf("Error while openning the file\n");
		return 0;
	}

	int numLines = 0;
	while((read = getline(&line, &len, file)) != -1){
		data[numLines] = malloc(sizeof(char)*len);
		sprintf(data[numLines], "%s", line);
		data[numLines][strlen(line)-1] = 0;
		numLines++;
	}

	fclose(file);
	if(line){
		free(line);
	}

	mergeSort(data, 0, numLines - 1, isNumeric);

	for(int i = 0; i < numLines; i++){
		printf("%s\n", data[i]);
	}

	return 0;
}

int numcmp(char *n1, char *n2){
	double val1, val2;
	val1 = atof(n1);
	val2 = atof(n2);

	if(val1 < val2){
		return -1;
	} else if (val1 > val2){
		return 1;
	}

	return 0;
}

void merge(char *arr[], int begin, int middle, int end, int (*comp)(void*, void*)){
	int i, j, k;
	int left_size = middle - begin + 1;
	int right_size = end - middle;

	char* left[left_size];
	char* right[right_size];

	for (i = 0; i < left_size; i++){
		left[i] = arr[begin + i];
	}

	for (j = 0; j < right_size; j++){
		right[j] = arr[middle + j + 1];
	}

	i = j = 0;
	k = begin;

	while(i < left_size && j < right_size){
		if((*comp)(left[i], right[j]) <= 0){
			arr[k] = left[i];
			i++;
		} else{
			arr[k] = right[j];
			j++;
		}
		k++;
	}


	while(i < left_size){
		arr[k] = left[i];
		i++;
		k++;
	}

	while(j < right_size){
		arr[k] = right[j];
		j++;
		k++;
	}

}

void mergeSort(char* arr[], int begin, int end, int isNumeric){
	if(begin < end){
		int middle = begin + (end - begin) / 2;
		mergeSort(arr, begin, middle, isNumeric);
		mergeSort(arr, middle + 1, end, isNumeric);
		merge(arr, begin, middle, end, (int (*)(void*, void*)) (isNumeric ? numcmp : strcmp));
	}
}

