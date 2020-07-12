#pragma once
/*
	���ͼ�����а��
*/
#include "Passenger.h"
#define MAX_SCHEDULE 16		//��һ�ǵ���һ���������
class Passenger;
struct schedule {						//��һ�����е���һ�����е�����
	char type[MAX_SCHEDULE];			//������� 0Ϊ����1Ϊ��2Ϊ�ɻ�
	char time_start[MAX_SCHEDULE];		//��ο�ʼʱ��
	char time_exist[MAX_SCHEDULE];		//��γ���ʱ��
	int len;							//���еİ������
};
struct link {		//����ڵ㣬���ڼ�DFS
	int val;
	link *next;
};
class Graph {
	int city_num;					//��������
	char *city_danger;				//���ն� 0�� 1�� 2��
	bool **connected;				//connected[i][j] ��i��j�Ƿ�ֱ�ӿɴ� �ɴ�Ϊ1���ɴ�Ϊ0
	schedule ***schedule_between;	//schedule[i][j]	Ϊָ���i��j��ε�ָ�� ��·��Ϊ��ָ��	
	int have_xy = 0;				//������Ϣ�ļ��Ƿ��г�����Ϣ
	int *x, *y;						//������е�λ��
	int path_risk_calculate = 1;	//�Ƿ���㽻ͨ��ʽ�����ķ���ֵ

	void auto_set();				//����Graph�� ��ʼ������
	int if_negtive(int a);			//����ʱ���ʹ��
	void set_danger(int *danger);	//���ó���Σ�ն�
	void _calculate_min(char *path, Passenger *p);	//dfs�ҵ�һ��·��ʱ����	����˿�p��·��path�ϵ���С����
	void dfs(link *start, link *pre, bool *book, Passenger *p);	//calculate_min���� ���ҳ˿�p����������·��
	char *get_path(link *start, Passenger *p);		//dfs�ҵ�·��ʱ���� ����·���ַ���
public:
	Graph(const char *file_name);				//���ļ�������
	void calculate_min(Passenger *p);			//����˿�p·�� �����㷨

	void add_schedule(int *a);					//������Ԫ���������µİ��
	void set_path_risk_calculate(int a) { path_risk_calculate = a; }	//���ڽ�path_risk_calculate��0 �Ӷ������㽻ͨ��ʽ�ķ���ֵ
	schedule ***get_schedules() { return schedule_between; }
	int get_city_num() { return city_num; }
	bool **get_connect() { return connected; }
	int *get_xp() { return x; }
	int *get_yp() { return y; }
	int if_have_xy() { return have_xy; }
};
