#include "scheduling_simulator.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <signal.h>
struct node{
	int pid;
	char task[100000];
	char time_q[100];
	char prior[100];
	struct node* next;
};

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
void hw_suspend(int msec_10)
{
	return;
}

void hw_wakeup_pid(int pid)
{
	return;
}

int hw_wakeup_taskname(char *task_name)
{
    return 0;
}

int hw_task_create(char *task_name)
{
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
        HQ_HEAD->pid = Q_number;
        HQ_HEAD->next = NULL;
        HQ_TAIL = HQ_HEAD;
    } else {
        struct node* ptr = (struct node*)malloc(sizeof(struct node));
		printf("push HQ\n");
        ptr->pid = Q_number;
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
		//HQ_HEAD->time_q = time;
		//printf("push %d\n",Q_number + 1);
		//printf("name %s\n",name);
		sprintf(LQ_HEAD->task,"%s",name);
		//printf("push %d\n",Q_number + 1);
		sprintf(LQ_HEAD->time_q,"%s",time);
		//printf("push %d\n",Q_number + 1);
		sprintf(LQ_HEAD->prior,"%s",priority);
		//printf("push %d\n",Q_number + 1);
		//HQ_HEAD->task = name;
        LQ_HEAD->pid = Q_number;
        LQ_HEAD->next = NULL;
        LQ_TAIL = LQ_HEAD;
    } else {
        struct node* ptr = (struct node*)malloc(sizeof(struct node));
		//printf("push %d\n",Q_number + 1);
        ptr->pid = Q_number;
		sprintf(ptr->task,"%s",name);
		sprintf(ptr->prior,"%s",priority);
		sprintf(ptr->time_q,"%s",time);		
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
		//HQ_HEAD->time_q = time;
		printf("push %d\n",Q_number + 1);
		printf("name %s\n",name);
		sprintf(Q_HEAD->task,"%s",name);
		printf("push %d\n",Q_number + 1);
		sprintf(Q_HEAD->time_q,"%s",time);
		printf("push %d\n",Q_number + 1);
		sprintf(Q_HEAD->prior,"%s",priority);
		printf("push %d\n",Q_number + 1);
		//HQ_HEAD->task = name;
        Q_HEAD->pid = Q_number + 1;
        Q_HEAD->next = NULL;
        Q_TAIL = Q_HEAD;
    } else {
        struct node* ptr = (struct node*)malloc(sizeof(struct node));
		printf("push %d\n",Q_number + 1);
        ptr->pid = Q_number + 1;
		sprintf(ptr->task,"%s",name);
		sprintf(ptr->prior,"%s",priority);
		sprintf(ptr->time_q,"%s",time);		
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

	printf("%d %s 0 %s %s\n",HEAD->pid,HEAD->task,HEAD->prior,HEAD->time_q);
	tmp++;
	if(tmp<Q_number){
		printQ(HEAD->next);
	}
	else{
		return 0;
	}


}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
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
					//task_name = pch;
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
							//sprintf(time_quantum,"%s",pch);
							//time_quantum = pch;
						}
						else if(pch[1] == 'p'){
							prior_flag = 1;
							//sprintf(prior,"%s",pch);
							//prior = pch;
						}				
					}
				}
				pch = strtok(NULL,delim);
				count ++;
			}
			printf("task name:%s\nprior:%s\ntime:%s\n",task_name, prior, time_quantum);
			//push to Q
			Push(task_name,time_quantum,prior);
			//printf("finish psuh");
			if(strcmp(prior,"L")>= 0){
				printf("HERE L \n");
				LPush(task_name,time_quantum,prior);
			}
			else if(prior == 'H'){
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

			return;

		}


	}



}
void signal_handler(int signal_number){
	printf("\n Ctrl + Z :) \n");
	shell();
}
void simulation(){
	signal(SIGTSTP, signal_handler);
	printf("Hello\n");

	while(1);
	return 0;
}
int main()
{

	shell();
	simulation();

	return 0;
}
