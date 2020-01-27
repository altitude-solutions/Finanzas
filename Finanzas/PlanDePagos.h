#pragma once

#include <QWidget>
#include "ui_PlanDePagos.h"
// Other imports
#include <QHash>

// Json imports
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>

enum CasosPlanDePagos_enum {
	CasoCredito = 0,
	CasoLineaDeCredito = 1,
	CasoLeasing = 2,
	CasoLeaseBack = 3,
	CasoSeguro = 4
};

class PlanDePagos : public QWidget {
	Q_OBJECT

public:
	PlanDePagos(QWidget *parent = Q_NULLPTR);
	~PlanDePagos();

	// Auth data setter
	void setAuthData (QString address, QString token, QString userName);

private slots:
	// Operation type changed slot
	void onOperationTypeChanged (int type);
	// Datepickers slots
	void fechaFirmaChanged (QDate date);
	void fechaDesem1Changed (QDate date);
	// Monto operación changed
	void montoChanged (QString monto);
	// Tipo tasa changed slot
	void tipoTasaChanged (int type);
	// Interés fijo changed
	void interesFijoChanged (QString interes);
	// Interés variable changed
	void interesVariableChanged (QString interes);
	// iva and pago iva autofill
	void ivaAutoFill (QString monto);
	void pagoIvaAutofill (QString monto);
	// cuota inicial changed
	void cuotaInicialChanged (QString cuotaInicial);
	// Pago interes changed
	void pagoInteresChanged (QString interes);
	// plazo changed slot
	void onPlazoChanged (int value);
	// Montos desembolso changed
	void onDesem1Changed (QString desem1);
	void onDesem2Changed (QString desem2);
	void onDesem3Changed (QString desem3);
	void onDesem4Changed (QString desem4);
	void onDesem5Changed (QString desem5);
	// Frecuencia de pagos changed
	void onFrecuenciaDePagosChanged (QString frecuencia);
	// codigo linea de credito changed, needs to be verified
	void lookForLineaCredito (QString codigo);
	// numero cuota changed
	void numeroCuotaChanged (int value);
	// Pago cuota changed
	void pagoCuotaChanged (QString pago);
	// Pagp capital changed
	void pagoCapitalChanged (QString capital);
	// Pago iva changed
	void pagoIvaChanged (QString iva);
	// Save slots
	void onSavePlan ();
	void onSaveCouta ();
private:
	// data loaders
	void loadEntidadesFinancieras ();
	void loadTiposDeEntidad ();
	void loadLineasDeCredito ( int entidad_id );
	void loadEmpresasGrupo ();

	// reset validators
	void resetPlanValidators ();
	void resetCuotaValidators ();

	// table operations
	void setTableHeaders ();
	void refreshTable (QJsonDocument data);

	// Clear fields
	void clearFields ();

	// App cases
	void casoCredito ();
	void casoLineaDeCredito ();
	void casoLeasing ();
	void casoLeaseBack ();
	void casoSeguro ();
	
	// App cases setup
	void casoCreditoSetup ();
	void casoLineaDeCreditoSetup ();
	void casoLeasingSetup ();
	void casoLeaseBackSetup ();

	// App cases validators
	bool checkCasoCredito ();
	bool checkCasoLineaDeCredito ();
	bool checkCasoLeasing ();
	bool checkCasoLeaseBack ();
	bool checkCasoSeguro ();

	// moneda changed
	void onMonedaChanged (QString moneda);

	// Cuota validator
	bool checkCuota ();
	
	// Swap fields
	void lockGeneraInfoEnableCuotaInfo ();


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

	// Current plan data
	int planID;
	double totalPaid;
	QList<int> paidCuotas;
	QDate lastDate;


	// Validation flags
	bool* planDataIsCorrect;
	bool* cuotaDataIsCorrect;
};
