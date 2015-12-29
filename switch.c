int switch_prob(int x, int n)
{
	int result = x;
	switch(n) 
	{
		case 0x32:
		case 0x34:
			result = result << 2;
			break;
		case 0x35:
			result = result >> 2;
			break;
		case 0x36:
			result *= 3;
		case 0x37:
			result *= result;
		// case 0x33:
		default:
			result += 10;
	}
	return result; 
}

/* (gdb) x/6w 0x80485d0
0x80485d0: 0x08048438 0x08048448 0x08048438 0x0804843d 
0x80485e0: 0x08048442 0x08048445

// Jump table break down.
// 32	 33		 34 	35	 	36		37
// L10	 L16	L10		L12		L14		L15

Fill in the body of the switch statement with C code that will have the same behavior as the machine code.

1 08048420 <switch_prob>:
2 8048420: 55 					push %ebp
3 8048421: 89 e5				mov %esp, %ebp
4 8048423: 8b 45 08				mov 0x8(%ebp), %eax // Load x into eax Result = x
5 8048426: 8b 55 0c				mov 0xc(%ebp), %edx // Load n into edx
6 8048429: 83 ea 32 			sub $0x32, %edx // edx == n -= 32
7 804842c: 83 fa 05				cmp $0x5, %edx // n - 5 
8 804842f: 77 17				ja	8048448 <switch_prob + 0x28> // if n > 5, go to default L16
9 8048431: ff 24 95 d0 85 04 08 jmp *0x80485d0(,%edx,4) // Jump Table
10 8048438: c1 e0 02			shl $0x2, %eax // Result << 2
11 804843b: eb 0e				jmp 804844b <switch_prob+0x2b> // Jump to Line 17
12 804843d: c1 f8 02			sar $0x2,%eax // Result >> 2
13 8048440: eb 09				jmp 804844b <switch_prob+0x2b> // Jump to Line 17
14 8048442: 8d 04 40			lea (%eax,%eax,2),%eax // eax = (eax + 2*eax)
15 8048445: 0f af c0			imul(%eax, %eax) // result *= result
16 8048448: 83 c0 0a			add $0xa, %eax // result += 10
17 804844b: 5d					pop %ebp
18 804844c: c3					ret
*/
