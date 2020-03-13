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

	void setWindowData (QString targetUrl, QString token, int cuota, QDate minDate, OperacionesFinancieras::TiposDeOperacion caso, OperacionesFinancieras::FrecuenciaDePagos freq, int parentID, int editingID = 0, bool editing = false);

public slots:
	void onSaveClicked ();
	void onMontoCuotaChanged (double monto);
	void onPagoCapitalChanged (double capital);
	void onPagoInteresChanged (double interes);

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
