/*
 * fault_handler.c
 *
 *  Created on: Oct 12, 2022
 *      Author: HP
 */

#include "trap.h"
#include <kern/proc/user_environment.h>
#include "../cpu/sched.h"
#include "../disk/pagefile_manager.h"
#include "../mem/memory_manager.h"

//2014 Test Free(): Set it to bypass the PAGE FAULT on an instruction with this length and continue executing the next one
// 0 means don't bypass the PAGE FAULT
uint8 bypassInstrLength = 0;

//===============================
// REPLACEMENT STRATEGIES
//===============================
//2020
void setPageReplacmentAlgorithmLRU(int LRU_TYPE)
{
	assert(LRU_TYPE == PG_REP_LRU_TIME_APPROX || LRU_TYPE == PG_REP_LRU_LISTS_APPROX);
	_PageRepAlgoType = LRU_TYPE ;
}
void setPageReplacmentAlgorithmCLOCK(){_PageRepAlgoType = PG_REP_CLOCK;}
void setPageReplacmentAlgorithmFIFO(){_PageRepAlgoType = PG_REP_FIFO;}
void setPageReplacmentAlgorithmModifiedCLOCK(){_PageRepAlgoType = PG_REP_MODIFIEDCLOCK;}
/*2018*/ void setPageReplacmentAlgorithmDynamicLocal(){_PageRepAlgoType = PG_REP_DYNAMIC_LOCAL;}
/*2021*/ void setPageReplacmentAlgorithmNchanceCLOCK(int PageWSMaxSweeps){_PageRepAlgoType = PG_REP_NchanceCLOCK;  page_WS_max_sweeps = PageWSMaxSweeps;}

//2020
uint32 isPageReplacmentAlgorithmLRU(int LRU_TYPE){return _PageRepAlgoType == LRU_TYPE ? 1 : 0;}
uint32 isPageReplacmentAlgorithmCLOCK(){if(_PageRepAlgoType == PG_REP_CLOCK) return 1; return 0;}
uint32 isPageReplacmentAlgorithmFIFO(){if(_PageRepAlgoType == PG_REP_FIFO) return 1; return 0;}
uint32 isPageReplacmentAlgorithmModifiedCLOCK(){if(_PageRepAlgoType == PG_REP_MODIFIEDCLOCK) return 1; return 0;}
/*2018*/ uint32 isPageReplacmentAlgorithmDynamicLocal(){if(_PageRepAlgoType == PG_REP_DYNAMIC_LOCAL) return 1; return 0;}
/*2021*/ uint32 isPageReplacmentAlgorithmNchanceCLOCK(){if(_PageRepAlgoType == PG_REP_NchanceCLOCK) return 1; return 0;}

//===============================
// PAGE BUFFERING
//===============================
void enableModifiedBuffer(uint32 enableIt){_EnableModifiedBuffer = enableIt;}
uint8 isModifiedBufferEnabled(){  return _EnableModifiedBuffer ; }

void enableBuffering(uint32 enableIt){_EnableBuffering = enableIt;}
uint8 isBufferingEnabled(){  return _EnableBuffering ; }

void setModifiedBufferLength(uint32 length) { _ModifiedBufferLength = length;}
uint32 getModifiedBufferLength() { return _ModifiedBufferLength;}

//===============================
// FAULT HANDLERS
//===============================

//Handle the table fault
void table_fault_handler(struct Env * curenv, uint32 fault_va)
{
	//panic("table_fault_handler() is not implemented yet...!!");
	//Check if it's a stack page
	uint32* ptr_table;
#if USE_KHEAP
	{
		ptr_table = create_page_table(curenv->env_page_directory, (uint32)fault_va);
	}
#else
	{
		__static_cpt(curenv->env_page_directory, (uint32)fault_va, &ptr_table);
	}
#endif
}

//Handle the page fault

void page_fault_handler(struct Env * curenv, uint32 fault_va)
{

#if USE_KHEAP
		struct WorkingSetElement *victimWSElement = NULL;
		uint32 wsSize = LIST_SIZE(&(curenv->page_WS_list));
#else
		int iWS =curenv->page_last_WS_index;
		uint32 wsSize = env_page_ws_get_size(curenv);
#endif
		if(isPageReplacmentAlgorithmFIFO()){
	if(wsSize < (curenv->page_WS_max_size))
	{

		bool ok=(( fault_va >= USER_HEAP_START && fault_va < USER_HEAP_MAX)||( fault_va >= USTACKBOTTOM && fault_va < USTACKTOP));
		//cprintf("PLACEMENT=========================WS Size = %d\n", wsSize );
	struct FrameInfo *ptr_frame_info=NULL;
	allocate_frame(&ptr_frame_info);
	// down
	if(ptr_frame_info==NULL)
		panic("page_fault_handler()  :DDD...!! EE for tracing");
	map_frame(curenv->env_page_directory,ptr_frame_info,fault_va,PERM_WRITEABLE|PERM_USER);
	uint32 pf=pf_read_env_page(curenv,(void*)fault_va);

	if(pf==E_PAGE_NOT_EXIST_IN_PF&&ok==0){
		cprintf("[%08s] user5 PAGE fault va %08x\n", curenv->prog_name, fault_va);

		sched_kill_env(curenv->env_id);
	}
//	else if(pf==E_PAGE_NOT_EXIST_IN_PF&&ok==1){
	//JUST ADD TO THE WORKING SET
//	}

	struct WorkingSetElement *ptr_WS_element = env_page_ws_list_create_element(curenv,fault_va);//creaate element
	LIST_INSERT_TAIL(&(curenv->page_WS_list),ptr_WS_element); //add it to the list
	curenv->page_last_WS_index++;
	uint32 wsSize2 = LIST_SIZE(&(curenv->page_WS_list));
	if(wsSize2 < (curenv->page_WS_max_size))// if the list is not full el sahm 3la el mkan el next elfade(cold page fault)
		curenv->page_last_WS_element=NULL;
	else{
		// el list f al sahm rg3 aawl element

		curenv->page_last_WS_element=LIST_FIRST(&(curenv->page_WS_list));
		curenv->page_last_WS_index%=curenv->page_WS_max_size;}
	//cprintf("PLACEMENT=========================WS Size = %d\n", wsSize );



	}
	else
	{



		//cprintf("REPLACEMENT=========================WS Size = %d\n", wsSize );
		//refer to the project presentation and documentation for details
//			struct WorkingSetElement *eldoor=NULL;
//			int cnt=0;
//			LIST_FOREACH(eldoor, &(curenv->page_WS_list))
//				{
//				if(cnt==curenv->page_last_WS_index)
//				{
//					atrod=eldoor->virtual_address;
//				}
//
//				cnt++;
//
//				}
//			cprintf("REPLACEMENT=========================WS Size = %d\n", wsSize );
			/*if it exist it wont page fault :)*/
//			struct WorkingSetElement *ptr_sWS_element;
//			LIST_FOREACH(ptr_sWS_element, &(curenv->page_WS_list))
//					{
//						if (ROUNDDOWN(ptr_sWS_element->virtual_address, PAGE_SIZE) == ROUNDDOWN(fault_va,PAGE_SIZE))
//						{
//return;
//
//						}
//					}
		//cprintf("rererePLACEMENT=========================WS Size = %d\n", wsSize );


		bool put=0;
		if(curenv->page_last_WS_element==LIST_FIRST(&(curenv->page_WS_list))){
			put=1;
		}
		struct WorkingSetElement *before=curenv->page_last_WS_element->prev_next_info.le_prev;
			struct WorkingSetElement *atrod=curenv->page_last_WS_element;
			uint32 ok2=(pt_get_page_permissions(curenv->env_page_directory, (uint32)atrod->virtual_address)&PERM_MODIFIED);
			if(ok2){
				struct FrameInfo *ptr_frame_info;
				uint32 *ptr_table=NULL;
				ptr_frame_info=get_frame_info(curenv->env_page_directory,(uint32)atrod->virtual_address,&ptr_table);
//				cprintf("REPLACEMENT=========================WS Size = %d\n", wsSize );

				pf_update_env_page(curenv,(uint32)atrod->virtual_address,ptr_frame_info);
//				cprintf("REPLACEMENT=========================WS Size = %d\n", wsSize );

			}
//			if(curenv->page_last_WS_element==LIST_LAST(&(curenv->page_WS_list))){
//				curenv->page_last_WS_element=LIST_FIRST(&(curenv->page_WS_list));
//					}
//			else{
//				curenv->page_last_WS_element=LIST_NEXT(curenv->page_last_WS_element);
//
//			}
//			cprintf("REPLACEMENT=========================WS Size = %u and %u\n", ROUNDDOWN(atrod->virtual_address,PAGE_SIZE),ROUNDDOWN(fault_va,PAGE_SIZE) );

        	unmap_frame(curenv->env_page_directory, (uint32)atrod->virtual_address);
			env_page_ws_invalidate(curenv,(uint32)atrod->virtual_address);
			struct FrameInfo *ptr_frame_info;
			bool ok=(( fault_va >= USER_HEAP_START && fault_va < USER_HEAP_MAX)||( fault_va >= USTACKBOTTOM && fault_va < USTACKTOP));
			ptr_frame_info=NULL;
			allocate_frame(&ptr_frame_info);

			// down
			if(ptr_frame_info==NULL)
				panic("page_fault_handler()  :DDD...!! EE for tracing");
			map_frame(curenv->env_page_directory,ptr_frame_info,fault_va,PERM_WRITEABLE|PERM_USER);
			uint32 pf=pf_read_env_page(curenv,(void*)fault_va);

			if(pf==E_PAGE_NOT_EXIST_IN_PF&&ok==0){
				panic("page_fault_handler()  :DDD...!! EE for tracing");

				sched_kill_env(curenv->env_id);
			}
		//	else if(pf==E_PAGE_NOT_EXIST_IN_PF&&ok==1){
			//JUST ADD TO THE WORKING SET
		//	}

			struct WorkingSetElement *ptr_WS_element = env_page_ws_list_create_element(curenv,fault_va);
			/*TEST ARE A UPDATE YOU DONT HAVE TO MOVE THE ARROWS*/
//			if(put){
//				LIST_INSERT_HEAD(&(curenv->page_WS_list),ptr_WS_element);
//			}
//			else{
//				LIST_INSERT_AFTER(&(curenv->page_WS_list),before,ptr_WS_element);
//			}
			LIST_INSERT_TAIL(&(curenv->page_WS_list),ptr_WS_element);

			//TODO: [PROJECT'23.MS3 - #1] [1] PAGE FAULT HANDLER - FIFO Replacement
			// Write your code here, remove the panic and write your code
//			panic("page_fault_handler() FIFO Replacement is not implemented yet...!!");


	}

		}
		else{

			if(curenv->ActiveListSize+curenv->SecondListSize >LIST_SIZE(&(curenv->ActiveList))+LIST_SIZE(&(curenv->SecondList)) ){

				if(LIST_SIZE(&(curenv->ActiveList))<curenv->ActiveListSize){
				//	cprintf("NO WAYSSS :d :D=========================WS Size = %d\n", wsSize );


					bool ok=(( fault_va >= USER_HEAP_START && fault_va < USER_HEAP_MAX)||( fault_va >= USTACKBOTTOM && fault_va < USTACKTOP));
							//cprintf("PLACEMENT=========================WS Size = %d\n", wsSize );
						struct FrameInfo *ptr_frame_info=NULL;
						allocate_frame(&ptr_frame_info);
						// down
						if(ptr_frame_info==NULL)
							panic("page_fault_handler()  :DDD...!! EE for tracing");
						map_frame(curenv->env_page_directory,ptr_frame_info,fault_va,PERM_WRITEABLE|PERM_USER|PERM_PRESENT);
						uint32 pf=pf_read_env_page(curenv,(void*)fault_va);

						if(pf==E_PAGE_NOT_EXIST_IN_PF&&ok==0){
							//cprintf("[%08s] user5 PAGE fault va %08x\n", curenv->prog_name, fault_va);

							sched_kill_env(curenv->env_id);
						}

						struct WorkingSetElement *ptr_WS_element = env_page_ws_list_create_element(curenv,fault_va);//creaate element
						LIST_INSERT_HEAD(&(curenv->ActiveList),ptr_WS_element); //add it to the list


				}
				else{
					//cprintf("NO WAY :d :D=========================WS Size = %d\n", wsSize );

					env_page_ws_print(curenv);
					struct WorkingSetElement *ptr=NULL;
					bool hellya=0;
								LIST_FOREACH(ptr, &(curenv->SecondList))
									{
									if(ROUNDDOWN(ptr->virtual_address,PAGE_SIZE)==ROUNDDOWN(fault_va,PAGE_SIZE)){
										//cprintf("hell ya :D=========================WS Size = %d\n", wsSize );
										hellya=1;
										break;

									}

									}
								if(hellya){
									struct WorkingSetElement *atrod=ptr;
									//cprintf("hell ya NEEDED==WS Size = %d\n", ptr->virtual_address);

									LIST_REMOVE(&(curenv->SecondList),ptr );
									LIST_INSERT_HEAD(&(curenv->ActiveList),ptr);
									struct WorkingSetElement *pt=LIST_LAST(&(curenv->ActiveList));
									LIST_REMOVE(&(curenv->ActiveList),pt );
									LIST_INSERT_HEAD(&(curenv->SecondList),pt);
									 pt_set_page_permissions(curenv->env_page_directory,LIST_FIRST(&(curenv->ActiveList))->virtual_address,PERM_PRESENT,0);
										 pt_set_page_permissions(curenv->env_page_directory,LIST_FIRST(&(curenv->SecondList))->virtual_address,0,PERM_PRESENT);

										return;



								}

					bool ok=(( fault_va >= USER_HEAP_START && fault_va < USER_HEAP_MAX)||( fault_va >= USTACKBOTTOM && fault_va < USTACKTOP));
											//cprintf("PLACEMENT=========================WS Size = %d\n", wsSize );
										struct FrameInfo *ptr_frame_info=NULL;
										allocate_frame(&ptr_frame_info);
										// down
										if(ptr_frame_info==NULL)
											panic("page_fault_handler()  :DDD...!! EE for tracing");
										map_frame(curenv->env_page_directory,ptr_frame_info,fault_va,PERM_WRITEABLE|PERM_USER|PERM_PRESENT);
										uint32 pf=pf_read_env_page(curenv,(void*)fault_va);

										if(pf==E_PAGE_NOT_EXIST_IN_PF&&ok==0){
											//cprintf("[%08s] user5 PAGE fault va %08x\n", curenv->prog_name, fault_va);

											sched_kill_env(curenv->env_id);
										}

										struct WorkingSetElement *ptr_WS_element = env_page_ws_list_create_element(curenv,fault_va);//creaate element
										LIST_INSERT_HEAD(&(curenv->ActiveList),ptr_WS_element);
										struct WorkingSetElement *tr=LIST_LAST(&(curenv->ActiveList));
											LIST_REMOVE(&(curenv->ActiveList),tr );
										LIST_INSERT_HEAD(&(curenv->SecondList),tr);
//										cprintf("before=%d",LIST_SIZE(&(curenv->ActiveList)));
//
//										cprintf("[%08s] user5 AFTER REMVOE== fault va %08x\n", curenv->prog_name, fault_va);
//										cprintf("AFTER=%d",LIST_SIZE(&(curenv->ActiveList)));

										 pt_set_page_permissions(curenv->env_page_directory,LIST_FIRST(&(curenv->SecondList))->virtual_address,0,PERM_PRESENT);



//add it to the list

				}
			}
			else{
				struct WorkingSetElement *ptr=NULL;
								bool hellya=0;
											LIST_FOREACH(ptr, &(curenv->SecondList))
												{
												if(ROUNDDOWN(ptr->virtual_address,PAGE_SIZE)==ROUNDDOWN(fault_va,PAGE_SIZE)){
												//	cprintf("hell ya :D=========================WS Size = %d\n", wsSize );
													hellya=1;
													break;

												}

												}
											if(hellya){
												struct WorkingSetElement *atrod=ptr;
											//	cprintf("hell ya NEEDED==WS Size = %d\n", ptr->virtual_address);

												LIST_REMOVE(&(curenv->SecondList),ptr );
												LIST_INSERT_HEAD(&(curenv->ActiveList),ptr);
												struct WorkingSetElement *pt=LIST_LAST(&(curenv->ActiveList));
												LIST_REMOVE(&(curenv->ActiveList),pt );
												LIST_INSERT_HEAD(&(curenv->SecondList),pt);
												 pt_set_page_permissions(curenv->env_page_directory,LIST_FIRST(&(curenv->ActiveList))->virtual_address,PERM_PRESENT,0);
													 pt_set_page_permissions(curenv->env_page_directory,LIST_FIRST(&(curenv->SecondList))->virtual_address,0,PERM_PRESENT);

													return;



											}
											struct WorkingSetElement *atrod=LIST_LAST(&(curenv->SecondList) );
;
														uint32 ok2=(pt_get_page_permissions(curenv->env_page_directory, (uint32)atrod->virtual_address)&PERM_MODIFIED);
														if(ok2){
															struct FrameInfo *ptr_frame_info;
															uint32 *ptr_table=NULL;
															ptr_frame_info=get_frame_info(curenv->env_page_directory,(uint32)atrod->virtual_address,&ptr_table);
											//				cprintf("REPLACEMENT=========================WS Size = %d\n", wsSize );

															pf_update_env_page(curenv,(uint32)atrod->virtual_address,ptr_frame_info);
											//				cprintf("REPLACEMENT=========================WS Size = %d\n", wsSize );

														}
														unmap_frame(curenv->env_page_directory, (uint32)atrod->virtual_address);
														env_page_ws_invalidate(curenv,(uint32)atrod->virtual_address);
											bool ok=(( fault_va >= USER_HEAP_START && fault_va < USER_HEAP_MAX)||( fault_va >= USTACKBOTTOM && fault_va < USTACKTOP));
																						//cprintf("PLACEMENT=========================WS Size = %d\n", wsSize );
																					struct FrameInfo *ptr_frame_info=NULL;
																					allocate_frame(&ptr_frame_info);
																					// down
																					if(ptr_frame_info==NULL)
																						panic("page_fault_handler()  :DDD...!! EE for tracing");
																					map_frame(curenv->env_page_directory,ptr_frame_info,fault_va,PERM_WRITEABLE|PERM_USER|PERM_PRESENT);
																					uint32 pf=pf_read_env_page(curenv,(void*)fault_va);

																					if(pf==E_PAGE_NOT_EXIST_IN_PF&&ok==0){
																						//cprintf("[%08s] user5 PAGE fault va %08x\n", curenv->prog_name, fault_va);

																						sched_kill_env(curenv->env_id);
																					}

																					struct WorkingSetElement *ptr_WS_element = env_page_ws_list_create_element(curenv,fault_va);//creaate element
																					LIST_INSERT_HEAD(&(curenv->ActiveList),ptr_WS_element);
																					struct WorkingSetElement *tr=LIST_LAST(&(curenv->ActiveList));
																						LIST_REMOVE(&(curenv->ActiveList),tr );
																					LIST_INSERT_HEAD(&(curenv->SecondList),tr);
											//										cprintf("before=%d",LIST_SIZE(&(curenv->ActiveList)));
											//
											//										cprintf("[%08s] user5 AFTER REMVOE== fault va %08x\n", curenv->prog_name, fault_va);
											//										cprintf("AFTER=%d",LIST_SIZE(&(curenv->ActiveList)));

																					 pt_set_page_permissions(curenv->env_page_directory,LIST_FIRST(&(curenv->SecondList))->virtual_address,0,PERM_PRESENT);



			}

		}

}

void __page_fault_handler_with_buffering(struct Env * curenv, uint32 fault_va)
{
	panic("this function is not required...!!");
}



