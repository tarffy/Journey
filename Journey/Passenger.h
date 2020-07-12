#pragma once
#pragma execution_character_set("utf-8")
#include "graph.h"
#include <qstring.h>
#include <fstream>
#include <vector>
using std::ofstream;
using std::vector;
using std::pair;
class Graph;
struct path_result {
	double danger, min_danger;			//��ǰΣ�ն�	Ŀǰ��СΣ�ն�
	int len;							//��ͨ��ʽѡ��
	int time, time_when_min;			//��ǰʱ��	��СΣ�ն�ʱʱ��
	int list_filled = 0;				//list����·��������
	char *schedule_choice_now;			//Ŀǰ�İ��ѡ��	����DFSʱ�洢Ŀǰ���ѡ��
	char *schedule_choice_final;		//��ǰ��СΣ�նȵİ��ѡ��
	char *path_now;						//Ŀǰѡ��·��		
	char *path_finally;					//��ǰ��СΣ�ն�·��ѡ��
	//�������ֵ��͵�����·������Ϣ ����·�������ѡ�񡢷���ֵ����ʱ ʵ���Ͽ���ȡ��time_when_min,schedule_choice_final,path_finally
	vector< pair< pair<char *, char *>, pair<double, int> >> list;	
};
class Passenger {			//�˿͵����
	//���ҳ����·���� �Ѿ������λ��	ֵΪ1��ʾ�ѵ�����1�ĳ��� λ����1���л�1���е���һ�����е�·��
	int cur;		
	int time_to_arrive_next;	//����˿͵�����һ�����е�Сʱ�� �ж�
	int last_hour = -1;			//����˿��ϴθ���״̬ʱ��Сʱ�� �����ظ����³��ֲ���Ԥ�ϵĴ���
	//status[0]Ϊ1��ʾ�˿�����������֮��	��ʱstatus[1],status[2]��ʾ�����������м�
	//status[0]Ϊ0��ʾ��һ������ͣ��	statusp[1]��ʾ�����ڳ���	ʵ���Ͽɴ�������cur
	int status[3];
	void set_status(int status_now, int src, int dest = 0) { 
		status[0] = status_now;
		status[1] = src;
		status[2] = dest;
	}
public:
	Passenger(int source, int destination, int start_time = 0, int method = 0, int hours_limit = 0);		//����Ԫ�鹹������
	Passenger(const char *file_name);	//���ļ�����˿���Ϣ
	int source, destination;	//��ʼ�����ֹ�� �ó��б�ű�ʾ
	int method;					//�˿Ͳ��ó��в��� 0	��ͷ���	1 �޶�ʱ����ͷ���
	int start_time;				//����ʱ��
	int time_limit;				//ʱ������	methodΪ1ʱ��Ч
	int arrived = 0;			//�˿��Ƿ񵽴�Ŀ�ĵ� �����ж��Ƿ����״̬����
	ofstream *out_file;			//�˿�������־������ļ���
	path_result *pr;			//����˿ͳ��й滮������

	void set_passenger(int source, int destination, int start_time = 0, int method = 0, int hours_limit = 0);
	void set_path_result();						//��ʼ��path_result
	QString update(Graph *g, int hours);		//�Գ˿���Ϣ���и��� hoursΪ����Сʱ�� ���س˿�״̬�ַ���
	void set_ofstream(int num);					//���˿���num������ļ���
	int *get_status() { return status; }

	void out_path_result(int if_finnally = 0);//����2Ϊ�Ƿ��ӡ���	���ڲ���
};