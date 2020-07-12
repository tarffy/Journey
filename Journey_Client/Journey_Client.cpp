#include "Journey_Client.h"
#include <qaction.h>
#include <qinputdialog.h>

Journey_Client::Journey_Client(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);
	passenger_adder = new Add_passenger;

	connect(ui.actionaddpassenger, &QAction::triggered, this, &Journey_Client::get_passenger);
	connect(passenger_adder, &Add_passenger::input_ready, this, &Journey_Client::write_passenger);
	connect(ui.actionchoose, &QAction::triggered, this, &Journey_Client::choose_plan);

	socket = new QTcpSocket(this);
	socket->connectToHost(QHostAddress::LocalHost, 8888);
	connect(socket, &QTcpSocket::readyRead, this, &Journey_Client::read);
}
void Journey_Client::get_passenger()
{
	passenger_adder->if_success = 0;
	passenger_adder->exec();

}

void Journey_Client::read()
{
	QString str = socket->readAll();
	QStringList list = str.split(' ');
	switch (list.at(0).toInt())
	{
	case 0://读入城市数量 现在时间信息
	{
		passenger_adder->city_num = list.at(1).toInt();
		passenger_adder->hour_now = list.at(2).toInt();
		break;
	}
	case 1://读入旅行规划
	{
		ui.labelplan->setText(list.at(1));
		break;
	}
	case 2://读入当前时刻信息
	{
		ui.liststatus->addItem(list.at(1));
		ui.liststatus->setCurrentRow(ui.liststatus->count());
		break;
	}
	case 3://读入ranking等待选择
	{	
		QString res;
		size = list.at(1).toInt();
		for (int i = 0; i < size; i++) {
			res.append(list.at(i + 2));
			res.append('\n');
		}
		ui.labelplan->setText(res);
		if (size == 0) {
			ui.labelplan->setText("无满足条件路径。");
		}
		break;
	}
	}
}

void Journey_Client::write_passenger()
{
	int *info = passenger_adder->passenger_info;
	QString str = QString("0 %1 %2 %3 %4 %5").arg(info[0]).arg(info[1]).arg(info[2]).arg(info[3]).arg(info[4]);
	ui.labelinfo->setText(QString("起始城市编号:%1\t目的城市编号:%2\t出发时间:%3日%4时\n%5")
		.arg(info[0]).arg(info[1]).arg(info[2] / 24).arg(info[2] % 24).arg(info[3] ? QString("限时风险最低 限时为%1").arg(info[4]) : QString("不限时风险最低")));
	socket->write(str.toStdString().data());
}

void Journey_Client::choose_plan()
{

	QString str;
	int i;
	for (i = 1; i <size; i++) {
			str.append(QString("%1 ").arg(i));		
	}
	str.append(QString("%1").arg(i));
	QStringList list=str.split(' ');
	int choice = QInputDialog::getItem(this, "选择方案", "请选择方案序号", list, 0, false).toInt();
	QString result = QString("1 %1").arg(choice-1);		//传回去0开始的下标
	socket->write(result.toStdString().data());
}
