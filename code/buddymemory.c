#ifndef HEADERS_H
#define HEADERS_H
#include "headers.h"
#endif

unsigned int** emptySlots;
unsigned int* numberEmptySlots;

/*
buddy_start, buddy_remove, buddy_insert, nextPowerOf2
use those
rest are helper functions

buddy start is called once to initialize the data structure
any time you create a new process (fork it) use buddy insert
every time a process terminates call the remove function
nextpowerof2 is used for printing

when you try to preempt a process, try inserting if it succeeds then switch the currently running process with the new one, else then don't switch and don't create
the new process (insert returns a -1 on failure)

for printing 
when creating
  int NextPowerOfTwo = __nextPowerOf2(Running->process->memsize);
  fprintf(memOutlog, "At time %d allocated %d bytes for process %d from %d to %d\n",
  getClk(), Running->process->memsize, Running->process->id, Running->process->memstart, Running->process->memstart + NextPowerOfTwo - 1);

when deleting
  fprintf(memOutlog, "At time %d freed %d bytes from process %d from %d to %d\n",
        getClk(), Running->process->memsize, Running->process->id, Running->process->memstart, Running->process->memstart + SmallestPowerOfTwo - 1);
*/

/*
     A function that returns the number if is a power of 2
     and returns the closet number which is a power of 2 of the number is not itself
*/
 int __nextPowerOf2(int num) {
  int count = 0;

    // Checking if the number itself is a power of 2
    // example of how it works: supposed num = 8 ----> 1000
    // the condition now equates to:
    // 1000 && !(1000 & 0111)
    // 1000 && !(0000)
    // True && ! False ----> True
    // only an exact power of 2 would make the right hand side ! False
    if(num && !(num&(num - 1))){
        return num;
    }

    // Having reached this point means that num is not a power of 2
    // So we allocate the smallest power of 2 greater than num.
    // example of how it works: supposed num = 9 ----> 1001
    // 1001 is true, so we add count (count = 1 now), then right shift
    // 100 is true, so we add count (count = 2 now), then right shift
    // 10 is true, so we add count (count = 3 now), then right shift
    // 1 is true, so we add count (count = 4 now), then right shift
    // 0 is false, exit while loop
    while(num){
        count++;
        num >>= 1;        // num = num >> 1;
    }

    // now count = 4, so 1 << 4 = 10000 ----> 16, which is the smallest
    // power of 2 greater than 9
    return 1 << count;
    // Notice that 1 << 0 = 1, so it handles this corner case well.
}

unsigned int __logOfPower2(unsigned int n) {
  unsigned count = 0;
  while (n != 0) {
    n >>= 1;
    count += 1;
  }

  return count - 1;
}

/* Slots */

int init_slots__(int size, unsigned int*** _emptySlots, unsigned int** _numberEmptySlots) {
  unsigned int slotsNum = __logOfPower2(size) + 1;

  // allocate slots of slotsNum size (first dimension is 11 elements (0 (2^0 = 1) -> 10 (2^10 = 1024)) )
  unsigned int** emptySlots = (unsigned int**)malloc((slotsNum) * sizeof(unsigned int*));

  for (int i = 0; i < slotsNum; i++) {
    /*
      each index corresponds to possible [size] locations
      i.e: at 0 : 2^0 = 1 correponds to 1024/1 possible locations
           at 8 : 2^8 = 256 corresponds to 1024/256 = 4 possible location
      size is divided by 2 for each iteration, at i = 10, size = 1
      meaning that for 2^10 = 1024 there's only one possible partition.
    */
    emptySlots[i] = (unsigned int*)malloc(size * sizeof(unsigned int));

    // if (emptySlots[i] == NULL) {
    //   for (int j = i - 1; j >= 0; j--)
    //     free(emptySlots[j]);

    //   free(emptySlots);

    //   return -1;
    // }

    size /= 2;
  }

  *_emptySlots = emptySlots;

  // allocate numberEmptySlots, an array of slotsNum (previously calculated) entries
  unsigned int* numberEmptySlots = (unsigned int*)malloc((slotsNum) * sizeof(unsigned int));

  memset(numberEmptySlots, 0, (slotsNum) * sizeof(unsigned int));
  fflush(stdout);

  *_numberEmptySlots = numberEmptySlots;

  return 1;
}

int slot_insert__(unsigned int slot_index, int position) {
  if (slot_index < 0)
    return -1;

  //looping index on empty slots
  int i = 0;

  /*should insert slot at correct index (comparing positions)
    empty slots are sorted according to their position in memory
    should increment as long as my position is greater than current position
    and my index is less than number of empty slots of given size
  */
  while (i < numberEmptySlots[slot_index] && emptySlots[slot_index][i] < position) {
    i++;
  }

  /*
    shifting current slots in order to insert in correct position
  */
  for (int j = numberEmptySlots[slot_index] - 1; j >= i; j--) {
    emptySlots[slot_index][j + 1] = emptySlots[slot_index][j];
  }

  emptySlots[slot_index][i] = position;

  numberEmptySlots[slot_index]++;

  return i;
}

bool slot_remove__(unsigned int slot_index, unsigned int procIdx) {
  if (procIdx >= numberEmptySlots[slot_index])  
    return 0;
  
  //shifting other slots to remove current slot
  for (int i = procIdx; i < numberEmptySlots[slot_index] - 1; i++) {
    emptySlots[slot_index][i] = emptySlots[slot_index][i + 1];
  }

  //decrease number of empty slots
  numberEmptySlots[slot_index]--;

  return 1;
}


bool buddy_split__(unsigned int slot_index) {

  //std::cout << "entered split" << std::endl;
  unsigned int slotsNum = __logOfPower2(1024) + 1;
  if (slot_index > slotsNum -1) return 0;

  if (slot_index == 0)
    return 0;

  /* while there are no empty slots of current size, recursively split on next greater size (next power of 2)
    initially, memory has one entry of size = 1024, so recursion base condition is
    next size with available empty slots (will never TLE since there's always an empty slot of size 1024)
  */

  if (numberEmptySlots[slot_index] == 0)
    if (buddy_split__(slot_index + 1) == 0) return 0;
  
  //get position of first empty slot of given size
  unsigned int position = emptySlots[slot_index][0];

  //remove slot of this size, and then split into two strictly smaller-sized slots
  slot_remove__(slot_index, 0);

  /*insert two slots instead of removed one, each of size (prevSize/2)
    first slot starts at start of prevSlot and second starts at start_of_prevSlot+(prevSize/2)
  */
  slot_insert__(slot_index - 1, position);
  slot_insert__(slot_index - 1, position + (1 << (slot_index - 1)));

  return 1;
}


void buddy_check_merge__(unsigned int slot_index, unsigned int element_index) {
  unsigned int elemPos = emptySlots[slot_index][element_index];
  unsigned int isOddIdx = ((elemPos / (1 << slot_index)) % 2);

  if (isOddIdx)  // oddIdx -> check if left of me is free
  {
    if (element_index > 0 && elemPos - emptySlots[slot_index][element_index - 1] == (1 << slot_index)) {
      // I have a left and it is the one right before me
      unsigned int slot_start = emptySlots[slot_index][element_index - 1];

      slot_remove__(slot_index, element_index);
      slot_remove__(slot_index, element_index - 1);

      unsigned int insertedAt = slot_insert__(slot_index + 1, slot_start);

      buddy_check_merge__(slot_index + 1, insertedAt);
    }
  }
  else {  // evenIdx -> check if right of me is free
    if (element_index < ((int)numberEmptySlots[slot_index] - 1) && emptySlots[slot_index][element_index + 1] - elemPos == (1 << slot_index)) {
      // I have a right and it is the one right after me
      unsigned int slot_start = emptySlots[slot_index][element_index];

      slot_remove__(slot_index, element_index + 1);
      slot_remove__(slot_index, element_index);

      unsigned int insertedAt = slot_insert__(slot_index + 1, slot_start);

      buddy_check_merge__(slot_index + 1, insertedAt);
    }
  }
}

int buddy_insert(unsigned int size) {
  // get index and size occupied by process that will be be allocated
  unsigned int nearestSize = __nextPowerOf2(size);
  unsigned int index = __logOfPower2(nearestSize);

  if (index > 8) { printf("bigger than (256) max size for a process \n");  return -1; }

  // if there is no empty slot of required size, recursively split until you reach required size
  if (numberEmptySlots[index] == 0) {
    if (buddy_split__(index + 1) == 0) return -1;
  }

  //position at which to allocate process
  unsigned int position = emptySlots[index][0];

  //remove first empty slot of 2^index size (slot is occupied by process now)  
  slot_remove__(index, 0);  

  return position;
}

bool buddy_remove(int position, int size) {
  // get index and size occupied by process that will be removed
  unsigned int nearestSize = __nextPowerOf2(size);
  unsigned int index = __logOfPower2(nearestSize);

  // insert an empty slot with calculated size and index
  int emptySlot = slot_insert__(index, position);

  // check if we could merge after each removal of process
  buddy_check_merge__(index, emptySlot);

  return 1;
}

void buddy_start() {
  int size = 1024;
  init_slots__(size, &emptySlots, &numberEmptySlots);

  unsigned int logSize = __logOfPower2(size);
  emptySlots[logSize][0] = 0;
  numberEmptySlots[logSize] = 1;
}

//for debugging all done :)

/*void main() {
    //corner cases insert 1
    buddy_start();
    std::cout << "allocated" << std::endl;
    std::cout << " hmm1 " << buddy_insert(300) << std::endl << std::endl;
    std::cout << " hmm1 " << buddy_insert(256) << std::endl << std::endl;
    std::cout << " hmm2 " << buddy_insert(256) << std::endl << std::endl;
    std::cout << " hmm3 " << buddy_insert(256) << std::endl << std::endl;
    int pos = buddy_insert(256);
    std::cout << " hmm4 " << pos << std::endl<< std::endl;
    

    
    std::cout << " hmm5 " << buddy_insert(256) << std::endl << std::endl;
    std::cout << " hmm5 " << buddy_insert(100) << std::endl << std::endl;
    std::cout << " removing " << buddy_remove(pos, 256) << std::endl << std::endl;
    std::cout << " hmm2 " << buddy_insert(256) << std::endl << std::endl;

}

void main() {
    //corner cases insert 1
    buddy_start();
    std::cout << "allocated" << std::endl;
    std::cout << " hmm1 " << buddy_insert(300) << std::endl << std::endl;
    //std::cout << " hmm1 " << buddy_insert(256) << std::endl << std::endl;
    std::cout << " hmm1 " << buddy_insert(1) << std::endl << std::endl;
    std::cout << " hmm2 " << buddy_insert(256) << std::endl << std::endl;
    std::cout << " hmm3 " << buddy_insert(256) << std::endl << std::endl;
    int pos = buddy_insert(256);
    std::cout << " hmm4 " << pos << std::endl<< std::endl;
    std::cout << " hmm55 " << buddy_insert(256) << std::endl << std::endl;
    

    
    std::cout << " hmm5 " << buddy_insert(256) << std::endl << std::endl;
    std::cout << " hmm5 " << buddy_insert(100) << std::endl << std::endl;
    std::cout << " removing " << buddy_remove(pos, 256) << std::endl << std::endl;
    std::cout << " hmm2 " << buddy_insert(256) << std::endl << std::endl;

}*/