/* Author: Jiri Matejka */
/* Login: xmatej52 */
/* 1st project of IZP - Processing the string */

#include <stdio.h>
#include <time.h>
#include <limits.h>
#define MAX 101

/*----------------------------------------------------------------------------*/
/*----------------------------Validation functions----------------------------*/
/*----------------------------------------------------------------------------*/

/* isInputvalid function return 1 in case input string is valid
  or 0 if its not */
int isInputvalid (char *text);

/* isPalindrome function returns 1 in case input string is palindrome
  or it returns 0 if its not */
int isPalindrome (char *text);

/* isPrime function returns 1 in case input integer number is a prime
  or it returns 0 if its not */
int isPrime (int a);

/* isNumber function returns 1 in case input string is made of number characters
  or it	return 0 if its not */
int isNumber (char *text);

/* isDate function returns 1 in case input is date in format (yyyy-mm-dd)
  or it returns 0 if its not */
/* WARNING: isDate can not recognize date from number! */
int isDate (char *text);

/* isDateValid function return 1 in case input date is valide or
  it returns 0 if its not */
int isDateValid (int y, int m, int d, char *text);

/*----------------------------------------------------------------------------*/
/*-------------------------------Other functions------------------------------*/
/*----------------------------------------------------------------------------*/

/* getStringLength function returns the length of input string */
int getStringLength (char *text);

/* StringToInteger function finds out the value of input string of
  number characters */
int StringToInteger (char *text);

/* getWeekday function returns the value in interval <-1; 6>, thanks to	this we
  can find out the weekday. -1 is an error alert */
int getWeekday (char *text);

/*----------------------------------------------------------------------------*/
/*--------------------------------Main function-------------------------------*/
/*----------------------------------------------------------------------------*/

int main (int argc, char *argv[]) {
	int i = 1;

	/* In case user inputs argument(s), argc is bigger than 1 */
	if (argc > 1) {
		printf("Argument(s):");
		for(i = 1; i < argc; i++)
			printf(" \"%s\"", argv[i]);
		
		printf(" are invalid.\n\n\nAbout program\n");
		printf("Author: Matějka Jiří\n1st project - Processing of string\n\n");
		printf("Behaviour of the program\n\nProgram scan input string and devides");
		printf(" it into words\nIn case of input is number, it checks if its ");
		printf("prime prime or not and then it prints the value of a number\nIn ");
		printf("case the input is date in format YYYY-MM-DD, it prints both day ");
		printf("in week and input date\nEverythink else is taken as words and ");
		printf("program checks if its palindrome and prints input string\nError");
		printf("messages are will be written on stderr output\n");
	}

	else {
		char text[MAX];
		int a_isnumber, a_isprime, a_ispalindrome, a_isdate, value, scanf_error;
 
		/* Unless input is from file, cycle will never stop */
		while ((scanf_error = scanf("%100s", text) == 1)) {
			if (!scanf_error || !isInputvalid(text)){
				fprintf(stderr, "Invalid input\n");
				continue;
			}
			if (text[0] == '\0') {
				fprintf(stderr, "Invalid input\n");
				continue;	
			}

			a_isnumber = isNumber(text);

			/* In case its string of number characters, we convert string to number 
			  and then check if its prime or not */
			if (a_isnumber) {
			value = StringToInteger(text);
			
			if (value < 0) {
			fprintf(stderr, "number: %s (input is too long, can not use ", text);
			fprintf(stderr, "isPrime)\n");
			}

			if (value >= 0) {
				a_isprime = isPrime(value);
					
				if(a_isprime) 
					printf("number: %i (prime)\n", value);

				else
					printf("number: %i\n", value);
				}
			}

			/* In case its not string of characters, program check if the string is in
			  date format or if its just a word or palindrome */
			else {
				a_isdate = isDate(text);
					
				/* Function isDate is called here because it must be sure that input is 
				  string. Function isDate can not recognize date from number */
				if (a_isdate){
					int weekday = getWeekday(text);

					if (weekday >= 0 && weekday <= 6) {
						char *days[] = { "Sun", "Mon","Tue", "Wed","Thu", "Fri", "Sat"};
						printf("date: %s %s\n", days[weekday], text);
					}
				}
 
				else {
					a_ispalindrome = isPalindrome(text);
					
					if (a_ispalindrome)
						printf("word: %s (palindrome)\n", text);

					else
						printf("word: %s\n", text);
				}
			}
		}
	}
 
	return 0;
}

/*----------------------------------------------------------------------------*/
/*----------------------------Validation functions----------------------------*/
/*----------------------------------------------------------------------------*/

int isInputvalid (char *text) {
	for (int i = 0; text[i] != '\0'; i++)
		if (text[i] < ' ' || text[i] > '~')// other characters can do mess on output
			return 0;
	
	return 1;
}

int isPalindrome (char *text) {
	int length = getStringLength (text);

	for (int i = 0; text[i] != '\0'; i++, length--)
		if (text[i] != text[length])
			return 0;

	return 1;
}

int isPrime (int a) {
	if (a == 2)
		return 1;
	if ( a < 2 || a%2 == 0)
		return 0; 
	
	for (int i = 3; i <= a/2 + 1; i+=2)
		/* Thanks to modulo we can find out, if input number is divided by something
		or its not */
		if (a%i == 0) 
			return 0;

	return 1;
}

int isNumber (char *text) {
	for(int i=0; text[i] != '\0'; i++)
		if (text[i] < '0' || text[i] > '9')
			return 0;

	return 1;
}

int isDate (char *text) {
	int b = 0;
	
	if (getStringLength(text) != 9)
		return 0;

	if (text[4]=='-' && text[7] == '-') {
		text[4] = '0';
		text[7] = '0';
		b=1;
	}

	int a = isNumber(text);
 
	if (b) {
		text[4] = '-';
		text[7] = '-';
	}

	return (a);
}

int isDateValid (int y, int m, int d, char *text) {

	/* mktime is declared from year 1900 */
	if (y < 1900) {
		fprintf(stderr, "date: %s (wrong date format, value of year ", text);
		fprintf(stderr, "too small)\n");
		return 0;
	}

	if ((m == 4 || m == 6 || m == 9 || m == 11 ) && d > 30) {
	fprintf(stderr, "date: %s (wrong date format, value of day ", text);
	fprintf(stderr, "is too big)\n");
	return 0;
	}

	if (m > 12 || d < 1 || m < 1 || d > 31) {
		fprintf(stderr, "date: %s (wrong date format, check value of month)", text);
		fprintf(stderr, " and value of day\n");
		return 0;
	}

	/* in case day > 28 in month February when its not leap-year it returns 0 */
	if ((!((y%4 == 0) && !(y%100 == 0)) || (y%400 == 0)) && (m == 2 && d > 28)) {
		fprintf(stderr, "date: %s wrong date format, value of day too big ", text);
		fprintf(stderr, "(keep leap-year in mind)\n");
		return 0;
	}

	if (m == 2 && d > 29) {
		fprintf(stderr, "date: %s (wrong date format, value of day is too ", text);
		fprintf(stderr, "big)\n");
		return 0;
	} 
	
	return 1;
}

/*----------------------------------------------------------------------------*/
/*-------------------------------Other functions------------------------------*/
/*----------------------------------------------------------------------------*/

int getStringLength (char *text) {
	int i = 0;	
	for (; text[i] != '\0';i++);
	
	return i-1;
}

int StringToInteger (char *text) {
	int long long value = 0;
	int test = 0;
	for (int i = 0; text[i] != '\0'; i++){
		
		/* testing overflow */
		test = value * 10 + ( text[i] - '0' );
		if (test < value)
			return -1;


		/* the value of number character (8 for example) - value of
		  character 0 = number (8) 	*/
		value = value * 10 + ( text[i] - '0' );		

	}

	return(value);
}

int getWeekday (char *text) {
	int y, m, d;
	char text2[5], text3[3];

	text2[4] = '\0';
	text3[2] = '\0';

	for (int j = 0; j<3; j++ ){
		if(j == 0) {
			for (int k = 0; k < 4; k++)
				text2[k] = text[k];
		
			y = StringToInteger(text2); // now we now the value of year
		}

		if (j == 1) {
			text3[0] = text[5];
			text3[1] = text[6];
			m = StringToInteger(text3); // now we know the value of month
		}

		if (j == 2) {
			text3[0] = text[8];
			text3[1] = text[9];
			d = StringToInteger(text3); // now we know the value of day
		}
	}

	

	if (isDateValid(y, m, d, text)) {
		time_t the_time;
		struct tm *input_time;
 
		time (&the_time); // current local time and date
		input_time = localtime (&the_time);
 
		input_time->tm_year = y - 1900; // number of years since 1900
		input_time->tm_mon = m - 1; // January has value 0
		input_time->tm_mday = d; // Day in month
		input_time->tm_hour = 0;
		input_time->tm_min = 0;
		input_time->tm_sec = 0;
		input_time->tm_isdst = -1;
		mktime (input_time);
 
		if (mktime (input_time) == -1) { // mktime was not able to recognize date
			fprintf(stderr, "date: %s (Unknown date error)\n", text);
			return -1;
	}
		if (input_time->tm_wday < 0 || input_time->tm_wday > 6) { // other errors
			fprintf(stderr, "date: %s (Unknown date error)\n", text);
			return -1;
	}

		else
			return(input_time->tm_wday);
	}

	else
		return -1;
}
