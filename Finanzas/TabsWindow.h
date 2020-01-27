#pragma once

#include <QMainWindow>
#include "ui_TabsWindow.h"
#include "AppHeader.h"
#include "LineaDeCredito.h"
#include "PlanDePagos.h"
#include "PagosEfectivos.h"

class TabsWindow : public QMainWindow {
	Q_OBJECT

public:
	TabsWindow(QWidget *parent = Q_NULLPTR);
	~TabsWindow();

public slots:
	void onLoginSuccess (QString address, QString userName, QString realName, QString token);

signals:
	void logout ();

private:
	Ui::TabsWindow ui;
	QWidget* firstTab;
	AppHeader* firtsTabHeader;
	QWidget* secondTab;
	AppHeader* secondTabHeader;
	LineaDeCredito* lineaDeCredito;
	PlanDePagos* planDePagos;
	QWidget* thirdTab;
	AppHeader* thirdTabHeader;
	PagosEfectivos* pagosEfectivos;
};
