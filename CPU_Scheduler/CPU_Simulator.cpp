#include "iostream"
#include "algorithm"
#include "vector"
#include "fstream"
#include <stdio.h>
using namespace std;

//���� task�� ��Ÿ���� ����ü
typedef struct task {
	int Arrival_time;
	int Execution_time;
	int lifetime;
	int task_num;
	int priority;
	int Peroid;
}task;

//vector ����ü�� �켱������ ���� ������ �� ���� �Լ�
bool Arrival(task first, task second);
bool EDF(task first, task second);
bool RM(task first, task second);
bool User_prior(task first, task second);

int main(void) {
	int duration, num;
	int current_time = 0; // ���� �ð����� ����
	int i, j, mode;
	int disp_begin, disp_end;
	int howmany = 1;
	int exist = 0;
	int missed = 0;

	vector<task> vtask;// ��� �ֱ� task�� ���ֱ� task�� ����ü�� ����
	vector<int> disp; // �����층 �ϴ� ��� ���� �����ϰ� ���ϴ� �ð����� task�� display �� �� �ְ� �Ѵ�.
	vector<int> MaxRes_time; // �־��� ����ð� ����

	//average response time�� ���ϱ����� ���͵� ����
	vector<double> response_time; 
	vector<int> Group_num;


	//�ʱ� ȭ�� ����
	FILE *fp = fopen("RM_input.txt", "r");
	printf("Duration�� �Է��Ͻÿ�:");
	scanf("%d", &duration);
	printf("\n");
	printf("Mode�� �Է��Ͻÿ� ( 1.EDF, 2.RM 3.User Priority ) : ");
	scanf("%d", &mode);
	printf("\n");
	fscanf(fp, "%d", &num);

	task a;
	a.task_num = 1;

	//������ task�� vtask ��� vector ����ü�� �ִ� ����. �ֱ��Լ���� arrival time�� �������� ������ ����ü�� ����� �ִ´�.
	for (i = 0; i < num; i++) {
		//task�� ������ŭ vector ���ڸ� �����ϰ� 0���� �ʱ�ȭ
		response_time.push_back(0);
		Group_num.push_back(0);
		MaxRes_time.push_back(0);

		int IsPeriod;
		int Period;
		int deadline;

		fscanf(fp, "%d", &IsPeriod);//�ֱ����� ���ֱ����� check �ֱ�� 1 ���ֱ�� 0���� ����
		if (IsPeriod == true) {
			if (mode == 3)//User Priority mode
				fscanf(fp, "%d", &a.priority);

			fscanf(fp, "%d", &a.Arrival_time);
			fscanf(fp, "%d", &a.Execution_time);
			fscanf(fp, "%d", &deadline);
			a.lifetime = a.Arrival_time + deadline;
			fscanf(fp, "%d", &Period);

			if (mode == 2)//RM mode
				a.Peroid = Period;

			//�ֱ��Լ��� ��� duration���� arrival time�� ���� ����ü�� �ִ´�.
			while (1) {
				vtask.push_back(a);
				howmany++;
				Group_num[i]++;
				if ((a.Arrival_time + Period) < duration) {
					a.Arrival_time += Period;
					a.lifetime += Period;
					if (a.Arrival_time + a.Execution_time > duration) {
						a.task_num++;
						break;
					}
				}
				else {
					a.task_num++;
					break;
				}

			}
		}
		else {//���ֱ� �Լ��� ���
			if (mode != 2) {//RM mode�� �ֱ��Լ����� ���
				if (mode == 3)//User priority mode
					fscanf(fp, "%d", &a.priority);
				fscanf(fp, "%d", &a.Arrival_time);
				fscanf(fp, "%d", &a.Execution_time);
				fscanf(fp, "%d", &deadline);
				a.lifetime = a.Arrival_time + deadline;
				vtask.push_back(a);
				a.task_num++;
				howmany++;
				Group_num[i]++;
			}
		}

	}

	//������ mode�� ���� vector ����ü�� sorting�Ѵ�.
	sort(vtask.begin(), vtask.end(), Arrival);
	if (mode == 1)
		sort(vtask.begin(), vtask.end(), EDF);
	else if (mode == 2)
		sort(vtask.begin(), vtask.end(), RM);
	else if (mode == 3)
		sort(vtask.begin(), vtask.end(), User_prior);
	howmany--;

	//���ĵ� vector ����ü�� ������ Scheduling�� ����
	while (1) {
		exist = 0;
		//current_time�� ���� �ð������� �����ϸ� �� ���� duration���� Ŀ���� �����층 �Ϸ��Ѵ�.
		if (current_time > duration) {
			printf("Missed tasks number : %d\n", missed);
			if (missed > 5) {
				printf("Scheduling failed\n");
				return 0;
			}
			else
				printf("Scheduling finished\n");
			break;
		}
		else {
			//������ task���� ������ ����ڰ� ���ϴ� �켱������ simulating�Ѵ�.
			//�ð��� task���� disp�� �����Ѵ�.
			for (i = 0; i < howmany; i++) {
				
				if (vtask[i].Arrival_time <= current_time) {
					exist = 1;
					//duration�� �ʰ����� �ʵ��� �Ѵ�.
					if (current_time + vtask[i].Execution_time < duration) {
						if (vtask[i].lifetime > current_time + vtask[i].Execution_time) {
							response_time[vtask[i].task_num - 1] += current_time - vtask[i].Arrival_time;
							MaxRes_time[vtask[i].task_num - 1] = max(MaxRes_time[vtask[i].task_num - 1], current_time - vtask[i].Arrival_time);
							printf("Task[%d] Execution time : %d ~ %d\n", vtask[i].task_num, current_time, current_time + vtask[i].Execution_time);
							for (j = vtask[i].Arrival_time; j < vtask[i].Arrival_time + vtask[i].Execution_time; j++, current_time++) {
								disp.push_back(vtask[i].task_num);
							}

						}
						else {
							printf("Missed Task[%d] Arrival time : %d Current time : %d\n", vtask[i].task_num, vtask[i].Arrival_time, current_time);
							missed++;
						}
					}
					vtask.erase(vtask.begin() + i);
					howmany--;
					break;
				}

			}
			//�ƹ� task�� �����ϰ� ���� ���� ��� 0���� display
			if (exist == 0) {
				disp.push_back(0);
				current_time++;
			}
		}


	}


	for (i = 0; i < num; i++) {
		printf("Task[%d] Sum of Response Time : %f\n", i + 1, response_time[i]);
		printf("Task[%d] number of tasks : %d\n",i+1, Group_num[i]);
		printf("Task[%d] Average Response Time : %f\n", i + 1, response_time[i] / Group_num[i]);
		printf("Task[%d] Worst Response Time : %d\n", i + 1, MaxRes_time[i]);
	}

	//disp vector�� �̿��Ͽ� ���ϴ� ������ display�Ѵ�.
	printf("���ϴ� �ð� ������ �Է��Ͻÿ� : ");
	scanf("%d %d", &disp_begin, &disp_end);
	for (j = disp_begin; j < disp_end; j++) {
		if (j % 10 == 0)
			printf("%10d", j + 10);
	}
	printf("\n");
	for (j = disp_begin; j < disp_end; j++) {
		printf("%d", disp[j]);
	}
	printf("\n");
	return 0;
}

bool EDF(task first, task second) {
	return first.lifetime < second.lifetime;
}
bool RM(task first, task second) {
	return first.Peroid < second.Peroid;
}
bool User_prior(task first, task second) {
	return first.priority < second.priority;
}
bool Arrival(task first, task second) {
	return first.Arrival_time< second.Arrival_time;
}