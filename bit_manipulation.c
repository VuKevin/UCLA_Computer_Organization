// Bit manipulation without use of control flow statements // 

/* isTmin: returns 1 if x is equal to int_min (0x80000000), else x doesn't equal int_min and return 0 */
int isTMin(int x)
{
  int int_min = 0x80000000;
  int a  = !((~x+1) ^ x) | !x);
  return (a & 1) | (~a & 0);
}
// ~x+1 is the two's complement XOR'd with itself will return 0 if INT_MIN 
// since the two's complement of INT_MIN is INT_MIN
// | !x to take care of the 0 case

/* bitXOR: mirror the XOR bitwise operator using only ~ and & */
int bitXOR(int a, int b)
{
  return ~( ~(a & b) & ~(~a & b) );
}
// Utilize deMorgan's Law a | b == ~(~a & ~b)
// x XOR y means it will only return true if x and y disagree
// Either x went to the party and not y OR x didn't go to the party and y did
// (x & ~y) | (~x & y)
// Demorgan's law applied: ~(~(x & ~y) & ~(~x & y))

/* allOddBits: return true (1) if all the oddBits are equal to the value of 1, else return 0 */
/* ex. OxFFFFFFFD -> 0 , 0xAAAAAAAA -> 1
int allOddBits(int x)
{
  int mask1 = (0x55 << 8) ^ (0x55); // mask1 = 0x5555
  int mask2 = (mask1 << 16) ^ (mask1); // mask 2 = 0x55555555
  int mask3 = ~mask2; // mask 3 = 0xAAAAAAAA
  return !((x & mask3) ^ mask3);
}
// AND mask the input with mask 3 'nd XOR with mask 3
// If they agree, the XOR will return false
// If they disagree, the XOR will return true
// We want the opposite, so ! the result

/* divPwr2: do x/2^n where 0 <= n <= 30 */
/* if x = 15 and n = 2, 15/4 = 3.ff drop the ff for int division */
int divPwr2(int x, int n)
{
  int a = x >> 31; // Determine the sign, result either 0x00000000 or 0x11111111
  int b = (1 << n) + ~0; // Determine 2^n
  int maskedResult = a & b; // Mask with the sign
  int add = maskedResult + x;
  return (add >> n); // Perform the arithmetic division
}

/* rotateRight: Rotate x to the right by n bits, wrap around semantics */
int rotateRight(int x, int n)
{
  int a = !(n & 31); // Check for shift by 0, you can shift up to 31. 
  int y = ((x >> n) & ~(~0 << (31 + (~n + 1)) << 1)) | (x << (32 + (~n + 1)));
	return (a & x) | (~a & y);
}
// (x >> n), shift to the right by n, remaining bits: 32-n
// ~(~0 << (31 + (~n + 1)) << 1)) is a mask to protect the remaining bits from being changed
// OR with (x << (32 + (~n + 1))) b/c bit | 1 -> 1 'nd bit | 0 -> bit and so you restore what you lost from the right shift

/* isNonNegative: return 1 if x >=0, return 0 otherwise */
int isNonNegative(int x)
{
	return ~(~x >> 31) + 1;
}
// invert x, then shift right by 31 bits
// So if x was originally positive or zero, ~x >> 31 would result in 0xFFFFFFFF 
// if x was originally negative, ~x >> 31 would result in 0x00000000
// Invert again and add 1. Therefore, if x is positive or 0 0xFFFFFFFF becomes 0x00000000 and add 1 to be true
// if x is negative, 0x00000000 becomes 0xFFFFFFFF which is negative 1 and add 1 to be false

/* sm2tc: convert from signed magnitured to 2's complement */
int sm2tc(int x)
{
  int a = ~(x >> 31); // Check the sign, let a be 1 (true) if it is positive
  int y = (~x + 1) ^ (~0 << 31); // Find the 2's complement and XOR it with INT_MIN 0x80000000 to deal with the special case of int min
  return ( (a & x) | (~a & y) );
}

/* howManyBits: What is the minimum number of bits required to represent x in 2's complement? */
int howManyBits(int x)
{
  int a,b,c,d,e,f;
	int x_prime = (x >> 31) ^ x; 
	a = !(!(x_prime >> 16)) << 4; // a will be 16 if it has a 1 in the first half  [16* | 16]
	x_prime = x_prime >> a; // Shift right a which will be 0 or 16
	b = !(!(x_prime >> 8)) << 3; // b will be 8 if it has a 1 in the first half [8* | 8 ]
	x_prime = x_prime >> b;
	c = !(!(x_prime >> 4)) << 2; // c will be 4 if it has a 1 in the first half [4*| 4 ]
	x_prime = x_prime >> c;
	d = !(!(x_prime >> 2)) << 1; // d will be 2 if it has a 1 in the first half [2* | 2]
	x_prime = x_prime >> d;
	e = !(!(x_prime >> 1)) << 0; // e will be 1 if it has a 1 in the first half [1* | 1]
	f = !(!x_prime);
	return (a + b + c + d + e + f + 1);
}
// !(!(x_prime >> 16)) means that if any bit in the 16 bit half is equal to 1, it will return 1 else 0
