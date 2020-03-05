#include "Operacion.h"
#include "Operacion.h"

Operacion::Operacion(QObject *parent): QObject(parent) {
	id = 0;
	tipoOperacion = OperacionesFinancieras::TiposDeOperacion::NONE;
	numeroContrato = "";
	fechaFirma = QDate::currentDate();
	concepto = "";
	detalle = "";
	currency = OperacionesFinancieras::Moneda::NONE;
	monto = 0;
	iva = 0;
	rateType = OperacionesFinancieras::TipoTasa::NONE;
	tasaFija = 0;
	tasaVariable = 0;
	plazo = 0;
	frecuencia = OperacionesFinancieras::FrecuenciaDePagos::NONE;
	fechaVencimiento = QDate::currentDate ();
	// other tables
	empresaGrupo = 0;
	entidadFinanciera = 0;
}

Operacion::~Operacion() {

}

void Operacion::deleteRes (int id) {

}

void Operacion::setContractNumber (QString contractNumber) {

}

void Operacion::setSignDate (QDate date) {

}

void Operacion::setConcept (QString concept) {

}

void Operacion::setDetail (QString detail) {

}

void Operacion::setCurrency (OperacionesFinancieras::Moneda currency) {
	this->currency = currency;
}

void Operacion::setAmmount (double ammount) {

}

void Operacion::setIVA (double iva) {

}

void Operacion::setRateType (OperacionesFinancieras::TipoTasa rateType) {
	this->rateType = rateType;
}

void Operacion::setStaticRate (double staticRate) {

}

void Operacion::setDynamicRate (double dynamicRate) {

}

void Operacion::setLifetime (int lifetime) {

}

void Operacion::setFrequency (OperacionesFinancieras::FrecuenciaDePagos freq) {

}

void Operacion::setExpirationDate (QDate date) {

}

void Operacion::setEnterprise (int enterprise_ID) {

}

void Operacion::setEntity (int entity_ID) {

}

