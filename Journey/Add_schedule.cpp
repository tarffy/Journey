#include "Add_schedule.h"
#include <qpushbutton.h>
#include <qmessagebox.h>
Add_schedule::Add_schedule(QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);
	setFixedSize(390, 388);
	connect(ui.button_accept, &QPushButton::clicked, this, &Add_schedule::check_input);
	connect(ui.button_declined, &QPushButton::clicked, this, &Add_schedule::input_ready);
	connect(ui.button_declined, &QPushButton::clicked, this, &Add_schedule::hide);
}

Add_schedule::~Add_schedule()
{
}

void Add_schedule::check_input()
{
	int input[5];
	input[0] = ui.input_sorce->text().toInt();
	input[1] = ui.input_destination->text().toInt();
	if (ui.type_flight->isChecked()) {
		input[2] = 2;
	}
	else if (ui.type_trian->isChecked()) {
		input[2] = 1;
	}
	else input[2] = 0;
	input[3] = ui.time_start->text().toInt();
	input[4] = ui.time_exist->text().toInt();
	if (input[0] > city_num || input[1] > city_num) {
		QMessageBox::information(this, "提示", QString("起点城市或目标城市编号超过城市数量，请重新输入！\n最大城市编号为%1。").arg(city_num));
		return;
	}
	if (input[0] == input[1]) {
		QMessageBox::information(this, "提示", QString("起点城市和目的城市不应相同，请重新输入。"));
		return;
	}
	for (int i = 0; i < 5;i++) {
		schedule_info[i] = input[i];
	}
	if_success = 1;
	hide();
	emit input_ready();
}
