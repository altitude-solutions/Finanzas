#pragma once

#include <QWidget>
#include "ui_PagosEfectivos.h"
#include <QHash>

class PagosEfectivos : public QWidget {
	Q_OBJECT

public:
	PagosEfectivos(QWidget *parent = Q_NULLPTR);
	~PagosEfectivos();

	void setAuthToken (QString token);
	void setUserName (QString userName);

private slots:

	void saveButtonClicked ();
	void findButtonClicked();

private:
	void loadPlanesData ();

	Ui::PagosEfectivos ui;
	QString token;
	QString userName;

	QHash<QString, QJsonDocument> planesDePagoData;
};
