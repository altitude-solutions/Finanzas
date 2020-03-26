#pragma once

#include <QWidget>
#include <QDialog>
#include "ui_AddCuotaDelPlan.h"

#include "Operacion.h"
#include "CuotasPlanDePagos.h"

#include "OperacionesFinancieras.h"


class AddCuotaDelPlan : public QDialog {
	Q_OBJECT

public:
	AddCuotaDelPlan(QWidget *parent = Q_NULLPTR);
	~AddCuotaDelPlan();

	void setValidationParams (QString targetURL, QString token, Operacion *op, CuotasPlanDePagos *lastDue = nullptr, bool editing = false, CuotasPlanDePagos* currentDue = nullptr);

private slots:
	void onSaveClicked ();
	void capitalAutofill (double ammount);
	void catchError (DueValidationError errorCode, QString message);
	void capitalChanged (double capital);
	void interestChanged (double interest);

	void ivaChanged (double iva);

private:
	Ui::AddCuotaDelPlan ui;

	bool editing;
	int editing_ID;

	QString targetURL;
	QString token;

	CuotasPlanDePagos* currentDue;
	int parentOp_ID;

	OperacionesFinancieras::TiposDeOperacion operationType;
};
