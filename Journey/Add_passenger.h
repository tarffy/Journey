#pragma once
#pragma execution_character_set("utf-8")
#include <QWidget>
#include "ui_Add_passenger.h"
#include <qdialog.h>
class Add_passenger : public QDialog
{
	Q_OBJECT

public:
	int passenger_info[5];
	int city_num;
	int hour_now;
	int if_success = 0;
	Add_passenger(QWidget *parent = Q_NULLPTR);
	~Add_passenger();
signals:
	void input_ready();
private:
	Ui::Add_passenger ui;
	void check_input();
};
