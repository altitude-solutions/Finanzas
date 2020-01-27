#pragma once

#include <QWidget>
#include "ui_AppHeader.h"
#include <QTimer>

class AppHeader : public QWidget {
	Q_OBJECT
signals:
	void logoutButton ();

public:
	AppHeader(QWidget *parent = Q_NULLPTR);
	~AppHeader();

	void setAppName (QString appName);
	void setUserName (QString userName);

private slots:
	void updateClock ();

private:
	Ui::AppHeader ui;
	QTimer timer;
};
