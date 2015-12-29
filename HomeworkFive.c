// CS 33 | KEVIN VU | FALL '15' | hello-thread.c | create & reap n joinable peer threads
#include "csapp.h"

void *thread(void *vargp) // Peer thread
{
	printf("Hello, world!\n"); // Print
	return NULL; // Terminate
}

// argc and argv are how command lines are passed into main
// argc = argument count
// argv = argument vector, list of strings representing the arguments
int main(int argc, char *argv[]) 
{
	pthread_t tid; // 
	long nArgs = strtol(argv[1], NULL, 0); // Convert String to Long. Start at argument 1, and continue until NULL is reached; [1] because [0] holds the program itself
	int i;
	for (i = 0; i < nArgs; i++) // Create & reap nArgs joinable peer threads
	{
		pthread_create(&tid, NULL, thread, NULL); // Create peer thread
		pthread_join(tid, NULL); // Wait for termination
	}
	
	exit(0);
}

// Example input: 
// $ gcc hello-thread.c -o g csapp.c -pthread
// $ ./g 8
// Example output:
// Hello, world!
// Hello, world!
// Hello, world!
// Hello, world!
// Hello, world!
// Hello, world!
// Hello, world!
// Hello, world!


*/
12.18

- Trajectory A is unsafe starting from L1 into the unsafe region
- Trajectory B is safe 
- Trajectory C is unsafe starting from L2 into the unsafe region


12.24

The functions in the RIO I/O package are thread safe. Are they reentrant as well? (Does a fxn that is thread safe imply it is reentrant?)

Answer: From observations it seems that the functions in the RIO I/O package are reentrant depending on the function.
rio_readn is reentrant, rio_writen is reentrant, rio_readinitb isn't, rio_read isn't, rio_readlineb isn't, and rio_readnb isn't
The ones that aren't are because there are global struct/variables used.

Explanation: 
* No, thread-safe does not always imply that it is reentrant. *
Thread-safe implies that the function delivers a correct output while repeatedly being called from several concurrent threads. Reentrent implies that a function accesses variables that are not shared when calling by multiple threads occurs. Therefore, reentrant fxns will always be called as the thread-safe fxns. Robust I/O packages are either buffered or unbuffered fxns where buffered would be thread safe (user can read some text, some binary data, some text..). Each of the Robust I/O functions passes in as an argument a pointer to the buffer. Then, ops are exclusively made on the current buffer and the local stack variables. 

In short,
The thread safe functions in the RIO I/O package being "thread safe" mean that the function can be called in sync by threads, even in the case of shared data (references to the shared data are stored for later use)
Meanwhile, a reentrant function means that the function can be called in sync by threads, BUT it is only able to do so if each call  uses its respective data.

EXAMPLES: (From Wiki)
CASE 1: Neither thread-safe nor reentrant (Using a nonconstant global variable)
int t
void swap(int *x, int *y)
{
	t = *x;
	*x = *y;
	*y = t;
}

CASE 2: Thread-safe but NOT reentrant
_thread int t; // Thread local variable
void swap(int *x, int *y)
{
	t = *x;
	*x = *y;
	*y = t;
}

CASE 3: NOT Thread-Safe but reentrant
int t;
void swap(int *x, int *y)
{
	int s;
	s = t; // Save global "state" in a local variable
	t = *x;
	*x = *y;
	*y = t
	t = s; // Restore the global "state"
}

CASE 4: Thread-safe AND Reentrant (local variable use)
void swap(int *x, int *y)
{
	int t;
	t = *x;
	*x = *y;
	*y = t;
}

*/
