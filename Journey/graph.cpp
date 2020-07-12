#include "graph.h"
#include <iostream>
#include <fstream>
#include <qdebug.h>
#define MAX_PATHS 256
#define bianchang 30
using std::cin;
using std::cout;
using std::endl;
using std::ifstream;

Graph::Graph(const char *file_name)
{
	ifstream in(file_name);
	in >> city_num;
	auto_set();
	int *city_danger = new int[city_num];
	for (int i = 0; i < city_num; i++)in >> city_danger[i];
	set_danger(city_danger);
	int times;
	int schedule[5];
	in >> times;
	for (int i = 0; i < times; i++) {
		for (int j = 0; j < 5; j++)in >> schedule[j];
		add_schedule(schedule);
	}
	int tem;
	in >> tem;
	if (!in.eof()) {
		have_xy = 1;
		x = new int[city_num];
		y = new int[city_num];
		for (int i = 0; i < city_num; i++) {
			int xtem, ytem;
			in >> xtem>>ytem;
			x[i] = xtem - bianchang;
			y[i] = ytem - bianchang;
		}
	}
	in.close();
}
void Graph::auto_set()
{
	city_danger = new char[city_num];
	connected = new bool*[city_num];
	for (int i = 0; i < city_num; i++)connected[i] = new bool[city_num];
	for (int i = 0; i < city_num; i++)
		for (int j = 0; j < city_num; j++)
			connected[i][j] = 0;
	schedule_between = new schedule**[city_num];
	for (int i = 0; i < city_num; i++)schedule_between[i] = new schedule*[city_num];
	for (int i = 0; i < city_num; i++)
		for (int j = 0; j < city_num; j++)
			schedule_between[i][j] = nullptr;
}

void Graph::add_schedule(int * res)	 //少一个最大schedule处理
{	
	connected[res[0]][res[1]] = 1;
	if (schedule_between[res[0]][res[1]] == nullptr) {
		schedule_between[res[0]][res[1]] = new schedule;
		schedule_between[res[0]][res[1]]->len = 0;
	}
	if (schedule_between[res[0]][res[1]]->len == MAX_SCHEDULE)
	{
		cout << "Too many schedules!\n";
		exit(1);
	}
	schedule *tem = schedule_between[res[0]][res[1]];
	tem->type[tem->len] = res[2];
	tem->time_start[tem->len] = res[3];
	tem->time_exist[tem->len] = res[4];
	tem->len++;
}



void Graph::dfs(link *start, link *pre, bool *book, Passenger *p) {
	if (pre->val == p->destination) {
		char *path = get_path(start,p);
		////测试
		//QString res;
		//int cur = 0;
		//while (path[cur] != p->destination) {
		//	res.append(QString("%1 ").arg((int)path[cur]));
		//	cur++;
		//}
		//res.append(QString("%1 ").arg((int)path[cur]));
		//qDebug() << res;
		//p->pr->list.push_back(pair<pair<QString, QString>, pair<double, int>>(pair<QString, QString>("", ""), pair<double, int>(1000, -1)));
		
		p->pr->path_now = path;
		_calculate_min(path,p);
		delete[]path;
		return;
	}
	int source = pre->val;		//序号
	book[source] = 1;
	pre->next = new link;
	link *tem = pre->next;
	for (int i = 0; i < city_num; i++) {
		if (!book[i]&&connected[source][i]) {
			tem->val = i;
			dfs(start, tem, book,p);
		}
	}
	delete tem;
	book[source] = 0;
}
char *Graph::get_path(link *start, Passenger *p) {
	char *path = new char[city_num+1];
	link *tem = start;
	int i = 0;
	while (tem->val != p->destination) {
		path[i] = (char)tem->val;
		tem = tem->next;
		i++;
	}
	path[i++] = (char)p->destination;
	path[i] = '\0';
	return path;
}

void Graph::_calculate_min(char *path, Passenger *p) {		//计算一条确定路径上班次选择的最小值
	//path[0]是起点城市char	path[1]是下一步
	static const double city_danger_rate[3] = { 0.2,0.5,0.9 };
	static const double schedule_danger_rate[3] = { 2,5,9 };
	path_result *prnow = p->pr;
	double danger_now = prnow->danger;
	int time_now = prnow->time;
	schedule *tem = schedule_between[path[0]][path[1]];		//从起点城市到下一步城市的班次
	double source_city_rate = city_danger_rate[city_danger[path[0]]];	//源城市的危险度 0.2 0.5 0.9
	
	
	for (int i = 0; i < tem->len; i++) {
		int pause_time = if_negtive(tem->time_start[i] - time_now);
		double pause_danger;
		if (path[0] == p->source) {
			pause_danger = 0;	//起始城市不算风险
		}
		else if (pause_time == 0) {
			pause_danger = 1 * source_city_rate;	//停留时间为0加一小时停留风险
		}
		else {
			pause_danger= source_city_rate * pause_time;
		}
		prnow->danger = prnow->danger +
			pause_danger +
			path_risk_calculate * source_city_rate * schedule_danger_rate[tem->type[i]] * tem->time_exist[i];
		prnow->time = prnow->time + pause_time + tem->time_exist[i];
		prnow->schedule_choice_now[prnow->len++] = (char)i;			//值为选择的
		if (path[1] == p->destination) {
			int time_leave_src = schedule_between[prnow->path_now[0]][prnow->path_now[1]]->time_start[prnow->schedule_choice_now[0]];
			int time_delt = p->start_time % 24 <= time_leave_src ? time_leave_src - p->start_time % 24 : time_leave_src - p->start_time % 24 + 24;
			int time_used = prnow->time - p->start_time - time_delt;
			if (p->method == 0 || (p->method == 1 &&  time_used<= p->time_limit)) {	
				//生成风险前五 最低
				int insert_pos = 0,cur;
				for (cur = prnow->list_filled-1; cur >=0 ; cur--) {
					if (prnow->danger >= prnow->list[cur].second.first) {
						break;
					}
				}
				insert_pos=cur + 1;
				//将新项插入到指定位置
				if (insert_pos != 5) { 
					int flag = 1;//代表第五个位置需要delete
					if (prnow->list_filled < 5) {
						prnow->list_filled++;
						flag = 0;
					}
					if (flag) {
						auto &tem = prnow->list[prnow->list_filled-1];
						delete[] tem.first.first;
						delete[] tem.first.second;
					}
					for (int i = prnow->list_filled - 1; i >= insert_pos; i--) {
						if (i == insert_pos) {
							auto &tem = prnow->list[i];
							tem.first.first = new char[city_num+1];
							tem.first.second = new char[city_num + 1];
							int cur_char = 0;
							while (prnow->path_now[cur_char] != p->destination) {
								tem.first.first[cur_char] = prnow->path_now[cur_char];
								cur_char++;
							}
							tem.first.first[cur_char++] = (char)p->destination;
							tem.first.first[cur_char] = '\0';

							tem.second.first = prnow->danger;
							tem.second.second = time_used;

							int j_char = 0;
							for (; j_char < prnow->len; j_char ++)tem.first.second[j_char] = prnow->schedule_choice_now[j_char];
							tem.first.second[j_char] = prnow->schedule_choice_now[j_char] = 127;

						}
						else {//把前一位后移
							auto &tem = prnow->list[i];
							auto &pre= prnow->list[i-1];
							tem.first.first = pre.first.first;
							tem.first.second = pre.first.second;
							tem.second.first = pre.second.first;
							tem.second.second = pre.second.second;
						}
					}
				}
			}
			if (prnow->danger < prnow->min_danger) {
				if (p->method&&time_used > p->time_limit) {

				}
				else {					
					int cur = 0;
					while (prnow->path_now[cur] != p->destination) {
						prnow->path_finally[cur] = prnow->path_now[cur];
						cur++;
					}
					prnow->path_finally[cur++] = (char)p->destination;
					prnow->path_finally[cur] = '\0';

					prnow->min_danger = prnow->danger;
					prnow->time_when_min = time_used;
					int j = 0;
					for (; j < prnow->len; j++)prnow->schedule_choice_final[j] = prnow->schedule_choice_now[j];
					prnow->schedule_choice_final[j] = prnow->schedule_choice_now[j] = 127;	//

				}
			}
		}
		else {
			_calculate_min(path + 1, p);
		}
		prnow->danger = danger_now;
		prnow->time = time_now;
		prnow->schedule_choice_now[--prnow->len] = '\0';

	}

}

void Graph::calculate_min(Passenger *p)
{
	if (p->source > city_num || p->destination  > city_num) {
		p->pr->min_danger = 1000;
		return;
	}
	link *start = new link;
	start->val = p->source;
	bool *book = new bool[city_num];
	for (int i = 0; i < city_num; i++)book[i] = 0;
	dfs(start, start, book, p);
	delete start;
	delete[] book;
}



void Graph::set_danger(int * danger)
{
	for (int i = 0; i < city_num; i++)
		city_danger[i] = (char)danger[i];
}


int Graph::if_negtive(int a)
{
	if (a < 0)  return a + 24 * ((24 - a) / 24);
	else return a;
}



