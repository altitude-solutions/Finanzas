#pragma once

#include "Operacion.h"


class OperacionLeasing : public Operacion {
	Q_OBJECT

public:
	OperacionLeasing(QObject *parent);
	~OperacionLeasing();

	bool validate ();
	void save (QString targetURL, QString token);
	void update (QString targetURL, QString token);

	void createFirstDue (QString targetURL, QString token);
};
