#pragma once
/*
	存放图和所有班次
*/
#include "Passenger.h"
#define MAX_SCHEDULE 16		//从一城到另一城最大数量
class Passenger;
struct schedule {						//从一个城市到另一个城市的数量
	char type[MAX_SCHEDULE];			//班次类型 0为汽车1为火车2为飞机
	char time_start[MAX_SCHEDULE];		//班次开始时间
	char time_exist[MAX_SCHEDULE];		//班次持续时间
	int len;							//已有的班次数量
};
struct link {		//链表节点，用于简化DFS
	int val;
	link *next;
};
class Graph {
	int city_num;					//城市数量
	char *city_danger;				//风险度 0低 1中 2高
	bool **connected;				//connected[i][j] 从i到j是否直接可达 可达为1不可达为0
	schedule ***schedule_between;	//schedule[i][j]	为指向从i到j班次的指针 无路径为空指针	
	int have_xy = 0;				//城市信息文件是否有城市信息
	int *x, *y;						//储存城市的位置
	int path_risk_calculate = 1;	//是否计算交通方式产生的风险值

	void auto_set();				//构建Graph类 初始化对象
	int if_negtive(int a);			//计算时间差使用
	void set_danger(int *danger);	//设置城市危险度
	void _calculate_min(char *path, Passenger *p);	//dfs找到一条路径时调用	计算乘客p在路径path上的最小风险
	void dfs(link *start, link *pre, bool *book, Passenger *p);	//calculate_min调用 查找乘客p满足条件的路径
	char *get_path(link *start, Passenger *p);		//dfs找到路径时调用 返回路径字符串
public:
	Graph(const char *file_name);				//从文件构造类
	void calculate_min(Passenger *p);			//计算乘客p路径 基础算法

	void add_schedule(int *a);					//读入五元数组增加新的班次
	void set_path_risk_calculate(int a) { path_risk_calculate = a; }	//用于将path_risk_calculate置0 从而不计算交通方式的风险值
	schedule ***get_schedules() { return schedule_between; }
	int get_city_num() { return city_num; }
	bool **get_connect() { return connected; }
	int *get_xp() { return x; }
	int *get_yp() { return y; }
	int if_have_xy() { return have_xy; }
};
