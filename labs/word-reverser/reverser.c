#include <stdio.h>
#include <string.h>

#define IN 	1
#define OUT 0

int main(){
    // Place your magic here
    int newLine, state, newWord, newChar;
    char c;
    char words[256]="";
    state = OUT;
    newLine = newWord = newChar = 0;
    while((c = getchar()) != EOF){
    	
    	if(c == '\n'){
    		newLine++;
    		for(int x = newChar; x >= 0; x--){
    			printf("%c", words[x]);
    		}
    		printf("\n");
    		memset(words, 0, sizeof words);
    	} else{
    		words[newChar] = c;
    		newChar++;
    	}

    }
    return 0;
}
