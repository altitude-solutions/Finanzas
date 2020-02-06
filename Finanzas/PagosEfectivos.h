#pragma once

#include <QWidget>
#include "ui_PagosEfectivos.h"
#include <QHash>

class PagosEfectivos : public QWidget {
	Q_OBJECT

public:
	PagosEfectivos (QWidget* parent = Q_NULLPTR);
	~PagosEfectivos ();

	void setAuthData (QString address, QString token, QString userName);

public slots:
	void onTabSelected ();

private slots:
	void saveButtonClicked ();
	void findButtonClicked ();

private:
	void setTableHeaders ();
	void loadEmpresasGrupo ();
	void loadEntidadesFinancieras ();
	void loadPlanesData ();


	Ui::PagosEfectivos ui;

	// Auth data
	QString token;
	QString userName;
	QString targetAddress;

	// App data
	QHash <QString, QString> listaEmpresas;
	QHash <QString, QHash<QString, QString>> listaEntidades;


	QHash<QString, QJsonDocument> planesDePagoData;
};
