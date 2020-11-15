/*
 *   Sieve of Eratosthenes
 *
 *   Programmed by Michael J. Quinn
 *
 *   Last modification: 7 September 2001
 */

#include "mpi.h"
#include <math.h>
#include <stdio.h>
#include<stdlib.h>
#define MIN(a,b)  ((a)<(b)?(a):(b))

int main (int argc, char *argv[])
{
   int    count;        /* Local prime count */
   double elapsed_time; /* Parallel execution time */
   int    first;        /* Index of first multiple */
   int    global_count; /* Global prime count */
   int    high_value;   /* Highest value on this proc */
   int    i;
   int    id;           /* Process ID number */
   int    index;        /* Index of current prime */
   int    low_value;    /* Lowest value on this proc */
   char  *marked;       /* Portion of 2,...,'n' */
   int    n;            /* Sieving from 2, ..., 'n' */
   int    p;            /* Number of processes */
   int    proc0_size;   /* Size of proc 0's subarray */
   int     prime;        /* Current prime */
   int    size;         /* Elements in 'marked' */
   int n1;
   int r;
   MPI_Init (&argc, &argv);

   /* Start the timer */

   MPI_Comm_rank (MPI_COMM_WORLD, &id);
   MPI_Comm_size (MPI_COMM_WORLD, &p);
   MPI_Barrier(MPI_COMM_WORLD);
   elapsed_time = -MPI_Wtime();
  
   if (argc != 2) {
      if (!id) printf ("Command line: %s <m>\n", argv[0]);
      MPI_Finalize();
      exit (1);
   }
  
   n = atoi(argv[1]);
   /* Figure out this process's share of the array, as
      well as the integers represented by the first and
      last array elements */
   if (n % 2 != 0)//n输入为奇数，n1为筛去偶数后的数字个数
	   n1= (n - 3) / 2 + 1;
   else//n输入为偶数，n1筛去偶数后的数字个数
	   n1= (n - 2) / 2;
   
   r = n1 % p;
   low_value = 3 + id*2*(n1/p)+2*MIN(r,id);
   high_value = 1 + (id+1)*2*(n1/p)+2*MIN(r,id+1);
   size = (high_value - low_value)/2 + 1;

   /* Bail out if all the primes used for sieving are
      not all held by process 0 */

   proc0_size = n1/p;

   if ((3 + proc0_size*2) <=(int) sqrt((double) n)) {
      if (!id) printf ("Too many processes\n");
      MPI_Finalize();
      exit (1);
   }

   /* Allocate this process's share of the array. */

   marked = (char *) malloc (size);

   if (marked == NULL) {
      printf ("Cannot allocate enough memory\n");
      MPI_Finalize();
      exit (1);
   }

   for (i = 0; i < size; i++) marked[i] = 0;//初始化被筛选的数组
   //if (!id) index = 0;
   const int  max_select_prime = (int)sqrt((double)n)+1;  
   //int max1 = max_select_prime + 1;
   int * check = (int *)calloc((max_select_prime),sizeof(int));
   int * primelist = (int *)calloc((max_select_prime), sizeof(int));
   int pos = 0;//记素数在prime中存放的下标
   /*int  *tempprime;
   tempprime = primelist;*/
   
   for (int i = 2; i < max_select_prime; i++)
   {
	   if (!check[i])//如果i是素数
	   {
		   //*(tempprime+(pos++))= i;
		   primelist[pos++] = i;
	   }
	   for (int j = 0; j < pos && i*primelist[j] < max_select_prime; j++)
	   {
		   check[i*primelist[j]] = 1;//筛掉
		   if (i%primelist[j] == 0)break;//如果i是后者倍数，不再乘下一个素数
		   //比如2*2后不再2*3，因为3*2等于6
		   //3*3后不再3*5，因为5*3=15
	   }
   }//筛选出到根号n的所有素数，存到prime所指向的空间中
   

   int tempcount = 1;
   prime = primelist[tempcount];
   count = pos - 1;
   while(tempcount <= count) {
	   if (prime * prime > low_value&&(prime * prime <=high_value))//最小值小于prime^2
	   {
			   first = (prime * prime - low_value) / 2;
	   }
	   else if (prime * prime >high_value)
	   {
		   first = size;
	   }
		   
      else {
         if (!(low_value % prime)) first = 0;//最小值等于prime^2
		 else//最小值大于prime^2
		 {
			 if ((low_value / prime) % 2 == 0)
				 first = ((low_value/(prime)+1)*(prime)-low_value)/2;
			 else
				 first = ((low_value/(prime) +2)*(prime)-low_value) / 2;
		 }
      }
      for (i = first; i < size; i += prime) marked[i] = 1;
      /*if (!id) {
         while (marked[++index]);
         prime = index*2 + 3;
      }
      if (p > 1) MPI_Bcast (&prime,  1, MPI_INT, 0, MPI_COMM_WORLD);*/
	  tempcount++;
	  prime = primelist[tempcount];
   };
   count = 0;
   for (i = 0; i < size; i++)
      if (!marked[i]) count++;
   if (p > 1) MPI_Reduce(&count, &global_count, 1, MPI_INT, MPI_SUM,
	   0, MPI_COMM_WORLD);
   else if (p = 1)
	   global_count = count;
   /* Stop the timer */
   
   elapsed_time += MPI_Wtime();
   realloc(check, (max_select_prime) * sizeof(int));
   realloc(primelist, (max_select_prime) * sizeof(int));

   /* Print the results */

   if (!id) {
      printf ("There are %d primes less than or equal to %d\n",
         global_count+1, n);
      printf ("SIEVE (%d) %10.6f\n", p, elapsed_time);
   }
   MPI_Finalize ();
   return 0;
}
