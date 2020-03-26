#pragma once

#include <QObject>

#include "Operacion.h"


class OperacionImpuestosNacionales : public Operacion {
	Q_OBJECT

public:
	OperacionImpuestosNacionales(QObject *parent);
	~OperacionImpuestosNacionales();

	bool validate ();
	void save (QString targetURL, QString token);
	void update (QString targetURL, QString token);

	void createFirstDue (QString targetURL, QString token);
};
