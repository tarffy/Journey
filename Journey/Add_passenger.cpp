#include "Add_passenger.h"
#include <qpushbutton.h>
#include <qmessagebox.h>
Add_passenger::Add_passenger(QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);
	setFixedSize(381, 412);
	ui.input_limit->setReadOnly(1);
	connect(ui.button_accept, &QPushButton::clicked,this,&Add_passenger::check_input);
	connect(ui.choose_method1, &QRadioButton::clicked, [=]() {
		ui.input_limit->setReadOnly(0);
	});
	connect(ui.choose_method0, &QRadioButton::clicked, [=]() {
		ui.input_limit->setReadOnly(1);
	});
	connect(ui.button_declined, &QPushButton::clicked, this, &Add_passenger::input_ready);
	connect(ui.button_declined, &QPushButton::clicked, this, &Add_passenger::hide);
}

Add_passenger::~Add_passenger()
{
}

void Add_passenger::check_input()
{
	int input[5];
	input[0]= ui.input_sorce->text().toInt();
	input[1]= ui.input_destination->text().toInt();
	input[2] = ui.time_start->text().toInt();
	input[3] = ui.choose_method1->isChecked();
	input[4]=ui.input_limit->text().toInt();

	if (input[0] > city_num || input[1] > city_num) {
		QMessageBox::information(this, "提示", QString( "起点城市或目标城市编号超过城市数量，请重新输入！\n最大城市编号为%1。").arg(city_num));
		return;
	}
	if (input[0] == input[1]) {
		QMessageBox::information(this, "提示", QString("起点城市和目的城市不应相同，请重新输入。"));
		return;
	}
	if (ui.choose_today->isChecked()) {
		if (input[2] < hour_now % 24) {
			QMessageBox::information(this, "提示", QString("现在时间是%1日%2时，乘客开始时间已超过现在时间，请重新输入！").arg(hour_now/24).arg(hour_now%24));
			return;
		}
		passenger_info[2] = hour_now / 24 * 24 + input[2];
	}
	else {
		passenger_info[2] = hour_now / 24 * 24 + input[2]+24;
	}
	passenger_info[0] = input[0];
	passenger_info[1] = input[1];
	passenger_info[3] = input[3];
	passenger_info[4] = input[4];
	if_success = 1;
	hide();
	emit input_ready();
}
