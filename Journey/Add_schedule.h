#pragma once

#include <QDialog>
#include "ui_Add_schedule.h"

class Add_schedule : public QDialog
{
	Q_OBJECT

public:
	Add_schedule(QWidget *parent = Q_NULLPTR);
	~Add_schedule();
	int city_num;
	int if_success = 0;
	int schedule_info[5];
signals:
	void input_ready();
private:
	Ui::Add_schedule ui;
	void check_input();
};
