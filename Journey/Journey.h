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
	Graph *graph;					//城市和班次信息
	Passenger **passengers;			//乘客信息
	QTimer *timer;					//定时器
	Add_passenger *passenger_adder;	//用于对话框方式添加乘客
	Add_schedule *schedule_adder;	//用于对话框方式添加班次
	QTcpServer *server;				//服务端
	QTcpSocket **sockets;			//建立的socket
	QStringList city_names;			//城市名
	int Graph_load = 0;				//是否读取城市信息
	int passenger_num = 0;			//乘客的数量
	int hours = 0;					//现在时间 小时计
	int timer_status = 0;			//计时器状态 0为未开始	1为计时中 2为暂停中
	int path_risk_calculate = 1;	//是否计算交通方式的风险值 需要在读取城市信息前设置
	void set_city_name();			//设置城市名
	void read_graph_from_file();	//从文件中读取城市信息
	void read_passenger_from_file();//从文件读取乘客信息
	void set_column_width();		//调整界面列宽
	void timer_trigger();			//按开始 暂停按钮后调整计时器状态
	void update_timer();			//更新界面显示时间
	void update_passengers();		//对所有乘客状态进行更新
	void timer_pause();				//将定时器状态设为暂停
	void get_passenger();			//对话框方式增加乘客
	void add_passenger();			//get_passenger()成功的话调用
	void put_passenger_to_tree_up(Passenger *p);		//将乘客信息放到上半部分信息树
	void put_passenger_to_tree_down(int index);			//计算乘客规划后 将规划和当前状态放到下部分树
	void get_schedule();			//对话框方式增加班次
	void add_schedule();			//get_schedule()成功的话调用
	bool eventFilter(QObject *obj, QEvent *e);	//进行事件过滤用于在左侧画图
	void paint();					//用于在左侧画图
	void accept_connetion();		//建立TCP连接
	void read();					//从TCP连接获取信息
	void write_status();			//向服务端写目前城市信息 保证用户端添加乘客合法(起点目的不超过城市数目)
	void write_plan(int index,int choice=0);	//向服务端index写用户选择的详细规划
	void write_status_now(QString &str,int i);	//向服务端写乘客目前状态
	void write_ranking();						//向服务端写 程序给用户提供的选择
	void out_log(QString &s,int i);				//输出日志
	void set_path_risk();						//按按钮后不再计算交通方式风险
};
