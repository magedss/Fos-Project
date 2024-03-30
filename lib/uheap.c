#include <inc/lib.h>
//#include <lib/syscall.c>

//==================================================================================//
//============================== GIVEN FUNCTIONS ===================================//

//==================================================================================//
struct pair {
	  bool allocated;
	   uint32 numpages;
	   uint32 start;
};
struct pair page_allocator_arr2[NUM_OF_UHEAP_PAGES];


int FirstTimeFlag = 1;
void InitializeUHeap()
{
	if(FirstTimeFlag)
	{
#if UHP_USE_BUDDY
		initialize_buddy();
		cprintf("BUDDY SYSTEM IS INITIALIZED\n");
#endif
		FirstTimeFlag = 0;
	}
}

//==================================================================================//
//============================ REQUIRED FUNCTIONS ==================================//
//==================================================================================//

//=============================================
// [1] CHANGE THE BREAK LIMIT OF THE USER HEAP:
//=============================================
/*2023*/
void* sbrk(int increment)
{
	return (void*) sys_sbrk(increment);
}

//=================================
// [2] ALLOCATE SPACE IN USER HEAP:
//=================================
void* malloc(uint32 size)
{
	//==============================================================
	//DON'T CHANGE THIS CODE========================================
	InitializeUHeap();
	if (size == 0) return NULL ;
	//==============================================================
	//TODO: [PROJECT'23.MS2 - #09] [2] USER HEAP - malloc() [User Side]
	// Write your code here, remove the panic and write your code
	//Use sys_isUHeapPlacementStrategyFIRSTFIT() and	sys_isUHeapPlacementStrategyBESTFIT()
	//to check the current strategy
	/*start*/

 uint32 Hard_LIMIT=sys_hardlimit();
 uint32 AvilableSPACE=(USER_HEAP_MAX-(Hard_LIMIT+PAGE_SIZE));
 uint32 AvilableSTART=Hard_LIMIT+PAGE_SIZE;//132100096 NOPE

 	if(size <= DYN_ALLOC_MAX_BLOCK_SIZE){
 		// cprintf("man1\n");
 		return (void*)alloc_block_FF(size);}

 	else{
 		int needed_pages=ROUNDUP(size,PAGE_SIZE)/PAGE_SIZE;
 			int my_pages=AvilableSPACE/PAGE_SIZE;
 			//cprintf("here%d,,,,,%u\n",my_pages*my_pages);
 			if(ROUNDUP(size,PAGE_SIZE)<(AvilableSPACE)){
 				int cnt=0;
 				uint32 foundi=0;
 				bool ok=0;
 				unsigned int i;
 			for( i=(AvilableSTART);i<USER_HEAP_MAX;){
 				unsigned int j=((i-AvilableSTART)/PAGE_SIZE);
 				if(page_allocator_arr2[j].allocated==1)
 					cnt=0;
 				else
 					cnt++;
 				if(cnt==needed_pages){
 					ok=1;
 					break;
 				}
 				i+=PAGE_SIZE;

 }
 	i=i-((needed_pages-1)*PAGE_SIZE);
 				//cprintf("hedasafsdfaasdfgdasgdsfgsdfgdsfhdfhdfsre%d,,,,,%u\n",i);
 				if(ok){
 					//cprintf("MAN:%d and %d",i,AvilableSTART); max shares test
 					page_allocator_arr2[(i-AvilableSTART)/PAGE_SIZE].start=i;
 					page_allocator_arr2[(i-AvilableSTART)/PAGE_SIZE].numpages=needed_pages;
 					int j=((i-AvilableSTART)/PAGE_SIZE);
 					int to =j+needed_pages+1;
 					int c=needed_pages;

 					while(j<my_pages){
 									if (page_allocator_arr2[j].allocated==0){
 										c--;
 										page_allocator_arr2[j].allocated=1;
 										page_allocator_arr2[j].start=i;
 										page_allocator_arr2[j].numpages=needed_pages;//useless
 									}
 									if(c==0){
 										break;
 									}
 									j++;
 								}
 					sys_allocate_user_mem(i,size);




 	     return (void*)i;

 			}
 			//end of the size checking condition
 			return (void*)NULL;

 			}
 			else{
 	  		  //cprintf("REQUIRED SIZED IS BIG");// COMMENT IT LATER

 				return (void*)NULL;
 			}
 	}
 	 // cprintf("not ok for any start address");// COMMENT IT LATER

 //	kpanic_into_prompt("kmalloc() is not implemented yet...!!");

 	return (void*)NULL;

	return NULL;
	}

//=================================
// [3] FREE SPACE FROM USER HEAP:
//=================================
void free(void* virtual_address)
{
	uint32 Hard_LIMIT= sys_hardlimit();
	if((uint32)virtual_address<Hard_LIMIT){
			//cprintf("MAN:%d and %d");

			free_block(virtual_address);
		}
		else{
			//cprintf("MAN:%d and %d");

		unsigned int AvilableSTART=Hard_LIMIT+PAGE_SIZE;
		uint32 cur=(uint32)virtual_address;
		cur=((cur-AvilableSTART)/PAGE_SIZE);
		uint32 from=(page_allocator_arr2[cur].start-AvilableSTART);
		uint32 address=page_allocator_arr2[cur].start;
		from/=PAGE_SIZE;
		uint32 HOW_MANY=page_allocator_arr2[from].numpages;
		sys_free_user_mem(ROUNDDOWN(address,PAGE_SIZE),HOW_MANY*PAGE_SIZE);
		for(int i=0;i<HOW_MANY;i++,from++){
			page_allocator_arr2[from].allocated=0;
			page_allocator_arr2[from].numpages=0;
			page_allocator_arr2[from].start=0;
			address+=PAGE_SIZE;

		}}

}


//=================================
// [4] ALLOCATE SHARED VARIABLE:
//=================================
void* smalloc(char *sharedVarName, uint32 size, uint8 isWritable)
{
	//==============================================================
	//DON'T CHANGE THIS CODE========================================
	InitializeUHeap();
	if (size == 0) return NULL ;
	//==============================================================
	panic("smalloc() is not implemented yet...!!");
	return NULL;
}

//========================================
// [5] SHARE ON ALLOCATED SHARED VARIABLE:
//========================================
void* sget(int32 ownerEnvID, char *sharedVarName)
{
	//==============================================================
	//DON'T CHANGE THIS CODE========================================
	InitializeUHeap();
	//==============================================================
	// Write your code here, remove the panic and write your code
	panic("sget() is not implemented yet...!!");
	return NULL;
}


//==================================================================================//
//============================== BONUS FUNCTIONS ===================================//
//==================================================================================//

//=================================
// REALLOC USER SPACE:
//=================================
//	Attempts to resize the allocated space at "virtual_address" to "new_size" bytes,
//	possibly moving it in the heap.
//	If successful, returns the new virtual_address, in which case the old virtual_address must no longer be accessed.
//	On failure, returns a null pointer, and the old virtual_address remains valid.

//	A call with virtual_address = null is equivalent to malloc().
//	A call with new_size = zero is equivalent to free().

//  Hint: you may need to use the sys_move_user_mem(...)
//		which switches to the kernel mode, calls move_user_mem(...)
//		in "kern/mem/chunk_operations.c", then switch back to the user mode here
//	the move_user_mem() function is empty, make sure to implement it.
void *realloc(void *virtual_address, uint32 new_size)
{
	//==============================================================
	//DON'T CHANGE THIS CODE========================================
	InitializeUHeap();
	//==============================================================

	//TODO: [PROJECT'23.MS2 - BONUS] [2] USER HEAP - realloc() [User Side]
	// Write your code here, remove the panic and write your code
	panic("realloc() is not implemented yet...!!");
	return NULL;

}


//=================================
// FREE SHARED VARIABLE:
//=================================
//	This function frees the shared variable at the given virtual_address
//	To do this, we need to switch to the kernel, free the pages AND "EMPTY" PAGE TABLES
//	from main memory then switch back to the user again.
//
//	use sys_freeSharedObject(...); which switches to the kernel mode,
//	calls freeSharedObject(...) in "shared_memory_manager.c", then switch back to the user mode here
//	the freeSharedObject() function is empty, make sure to implement it.

void sfree(void* virtual_address)
{
	// Write your code here, remove the panic and write your code
	panic("sfree() is not implemented yet...!!");
}


//==================================================================================//
//========================== MODIFICATION FUNCTIONS ================================//
//==================================================================================//

void expand(uint32 newSize)
{
	panic("Not Implemented");

}
void shrink(uint32 newSize)
{
	panic("Not Implemented");

}
void freeHeap(void* virtual_address)
{
	panic("Not Implemented");

}
