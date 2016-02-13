#include <stdlib.h> // realloc
#include <limits.h> // UINT_MAX [unsigned]
#include <errno.h> // ENO_MEM 

void *arealloc(void *ptr, size_t nmemb, size_t size)
{
	// Acts like realloc (ptr, nmemb * size)
	// If multiplication overflows
	if (((((size_t)(-1)) / nmemb) < size) && nmemb > 0) // Or use UINT_MAX  
	// Then not enough available memory. Alternatively, use (size != 0&& (nmemb * size)/size != nmemb)
	{
		// cerr << "Overflow!" << endl;
		errno = ENOMEM; //  set_errno(EN0MEM);
		return NULL;
	}
	else // it's alright to multiply
		return realloc(ptr, nmemb * size);
}
// TEST: 	int a = 65537;
	// int b = 65537;
// Check for Divide by Zero
// UINT_MAX is the upperbound of size_t
