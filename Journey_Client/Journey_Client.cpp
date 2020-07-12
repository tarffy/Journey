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
	case 0://����������� ����ʱ����Ϣ
	{
		passenger_adder->city_num = list.at(1).toInt();
		passenger_adder->hour_now = list.at(2).toInt();
		break;
	}
	case 1://�������й滮
	{
		ui.labelplan->setText(list.at(1));
		break;
	}
	case 2://���뵱ǰʱ����Ϣ
	{
		ui.liststatus->addItem(list.at(1));
		ui.liststatus->setCurrentRow(ui.liststatus->count());
		break;
	}
	case 3://����ranking�ȴ�ѡ��
	{	
		QString res;
		size = list.at(1).toInt();
		for (int i = 0; i < size; i++) {
			res.append(list.at(i + 2));
			res.append('\n');
		}
		ui.labelplan->setText(res);
		if (size == 0) {
			ui.labelplan->setText("����������·����");
		}
		break;
	}
	}
}

void Journey_Client::write_passenger()
{
	int *info = passenger_adder->passenger_info;
	QString str = QString("0 %1 %2 %3 %4 %5").arg(info[0]).arg(info[1]).arg(info[2]).arg(info[3]).arg(info[4]);
	ui.labelinfo->setText(QString("��ʼ���б��:%1\tĿ�ĳ��б��:%2\t����ʱ��:%3��%4ʱ\n%5")
		.arg(info[0]).arg(info[1]).arg(info[2] / 24).arg(info[2] % 24).arg(info[3] ? QString("��ʱ������� ��ʱΪ%1").arg(info[4]) : QString("����ʱ�������")));
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
	int choice = QInputDialog::getItem(this, "ѡ�񷽰�", "��ѡ�񷽰����", list, 0, false).toInt();
	QString result = QString("1 %1").arg(choice-1);		//����ȥ0��ʼ���±�
	socket->write(result.toStdString().data());
}
