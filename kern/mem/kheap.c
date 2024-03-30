#include "kheap.h"

#include <inc/memlayout.h>
#include <inc/dynamic_allocator.h>
#include "memory_manager.h"

struct pair {
   bool allocated;
   uint32 numpages;
   uint32 start;
};
int frame_to_page[1048580];
struct pair page_allocator_arr2[NUM_OF_KHEAP_PAGES];


int initialize_kheap_dynamic_allocator(uint32 daStart, uint32 initSizeToAllocate, uint32 daLimit)
{
	//TODO: [PROJECT'23.MS2 - #01] [1] KERNEL HEAP - initialize_kheap_dynamic_allocator()
	//Initialize the dynamic allocator of kernel heap with the given start address, size & limit
	//All pages in the given range should be allocated
	//Remember: call the initialize_dynamic_allocator(..) to complete the initialization
	//Return:
	//	On success: 0
	//	Otherwise (if no memory OR initial size exceed the given limit): E_NO_MEM

	//Comment the following line(s) before start coding...
	if(daStart+initSizeToAllocate>daLimit)
	    	  {
	    		  return E_NO_MEM;
	    	  }
	HEAP_START=daStart;
	brk=daStart+initSizeToAllocate;
	Hard_LIMIT=daLimit;
	// cprintf("%u and  %u\n",initSizeToAllocate,ROUNDUP(initSizeToAllocate,PAGE_SIZE) );//s

	struct FrameInfo *ptr_frame = NULL ;
	uint32 initrounded=  ROUNDUP(initSizeToAllocate,PAGE_SIZE);
	for(uint32 i = daStart; i < brk ; i +=PAGE_SIZE)
      {
    	  int ret=allocate_frame(&ptr_frame);
    	  if (ret==E_NO_MEM)
    	  {
    		  free_frame(ptr_frame);
              return E_NO_MEM;
    	  }
    	 map_frame(ptr_page_directory,ptr_frame,i,PERM_WRITEABLE);
  		ptr_frame->va=i;

		  frame_to_page[kheap_physical_address(i)/PAGE_SIZE]=i;

      }
		initialize_dynamic_allocator(daStart,initrounded);
		return 0;
}

void* sbrk(int increment)
{
	uint32 oldbrk=brk;


if(increment==0){
	return (void*)oldbrk;
}
    if (increment > 0) {
    	// less than or equal?
    	brk=ROUNDDOWN(brk,PAGE_SIZE);
    	if( ROUNDUP(increment+brk,PAGE_SIZE)<=Hard_LIMIT){
        int num_pages = ROUNDUP(increment,PAGE_SIZE)/PAGE_SIZE;
        for (int i = 0; i < num_pages; i++) {
            struct FrameInfo* ptr_frame = NULL;
            int ret = allocate_frame(&ptr_frame); // should i check >?

    		if(ret!=0){
    		// cprintf("couldnt allocate");//s
   		  free_frame(ptr_frame);

    		 return NULL;}
            map_frame(ptr_page_directory, ptr_frame, (uint32)brk, PERM_WRITEABLE);
    		ptr_frame->va=brk;

            frame_to_page[kheap_physical_address(brk)/PAGE_SIZE]=brk;
            brk += PAGE_SIZE;}
    	}
        else{
panic("laugh at this user "); }

    	return (void*)oldbrk;
    }

    else if (increment < 0) {


    	if( -increment+oldbrk>HEAP_START){
    		int kon=oldbrk;
    		if((kon%PAGE_SIZE)){
    			int now=ROUNDDOWN(oldbrk,PAGE_SIZE);
    			now=kon-now;
    			if(-increment<now){
    				brk+=increment;

    			return (void*)brk;}
    			else if(-increment>=now){
    				struct FrameInfo *ptr_frame_info;
    				uint32 *ptr_table = NULL;
    				brk-=now;
    				increment+=now;
    				ptr_frame_info=get_frame_info(ptr_page_directory, brk, &ptr_table);
    				        	free_frame(ptr_frame_info);
    				        	unmap_frame(ptr_page_directory, brk);
    			}
    		}
    		while (increment!=0){
    			if(-increment>=PAGE_SIZE){
    				increment+=PAGE_SIZE;
    				brk-=PAGE_SIZE;
    				struct FrameInfo *ptr_frame_info;
    				    				uint32 *ptr_table = NULL;
    				ptr_frame_info=get_frame_info(ptr_page_directory, brk, &ptr_table);
    				    				        	free_frame(ptr_frame_info);
    				    				        	unmap_frame(ptr_page_directory, brk);

    			}
    			else{
    			brk+=increment;
    			increment=0;
    			return (void*)brk;
    			}
    		}
     }
    	else
    		panic("laugh at this user ");

    }
    return (void*)brk;
}
//#define INITIAL_KHEAP_ALLOCATIONS (0 + KERNEL_SHARES_ARR_INIT_SIZE + KERNEL_SEMAPHORES_ARR_INIT_SIZE) // + ROUNDUP(num_of_ready_queues * sizeof(uint8), PAGE_SIZE) + ROUNDUP(num_of_ready_queues * sizeof(struct Env_Queue), PAGE_SIZE))
//#define ACTUAL_START ((KERNEL_HEAP_START + DYN_ALLOC_MAX_SIZE + PAGE_SIZE) + INITIAL_KHEAP_ALLOCATIONS)

//SHOULD I ROUND DOWN OR ROUND UP?

void* kmalloc(unsigned int size)
{
uint32 AvilableSPACE=(KERNEL_HEAP_MAX-(Hard_LIMIT+PAGE_SIZE));
uint32 AvilableSTART=Hard_LIMIT+PAGE_SIZE;//132100096 NOPE

	if(size <= DYN_ALLOC_MAX_BLOCK_SIZE){
	//	cprintf("laugh at this user ");
		return (void*)alloc_block_FF(size);}

	else{
		//cprintf("laugh at this user ");
		//===O(N^2) ==
	//BDL MAMSK KOL PAGE W AMSHE L A5R EL ARRAY
		// EL AWL HA3D LW FE MKAN WRA BA3DO B COUNTER 3ADE O(1)
		//THEN LW FE HA5OD EL START ADDDRESS W AMSHE KMAN LOOP A MARK W ALLOCATE W AMAP
		// MAFESH NULL

//		int needed_pages=ROUNDUP(size,PAGE_SIZE)/PAGE_SIZE;
//		int my_pages=AvilableSPACE/PAGE_SIZE;
//
//		//cprintf("here%d,,,,,%u\n",my_pages*my_pages);
//
//		if(ROUNDUP(size,PAGE_SIZE)<(AvilableSPACE)){
//		for(unsigned int i=(AvilableSTART);i<KERNEL_HEAP_MAX;){
//			unsigned int j=((i-AvilableSTART)/PAGE_SIZE);
//			unsigned int to=j+needed_pages;
//			//cprintf("here%d,,,,,%d\n",i,j);
//			int c=needed_pages;
//			bool ok=0;
//			while(j+needed_pages<my_pages){
//				///cprintf("here%d,,,,,%u\n",i);
//				if (page_allocator_arr2[j].allocated==0)
//					c--;
//				else{
//					//cprintf("poi%d,,,,,%u\n",i);
//
//					break;}
//				if(c==0){
//					ok=1;
//					break;
//				}
//				j++;
//			}
//			//cprintf("hedasafsdfaasdfgdasgdsfgsdfgdsfhdfhdfsre%d,,,,,%u\n",i);
//			if(ok){
//				//cprintf("MAN:%d and %d",i,AvilableSTART); max shares test
//				page_allocator_arr2[(i-AvilableSTART)/PAGE_SIZE].start=i;
//				page_allocator_arr2[(i-AvilableSTART)/PAGE_SIZE].numpages=needed_pages;
//				int j=((i-AvilableSTART)/PAGE_SIZE);
//				int to =j+needed_pages+1;
//				int c=needed_pages;
//
//				while(j<my_pages){
//								if (page_allocator_arr2[j].allocated==0){
//									c--;
//									page_allocator_arr2[j].allocated=1;
//									page_allocator_arr2[j].start=i;
//									page_allocator_arr2[j].numpages=needed_pages;//useless
//								}
//								if(c==0){
//									break;
//								}
//								j++;
//							}
//				struct FrameInfo *ptr_frame_info;
//				uint32 *ptr_table = NULL;
//				uint32 va=i;
//				uint32 index=va-AvilableSTART;
//				index/=PAGE_SIZE;
//for(uint32 k=0;k<needed_pages;k++){
//	//cprintf("herehereee%d?=%d",i,AvilableSTART);
//	ptr_table = NULL;
//	ptr_frame_info = get_frame_info(ptr_page_directory, va, &ptr_table);
//	if (ptr_frame_info != NULL){
//  	//	cprintf("==ELFRAME MLIAN ==,,%d",i);
//     return NULL;}
//
//		int ret = allocate_frame(&ptr_frame_info) ;
//		if(ret!=0){
//			free_frame(ptr_frame_info);
//			for(uint32 g=0;g<needed_pages;g++){
//				page_allocator_arr2[index].allocated=0;
//				index++;
//			}
//		 // cprintf("couldnt allocate");//s
//		 return NULL;}
//
//	ret= map_frame(ptr_page_directory, ptr_frame_info, va, PERM_WRITEABLE);
//	if(ret!=0){
//		unmap_frame(ptr_page_directory, va);
//		for(uint32 g=0;g<needed_pages;g++){
//						page_allocator_arr2[index].allocated=0;
//						index++;
//					  	//	cprintf("==ELFRAME MLIAN ==,,%d",i);
//
//					}
//     return NULL;}
//
//	//get frame number and save the va to return it in ~O(1)-M
//	frame_to_page[	kheap_physical_address(va)/PAGE_SIZE]=va;
////	if(kheap_physical_address(va)/PAGE_SIZE==1015813)
////			cprintf("free\n");
//
//	//INCREMENT THE ADDRESS
//    va+=PAGE_SIZE;
//}
////return the start address if ok
//     return (void*)i;
// }
//			//increment the outter loop
//
//			i+=PAGE_SIZE;
//		}
//		//end of the size checking condition
//		return (void*)NULL;
//
//		}
//		else{
//  		  //cprintf("REQUIRED SIZED IS BIG");// COMMENT IT LATER
//
//			return (void*)NULL;
//		}

		// O(N)
		int needed_pages=ROUNDUP(size,PAGE_SIZE)/PAGE_SIZE;
			int my_pages=AvilableSPACE/PAGE_SIZE;
			//cprintf("here%d,,,,,%u\n",my_pages*my_pages);
			if(ROUNDUP(size,PAGE_SIZE)<(AvilableSPACE)){
				int cnt=0;
				uint32 foundi=0;
				bool ok=0;
				unsigned int i;
			for( i=(AvilableSTART);i<KERNEL_HEAP_MAX;){
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
					struct FrameInfo *ptr_frame_info;
					uint32 *ptr_table = NULL;
					uint32 va=i;
					uint32 index=va-AvilableSTART;
					index/=PAGE_SIZE;
	for(uint32 k=0;k<needed_pages;k++){
		//cprintf("herehereee%d?=%d",i,AvilableSTART);
		ptr_table = NULL;
		ptr_frame_info = get_frame_info(ptr_page_directory, va, &ptr_table);
		if (ptr_frame_info != NULL){
	  	//	cprintf("==ELFRAME MLIAN ==,,%d",i);
	     return NULL;}

			int ret = allocate_frame(&ptr_frame_info) ;
			if(ret!=0){
				free_frame(ptr_frame_info);
				for(uint32 g=0;g<needed_pages;g++){
					page_allocator_arr2[index].allocated=0;
					index++;
				}
			 // cprintf("couldnt allocate");//s
			 return NULL;}

		ret= map_frame(ptr_page_directory, ptr_frame_info, va, PERM_WRITEABLE);
		if(ret!=0){
			unmap_frame(ptr_page_directory, va);
			for(uint32 g=0;g<needed_pages;g++){
							page_allocator_arr2[index].allocated=0;
							index++;
						  	//	cprintf("==ELFRAME MLIAN ==,,%d",i);
							//DONT FORGET TO REMOVE FROM THE ARRAY FRAME TO PAGE BUT..
							//THIS CONDITION SHOULDNT HAPPEN ITS JUST FOR DEBUGGING

						}
	     return NULL;}
		//	if(kheap_physical_address(va)/PAGE_SIZE==1015813)
			//			cprintf("free\n");
		//get frame number and save the va to return it in ~O(1)-M
		ptr_frame_info->va=va;
		frame_to_page[kheap_physical_address(va)/PAGE_SIZE]=va;


		//INCREMENT THE ADDRESS
	    va+=PAGE_SIZE;
	}
	//return the start address if ok
	// cprintf("%u ancscdscsdd  %u\n");//s

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
}

void kfree(void* virtual_address)
{
	//TODO: [PROJECT'23.MS2 - #04] [1] KERNEL HEAP - kfree()
	//refer to the project presentation and documentation for details
	// Write your code here, remove the panic and write your code
	// JUST COMMENTS
	if((uint32)virtual_address<=Hard_LIMIT){
		frame_to_page[kheap_physical_address((uint32)virtual_address)/PAGE_SIZE]=0;
		free_block(virtual_address);
	//	cprintf("here%d,,,,,%u\n");

	}
	else{
	unsigned int AvilableSTART=Hard_LIMIT+PAGE_SIZE;
	uint32 cur=(uint32)virtual_address;
	cur=((cur-AvilableSTART)/PAGE_SIZE);
	uint32 from=(page_allocator_arr2[cur].start-AvilableSTART);
	uint32 address=page_allocator_arr2[cur].start;
	from/=PAGE_SIZE;
	uint32 HOW_MANY=page_allocator_arr2[from].numpages;
	struct FrameInfo *ptr_frame_info;
	struct FrameInfo *ptr2;
    uint32 *ptr_table = NULL;
	for(int i=0;i<HOW_MANY;i++,from++){
		page_allocator_arr2[from].allocated=0;
		page_allocator_arr2[from].numpages=0;
		page_allocator_arr2[from].start=0;
		ptr_table = NULL;
		ptr_frame_info = get_frame_info(ptr_page_directory, address, &ptr_table);
//		ptr2=to_frame_info(268410885);
//		bool why=0;
//		if(ptr2==ptr_frame_info){
//			why=1;
//			cprintf("a2fsh\n");}
		free_frame(ptr_frame_info);
		unmap_frame(ptr_page_directory,address);
        frame_to_page[kheap_physical_address(address)/PAGE_SIZE]=0;

		address+=PAGE_SIZE;

	}}


}

unsigned int kheap_virtual_address(unsigned int physical_address)
{
	//EFFICIENT IMPLEMENTATION ~O(1) IS REQUIRED ==================
	// HAT EL FRAME INFO AND CHECK IF ITS NULL RETURN 0; XX
	// old solution : getting my va from array
		// new -> struct el frame shail el val
	   bool found=1;
unsigned int  temp=physical_address;
unsigned int  offset=temp&0x00000FFF;
physical_address-=offset;
struct FrameInfo *ptr_frame_info=to_frame_info(physical_address);
//      uint32 va=frame_to_page[physical_address];
//       unsigned int  add=(va+offset);
//       if(frame_to_page[physical_address]==0)
//	     found=0;
//     return ((add)*found);
if(ptr_frame_info==NULL)
	return 0;
uint32 va=ptr_frame_info->va;
if(ptr_frame_info->va!=0)
va+=offset;

return (va);

}

unsigned int kheap_physical_address(unsigned int virtual_address)
{

	uint32 *ptr_table = NULL;
	get_page_table(ptr_page_directory,virtual_address,&ptr_table);
	if(ptr_table==NULL)
		return 0;
	int index=PTX(virtual_address);
	uint32 offset=virtual_address&0x00000FFF;
	uint32 ret=ptr_table[index]&0xFFFFF000;
	ret|=offset;
	return ret;

}


void kfreeall()
{
	panic("Not implemented!");

}

void kshrink(uint32 newSize)
{
	panic("Not implemented!");
}

void kexpand(uint32 newSize)
{
	panic("Not implemented!");
}




//=================================================================================//
//============================== BONUS FUNCTION ===================================//
//=================================================================================//
// krealloc():

//	Attempts to resize the allocated space at "virtual_address" to "new_size" bytes,
//	possibly moving it in the heap.
//	If successful, returns the new virtual_address, in which case the old virtual_address must no longer be accessed.
//	On failure, returns a null pointer, and the old virtual_address remains valid.

//	A call with virtual_address = null is equivalent to kmalloc().
//	A call with new_size = zero is equivalent to kfree().

void *krealloc(void *virtual_address, uint32 new_size)
{
	//TODO: [PROJECT'23.MS2 - BONUS#1] [1] KERNEL HEAP - krealloc()
	// Write your code here, remove the panic and write your code
	if(virtual_address==NULL){
		return kmalloc(new_size);}
	if((uint32)virtual_address<=Hard_LIMIT){
		return realloc_block_FF(virtual_address,new_size);
	}
	if(new_size==0){
		kfree(virtual_address);
	}
	uint32 AvilableSTART=Hard_LIMIT+PAGE_SIZE;//132100096 NOPE

	int j=(((uint32)virtual_address-(AvilableSTART))/PAGE_SIZE);
	uint32 mastart=page_allocator_arr2[j].start;
	j=((page_allocator_arr2[j].start-(AvilableSTART))/PAGE_SIZE);
	uint32 pg=page_allocator_arr2[j].numpages;
	for(int i=j;i<pg;i++){
		page_allocator_arr2[i].allocated=0;
		page_allocator_arr2[i].numpages=0;
		page_allocator_arr2[i].start=0;
	}
	uint32 AvilableSPACE=(KERNEL_HEAP_MAX-(Hard_LIMIT+PAGE_SIZE));

	if(ROUNDUP(new_size,PAGE_SIZE)<AvilableSPACE){
		uint32 needed_pages=ROUNDUP(new_size,PAGE_SIZE)/PAGE_SIZE;
		int cnt=0;
						uint32 foundi=0;
						bool ok=0;
						unsigned int i;
					for( i=(Hard_LIMIT+PAGE_SIZE);i<KERNEL_HEAP_MAX;){
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
			if(ok){
				//reallocate
				kfree(virtual_address);

							//cprintf("MAN:%d and %d",i,AvilableSTART); max shares test
							page_allocator_arr2[(i-AvilableSTART)/PAGE_SIZE].start=i;
							page_allocator_arr2[(i-AvilableSTART)/PAGE_SIZE].numpages=needed_pages;
							int j=((i-AvilableSTART)/PAGE_SIZE);
							int to =j+needed_pages+1;
							int c=needed_pages;
							int my_pages=AvilableSPACE/PAGE_SIZE;


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
							struct FrameInfo *ptr_frame_info;
							uint32 *ptr_table = NULL;
							uint32 va=i;
							uint32 index=va-AvilableSTART;
							index/=PAGE_SIZE;
			for(uint32 k=0;k<needed_pages;k++){
				//cprintf("herehereee%d?=%d",i,AvilableSTART);
				ptr_table = NULL;
				ptr_frame_info = get_frame_info(ptr_page_directory, va, &ptr_table);
				if (ptr_frame_info != NULL){
			  	//	cprintf("==ELFRAME MLIAN ==,,%d",i);
					for(int k=j;k<pg;k++){
										page_allocator_arr2[k].allocated=1;
										page_allocator_arr2[k].numpages=pg;
										page_allocator_arr2[k].start=mastart;
									}
			     return NULL;}

					int ret = allocate_frame(&ptr_frame_info) ;
					if(ret!=0){
						free_frame(ptr_frame_info);
						for(uint32 g=0;g<needed_pages;g++){
							page_allocator_arr2[index].allocated=0;
							index++;
						}
						for(int k=j;k<pg;k++){
											page_allocator_arr2[k].allocated=1;
											page_allocator_arr2[k].numpages=pg;
											page_allocator_arr2[k].start=mastart;
										}
					 // cprintf("couldnt allocate");//s
					 return NULL;}

				ret= map_frame(ptr_page_directory, ptr_frame_info, va, PERM_WRITEABLE);
				if(ret!=0){
					unmap_frame(ptr_page_directory, va);
					for(uint32 g=0;g<needed_pages;g++){
									page_allocator_arr2[index].allocated=0;
									index++;
								  	//	cprintf("==ELFRAME MLIAN ==,,%d",i);
									//DONT FORGET TO REMOVE FROM THE ARRAY FRAME TO PAGE BUT..
									//THIS CONDITION SHOULDNT HAPPEN ITS JUST FOR DEBUGGING

								}
					for(int k=j;k<pg;k++){
										page_allocator_arr2[k].allocated=1;
										page_allocator_arr2[k].numpages=pg;
										page_allocator_arr2[k].start=mastart;
									}
			     return NULL;}
				//	if(kheap_physical_address(va)/PAGE_SIZE==1015813)
					//			cprintf("free\n");
				//get frame number and save the va to return it in ~O(1)-M
				ptr_frame_info->va=va;
				frame_to_page[kheap_physical_address(va)/PAGE_SIZE]=va;


				//INCREMENT THE ADDRESS
			    va+=PAGE_SIZE;
			}
			//return the start address if ok
			     return (void*)i;


			}
			else{
				//rg3 el marks
					for(int k=j;k<pg;k++){
						page_allocator_arr2[k].allocated=1;
						page_allocator_arr2[k].numpages=pg;
						page_allocator_arr2[k].start=mastart;
					}
				return NULL;

			}
	}





	return NULL;
	//panic("krealloc() is not implemented yet...!!");
}
