// CS 33 Hwk 1
// Kevin Vu


// 2.64 Handling odd bits, return 1 if that odd bit equals 1; return 0 otherwise

int any_odd_one(unsigned x)
{
	return !!(x & 0xAAAAAAAA) // If ‘even’ case, you would use 5. Index 0 starts on the right side 
}

int any_odd_one(unsigned x)
{
	return (x & 0xAAAAAAAA) != 0 // Returns 1 (true) when the odd bit doesn’t equal 0 (therefore was equal to 1) 
	// Returns 0 (false) when the odd bit equals 0 */
}

// 2.71 
// xbyte: Turns 4 signed bytes (bytenum) into 32-bit unsigned, bytes within word ordered 0 to 3 (LSB->MSB)
// use two’s-complement arithmetic and arithmetic right shifts

// That is, the function will extract the designated byte 
//and sign extend it to be a 32-bit int.
/* Failed attempt at xbyte */
int xbyte(packed_t word, int bytenum)
{
    return (word >> (bytenum << 3)) & 0xFF; 
}

// A. What is wrong with the prototype's code?
/*
		0			1			2			3			bytenum in decimal
		
		0000		0001		0010		0011		bytenum in binary
		
		0000		1000		10000		11000		bytenum << 3
		
		0			8			16			24			bytenum in decimal=
		
		word>>0		word>>8		word>>16	word>>24	word >> (bytenum << 3) ** ARITHMETIC **
										
														result & 0xFF ** TWO'S-COMPLEMENT (Invert & Add one) so 0xFF == 0000 0000 + == 0000 0001 
																				 0001
																				 
	The problem is here. That is, using the bitwise operator & with the mask 0xFF causes the Most Significant Bit (MSB) to be 0 (anything & with 0 returns 0). This is a problem because the MSB is a signed bit. As a result, this function will never return a negative output

*/

// B. Correct solution
int xbyte(packed_t word, int bytenum)
{
	// Work with 8 bits
	// Invert the significance of the bytes
	int reverseSig = 3 - bytenum;
	// logical left shift 3 to use to find bits to traverse (converting to 32)
	int bitsToTraverse = reverseSig << 3;
	// find byte to extract
	int result = word << bitsToTraverse;
	// "Sign extend it to be 32-bits from 8"
	result = result >> 24;
	return result;
}

// 2.72
//You are given the task of writing a function that will copy an integer val into a buffer buf, but it should do so only if enough space is available in the buffer.
// Here is the code you write:
/* Copy integer into buffer if space is available */
/* WARNING: The following code is buggy */
void copy_int(int val, void *buf, int maxbytes)
{
    if (maxbytes-sizeof(val) >= 0)
            memcpy(buf, (void *) &val, sizeof(val));
}
// This code makes use of the library function memcpy. Although its use is a bit artificial here, where we simply want to copy an int, it illustrates an approach commonly used to copy larger data structures.
// You carefully test the code and discover that it always copies the value to the buffer, even when maxbytes is too small.

// A. Explain why the conditional test in the code always succeeds. Hint: The sizeof operator returns a value of type size_t.
/* 
	Variables:
		val is the int to copy
		maxbytes is the # of bytes available in the buffer
		note: int == 4bytes
		maxbytes-sizeof(val) >= 0
		maxbytes-4 >= 0
	Assume maxbytes has reached 3.  Naturally, we would see that 3-4 is not >= 0 and thus the condition would fail. However, because we have a signed - an unsigned variable, the result will become unsigned (positive). Therefore, the conditional test in the code always succeeds	
*/ 

// B. Show how you can rewrite the conditional test to make it work properly.

/*
	void copy_int(int val, void *buf, int maxbytes)
	{
		if (maxbytes >= sizeof(val))
				memcpy(buf, (void *) &val, sizeof(val));
	}
*/
	
// 2.81

// We are running programs on a machine where values of type int are 32 bits. They are represented in two’s complement, and they are right shifted arithmetically. Values of type unsigned are also 32 bits.
// We generate arbitrary values x and y, and convert them to unsigned values as
// follows:

/* Create some arbitrary values */
int x = random();
int y = random();
/* Convert to unsigned */
unsigned ux = (unsigned) x;
unsigned uy = (unsigned) y;
// For each of the following C expressions, you are to indicate whether or not the expression always yields 1. If it always yields 1, describe the underlying mathematical principles. Otherwise, give an example of arguments that make it yield 0.
	// A.	(x<y) == (-x>-y) 
		/*
			-x > - y == -x > -y
			
			If you let x be the minimum int value (INT_MIN)
			and y be an increment higher (INT_MIN + 1) or even 0, it will yield false
			Therefore, FALSE (Yields 0)
		*/
	
	// B.	((x+y)<<4) + y-x == 17*y+15*x 
		/*
			(x+y )* 2^4  +y-x
			(x+y) * 16	+y-x
			16x + 16y + y - x == 17y + 15x
			
			Therefore, TRUE (Yields 1)
		*/
		
	// C.	~x+~y+1 == ~(x+y) 
		/*
			If given, x to be INT-MIN while y = -1, this will return false.
			
			Therefore, FALSE (Yields 0)
		*/
		
		
	// D.	(ux-uy) == -(unsigned)(y-x) 
		/*
			Rearrange: (ux - uy) == - (unsigned) (-x+y)
						(ux - uy) == (unsigned) (x-y)
						unsigned (x-y) 	// Operators don't account/care for signed or unsigned
			Alternately,
				if you cast the LHS int,
				(int) ux-uy = x - y  == -(y - x)
				
				Therefore, TRUE (Yields 1)
		*/
		
	// E.	((x>>2)<<2)<=x 
		/*
			(x / 2^2) * 2^2   <= x
				x/4 * 4	 <=x
				x <= x 
				
				
			Therefore, TRUE (Yields 1)
		*/
		
		
// In addition, implement a function with signature 'void *arealloc (void *ptr, size_t nmemb, size_t size);', which acts like 'realloc (ptr, nmemb * size)' except that it does the right thing if the multiplication would overflow, namely return NULL and set errno to ENOMEM.
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
