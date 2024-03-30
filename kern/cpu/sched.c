#include "sched.h"

#include <inc/assert.h>

#include <kern/proc/user_environment.h>
#include <kern/trap/trap.h>
#include <kern/mem/kheap.h>
#include <kern/mem/memory_manager.h>
#include <kern/tests/utilities.h>
#include <kern/cmd/command_prompt.h>
uint32 isSchedMethodRR(){if(scheduler_method == SCH_RR) return 1; return 0;}
uint32 isSchedMethodMLFQ(){if(scheduler_method == SCH_MLFQ) return 1; return 0;}
uint32 isSchedMethodBSD(){if(scheduler_method == SCH_BSD) return 1; return 0;}


//===================================================================================//
//============================ SCHEDULER FUNCTIONS ==================================//
//===================================================================================//

//===================================
// [1] Default Scheduler Initializer:
//===================================
void sched_init()
{
	old_pf_counter = 0;

	sched_init_RR(INIT_QUANTUM_IN_MS);

	init_queue(&env_new_queue);
	init_queue(&env_exit_queue);
	scheduler_status = SCH_STOPPED;
}

//=========================
// [2] Main FOS Scheduler:
//=========================
void
fos_scheduler(void)
{
	//	cprintf("inside scheduler\n");

	chk1();
	scheduler_status = SCH_STARTED;

	//This variable should be set to the next environment to be run (if any)
	struct Env* next_env = NULL;

	if (scheduler_method == SCH_RR)
	{
		// Implement simple round-robin scheduling.
		// Pick next environment from the ready queue,
		// and switch to such environment if found.
		// It's OK to choose the previously running env if no other env
		// is runnable.

		//If the curenv is still exist, then insert it again in the ready queue
		if (curenv != NULL)
		{
			enqueue(&(env_ready_queues[0]), curenv);
		}

		//Pick the next environment from the ready queue
		next_env = dequeue(&(env_ready_queues[0]));

		//Reset the quantum
		//2017: Reset the value of CNT0 for the next clock interval
		kclock_set_quantum(quantums[0]);
		//uint16 cnt0 = kclock_read_cnt0_latch() ;
		//cprintf("CLOCK INTERRUPT AFTER RESET: Counter0 Value = %d\n", cnt0 );

	}
	else if (scheduler_method == SCH_MLFQ)
	{
		next_env = fos_scheduler_MLFQ();
	}
	else if (scheduler_method == SCH_BSD)
	{
		//cprintf("hereeeeeeeeeeeeeeeeeeebsd\n");

		next_env = fos_scheduler_BSD();
	}
	//temporarily set the curenv by the next env JUST for checking the scheduler
	//Then: reset it again
	struct Env* old_curenv = curenv;
	curenv = next_env ;
	chk2(next_env) ;
	curenv = old_curenv;

	//sched_print_all();
	//cprintf("hereeeeeeeeeeeeeeeeeee sched3\n");
	if(next_env != NULL)
	{
		//		cprintf("\nScheduler select program '%s' [%d]... counter = %d\n", next_env->prog_name, next_env->env_id, kclock_read_cnt0());
		//		cprintf("Q0 = %d, Q1 = %d, Q2 = %d, Q3 = %d\n", queue_size(&(env_ready_queues[0])), queue_size(&(env_ready_queues[1])), queue_size(&(env_ready_queues[2])), queue_size(&(env_ready_queues[3])));
		env_run(next_env);
		//cprintf("hereeeeeeeeeeeeeeeeeee sched4\n");
	}
	else
	{
		/*2015*///No more envs... curenv doesn't exist any more! return back to command prompt
		curenv = NULL;
		//lcr3(K_PHYSICAL_ADDRESS(ptr_page_directory));
		lcr3(phys_page_directory);

		//cprintf("SP = %x\n", read_esp());

		scheduler_status = SCH_STOPPED;
		//cprintf("[sched] no envs - nothing more to do!\n");
		while (1)
			run_command_prompt(NULL);

	}
}

//=============================
// [3] Initialize RR Scheduler:
//=============================
void sched_init_RR(uint8 quantum)
{

	// Create 1 ready queue for the RR
	num_of_ready_queues = 1;
#if USE_KHEAP
	sched_delete_ready_queues();
	env_ready_queues = kmalloc(sizeof(struct Env_Queue));
	quantums = kmalloc(num_of_ready_queues * sizeof(uint8)) ;
#endif
	quantums[0] = quantum;
	kclock_set_quantum(quantums[0]);
	init_queue(&(env_ready_queues[0]));

	//=========================================
	//DON'T CHANGE THESE LINES=================
	scheduler_status = SCH_STOPPED;
	scheduler_method = SCH_RR;
	//=========================================
	//=========================================
}

//===============================
// [4] Initialize MLFQ Scheduler:
//===============================
void sched_init_MLFQ(uint8 numOfLevels, uint8 *quantumOfEachLevel)
{
#if USE_KHEAP
	//=========================================
	//DON'T CHANGE THESE LINES=================
	sched_delete_ready_queues();
	//=========================================
	//=========================================

	//=========================================
	//DON'T CHANGE THESE LINES=================
	scheduler_status = SCH_STOPPED;
	scheduler_method = SCH_MLFQ;
	//=========================================
	//=========================================
#endif
}

//===============================
// [5] Initialize BSD Scheduler:
//===============================
void sched_init_BSD(uint8 numOfLevels, uint8 quantum)
{
#if USE_KHEAP
 /*SAME AS ROUND ROBIN JUST WITH LEVELS */
	//sched_delete_ready_queues();

	loadavg=0;
	readypr=0;
	env_ready_queues = kmalloc(numOfLevels*sizeof(struct Env_Queue));
	quantums = kmalloc(1* sizeof(uint8)) ;
	kclock_set_quantum(quantum);
	quantums[0] = quantum;
	for(int i=0;i<numOfLevels;i++){

		init_queue(&env_ready_queues[i]);

		}
	 num_of_ready_queues = numOfLevels;

		//cprintf("FINISH init bsd SUCCESSFULLY\n");

	//=========================================
	//DON'T CHANGE THESE LINES=================
	scheduler_status = SCH_STOPPED;
	scheduler_method = SCH_BSD;
	//=========================================
	//=========================================
#endif
}


//=========================
// [6] MLFQ Scheduler:
//=========================
struct Env* fos_scheduler_MLFQ()
{
	panic("not implemented");
	return NULL;
}

//=========================
// [7] BSD Scheduler:
//=========================
struct Env* fos_scheduler_BSD()
{

//	for(int i=num_of_ready_queues-1;i>=0;i--){
//
//			if(LIST_SIZE(&env_ready_queues[i])!=0){
//				struct Env* ptr;
//									LIST_FOREACH(ptr, &(env_ready_queues[i]))
//									 {
//										cprintf("hereeeeeeeeeeeeeeeeeee %d\n",ptr->priority);
//
//
//									}
//			}
//		}


	if(curenv!=NULL){
		//cprintf("hereeeeeeeeeeeeeeeeeee %d\n",LIST_SIZE(&env_ready_queues[curenv->priority]));

		enqueue(&(env_ready_queues[curenv->priority]), curenv);
		//cprintf("hereeeeeeeeeeeeeeeeeee2 %d\n",LIST_SIZE(&env_ready_queues[curenv->priority]));
	}


	for(int i=num_of_ready_queues-1;i>=0;i--){

		if(LIST_SIZE(&env_ready_queues[i])!=0){
			kclock_set_quantum(quantums[0]);
			struct Env* next=dequeue(&(env_ready_queues[i]));
			//cprintf("NEXT ENV = %u\n",next);
			return next;

		}
	}
	loadavg=0;
	return NULL;
}

//========================================
// [8] Clock Interrupt Handler
//	  (Automatically Called Every Quantum)
//========================================
void clock_interrupt_handler()
{
	//TODO: [PROJECT'23.MS3 - #5] [2] BSD SCHEDULER - Your code is here
if(scheduler_method==SCH_BSD){

		fixed_point_t SIXTY=fix_int(60);
		fixed_point_t ONE =fix_int(1);
		fixed_point_t FIFTY=fix_int(59);

		// so ineed to updaate loaad avg and recent cpu and priority
		// load averge is global
		//recent is for every E every one second  (but every tick increment just the curenv)
		//priority for every e every 4 ticks RE SORT EVERY QUEUE IS A MUST ?  dont forget the running one
		// because it might get baack to the qu
		/*FINALLY :DDDDDDD the current env dosent have to be the highest PR after the priority modify :D*/
		 bool everytick=1;
		 bool everysecond=timer_ticks()%100;
		 bool every4ticks=(timer_ticks()%4);
//		 every4ticks=!every4ticks
		 if(everytick)
		curenv->recentcpu=curenv->recentcpu+(1<<14);

		if(everysecond==0){
			readypr=0;
			for(int i=num_of_ready_queues-1;i>=0;i--){
					readypr+=LIST_SIZE(&env_ready_queues[i]);
				}
			fixed_point_t temp=__mk_fix(loadavg);
			fixed_point_t a=fix_mul(fix_div(FIFTY,SIXTY),temp);
			fixed_point_t b= fix_scale(fix_div(ONE,SIXTY),readypr);
			temp=fix_add(a,b);
			loadavg=temp.f;


			long long t1=(long long)loadavg*2;
								long long t2=t1+(long long)(1<<14);
								long long ans=t1* (long long)(1<<14)/(t2);
								ans=ans*(long long)curenv->recentcpu/(long long)(1<<14);
								ans=ans+((long long )curenv->nicevalue*(long long)(1<<14));
							curenv->recentcpu=ans;
							//cprintf("before recent%u\n",loadavg);
			for(int i=num_of_ready_queues-1;i>=0;i--){
				if(LIST_SIZE(&env_ready_queues[i])!=0){
					struct Env* ptr;
					LIST_FOREACH(ptr, &(env_ready_queues[i]))
					 {
						long long t1=(long long)loadavg*2;
						long long t2=t1+(long long)(1<<14);
						long long ans=t1*(long long)(1<<14)/(t2);
						ans=ans*(long long)ptr->recentcpu/(long long)(1<<14);
						ans=ans+((long long )ptr->nicevalue*(long long)(1<<14));
					ptr->recentcpu=ans;
					}

				}

			}
		//	cprintf("after recent%u\n",loadavg);


		}
		if(every4ticks==0){
			//cprintf("before pri%u\n",loadavg);
			fixed_point_t temp=__mk_fix((long long) (curenv->recentcpu)/(long long)4);
			long long npr= (long long)PRI_MAX-(long long)fix_round(temp)-2*curenv->nicevalue;
			if(npr>PRI_MAX)
					npr=PRI_MAX;
						if(npr<PRI_MIN)
								npr=PRI_MIN;
			curenv->priority=npr;
			for(int i=num_of_ready_queues-1;i>=0;i--){
				if(LIST_SIZE(&env_ready_queues[i])!=0){
					struct Env* ptr;
										LIST_FOREACH(ptr, &(env_ready_queues[i]))
										 {
											fixed_point_t temp=__mk_fix((long long) (ptr->recentcpu)/(long long)4);
														long long npr= PRI_MAX-(long long)fix_round(temp)-(long long)2*ptr->nicevalue;
														if(npr>PRI_MAX)
															npr=PRI_MAX;
														if(npr<PRI_MIN)
															npr=PRI_MIN;
														ptr->priority=npr;

										}

				}
			}
			//cprintf("before moving pri%u\n",loadavg);
			for(int i=num_of_ready_queues-1;i>=0;i--){
						if(LIST_SIZE(&env_ready_queues[i])!=0){
							struct Env* ptr;
												LIST_FOREACH(ptr, &(env_ready_queues[i]))
												 {
												if(ptr->priority!=i){
													struct Env* me=ptr;
															LIST_REMOVE(&(env_ready_queues[i]),ptr);
													enqueue(&(env_ready_queues[i]),me);

												}

												}

						}
					}


		}

}





	/********DON'T CHANGE THIS LINE***********/
	ticks++ ;
	if(isPageReplacmentAlgorithmLRU(PG_REP_LRU_TIME_APPROX))
	{
		update_WS_time_stamps();
	}
	//cprintf("Clock Handler\n") ;
	fos_scheduler();
	/*****************************************/
}

//===================================================================
// [9] Update LRU Timestamp of WS Elements
//	  (Automatically Called Every Quantum in case of LRU Time Approx)
//===================================================================
void update_WS_time_stamps()
{
	struct Env *curr_env_ptr = curenv;

	if(curr_env_ptr != NULL)
	{
		struct WorkingSetElement* wse ;
		{
			int i ;
#if USE_KHEAP
			LIST_FOREACH(wse, &(curr_env_ptr->page_WS_list))
			{
#else
			for (i = 0 ; i < (curr_env_ptr->page_WS_max_size); i++)
			{
				wse = &(curr_env_ptr->ptr_pageWorkingSet[i]);
				if( wse->empty == 1)
					continue;
#endif
				//update the time if the page was referenced
				uint32 page_va = wse->virtual_address ;
				uint32 perm = pt_get_page_permissions(curr_env_ptr->env_page_directory, page_va) ;
				uint32 oldTimeStamp = wse->time_stamp;

				if (perm & PERM_USED)
				{
					wse->time_stamp = (oldTimeStamp>>2) | 0x80000000;
					pt_set_page_permissions(curr_env_ptr->env_page_directory, page_va, 0 , PERM_USED) ;
				}
				else
				{
					wse->time_stamp = (oldTimeStamp>>2);
				}
			}
		}

		{
			int t ;
			for (t = 0 ; t < __TWS_MAX_SIZE; t++)
			{
				if( curr_env_ptr->__ptr_tws[t].empty != 1)
				{
					//update the time if the page was referenced
					uint32 table_va = curr_env_ptr->__ptr_tws[t].virtual_address;
					uint32 oldTimeStamp = curr_env_ptr->__ptr_tws[t].time_stamp;

					if (pd_is_table_used(curr_env_ptr->env_page_directory, table_va))
					{
						curr_env_ptr->__ptr_tws[t].time_stamp = (oldTimeStamp>>2) | 0x80000000;
						pd_set_table_unused(curr_env_ptr->env_page_directory, table_va);
					}
					else
					{
						curr_env_ptr->__ptr_tws[t].time_stamp = (oldTimeStamp>>2);
					}
				}
			}
		}
	}
}

