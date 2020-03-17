#pragma once

#include <QWidget>
#include "ui_PagosEfectivos.h"
#include <QHash>
#include <QStringListModel>

#include "OperacionesFinancieras.h"
#include "AddCuotaEfectiva.h"

class PagosEfectivos : public QWidget {
	Q_OBJECT

public:
	PagosEfectivos (QWidget* parent = Q_NULLPTR);
	~PagosEfectivos ();

	void setAuthData (QString address, QString token, QString userName);

public slots:
	void onTabSelected ();

private slots:
	void searchPlan (QString  filter);
	void loadSelectedPlan (int id);

private:
	void loadEmpresasGrupo ();
	void loadEntidadesFinancieras ();
	void loadPlanesData (QString query = "");
	void setPlanTableHeaders ();

	void setupTableClipboard ();

	Ui::PagosEfectivos ui;

	// Auth data
	QString token;
	QString userName;
	QString targetAddress;

	// App data
	QHash <QString, QString> listaEmpresas;
	QHash <QString, QHash<QString, QString>> listaEntidades;

	QStringListModel *planesDePagoModel;
	QHash<QString, int> planesDePagoData;

	int currentPlan;
	bool planLoaded;
};
