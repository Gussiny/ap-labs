#include <stdio.h>
#include <stdlib.h>

int mystrlen(char *str){
	int lenght = 0;
	while(str[lenght] != '\0'){
		lenght++;
	}
    return lenght;
}

char *mystradd(char *origin, char *addition){
	int lenOrigin = mystrlen(origin);
	int lenAddition = mystrlen(addition);
	char *str = malloc(lenOrigin + lenAddition);
	int i = 0;

	while(origin[i] != '\0'){
		str[i] = origin[i];
		i++;
	}
	
	i = 0;
	while(addition[i] != '\0'){
		str[i+lenOrigin] = addition[i];
		i++;
	}

	str[lenOrigin + lenAddition] = '\0';
    return str;
}

int mystrfind(char *origin, char *substr){
	int lenOrigin = mystrlen(origin);
	int lenSubst = mystrlen(substr);
	int i = 0;
	int position = -1;
	while(i < lenOrigin){

		if(origin[i] == substr[0]){
			int j = 1;
			position = i;
			while(j < lenSubst){
				if(origin[i+j] != substr[j]){
					position = -1;
					break;
				}
				j++;
			}
			if(j == lenSubst){
				return position;
			}
		}
		i++;
	}

    return position;
}
