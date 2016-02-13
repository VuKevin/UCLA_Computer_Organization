// Kevin Vu  CS 33 FK215 OPENMP Lab
- DL openmplab.tgz file from CCLE and unzip it.
- Copy it to lnxsrv09.seas.ucla gcc version 5.2.0 (gcc --version to check) via WinSCP
- cd to /w/home.04/ce/ugrad/hoai/openmplab
- Run the unaltered code via make seq (produce an executable called seq)
- SPEEDING UP:
	I. Determine what to speed up with gprof
	II. To speed it up: OpenMP, an API for shared memory, thread-based parallelism.
- List of #pragma omp directives: http://openmp.org/mp-documents/omp-hands-on-SC08.pdf
- Using gprof with seq:	
	make seq GPROF=1 [Compile with GPROF=1]
	./seq [Execute the executable and produce gmon.out]
		initial result:
		FUNC TIME : 0.562465
		TOTAL TIME : 2.547452
	gprof seq | less [Run gprof with the executable as the argument]
		Note: | less makes the text more readable
		quit "less" with q.
		Flat profile:
			Each sample counts as 0.01 seconds.
			  %   cumulative   self              self     total
			 time   seconds   seconds    calls  ms/call  ms/call  name
			 59.40      0.35     0.35       15    23.36    25.14  func1
			 23.76      0.49     0.14  5177344     0.00     0.00  rand2
			  5.09      0.52     0.03   491520     0.00     0.00  findIndexBin
			  5.09      0.55     0.03        1    30.04   143.61  addSeed
			  3.39      0.57     0.02                             sequence
			  1.70      0.58     0.01        1    10.01    10.01  imdilateDisk
			  1.70      0.59     0.01                             filter
			  0.00      0.59     0.00   983042     0.00     0.00  round
			  0.00      0.59     0.00       16     0.00     0.00  dilateMatrix
			  0.00      0.59     0.00       15     0.00     0.00  func2
			  0.00      0.59     0.00       15     0.00     0.00  func3
			  0.00      0.59     0.00       15     0.00     0.00  func4
			  0.00      0.59     0.00       15     0.00     2.00  func5
			  0.00      0.59     0.00       15     0.00     0.00  rand1
			  0.00      0.59     0.00        2     0.00     0.00  get_time
			  0.00      0.59     0.00        1     0.00     0.00  elapsed_time
			  0.00      0.59     0.00        1     0.00     0.00  fillMatrix
			  0.00      0.59     0.00        1     0.00     0.00  func0
- Using gprof with omp (what we want to optimize to compare to seq)
	* make omp GPROF=1
	* ./omp
		result:
		FUNC TIME : 0.034786
		TOTAL TIME : 2.219360
	* gprof omp | less [Run gprof with the executable as the argument]
	Flat profile:
	Each sample counts as 0.01 seconds.
	  %   cumulative   self              self     total
	 time   seconds   seconds    calls  ms/call  ms/call  name
	 63.84      0.44     0.44                             filter
	 18.86      0.57     0.13                             func3
	  4.35      0.60     0.03    10000     0.00     0.00  imdilateDisk
	  2.90      0.62     0.02  4266457     0.00     0.00  func1
	  2.90      0.64     0.02                             dilateMatrix
	  1.45      0.65     0.01        1    10.01    10.01  addSeed
	  1.45      0.66     0.01                             elapsed_time
	  1.45      0.67     0.01                             func2
	  1.45      0.68     0.01                             func4
	  1.45      0.69     0.01                             get_time
	  0.00      0.69     0.00       16     0.00     0.00  rand2
	  0.00      0.69     0.00        2     0.00     0.00  func0
	  0.00      0.69     0.00        1     0.00     0.00  fillMatrix
	  0.00      0.69     0.00        1     0.00     0.00  func5
	  0.00      0.69     0.00        1     0.00     0.00  rand1
NOTES:
	- Making a variable within the 'parallel' block private to each thread #pragma omp parallel private(a) where a is an int.
		- firstprivate(a) retains the value that a had outside the parallel block and uses it as an initalized value for each thread
	- Ensure you can use accumulators via 'reduction' keyword, allows you to collect results into one shared variable among threads
		int acc = 0;
		#pragma omp parallel for reduction(+:acc)
		for(i = 0; i < N; i++)
		{
		 acc += <PARALLELIZABLE CODE>
		}
		printf(“%d\n”, acc);
		Format: (<operation> : <variable>); 
- CHECKING FOR MEMORY LEAKS: 
	$ make omp MTRACE=1
	$ ./omp
	$ make checkmem
- CHECKING THAT OUTPUT.TXT == CORRECT.TXT via make check which should output silence
	make check
- Steps to reproduce:
	Added inline functions for fabs and pow.
	Changes to func0: #pragma omp parallel for private(i), hoisted & reciprocal'd n to avoid expensive division
	Changes to func1: #pragma omp parallel for private(i), #pragma omp parallel for (index_X, index_Y, j), hoisted & reciprocal Ones, combined two nested for loops, somewhat "hoisted" out probablity[i] = 0 into a double p
	Changes to func2: #pragma omp parallel for private(i) + reduction for the accumulators, reciprocal'd sumWeights
	Changes to func3: #pragma omp parallel for private(i) reduction(+:estimate_x) reduction(+:estimate_y)
	Changes to func4: #pragma omp parallel for private(i), hoisted & reciprocal'd n
	Changes to func5: #pragma omp parallel for private(i,j), (i), hoisted & reciprocal'd n, changed an if statement to a ternary operation (only relevant if cmov actually helps)
RESULTING SPEED UP ESTIMATES:
	FUNC TIME SEQ/FUNC TIME OMP: .562465/.034786 ~ 16x Speed Up
