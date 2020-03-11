#pragma once

#include "Operacion.h"

class OperacionLeaseBack : public Operacion {
	Q_OBJECT

public:
	OperacionLeaseBack(QObject *parent);
	~OperacionLeaseBack();

	bool validate ();
	void save (QString targetURL, QString token);
	void update (QString targetURL, QString token);
};
