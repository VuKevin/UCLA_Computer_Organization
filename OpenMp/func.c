// Open MP Lab
// Kevin Vu
// CS 33, F2K15
#include "func.h"
#include "util.h"
#include "omp.h"

inline int fabsprime(int a) // Added to speed up fabs
{
	return (a > 0) ? a : -a;
}

inline int powprime(const int b, const int e) // Added to speed up pow
{
	return (e == 0) ? 1: (b * powprime(b, e-1));
}

void func0(double *weights, double *arrayX, double *arrayY, int xr, int yr, int n)
{
	int i;
	double n_prime = 1 / (double) n; //Hoist
	#pragma omp parallel for private(i) // Multithreading to run this in parallel 
	for(i = 0; i < n; i++)
	{
		weights[i] = n_prime;
		arrayX[i] = xr;
		arrayY[i] = yr;
	}
}

void func1(int *seed, int *array, double *arrayX, double *arrayY,
			double *probability, double *objxy, int *index,
			int Ones, int iter, int X, int Y, int Z, int n)
{
	int i, j;
   	int index_X, index_Y;
	int max_size = X*Y*Z;
	
	
	#pragma omp parallel for private(i)
   	for(i = 0; i < n; i++)
	{
   		arrayX[i] += 1 + 5*rand2(seed, i);
   		arrayY[i] += -2 + 2*rand2(seed, i);
   	}
	double ones_prime = 1 / ((double) Ones); // Hoist and removed expensive division
	#pragma omp parallel for private(index_X, index_Y, j)
   	for(i = 0; i < n; i++) 
	{
		double p = 0;
   		for(j = 0; j < Ones; j++) // Combined two for loops into one
		{
   			index_X = round(arrayX[i]) + objxy[j*2 + 1];
   			index_Y = round(arrayY[i]) + objxy[j*2];
   			index[i*Ones + j] = fabsprime(index_X*Y*Z + index_Y*Z + iter);
   			if(index[i*Ones + j] >= max_size)
   				index[i*Ones + j] = 0;
   		// probability[i] = 0;
			// changed from probability[i]
   			p += (powprime((array[index[i*Ones + j]] - 100),2) -
   							  powprime((array[index[i*Ones + j]]-228),2))/50.0;
   		}
   		probability[i] = p * (ones_prime); // Changed to multiply 
   	}
}

void func2(double *weights, double *probability, int n)
{
	int i;
	double sumWeights=0;
	
	#pragma omp parallel for private(i)
	for(i = 0; i < n; i++)
   		weights[i] *= exp(probability[i]);

	#pragma omp parallel for private(i) reduction (+:sumWeights)
   	for(i = 0; i < n; i++)
   		sumWeights += weights[i];
	
	#pragma omp parallel for private(i)
	// Division is more expensive than multiplication so: Change divide into * and do the reciprocal of sumWeights
	for(i = 0; i < n; i++)
   		weights[i] *= (1/sumWeights);
}

void func3(double *arrayX, double *arrayY, double *weights, double *x_e, double *y_e, int n)
{
	double estimate_x=0.0;
	double estimate_y=0.0;
    int i;

	#pragma omp parallel for private(i) reduction(+:estimate_x) reduction(+:estimate_y)
	for(i = 0; i < n; i++)
	{
   		estimate_x += arrayX[i] * weights[i];
   		estimate_y += arrayY[i] * weights[i];
   	}

	*x_e = estimate_x;
	*y_e = estimate_y;

}

void func4(double *u, double u1, int n)
{
	int i;
	double n_prime = 1 / ((double) n); // Hoist & Reciprocal
	#pragma omp parallel for private(i)
	for(i = 0; i < n; i++)
	{
   		u[i] = u1 + i*(n_prime);
   	}
}

void func5(double *x_j, double *y_j, double *arrayX, double *arrayY, double *weights, double *cfd, double *u, int n)
{
	int i, j;

	#pragma omp parallel for private(i,j)
	for(j = 0; j < n; j++)
	{
   		//i = findIndex(cfd, n, u[j]);
   		i = findIndexBin(cfd, 0, n, u[j]);
   		// if(i == -1) // if statement to ternary operator
   			// i = n-1;
		i = (i == -1)? n-1 : i;
   		x_j[j] = arrayX[i];
   		y_j[j] = arrayY[i];

   	}

	double n_prime = 1 / ((double) n); // Hoist & Reciprocal
	#pragma omp parallel for private(i)
	for(i = 0; i < n; i++)
	{
		arrayX[i] = x_j[i];
		arrayY[i] = y_j[i];
		weights[i] = n_prime;
	} 
}
