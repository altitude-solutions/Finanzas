#pragma once

#include <QDialog>
#include "ui_AddCuotaEfectiva.h"
#include "PlanDePagos.h"
#include "OperacionesFinancieras.h"


class AddCuotaEfectiva: public QDialog {
	Q_OBJECT

public:
	AddCuotaEfectiva(QWidget *parent = Q_NULLPTR);
	~AddCuotaEfectiva();

	void setWindowData (QString targetUrl, QString token, int cuota, QDate minDate, OperacionesFinancieras::TiposDeOperacion caso, OperacionesFinancieras::FrecuenciaDePagos freq, int parentID, QDate currentDate, double total = 0, double cap = 0, double inte = 0, double  iva = 0, int editingID = 0, bool editing = false);

public slots:
	void onSaveClicked ();
	void capitalAutoFill (double monto);
	void onPagoCapitalChanged (double capital);
	void onPagoInteresChanged (double interes);

	void onIvaChanged (double iva);

private:
	Ui::AddCuotaEfectiva ui;

	// Window data
	QString targetUrl;
	QString token;
	bool editing;
	int editingID;
	int parentID;
	OperacionesFinancieras::TiposDeOperacion caso;
};
