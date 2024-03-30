/*
 * dynamic_allocator.c
 *
 *  Created on: Sep 21, 2023
 *      Author: HP
 */
#include <inc/assert.h>
#include <inc/string.h>
#include "../inc/dynamic_allocator.h"
struct MemBlock_LIST BLOCKS;
bool is_initialized=0;
//=====================================================
// 1) GET BLOCK SIZE (including size of its meta data):
//=====================================================
uint32 get_block_size(void* va)
{
	struct BlockMetaData *curBlkMetaData = ((struct BlockMetaData *)va - 1) ;
	return curBlkMetaData->size ;
}

//===========================
// 2) GET BLOCK STATUS:
//===========================
int8 is_free_block(void* va)
{
	struct BlockMetaData *curBlkMetaData = ((struct BlockMetaData *)va - 1) ;
	return curBlkMetaData->is_free ;
}

//===========================================
// 3) ALLOCATE BLOCK BASED ON GIVEN STRATEGY:
//===========================================
void *alloc_block(uint32 size, int ALLOC_STRATEGY)
{
	void *va = NULL;
	switch (ALLOC_STRATEGY)
	{
	case DA_FF:
		va = alloc_block_FF(size);
		break;
	case DA_NF:
		va = alloc_block_NF(size);
		break;
	case DA_BF:
		va = alloc_block_BF(size);
		break;
	case DA_WF:
		va = alloc_block_WF(size);
		break;
	default:
		cprintf("Invalid allocation strategy\n");
		break;
	}
	return va;
}

//===========================
// 4) PRINT BLOCKS LIST:
//===========================

void print_blocks_list(struct MemBlock_LIST list)
{
	cprintf("=========================================\n");
	struct BlockMetaData* blk ;
	cprintf("\nDynAlloc Blocks List:\n");
	LIST_FOREACH(blk, &list)
	{
		cprintf("(size: %d, isFree: %d)\n", blk->size, blk->is_free) ;
	}
	cprintf("=========================================\n");

}

//==================================
// [1] INITIALIZE DYNAMIC ALLOCATOR:
//==================================
uint32 dynamicAllocatorSize = 0;
void* dynamicAllocatorStart = NULL;
struct BlockMetaData* firstBlock = NULL;

void initialize_dynamic_allocator(uint32 daStart, uint32 initSizeOfAllocatedSpace)
{


	if (initSizeOfAllocatedSpace == 0)
		return ;
	is_initialized=1;
	dynamicAllocatorSize=initSizeOfAllocatedSpace;
    struct BlockMetaData* FirstMetaDatalBlock = (struct BlockMetaData*)daStart;
    FirstMetaDatalBlock->size = initSizeOfAllocatedSpace;
    FirstMetaDatalBlock->is_free = 1;

LIST_INSERT_HEAD(&BLOCKS,FirstMetaDatalBlock);

}

//=========================================
// [4] ALLOCATE BLOCK BY FIRST FIT:
//=========================================
void *alloc_block_FF(uint32 size)
{
	/* SO USING WHILE LOOP INSTEAD OF FOREACH DOSENT WORK*/
	/* WHY? I HAVE NO IDEA :)))))))))))))))*/
	struct BlockMetaData* CurrentBlock;
//	uint32 required_size = size + sizeOfMetaData();
	if(size==0)
		return NULL;
	if (!is_initialized)
	{
	uint32 required_size = size + sizeOfMetaData();
	uint32 da_start = (uint32)sbrk(required_size);
	//get new break since it's page aligned! thus, the size can be more than the required one
	uint32 da_break = (uint32)sbrk(0);
	initialize_dynamic_allocator(da_start, da_break - da_start);
	}

	LIST_FOREACH(CurrentBlock,&BLOCKS){

		if(CurrentBlock->is_free==1&&(CurrentBlock->size>=size+sizeof(struct BlockMetaData))){
			CurrentBlock->is_free=0;
		//	bool ok=0;

			if(CurrentBlock->size>=size+(2*sizeof(struct BlockMetaData))){
				struct BlockMetaData* NwDataBlock=(struct BlockMetaData*)((uint32)CurrentBlock+sizeof(struct BlockMetaData)+size);
				int actual_old_size=CurrentBlock->size-sizeof(struct BlockMetaData);
				NwDataBlock->is_free=1;
				NwDataBlock->size=actual_old_size-size;
				CurrentBlock->size=size+sizeof(struct BlockMetaData);
			//ok=1;
				LIST_INSERT_AFTER(&BLOCKS,CurrentBlock,NwDataBlock);
			}

			return (void*)((uint32)CurrentBlock+sizeof(struct BlockMetaData));
		}
	}
	uint32* sbrkPOINTER=(uint32*)sbrk(size+sizeof(struct BlockMetaData));
	uint32* no=(uint32*)-1;
	if(sbrkPOINTER==no){
		return NULL;
	}
	uint32 a=ROUNDUP(size+sizeof(struct BlockMetaData),PAGE_SIZE);
	a=a-(size+sizeof(struct BlockMetaData));
	struct BlockMetaData* sbrked = (struct BlockMetaData *)((uint32)sbrkPOINTER);
			sbrked->size = size+sizeof(struct BlockMetaData);
			sbrked->is_free = 0;
	if(a>sizeof(struct BlockMetaData)){

	//	cprintf("\nDynAlloc Blocks List:%u\n",a);

		struct BlockMetaData* NwDataBlock=(struct BlockMetaData*)((uint32)sbrkPOINTER+size+sizeof(struct BlockMetaData));

						NwDataBlock->is_free=1;
						NwDataBlock->size=a;
						LIST_INSERT_TAIL(&BLOCKS,sbrked);
						LIST_INSERT_TAIL(&BLOCKS,NwDataBlock);


	}
	else{
		sbrked->size=size+sizeof(struct BlockMetaData)+a;
		LIST_INSERT_TAIL(&BLOCKS,sbrked);
	}
	//print_blocks_list(BLOCKS);
	return (void*)((uint32)sbrked+sizeof(struct BlockMetaData));
}

//=========================================
// [5] ALLOCATE BLOCK BY BEST FIT:
//=========================================
void *alloc_block_BF(uint32 size)
{
	//TODO: [PROJECT'23.MS1 - BONUS] [3] DYNAMIC ALLOCATOR - alloc_block_BF()
// INT_MAXIMUM THEN MINIMIZE WITH FOR EACH TO LOCATE THE BEST BLOCK
	//THEN COPY PASTE THE FIRST BLOCK
	int minimize=100000000;
	struct BlockMetaData* BestFit=NULL;
	struct BlockMetaData* CurrentBlock= LIST_FIRST(&BLOCKS);

	panic("alloc_block_BF is not implemented yet");
	return NULL;
}
void *alloc_block_WF(uint32 size){
	panic("alloc_block_WF is not implemented yet");
		return NULL;
}
//=========================================
// [7] ALLOCATE BLOCK BY NEXT FIT:
//=========================================
void *alloc_block_NF(uint32 size)
{
	panic("alloc_block_NF is not implemented yet");
	return NULL;
}


//===================================================
// [8] FREE BLOCK WITH COALESCING:
//===================================================
void free_block(void *va)
{
	if(va==NULL){
		return;
	}
	struct BlockMetaData *cur = (struct BlockMetaData *)va - 1;
	struct BlockMetaData *previous=LIST_PREV(cur) ;
	struct BlockMetaData *nxt=LIST_NEXT(cur) ;
			cur->is_free=1;
	if(nxt!=NULL&&nxt->is_free){
		cur->size+=nxt->size;
		nxt->is_free=0;
		nxt->size=0;
		LIST_REMOVE(&BLOCKS,nxt);
	}
	if(previous!=NULL&&previous->is_free){
			previous->size+=cur->size;
			cur->is_free=0;
			cur->size=0;
			LIST_REMOVE(&BLOCKS,cur);
		}
	//panic(pfree_block is not implemented yet");
}

//=========================================
// [4] REALLOCATE BLOCK BY FIRST FIT:
//=========================================
void *realloc_block_FF(void* va, uint32 new_size)
{

	//TODO: [PROJECT'23.MS1 - #8] [3] DYNAMIC ALLOCATOR - realloc_block_FF()
	if(va!=NULL&&new_size==0){
		free_block(va);
		return NULL;
	}
	if(va==NULL&&new_size==0){
		return NULL;
	}
	if(va==NULL&&new_size!=0){
			return alloc_block_FF(new_size);
		}
	struct BlockMetaData *cur = (struct BlockMetaData *)va - 1;
	int OSIZE=cur->size-sizeof(struct BlockMetaData);
	if(new_size+sizeof( struct BlockMetaData)<cur->size){
		int total=cur->size;
		cur->size=cur->size-sizeof( struct BlockMetaData)-new_size;
		cur->size=total-cur->size;
		int diffrence=total-sizeof( struct BlockMetaData)-new_size;
		struct BlockMetaData *nxt = LIST_NEXT(cur);
		 if(nxt!=NULL&&nxt->is_free==0){
					if(diffrence<sizeof( struct BlockMetaData)){
						cur->size+=diffrence;
						return va;
					}
					else{
						uint32 newAddress=((uint32)va+new_size);
						struct BlockMetaData *addnew = (struct BlockMetaData *)newAddress;
						addnew->is_free=1;
						addnew->size=diffrence;
						LIST_INSERT_AFTER(&BLOCKS,cur,addnew);
						return va;
					}

				}
		 else if(nxt!=NULL&&nxt->is_free==1){
			nxt->size=nxt->size+diffrence;
			nxt=va+new_size;
			//LIST_NEXT(LIST_NEXT(cur)).prev_next_info.le_prev=nxt;;
			return va;

		}


	}
	OSIZE=cur->size-sizeof(struct BlockMetaData);
	if(OSIZE==new_size){
		return va;
	}
	struct BlockMetaData *nxt = LIST_NEXT(cur);
				if(nxt!=NULL&&nxt->is_free==1){
					cur->size+=nxt->size;
					nxt->size=0;
					nxt->is_free=0;

				}

	if(new_size-sizeof( struct BlockMetaData)<=cur->size){
		int total=cur->size;
			cur->size=cur->size-sizeof( struct BlockMetaData)-new_size;
			cur->size=total-cur->size;
			int diffrence=total-sizeof( struct BlockMetaData)-new_size;
			if(diffrence<sizeof( struct BlockMetaData)){
							cur->size=new_size+sizeof( struct BlockMetaData);
							return va;
						}
						else{
							uint32 newAddress=((uint32)va+new_size); //MIGHT CHANGE
							struct BlockMetaData *addnew = (struct BlockMetaData *)newAddress;
							addnew->is_free=1;
							addnew->size=diffrence;
							LIST_INSERT_AFTER(&BLOCKS,cur,addnew);
							cur->size=new_size+sizeof( struct BlockMetaData);
							return va;
						}

	}
	else{
		free_block(va);
	return	alloc_block_FF(new_size);
	}

	return NULL;
}
