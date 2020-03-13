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

// Operation namespace, utility functions
#include "OperacionesFinancieras.h"

// Clases imports
#include "Operacion.h"		// abstract
#include "OperacionCredito.h"
#include "OperacionLineaDeCredito.h"
#include "OperacionLeasing.h"
#include "OperacionLeaseBack.h"


#include "AddCuotaDelPlan.h"


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
	// currency selected
	void currencySelected (QString currency);
	// interest rate type selected
	void rateTypeSelected (QString rateType);
	// enable and diable desembolsos
	void desem_1_changed (QString desem1);
	void desem_2_changed (QString desem2);
	void desem_3_changed (QString desem3);
	void desem_4_changed (QString desem4);
	// catch model validation errors
	void catchErrors (OperationValidationErros error, QString errorMessage);
	// load selected plan from left list
	void loadClickedPlan (QListWidgetItem *itm);
	// plazo changed, update expiration date
	void termChanged (int value);
	// expiration date  changed, update term
	void expirationDateChanged (QDate date);
	// sign date changed, update expiration date
	void signDateChanged (QDate date);


private:
	// data loaders
	void loadEmpresasGrupo ();
	void loadEntidadesFinancieras ();
	void loadTiposDeEntidad ();
	void loadLineasDeCredito (int entidad_ID, int empresa_ID);
	void loadPlanesDePago (QString query = "");
	void reloadSelectedPlan ();
	// table operations
	void setTableHeaders ();
	// clear fields and block them
	void clearFields ();
	void blockFields ();
	void resetFields ();
	// unlock fields
	void unlockField ();
	// after plan saved enable dues
	void enableDueButtons ();
	// unlock plan editing
	void makePlanEditable ();
	//======================================================
	//======================= button =======================
	// new button clicked event
	void onNewClicked ();
	// clear clicked event: resetFiels
	void onClearClicked ();
	// save button clicked
	void onSaveClicked ();
	// add due clicked
	void onAddDue ();
	// delete plan clicked
	void onDeletePlan ();
	// update plan clicked
	void onUpdatePlan ();
	// update due clicked
	void onUpdateDue ();
	// delete due clicked
	void onDeleteDue ();
	//======================================================

	// setup callback(buttons) connections
	void setupConnections ();
	// setup ui connections (autofills, number checkers, some validators and desembolsos enable/disable behavior)
	void setupUiConnections ();

	//======================================================
	//================== update operation ==================
	void updateModel ();
	//======================================================

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
	QHash <QString, QString> listaOperaciones;

	Operacion *currentOperation;

	// Current plan data
	double saldoCapital;
	double saldoCapitalReal;
	double creditoFiscal;

	bool loadedFromLeftlist;
	bool editingPlan;
};