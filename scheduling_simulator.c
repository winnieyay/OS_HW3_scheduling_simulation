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
	char task[10000000];
	char time_q[1000000];
	char prior[1000000];
	struct node* next;
	enum TASK_STATE status;
	int queing_time;
};
int uc_flag[40];
int first_in_flag = 0;
int HQ_check_flag = 0;
int LQ_check_flag = 0;
int time_flag_for_set = 0;
int mode_flag = 0;
//count suspend
int pid_time_flag[40];
static ucontext_t uc[41];
struct node* find_pid_in_Q(struct node* ptr,int pid);
struct node* find_parent(struct node* parent);
void alarm_signal(int signal_number);
int find_name(struct node* find,char *task_name);
void closetime();
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
int all_empty_flag = 0;
int main_or_task_flag = 0;
void hw_suspend(int msec_10)
{

	//waiting is OK?
	//swap?
	printf("HELLO in sus %d \n",running_flag);
	struct node* tmp_Q_in_sus;
	tmp_Q_in_sus = find_pid_in_Q(Q_HEAD,running_flag);
	tmp_Q_in_sus->status = TASK_WAITING;
	if(strcmp(tmp_Q_in_sus->prior,"H") == 0){
		//printf("HELLO in sus H\n");
		struct node* tmp_HQ_in_sus;
		tmp_HQ_in_sus = find_pid_in_Q(HQ_HEAD,running_flag);
		tmp_HQ_in_sus->status = TASK_WAITING;
		//printf("HELLO in sus pid %d /  state %d\n",tmp_HQ_in_sus->pid,tmp_HQ_in_sus->status);
	}
	else{
		struct node* tmp_LQ_in_sus;
		tmp_LQ_in_sus = find_pid_in_Q(LQ_HEAD,running_flag);
		tmp_LQ_in_sus->status = TASK_WAITING;
	}
	pid_time_flag[tmp_Q_in_sus->pid] = msec_10;
	//change state to TASK_WAITING
	//set context
	//reschedule
	swapcontext(&uc[running_flag],&uc[0]);
	//return;
}

void hw_wakeup_pid(int pid)
{
	//done
	struct node* tmp_Q_in_wake_pid;
	tmp_Q_in_wake_pid = find_pid_in_Q(Q_HEAD,pid);
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
	struct node* tmp_wake = Q_HEAD;
	struct node* tmp_wake_find;
	struct node* tmp_wake_p;
	int wake_pid = 0;
	int count_wake = 0;
	for(int i=0;i < Q_number;i++){
		//find pid of task name
		wake_pid = find_name(tmp_wake,task_name);
		//pid found
		if(wake_pid != -1){
			count_wake++;
			tmp_wake_find = find_pid_in_Q(Q_HEAD,wake_pid);
			tmp_wake_find->status = TASK_READY;
			if(strcmp(tmp_wake_find->prior,"H") ==0){
				tmp_wake_p = find_pid_in_Q(HQ_HEAD,wake_pid);
				tmp_wake_p->status = TASK_READY;
				pid_time_flag[wake_pid] = 0;
			}
			else{
				tmp_wake_p = find_pid_in_Q(LQ_HEAD,wake_pid);
				tmp_wake_p->status = TASK_READY;
				pid_time_flag[wake_pid] = 0;
			}
		}
		if( i != Q_number - 1){
			tmp_wake = tmp_wake->next;
		}
	}
	//int pid_time_flag[40];

	//use recursive to find
    //change state from WAITING to READY
	//return number of tasks which is waken up
	return count_wake;
}
int find_name(struct node* find,char *task_name){

		if(strcmp(find->task,task_name) == 0){
			return find->pid;
		}
		else{
			return -1;
		}
	
}
int hw_task_create(char *task_name)
{
	//done
	//modify: change prior
	//modify: check and change all to task1 or task3
	//printf("HI create\n");
	printf("in create\n");
    char task_tmp_name[1000];
	char prior_tmp[1000];
	char time_quantum[1000];
	memset(time_quantum,0,sizeof(time_quantum));
	memset(prior_tmp,0,sizeof(prior_tmp));
	memset(task_tmp_name,0,sizeof(task_tmp_name));
	if(strcmp(task_name,"task1")==0){
		/*
		printf("Task 1 created\n");
		sprintf(task_tmp_name,"%s",task_name);
		strcat(prior,"H");
		strcat(time_quantum,"L");
		Push(task_tmp_name,time_quantum,prior);
		LPush(task_tmp_name,time_quantum,prior);
		return Q_TAIL->pid;
		*/
	}
	else if(strcmp(task_name,"task2")==0){
		/*
		printf("Task 2 created\n");
		strcat(prior,"L");
		strcat(time_quantum,"S");
		Push(task_name,time_quantum,prior);
		LPush(task_name,time_quantum,prior);
		return Q_TAIL->pid;
		*/
	}
	else if(strcmp(task_name,"task3")==0){
		//printf("Task 3 created\n");
		sprintf(task_tmp_name,"%s",task_name);
		strcat(prior_tmp,"L");
		//no prior???
		strcat(time_quantum,"S");
		//printf("%s\n",prior_tmp);
		//printf("%s\n",time_quantum);
		//printf("Task 3 created 2\n");
		//Push(task_tmp_name,time_quantum,prior_tmp);
		//printf("Task 3 created 3 check :%d %s\n",Q_TAIL->pid,Q_TAIL->prior);
		Push(task_tmp_name,time_quantum,prior_tmp);
		LPush(task_tmp_name,time_quantum,prior_tmp);
		//printf("Task 3 created 3 check :%d %s\n",Q_TAIL->pid,Q_TAIL->prior);
		return Q_TAIL->pid;
	}
	else if(strcmp(task_name,"task4")==0){
		/*
		printf("Task 4 created\n");
		strcat(prior,"L");
		strcat(time_quantum,"S");
		Push(task_name,time_quantum,prior);
		LPush(task_name,time_quantum,prior);
		return Q_TAIL->pid;
		*/
	}
	else if(strcmp(task_name,"task5")==0){
	
	}
	else if(strcmp(task_name,"task6")==0){

	}
	else{
		return -1;
	}
	//create task task_name
	//return pid of created task
	//return -1 if there is no function name task_name
	//return 0; // the pid of created task name
}
void count_for_sus(){
	//printf("count for sus\n");
	for(int x=0;x<40;x++){
		if(pid_time_flag[x]>0){
			pid_time_flag[x] = pid_time_flag[x] - 10;
			if(pid_time_flag[x] <= 0 ){
				struct node* tmp_for_count;
				tmp_for_count = find_pid_in_Q(Q_HEAD,x);
				tmp_for_count->status = TASK_READY;
				if(strcmp(tmp_for_count->prior,"H")==0){
					struct node* tmp_for_count_H;
					tmp_for_count_H = find_pid_in_Q(HQ_HEAD,x);
					tmp_for_count_H->status = TASK_READY;
				}
				else{
					struct node* tmp_for_count_L;
					tmp_for_count_L = find_pid_in_Q(LQ_HEAD,x);
					tmp_for_count_L->status = TASK_READY;
				}
				//find_pid_in_Q();
				//change status;
			}
		}
	}
}
//modify////////////////////////////////////////////////////////////////////////////////////////////////////
//1.High priority 2.Low priority 3.All
void HPush(char* name,char* time,char* priority)
{
    if(HQ_HEAD == NULL) {
		//printf("push HQ\n");
        HQ_HEAD = (struct node*)malloc(sizeof(struct node));
		sprintf(HQ_HEAD->task,"%s",name);
		sprintf(HQ_HEAD->time_q,"%s",time);
		sprintf(HQ_HEAD->prior,"%s",priority);
		HQ_HEAD->status = TASK_READY;
        HQ_HEAD->pid = Q_number;
        HQ_HEAD->next = NULL;
		HQ_HEAD->queing_time = 0;
        HQ_TAIL = HQ_HEAD;
    } else {
        struct node* ptr = (struct node*)malloc(sizeof(struct node));
		//printf("push HQ\n");
        ptr->pid = Q_number;
		ptr->status = TASK_READY;
		//printf("status %d\n",ptr->status);
		sprintf(ptr->task,"%s",name);
		sprintf(ptr->prior,"%s",priority);
		sprintf(ptr->time_q,"%s",time);	
		ptr->queing_time = 0;	
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
	//printf("free!\n");
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

//me == ??
//purpose:remove and push
//we need to know parent
//
//if parent->next->pid = running_flag  //we got it's parent!!
// parent->next = parent->next->next;
//in main: 
// parent = remove_and_push(HQ_HEAD);
// parent->next = tmp_for_HQ->next;
// QTAIL->next = tmp_for_HQ;
// 
struct node* find_parent(struct node* parent){
	if(parent->next->pid == running_flag){
		return parent;
	}
	else{
		find_parent(parent->next);
	}
	//if()
	//check if me==TAIL
		//no prior???
	//check prior
		//no prior???
	//if me != TAIL
		//no prior???
	// write recursive to find be
		//no prior???
	//	before->next = me->next
	//if me == TAIL
	// before->next = NULL
	// HQTAIL = before
}
void remove_from_Q(int pri){
	if(pri == 0){
		//low prior
		struct node* remove_me;
		remove_me = find_pid_in_Q(LQ_HEAD,running_flag);
		//modify??
		if(remove_me->pid == LQ_HEAD->pid){
			LPop();
		}
		else if(remove_me->pid == LQ_TAIL->pid){
			struct node* remove_H;
			remove_H = find_parent(LQ_HEAD);
			remove_H->next = NULL;
			LQ_TAIL = remove_H;
			free(remove_me);
			LQ_number--;
		}
		else{
			struct node* remove_H;
			remove_H = find_parent(LQ_HEAD);
			remove_H->next = remove_me->next;
			free(remove_me);
			LQ_number--;

		}
	}
	else{
		
		struct node* remove_me;
		remove_me = find_pid_in_Q(HQ_HEAD,running_flag);
		//modify??
		if(remove_me->pid == HQ_HEAD->pid){
			HPop();
		}
		else if(remove_me->pid == HQ_TAIL->pid){
			struct node* remove_H;
			remove_H = find_parent(HQ_HEAD);
			remove_H->next = NULL;
			HQ_TAIL = remove_H;
			free(remove_me);
			HQ_number--;
		}
		else{
			struct node* remove_H;
			remove_H = find_parent(HQ_HEAD);
			remove_H->next = remove_me->next;
			free(remove_me);
			HQ_number--;

		}
	}

}
//Low priority
void LPush(char* name,char* time,char* priority)
{
    if(LQ_HEAD == NULL) {
		//printf("push LQ\n");
        LQ_HEAD = (struct node*)malloc(sizeof(struct node));
		sprintf(LQ_HEAD->task,"%s",name);
		sprintf(LQ_HEAD->time_q,"%s",time);
		sprintf(LQ_HEAD->prior,"%s",priority);
		LQ_HEAD->status = TASK_READY;
        LQ_HEAD->pid = Q_number;
        LQ_HEAD->next = NULL;
		LQ_HEAD->queing_time = 0;
        LQ_TAIL = LQ_HEAD;
    } else {
        struct node* ptr = (struct node*)malloc(sizeof(struct node));
        ptr->pid = Q_number;
		sprintf(ptr->task,"%s",name);
		sprintf(ptr->prior,"%s",priority);
		sprintf(ptr->time_q,"%s",time);	
		ptr->status = TASK_READY;	
		ptr->queing_time = 0;
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

void Push(char* name,char* time_pq,char* priority)
{
    if(Q_HEAD == NULL) {
		//printf("push %d\n",Q_number + 1);
        Q_HEAD = (struct node*)malloc(sizeof(struct node));
		sprintf(Q_HEAD->task,"%s",name);
		sprintf(Q_HEAD->time_q,"%s",time_pq);	
		sprintf(Q_HEAD->prior,"%s",priority);
		Q_HEAD->status = TASK_READY;
        Q_HEAD->pid = Q_number + 1;
        Q_HEAD->next = NULL;
		Q_HEAD->queing_time = 0;
        Q_TAIL = Q_HEAD;
    } else {
        struct node* ptr = (struct node*)malloc(sizeof(struct node));

        ptr->pid = Q_number + 1;
		sprintf(ptr->task,"%s",name);
		sprintf(ptr->prior,"%s",priority);
		sprintf(ptr->time_q,"%s",time_pq);	
		ptr->status = TASK_READY;	
		ptr->queing_time = 0;
		//printf("check in push: %d %s %s\n",ptr->pid,ptr->prior,priority);
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

	printf("%d   %s  %s   %d   %s   %s\n",HEAD->pid,HEAD->task,state_array[HEAD->status],HEAD->queing_time,HEAD->prior,HEAD->time_q);
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
	printf("STOP!!\n");
	if(all_empty_flag == 1){
		all_empty_flag = 0;
		//printf("Stop all empty\n");
		count_q_time(Q_HEAD);;
		count_for_sus();

	}
	else{
	//printf("Stop!\n");

	struct node* tmp_alarm;
	tmp_alarm = find_pid_in_Q(Q_HEAD,running_flag);
	tmp_alarm->status = TASK_READY;

	//printf("Stopp!\n");
	if(strcmp(tmp_alarm->prior,"H")==0){
		//printf("Stop in H!\n");
		struct node* tmp_alarm_H;
		tmp_alarm_H = find_pid_in_Q(HQ_HEAD,running_flag);
		tmp_alarm_H->status = TASK_READY;
		if(HQ_HEAD->pid == HQ_TAIL->pid){

		}
		else if(HQ_HEAD->pid == running_flag){
			HQ_HEAD = tmp_alarm_H->next;
			tmp_alarm_H->next = NULL;
			HQ_TAIL->next = tmp_alarm_H;
			HQ_TAIL = tmp_alarm_H;
		}
		if(HQ_TAIL->pid != running_flag){
			//printf("Back!! %d\n\n",running_flag);
			struct node* tmp_parent_H;
			tmp_parent_H = find_parent(HQ_HEAD);
			printf("bye bye!! %d\n\n",running_flag);
			tmp_parent_H->next = tmp_alarm_H->next;
			tmp_alarm_H->next = NULL;
			HQ_TAIL->next = tmp_alarm_H;
			HQ_TAIL = tmp_alarm_H;
		}
		else{
		
		}
	}
	else{
		struct node* tmp_alarm_H;
		tmp_alarm_H = find_pid_in_Q(LQ_HEAD,running_flag);
		tmp_alarm_H->status = TASK_READY;
		if(LQ_HEAD->pid == LQ_TAIL->pid){

		}
		else if(LQ_HEAD->pid == running_flag){
			LQ_HEAD = tmp_alarm_H->next;
			tmp_alarm_H->next = NULL;
			LQ_TAIL->next = tmp_alarm_H;
			LQ_TAIL = tmp_alarm_H;
		}
		if(LQ_TAIL->pid != running_flag){
			//printf("Back!! %d\n\n",running_flag);
			struct node* tmp_parent_H;
			tmp_parent_H = find_parent(LQ_HEAD);
			//printf("bye bye!! %d\n\n",running_flag);
			tmp_parent_H->next = tmp_alarm_H->next;
			tmp_alarm_H->next = NULL;
			LQ_TAIL->next = tmp_alarm_H;
			LQ_TAIL = tmp_alarm_H;
		}
		else{
		
		}

	}
	//printf("change status\n");
	swapcontext(&uc[running_flag],&uc[0]);

	return;

	}
	
}
void shell(){

	mode_flag = 1;
	char input_char[1000000];
	char task_name[10000];
	char prior[100];
	char time_quantum[100];
	t.it_interval.tv_sec = 0;
	t.it_interval.tv_usec = 0;
	t.it_value.tv_sec = 0;
	t.it_value.tv_usec = 0;
	setitimer(ITIMER_REAL, &t, NULL);

	while(1){

		char buff[50];
		memset(buff,0,sizeof(buff));
		memset(task_name,0,sizeof(task_name));
		memset(time_quantum,0,sizeof(time_quantum));
		memset(prior,0,sizeof(prior));
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
			//printf("this is a\n");
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
			//printf("task name:%s\nprior:%s\ntime:%s\n",task_name, prior, time_quantum);
			//push to Q
			Push(task_name,time_quantum,prior);
			if(strcmp(prior,"L")>= 0){
				//printf("HERE L \n");
				LPush(task_name,time_quantum,prior);
			}
			else if(strcmp(prior,"H")>= 0){
				//printf("HERE H \n");
				HPush(task_name,time_quantum,prior);
			}
		}
		else if(buff[0] == 'p'){
			tmp = 0;
			printQ(Q_HEAD);
		}
		else if(buff[0] == 'r'){
			//modify remove

		}
		else if(buff[0] == 's'){
			//settime();
			mode_flag = 0;
			//modify
			//return or set???
			return;
		}
	}
}
void signal_handler(int signal_number){
	//modify : if in shell mode, don't response
	if(mode_flag == 1){
		//closetime();
	}
	else{
		///modify
		//use context
		printf("\n Ctrl + Z :) \n");
		t.it_interval.tv_sec = 0;
		t.it_interval.tv_usec = 0;
		t.it_value.tv_sec = 0;
		t.it_value.tv_usec = 0;
		setitimer(ITIMER_REAL, &t, NULL);
		//swapcontext(&)
		
		if(main_or_task_flag == 0){
			swapcontext(&uc[0],&uc[40]);
		}
		else{
			swapcontext(&uc[running_flag],&uc[40]);
		}
		
		//shell();

	}
	
}
//modify////////////////////////////////////////////////////////////
struct node* check_status(struct node* ptr){
	//modify : if no READY ->set flag
	//done but need to be check
	//printf("check pid and status:%d // %d\n",ptr->pid,ptr->status);
	if(ptr->status == 1){
		//printf("check st func 1: %d\n",ptr->pid);
		return ptr;
	}
	else{
		//printf("check st func: %d\n",ptr->pid);
		if(strcmp(ptr->prior,"H")==0){
			if(ptr->pid == HQ_TAIL->pid){
				HQ_check_flag = 1;
				return ptr;		
			}
			else{
				check_status(ptr->next);
			}
			
		}
		else{
			if(ptr->pid == LQ_TAIL->pid){
				//modify in main
				LQ_check_flag = 1;
				return ptr;
			}
			else{
				check_status(ptr->next);
			}
			
		}
		//check_status(ptr->next);
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
void count_q_time(struct node* start){

	if(start->pid == Q_TAIL->pid){
		if(start->status == 1){
			start->queing_time=start->queing_time+10 ;
		}	
	}
	else{
		if(start->status == 1){
			//start
			start->queing_time= start->queing_time +10 ;
			count_q_time(start->next);
		}
		else{
			count_q_time(start->next);
		}
	}
	

}
void settime(){
	//modify :for short time 
	//use time_flag_for_set
	//1 == long  0 == short
	if(all_empty_flag == 1){
		t.it_interval.tv_sec = 0;
		t.it_interval.tv_usec = 0;
		t.it_value.tv_sec = 0;
		t.it_value.tv_usec = 10;
		//printf("DONE set time in empty\n");
		setitimer(ITIMER_REAL,&t,NULL);
	}
	else{
		if(time_flag_for_set == 0){
			t.it_interval.tv_sec = 0;
			t.it_interval.tv_usec = 0;
			t.it_value.tv_sec = 0;
			t.it_value.tv_usec = 10;
			printf("DONE set time short\n");
			setitimer(ITIMER_REAL,&t,NULL);
		}
		else{
			t.it_interval.tv_sec = 0;
			t.it_interval.tv_usec = 0;
			t.it_value.tv_sec = 0;
			t.it_value.tv_usec = 20;
			printf("DONE set time long\n");
			setitimer(ITIMER_REAL,&t,NULL);
		}

	}


}
void closetime(){
	//modify :for short time 
	//use time_flag_for_set
	//1 == long  0 == short
	t.it_interval.tv_sec = 0;
	t.it_interval.tv_usec = 0;
	t.it_value.tv_sec = 0;
	t.it_value.tv_usec = 0;
	//printf("DONE close time\n");
	setitimer(ITIMER_REAL,&t,NULL);

}
int main()
{
	if(first_in_flag == 0){
		//char *stack = (char*)malloc(10000);
		signal(SIGTSTP, signal_handler);
		signal(SIGALRM,alarm_signal);
		

		for(int j=0;j<40;j++){
			uc_flag[j] = 0;
			pid_time_flag[j] = -1;
		}

		getcontext(&uc[0]);
		char *stack = (char*)malloc(1000);
		uc[0].uc_link = NULL;
		uc[0].uc_stack.ss_sp = stack;
		uc[0].uc_stack.ss_size = sizeof(stack);
		/*
		getcontext(&uc[40]);
		uc[40].uc_link = &uc[0];
		uc[40].uc_stack.ss_sp = stack;
		uc[40].uc_stack.ss_size = sizeof(stack);
		*/
		
		for(int i=1;i<41;i++){
			char *stack = (char*)malloc(10000000);
			getcontext(&uc[i]);
			uc[i].uc_link = &uc[0];
			uc[i].uc_stack.ss_sp = stack;
			uc[i].uc_stack.ss_size = sizeof(stack);
		}
		makecontext(&uc[40], shell, 0);
		first_in_flag = 1;

	}
	shell();
	while(1){
		printf("HI\n");
		//printf("count my Q\n");
		
		if(HQ_number == 0 && LQ_number == 0){
			//printf("lol\n");
			shell();
		}
		else if(HisEmpty()){
			HQ_check_flag = 1;
		}
		//if high Q is not empty
		else if(!HisEmpty()){
			printf("HI H\n");
			//fix
			struct node* tmp_for_HQ;
			tmp_for_HQ = check_status(HQ_HEAD);
			if(HQ_check_flag == 1){
				printf("check HQ\n");
				//nothing to do in HQ
			}
			else{
				if(strcmp(tmp_for_HQ->time_q,"L") ==0){
					time_flag_for_set = 1;
				}
				else{
					time_flag_for_set = 0;
				}
				//if(strcmp(tmp_for_HQ->time_q,"L") ==0){

					//time_flag_for_set = 1;
					//printf("I am L\n");
					running_flag = tmp_for_HQ->pid;
					tmp_for_HQ->status = TASK_RUNNING;
					//fix
					struct node* tmp_for_Q;
					tmp_for_Q = find_pid_in_Q(Q_HEAD,tmp_for_HQ->pid);
					tmp_for_Q->status = TASK_RUNNING;
					if(time_flag_for_set == 1){
						count_q_time(Q_HEAD);
						count_q_time(Q_HEAD);
						count_for_sus();
						count_for_sus();
					}
					else{
						count_q_time(Q_HEAD);
						count_for_sus();

					}
					
					if(strcmp(tmp_for_HQ->task,"task1") == 0){
						printf("Hello 1 \n");
						settime();
						if(uc_flag[tmp_for_Q->pid] == 0){
							makecontext(&uc[tmp_for_Q->pid], task1, 0);
							uc_flag[tmp_for_Q->pid] = 1;
						}
						
						//printf("GO\n");
						main_or_task_flag = 1;
						swapcontext(&uc[0],&uc[tmp_for_HQ->pid]);
						main_or_task_flag = 0;
						//printf("out\n");
						closetime();
						//task1();
						//if status != waiting or ready then terminated
						if(tmp_for_Q->status == 0){
							//modify!!! use remove?
							//printf("status : %d",tmp_for_Q->status);		
							tmp_for_Q->status = TASK_TERMINATED;
							tmp_for_HQ->status = TASK_TERMINATED;
							printf("yoyo %d\n",running_flag);
							remove_from_Q(1);
						}
						//free(tmp_for_HQ);
					}
					else if(strcmp(tmp_for_HQ->task,"task2") == 0){
						printf("Hello 2 \n");
						//settime();
						
						if(uc_flag[tmp_for_Q->pid] == 0){
							printf("NEW\n");
							makecontext(&uc[tmp_for_Q->pid], task2, 0);
							uc_flag[tmp_for_Q->pid] = 1;
						}
						//printf("out set time\n");
						//printf("GO 2\n");
						main_or_task_flag = 1;
						printf("GO 2 flag = %d \n",running_flag);
						printf("GO 2 swap = %d \n",tmp_for_Q->pid);
						settime();
						printf("out set time\n");
						swapcontext(&uc[0],&uc[running_flag]);
						closetime();
						printf("OUT 2\n");
						main_or_task_flag = 0;
						//printf("out 2\n");
						

						if(tmp_for_Q->status == 0){
							//printf("status : %d",tmp_for_Q->status);		
							tmp_for_Q->status = TASK_TERMINATED;
							tmp_for_HQ->status = TASK_TERMINATED;
							printf("yoyo %d\n",running_flag);
							remove_from_Q(1);
						}
						
					}
					else if(strcmp(tmp_for_HQ->task,"task3") == 0){
						printf("Hello 3 \n");
						settime();
						if(uc_flag[tmp_for_Q->pid] == 0){
							makecontext(&uc[tmp_for_Q->pid], task3, 0);
							uc_flag[tmp_for_Q->pid] = 1;
						}
						
						//printf("GO 3\n");
						main_or_task_flag = 1;
						swapcontext(&uc[0],&uc[tmp_for_Q->pid]);
						main_or_task_flag = 0;
						//printf("out 3\n");
						closetime();

						if(tmp_for_Q->status == 0){
							//printf("status : %d",tmp_for_Q->status);		
							tmp_for_Q->status = TASK_TERMINATED;
							tmp_for_HQ->status = TASK_TERMINATED;
							printf("yoyo %d\n",running_flag);
							remove_from_Q(1);
						}
					}
					//modify//////////////////////////////////////////////////
					else if(strcmp(tmp_for_HQ->task,"task4") == 0){
						//task4();
						printf("Hello 4 \n");
						settime();
						if(uc_flag[tmp_for_Q->pid] == 0){
							makecontext(&uc[tmp_for_Q->pid], task4, 0);
							uc_flag[tmp_for_Q->pid] = 1;
						}
						
						//printf("GO 4\n");
						main_or_task_flag = 1;
						swapcontext(&uc[0],&uc[tmp_for_Q->pid]);
						main_or_task_flag = 0;
						//printf("out 4\n");
						closetime();

						if(tmp_for_Q->status == 0){
							//printf("status : %d",tmp_for_Q->status);		
							tmp_for_Q->status = TASK_TERMINATED;
							tmp_for_HQ->status = TASK_TERMINATED;
							printf("yoyo %d\n",running_flag);
							remove_from_Q(1);
						}	
					}
					else if(strcmp(tmp_for_HQ->task,"task5") == 0){
						printf("Hello 5 \n");
						settime();
						if(uc_flag[tmp_for_Q->pid] == 0){
							makecontext(&uc[tmp_for_Q->pid], task5, 0);
							uc_flag[tmp_for_Q->pid] = 1;
						}
						
						//printf("GO 5\n");
						main_or_task_flag = 1;
						swapcontext(&uc[0],&uc[tmp_for_Q->pid]);
						main_or_task_flag = 0;
						//printf("out 5\n");
						closetime();

						if(tmp_for_Q->status == 0){
							//printf("status : %d",tmp_for_Q->status);		
							tmp_for_Q->status = TASK_TERMINATED;
							tmp_for_HQ->status = TASK_TERMINATED;
							printf("yoyo %d\n",running_flag);
							remove_from_Q(1);
						}	
					}
					else if(strcmp(tmp_for_HQ->task,"task6") == 0){
						printf("Hello 6 \n");
						settime();
						if(uc_flag[tmp_for_Q->pid] == 0){
							makecontext(&uc[tmp_for_Q->pid], task6, 0);
							uc_flag[tmp_for_Q->pid] = 1;
						}
						
						//printf("GO 6\n");
						main_or_task_flag = 1;
						swapcontext(&uc[0],&uc[tmp_for_Q->pid]);
						main_or_task_flag = 0;
						//printf("out 6\n");
						closetime();

						if(tmp_for_Q->status == 0){
							//printf("status : %d",tmp_for_Q->status);		
							tmp_for_Q->status = TASK_TERMINATED;
							tmp_for_HQ->status = TASK_TERMINATED;
							printf("yoyo %d\n",running_flag);
							remove_from_Q(1);
						}	
					}	
					else{
						printf("NO!\n");
						//while(1);
					}
			}

			
		}
		//modify: it will not run LQ
		//use flag?

		//modify!!!!!!!!!!!!!!! where is my logic!!!
		if(HQ_check_flag == 1){
			HQ_check_flag = 0;
			
			if(!LisEmpty()){
				//if LQ has no task ready
				//set HQ_check_flag = 1
				printf("HI L\n");
				//fix
				struct node* tmp_for_LQ;
				tmp_for_LQ = check_status(LQ_HEAD);
				if(LQ_check_flag == 1){
					HQ_check_flag = 1;
					LQ_check_flag = 0;
					//nothing to do in HQ
				}
				else{
					if(strcmp(tmp_for_LQ->time_q,"L") ==0){
						time_flag_for_set = 1;
					}
					else{
						time_flag_for_set = 0;
					}
					//if(strcmp(tmp_for_HQ->time_q,"L") ==0){

					//time_flag_for_set = 1;
					//printf("I am L\n");
					running_flag = tmp_for_LQ->pid;
					tmp_for_LQ->status = TASK_RUNNING;
					//fix
					struct node* tmp_for_Q;
					tmp_for_Q = find_pid_in_Q(Q_HEAD,tmp_for_LQ->pid);
					tmp_for_Q->status = TASK_RUNNING;
					if(time_flag_for_set == 1){
						
						count_q_time(Q_HEAD);
						count_q_time(Q_HEAD);
						count_for_sus();
						count_for_sus();
					}
					else{
						count_q_time(Q_HEAD);
						count_for_sus();

					}
					printf("MMM\n");
					if(strcmp(tmp_for_LQ->task,"task1") == 0){
						printf("Hello 1 \n");
						settime();
						if(uc_flag[tmp_for_Q->pid] == 0){
							makecontext(&uc[tmp_for_Q->pid], task1, 0);
							uc_flag[tmp_for_Q->pid] = 1;
						}
						
						//printf("GO\n");
						main_or_task_flag = 1;
						swapcontext(&uc[0],&uc[tmp_for_LQ->pid]);
						main_or_task_flag = 0;
						//printf("out\n");
						closetime();
						//task1();
						//if status != waiting or ready then terminated
						if(tmp_for_Q->status == 0){
							//modify!!! use remove?
							//printf("status : %d",tmp_for_Q->status);		
							tmp_for_Q->status = TASK_TERMINATED;
							tmp_for_LQ->status = TASK_TERMINATED;
							printf("yoyo %d\n",running_flag);
							remove_from_Q(0);
						}
						//free(tmp_for_HQ);
					}
					else if(strcmp(tmp_for_LQ->task,"task2") == 0){
						printf("Hello 2 \n");
						settime();
						if(uc_flag[tmp_for_Q->pid] == 0){
							makecontext(&uc[tmp_for_Q->pid], task2, 0);
							uc_flag[tmp_for_Q->pid] = 1;
						}
						
						//printf("GO 2\n");
						main_or_task_flag = 1;
						swapcontext(&uc[0],&uc[tmp_for_Q->pid]);
						main_or_task_flag = 0;
						//printf("out 2\n");
						closetime();

						if(tmp_for_Q->status == 0){
							//printf("status : %d",tmp_for_Q->status);		
							tmp_for_Q->status = TASK_TERMINATED;
							tmp_for_LQ->status = TASK_TERMINATED;
							printf("yoyo %d\n",running_flag);
							remove_from_Q(0);
						}
						
					}
					else if(strcmp(tmp_for_LQ->task,"task3") == 0){
						printf("Hello 3 \n");
						settime();
						if(uc_flag[tmp_for_Q->pid] == 0){
							makecontext(&uc[tmp_for_Q->pid], task3, 0);
							uc_flag[tmp_for_Q->pid] = 1;
						}
						
						//printf("GO 3\n");
						main_or_task_flag = 1;
						swapcontext(&uc[0],&uc[tmp_for_Q->pid]);
						main_or_task_flag = 0;
						//printf("out 3\n");
						closetime();

						if(tmp_for_Q->status == 0){
							//printf("status : %d",tmp_for_Q->status);		
							tmp_for_Q->status = TASK_TERMINATED;
							tmp_for_LQ->status = TASK_TERMINATED;
							printf("yoyo %d\n",running_flag);
							remove_from_Q(0);
						}
					}
					//modify//////////////////////////////////////////////////
					else if(strcmp(tmp_for_LQ->task,"task4") == 0){
						//task4();
						printf("Hello 4 \n");
						settime();
						if(uc_flag[tmp_for_Q->pid] == 0){
							makecontext(&uc[tmp_for_Q->pid], task4, 0);
							uc_flag[tmp_for_Q->pid] = 1;
						}
						
						//printf("GO 4\n");
						main_or_task_flag = 1;
						swapcontext(&uc[0],&uc[tmp_for_Q->pid]);
						main_or_task_flag = 0;
						//printf("out 4\n");
						closetime();

						if(tmp_for_Q->status == 0){
							//printf("status : %d",tmp_for_Q->status);		
							tmp_for_Q->status = TASK_TERMINATED;
							tmp_for_LQ->status = TASK_TERMINATED;
							printf("yoyo %d\n",running_flag);
							remove_from_Q(0);
						}	
					}
					else if(strcmp(tmp_for_LQ->task,"task5") == 0){
						printf("Hello 5 \n");
						settime();
						if(uc_flag[tmp_for_Q->pid] == 0){
							makecontext(&uc[tmp_for_Q->pid], task5, 0);
							uc_flag[tmp_for_Q->pid] = 1;
						}
						
						//printf("GO 5\n");
						main_or_task_flag = 1;
						swapcontext(&uc[0],&uc[tmp_for_Q->pid]);
						main_or_task_flag = 0;
						//printf("out 5\n");
						closetime();

						if(tmp_for_Q->status == 0){
							//printf("status : %d",tmp_for_Q->status);		
							tmp_for_Q->status = TASK_TERMINATED;
							tmp_for_LQ->status = TASK_TERMINATED;
							printf("yoyo %d\n",running_flag);
							remove_from_Q(0);
						}	
					}
					else if(strcmp(tmp_for_LQ->task,"task6") == 0){
						//printf("Hello 6 \n");
						settime();
						if(uc_flag[tmp_for_Q->pid] == 0){
							makecontext(&uc[tmp_for_Q->pid], task6, 0);
							uc_flag[tmp_for_Q->pid] = 1;
						}
						
						//printf("GO 6\n");
						main_or_task_flag = 1;
						swapcontext(&uc[0],&uc[tmp_for_Q->pid]);
						main_or_task_flag = 0;
						//printf("out 6\n");
						closetime();

						if(tmp_for_Q->status == 0){
							//printf("status : %d",tmp_for_Q->status);		
							tmp_for_Q->status = TASK_TERMINATED;
							tmp_for_LQ->status = TASK_TERMINATED;
							printf("yoyo %d\n",running_flag);
							remove_from_Q(0);
						}	
					}	
					else{
						printf("NO!\n");
						//while(1);
					}
				}



				//use check status
				//if check flag = 1  go back to while
			}
			else{
				//LQ is empty
				printf("WW %d\n",LQ_number);
				HQ_check_flag = 1;

			}
		}
		if(HQ_check_flag == 1){
			HQ_check_flag = 0;
			all_empty_flag = 1;
			printf("ALL EMPTY\n");
			settime();
			while(all_empty_flag == 1){

			}
		}
	}
	return 0;
}
