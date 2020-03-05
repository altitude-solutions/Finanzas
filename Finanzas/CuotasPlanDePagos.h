#pragma once

#include <QObject>

#include <QDate>

class CuotasPlanDePagos : public QObject {
	Q_OBJECT

public:
	CuotasPlanDePagos(QObject *parent);
	~CuotasPlanDePagos();

private:
	int id;
	int numeroCuota;
	QDate fechaPago;
	double montoTotal;
	double pagoCapital;
	double pagoInteres;
	double pagoIva;
};
