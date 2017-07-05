#include "iostream"
#include "algorithm"
#include "vector"
#include "fstream"
#include <stdio.h>
using namespace std;

//개별 task를 나타내는 구조체
typedef struct task {
	int Arrival_time;
	int Execution_time;
	int lifetime;
	int task_num;
	int priority;
	int Peroid;
}task;

//vector 구조체를 우선순위에 따라 정렬할 때 쓰는 함수
bool Arrival(task first, task second);
bool EDF(task first, task second);
bool RM(task first, task second);
bool User_prior(task first, task second);

int main(void) {
	int duration, num;
	int current_time = 0; // 현재 시간값을 유지
	int i, j, mode;
	int disp_begin, disp_end;
	int howmany = 1;
	int exist = 0;
	int missed = 0;

	vector<task> vtask;// 모든 주기 task와 비주기 task를 구조체에 저장
	vector<int> disp; // 스케쥴링 하는 모든 값을 유지하고 원하는 시간대의 task를 display 할 수 있게 한다.
	vector<int> MaxRes_time; // 최악의 응답시간 유지

	//average response time을 구하기위한 벡터들 정의
	vector<double> response_time; 
	vector<int> Group_num;


	//초기 화면 설정
	FILE *fp = fopen("RM_input.txt", "r");
	printf("Duration을 입력하시오:");
	scanf("%d", &duration);
	printf("\n");
	printf("Mode를 입력하시오 ( 1.EDF, 2.RM 3.User Priority ) : ");
	scanf("%d", &mode);
	printf("\n");
	fscanf(fp, "%d", &num);

	task a;
	a.task_num = 1;

	//각각의 task를 vtask 라는 vector 구조체에 넣는 과정. 주기함수라면 arrival time을 기준으로 각각의 구조체로 나누어서 넣는다.
	for (i = 0; i < num; i++) {
		//task의 갯수만큼 vector 인자를 생성하고 0으로 초기화
		response_time.push_back(0);
		Group_num.push_back(0);
		MaxRes_time.push_back(0);

		int IsPeriod;
		int Period;
		int deadline;

		fscanf(fp, "%d", &IsPeriod);//주기인지 비주기인지 check 주기는 1 비주기는 0으로 설정
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

			//주기함수의 경우 duration까지 arrival time에 따라 구조체에 넣는다.
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
		else {//비주기 함수의 경우
			if (mode != 2) {//RM mode는 주기함수만을 고려
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

	//각각의 mode에 따라 vector 구조체를 sorting한다.
	sort(vtask.begin(), vtask.end(), Arrival);
	if (mode == 1)
		sort(vtask.begin(), vtask.end(), EDF);
	else if (mode == 2)
		sort(vtask.begin(), vtask.end(), RM);
	else if (mode == 3)
		sort(vtask.begin(), vtask.end(), User_prior);
	howmany--;

	//정렬된 vector 구조체를 가지고 Scheduling을 실행
	while (1) {
		exist = 0;
		//current_time을 현재 시간값으로 유지하며 그 값이 duration보다 커지면 스케쥴링 완료한다.
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
			//도착한 task들을 가지고 사용자가 원하는 우선순위로 simulating한다.
			//시간별 task들을 disp에 유지한다.
			for (i = 0; i < howmany; i++) {
				
				if (vtask[i].Arrival_time <= current_time) {
					exist = 1;
					//duration을 초과하지 않도록 한다.
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
			//아무 task도 수행하고 있지 않은 경우 0으로 display
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

	//disp vector를 이용하여 원하는 구간을 display한다.
	printf("원하는 시간 구간을 입력하시오 : ");
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