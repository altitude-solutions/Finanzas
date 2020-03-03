#pragma once

#include <QDialog>
#include "ui_AddCuotaEfectiva.h"
#include "PlanDePagos.h"


class AddCuotaEfectiva: public QDialog {
	Q_OBJECT

public:
	AddCuotaEfectiva(QWidget *parent = Q_NULLPTR);
	~AddCuotaEfectiva();

	void setWindowData (QString targetUrl, QString token, int cuota, QDate minDate, CasosPlanDePagos_enum caso, int parentID, int editingID = 0, bool editing = false);

public slots:
	void onSaveClicked ();
	void onMontoCuotaChanged (QString monto);
	void onPagoCapitalChanged (QString capital);
	void onPagoInteresChanged (QString interes);

private:
	Ui::AddCuotaEfectiva ui;

	// Window data
	QString targetUrl;
	QString token;
	bool editing;
	int editingID;
	int parentID;
	CasosPlanDePagos_enum caso;
};
