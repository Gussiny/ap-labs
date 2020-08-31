#include <stdio.h>
#include <stdlib.h>
/* month_day function's prototype*/
void month_day(int year, int yearday, int *pmonth, int *pday);
char *month_name(int n);

int main(int argc, char ** argv) {
	int day, month, year, yearday;

	year = atoi(argv[1]);
	yearday = atoi(argv[2]);

	day = month = 0;

	month_day(year, yearday, &month, &day);
	printf("%s %2d, %d\n", month_name(month), day, year);
    return 0;
}

void month_day(int year, int yearday, int *pmonth, int *pday){
	int leap;

	static char daytab[2][13] = { {0,31,28,31,30,31,30,31,31,30,31,30,31} , {0,31,29,31,30,31,30,31,31,30,31,30,31} };
	leap = year%4 == 0 && year%100 != 0 || year%400 == 0;

	if( (leap == 0 && yearday > 365) || (leap == 1 && yearday > 366)) {
		printf("Invalid yearday\n");
		return;
	} 

	while( yearday > daytab[leap][*pmonth]){
		yearday -= daytab[leap][*pmonth];
		(*pmonth)++;
	}
	*pday = yearday;
}

char *month_name(int n){
	static char *name[] = {
		"Ilegal Month",
		"January", "February", "March",
		"April", "May", "June",
		"July", "August", "September",
		"October", "November", "December"
	};

	return (n < 1 || n > 12) ? name[0] : name[n];
}
