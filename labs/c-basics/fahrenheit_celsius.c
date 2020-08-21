#include <stdio.h>
#include <stdlib.h>


/* print Fahrenheit-Celsius table */

int main(int argc, char const *argv[])
{
    int fahr, LOWER, UPPER, STEP;

    if(argv[1] != NULL){
    	LOWER = atoi(argv[1]);
    }
    if(argv[2] != NULL && argv[3] != NULL){
    	UPPER = atoi(argv[2]);
    	STEP = atoi(argv[3]);

    	if(STEP == 0){
    		printf("INVALID STEP\n");
    	} else{
    		for (fahr = LOWER; fahr <= UPPER; fahr = fahr + STEP)
    			printf("Fahrenheit: %3d, Celcius: %6.1f\n", fahr, (5.0/9.0)*(fahr-32));
    	}

    } else{
    	printf("Fahrenheit: %3d, Celcius: %6.1f\n", LOWER, (5.0/9.0)*(LOWER-32));
    }
    
    return 0;
}
