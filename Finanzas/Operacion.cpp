#include "Operacion.h"
#include "Operacion.h"

Operacion::Operacion(QObject *parent): QObject(parent) {
	id = 0;
	operationType = OperacionesFinancieras::TiposDeOperacion::NONE;
	contractNumber = "";
	signDate = QDate::currentDate();
	concept = "";
	detail = "";
	currency = OperacionesFinancieras::Moneda::NONE;
	ammount = 0;
	iva = 0;
	rateType = OperacionesFinancieras::TipoTasa::NONE;
	staticRate = 0;
	dynamicRate = 0;
	term = 0;
	frequency = OperacionesFinancieras::FrecuenciaDePagos::NONE;
	expirationDate = QDate::currentDate ();
	// other tables
	enterprise = 0;
	entity = 0;

	// Desembolsos
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

	initialDue = 0;
}

Operacion::~Operacion() {

}

// Delete CRUD operation
void Operacion::deleteRes (QString targetURL, QString token) {

}


//===========================================================================================================================
//========================================================= Setters =========================================================
void Operacion::setContractNumber (QString contractNumber) {
	this->contractNumber = contractNumber;
}

void Operacion::setSignDate (QDate date) {
	this->signDate = date;
}

void Operacion::setConcept (QString concept) {
	this->concept = concept;
}

void Operacion::setDetail (QString detail) {
	this->detail = detail;
}

void Operacion::setCurrency (OperacionesFinancieras::Moneda currency) {
	this->currency = currency;
}

void Operacion::setAmmount (double ammount) {
	this->ammount = ammount;
}

void Operacion::setIVA (double iva) {
	this->iva = iva;
}

void Operacion::setRateType (OperacionesFinancieras::TipoTasa rateType) {
	this->rateType = rateType;
}

void Operacion::setStaticRate (double staticRate) {
	this->staticRate = staticRate;
}

void Operacion::setDynamicRate (double dynamicRate) {
	this->dynamicRate = dynamicRate;
}

void Operacion::setTerm (int term) {
	this->term = term;
}

void Operacion::setFrequency (OperacionesFinancieras::FrecuenciaDePagos freq) {
	this->frequency = freq;
}

void Operacion::setExpirationDate (QDate date) {
	this->expirationDate = date;
}

void Operacion::setEnterprise (int enterprise_ID) {
	this->enterprise = enterprise_ID;
}

void Operacion::setEntity (int entity_ID) {
	this->entity = entity_ID;
}

OperacionesFinancieras::TiposDeOperacion Operacion::getOperationType () {
	return this->operationType;
}

void Operacion::setFechaDesem_1 (QDate date) {
	this->fechaDesem_1 = date;
}

void Operacion::setMontoDesem_1 (double ammount) {
	this->montoDesem_1 = ammount;
}

void Operacion::setFechaDesem_2 (QDate date) {
	this->fechaDesem_2 = date;
}

void Operacion::setMontoDesem_2 (double ammount) {
	this->montoDesem_2 = ammount;
}

void Operacion::setFechaDesem_3 (QDate date) {
	this->fechaDesem_3 = date;
}

void Operacion::setMontoDesem_3 (double ammount) {
	this->montoDesem_3 = ammount;
}

void Operacion::setFechaDesem_4 (QDate date) {
	this->fechaDesem_4 = date;
}

void Operacion::setMontoDesem_4 (double ammount) {
	this->montoDesem_4 = ammount;
}

void Operacion::setFechaDesem_5 (QDate date) {
	this->fechaDesem_5 = date;
}

void Operacion::setMontoDesem_5 (double ammount) {
	this->montoDesem_5 = ammount;
}

void Operacion::setInitialDue (double ammount) {
	this->initialDue = ammount;
}
//===========================================================================================================================
//======================================================== getters ==========================================================
int Operacion::getID () {
	return this->id;
}

QString Operacion::getContractNumber () {
	return this->contractNumber;
}

QDate Operacion::getSignDate () {
	return this->signDate;
}

QString Operacion::getConcept () {
	return this->concept;
}

QString Operacion::getDetail () {
	return this->detail;
}

OperacionesFinancieras::Moneda Operacion::getCurrency () {
	return this->currency;
}

double Operacion::getAmmount () {
	return this->ammount;
}

double Operacion::getIVA () {
	return this->iva;
}

OperacionesFinancieras::TipoTasa Operacion::getRateType () {
	return this->rateType;
}

double Operacion::getStaticRate () {
	return this->staticRate;
}

double Operacion::getDynamicRate () {
	return this->dynamicRate;
}

OperacionesFinancieras::FrecuenciaDePagos Operacion::getFrequency () {
	return this->frequency;
}

QDate Operacion::getExpirationDate () {
	return this->expirationDate;
}

int Operacion::getEnterprise () {
	return this->enterprise;
}

int Operacion::getEntity () {
	return this->entity;
}

QDate Operacion::getFechaDesem_1 () {
	return this->fechaDesem_1;
}

double Operacion::getMontoDesem_1 () {
	return this->montoDesem_1;
}

QDate Operacion::getFechaDesem_2 () {
	return this->fechaDesem_2;
}

double Operacion::getMontoDesem_2 () {
	return this->montoDesem_2;
}

QDate Operacion::getFechaDesem_3 () {
	return this->fechaDesem_3;
}

double Operacion::getMontoDesem_3 () {
	return this->montoDesem_3;
}

QDate Operacion::getFechaDesem_4 () {
	return this->fechaDesem_4;
}

double Operacion::getMontoDesem_4 () {
	return this->montoDesem_4;
}

QDate Operacion::getFechaDesem_5 () {
	return this->fechaDesem_5;
}

double Operacion::getMontoDesem_5 () {
	return this->montoDesem_5;
}

double Operacion::getInitialDue () {
	return this->initialDue;
}
//===========================================================================================================================