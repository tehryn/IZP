/* Author: Jiri Matejka */
/* Login: xmatej52 */
/* 2nd project of IZP - iterative calculations */

#include <stdio.h>
#include <math.h>
#include <limits.h>
#include <float.h>
#include <string.h>
#include <stdlib.h>

/*----------------------------Validation functions----------------------------*/

/* function isInputvalid return 1 in case arguments are valid otherwise
  it returns 0 */
int isInputvalid (int argc, char **argv);

/* function isInputvalid_log supports function isInputvalid */
int isInputvalid_log (char **argv);

/* function isInputvalid_iter supports function isInputvalid */
int isInputvalid_iter (char **argv);

/* function isNumber return 1 in case input string is number otherwise
  it returns 0 */
int isNumber(char *text);

/* function isUnsignedInteger return 1 in case input string is unsigned integer 
  otherwise it returns 0 */
int isUnsignedInteger (char *text);

/*----------------------------------------------------------------------------*/
/*-----------------------------logarithm functions----------------------------*/

/* function taylor_log calculate natural logarithm */
double taylor_log(double x, unsigned int n);

/* support function for taylor_log_bis */
unsigned int taylor_log_iter(double x,double z,double eps,unsigned int iters);

/* function taylor_log_bis seeks the number if needed iterations for 
  taylor_log function*/
unsigned int taylor_log_bis(double x,double eps);

/* function cfrac_log_bis seeks the number if needed iterations for 
  cfrac_log function */
unsigned int cfrac_log_iter(double x, double eps);

/* function cfrac_log calculate natural logarithm*/
double cfrac_log(double x, unsigned int n);

/*----------------------------------------------------------------------------*/
/*-------------------------------Output functions-----------------------------*/

/* output for valid arguments "--log x n". Returns -1 when failed */
int output_log(char **argv);

/* output for valid arguments "--iter MIN MAX EPS". Returns -1 when failed */
int output_iter(char **argv);

/* output for invalid arguments */
void InvalidInput ();

/* in case unexpected values */
void alternativeOutput_log (char **argv);

/* in case unexpected values */
void alternativeOutput_iter (char **argv);

/* print info about program */
void help();
/*----------------------------------------------------------------------------*/
/*-------------------------------Other functions------------------------------*/

/* function needed to count power for, in this project, only for argument eps */
double makePower (double a, long long int b);

/* function xabs return absolute value of x */
double xabs (double x);
/*----------------------------------------------------------------------------*/
/*--------------------------------Main function-------------------------------*/
int main(int argc, char *argv[]) {
	if (!strcmp(argv[1], "--help")){
		help();
		return 0;
	}
	if (isInputvalid(argc, argv)) {
		if (!strcmp(argv[1], "--log"))
			if (output_log(argv) != -1) 
				return 0;
		if (!strcmp(argv[1], "--iter"))
			if (output_iter(argv) != -1) 
				return 0;		
	}

	else {
		InvalidInput();
		return -1;
	}
	
	fprintf(stderr, "Unknown error\n");
	return -1;
}
/*----------------------------------------------------------------------------*/
/*----------------------------Validation functions----------------------------*/
int isInputvalid (int argc, char **argv) {
	
	/* validation of argument "--log" */
	if (argc == 4 && !strcmp(argv[1], "--log") && isInputvalid_log(argv))
		return 1;		

	/* validation of argument "--iter" */
	if (argc == 5 && !strcmp(argv[1], "--iter") && isInputvalid_iter (argv))	
		return 1;

	return 0;
}

int isInputvalid_log (char **argv) {
	if (isNumber(argv[2]) && isUnsignedInteger(argv[3])) {
	                         //argv[3] must be unsigned integer

		double b = strtod(argv[3], '\0');
		if (b < 1.0 || b > UINT_MAX)
			return 0;

		return 1;		
	}

	return 0;
}

int isInputvalid_iter (char **argv) {
	if (isNumber(argv[2]) && isNumber(argv[3]) && isNumber(argv[4])) {
		double eps = strtod(argv[4], '\0'), a = strtod(argv[2], '\0');
		double b = strtod(argv[3], '\0');

	if (a > b || eps < 1e-12 || isinf(eps))
		return 0;
		
		return 1;
	}

	return 0;
}

int isNumber(char *text) {
	/* 1 means, that dot or power has not been used yet */
	int dot_check = 1, power_check = 1;
	int i = 0;

	if (!strcmp(text, "inf"))
		return 1;
	
	if ((text[0] == '+' || text[0] == '-'))
		i++;

	for (; text[i] != '\0'; i++) {

		if (text[i] == '.' && (dot_check)) { 
			dot_check = 0;
			i++;
		}

		/* we have to set dot as unused after power and check sing again */
		if ((text[i] == 'e' || text[i] == 'E') && i && power_check) {
			power_check = 0;
			dot_check = 1;
			i++;

			if ((text[i] == '+' || text[i] == '-'))
				i++;
		}
		
		if (text[i] < '0' || text[i] > '9')
			return 0;
	}

	return 1;
}

int isUnsignedInteger (char *text) {
	unsigned int i = 0, power_check = 1; // 1 means, that dot or power has not
	                                                               //been used yet
	if ((text[0] == '+'))
		i++;

	for (; text[i] != '\0'; i++) {
		if ((text[i] == 'e' || text[i] == 'E') && i && power_check) {
			power_check = 0;
			i++;

			if ((text[i] == '+'))
				i++;
		}

		if (text[i] < '0' || text[i] > '9')
			return 0;
	}

	return 1;
}

/*----------------------------------------------------------------------------*/
/*------------------------------logarithm functions---------------------------*/
double taylor_log(double x, unsigned int n) {
	if (isinf(x))
		return INFINITY;
	if (isnan(x) || isnan (n))
		return NAN;
	if (x < 0.0)
		return NAN;
	if (x <= DBL_MIN)
		return -1*INFINITY;
	
	if (x < 1) { // input has been already checked so x > 0
		double log_t = 0.0, z = 1.0 - x, t = 1.0;

		if (n == 0)
			return 1.0;

		for (unsigned int i = 1; i <= n; i++) {
			t *= z;			
			log_t -= t/i;
		}
		return log_t;	
	}
	
	else { // input has been already checked so x > 0
		double log_t = 0.0, t = 1.0;

		if (n == 0)
			return 1.0;

		for (unsigned int i = 1; i <= n; i++) {
			t *= ((x - 1.0) / x);
			log_t += t/i;
		}
		return log_t;	
	}
}

unsigned int taylor_log_iter(double x,double z,double eps,unsigned int iters) {	
	
	for (; makePower(z, iters)/iters > eps; iters++);

	/* This will make result correct, but slows also program */
	double saved_log = log(x);
	for (; xabs(taylor_log(x, iters) - saved_log) > eps; iters++)
		if (iters == UINT_MAX)
			break;
			
	return iters;

}

unsigned int taylor_log_bis(double x,double eps) {
	/* Calculating logarithm would be too slow. Better will be if we calculate
	  only last member of taylor polynom and than compare it with EPS */	
	unsigned int iters = UINT_MAX/2, exp = 2;
	double cut = UINT_MAX / makePower(2.0, exp);
		
	if (x < 1.0) {
		double z = 1.0 - x;
		for (; cut > 1.0; exp++) {
			cut = UINT_MAX/makePower(2.0, exp);

			if(cut > iters || (UINT_MAX - cut) < iters) { // oferflow				
				iters = 1;				
				return taylor_log_iter(x, z, eps, iters); // we will try to cout iters  
			}                                                      //without bisection
			/* this command desides in which interval result is */
			iters +=  makePower(z, iters)/iters > eps ? cut : -cut;
		}

		return taylor_log_iter(x, z, eps, iters);
	}
	
	double z = (x - 1.0)/x;

	for (; cut > 1.0; exp++) {
		cut = UINT_MAX/makePower(2.0, exp);
		
			if(cut > iters || (UINT_MAX - cut) < iters) { // oferflow				
				iters = 1;				
				return taylor_log_iter(x, z, eps, iters); // we will try to cout iters  
			}                                                      //without bisection
		/* this command desides in which interval result is */
		iters +=  makePower(z, iters)/iters > eps ? cut : -cut;
	}

	return taylor_log_iter(x, z, eps, iters);
}

double cfrac_log(double x, unsigned int n) {	
	if (isinf(x))
		return INFINITY;
	if (isnan(x) || isnan (n))
		return NAN;
	if (x < 0.0)
		return NAN;
	if (x <= DBL_MIN)
		return -1*INFINITY;

	double z = (x - 1.0) / (x + 1.0), cf_log= 0.0, odd = 2*n - 1;
	double count = (n - 1), z_power = z * z;
	unsigned int i = n;

	if (n == 0)
		return 2*z;
	
	for (; i > 1; i--, odd -= 2.0, count--)
		cf_log = (count * count * z_power) / (odd - cf_log);
	
	return (2 * z) / (odd - cf_log); // last iteration
}

unsigned int cfrac_log_bis(double x, double eps) {
	const unsigned int INTERVAL = 100000; //the bigger interval is, the slower
	unsigned int iters = INTERVAL/2, exp = 2; //exp must be 2	        //program is
	double cut = INTERVAL / makePower(2, exp);
	double saved_log= log(x);

	for (; cut > 1.0; exp++) {
		cut = INTERVAL/makePower(2, exp);

		if(cut > iters || (UINT_MAX - cut) < iters) // oferflow				
			break;

		/* this command desides in which interval result is */
		if (xabs(cfrac_log(x, iters) - saved_log) > eps)
			iters += cut;
		
		else 
			iters -= cut;
	}
	
	/* this will secure, that resul will be correct */
	for (; xabs(cfrac_log(x, iters) - saved_log) > eps; iters++)
		if (iters == UINT_MAX) // overflow
			break;

	return iters;
}

/*----------------------------------------------------------------------------*/
/*-------------------------------Output functions-----------------------------*/
int output_log(char **argv) {
	const double ZERO = 0.0; // 0.0 != 0.0, but zero == zero
	double x = strtod(argv[2], '\0'), n = strtod(argv[3], '\0');
	if (x < ZERO || (x >= ZERO && x <= DBL_MIN)) {
		alternativeOutput_log(argv);
		return 1;
	}

	printf("       log(%g) = %.12g\n", x, log(x));
	printf("    cf_log(%g) = %.12g\n", x, cfrac_log(x, n));
	printf("taylor_log(%g) = %.12g\n", x, taylor_log (x, n));

	return 1; //succes
}

int output_iter(char **argv) {
	const double ZERO = 0.0; // 0.0 != 0.0, but zero == zero
	double min = strtod(argv[2], '\0'), max = strtod(argv[3], '\0');
	double eps = strtod(argv[4], '\0');
	if (min < ZERO || (min >= ZERO && min < DBL_MIN)) {
		alternativeOutput_iter(argv);
		return 1;
	}
	if (max < ZERO || (max >= ZERO && max < DBL_MIN)) {
		alternativeOutput_log(argv);
		return 1;
	}

	printf("       log(%g) = %.12g\n", min, log(min));
	printf("       log(%g) = %.12g\n", max, log(max));

	unsigned int iters_min = cfrac_log_bis(min, eps);
	unsigned int iters_max = cfrac_log_bis(max, eps);
	unsigned int a = iters_min > iters_max ? iters_min : iters_max;

	if (a <= 0) 
		return -1; //error

	if (a == UINT_MAX)
		printf("WARNING: Maximum number of iterations has been reached\n");

	printf("continued fraction iterations = %d\n", a);
	
	double result = cfrac_log (min, a);

	printf("    cf_log(%g) = %.12g\n", min, result);

	result = cfrac_log (max, a);

	printf("    cf_log(%g) = %.12g\n", max, result);

	iters_min = taylor_log_bis(min, eps);
	iters_max = taylor_log_bis(max, eps);
	a = iters_min > iters_max ? iters_min : iters_max;

	if (a <= 0)
		return -1; //error

	if (a == UINT_MAX)
		printf("WARNING: Maximum number of iterations has been reached\n");

	printf("taylor polynomial iterations = %d\n", a);
	printf("taylor_log(%g) = %.12g\n", min, taylor_log (min, a));
	printf("taylor_log(%g) = %.12g\n", max, taylor_log (max, a));

	return 1; //succes
}

void help() {
	printf("2nd project of IZP - iterative calculations\n");
	printf("Author:Jiri Matejka\n");
	printf("login: xmatej52\n");
	printf("\nStart program with following arguments:\n\n");
	printf("1. --log X N\n");
	printf("to calculate the natural logarithm of the number X in N iterations");
	printf("\n2. --iter MIN MAX EPS\n");
	printf("to find exact number of needed iteratins to calculate each number\n");
	printf("from interval [MIN, MAX]. EPS >= 1e-12 and it means tolarance.\n");
	return;
}

void InvalidInput () {
	fprintf(stderr, "Arguments you have entered are invalid.\n");
	fprintf(stderr, "Please start program with following arguments:\n\n");
	fprintf(stderr, "1. --log X N\n");
	fprintf(stderr, "to calculate the natural logarithm of the number X in N iterations");
	fprintf(stderr, "\n2. --iter MIN MAX EPS\n");
	fprintf(stderr, "to find exact number of needed iteratins to calculate each number\n");
	fprintf(stderr, "from interval [MIN, MAX]. EPS >= 1e-12 and it means tolarance.\n");
	fprintf(stderr, "\n2nd project of IZP - iterative calculations\n");
	fprintf(stderr, "Author:Jiri Matejka\n");
	fprintf(stderr, "login: xmatej52\n");
	return;
}

void alternativeOutput_log (char **argv) {
		
	double x = strtod(argv[2], '\0');
	const double ZERO = 0.0; // 0.0 != 0.0, but zero == zero
	if ((x < DBL_MIN && x >= ZERO) || isinf(x)) {
		double result = -INFINITY;
		printf("       log(%g) = %.12g\n", x, log(x));
		printf("    cf_log(%g) = %.12g\n", x, result);
		printf("taylor_log(%g) = %.12g\n", x, result);
	}

	if (x < ZERO) {
		double result = NAN;
		printf("       log(%g) = %.12g\n", x, log(x));
		printf("    cf_log(%g) = %.12g\n", x, result);
		printf("taylor_log(%g) = %.12g\n", x, result);
	}

	return;
}

void alternativeOutput_iter (char **argv) {
	double min = strtod(argv[2], '\0'), max = strtod(argv[3], '\0');
	double eps = strtod(argv[4], '\0');	
	const double ZERO = 0.0; // 0.0 != 0.0, but zero == zero

	printf("       log(%g) = %.12g\n", min, log(min));
	printf("       log(%g) = %.12g\n", max, log(max));

	if (min < DBL_MIN && min >= ZERO) {
		double result = -INFINITY;
		int a = 1;

		if (max >= DBL_MIN)
			a = cfrac_log_bis(max, eps);

		printf("continued fraction iterations = %d\n", a);
		printf("    cf_log(%g) = %.12g\n", min, -INFINITY);

		if (max < DBL_MIN && max >= ZERO) {
			printf("    cf_log(%g) = %.12g\n", max, -INFINITY);
			printf("taylor polynomial iterations = %d\n", a);
			printf("taylor_log(%g) = %.12g\n", min, -INFINITY);
			printf("taylor_log(%g) = %.12g\n", max, -INFINITY);
			return;
		}

		result = cfrac_log (max, a);
		printf("    cf_log(%g) = %.12g\n", max, result);
		a = taylor_log_bis(max, eps);
		result = taylor_log (max, a);
		printf("taylor polynomial iterations = %d\n", a);
		printf("taylor_log(%g) = %.12g\n", min, -INFINITY);
		printf("taylor_log(%g) = %.12g\n", max, result);
	}

	if (min < ZERO) {
		double result = NAN;
		int a = 1;

		printf("continued fraction iterations = %d\n", a);
		printf("    cf_log(%g) = %.12g\n", min, NAN);

		if (max < DBL_MIN && max >= ZERO) {
			printf("    cf_log(%g) = %.12g\n", max, -INFINITY);
			printf("taylor polynomial iterations = %d\n", a);
			printf("taylor_log(%g) = %.12g\n", min, NAN);
			printf("taylor_log(%g) = %.12g\n", max, -INFINITY);
			return;
		}

		if (max < ZERO) {
			printf("    cf_log(%g) = %.12g\n", max, NAN);
			printf("taylor polynomial iterations = %d\n", a);
			printf("taylor_log(%g) = %.12g\n", min, NAN);
			printf("taylor_log(%g) = %.12g\n", max, NAN);
			return;
		}
		a = cfrac_log_bis(max, eps);
		result = cfrac_log (max, a);
		printf("    cf_log(%g) = %.12g\n", max, result);
		a = taylor_log_bis(max, eps);
		result = taylor_log (max, a);
		printf("taylor polynomial iterations = %d\n", a);
		printf("taylor_log(%g) = %.12g\n", min, NAN);
		printf("taylor_log(%g) = %.12g\n", max, result);
		
	}
	return;
}
/*----------------------------------------------------------------------------*/
/*-------------------------------Other functions------------------------------*/

double makePower (double x, long long int exp) {
	if ( exp < 0.0) {
		x = 1.0/x;
		exp = -exp; //now exp > 0
	}
	
	if (exp == 0.0)
		return 1.0;
	if (exp == 1.0)
		return x;

	double y = 1.0;

	while (exp > 1) {
		
		if (exp%2 == 0)
			exp/=2;

		else {
			exp = (exp - 1)/2;
			y *= x;
		}

		x*=x;
	}
	return x*y;
}

double xabs(double x) {
	return x > 0 ? x : -x;
}
/*----------------------------------------------------------------------------*/
