#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_Journey_Client.h"
#include "Add_passenger.h"
#include <QtNetwork>
#include <qnetwork.h>
class Journey_Client : public QMainWindow
{
    Q_OBJECT

public:
    Journey_Client(QWidget *parent = Q_NULLPTR);

private:
    Ui::Journey_ClientClass ui;
	Add_passenger *passenger_adder;
	QTcpSocket *socket;
	int size=5;//方案数量
	void get_passenger();
	void add_passenger();
	void read();
	void write_passenger();
	void choose_plan();
};
