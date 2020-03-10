#pragma once

#include <QObject>

#include "Operacion.h"


class OperacionLineaDeCredito : public Operacion {
	Q_OBJECT

public:
	OperacionLineaDeCredito(QObject *parent);
	~OperacionLineaDeCredito();

	bool validate ();
	void save (QString targetURL, QString token);
	void update (QString targetURL, QString token);
};
