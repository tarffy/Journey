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
	double danger, min_danger;			//当前危险度	目前最小危险度
	int len;							//交通方式选择
	int time, time_when_min;			//当前时间	最小危险度时时间
	int list_filled = 0;				//list已有路径的数量
	char *schedule_choice_now;			//目前的班次选择	用于DFS时存储目前班次选择
	char *schedule_choice_final;		//当前最小危险度的班次选择
	char *path_now;						//目前选择路径		
	char *path_finally;					//当前最小危险度路径选择
	//储存风险值最低的五条路径的信息 包括路径、班次选择、风险值和用时 实际上可以取代time_when_min,schedule_choice_final,path_finally
	vector< pair< pair<char *, char *>, pair<double, int> >> list;	
};
class Passenger {			//乘客的情况
	//已找出最佳路径后 已经到达的位置	值为1表示已到达编号1的城市 位置在1城市或1城市到下一个城市的路上
	int cur;		
	int time_to_arrive_next;	//储存乘客到达下一个城市的小时数 判断
	int last_hour = -1;			//储存乘客上次更新状态时的小时数 避免重复更新出现不可预料的错误
	//status[0]为1表示乘客在两个城市之间	此时status[1],status[2]表示在哪两个城市间
	//status[0]为0表示在一个城市停留	statusp[1]表示其所在城市	实际上可代替上面cur
	int status[3];
	void set_status(int status_now, int src, int dest = 0) { 
		status[0] = status_now;
		status[1] = src;
		status[2] = dest;
	}
public:
	Passenger(int source, int destination, int start_time = 0, int method = 0, int hours_limit = 0);		//从五元组构建对象
	Passenger(const char *file_name);	//从文件读入乘客信息
	int source, destination;	//起始点和终止点 用城市编号表示
	int method;					//乘客采用出行策略 0	最低风险	1 限定时间最低风险
	int start_time;				//出发时间
	int time_limit;				//时间限制	method为1时有效
	int arrived = 0;			//乘客是否到达目的地 用来判断是否进行状态更新
	ofstream *out_file;			//乘客旅行日志的输出文件流
	path_result *pr;			//储存乘客出行规划计算结果

	void set_passenger(int source, int destination, int start_time = 0, int method = 0, int hours_limit = 0);
	void set_path_result();						//初始化path_result
	QString update(Graph *g, int hours);		//对乘客信息进行更新 hours为现在小时数 返回乘客状态字符串
	void set_ofstream(int num);					//将乘客与num号输出文件绑定
	int *get_status() { return status; }

	void out_path_result(int if_finnally = 0);//参数2为是否打印结果	用于测试
};