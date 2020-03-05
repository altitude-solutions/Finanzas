#pragma once

#include <QObject>

#include "Operacion.h"

class OperacionCredito : public Operacion {
	Q_OBJECT

public:
	OperacionCredito(QObject *parent);
	~OperacionCredito();

	bool validate ();
	void save ();
	void load (int id);
	void update ();


private:
	// desembolsos
	QDate fechaDesem_1;
	double montoDesem_1;
	QDate fechaDesem_2;
	double montoDesem_2;
	QDate fechaDesem_3;
	double montoDesem_3;
	QDate fechaDesem_4;
	double montoDesem_4;
	QDate fechaDesem_5;
	double montoDesem_5;
};
