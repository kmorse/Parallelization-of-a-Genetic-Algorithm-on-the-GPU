/**
 * description:
 * 	Simple genetic algorithm for finding equation that equals target number
 * 	example for number 27 has many results:
 * 		- 7+29-8-1
 * 		- 15+28-16
 * 	this code isn't really perfect ^_^ but shows some basics.
 * 	tested in linux fedora 14 with valgrind.
 * 
 * author: ADRABI Abderrahim (adrabi[at]mail[dot]ru)
 * date: 2011-10-03
 */
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#define POPSIZE		1024
#define ELITRATE	0.1f
#define MUTATIONRATE	0.25f
#define ELEMENTS	8
#define MUTATION	RAND_MAX * MUTATIONRATE
#define TARGET		5270

/**
 * Basic elements for construction an equation
 */
static const char *BIN_ELEMENTS[12] = {
  "0000\0",			// 0
  "0001\0",			// 1
  "0010\0",			// 2
  "0011\0",			// 3
  "0100\0",			// 4 
  "0101\0",			// 5 
  "0110\0",			// 6
  "0111\0",			// 7
  "1000\0",			// 8
  "1001\0",			// 9
  "1010\0",			// + 
  "1011\0"			// -
};

/**
 * Structure base of genome
 */
typedef struct
{
  unsigned int fitness;
  char *gen;
} ga_struct;

/**
 * Initialize new random population
 */
void
init_population (ga_struct * population, ga_struct * beta_population)
{
  const int bin_size = (sizeof (char) * ELEMENTS * 4) + 1;
  int index = 0;
  for (; index < POPSIZE; index++)
    {
      // default initialization/ create empty genome
      population[index].fitness = 0;
      population[index].gen = malloc (bin_size);
      *population[index].gen = '\0';

      // default initialization/ create empty genome
      beta_population[index].fitness = 0;
      beta_population[index].gen = malloc (bin_size);
      *beta_population[index].gen = '\0';

      int e = 0;
      for (; e < ELEMENTS; e++)
	{
	  // put random element in population
	  // 12 is count of elements in BIN_ELEMENTS array
	  strcat (population[index].gen, BIN_ELEMENTS[(rand () % 12)]);
	}
    }
}

/**
 * Calculate each individual fitness in population
 */
void
cal_fitness (ga_struct * population)
{
  int index = 0;
  int unsigned fitness = 0;
  for (; index < POPSIZE; index++)
    {
      char *gen_str = population[index].gen;
      //printf("gen, %f", population[index].gen);
      int gen_len = strlen (gen_str), sum = 0, current_value = 0, step = 0;
      unsigned int last_operator_index = -1;
      char last_operator = (char) 0;

      for (; step < ELEMENTS; step++)
	{
	  char element[5] = "\0";
	  strncpy (element, gen_str, 4);
	  //printf("element, %c", element[0]);
	  //printf("element1, %c", element[1]);
	  //printf("element2, %c", element[2]);
	  //printf("element3, %c", element[3]);
	  if (strcmp ("0000", element) == 0)
	    {
	      current_value *= 10;
	    }
	  else if (strcmp ("0001", element) == 0)
	    {
	      current_value = (current_value * 10) + 1;
	    }
	  else if (strcmp ("0010", element) == 0)
	    {
	      current_value = (current_value * 10) + 2;
	    }
	  else if (strcmp ("0011", element) == 0)
	    {
	      current_value = (current_value * 10) + 3;
	    }
	  else if (strcmp ("0100", element) == 0)
	    {
	      current_value = (current_value * 10) + 4;
	    }
	  else if (strcmp ("0101", element) == 0)
	    {
	      current_value = (current_value * 10) + 5;
	    }
	  else if (strcmp ("0110", element) == 0)
	    {
	      current_value = (current_value * 10) + 6;
	    }
	  else if (strcmp ("0111", element) == 0)
	    {
	      current_value = (current_value * 10) + 7;
	    }
	  else if (strcmp ("1000", element) == 0)
	    {
	      current_value = (current_value * 10) + 8;
	    }
	  else if (strcmp ("1001", element) == 0)
	    {
	      current_value = (current_value * 10) + 9;
	    }
	  // ignore elements that have two flowed operators or ends with operators like:
	  //         * 1+-1++-1
	  //         * 2333+55+
	  else if (strcmp ("1010", element) == 0
		   && step - last_operator_index > 1 && step + 1 < ELEMENTS)
	    {
	      if (last_operator == (char) 0)
		{
		  sum = current_value;
		}
	      else if (last_operator == '+')
		{
		  sum += current_value;
		}
	      else if (last_operator == '-')
		{
		  sum -= current_value;
		}

	      current_value = 0;
	      last_operator_index = step;
	      last_operator = '+';
	    }
	  // ignore elements that have two flowed operators or ends with operators like:
	  //         * 1+-1++-1
	  //         * 2333+55+
	  else if (strcmp ("1011", element) == 0
		   && step - last_operator_index > 1 && step + 1 < ELEMENTS)
	    {
	      if (last_operator == (char) 0)
		{
		  sum = current_value;
		}
	      else if (last_operator == '+')
		{
		  sum += current_value;
		}
	      else if (last_operator == '-')
		{
		  sum -= current_value;
		}

	      current_value = 0;
	      last_operator_index = step;
	      last_operator = '-';
	    }
	  else
	    {
	      /// error the binary string not found
	      sum = 999999;
	      break;
	    }
	  gen_str += 4;
	}

      if (last_operator == '+')
	{
	  sum += current_value;
	}
      else if (last_operator == '-')
	{
	  sum -= current_value;
	}

      // abs, because fitness is unsigned integer ^_^
      population[index].fitness = abs (sum - TARGET);
    }
}

/**
 * sort function needed by quick sort
 */
int
sort_func (const void *e1, const void *e2)
{
  return ((ga_struct *) e1)->fitness - ((ga_struct *) e2)->fitness;
}

/**
 * sort population by fitness
 */
inline void
sort_by_fitness (ga_struct * population)
{
  qsort (population, POPSIZE, sizeof (ga_struct), sort_func);
}

/**
 * select elit element in top array after sort
 */
void
elitism (ga_struct * population, ga_struct * beta_population, int esize)
{
  const int gen_len = ELEMENTS * 4 + 1;
  int index = 0;
  for (; index < esize; index++)
    {
      int e = 0;
      for (; e < gen_len; e++)
	{
	  beta_population[index].gen[e] = population[index].gen[e];
	}
    }
}

/**
 * mutate an individual with random rate
 */
void
mutate (ga_struct * member)
{
  int tsize = strlen (member->gen);
  int number_of_mutations = rand () % 10;
  int m = 0;
  for (; m < number_of_mutations; m++)
    {
      int apos = rand () % tsize;

      if (member->gen[apos] == '0')
	{
	  member->gen[apos] = '1';
	}
      else
	{
	  member->gen[apos] = '0';
	}
    }
}

/**
 * mate randomly the rest of population after elitism
 */
void
mate (ga_struct * population, ga_struct * beta_population)
{
  int esize = POPSIZE * ELITRATE;

  // elitism of top elements in array
  elitism (population, beta_population, esize);

  // mate the rest of shitty population xD
  int m = esize, i1 = -1, i2 = -1, pos = -1, tsize = ELEMENTS * 4 + 1;
  for (; m < POPSIZE; m++)
    {
      pos = rand () % tsize;
      i1 = rand () % POPSIZE;
      i2 = rand () % POPSIZE;

      int i = 0;
      for (; i < pos; i++)
	{
	  beta_population[m].gen[i] = population[i1].gen[i];
	}
      for (i = pos; i < tsize; i++)
	{
	  beta_population[m].gen[i] = population[i2].gen[i];
	}

      if (rand () < MUTATION)
	{
	  mutate (&beta_population[m]);
	}
    }
}

/**
 * decode binary string to readable format
 */
void
decode_gen (ga_struct * member)
{
  char *gen_str = member->gen;
  int step = 0;

  for (; step < ELEMENTS; step++)
    {
      char element[5] = "\0";
      strncpy (element, gen_str, 4);
      if (strcmp ("0000", element) == 0)
	{
	  printf ("0");
	}
      else if (strcmp ("0001", element) == 0)
	{
	  printf ("1");
	}
      else if (strcmp ("0010", element) == 0)
	{
	  printf ("2");
	}
      else if (strcmp ("0011", element) == 0)
	{
	  printf ("3");
	}
      else if (strcmp ("0100", element) == 0)
	{
	  printf ("4");
	}
      else if (strcmp ("0101", element) == 0)
	{
	  printf ("5");
	}
      else if (strcmp ("0110", element) == 0)
	{
	  printf ("6");
	}
      else if (strcmp ("0111", element) == 0)
	{
	  printf ("7");
	}
      else if (strcmp ("1000", element) == 0)
	{
	  printf ("8");
	}
      else if (strcmp ("1001", element) == 0)
	{
	  printf ("9");
	}
      else if (strcmp ("1010", element) == 0)
	{
	  printf ("+");
	}
      else if (strcmp ("1011", element) == 0)
	{
	  printf ("-");
	}
      gen_str += 4;
    }

  printf ("\n");
}

/**
 * free memory before exit program
 */
void
free_population (ga_struct * population)
{
  int index = 0;
  for (; index < POPSIZE; index++)
    {
      free (population[index].gen);
    }
  free (population);
}

/**
 * swap arrays pointers
 */
void
swap (ga_struct ** p1, ga_struct ** p2)
{
  ga_struct *tmp = *p1;
  *p1 = *p2;
  *p2 = tmp;
}

/**
 * main program
 */
int
main (void)
{

	float cpu1,cpu2;	
	cpu1 = ((float) clock())/CLOCKS_PER_SEC;
  srand (time (NULL));

  ga_struct *population = malloc (sizeof (ga_struct) * POPSIZE);
  ga_struct *beta_population = malloc (sizeof (ga_struct) * POPSIZE);

  init_population (population, beta_population);


  char *gen_str = population[0].gen;
char element[5] = "\0";
	  strncpy (element, gen_str, 4);
	  //if (strcmp ("0000", element) == 0)
	  
	  
  int index = 0;
  for (; index < POPSIZE; index++)
    {
      
      cal_fitness (population);
      
      sort_by_fitness (population);
      

      // print current best individual
      printf ("binary string: %s - fitness: %d\n", population[0].gen,
	      population[0].fitness);

      if (population[0].fitness == 0)
	{
	  //~ print equation
	  decode_gen (&population[0]);
	  break;
	}
	  
      mate (population, beta_population);
      swap (&population, &beta_population);
      
    }

  free_population (population);
  free_population (beta_population);
cpu2 = ((float) clock())/CLOCKS_PER_SEC;
  

  printf("Execution time (s) = %le\n",cpu2-cpu1);

  return 0;
}

