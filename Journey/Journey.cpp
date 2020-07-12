#include "Journey.h"
#include <qfiledialog.h>
#include <qstring.h>
#include <qaction.h>
#include <qsize.h>
#include <qtimer.h>
#include <qmessagebox.h>
#include <qpainter.h>

#define MAX_PASSENGERS 16
#define TIMER_INTERNAL 2000 //10000
Journey::Journey(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);
	ui.board->installEventFilter(this);		//画图
	server = new QTcpServer(this);
	server->listen(QHostAddress::Any, 8888);
	sockets = new QTcpSocket*[MAX_PASSENGERS];
	for (int i = 0; i < MAX_PASSENGERS; i++)sockets[i] = nullptr;
	connect(server, &QTcpServer::newConnection, this, &Journey::accept_connetion);

	passengers = new Passenger*[MAX_PASSENGERS];

	passenger_adder = new Add_passenger;
	schedule_adder = new Add_schedule;
	set_column_width();
	set_city_name();
	timer=new QTimer(this);

	//从文件读取图
	connect(ui.actionfile, &QAction::triggered, this,&Journey::read_graph_from_file);				
	//从文件读取乘客信息
	connect(ui.actionfile_passenger, &QAction::triggered, this, &Journey::read_passenger_from_file);
	//从对话框读入乘客信息
	connect(ui.actionaddpassenger, &QAction::triggered, this, &Journey::timer_pause);
	connect(ui.actionaddpassenger, &QAction::triggered, this, &Journey::get_passenger);
	connect(passenger_adder, &Add_passenger::input_ready, this, &Journey::add_passenger);
	//从对话框读入班次信息
	connect(ui.actionaddschedule, &QAction::triggered, this, &Journey::timer_pause);
	connect(ui.actionaddschedule, &QAction::triggered, this, &Journey::get_schedule);
	connect(schedule_adder, &Add_schedule::input_ready, this, &Journey::add_schedule);
	//不计算路上风险
	connect(ui.actionpathrisk, &QAction::triggered, this, &Journey::set_path_risk);
	//更新label显示的日和时间
	connect(timer, &QTimer::timeout, this, &Journey::update_timer);
	//更新所有乘客信息	修改QTree
	connect(timer, &QTimer::timeout, this, &Journey::update_passengers);
	//按时间按钮更改状态
	connect(ui.time_trigger, &QPushButton::clicked, this, &Journey::timer_trigger);
	//关闭窗口
	connect(ui.actionexit, &QAction::triggered, this, &QWidget::close);

}

void Journey::set_city_name()
{
	QString str("0北京 1成都 2福州 3广州 4哈尔滨 5海口 6呼和浩特 7济南 8昆明 9拉萨 10兰州 11南宁 12上海 13台北 14乌鲁木齐 15武汉");
	city_names = str.split(' ');
}

void Journey::read_graph_from_file()
{	
	//已经读取了
	if (Graph_load) {
		QMessageBox::information(this, "提升", "已读入城市数据。");
		return;
	}
	QString file_name = QFileDialog::getOpenFileName(this, "读入图信息", "D:\\file\\qt\\Journey", "InputFile (*.txt *.in)");
	if (file_name == "")return;
	QByteArray ba = file_name.toLatin1();
	graph = new Graph(ba.data());
	graph->set_path_risk_calculate(path_risk_calculate);
	Graph_load = 1;
	paint();
	if (passenger_num)for (int i = 0; i < passenger_num; i++) {
			graph->calculate_min(passengers[passenger_num]);
			put_passenger_to_tree_down(passenger_num);

	}
}

void Journey::read_passenger_from_file()
{
	QString file_name = QFileDialog::getOpenFileName(this, "读入乘客信息", "D:\\file\\qt\\Journey", "InputFile (*.txt *.in)");
	if (file_name == "")return;
	QByteArray ba = file_name.toLatin1();
	passengers[passenger_num] = new Passenger(ba.data());
	Passenger *p_now = passengers[passenger_num];
	p_now->set_ofstream(passenger_num);

	put_passenger_to_tree_up(p_now);
	if (Graph_load) {
		graph->calculate_min(passengers[passenger_num]);
		put_passenger_to_tree_down(passenger_num);
	}
		


	passenger_num++;
}

void Journey::set_column_width()
{
	ui.passenger_tree->setColumnWidth(0, 120);
	ui.passenger_tree->setColumnWidth(1, 40);
	ui.passenger_tree->setColumnWidth(2, 40);
	ui.passenger_tree->setColumnWidth(3, 40);
	ui.passenger_tree->setColumnWidth(4, 40);
}

void Journey::timer_trigger()
{
	if (!Graph_load || !passenger_num) {
		QMessageBox::information(this, "提示", "未读入图信息或未读入乘客信息！");
		return;
	}
	switch (timer_status)
	{
	case 0:
		timer->start(TIMER_INTERNAL);
		ui.time_trigger->setText("暂停");
		timer_status = 1;
		break;
	case 1:
		timer->stop();
		ui.time_trigger->setText("继续");
		timer_status = 2;
		break;
	case 2:
		timer->start();
		ui.time_trigger->setText("暂停");
		timer_status = 1;
		break;
	}
}

void Journey::update_timer()
{
	hours++;
	ui.days_and_hours->setText(QString("%1日%2时").arg(hours/24).arg(hours%24));
}



void Journey::update_passengers()
{
	QString tem;
	for (int i = 0; i < passenger_num; i++) {
		tem = passengers[i]->update(graph, hours);
		out_log(tem,i);
		if (sockets[i])write_status_now(tem,i);
		if (tem == "") {
			
			continue;
		}
		QTreeWidgetItem * passenger_top_item = ui.passenger_status_tree->topLevelItem(i);
		passenger_top_item->removeChild(passenger_top_item->takeChild(1));
		QTreeWidgetItem *status_now = new QTreeWidgetItem(QStringList() << tem);
		passenger_top_item->insertChild(1, status_now);
	}
	update();
}

void Journey::timer_pause()
{
	if (timer_status == 1) {
		timer->stop();
		ui.time_trigger->setText("继续");
		timer_status = 2;
	}
}

void Journey::get_passenger()
{
	if (Graph_load == 0) {
		QMessageBox::information(this, "提示", QString("未读入城市。"));
		return;
	}
	passenger_adder->if_success = 0;
	passenger_adder->city_num = graph->get_city_num();
	passenger_adder->hour_now = get_hours();
	passenger_adder->exec();

}

void Journey::add_passenger()
{

	if (passenger_adder->if_success == 0)return;
	int *info = passenger_adder->passenger_info;
	passengers[passenger_num] = new Passenger(info[0], info[1],info[2],info[3],info[4]);
	Passenger *p_now = passengers[passenger_num];
	p_now->set_ofstream(passenger_num);

	put_passenger_to_tree_up(p_now);
	if (Graph_load) {
		graph->calculate_min(passengers[passenger_num]);
		put_passenger_to_tree_down(passenger_num);
	}

	passenger_num++;
}

void Journey::put_passenger_to_tree_up(Passenger *p_now)
{
	QTreeWidgetItem * passenger_top_item = new QTreeWidgetItem(QStringList() << QString("乘客%1").arg(passenger_num));
	ui.passenger_tree->addTopLevelItem(passenger_top_item);
	QString detail = QString("%1,%2,%3,%4,%5").arg(passenger_num).arg(p_now->source).arg(p_now->destination)
		.arg(p_now->start_time).arg(p_now->method != 0 ? p_now->time_limit : 0);
	QTreeWidgetItem * detail_item = new QTreeWidgetItem(detail.split(','));
	passenger_top_item->addChild(detail_item);
}

void Journey::put_passenger_to_tree_down(int index)
{
	Passenger *p_now = passengers[index];
	QTreeWidgetItem * passenger_top_item = new QTreeWidgetItem(QStringList() << QString("乘客%1").arg(index));
	ui.passenger_status_tree->addTopLevelItem(passenger_top_item);
	if (p_now->pr->min_danger == 1000) {
		QTreeWidgetItem *status = new QTreeWidgetItem(QStringList() << QString("没有这种路径！"));
		passenger_top_item->addChild(status);
	}
	else {
		QString s("");
		int i = 0;
		while (p_now->pr->path_finally[i] != p_now->destination) {
			s.append(QString("%1 ").arg((int)p_now->pr->path_finally[i]));
			i++;
		}
		s.append(QString("%1 ").arg((int)p_now->pr->path_finally[i]));
		QTreeWidgetItem *path_f = new QTreeWidgetItem(QStringList() << QString("路径选择为 ") + s);
		QTreeWidgetItem *status = new QTreeWidgetItem(QStringList() << QString("未开始"));
		passenger_top_item->addChild(path_f);
		passenger_top_item->addChild(status);
	}
}

void Journey::get_schedule()
{
	schedule_adder->if_success = 0;
	schedule_adder->city_num = graph->get_city_num();
	schedule_adder->exec();
}

void Journey::add_schedule()
{
	if (schedule_adder->if_success == 0)return;
	graph->add_schedule(schedule_adder->schedule_info);
//	ui.debug->setText(QString("%1 %2 %3 %4 %5").arg(schedule_adder->schedule_info[0]).arg(schedule_adder->schedule_info[1]).arg(schedule_adder->schedule_info[2]).arg(schedule_adder->schedule_info[3]).arg(schedule_adder->schedule_info[4]));
}

bool Journey::eventFilter(QObject * obj, QEvent * e)
{
	

		if (obj == ui.board && e->type() == QEvent::Paint) {//在frame上画图
			paint();
			return true;
		}
		else {}
		return QWidget::eventFilter(obj, e);


}

void Journey::paint()
{
	int bianchang = 50, delt = bianchang / 2;
	QPixmap city = QPixmap(":/Image/city.png").scaled(bianchang, bianchang);
	QPainter pa(ui.board);
	pa.drawPixmap(0, 0, QPixmap(":/Image/map.png"));
	pa.setRenderHint(QPainter::Antialiasing);
	if (Graph_load&&graph->if_have_xy() ) {
		int city_num = graph->get_city_num();
		QString *between_citys = new QString[city_num*(1+city_num)/2];	
		QString *in_citys = new QString[city_num];
		//for between i and j (i<j)	strings[2n-i+1]*i/2+(j-i)
		for (int i = 0; i < passenger_num; i++) {
			int *status = passengers[i]->get_status();
			if (status[0]==1) {
				if (status[1] > status[2]) {
					int tem = status[2];
					status[2] = status[1];
					status[1] = tem;
				}
				int tem = (2 * city_num - status[1] + 1)*status[1] / 2 + status[2] - status[1];
				if (between_citys[tem] != "")between_citys[tem].append(',');
				between_citys[tem].append(QString("乘客%1").arg(i));
			}
			else if(status[0] == 0){
				int tem = status[1];
				if (in_citys[tem] != "")in_citys[tem].append(',');
				in_citys[tem].append(QString("乘客%1").arg(i));
			}
		}
		int *x = graph->get_xp(), *y = graph->get_yp();
		bool **graph_connect = graph->get_connect();
		QPen pen;
		

		for (int i = 0; i < city_num; i++) {
			for (int j = i + 1; j < city_num; j++) {
				if (graph_connect[i][j] || graph_connect[j][i]) {
					int tem = (2 * city_num - i + 1) * i / 2 + (j - i);
					if (between_citys[tem] != "") {
						pen.setWidth(3);
						pen.setColor(Qt::red);
						pa.setPen(pen); 
						pa.drawLine(x[i] + delt, y[i] + delt, x[j] + delt, y[j] + delt);
					}
					else {
						pen.setWidth(1);
						pen.setColor(Qt::black);
						pa.setPen(pen);
						pa.drawLine(x[i] + delt, y[i] + delt, x[j] + delt, y[j] + delt);
					}
				}
				
			}
		}
		pa.setFont(QFont("Times", 12));
		for (int i = 0; i < city_num; i++) {
			pa.drawPixmap(x[i], y[i], city);
			pa.drawText(x[i], y[i], city_names.at(i));
		}
		pen.setColor(QColor(255,128,64));
		pa.setPen(pen);
		pa.setFont(QFont("Times", 15,3));
		for (int i = 0; i < city_num; i++) {
			if (in_citys[i] != "")pa.drawText(x[i], y[i] + bianchang/2, in_citys[i]);
			for (int j = i + 1; j < city_num; j++) {
				int tem = (2 * city_num - i + 1) * i / 2 + j - i;
				if(between_citys[tem]!="")
					pa.drawText((x[i]+x[j])/2 + bianchang/2, (y[i] + y[j]) / 2 + bianchang/2, between_citys[tem]);
			
			}
		}

	}

}

void Journey::accept_connetion()
{
	sockets[passenger_num]= server->nextPendingConnection();
	connect(sockets[passenger_num], &QTcpSocket::readyRead, this, &Journey::read);
	write_status();
}


void Journey::read()
{
	QTcpSocket *socket_now = (QTcpSocket *)sender();
	QString str = socket_now->readAll();
	QStringList list= str.split(' ');
	switch (list.at(0).toInt())
	{
	case 0:			//读取乘客信息
	{
		int data[5];
		for (int i = 0; i < 5; i++)data[i] = list.at(i+1).toInt();
		passengers[passenger_num] = new Passenger(data[0], data[1], data[2], data[3], data[4]);
		Passenger *p_now = passengers[passenger_num];
		p_now->set_ofstream(passenger_num);
		put_passenger_to_tree_up(p_now);
		if (Graph_load) {
			graph->calculate_min(passengers[passenger_num]);
			
		}
		write_ranking();
		//write_plan();
		passenger_num++;
		break;
	}
	case 1:
	{
		int cur = 0;
		while (socket_now != sockets[cur])cur++;
		int choice = list.at(1).toInt();
		auto tem = passengers[cur]->pr->list[choice];
		delete[] passengers[cur]->pr->path_finally;
		delete[] passengers[cur]->pr->schedule_choice_final;
		passengers[cur]->pr->path_finally = tem.first.first;
		passengers[cur]->pr->schedule_choice_final = tem.first.second;
		put_passenger_to_tree_down(cur);
		write_plan(cur,choice);
		
		break;
	}
	}
	
}

void Journey::write_status()	//告诉图的城市数量和现在时间
{
	QString str = QString("0 %1 %2").arg(Graph_load ? graph->get_city_num() : 1000).arg(get_hours());
	sockets[passenger_num]->write(str.toStdString().data());
}

void Journey::write_plan(int index,int choice)
{
	QString result("1 ");
	int cur = 0,start_hours=passengers[index]->start_time,end_hours;
	Passenger *p_now = passengers[index];
	auto &rank_choice = p_now->pr->list[choice];
	char *str1 = rank_choice.first.first, *str2 = rank_choice.first.second;
	while (str1[cur] != p_now->destination) {
		schedule *s_now = graph->get_schedules()[str1[cur]][str1[cur+1]];
		QString transportation;
		if (s_now->type[(int)str2[cur]] == 0)transportation = "汽车";
		else if (s_now->type[(int)str2[cur]] == 1)transportation = "火车";
		else transportation = "飞机";
		int tem = start_hours / 24 * 24 + s_now->time_start[str2[cur]];
		start_hours = tem >= start_hours ? tem : tem + 24;
		end_hours = start_hours + s_now->time_exist[str2[cur]];
		result.append(QString("%1日%2时从%3乘%4出发,").arg(start_hours / 24).arg(start_hours % 24)
			.arg(city_names.at((int)str1[cur]) ).arg(transportation));
		result.append(QString("于%1日%2时到达%3城。").arg(end_hours/24).arg(end_hours % 24)
			.arg(city_names.at((int)str1[cur + 1]) ));
		start_hours = end_hours;
		cur++;
	}
	result.append(QString("到达目的地,风险为%1，耗时为%2。 ").arg(rank_choice.second.first).arg(rank_choice.second.second));
	*p_now->out_file << QString("用户选择的规划为：").toStdString()<<result.split(' ').at(1).toStdString()<<'\n';
	sockets[index]->write(result.toStdString().data());
}

void Journey::write_status_now(QString &str,int i)
{
	QString result=QString("2 %1日%2时,").arg(hours/24).arg(hours%24);
	if (str == "") { 
		if (!sockets[i]->isOpen())return;
		result.append("已到达目的地。"); 
		sockets[i]->write(result.toStdString().data());
		sockets[i]->close();
	}
	else {
		result.append(str);
		sockets[i]->write(result.toStdString().data());
	}
}

void Journey::write_ranking()
{
	QString result("3 ");
	Passenger *p_now = passengers[passenger_num];
	path_result *pr_now = p_now->pr;
	int size = pr_now->list_filled;
	
	result.append(QString("%1 ").arg(size));
	for (int i = 0; i < size; i++) {
	QString tem1(QString("计划%1：").arg(i+1));
	char *tem_str = pr_now->list[i].first.first;
	int  cur = 0;
	while ((int)tem_str[cur] != p_now->destination) {
		tem1.append(QString("%1城，").arg(int(tem_str[cur])));
		cur++;
	}
	tem1.append(QString("%1城。").arg(int(tem_str[cur])));
	tem1.append(QString("风险值为%1，耗时为%2。 ").arg(pr_now->list[i].second.first).arg(pr_now->list[i].second.second));
	result.append(tem1);
}
	qDebug() << result;
	qDebug() << size;
	sockets[passenger_num]->write(result.toStdString().data());
}

void Journey::out_log(QString &s,int i)
{
	ofstream *file_now = passengers[i]->out_file;
	if (passengers[i]->arrived) {
		if (file_now->is_open()) {
			*file_now << QString("%1日%2时,").arg(hours / 24).arg(hours % 24).toStdString() <<
				s.toStdString() << '\n';
			file_now->close();
		}
		return;
	}
	*file_now << QString("%1日%2时,").arg(hours / 24).arg(hours % 24).toStdString()<<
		s.toStdString() << '\n';

}

void Journey::set_path_risk()
{
	path_risk_calculate = 0;
	QMessageBox::information(this, "提示", "本次运行期间不计算交通工具风险值");
}
