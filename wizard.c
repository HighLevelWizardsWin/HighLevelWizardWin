#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <assert.h>
#include <unistd.h> //CHANGE ME REMOVE BEFORE TURNING IN

#include "cube.h"
#include "wizard.h"


void *wizard_func(void *wizard_descr)
{
  
  //pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
  //pthread_testcancel();
  //pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);


  struct cube* cube;
  struct room *newroom;
  struct room *oldroom;
  struct wizard* self;
  struct wizard* other;


  self = (struct wizard*)wizard_descr;
  assert(self);
  cube = self->cube;
  assert(cube);
  
  /* Sets starting room */
  oldroom = cube->rooms[self->x][self->y];
  assert(oldroom);
  
  /* Chooses the new room */
  newroom = choose_room(self);

  /* Infinite loop */
  while (1)
  {
  	// check if the thread should be killed or not, if so kill it here
	if(self->threadKill == 1)
	{
		printf("Wizard %c%d killed its thread \n", self->team, self->id);
		pthread_exit(NULL);
	}
	else {printf("threadKill == %d \n", self->threadKill);}

  	if (self->status == 0) // Needed? Could be controlled somewhere else
  	{
      /* Loops until he's able to get a hold on both the old and new rooms */
      while (1)
	  {
	    printf("Wizard %c%d in room (%d,%d) wants to go to room (%d,%d)\n",
	    self->team, self->id, oldroom->x, oldroom->y, newroom->x, newroom->y);
	    
	    if (!try_room(self, oldroom, newroom))
	    {
          /* Waits a random amount of time */
          printf("The door is locked. You should learn teleport. \n");
          dostuff();
          
          /* Chooses the new room */
          newroom = choose_room(self);
          
          /* Goes back to the initial state and try again */
          continue;
        }
	    else break;
	  }
        
      printf("Wizard %c%d in room (%d,%d) moves to room (%d,%d)\n",
	     self->team, self->id, 
	     oldroom->x, oldroom->y, newroom->x, newroom->y);
  
      /* Fill in */
      // I don't see anything that should go here 

      /* Self is active and has control over both rooms */
      switch_rooms(self, oldroom, newroom);
  
      other = find_opponent(self, newroom);
  
      /* If there is not another wizard does nothing */
      if (other == NULL)
  	  {
  	    printf("Wizard %c%d in room (%d,%d) finds nobody around:  ¯\\_(ツ)_/¯  \n",
  	  	 self->team, self->id, newroom->x, newroom->y);
  	    /* Fill in */
  	    // Seems fine to me...
  	  }
      else
  	  {
  	    if (other->team != self->team) /* Other is from opposite team */
  	    {
  	      /* Checks if the opponent is active */
  	      if (other->status == 0)
  	      {
  	  	  printf("Wizard %c%d in room (%d,%d) finds active enemy\n",
  	  	  self->team, self->id, newroom->x, newroom->y);
  
  	  	  fight_wizard(self, other, newroom);
  	  	  }
  	      else
  	  	  {
  	  	    printf("Wizard %c%d in room (%d,%d) finds enemy already frozen. What a nerd.\n",
  	  		 self->team, self->id, newroom->x, newroom->y);
  	  	  }
  	    }
  	    else /* Other is from same team */
  	    {
  	      /* Checks if the friend is frozen */
  	      if (other->status == 1)
  	  	  {
  	  	    free_wizard(self, other, newroom);
  	  	  }
  	  	  else
  	  	  {
  	  	  	printf("The Wizards flex at each other. This could last hours. \n");
  	  	  	dostuff(); // The flexing time
  	  	  }
  	    }
  
  	    /* Fill in */
  	    
  
  	  }

      /* Thinks about what to do next */
      dostuff();
  
      oldroom = newroom;
      newroom = choose_room(self);

      
    }
  }
  
  return NULL;
}