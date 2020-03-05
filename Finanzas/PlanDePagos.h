#pragma once

#include <QWidget>
#include "ui_PlanDePagos.h"
// Other imports
#include <QHash>
#include <QDate>

// Json imports
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>

// Clases imports
#include "OperacionesFinancieras.h"
#include "Operacion.h"
#include "OperacionCredito.h"


class PlanDePagos : public QWidget {
	Q_OBJECT

public:
	PlanDePagos(QWidget *parent = Q_NULLPTR);
	~PlanDePagos();

	// Auth data setter
	void setAuthData (QString address, QString token, QString userName);

public slots:
	// Current tab selected
	void onTabSelected ();

private slots:
	// operation type selected
	void operationTypeSelected (QString operation);

private:
	// data loaders
	void loadEmpresasGrupo ();
	void loadEntidadesFinancieras ();
	void loadTiposDeEntidad ();
	void loadLineasDeCredito (int entidad_ID);
	// table operations
	void setTableHeaders ();
	// clear fields and block them
	void clearFields ();
	void blockFields ();
	void resetFields ();
	// unlock fields
	void unlockField ();
	// new button clicked event
	void onNewClicked ();
	// setup callback(buttons) connections
	void setupConnections ();
	// setup ui connections (autofills, number checkers, some validators and desembolsos enable/disable behavior)
	void setupUiConnections ();

	// window
	Ui::PlanDePagos ui;

	// Auth data
	QString token;
	QString userName;
	QString targetAddress;

	// App data
	QHash <QString, QHash<QString, QString>> listaEntidades;
	QHash <QString, QString> listaTiposEntidades;
	QHash <QString, QHash <QString, QString>> lineasDeCredito;
	QHash <QString, QString> listaEmpresas;

	Operacion *currentOperation;

	// Current plan data
	double saldoCapital;
	double saldoCapitalReal;
	double creditoFiscal;
};