#include "scheduling_simulator.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <signal.h>
#include <unistd.h>
#include <sys/time.h>
#include <ucontext.h>
#include "task.h"
char const* state_array[] = {"TASK_RUNNING", "TASK_READY", "TASK_WAITING", "TASK_TERMINATED"};
struct node{
	int pid;
	char task[100000];
	char time_q[100];
	char prior[100];
	struct node* next;
	enum TASK_STATE status;

};
//char stack = ;
static ucontext_t uc[40];
//set a ucontext 
struct node* find_pid_in_Q(struct node* ptr,int pid);
void alarm_signal(int signal_number);
struct itimerval t;
struct node* HQ_HEAD = NULL;
struct node* HQ_TAIL = NULL;
int HQ_number = 0;
struct node* LQ_HEAD = NULL;
struct node* LQ_TAIL = NULL;
int LQ_number = 0;
int Q_number = 0;
struct node* Q_HEAD = NULL;
struct node* Q_TAIL = NULL;
int tmp;
int running_flag = 0; 
void hw_suspend(int msec_10)
{
	//waiting is OK?
	//swap?
	/*
	struct node* tmp_Q_in_sus;
	tmp_Q_in_sus = find_pid_in_Q(HQ_HEAD,running_flag);
	tmp_Q_in_sus = TASK_WAITING;
	if(strcmp(tmp_Q_in_sus->prior,"H") == 0){
		struct node* tmp_HQ_in_sus;
		tmp_HQ_in_sus = find_pid_in_Q(HQ_HEAD,running_flag);
		tmp_HQ_in_sus->status = TASK_WAITING;
	}
	else{
		struct node* tmp_LQ_in_sus;
		tmp_LQ_in_sus = find_pid_in_Q(LQ_HEAD,running_flag);
		tmp_LQ_in_sus->status = TASK_WAITING;
	}
	*/
	//change state to TASK_WAITING

	//reschedule
	return;
}

void hw_wakeup_pid(int pid)
{
	//done
	struct node* tmp_Q_in_wake_pid;
	tmp_Q_in_wake_pid = find_pid_in_Q(HQ_HEAD,pid);
	tmp_Q_in_wake_pid->status = TASK_READY;
	if(strcmp(tmp_Q_in_wake_pid->prior,"H") == 0){
		struct node* tmp_HQ_in_wake_pid;
		tmp_HQ_in_wake_pid = find_pid_in_Q(HQ_HEAD,pid);
		tmp_HQ_in_wake_pid->status = TASK_READY;
	}
	else{
		struct node* tmp_LQ_in_wake_pid;
		tmp_LQ_in_wake_pid = find_pid_in_Q(LQ_HEAD,pid);
		tmp_LQ_in_wake_pid->status = TASK_READY;
	}
	//change the state task PID from TASK_WAITING to READY	
	return;
}

int hw_wakeup_taskname(char *task_name)
{
    //change state from WAITING to READY
	return 0;
}

int hw_task_create(char *task_name)
{
    
	//create task task_name
	//return pid of created task
	//return -1 if there is no function name task_name
	return 0; // the pid of created task name
}
//modify////////////////////////////////////////////////////////////////////////////////////////////////////
//1.High priority 2.Low priority 3.All
void HPush(char* name,char* time,char* priority)
{
    if(HQ_HEAD == NULL) {
		printf("push HQ\n");
        HQ_HEAD = (struct node*)malloc(sizeof(struct node));
		sprintf(HQ_HEAD->task,"%s",name);
		sprintf(HQ_HEAD->time_q,"%s",time);
		sprintf(HQ_HEAD->prior,"%s",priority);
		HQ_HEAD->status = TASK_READY;
        HQ_HEAD->pid = Q_number;
        HQ_HEAD->next = NULL;
        HQ_TAIL = HQ_HEAD;
    } else {
        struct node* ptr = (struct node*)malloc(sizeof(struct node));
		printf("push HQ\n");
        ptr->pid = Q_number;
		ptr->status = TASK_READY;
		printf("status %d\n",ptr->status);
		sprintf(ptr->task,"%s",name);
		sprintf(ptr->prior,"%s",priority);
		sprintf(ptr->time_q,"%s",time);		
        ptr->next = NULL;
        HQ_TAIL->next = ptr;
        HQ_TAIL = ptr;
    }
    HQ_number++;
}
//modify
int HPop(void)
{
    struct node* ptr = HQ_HEAD;
    int result = ptr->pid;
    HQ_HEAD = ptr->next;
    free(ptr);
    HQ_number--;
    return result;
}
int HisEmpty(void)
{
    if(HQ_number == 0) {
        return 1;
    } else {
        return 0;
    }
}
//Low priority
void LPush(char* name,char* time,char* priority)
{
    if(LQ_HEAD == NULL) {
		printf("push LQ\n");
        LQ_HEAD = (struct node*)malloc(sizeof(struct node));
		sprintf(LQ_HEAD->task,"%s",name);
		sprintf(LQ_HEAD->time_q,"%s",time);
		sprintf(LQ_HEAD->prior,"%s",priority);
		LQ_HEAD->status = TASK_READY;
        LQ_HEAD->pid = Q_number;
        LQ_HEAD->next = NULL;
        LQ_TAIL = LQ_HEAD;
    } else {
        struct node* ptr = (struct node*)malloc(sizeof(struct node));
        ptr->pid = Q_number;
		sprintf(ptr->task,"%s",name);
		sprintf(ptr->prior,"%s",priority);
		sprintf(ptr->time_q,"%s",time);	
		ptr->status = TASK_READY;	
        ptr->next = NULL;
        LQ_TAIL->next = ptr;
        LQ_TAIL = ptr;
    }
    LQ_number++;
}
int LPop(void)
{
    struct node* ptr = LQ_HEAD;
    int result = ptr->pid;
    LQ_HEAD = ptr->next;
    free(ptr);
    LQ_number--;
    return result;
}
int LisEmpty(void)
{
    if(LQ_number == 0) {
        return 1;
    } else {
        return 0;
    }
}
//////////////////////////////////////////////////////////////////////////////

void Push(char* name,char* time,char* priority)
{
    if(Q_HEAD == NULL) {
		printf("push %d\n",Q_number + 1);
        Q_HEAD = (struct node*)malloc(sizeof(struct node));
		sprintf(Q_HEAD->task,"%s",name);
		sprintf(Q_HEAD->time_q,"%s",time);	
		sprintf(Q_HEAD->prior,"%s",priority);
		Q_HEAD->status = TASK_READY;
        Q_HEAD->pid = Q_number + 1;
        Q_HEAD->next = NULL;
        Q_TAIL = Q_HEAD;
    } else {
        struct node* ptr = (struct node*)malloc(sizeof(struct node));

        ptr->pid = Q_number + 1;
		sprintf(ptr->task,"%s",name);
		sprintf(ptr->prior,"%s",priority);
		sprintf(ptr->time_q,"%s",time);	
		ptr->status = TASK_READY;	
		printf("status: %d \n",ptr->status);
        ptr->next = NULL;
        Q_TAIL->next = ptr;
        Q_TAIL = ptr;
    }
    Q_number++;
}
//modify
int Pop(void)
{
    struct node* ptr = Q_HEAD;
    int result = ptr->pid;
    Q_HEAD = ptr->next;
    free(ptr);
    Q_number--;
    return result;
}
int isEmpty(void)
{
    if(Q_number == 0) {
        return 1;
    } else {
        return 0;
    }
}
void printQ(struct node* HEAD){

	printf("%d   %s  %s   0   %s   %s\n",HEAD->pid,HEAD->task,state_array[HEAD->status],HEAD->prior,HEAD->time_q);
	tmp++;
	if(tmp<Q_number){
		printQ(HEAD->next);
	}
	else{
		return;
	}
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void alarm_signal(int signal_number){

	printf("Stop!\n");
	/*
	if(HisEmpty()){
		printf("Stop!\n");
	}
	else{
		int tmp_pop;
		tmp_pop = HPop();
		printf("Stop! %d\n",tmp_pop);
	}
	*/
	return;
}
void shell(){

	char input_char[1000000];
	char task_name[1000000];
	char prior[100];
	char time_quantum[100];
	char trash[1000000];

	while(1){

		char buff[50];
		memset(buff,0,sizeof(buff));
		memset(task_name,0,sizeof(task_name));
		memset(time_quantum,0,sizeof(time_quantum));
		memset(prior,0,sizeof(prior));
		memset(trash,0,sizeof(trash));
		memset(buff,0,sizeof(buff));
		printf("$");
		int size = 1024;
		memset(buff,0,sizeof(buff));
		
		fgets(buff,sizeof(buff),stdin);
		puts(buff);
		
		int count = 0;
		int prior_flag = 0;
		int time_flag = 0;

		if(buff[0] == 'a'){
			printf("this is a\n");
			strcat(prior,"L");
			strcat(time_quantum,"S");
			char* delim_for_n = "\n";
			char* pch_for_n = NULL;
			pch_for_n = strtok(buff,delim_for_n);

			char* delim = " ";
			char* pch = NULL;
			pch = strtok(pch_for_n,delim);
			while(pch != NULL){
				if(count == 1){
					sprintf(task_name,"%s",pch);
				}
				else if(prior_flag == 1){
					prior_flag = 0;
					memset(prior,0,sizeof(prior));
					sprintf(prior,"%s",pch);
				}
				else if(time_flag == 1){
					time_flag = 0;
					memset(time_quantum,0,sizeof(time_quantum));
					sprintf(time_quantum,"%s",pch);
				}
				else{
					if(pch[0] == '-'){
						if(pch[1] == 't'){
							time_flag = 1;
						}
						else if(pch[1] == 'p'){
							prior_flag = 1;
						}				
					}
				}
				pch = strtok(NULL,delim);
				count ++;
			}
			printf("task name:%s\nprior:%s\ntime:%s\n",task_name, prior, time_quantum);
			//push to Q
			Push(task_name,time_quantum,prior);
			if(strcmp(prior,"L")>= 0){
				printf("HERE L \n");
				LPush(task_name,time_quantum,prior);
			}
			else if(strcmp(prior,"H")>= 0){
				printf("HERE H \n");
				HPush(task_name,time_quantum,prior);
			}
		}
		else if(buff[0] == 'p'){
			tmp = 0;
			printQ(Q_HEAD);
		}
		else if(buff[0] == 'r'){


		}
		else if(buff[0] == 's'){
			//t.it_value.tv_sec = 1;
			//setitimer(ITIMER_REAL, &t, NULL);
			return;
		}
	}
}
void signal_handler(int signal_number){
	printf("\n Ctrl + Z :) \n");
	//t.it_value.tv_sec = 0;
	//setitimer(ITIMER_REAL, &t, NULL);intf("\n Ctrl + Z :) \n");
	//t.it_value.tv_sec 
	shell();
}
//modify////////////////////////////////////////////////////////////
struct node* check_status(struct node* ptr){
	//if no READY
	if(ptr->status == 1){
		return ptr;
	}
	else if(ptr->status == 2 || ptr->status == 3){
		check_status(ptr->next);
	}
}
struct node* find_pid_in_Q(struct node* ptr,int pid){
	if(ptr->pid == pid){
		return ptr;
	}
	else{
		find_pid_in_Q(ptr->next,pid);
	}
}
void settime(){

	t.it_interval.tv_sec = 1;
	t.it_interval.tv_usec = 0;
	t.it_value.tv_sec = 1;
	t.it_value.tv_usec = 0;
	printf("DONE\n");
	setitimer(ITIMER_REAL,&t,NULL);

}
int main()
{

	char *stack = (char*)malloc(1024);
	signal(SIGTSTP, signal_handler);
	signal(SIGALRM,alarm_signal);
	// if(signal(SIGALRM,alarm_signal)<0){
	// 	printf("FUCK\n");
	// }
	shell();

	
	getcontext(&uc[0]);
	uc[0].uc_link = NULL;
	uc[0].uc_stack.ss_sp = stack;
	uc[0].uc_stack.ss_size = sizeof(stack);
	for(int i=1;i<40;i++){
		getcontext(&uc[i]);
		uc[i].uc_link = &uc[0];
		uc[i].uc_stack.ss_sp = stack;
		uc[i].uc_stack.ss_size = sizeof(stack);
	}
	/*
	t.it_interval.tv_sec = 200;
	t.it_interval.tv_usec = 0;
	t.it_value.tv_sec = 1;
	t.it_value.tv_usec = 0;
	*/
	/*
	t.it_interval.tv_sec = 1;
	t.it_interval.tv_usec = 0;
	t.it_value.tv_sec = 1;
	t.it_value.tv_usec = 0;
	setitimer(ITIMER_REAL,&t,NULL);
	*/
	//setitimer(ITIMER_REAL,&t,NULL);

	while(1){
		//printf("HI\n");
		if(HQ_number == 0 && LQ_number == 0){
			shell();
			printf("lol\n");
		}
		//if high Q is not empty
		else if(!HisEmpty()){
			printf("HI H\n");
			struct node* tmp_for_HQ;
			tmp_for_HQ = check_status(HQ_HEAD);
			//while(1);
			if(strcmp(tmp_for_HQ->time_q,"L") ==0){
				
				//setitimer(ITIMER_REAL,&t,NULL);
				printf("I am L\n");
				running_flag = tmp_for_HQ->pid;
				tmp_for_HQ->status = TASK_RUNNING;
				struct node* tmp_for_Q;
				tmp_for_Q = find_pid_in_Q(Q_HEAD,tmp_for_HQ->pid);
				tmp_for_Q->status = TASK_RUNNING;
				//timer
				/*
				t.it_interval.tv_sec = 10;
				t.it_interval.tv_usec = 0;
				t.it_value.tv_sec = 1;
				t.it_value.tv_usec = 0;
				*/
				if(strcmp(tmp_for_HQ->task,"task1") == 0){
					printf("Hello 1 \n");
					settime();
					//setitimer(ITIMER_REAL,&t,NULL);
					makecontext(&uc[tmp_for_Q->pid], task1, 0);
					//while(1);

					printf("GO\n");
					swapcontext(&uc[0],&uc[tmp_for_Q->pid]);

					//task1();
					//if status != waiting or ready then terminated
					if(tmp_for_Q->status != 1|| tmp_for_Q->status != 2){		
						tmp_for_Q->status = TASK_TERMINATED;
						tmp_for_HQ->status = TASK_TERMINATED;
						int tmp_print;
						tmp_print = HPop();
						printf("yoyo %d\n",tmp_print);
					}	
				}
				else if(strcmp(tmp_for_HQ->task,"task2") == 0){
					printf("Hello 2 \n");
					makecontext(&uc[tmp_for_Q->pid], task2, 0);
					swapcontext(&uc[0],&uc[tmp_for_Q->pid]);
					//task2();
					if(tmp_for_Q->status != 1|| tmp_for_Q->status != 2){
						tmp_for_Q->status = TASK_TERMINATED;
						tmp_for_HQ->status = TASK_TERMINATED;
						int tmp_print;
						tmp_print = HPop();
						printf("yoyo %d\n",tmp_print);
					}
					
				}
				else if(strcmp(tmp_for_HQ->task,"task3") == 0){
					printf("Hello 3\n");
					settime();
					makecontext(&uc[tmp_for_Q->pid], task3, 0);
					//printf("LLQL\n");
					//setcontext(&uc[tmp_for_Q->pid]);
					swapcontext(&uc[0],&uc[tmp_for_Q->pid]);
					//task3();
					if(tmp_for_Q->status != 1|| tmp_for_Q->status != 2){

						tmp_for_Q->status = TASK_TERMINATED;
						tmp_for_HQ->status = TASK_TERMINATED;
						int tmp_print;
						tmp_print = HPop();
						printf("yoyo %d\n",tmp_print);
					}
				}
				else if(strcmp(tmp_for_HQ->task,"task4") == 0){
					//task4();
					makecontext(&uc[tmp_for_Q->pid], task4, 0);
					swapcontext(&uc[0],&uc[tmp_for_Q->pid]);
					if(tmp_for_Q->status != 1|| tmp_for_Q->status != 2){

						tmp_for_Q->status = TASK_TERMINATED;
						tmp_for_HQ->status = TASK_TERMINATED;
						int tmp_print;
						tmp_print = HPop();
						printf("yoyo %d\n",tmp_print);
					}	
				}
				else if(strcmp(tmp_for_HQ->task,"task5") == 0){
					//task5();
					makecontext(&uc[tmp_for_Q->pid], task5, 0);
					swapcontext(&uc[0],&uc[tmp_for_Q->pid]);
					if(tmp_for_Q->status != 1|| tmp_for_Q->status != 2){

						tmp_for_Q->status = TASK_TERMINATED;
						tmp_for_HQ->status = TASK_TERMINATED;
						int tmp_print;
						tmp_print = HPop();
						printf("yoyo %d\n",tmp_print);
					}			
				}
				else if(strcmp(tmp_for_HQ->task,"task6") == 0){
					//task6();
					makecontext(&uc[tmp_for_Q->pid], task6, 0);
					swapcontext(&uc[0],&uc[tmp_for_Q->pid]);
					if(tmp_for_Q->status != 1|| tmp_for_Q->status != 2){

						tmp_for_Q->status = TASK_TERMINATED;
						tmp_for_HQ->status = TASK_TERMINATED;
						int tmp_print;
						tmp_print = HPop();
						printf("yoyo %d\n",tmp_print);
					}	
				}	
			}
			else if(strcmp(tmp_for_HQ->time_q,"S") ==0){
				printf("I am S\n");
			}
		}
		//modify: it will not run LQ
		//use flag?
		else if(LisEmpty()!=0){

		}
	}
	return 0;
}
