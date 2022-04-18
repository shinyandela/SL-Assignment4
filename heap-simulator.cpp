#include <stdio.h>
#include <stdlib.h>


struct node //block information
{
	int id;
	int start;
	int end;
	struct node *link;
} *block_list = NULL;//list pointer for allocated blocks

typedef struct node block_type;

//global variables for the simulator
int pm_size;
int hole_algo;
int remaining;
int best_so_far;

void Start() 
{
	//Initialiser for memory size and free memory,also choose hole fitting, can be thought as a constructor
	printf ("Enter size of physical memory: ");
	scanf ("%d", &pm_size);
	remaining = pm_size;
	best_so_far = pm_size;

	printf ("Enter hole-fitting algorithm (0=first fit, 1=best_fit): ");
	scanf ("%d", &hole_algo);
	return;
}

void printAllocationTable()
{
	block_type *next_block = block_list;
	// print table containing block id, starting address, ending address and actual memory address

	printf ("ID\tStart\tEnd(not inclusive)\n");
	printf ("------------------------------\n");

	while (next_block != NULL)
    {
    	printf ("%d\t%d\t%d\t\n", next_block->id, next_block->start,next_block->end);
    	next_block = next_block->link;
    }
	return;
}


void allocate()
{

	int block_id, block_size;
	block_type *new_block, *current_block, *best_block;
	int hole_start, hole_end;
	int best_start, best_end;
	int large_enough_hole = 0;

	printf ("Enter block id: "); //basically serves as the variable name for pointer to this location, used for free()
	scanf ("%d", &block_id);
	printf ("Enter block size: ");
	scanf ("%d", &block_size);

	if (block_size > remaining)
    {
    	printf ("Cannot fit the block - not enough memory!");
    	return;
    }

	new_block = (block_type *) malloc (sizeof (block_type));
	new_block->id = block_id;

	if (block_list == NULL) //empty allocation list
	{
    	new_block->start = 0;
    	new_block->end = block_size;
    	new_block->link = NULL;
    	block_list = new_block;

    	best_so_far -= block_size;	
    	remaining -= block_size;	
    	printAllocationTable ();
    	return;
    }
  //else traverse list until either appropriate hole is found or the end of the list is reached
	current_block = block_list;

	while (current_block != NULL)
    {
		if (block_id == current_block->id)
		{
			printf ("Duplicate id\n");
			return;
		}

      	hole_start = current_block->end;

      	if (current_block->link != NULL)
		{
			hole_end = current_block->link->start;
		}
		else
		{
	  		hole_end = pm_size;
		}

    	if (block_size <= (hole_end - hole_start))
		{
	  		large_enough_hole = 1;
	  		if (hole_algo == 0)
	    	{
	      		new_block->start = hole_start;
	      		new_block->end = hole_start + block_size;
	      		remaining -= block_size;
				new_block->link = current_block->link;
				current_block->link = new_block;
				printAllocationTable ();
				return;
	    	}
	  		//best fit
	  		else
	    	{
	      		//printf ("best so far: %d\n", best_so_far);
	      		if ((hole_end - hole_start) <= best_so_far)
				{	
		  			best_so_far = hole_end - hole_start;
		  			best_start = hole_start;
		  			best_end = hole_end;
		  			best_block = current_block;
				}
	    	}
		}
    	current_block = current_block->link;
    }       

	if (large_enough_hole == 0)
    {
    	printf ("No hole large enough");
    	return;
    }

	new_block->start = best_start;
	new_block->end = best_start + block_size;
	new_block->link = best_block->link;
	best_block->link = new_block;
	remaining -= block_size;
	printAllocationTable ();
	return;
}

void free_block()
{
	block_type *current_block = block_list;
	block_type *prev_block;
	int block_id;
	current_block = block_list;

	if (current_block == NULL)
    {
    	printf ("There are no blocks");
    	return;
    }

	printf ("Enter block id: ");
	scanf ("%d", &block_id);

	while ((current_block != NULL) && (block_id != current_block->id))//linear search till we find block id in allocation list
    {
		prev_block = current_block;
    	current_block = current_block->link;
    }
  	//if end of linked list reached, print block id not found
  	if (current_block == NULL)
    {
      	printf ("\nCould not find block ID\n");
    }
  	else
    {
      	prev_block->link = current_block->link;
      	remaining += current_block->end - current_block->start;
      	best_so_far += remaining;
      	free (current_block);
    }//if block_id found, free it and add memory to free(this is where deframgentation would be done)
  	printAllocationTable ();
  	return;
}

void defragment()
{
  	block_type *current_block;
  	int current_size;
  	int prev_end = 0;

  	current_block = block_list;
  	while (current_block != NULL)
    {
      	current_size = current_block->end - current_block->start;	
      	current_block->start = prev_end;
      	current_block->end = current_block->start + current_size;
      	prev_end = current_block->end;
      	current_block = current_block->link;
    }
  
  	printAllocationTable ();
  	return;
}

void End(block_type * node) //destructor
{

  	if (node == NULL)
    {
      	return;
    }
  	else
    {
      	End(node->link);
      	free(node);
    }
  	return;
}

int main ()
{
  	int userSelection;
  	do
    {
      	printf ("\n Memory Allocation Strategies\n");
      	printf ("--------------------------------\n");
      	printf ("1) Enter parameters\n");
      	printf ("2) Allocate memory for block\n");
      	printf ("3) Deallocate memory for block\n");
      	printf ("4) Defragment memory\n");
      	printf ("5) Quit program\n");

      	printf ("\nEnter Selection: ");
      	scanf ("%d", &userSelection);
      	switch (userSelection)
		{
		case 1:
	  		Start();
	  		break;
		case 2:
	  		allocate();
		  	break;
		case 3:
	  		free_block();
			break;
		case 4:
			defragment();
	  		break;
		}
	}while (userSelection != 5);
  	End(block_list);
  	printf ("End of program");
  	return 0;
}
