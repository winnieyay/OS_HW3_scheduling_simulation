#include "task.h"

void task1(void)   // may terminated
{
	unsigned int a = ~0;
	//printf("in 1 \n");
	while (a != 0) {
		a -= 1;
	}
}

void task2(void) // run infinite
{
	unsigned int a = 0;
	//printf("FUCK WHY\n");
	while (1) {
		//printf("in 2 %d \n", a);
		a = a + 1;
	}
}

void task3(void) // wait infinite
{
	hw_suspend(32768);
	//hw_suspend(100);
	fprintf(stdout, "task3: good morning~\n");
	fflush(stdout);
}

void task4(void) // sleep 5s
{
	//printf("in 4 \n");
	hw_suspend(500);
	fprintf(stdout, "task4: good morning~\n");
	fflush(stdout);
}

void task5(void)
{
	int pid = hw_task_create("task3");

	hw_suspend(1000);
	fprintf(stdout, "task5: good morning~\n");
	fflush(stdout);

	hw_wakeup_pid(pid);
	fprintf(stdout, "Mom(task5): wake up pid %d~\n", pid);
	fflush(stdout);
}

void task6(void)
{
	//printf("IN 6 \n");
	for (int num = 0; num < 5; ++num) {
		//printf("create task 3\n");
		hw_task_create("task3");
	}

	hw_suspend(1000);
	fprintf(stdout, "task6: good morning~\n");
	fflush(stdout);

	int num_wake_up = hw_wakeup_taskname("task3");
	fprintf(stdout, "Mom(task6): wake up task3=%d~\n", num_wake_up);
	fflush(stdout);
}
