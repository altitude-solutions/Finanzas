#include "OperacionCredito.h"

OperacionCredito::OperacionCredito(QObject *parent): Operacion(parent) {
	// desembolsos
	fechaDesem_1 = QDate::currentDate ();
	montoDesem_1 = 0;
	fechaDesem_2 = QDate::currentDate ();
	montoDesem_2 = 0;
	fechaDesem_3 = QDate::currentDate ();
	montoDesem_3 = 0;
	fechaDesem_4 = QDate::currentDate ();
	montoDesem_4 = 0;
	fechaDesem_5 = QDate::currentDate ();
	montoDesem_5 = 0;
}

OperacionCredito::~OperacionCredito() {

}

bool OperacionCredito::validate () {
	return false;
}

void OperacionCredito::save () {

}

void OperacionCredito::load (QJsonDocument object) {

}

void OperacionCredito::update (QJsonDocument object) {

}
