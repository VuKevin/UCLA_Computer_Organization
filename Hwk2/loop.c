long loop(long x, long n)
{
	long result = 0 ; 
	long mask;
	for (mask = 1 ; mask != 0 ; mask = mask << n )
	{
		result ^= (x & mask);
	}
	return result;
}

/* [x in %rdi, n in %rsi]

loop: 

    movq  %rsi, %rcx // Load n into rcx %rcx = n result = n
    movq  $1, %rdx // Load 1 into edx %rdx = 1 or var1 = 1 mask = 1
    movq  $0, %rax // Load 0 into eax %rax = 0 or var2 = 0 result = 0
    jmp   .L2 // Jump to Label 2
	
  .L3:
    movq  %rdi, %r8 // %r8 = x Load x into %r8
    andq  %rdx, %r8 // %r8 = %r8 & %rdx ---> %r8 = x & %rdx // %r8 = x & mask
    xorq   %r8, %rax // %rax = %rax ^ %r8
    salq  %cl, %rdx // %rdx << %cl
	
  .L2:
    testq %rdx, %rdx // If rdx != 0 If mask != 0
    jne   .L3 // Then Jump to label 3
    rep; ret // else return
*/

/* A. Which registers hold program values x, n , result, and mask? */
// Register %rdi holds the value of x, Register %rsi holds the value of n (which is later copied into rcx), 
// Register %rdx holds the value of mask, and Register %rax holds the value of result.

/* B. What are the initial values of result and mask? */
// Result = 0;
// Mask = 1;

/* C. What is the test condition for mask? */
// The test conditon for mask is in line 28 where mask != 0 (mask cannot equal 0)

/* D. How does mask get updated? */
// According to line 25, it gets updated by shifting to the left by n bits
