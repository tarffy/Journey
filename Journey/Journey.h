#pragma once
#pragma execution_character_set("utf-8")
#include <QtWidgets/QMainWindow>
#include "ui_Journey.h"
#include "graph.h"
#include "Passenger.h"
#include "Add_passenger.h"
#include "Add_schedule.h"
#include <QtNetwork>
class Journey : public QMainWindow
{
	Q_OBJECT

public:
	Journey(QWidget *parent = Q_NULLPTR);
	int get_hours() { return hours; }

private:

	Ui::JourneyClass ui;
	Graph *graph;					//���кͰ����Ϣ
	Passenger **passengers;			//�˿���Ϣ
	QTimer *timer;					//��ʱ��
	Add_passenger *passenger_adder;	//���ڶԻ���ʽ��ӳ˿�
	Add_schedule *schedule_adder;	//���ڶԻ���ʽ��Ӱ��
	QTcpServer *server;				//�����
	QTcpSocket **sockets;			//������socket
	QStringList city_names;			//������
	int Graph_load = 0;				//�Ƿ��ȡ������Ϣ
	int passenger_num = 0;			//�˿͵�����
	int hours = 0;					//����ʱ�� Сʱ��
	int timer_status = 0;			//��ʱ��״̬ 0Ϊδ��ʼ	1Ϊ��ʱ�� 2Ϊ��ͣ��
	int path_risk_calculate = 1;	//�Ƿ���㽻ͨ��ʽ�ķ���ֵ ��Ҫ�ڶ�ȡ������Ϣǰ����
	void set_city_name();			//���ó�����
	void read_graph_from_file();	//���ļ��ж�ȡ������Ϣ
	void read_passenger_from_file();//���ļ���ȡ�˿���Ϣ
	void set_column_width();		//���������п�
	void timer_trigger();			//����ʼ ��ͣ��ť�������ʱ��״̬
	void update_timer();			//���½�����ʾʱ��
	void update_passengers();		//�����г˿�״̬���и���
	void timer_pause();				//����ʱ��״̬��Ϊ��ͣ
	void get_passenger();			//�Ի���ʽ���ӳ˿�
	void add_passenger();			//get_passenger()�ɹ��Ļ�����
	void put_passenger_to_tree_up(Passenger *p);		//���˿���Ϣ�ŵ��ϰ벿����Ϣ��
	void put_passenger_to_tree_down(int index);			//����˿͹滮�� ���滮�͵�ǰ״̬�ŵ��²�����
	void get_schedule();			//�Ի���ʽ���Ӱ��
	void add_schedule();			//get_schedule()�ɹ��Ļ�����
	bool eventFilter(QObject *obj, QEvent *e);	//�����¼�������������໭ͼ
	void paint();					//��������໭ͼ
	void accept_connetion();		//����TCP����
	void read();					//��TCP���ӻ�ȡ��Ϣ
	void write_status();			//������дĿǰ������Ϣ ��֤�û�����ӳ˿ͺϷ�(���Ŀ�Ĳ�����������Ŀ)
	void write_plan(int index,int choice=0);	//������indexд�û�ѡ�����ϸ�滮
	void write_status_now(QString &str,int i);	//������д�˿�Ŀǰ״̬
	void write_ranking();						//������д ������û��ṩ��ѡ��
	void out_log(QString &s,int i);				//�����־
	void set_path_risk();						//����ť���ټ��㽻ͨ��ʽ����
};
