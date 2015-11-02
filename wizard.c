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
  init:
  sem_wait(&wizLock);
  if (self->status == 1 && cube->game_status){sem_post(&wizLock);}
  int trys = 0;

  // check if the thread should be killed or not, if so kill it here
	if(self->threadKill == 1 || check_winner(cube))
	{
	  //printf("Wizard %c%d killed its thread \n", self->team, self->id);
      if(!cube->single){sem_post(&wizLock);}
	  pthread_exit(NULL);
	}

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
          printf("Request denied, room locked!\n");
          dostuff();
          
          /* Chooses the new room */
          newroom = choose_room(self);
          
          if(trys >= 5)
            {
              if(!cube->single){sem_post(&wizLock);}
              dostuff();
              goto init;
            }
          else
          trys++;
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
  	    printf("Wizard %c%d in room (%d,%d) finds nobody around\n",
  	  	 self->team, self->id, newroom->x, newroom->y);
  	    /* Fill in */
  	    // Seems fine to me...
  	  }
      else
  	  {
  	    if (other->team != self->team) /* Other is from opposite team */
  	    {
  	      if (other->status == 0) /* Checks if the opponent is active */
  	      {
  	  	  printf("Wizard %c%d in room (%d,%d) finds active enemy.\n",
  	  	  self->team, self->id, newroom->x, newroom->y);
  
  	  	  fight_wizard(self, other, newroom);
  	  	  }
  	      else
  	  	  {
  	  	    printf("Wizard %c%d in room (%d,%d) finds enemy already frozen.\n",
  	  		 self->team, self->id, newroom->x, newroom->y);
  	  	  }
  	    }
  	    else /* Other is from same team */
  	    {
  	      if (other->status == 1) /* Checks if the friend is frozen */
  	  	  {
  	  	    free_wizard(self, other, newroom);
  	  	  }
  	  	  else /* Friend is not frozen. BRAG TIME! */
  	  	  {
  	  	  	//printf("The Wizards flex at each other. This could last hours. \n");
  	  	  	dostuff(); // The flexing time
  	  	  }
  	    }
  
  	    /* Fill in */
  	    if (cube->single)
        {
          int winner = check_winner(cube);
          if(winner == 1){printf("Team A won the game! \n");}
          else if(winner == 2){printf("Team B won the game! \n");}
        }
  
  	  }
  	  if(!cube->single){sem_post(&wizLock);}
      /* Thinks about what to do next */
      dostuff();
  
      oldroom = newroom;
      newroom = choose_room(self);

    }
    else
    {
    	if(!cube->single){sem_post(&wizLock);} //I was getting a deadlock when the wizard sem_wait() and then gets frozen, they never gave up control.
    }
  }
  
  return NULL;
}