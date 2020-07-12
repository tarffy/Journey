#include "Passenger.h"
#include <fstream>
#include <iostream>
#define RANKING_NUM 5

using std::ifstream;
using std::cout;
using std::endl;
#define MAX_CHOICE_LENGTH 16
Passenger::Passenger(int source, int destination, int start_time, int method, int hours_limit)
{
	this->source = source ;
	this->destination = destination ;
	this->start_time = start_time;
	this->method = method;
	this->time_limit = hours_limit;
	set_path_result();
	cur = 0;
	time_to_arrive_next = -1;
	pr->list.resize(RANKING_NUM);
	for (int i = 0; i < RANKING_NUM; i++) {
		auto tem = pr->list[i];
		tem.second.first = 1000;
	}
}


Passenger::Passenger(const char * file_name)
{
	ifstream in(file_name);
	int tem[5];
	for (int i = 0; i < 5; i++)in >> tem[i];
	set_passenger(tem[0], tem[1], tem[2], tem[3], tem[4]);
	set_path_result();
	cur = 0;
	time_to_arrive_next = -1;
	in.close();
	pr->list.resize(RANKING_NUM);
	for (int i = 0; i < RANKING_NUM; i++) {
		pr->list[i].second.first = 1000;
	}
}

void Passenger::set_passenger(int source, int destination, int start_time, int method, int hours_limit)
{
	this->source = source ;
	this->destination = destination;
	this->start_time = start_time;
	this->method = method;
	this->time_limit = hours_limit;
}

void Passenger::set_path_result()
{

		pr = new path_result;
		pr->schedule_choice_now = new char[MAX_CHOICE_LENGTH];
		pr->schedule_choice_final = new char[MAX_CHOICE_LENGTH];
		for (int i = 0; i < MAX_CHOICE_LENGTH; i++)pr->schedule_choice_final[i] = pr->schedule_choice_now[i] = '\0';
		pr->danger = 0;
		pr->time = start_time;
		pr->min_danger = 1000;
		pr->len = 0;
		pr->path_finally = new char[MAX_CHOICE_LENGTH + 1];
		for (int i = 0; i < MAX_CHOICE_LENGTH + 1; i++)pr->path_finally[i] = '\0';

}

void Passenger::out_path_result(int if_finally)
{
		if (pr->min_danger==1000) {
			cout << "No such path!" << endl;
			return;
		}
		if (if_finally) {
			cout << "Finally path:" << pr->path_finally << endl;
		}
		cout << "min danger and time: " << pr->min_danger << "   " << pr->time_when_min << endl;
		int cur = 0;
		cout << "choice final : ";
		while (pr->schedule_choice_final[cur] != 127)cout << pr->schedule_choice_now[cur++] - 0;
		cout << endl;

	
}

QString Passenger::update(Graph * g, int hours)
{
	QString result;
	if (pr->min_danger==1000||arrived||last_hour == hours)return QString("");
	schedule ***schedules = g->get_schedules();
	int days = hours / 24, hour = hours % 24;
	char *path_f = pr->path_finally, *schedule_f = pr->schedule_choice_final;
	schedule *schedule_now = schedules[path_f[cur]][path_f[cur + 1]];
	int tem_schedule_start=schedule_now->time_start[schedule_f[cur]];
	int time_to_leave = hours / 24 * 24 + tem_schedule_start >= hours ?
		hours / 24 * 24 + tem_schedule_start : hours / 24 * 24 + tem_schedule_start + 24;
	last_hour = hours;
	if (hours < start_time) {
		result= "乘客还没出发。";
		set_status(0, (int)path_f[cur]);
		return result;
	}
	if (time_to_arrive_next !=hours) {		//没到下一站
		if (hours  < time_to_leave&&time_to_arrive_next==-1) {
			result=QString("在%1城等班次。").arg((int)path_f[cur]);
			set_status(0, (int)path_f[cur]);
		}
		else if (hours  == time_to_leave && time_to_arrive_next == -1) {
			QString transportation;
			if (schedule_now->type[cur] == 0)transportation = "汽车";
			else if(schedule_now->type[cur] == 1)transportation = "火车";
			else transportation = "飞机";
			result = QString("从%1城乘%2出发。").arg((int)path_f[cur]).arg(transportation);
			time_to_arrive_next = hours + schedule_now->time_exist[schedule_f[cur]];
			set_status(1, (int)path_f[cur], (int)path_f[cur + 1]);
		}
		else {
			result = QString("在%1城和%2城之间。").arg(status[1]).arg(status[2]);
			set_status(1, (int)path_f[cur], (int)path_f[cur + 1]);
		}
	}
	else {
		cur++;		//到了下一站
		time_to_arrive_next = -1;
		if(path_f[cur]==destination){
			result = QString("到达%1城目的地。").arg((int)path_f[cur]);
			arrived = 1;
			set_status(0, (int)path_f[cur]);
			return result;
		}
		schedule_now = schedules[path_f[cur]][path_f[cur + 1]];
		tem_schedule_start = schedule_now->time_start[schedule_f[cur]];
		time_to_leave = hours / 24 * 24 + tem_schedule_start >= hours ?
		hours / 24 * 24 + tem_schedule_start : hours / 24 * 24 + tem_schedule_start + 24;
		if (time_to_leave == hours) {
			QString transportation;
			if (schedule_now->type[cur] == 0)transportation = "汽车";
			else if (schedule_now->type[cur] == 1)transportation = "火车";
			else transportation = "飞机";
			result = QString("到达%1城立刻乘%2前往%3城。").arg((int)path_f[cur])
				.arg(transportation).arg((int)path_f[cur+1]);
			time_to_arrive_next= hours + schedule_now->time_exist[schedule_f[cur]];
			set_status(1, (int)path_f[cur], (int)path_f[cur + 1]);
		}
		else {
			result = QString("到达%1城并等待班次。").arg((int)path_f[cur]);
			set_status(0, (int)path_f[cur]);
		}
	}
	return result;
}

void Passenger::set_ofstream(int num)
{
	out_file = new ofstream(QString("log/Passenger%1_log.txt").arg(num).toStdString());
}
