#include <stdio.h>

int main() {
	for (int i=1900; i<=3000; i++) 
		for (int j=1; j < 13; j++) 
			for (int k=1; k <= 31; k++) {
			if (j <= 9 && k >= 10)
				printf("%i-0%i-%i ", i, j, k);
			else if (k <= 9 && j >= 10)
				printf("%i-%i-0%i ", i, j, k);
			else if (k <= 9 && j <= 9)
				printf("%i-0%i-0%i ", i, j, k);
			else
				printf ("%i-%i-%i ", i, j, k);
			}
	return 0;     
}
