// CS 33 | KEVIN VU | FALL '15' | hello-thread.c | create & reap n joinable peer threads
#include "csapp.h"

void *thread(void *vargp) // Peer thread
{
	printf("Hello, world! \n"); // Print
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
// Resulting output:
// Hello, world!
// Hello, world!
// Hello, world!

# UCLA_Computer_Organization
