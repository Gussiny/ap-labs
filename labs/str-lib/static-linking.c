#include <stdio.h>

int mystrlen(char *str);
char *mystradd(char *origin, char *addition);
int mystrfind(char *origin, char *substr);

int main(int argc, char *argv[])
{
	/* code */
	if(argc < 2){
		printf("Invalid arguments\n");
	} else if(mystrfind(argv[1],"-add") >= 0){
		if(argc < 4){
			printf("Invalid arguments for add\n");
		} else{
			int lenght = mystrlen(argv[2]);
			char *newString = mystradd(argv[2],argv[3]);
			int newLength = mystrlen(newString);

			printf("Initial Lenght\t: %d \n", lenght);
			printf("New String\t: %s \n", newString);
			printf("New Lenght\t: %d \n", newLength);

		}
	} else if(mystrfind(argv[1],"-find") >= 0 ){
		if(argc < 4){
			printf("Invalid arguments for find\n");
		} else{
			int position = mystrfind(argv[2], argv[3]);
			if(position >= 0){
				printf("[\'%s\'] string was found at [%d] position\n", argv[3], position);
			} else{
				printf("The string was not found\n");
			}

		}
	}
	return 0;
}
