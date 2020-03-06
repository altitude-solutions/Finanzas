#pragma once

#include <QObject>

#include "Operacion.h"

class OperacionCredito : public Operacion {
	Q_OBJECT

public:
	OperacionCredito(QObject *parent);
	~OperacionCredito();

	bool validate ();
	void save (QString targetURL, QString token);
	void load (int id, QString targetURL, QString token);
	void update (QString targetURL, QString token);

private:

};
