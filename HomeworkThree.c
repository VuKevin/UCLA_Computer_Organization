// Kevin Vu CS 33
// 2nd EDITION

/* 3.64 ◆◆
For this exercise, we will examine the code generated by gcc for functions that have structures as arguments and return values, and from this see how these language features are typically implemented.
The following C code has a function word_sum having structures as argument and return values, and a function prod that calls word_sum:

typedef struct 
{
    int a;
    int *p;
} str1;

typedef struct 
{
    int sum;
    int diff;
} str2;

str2 word_sum(str1 s1) 
{
    str2 result;
    result.sum  = s1.a + *s1.p;
    result.diff = s1.a - *s1.p;
	return result;
}

int prod(int x, int y)
{
    str1 s1;
    str2 s2;
    s1.a = x;
    s1.p = &y;
    s2 = word_sum(s1);
    return s2.sum * s2.diff;
}
gcc generates the following code for these two functions:
....


The instruction ret $4 is like a normal return instruction, but it increments the stack pointer by 8 (4 for the return address plus 4 additional), rather than 4.


A. We can see in lines 5–7 of the code for word_sum that it appears as if three values are being retrieved from the stack, even though the function has only a single argument. Describe what these three values are.
- Lines 5 to 7
movl 8(%ebp), %eax
movl 12(%ebp), %ebx
movl 16(%ebp), edx

The value on line 5 is the result variable which will be returned at the end of word_sum / it is pointer to a struct str2* result 
The value on line 6 is the integer variable, s1.a.
The value on line 7 is the pointer to an int variable, s1.p

B. We can see in line 4 of the code for prod that 20 bytes are allocated in the stack frame. These get used as five fields of 4 bytes each. Describe how each of these fields gets used.

line 4 of prod:

subl $20, %esp // 20 byets are allocated in the stack. They get used as 5 fields of 4 bytes each.

The first field is pointing to the return structure. The next two fields points to s1.a and s2.p. Meanwhile, the last two fields store the result of word_sum. This is respect to s2.sum (%ebp - 8) and s2.diff (%ebp-4)

1st block: Storing integers x and y
2nd block: Values of s1.a and s2.p
3rd block: s2.sum and s2.diff
4th block: the product of 3rd block
5th block: result

C. How would you describe the general strategy for passing structures as arguments to a function?

A struct contains can data of different types, not necessarily always the same. To pass structures as arguments to a function:

Declaration of Function: void badfun(struct name) // struct is a keyword, name is the name associated with this particular struct and pass the structure directly in the stack as the common argument

void badfun(struct name)
{
	badfun(struct name);
}

The struct and its contents are placed in the stack. Meanwhile, the 1st element is passed in. The callee must use offsets with the first element as a base as means to retrieve other elements. The values of the struct are placed in reverse order on the stack


D. How would you describe the general strategy for handling a structure as a return value from a function? 
void yay(struct name)


Use return to give control back to the calling function.
Check the return type of the function as well as the argument that was passed into the function, in this case void and a struct
Use this knowledge to make sure everything aligns. ex. using the integer variables in a struct to computer their difference

When returning a structure, the struct and its contents are held in the stack and a pointer to the struct is returned
 
*/



/*

Consider the following union declaration:
union ele 
{
    struct
	{
		int *p;
		int y; 
	} e1;
	
    struct 
	{
        int x;
        union ele *next;
    } e2;
};

This declaration illustrates that structures can be embedded within unions.
The following procedure (with some expressions omitted) operates on a
linked list having these unions as list elements:
void proc (union ele *up)
{
	up->  = *(up->  ) - up->  ; 
}


A. What would be the offsets (in bytes) of the following fields: 
e1.p: 0
e1.y: 4
e2.x: 0
e2.next: 4


B. How many total bytes would the structure require?
8

C. The compiler generates the following assembly code for the body of proc:
		up at %ebp+8
		1 movl 8(%ebp), %edx // edx = up retrieves the up
		2 movl 4(%edx), %ecx // ecx = up + 4 // increment up by 4 offset and store into ecx
		3 movl (%ecx), %eax  // eax = MEM[up + 4] Eax's bit vectory equals *(up + 4)
		4 movl (%eax), %eax // eax = MEM[eax]  Eax's bit vector in memory and set it to eax
		5 subl (%edx), %eax //  eax = eax - MEM[up]
		6 movl %eax, 4(%ecx) // MEM[ECX + 4] = eax
		
		
On the basis of this information, fill in the missing expressions in the code for proc. Hint: Some union references can have ambiguous interpretations. These ambiguities get resolved as you see where the references lead. There is only one answer that does not perform any casting and does not violate any type constraints.


void proc (union ele *up)
{
	up->e2.next->e1.y  = *(up->e2.next->e1.p ) - up->e2.x ; 
}

*/
