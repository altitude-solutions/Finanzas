#pragma once

#include <QtWidgets/QWidget>
#include "ui_Login.h"
#include "TabsWindow.h"

class Login : public QWidget {
	Q_OBJECT

public:
	Login(QWidget *parent = Q_NULLPTR);

signals:
	void authDataRetrieved (QString address, QString userName, QString realName, QString token);

private slots:
	void onLoginButtonClicked ();
	void onLogout ();

private:
	Ui::LoginClass ui;
	TabsWindow mainWindow;
	QString targetAddress;
};
