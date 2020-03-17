#pragma once

#include <QObject>

#include <QDate>

class CuotasEfectivas : public QObject {
	Q_OBJECT

public:
	CuotasEfectivas(QObject *parent);
	~CuotasEfectivas();

private:
	int id;
	int numeroCuota;
	QDate fechaPago;
	double montoTotal;
	double pagoCapital;
	double pagoInteres;
	double pagoIva;
};
